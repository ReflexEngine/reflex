#include "apis/reflex_logger_api.h"
#include "lua_api.h"
#include "logger.h"

void logger_info(lua_State* L) {
    
    char* info = colorize("INFO:", COLOR_GREEN);
    char* message = get_as_string(L, 1);
    multi_coloredlog("%s %s", info, message);
    return lua_return(L, REFLEX_TYPE_BOOLEAN, 1);

}

void logger_warn(lua_State* L) {
    
    char* info = colorize("WARN:", COLOR_YELLOW);
    char* message = get_as_string(L, 1);
    multi_coloredlog("%s %s", info, message);
    return lua_return(L, REFLEX_TYPE_BOOLEAN, 1);

}

void logger_error(lua_State* L) {
 
    char* info = colorize("ERROR:", COLOR_RED);
    char* message = get_as_string(L, 1);
    multi_coloredlog("%s %s", info, message);
    return lua_return(L, REFLEX_TYPE_BOOLEAN, 1);

}

void logger_debug(lua_State* L) {
    
    char* info = colorize("DEBUG:", COLOR_MAGENTA);
    char* message = get_as_string(L, 1);
    multi_coloredlog("%s %s", info, message);
    return lua_return(L, REFLEX_TYPE_BOOLEAN, 1);

}

void logger_override_print(lua_State* L) {

    reflex_register_function_L(L, "print", logger_info);
    return lua_return(L, REFLEX_TYPE_BOOLEAN, 1);

}

void define_logger_api(LuaAPI* api) {

    reflex_register_global_table(api, "reflex"); // incase
    reflex_register_table_field(api, "reflex", "logger", REFLEX_TYPE_TABLE);
    reflex_register_table_field(api, "reflex.logger", "info", REFLEX_TYPE_FUNCTION, logger_info);
    reflex_register_table_field(api, "reflex.logger", "warn", REFLEX_TYPE_FUNCTION, logger_warn);
    reflex_register_table_field(api, "reflex.logger", "error", REFLEX_TYPE_FUNCTION, logger_error);
    reflex_register_table_field(api, "reflex.logger", "debug", REFLEX_TYPE_FUNCTION, logger_debug);
    reflex_register_table_field(api, "reflex.logger", "overridePrint", REFLEX_TYPE_FUNCTION, logger_override_print);

}