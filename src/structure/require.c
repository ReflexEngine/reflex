#include "require.h"
#include "fs.h"
#include <string.h>
#include <stdio.h>

#define DEFAULT_PACKAGE_PATH "reflex_deps/?.lua"

static char package_path[256] = DEFAULT_PACKAGE_PATH;

void reflex_set_package_path(const char *path) {
    snprintf(package_path, sizeof(package_path), "%s?.lua", path);
}

// Custom package loader function
int reflex_package_loader(lua_State *L) {
    const char *package_name = get_as_string(L, 1);
    if (!package_name) {
        lua_pushnil(L);
        lua_pushstring(L, "Invalid package name");
        return 2;
    }

    char file_path[512];
    snprintf(file_path, sizeof(file_path), "%s/%s.lua", package_path, package_name);

    char *script = fs_read(file_path);
    if (!script) {
        lua_pushnil(L);
        lua_pushfstring(L, "Module '%s' not found in Reflex path: %s", package_name, file_path);
        return 2;
    }

    if (luaL_loadstring(L, script) || lua_pcall(L, 0, 1, 0)) {
        const char *error_msg = lua_tostring(L, -1);
        lua_pushnil(L);
        lua_pushstring(L, error_msg);
        free(script);
        return 2;
    }

    lua_getglobal(L, "package");
    lua_getfield(L, -1, "loaded");
    lua_pushvalue(L, 1);
    lua_pushvalue(L, -4);
    lua_settable(L, -3);

    lua_pop(L, 2);

    free(script);
    return 1;
}

void reflex_require_init(LuaAPI *api) {
    lua_State *L = api->L;

    // Get package table
    lua_getglobal(L, "package");

    // Set package.path to prioritize Reflex Engine modules
    lua_pushfstring(L, "%s;%s", package_path, lua_tostring(L, -1));
    lua_setfield(L, -2, "path");

    // Get package.searchers (or package.loaders in Lua 5.1)
    lua_getfield(L, -1, "searchers");
    if (lua_istable(L, -1)) {
        int n = lua_rawlen(L, -1);
        
        // Move existing searchers up and insert Reflex loader at index 2
        for (int i = n; i >= 2; i--) {
            lua_rawgeti(L, -1, i);
            lua_rawseti(L, -2, i + 1);
        }

        lua_pushcfunction(L, reflex_package_loader);
        lua_rawseti(L, -2, 2);
    }

    lua_pop(L, 2);

    // Debug: Print package.path
    lua_getglobal(L, "package");
    lua_getfield(L, -1, "path");
    lua_pop(L, 2);
}
