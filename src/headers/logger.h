#ifndef LOGGER_H
#define LOGGER_H

#include "lua_api.h"

void println(const char* message);
void fprintln(FILE* stream, const char* message);
void logf(const char* format, ...);
void flogf(FILE* stream, const char* format, ...);

#endif