#include "fs.h"
#include "args.h"
#include "lua_api.h"
#include "error/LuaError.h"
#include "apis/process_api.h"
#include "reflex_api.h"
#include "logger.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

// ANSI color codes for terminal output
#define RESET       "\033[0m"
#define BOLD        "\033[1m"
#define DIM         "\033[2m"
#define ITALIC      "\033[3m"
#define UNDERLINE   "\033[4m"
#define BLUE        "\033[34m"
#define GREEN       "\033[32m"
#define CYAN        "\033[36m"
#define YELLOW      "\033[33m"
#define RED         "\033[31m"
#define MAGENTA     "\033[35m"
#define BRIGHT_BLUE "\033[94m"

// Unicode symbols for better visual hints
#define CHECK_MARK      "✓"
#define CROSS_MARK      "✗"
#define ARROW_RIGHT     "→"
#define INFO_SYMBOL     "ℹ"
#define WARNING_SYMBOL  "⚠"
#define ERROR_SYMBOL    "✘"
#define BULLET_POINT    "•"
#define SEPARATOR       "───────────────────────────────────────────────────"

void print_logo() {
    printlogf("\n");
    printlogf(BOLD CYAN "    ██████╗ ███████╗███████╗██╗     ███████╗██╗  ██╗\n");
    printlogf("    ██╔══██╗██╔════╝██╔════╝██║     ██╔════╝╚██╗██╔╝\n");
    printlogf("    ██████╔╝█████╗  █████╗  ██║     █████╗   ╚███╔╝ \n");
    printlogf("    ██╔══██╗██╔══╝  ██╔══╝  ██║     ██╔══╝   ██╔██╗ \n");
    printlogf("    ██║  ██║███████╗██║     ███████╗███████╗██╔╝ ██╗\n");
    printlogf("    ╚═╝  ╚═╝╚══════╝╚═╝     ╚══════╝╚══════╝╚═╝  ╚═╝\n" RESET);
    printlogf(DIM "    The modern Lua execution environment - v1.0.0\n" RESET);
    printlogf("\n");
}

void print_help() {
    print_logo();
    printlogf(BOLD "USAGE:\n" RESET);
    printlogf("  %sreflex%s %s<command>%s [options]\n\n", BOLD, RESET, CYAN, RESET);
    
    printlogf(BOLD "COMMANDS:\n" RESET);
    printlogf("  %srun%s <file.lua>     %sExecutes the specified Lua file%s\n", GREEN, RESET, DIM, RESET);
    printlogf("  %shelp%s               %sShow this help message%s\n\n", GREEN, RESET, DIM, RESET);
    
    printlogf(BOLD "OPTIONS:\n" RESET);
    printlogf("  %s--debug%s            Enable debug mode (additional info)\n", YELLOW, RESET);
    printlogf("  %s--%s                 Separate Reflex arguments from Lua script arguments\n\n", YELLOW, RESET);
    
    printlogf(BOLD "EXAMPLES:\n" RESET);
    printlogf("  %sreflex run my_script.lua%s\n", BRIGHT_BLUE, RESET);
    printlogf("    %s%s Executes 'my_script.lua' with default settings%s\n\n", DIM, ARROW_RIGHT, RESET);
    
    printlogf("  %sreflex run my_script.lua --debug -- --mainluaarg%s\n", BRIGHT_BLUE, RESET);
    printlogf("    %s%s Executes in debug mode with '--mainluaarg' passed to Lua%s\n\n", DIM, ARROW_RIGHT, RESET);
    
    printlogf(BOLD "DOCUMENTATION:\n" RESET);
    printlogf("  %shttps://github.com/reflexengine/reflex/wiki%s\n\n", UNDERLINE BLUE, RESET);
}

void print_success(const char *message) {
    printlogf("\n%s %s%s%s\n\n", GREEN CHECK_MARK, BOLD, message, RESET);
}

void print_error(const char *message) {
    printlogf("\n%s %s%s%s\n\n", RED ERROR_SYMBOL, BOLD RED, message, RESET);
}

void print_warning(const char *message) {
    printlogf("\n%s %s%s%s\n\n", YELLOW WARNING_SYMBOL, YELLOW, message, RESET);
}

void print_info(const char *message) {
    printlogf("\n%s %s%s\n\n", BLUE INFO_SYMBOL, message, RESET);
}

void print_execution_start(const char *filename) {
    printlogf("\n%s %sExecuting%s %s%s%s\n", ARROW_RIGHT, DIM, RESET, BOLD, filename, RESET);
    printlogf("%s\n", DIM SEPARATOR RESET);
}

void print_execution_end(bool success) {
    printlogf("%s\n", DIM SEPARATOR RESET);
    if (success) {
        print_success("Execution completed successfully");
    } else {
        print_error("Execution failed");
    }
}

void split_args_at_double_dash(Args *args, Args *reflex_args, Args *lua_args) {
    int split_index = -1;
    for (int i = 0; i < args->count; i++) {
        if (strcmp(args->values[i], "--") == 0) {
            split_index = i;
            break;
        }
    }

    if (split_index == -1) {
        *reflex_args = *args;
        lua_args->count = 0;
        lua_args->values = NULL;
        return;
    }

    reflex_args->count = split_index;
    reflex_args->values = args->values;

    lua_args->count = args->count - split_index - 1;
    lua_args->values = &args->values[split_index + 1];
}

int handle_command(LuaAPI *api, Args *args) {
    Command cmd = args_parse_command(args);

    if (cmd.command && strcmp(cmd.command, "help") == 0) {
        print_help();
        return 0;
    }

    if (cmd.command && strcmp(cmd.command, "run") == 0) {
        if (cmd.value_count == 0) {
            print_error("No file specified for 'run' command");
            printlogf("Try %sreflex help%s for usage information\n\n", BOLD, RESET);
            return 1;
        }
        
        const char *fileName = cmd.values[0];
        if (!fileName) {
            print_error("No file specified for 'run' command");
            return 1;
        }

        const char *contents = fs_read(fileName);
        if (!contents) {
            print_error("Failed to read file");
            printlogf("  %s%s File not found: %s%s\n\n", RED, ARROW_RIGHT, fileName, RESET);
            return 1;
        }
        
        define_reflex_builtin(api);

        Args reflex_args = {0}, lua_args = {0};
        split_args_at_double_dash(args, &reflex_args, &lua_args);

        bool debug_mode = args_has_flag(&reflex_args, "--debug");
        if (debug_mode) {
            print_info("Debug mode enabled");
        }

        if (lua_args.count > 0) {
            if (debug_mode) {
                printlogf("%s Passing %d argument(s) to Lua script\n", BLUE INFO_SYMBOL, lua_args.count);
                for (int i = 0; i < lua_args.count; i++) {
                    printlogf("  %s %s\n", BULLET_POINT, lua_args.values[i]);
                }
                printlogf("\n");
            }
            define_program_arguments(api, lua_args);
        }

        print_execution_start(fileName);

        // Set up the error handler before loading the script
        lua_pushcfunction(api->L, lua_error_handler);
        int error_handler_idx = lua_gettop(api->L);

        // Load the script
        int load_status = luaL_loadbuffer(api->L, contents, strlen(contents), fileName);
        if (load_status != 0) {
            // Handle syntax errors during loading
            const char *error_msg = lua_tostring(api->L, -1);
            
            // Create error info structure
            LuaErrorInfo errorInfo;
            memset(&errorInfo, 0, sizeof(LuaErrorInfo));
            
            // We need to manually set this for load errors
            strncpy(errorInfo.source, fileName, sizeof(errorInfo.source) - 1);
            strncpy(errorInfo.message, error_msg, sizeof(errorInfo.message) - 1);
            
            // Print the error with modern formatting
            printlogf("\n");
            printlogf("%s %s%sLua Syntax Error%s\n", RED ERROR_SYMBOL, BOLD, RED, RESET);
            printlogf("%s %sFile:%s %s\n", ARROW_RIGHT, BOLD, RESET, errorInfo.source);
            printlogf("%s %sError:%s %s\n\n", ARROW_RIGHT, BOLD, RESET, errorInfo.message);
            
            print_execution_end(false);
            return 1;
        }
        
        // Execute the script with error handler
        int result = lua_pcall(api->L, 0, LUA_MULTRET, error_handler_idx);
        
        // Remove the error handler from the stack
        lua_remove(api->L, error_handler_idx);
        
        print_execution_end(result == 0);
        return result != 0 ? 1 : 0;
    }

    print_error("Unknown command");
    printlogf("%s Unknown command: '%s'\n", ARROW_RIGHT, cmd.command);
    printlogf("%s Try %sreflex help%s for usage information\n\n", ARROW_RIGHT, BOLD, RESET);
    return 1;
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        print_logo();
        printlogf("%s Run %sreflex help%s for usage information\n\n", INFO_SYMBOL, BOLD, RESET);
        return 0;
    }

    Args args = args_parse(argc, argv);
    LuaAPI* api = reflex_new();

    // Handle the commands
    int result = handle_command(api, &args);

    // Clean up and exit
    reflex_free(api);
    args_free(&args);
    return result;
}