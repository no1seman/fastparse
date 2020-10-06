/**
 * Copyright 2019-present, GraphQL Foundation
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "lua.h"
    auto parse_query(lua_State *L) -> int;
    auto parse_schema(lua_State *L) -> int;
    auto luaopen_fastparse_lib(lua_State *L) -> int;

#ifdef __cplusplus
}
#endif
