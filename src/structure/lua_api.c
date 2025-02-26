#include "lua_api.h"
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

LuaAPI* reflex_new(void) {
    LuaAPI *api = (LuaAPI*)malloc(sizeof(LuaAPI));
    if (!api) {
        return NULL;
    }
    
    api->L = luaL_newstate();
    if (!api->L) {
        free(api);
        return NULL;
    }
    
    luaL_openlibs(api->L);
    return api;
}

void reflex_free(LuaAPI *api) {
    if (!api) {
        return;
    }
    
    lua_close(api->L);
    free(api);
}

void reflex_register_function(LuaAPI *api, const char *name, lua_CFunction func) {
    if (!api || !name || !func) {
        return;
    }
    
    lua_register(api->L, name, func);
}

void reflex_register_function_L(lua_State* L, const char *name, lua_CFunction func) {
    if (!L || !name || !func) {
        return;
    }
    
    lua_register(L, name, func);
} 

void reflex_register_global_number(LuaAPI *api, const char *name, double value) {
    if (!api || !name) {
        return;
    }
    
    lua_pushnumber(api->L, value);
    lua_setglobal(api->L, name);
}

void reflex_register_global_string(LuaAPI *api, const char *name, const char *value) {
    if (!api || !name || !value) {
        return;
    }
    
    lua_pushstring(api->L, value);
    lua_setglobal(api->L, name);
}

void reflex_register_global_table(LuaAPI *api, const char *table_name) {
    if (!api || !table_name) {
        return;
    }
    
    lua_newtable(api->L);
    lua_setglobal(api->L, table_name);
}

static void navigate_lua_stack(LuaAPI *api, const char *path) {
    if (!api || !path || !*path) {
        fprintf(stderr, "Error: Invalid path provided.\n");
        return;
    }

    // Make a copy of the path to safely tokenize
    char *path_copy = strdup(path);
    if (!path_copy) {
        fprintf(stderr, "Error: Memory allocation failed for path_copy.\n");
        return;
    }

    // Start with the global table
    lua_pushglobaltable(api->L);

    // Navigate through the Lua stack
    char *context = NULL;
    char *token = strtok_r(path_copy, ".", &context);
    
    while (token) {
        // Check if current table has this key
        lua_pushstring(api->L, token);
        lua_gettable(api->L, -2);
        
        if (lua_isnil(api->L, -1) || !lua_istable(api->L, -1)) {
            // Remove the nil or non-table value
            lua_pop(api->L, 1);
            
            // Create a new table at this position
            lua_pushstring(api->L, token);
            lua_newtable(api->L);
            lua_settable(api->L, -3);
            
            // Get the table we just created
            lua_pushstring(api->L, token);
            lua_gettable(api->L, -2);
        }
        
        // Move the new table to the top and remove the previous parent
        lua_remove(api->L, -2);
        
        token = strtok_r(NULL, ".", &context);
    }

    free(path_copy);
}

void reflex_register_table_field(LuaAPI *api, const char *table_path, const char *key, ReflexType type, ...) {
    if (!api || !table_path || !key) {
        fprintf(stderr, "Error: Invalid parameters provided.\n");
        return;
    }

    va_list args;
    va_start(args, type);

    // Navigate to the table where we want to set the field
    navigate_lua_stack(api, table_path);
    
    // Push the key
    lua_pushstring(api->L, key);
    
    // Push the value based on the provided type
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
        default:
            lua_pushnil(api->L);
            break;
    }
    
    // Set the field in the table
    lua_settable(api->L, -3);
    
    // Pop the table from the stack
    lua_pop(api->L, 1);
    
    va_end(args);
}

int lua_return(lua_State *L, ReflexType type, ...) {
    if (!L) {
        return 0; // Return 0 if Lua state is invalid
    }

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
        default:
            lua_pushnil(L);
            break;
    }

    va_end(args);
    return 1;  // Always return 1 value to Lua
}

int reflex_execute(LuaAPI *api, const char *sourceCode) {
    if (!api || !sourceCode) {
        return LUA_ERRRUN; // Return an error code if API or code is invalid
    }
    return luaL_dostring(api->L, sourceCode);
}

double get_as_number(lua_State *L, int index) {
    if (!L || !lua_isnumber(L, index)) {
        return 0.0; // Default value if not a number
    }
    return lua_tonumber(L, index);
}

const char* get_as_string(lua_State *L, int index) {
    if (!L || !lua_isstring(L, index)) {
        return NULL; // Return NULL if not a string
    }
    return lua_tostring(L, index);
}

int get_as_boolean(lua_State *L, int index) {
    if (!L || !lua_isboolean(L, index)) {
        return 0; // Default to false if not a boolean
    }
    return lua_toboolean(L, index);
}

lua_CFunction get_as_function(lua_State *L, int index) {
    if (!L || !lua_isfunction(L, index)) {
        return NULL;
    }
    return lua_tocfunction(L, index);
}

int get_as_table(lua_State *L, int index) {
    if (!L || !lua_istable(L, index)) {
        return LUA_NOREF; // Invalid reference
    }
    lua_pushvalue(L, index);
    return luaL_ref(L, LUA_REGISTRYINDEX); // Store in registry
}

void reflex_create_table_L(lua_State *L) {
    if (!L) {
        return;
    }
    lua_newtable(L);
}

void reflex_push_L(lua_State *L, ReflexType type, ...) {
    if (!L) {
        return;
    }

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
        default:
            lua_pushnil(L);
            break;
    }

    va_end(args);
}

void reflex_create_table(LuaAPI *api) {
    if (!api) {
        return;
    }
    lua_newtable(api->L);
}

void reflex_push(LuaAPI *api, ReflexType type, ...) {
    if (!api) {
        return;
    }
    
    va_list args;
    va_start(args, type);
    
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
        default:
            lua_pushnil(api->L);
            break;
    }
    
    va_end(args);
}