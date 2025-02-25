#include "version.h"
#include "lua.h"

const char* getVersion() {
    return "1.0.0";
}

const char* getLuaVersion() {
    return LUA_VERSION;
}