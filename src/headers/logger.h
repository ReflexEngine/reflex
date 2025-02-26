#ifndef LOGGER_H
#define LOGGER_H

#include "lua_api.h"

// ANSI color codes
typedef enum {
    COLOR_RESET = 0,
    COLOR_RED,
    COLOR_GREEN,
    COLOR_YELLOW,
    COLOR_BLUE,
    COLOR_MAGENTA,
    COLOR_CYAN,
    COLOR_WHITE
} LogColor;

void println(const char* message);
void fprintln(FILE* stream, const char* message);
void printlogf(const char* format, ...);
void flogf(FILE* stream, const char* format, ...);
// Print colored log to stdout
void coloredlog(LogColor color, const char* format, ...);

// Print colored log to specified file stream
void fcoloredlog(FILE* stream, LogColor color, const char* format, ...);

// Color a specific word or phrase
char* colorize(const char* text, LogColor color);

// Multi-colored log to stdout
void multi_coloredlog(const char* format, ...);

// Multi-colored log to specified file stream
void fmulti_coloredlog(FILE* stream, const char* format, ...);
// Free memory allocated by colorize function
void free_colored_text(char* colored_text);

#endif