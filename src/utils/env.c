#include "env.h"
#include <stdlib.h>
#include "lua_api.h"
#include "logger.h"

#define MAX_ENV_LINE 256  // Max line size in .env file

// Load .env file into environment
void load_env() {
    println("Loading ENV");
    FILE *file = fopen(".env", "r");
    println(file);
    if (!file) return;  // No .env file, ignore

    char line[MAX_ENV_LINE];

    while (fgets(line, sizeof(line), file)) {
        char *key = strtok(line, "=");
        char *value = strtok(NULL, "\n");

        printf(key, value);

        if (key && value) {
            setenv(key, value, 1); // Store in process environment
        }
    }

    fclose(file);
}