#pragma once

extern "C" {
#include "lua.h"
}
#include <sstream>

std::string lua_stackdump(lua_State *L);