#ifndef REQUIRE_H
#define REQUIRE_H

#include "lua_api.h"

// Initializes the package system (modifies `package` to support custom modules)
void reflex_require_init(LuaAPI *api);

// Loads a package by name
int reflex_require(lua_State *L);

// Registers a package dynamically
int reflex_register_package(lua_State *L, const char *package_name, const char *file_path);

// Sets the package search path
void reflex_set_package_path(const char *path);

#endif // REQUIRE_H
