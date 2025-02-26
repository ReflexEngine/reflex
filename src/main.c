#include "fs.h"
#include "args.h"
#include "lua_api.h"
#include "error/LuaError.h"
#include "apis/process_api.h"
#include "reflex_api.h"
#include "logger.h"
#include <stdio.h>
#include <string.h>

void print_help() {
    printlogf("Usage: reflex <command> [options]\n");
    printlogf("\nAvailable commands:\n");
    printlogf("  run <file.lua> [--debug] [--output <file>] [-- <lua-args>]\n");
    printlogf("      Executes the specified Lua file.\n");
    printlogf("      Options:\n");
    printlogf("        --debug      Enable debug mode (prints additional debugging info).\n");
    printlogf("        --            Separate Reflex arguments from Lua script arguments.\n");

    printlogf("\nExample usage:\n");
    printlogf("  reflex run my_script.lua --debug -- --mainluaarg\n");
    printlogf("      Executes 'my_script.lua' with the Lua argument '--mainluaarg'.\n");

    printlogf("\nFor more information, visit the documentation at https://github.com/reflexengine/reflex/wiki\n");
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
            printlogf("Error: No file specified for 'run' command.\n");
            return 1;
        }
        const char *fileName = cmd.values[0];
        if (!fileName) {
            printlogf("Error: No file specified for 'run' command.\n");
            return 1;
        }

        const char *contents = fs_read(fileName);
        if (!contents) {
            printlogf("Error: Failed to read file '%s'.\n", fileName);
            return 1;
        }
        define_reflex_builtin(api);

        Args reflex_args = {0}, lua_args = {0};
        split_args_at_double_dash(args, &reflex_args, &lua_args);

        if (args_has_flag(&reflex_args, "--debug")) {
            printlogf("Debug mode enabled for Reflex execution.\n");
        }


        if (lua_args.count > 0) {
            define_program_arguments(api, lua_args);
        }

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
            
            // Print the error
            lua_print_error(&errorInfo);
            return 1;
        }
        
        // Execute the script with error handler
        int result = lua_pcall(api->L, 0, LUA_MULTRET, error_handler_idx);
        
        // Remove the error handler from the stack
        lua_remove(api->L, error_handler_idx);
        
        return result != 0 ? 1 : 0;
    }

    printlogf("Error: Unknown command '%s'. Try 'help' for usage.\n", cmd.command);
    return 1;
}

int main(int argc, char *argv[]) {
    Args args = args_parse(argc, argv);
    LuaAPI* api = reflex_new();

    // Handle the commands
    int result = handle_command(api, &args);

    // Clean up and exit
    reflex_free(api);
    args_free(&args);
    return result;
}