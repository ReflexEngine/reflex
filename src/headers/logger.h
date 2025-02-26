#ifndef LOGGER_H
#define LOGGER_H

#include "lua_api.h"

void println(const char* message);
void fprintln(FILE* stream, const char* message);
void printlogf(const char* format, ...);
void fprintlogf(FILE* stream, const char* format, ...);

#endif