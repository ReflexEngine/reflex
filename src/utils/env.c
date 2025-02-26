#include "env.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_ENV_LINE 256  // Max line size in .env file

// Load .env file into environment
void load_env() {
    FILE *file = fopen(".env", "r");
    if (!file) return;  // No .env file, ignore

    char line[MAX_ENV_LINE];

    while (fgets(line, sizeof(line), file)) {
        // Remove newline if present
        line[strcspn(line, "\n")] = 0;

        char *key = strtok(line, "=");
        char *value = strtok(NULL, "");

        if (key && value) {
            setenv(key, value, 1); // Store in process environment
        }
    }

    fclose(file);
}
