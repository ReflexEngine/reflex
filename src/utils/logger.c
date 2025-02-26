#include "logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

void println(const char* message) {
    printf("%s\n", message);
}

void fprintln(FILE* stream, const char* message) {
    fprintf(stream, "%s\n", message);
}

// Prints a formatted message, like printf but with automatic newline
void printlogf(const char* format, ...) {
    va_list args;
    va_start(args, format);

    char *buffer;
    if (vasprintf(&buffer, format, args) != -1) {  // vasprintf allocates memory
        println(buffer);
        free(buffer);  // Free allocated memory after printing
    }

    va_end(args);
}

void fprintlogf(FILE* stream, const char* format, ...) {
    va_list args;
    va_start(args, format);

    char *buffer;
    if (vasprintf(&buffer, format, args) != -1) {  // vasprintf allocates memory
        fprintln(stream, buffer);
        free(buffer);  // Free allocated memory after printing
    }

    va_end(args);
}