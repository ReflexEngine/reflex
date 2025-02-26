#ifndef LUA_ERROR_H
#define LUA_ERROR_H

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <stdio.h>

/**
 * @brief Structure to hold formatted error information
 */
typedef struct {
    char message[512];      // Main error message
    char source[256];       // Source file where error occurred
    int line;               // Line number where error occurred
    char traceback[1024];   // Call stack traceback
    char context[512];      // Code context if available
} LuaErrorInfo;

/**
 * @brief Custom error handler function for Lua
 * 
 * @param L The Lua state
 * @return int Number of return values pushed onto the stack
 */
int lua_error_handler(lua_State *L);

/**
 * @brief Formats a Lua error into a more readable structure
 * 
 * @param L The Lua state
 * @param error_message The original error message
 * @param info Pointer to LuaErrorInfo structure to fill
 */
void lua_format_error(lua_State *L, const char *error_message, LuaErrorInfo *info);

/**
 * @brief Prints a formatted error to stderr with colors and formatting
 * 
 * @param info The error information structure
 */
void lua_print_error(const LuaErrorInfo *info);

/**
 * @brief Sets up error handling for a Lua state
 * 
 * @param L The Lua state
 * @return int Index of the error handler on the stack
 */
int lua_setup_error_handler(lua_State *L);

#endif // LUA_ERROR_H