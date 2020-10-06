#include "GraphQLAstVisitor.h"

#ifdef __cplusplus
extern "C"
{
#endif

#include "lua.h"

extern struct GraphQLAstVisitorCallbacks callbacks_test;

#define obj_begin lua_newtable
#define arr_begin obj_begin
#define obj_end(L, key) lua_setfield(L, -2, key);
#define arr_end(L) lua_rawseti(L, -2, lua_objlen(L, -2) + 1);
#define arr_end_(L, index) lua_rawseti(L, -2, index);
#define obj_string(L, key, value) \
    lua_pushstring(L, value);     \
    lua_setfield(L, -2, key);
#define obj_number(L, key, value) \
    lua_pushnumber(L, value);     \
    lua_setfield(L, -2, key);
#define obj_integer(L, key, value) \
    lua_pushinteger(L, value);     \
    lua_setfield(L, -2, key);
#define obj_boolean(L, key, value) \
    lua_pushboolean(L, value);     \
    lua_setfield(L, -2, key);
#define obj_null(L, key) \
    lua_pushnil(L); \
    
#ifdef __cplusplus
}
#endif
