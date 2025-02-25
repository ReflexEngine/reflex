#include "lua_api.h"
#include <stdarg.h>
#include <stdlib.h>

LuaAPI* reflex_new() {
    LuaAPI *api = (LuaAPI*)malloc(sizeof(LuaAPI));
    api->L = luaL_newstate();
    luaL_openlibs(api->L);
    return api;
}

void reflex_free(LuaAPI *api) {
    if (api) {
        lua_close(api->L);
        free(api);
    }
}

void reflex_register_function(LuaAPI *api, const char *name, lua_CFunction func) {
    lua_register(api->L, name, func);
}

void reflex_register_global_number(LuaAPI *api, const char *name, double value) {
    lua_pushnumber(api->L, value);
    lua_setglobal(api->L, name);
}

void reflex_register_global_string(LuaAPI *api, const char *name, const char *value) {
    lua_pushstring(api->L, value);
    lua_setglobal(api->L, name);
}

void reflex_register_global_table(LuaAPI *api, const char *table_name) {
    lua_newtable(api->L);
    lua_setglobal(api->L, table_name);
}

void reflex_register_table_field(LuaAPI *api, const char *table_name, const char *key, ReflexType type, ...) {
    va_list args;
    va_start(args, type);

    lua_getglobal(api->L, table_name);
    if (!lua_istable(api->L, -1)) {
        lua_pop(api->L, 1);
        reflex_register_global_table(api, table_name);
        lua_getglobal(api->L, table_name);
    }

    lua_pushstring(api->L, key);

    switch (type) {
        case REFLEX_TYPE_NUMBER:
            lua_pushnumber(api->L, va_arg(args, double));
            break;
        case REFLEX_TYPE_STRING:
            lua_pushstring(api->L, va_arg(args, const char *));
            break;
        case REFLEX_TYPE_BOOLEAN:
            lua_pushboolean(api->L, va_arg(args, int));
            break;
        case REFLEX_TYPE_FUNCTION:
            lua_pushcfunction(api->L, va_arg(args, lua_CFunction));
            break;
        case REFLEX_TYPE_TABLE:
            lua_newtable(api->L);
            break;
        case REFLEX_TYPE_NIL:
            lua_pushnil(api->L);
            break;
    }

    lua_settable(api->L, -3);
    lua_pop(api->L, 1);

    va_end(args);
}

int lua_return(lua_State *L, ReflexType type, ...) {
    va_list args;
    va_start(args, type);

    switch (type) {
        case REFLEX_TYPE_NUMBER:
            lua_pushnumber(L, va_arg(args, double));
            break;
        case REFLEX_TYPE_STRING:
            lua_pushstring(L, va_arg(args, const char *));
            break;
        case REFLEX_TYPE_BOOLEAN:
            lua_pushboolean(L, va_arg(args, int));
            break;
        case REFLEX_TYPE_FUNCTION:
            lua_pushcfunction(L, va_arg(args, lua_CFunction));
            break;
        case REFLEX_TYPE_TABLE:
            lua_newtable(L);
            break;
        case REFLEX_TYPE_NIL:
            lua_pushnil(L);
            break;
        default:
            lua_pushnil(L);
            break;
    }

    va_end(args);
    return 1;  // Always returning 1 value to Lua
}

int reflex_execute(LuaAPI *api, const char *sourceCode) {
    return luaL_dostring(api->L, sourceCode);
}

double get_as_number(lua_State *L, int index) {
    if (lua_isnumber(L, index)) {
        return lua_tonumber(L, index);
    }
    return 0; // Default value if not a number
}

const char* get_as_string(lua_State *L, int index) {
    if (lua_isstring(L, index)) {
        return lua_tostring(L, index);
    }
    return NULL; // Return NULL if not a string
}

int get_as_boolean(lua_State *L, int index) {
    if (lua_isboolean(L, index)) {
        return lua_toboolean(L, index);
    }
    return 0; // Default to false if not a boolean
}

lua_CFunction get_as_function(lua_State *L, int index) {
    if (lua_isfunction(L, index)) {
        return lua_tocfunction(L, index);
    }
    return NULL;
}

int get_as_table(lua_State *L, int index) {
    if (lua_istable(L, index)) {
        lua_pushvalue(L, index);
        return luaL_ref(L, LUA_REGISTRYINDEX); // Store in registry
    }
    return LUA_NOREF; // Invalid reference
}

void reflex_create_table_L(lua_State *L) {
    lua_newtable(L);
}

void reflex_push_L(lua_State *L, ReflexType type, ...) {
    va_list args;
    va_start(args, type);

    switch (type) {
        case REFLEX_TYPE_NUMBER:
            lua_pushnumber(L, va_arg(args, double));
            break;
        case REFLEX_TYPE_STRING:
            lua_pushstring(L, va_arg(args, const char *));
            break;
        case REFLEX_TYPE_BOOLEAN:
            lua_pushboolean(L, va_arg(args, int));
            break;
        case REFLEX_TYPE_FUNCTION:
            lua_pushcfunction(L, va_arg(args, lua_CFunction));
            break;
        case REFLEX_TYPE_TABLE:
            lua_newtable(L);
            break;
        case REFLEX_TYPE_NIL:
            lua_pushnil(L);
            break;
    }

    va_end(args);
}

void reflex_create_table(LuaAPI *api) {
    reflex_create_table_L(api->L);
}

void reflex_push(LuaAPI *api, ReflexType type, ...) {
    va_list args;
    va_start(args, type);

    reflex_push_L(api->L, type, args);

    va_end(args);
}