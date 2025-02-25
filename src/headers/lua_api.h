#ifndef LUA_API_H
#define LUA_API_H

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

typedef struct {
    lua_State *L;
} LuaAPI;

typedef enum {
    REFLEX_TYPE_NUMBER,
    REFLEX_TYPE_STRING,
    REFLEX_TYPE_BOOLEAN,
    REFLEX_TYPE_FUNCTION,
    REFLEX_TYPE_TABLE,
    REFLEX_TYPE_NIL
} ReflexType;

// API management functions
LuaAPI* reflex_new();
void reflex_free(LuaAPI *api);

// Registration functions
void reflex_register_function(LuaAPI *api, const char *name, lua_CFunction func);
void reflex_register_global_number(LuaAPI *api, const char *name, double value);
void reflex_register_global_string(LuaAPI *api, const char *name, const char *value);
void reflex_register_global_table(LuaAPI *api, const char *table_name);
void reflex_register_table_field(LuaAPI *api, const char *table_name, const char *key, ReflexType type, ...);

// Lua return helper
int lua_return(lua_State *L, ReflexType type, ...);

// Lua script execution
int reflex_execute(LuaAPI *api, const char *sourceCode);

// Separate get_as functions for each type
double get_as_number(lua_State *L, int index);
const char* get_as_string(lua_State *L, int index);
int get_as_boolean(lua_State *L, int index);
lua_CFunction get_as_function(lua_State *L, int index);
int get_as_table(lua_State *L, int index);

void reflex_create_table(LuaAPI *api);
void reflex_create_table_L(lua_State* state);
void reflex_push(LuaAPI *api, ReflexType type, ...);
void reflex_push_L(lua_State* state, ReflexType type, ...);

#endif // LUA_API_H
