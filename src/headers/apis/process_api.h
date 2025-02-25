#ifndef PROCESS_API_H
#define PROCESS_API_H

#include "lua_api.h"
#include "args.h"

// Function declarations for process-related functionalities
int process_platform(lua_State *L);
int process_pid(lua_State *L);

void define_program_arguments(LuaAPI *api, Args args);
// Register process global table
void define_process_api(LuaAPI *api);

#endif // PROCESS_API_H
