/**
 * Copyright 2019-present, GraphQL Foundation
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "GraphQLAstToLUA.h"
#include "LuaGraphQLParser.h"
#include "GraphQLAstVisitor.h"
extern "C"
{
#include "lauxlib.h"
}
#include <cstring>
#include "AstNode.h"
#include "LuaGraphQLAstVisitor.h"

constexpr auto MAX_MSG_LEN = 256;

auto parse_query(lua_State *L) -> int
{
    const char *query = luaL_checkstring(L, 1);

    const char *error = nullptr;

    auto AST = facebook::graphql::parseString(query, &error);

    if (!AST)
    {
        lua_pushnil(L);
        lua_pushstring(L, error);
        return 2;
    }

    graphql_node_visit(reinterpret_cast<const struct GraphQLAstNode *>(AST.get()), &callbacks_test, reinterpret_cast<void*>(L));

    return 1;
}

auto parse_schema(lua_State *L) ->int
{
    const char *query = luaL_checkstring(L, 1);
    const char *error = nullptr;

    auto AST = facebook::graphql::parseStringWithExperimentalSchemaSupport(query, &error);

    if (!AST)
    {
        lua_pushnil(L);
        lua_pushstring(L, error);
        return 2;
    }

    graphql_node_visit(reinterpret_cast<const struct GraphQLAstNode *>(AST.get()), &callbacks_test, reinterpret_cast<void*>(L));

    return 1;
}


auto luaopen_fastparse_lib(lua_State *L) -> int
{
    static const struct luaL_Reg lib[] = {
        {"parse_query", parse_query},
        {"parse_schema", parse_schema},
        {nullptr, nullptr}};
    luaL_newlib(L, lib);
    return 1;
}