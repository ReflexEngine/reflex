#include "reflex_api.h"
#include "lua_api.h"
#include <stdlib.h>
#include "env.h"
#include "apis/process_api.h"
#include "require.h"
#include "version.h"

// Get environment variable
int env_get(lua_State *L) {
    const char *key = get_as_string(L, 1);
    const char *value = getenv(key);
    if (value) {
        return lua_return(L, REFLEX_TYPE_STRING, value);
    }
    return lua_return(L, REFLEX_TYPE_STRING, ""); // Return empty string if not found
}

// Set environment variable
int env_set(lua_State *L) {
    const char *key = get_as_string(L, 1);
    int result;

    // Check if the value is nil
    if (lua_isnil(L, 2)) {
        result = unsetenv(key);  // Remove the environment variable
    } else {
        const char *value = luaL_checkstring(L, 2);
        result = setenv(key, value, 1);  // Set the environment variable
    }

    // Return 1 if success, 0 otherwise
    return lua_return(L, REFLEX_TYPE_BOOLEAN, result == 0 ? 1 : 0);
}

// Register environment helper
void register_environment_helper(LuaAPI *api) {
    reflex_register_global_table(api, "env");
    reflex_register_table_field(api, "env", "get", REFLEX_TYPE_FUNCTION, env_get);
    reflex_register_table_field(api, "env", "set", REFLEX_TYPE_FUNCTION, env_set);
    reflex_register_table_field(api, "env", "load", REFLEX_TYPE_FUNCTION, load_env);
}

int ReflexVersion(lua_State *L) {
    return lua_return(L, REFLEX_TYPE_STRING, getVersion());
}

void register_reflex_metadata(LuaAPI *api) {
    reflex_register_global_table(api, "reflex");
    reflex_register_table_field(api, "reflex", "version", REFLEX_TYPE_FUNCTION, ReflexVersion);
}

void define_reflex_builtin(LuaAPI *api) {
    register_reflex_metadata(api);
    register_environment_helper(api);
    define_process_api(api);
    reflex_require_init(api);
}