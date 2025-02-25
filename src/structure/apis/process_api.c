#include "apis/process_api.h"
#include "lua_api.h"
#include <unistd.h>
#include <stdlib.h>
#include <sys/utsname.h>
#include <string.h>
#include <windows.h>
#include <VersionHelpers.h>
#include "logger.h"
#include "version.h"
#include "args.h"
#include "lua.h"

int process_platform(lua_State *L) {
#if defined(_WIN32) || defined(_WIN64)
    if (IsWindows10OrGreater()) {
        return lua_return(L, REFLEX_TYPE_STRING, "Windows 10+");
    }
    if (IsWindows8OrGreater()) {
        return lua_return(L, REFLEX_TYPE_STRING, "Windows 8");
    }
    if (IsWindows7OrGreater()) {
        return lua_return(L, REFLEX_TYPE_STRING, "Windows 7");
    }
    return lua_return(L, REFLEX_TYPE_STRING, "Windows");
#else
    struct utsname sys_info;
    if (uname(&sys_info) == 0) {
        return lua_return(L, REFLEX_TYPE_STRING, sys_info.sysname);
    }
    return lua_return(L, REFLEX_TYPE_STRING, "Unknown");
#endif
}

int process_exit(lua_State *L) {
    int exitCode = get_as_number(L, 1); // Ensure integer conversion
    logf("Exiting application with code: %d", exitCode);
    fflush(stdout);
    exit(exitCode);
}

// Get the current process ID
int process_pid(lua_State *L)
{
    pid_t pid = getpid(); // Get the current process ID
    return lua_return(L, REFLEX_TYPE_NUMBER, (double)pid);
}

int process_version(lua_State *L) {

    return lua_return(L, REFLEX_TYPE_STRING, getVersion());

}

int process_versions(lua_State *L) {
    reflex_create_table_L(L);

    // Reflex Version
    lua_pushstring(L, "reflex");
    reflex_push_L(L, REFLEX_TYPE_STRING, getVersion());
    lua_settable(L, -3);

    lua_pushstring(L, "lua");
    reflex_push_L(L, REFLEX_TYPE_STRING, getLuaVersion());
    lua_settable(L, -3);

    return 1;
}

void define_program_arguments(LuaAPI *api, Args args) {
    // Create a new table on the Lua stack
    reflex_create_table_L(api->L); 

    // Iterate over the arguments and push them to the Lua table
    for (int i = 0; i < args.count; i++) {
        reflex_push_L(api->L, REFLEX_TYPE_STRING, args.values[i]);  // Push each argument as a string
        lua_rawseti(api->L, -2, i + 1); // Lua tables are 1-indexed, so set at i + 1
    }

    // Set the table as process.argv
    lua_getglobal(api->L, "process");
    lua_pushstring(api->L, "argv");
    lua_pushvalue(api->L, -3);  // Copy the argv table to the top of the stack
    lua_settable(api->L, -3);   // process.argv = argv_table
    
    // Clean up the stack (remove argv table and process table)
    lua_pop(api->L, 2);
}

// Register process helper (directly under the global `process` table)
void define_process_api(LuaAPI *api)
{
    reflex_register_global_table(api, "process"); // Register global table "process"
    reflex_register_table_field(api, "process", "platform", REFLEX_TYPE_FUNCTION, process_platform);
    reflex_register_table_field(api, "process", "pid", REFLEX_TYPE_FUNCTION, process_pid);
    reflex_register_table_field(api, "process", "exit", REFLEX_TYPE_FUNCTION, process_exit);
    reflex_register_table_field(api, "process", "version", REFLEX_TYPE_FUNCTION, process_version);
    reflex_register_table_field(api, "process", "versions", REFLEX_TYPE_FUNCTION, process_versions);
}