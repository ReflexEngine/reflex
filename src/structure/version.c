#include "version.h"
#include "lua.h"

const char* getVersion() {
    return "v0.0.1";
}

const char* getLuaVersion() {
    return LUA_VERSION;
}