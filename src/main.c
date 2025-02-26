#include "fs.h"
#include "args.h"
#include "reflex_api.h"
#include "apis/process_api.h"
#include "logger.h"
#include <stdio.h>
#include <string.h>

void print_help() {
    logf("Usage: reflex <command> [options]\n");
    logf("\nAvailable commands:\n");
    logf("  run <file.lua> [--debug] [--output <file>] [-- <lua-args>]\n");
    logf("      Executes the specified Lua file.\n");
    logf("      Options:\n");
    logf("        --debug      Enable debug mode (prints additional debugging info).\n");
    logf("        --            Separate Reflex arguments from Lua script arguments.\n");

    logf("\nExample usage:\n");
    logf("  reflex run my_script.lua --debug -- --mainluaarg\n");
    logf("      Executes 'my_script.lua' with the Lua argument '--mainluaarg'.\n");

    logf("\nFor more information, visit the documentation at https://github.com/reflexengine/reflex/wiki\n");
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
        const char *fileName = cmd.values[0];
        if (!fileName) {
            logf("Error: No file specified for 'run' command.\n");
            return 1;
        }

        const char *contents = fs_read(fileName);
        if (!contents) {
            logf("Error: Failed to read file '%s'.\n", fileName);
            return 1;
        }

        Args reflex_args = {0}, lua_args = {0};
        split_args_at_double_dash(args, &reflex_args, &lua_args);

        if (args_has_flag(&reflex_args, "--debug")) {
            logf("Debug mode enabled for Reflex execution.\n");
        }

        define_reflex_builtin(api);

        if (lua_args.count > 0) {
            define_program_arguments(api, lua_args);
        }

        int status = reflex_execute(api, contents);
        if (status) {
            const char *result = lua_tostring(api->L, -1);
            flogf(stderr, "Lua Error: %s\n", result);
            return 1;
        }

        return 0;
    }

    logf("Error: Unknown command '%s'. Try 'help' for usage.\n", cmd.command);
    return 1;
}

int main(int argc, char *argv[]) {
    Args args = args_parse(argc, argv);
    LuaAPI* api = reflex_new();

    // Handle the commands
    int result = handle_command(api, &args);

    // Clean up and exit
    args_free(&args);
    return result;
}
