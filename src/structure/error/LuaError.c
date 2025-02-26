#include "error/LuaError.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

// ANSI color codes for terminal output
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define ANSI_BOLD          "\x1b[1m"
#define ANSI_UNDERLINE     "\x1b[4m"

// Extract source file and line number from a Lua error message
static void extract_source_and_line(const char *error_message, char *source, int *line) {
    // Default values in case we can't parse
    strcpy(source, "unknown");
    *line = 0;
    
    if (!error_message) return;
    
    // Look for patterns like: "file.lua:123: "
    const char *colon = strchr(error_message, ':');
    if (colon) {
        // Extract filename
        size_t source_len = colon - error_message;
        if (source_len < 255) {
            strncpy(source, error_message, source_len);
            source[source_len] = '\0';
        }
        
        // Extract line number
        const char *line_start = colon + 1;
        char *line_end;
        *line = (int)strtol(line_start, &line_end, 10);
        
        // If conversion failed or no number found
        if (line_end == line_start) {
            *line = 0;
        }
    }
}

// Extract the actual error message without file and line number prefix
static void extract_message(const char *error_message, char *message) {
    if (!error_message) {
        strcpy(message, "Unknown error");
        return;
    }
    
    const char *msg_start = error_message;
    
    // Skip past "file:line: " if present
    const char *colon = strchr(error_message, ':');
    if (colon) {
        const char *next_colon = strchr(colon + 1, ':');
        if (next_colon) {
            msg_start = next_colon + 1;
            // Skip leading whitespace
            while (isspace(*msg_start)) msg_start++;
        }
    }
    
    strcpy(message, msg_start);
}

// Generate a traceback for the current Lua stack
static void generate_traceback(lua_State *L, char *traceback) {
    // Initialize traceback buffer
    traceback[0] = '\0';
    
    // Push debug.traceback function onto the stack
    lua_getglobal(L, "debug");
    if (!lua_istable(L, -1)) {
        lua_pop(L, 1);
        strcpy(traceback, "Error: debug library not available");
        return;
    }
    
    lua_getfield(L, -1, "traceback");
    if (!lua_isfunction(L, -1)) {
        lua_pop(L, 2);
        strcpy(traceback, "Error: debug.traceback not available");
        return;
    }
    
    // Call debug.traceback()
    lua_pushstring(L, "");  // Empty message
    lua_pushinteger(L, 2);  // Skip this function and debug.traceback
    lua_call(L, 2, 1);      // Call debug.traceback
    
    // Copy the result to our traceback buffer
    const char *tb = lua_tostring(L, -1);
    if (tb) {
        strncpy(traceback, tb, 1023);
        traceback[1023] = '\0';
    } else {
        strcpy(traceback, "Error: Failed to generate traceback");
    }
    
    // Clean up the stack
    lua_pop(L, 2);  // Pop the traceback string and debug table
}

// Attempt to extract code context from a source file
static void extract_context(const char *source_file, int line_number, char *context) {
    if (!source_file || source_file[0] == '\0' || line_number <= 0) {
        strcpy(context, "(No source context available)");
        return;
    }
    
    FILE *file = fopen(source_file, "r");
    if (!file) {
        strcpy(context, "(Unable to read source file for context)");
        return;
    }
    
    // Initialize context with empty string
    context[0] = '\0';
    
    char line_buffer[256];
    int current_line = 0;
    int context_start = line_number - 2;  // Show 2 lines before the error
    int context_end = line_number + 2;    // Show 2 lines after the error
    
    if (context_start < 1) context_start = 1;
    
    // Read through file to find the context lines
    while (fgets(line_buffer, sizeof(line_buffer), file) && current_line < context_end) {
        current_line++;
        
        if (current_line >= context_start) {
            char line_prefix[32];
            // Format line prefix with line number
            if (current_line == line_number) {
                snprintf(line_prefix, sizeof(line_prefix), "> %3d | ", current_line);
            } else {
                snprintf(line_prefix, sizeof(line_prefix), "  %3d | ", current_line);
            }
            
            // Ensure we don't overflow context buffer
            if (strlen(context) + strlen(line_prefix) + strlen(line_buffer) < 511) {
                strcat(context, line_prefix);
                strcat(context, line_buffer);
                // Add newline if not present in the read line
                if (line_buffer[strlen(line_buffer) - 1] != '\n') {
                    strcat(context, "\n");
                }
            } else {
                // Stop adding context if we're close to buffer limit
                break;
            }
        }
    }
    
    fclose(file);
    
    if (strlen(context) == 0) {
        strcpy(context, "(No context available)");
    }
}

// Our custom error handler function that gets pushed to the Lua stack
int lua_error_handler(lua_State *L) {
    // Get the error message from the top of the stack
    const char *error_message = lua_tostring(L, -1);
    if (!error_message) {
        // If not a string, try to convert with __tostring metamethod
        if (lua_type(L, -1) != LUA_TSTRING && luaL_callmeta(L, -1, "__tostring")) {
            error_message = lua_tostring(L, -1);
        } else {
            error_message = "(error object is not a string)";
        }
    }
    
    // Format the error information but leave the error on stack
    LuaErrorInfo info;
    memset(&info, 0, sizeof(LuaErrorInfo));
    lua_format_error(L, error_message, &info);
    
    // Print the error immediately
    lua_print_error(&info);
    
    // Return the original error to propagate it up the call chain
    return 1;
}

void lua_format_error(lua_State *L, const char *error_message, LuaErrorInfo *info) {
    if (!info) return;
    
    // Initialize the error info structure
    memset(info, 0, sizeof(LuaErrorInfo));
    
    if (!error_message) {
        strcpy(info->message, "Unknown error");
        strcpy(info->source, "unknown");
        info->line = 0;
        strcpy(info->traceback, "No traceback available");
        strcpy(info->context, "No context available");
        return;
    }
    
    // Extract components from the error message
    extract_source_and_line(error_message, info->source, &info->line);
    extract_message(error_message, info->message);
    
    // Generate a traceback of the current stack
    generate_traceback(L, info->traceback);
    
    // Try to get source code context
    extract_context(info->source, info->line, info->context);
}

void lua_print_error(const LuaErrorInfo *info) {
    if (!info) return;
    
    fprintf(stderr, "\n%s%s╭─ ReflexEngine Error ─────────────────────────────────────────╮%s\n", 
            ANSI_BOLD, ANSI_COLOR_RED, ANSI_COLOR_RESET);
    
    // Print the error message
    fprintf(stderr, "│ %s%sError:%s %s\n", 
            ANSI_BOLD, ANSI_COLOR_RED, ANSI_COLOR_RESET, info->message);
    
    // Print the location
    if (info->line > 0) {
        fprintf(stderr, "│ %sAt:%s %s:%d\n", 
                ANSI_COLOR_YELLOW, ANSI_COLOR_RESET, info->source, info->line);
    } else {
        fprintf(stderr, "│ %sAt:%s %s\n", 
                ANSI_COLOR_YELLOW, ANSI_COLOR_RESET, info->source);
    }
    
    // Print code context if available
    if (strlen(info->context) > 0 && 
        strcmp(info->context, "(Unable to read source file for context)") != 0 && 
        strcmp(info->context, "(No context available)") != 0 &&
        strcmp(info->context, "(No source context available)") != 0) {
        
        fprintf(stderr, "│\n│ %sCode context:%s\n│\n", ANSI_COLOR_CYAN, ANSI_COLOR_RESET);
        
        // Split the context into lines and print each one
        char *context_copy = strdup(info->context);
        char *line = strtok(context_copy, "\n");
        
        while (line) {
            // Check if this is the error line
            if (strncmp(line, "> ", 2) == 0) {
                fprintf(stderr, "│ %s%s%s\n", ANSI_COLOR_RED, line, ANSI_COLOR_RESET);
            } else {
                fprintf(stderr, "│ %s\n", line);
            }
            line = strtok(NULL, "\n");
        }
        
        free(context_copy);
    }
    
    // Print traceback
    fprintf(stderr, "│\n│ %sStack traceback:%s\n", ANSI_COLOR_MAGENTA, ANSI_COLOR_RESET);
    
    if (strlen(info->traceback) > 0) {
        char *traceback_copy = strdup(info->traceback);
        char *line = strtok(traceback_copy, "\n");
        int line_count = 0;
        
        // Skip the first line which often contains redundant info
        if (line) line = strtok(NULL, "\n");
        
        while (line && line_count < 5) {  // Limit to 5 frames to avoid clutter
            // Skip lines with debug.traceback references
            if (strstr(line, "debug.traceback") == NULL) {
                fprintf(stderr, "│   %s\n", line);
                line_count++;
            }
            line = strtok(NULL, "\n");
        }
        
        if (line) {
            fprintf(stderr, "│   %s...(more stack frames omitted)%s\n", ANSI_COLOR_BLUE, ANSI_COLOR_RESET);
        }
        
        free(traceback_copy);
    } else {
        fprintf(stderr, "│   %s(No traceback available)%s\n", ANSI_COLOR_BLUE, ANSI_COLOR_RESET);
    }
    
    fprintf(stderr, "%s╰───────────────────────────────────────────────────────────────╯%s\n\n", 
            ANSI_COLOR_RED, ANSI_COLOR_RESET);
}

int lua_setup_error_handler(lua_State *L) {
    // Push our error handler function onto the stack
    lua_pushcfunction(L, lua_error_handler);
    // Return its position in the stack
    return lua_gettop(L);
}