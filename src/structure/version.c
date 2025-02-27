#include "version.h"
#include "lua.h"
#include "uv.h"

const char* getVersion() {
    return "v0.0.1";
}

const char* getLuaVersion() {
    return LUA_VERSION;
}

const char* getLibuvVerison() {
    return uv_version_string();
}