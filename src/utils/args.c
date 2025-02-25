#include "args.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Initializes the args struct from `main()`
Args args_parse(int argc, char *argv[]) {
    Args args;
    args.count = argc;
    args.values = argv;
    return args;
}

// Checks if a flag exists (e.g., `-f` or `--flag`)
bool args_has_flag(Args *args, const char *flag) {
    for (int i = 1; i < args->count; i++) {
        if (strcmp(args->values[i], flag) == 0) {
            return true;
        }
    }
    return false;
}

// Gets the value after an argument (e.g., `-o filename.txt`)
const char* args_get_value(Args *args, const char *key) {
    for (int i = 1; i < args->count - 1; i++) {
        if (strcmp(args->values[i], key) == 0) {
            return args->values[i + 1]; // return value after the key
        }
    }
    return NULL; // Return NULL if the key is not found or no value follows it
}

// Checks if an argument exists (e.g., `--input`, `-f`)
bool args_has_arg(Args *args, const char *arg) {
    for (int i = 1; i < args->count; i++) {
        if (strcmp(args->values[i], arg) == 0) {
            return true;
        }
    }
    return false;
}

// Get a list of all flags (e.g., `-f`, `--verbose`)
void args_get_flags(Args *args, const char ***flags, int *count) {
    *count = 0;
    *flags = malloc(sizeof(char*) * args->count);
    
    for (int i = 1; i < args->count; i++) {
        if (args->values[i][0] == '-') {
            (*flags)[*count] = args->values[i];
            (*count)++;
        }
    }
}

// Get a list of all values associated with keys (e.g., `-o`, `--output`)
void args_get_values(Args *args, const char ***keys, const char ***values, int *count) {
    *count = 0;
    *keys = malloc(sizeof(char*) * args->count);
    *values = malloc(sizeof(char*) * args->count);
    
    for (int i = 1; i < args->count - 1; i++) {
        if (args->values[i][0] == '-') {
            (*keys)[*count] = args->values[i];
            (*values)[*count] = args->values[i + 1];
            (*count)++;
            i++; // Skip the next element (the value)
        }
    }
}

// Parse the command (e.g., `lua_runtime run --debug`)
Command args_parse_command(Args *args) {
    Command command = {0};
    
    // The first argument is the command name (e.g., "run")
    if (args->count > 1) {
        command.command = args->values[1]; // First argument is the command
    }

    // Loop through the remaining arguments to separate flags and values
    int flag_count = 0;
    int value_count = 0;
    command.flags = malloc(sizeof(char*) * args->count);
    command.values = malloc(sizeof(char*) * args->count);

    for (int i = 2; i < args->count; i++) {
        if (args->values[i][0] == '-') {
            // It's a flag
            command.flags[flag_count++] = args->values[i];
        } else {
            // It's a value (associated with the previous flag)
            command.values[value_count++] = args->values[i];
        }
    }

    command.flag_count = flag_count;
    command.value_count = value_count;

    return command;
}

// Print arguments (for debugging)
void args_print(Args *args) {
    printf("Arguments (%d):\n", args->count - 1);
    for (int i = 1; i < args->count; i++) {
        printf("  [%d] %s\n", i, args->values[i]);
    }
}

// Clean up any allocated memory after parsing (for lists of flags/values)
void args_free(Args *args) {
    // Free dynamically allocated memory for flags and values in the command
}

// Get all arguments (excluding the program name)
char** args_get_all(Args *args, int *count) {
    *count = args->count - 1; // Exclude program name
    char **all_args = malloc(sizeof(char*) * (*count));

    for (int i = 1; i < args->count; i++) {
        all_args[i - 1] = args->values[i]; // Exclude argv[0]
    }

    return all_args;
}

// Exclude specific arguments from the argument list at runtime
void args_exclude(Args *args, const char *exclude_arg) {
    int new_count = 0;

    for (int i = 1; i < args->count; i++) {
        if (strcmp(args->values[i], exclude_arg) != 0) {
            args->values[new_count++] = args->values[i];
        }
    }

    args->count = new_count + 1; // Add 1 for the program name
}