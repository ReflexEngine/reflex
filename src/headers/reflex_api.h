#ifndef REFLEX_API_H
#define REFLEX_API_H

#include "lua_api.h"

void register_reflex_metadata(LuaAPI *api);
void define_reflex_builtin(LuaAPI *api);

// Environment Helper
void register_environment_helper(LuaAPI *api);

#endif // REFLEX_API_H
