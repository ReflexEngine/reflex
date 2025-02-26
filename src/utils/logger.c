#include "logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

// Get ANSI color code string
const char* get_color_code(LogColor color) {
    switch (color) {
        case COLOR_RESET:   return "\033[0m";
        case COLOR_RED:     return "\033[31m";
        case COLOR_GREEN:   return "\033[32m";
        case COLOR_YELLOW:  return "\033[33m";
        case COLOR_BLUE:    return "\033[34m";
        case COLOR_MAGENTA: return "\033[35m";
        case COLOR_CYAN:    return "\033[36m";
        case COLOR_WHITE:   return "\033[37m";
        default:            return "\033[0m";
    }
}

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

void flogf(FILE* stream, const char* format, ...) {
    va_list args;
    va_start(args, format);

    char *buffer;
    if (vasprintf(&buffer, format, args) != -1) {  // vasprintf allocates memory
        fprintln(stream, buffer);
        free(buffer);  // Free allocated memory after printing
    }

    va_end(args);
}

// Print colored log to stdout
void coloredlog(LogColor color, const char* format, ...) {
    va_list args;
    va_start(args, format);

    char *buffer;
    if (vasprintf(&buffer, format, args) != -1) {
        printf("%s%s%s\n", get_color_code(color), buffer, get_color_code(COLOR_RESET));
        free(buffer);
    }

    va_end(args);
}

// Print colored log to specified file stream
void fcoloredlog(FILE* stream, LogColor color, const char* format, ...) {
    va_list args;
    va_start(args, format);

    char *buffer;
    if (vasprintf(&buffer, format, args) != -1) {
        fprintf(stream, "%s%s%s\n", get_color_code(color), buffer, get_color_code(COLOR_RESET));
        free(buffer);
    }

    va_end(args);
}

// Color a specific word or phrase
char* colorize(const char* text, LogColor color) {
    const char* color_code = get_color_code(color);
    const char* reset_code = get_color_code(COLOR_RESET);
    
    size_t total_len = strlen(text) + strlen(color_code) + strlen(reset_code) + 1;
    char* result = (char*)malloc(total_len);
    
    if (result) {
        sprintf(result, "%s%s%s", color_code, text, reset_code);
    }
    
    return result;
}

// Multi-colored log to stdout
void multi_coloredlog(const char* format, ...) {
    va_list args;
    va_start(args, format);
    
    char *buffer;
    if (vasprintf(&buffer, format, args) != -1) {
        printf("%s\n", buffer);
        free(buffer);
    }
    
    va_end(args);
}

// Multi-colored log to specified file stream
void fmulti_coloredlog(FILE* stream, const char* format, ...) {
    va_list args;
    va_start(args, format);
    
    char *buffer;
    if (vasprintf(&buffer, format, args) != -1) {
        fprintf(stream, "%s\n", buffer);
        free(buffer);
    }
    
    va_end(args);
}

// Free memory allocated by colorize function
void free_colored_text(char* colored_text) {
    if (colored_text) {
        free(colored_text);
    }
}