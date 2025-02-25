#ifndef ARGS_H
#define ARGS_H

#include <stdbool.h>

typedef struct {
    int count;       // Total number of arguments
    char **values;   // Array of argument values
} Args;

typedef struct {
    const char *command;       // Command name (e.g., "run")
    const char **flags;        // Flags associated with the command (e.g., "--debug")
    const char **values;       // Key-value pairs (e.g., "--output file")
    int flag_count;            // Number of flags
    int value_count;           // Number of key-value pairs
} Command;

// Initializes the args struct from `main()`
Args args_parse(int argc, char *argv[]);

// Checks if a flag exists (e.g., `-f` or `--flag`)
bool args_has_flag(Args *args, const char *flag);

// Gets the value after an argument (e.g., `-o filename.txt`)
const char* args_get_value(Args *args, const char *key);

// Checks if an argument exists (e.g., `--input`, `-f`)
bool args_has_arg(Args *args, const char *arg);

// Get a list of all flags (e.g., `-f`, `--verbose`)
void args_get_flags(Args *args, const char ***flags, int *count);

// Get a list of all values associated with keys (e.g., `-o`, `--output`)
void args_get_values(Args *args, const char ***keys, const char ***values, int *count);

// Parse the command (e.g., `lua_runtime run --debug`)
Command args_parse_command(Args *args);

// Print arguments (for debugging)
void args_print(Args *args);

// Clean up any allocated memory after parsing
void args_free(Args *args);
char** args_get_all(Args *args, int *count);
void args_exclude(Args *args, const char *exclude_arg);

#endif // ARGS_H
