#ifndef luaSD_MAXDEPTH
#define luaSD_MAXDEPTH 20
#endif

#include "lua.hpp"
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#define _CRT_NO_VA_START_VALIDATION

static std::string format(const std::string &format, ...)
{
	va_list args;
	va_start(args, format);
	size_t len = std::vsnprintf(NULL, 0, format.c_str(), args);
	va_end(args);
	std::vector<char> vec(len + 1);
	va_start(args, format);
	std::vsnprintf(&vec[0], len + 1, format.c_str(), args);
	va_end(args);
	return &vec[0];
}

static void lua_stackdumpvalue(lua_State *state, std::ostringstream &out, int stackIndex, int depth, int newline, int indentLevel, int performIndent)
{
	int t;
	/* ensure that the value is on the top of the stack */
	lua_pushvalue(state, stackIndex);
	t = lua_type(state, -1);
	switch (t)
	{
	case LUA_TNUMBER:
		lua_pushfstring(state, "%f (%s)", lua_tonumber(state, -1), luaL_typename(state, -1));
		break;
	case LUA_TSTRING:
		lua_pushfstring(state, "%s (%s)", lua_tostring(state, -1), luaL_typename(state, -1));
		break;
	case LUA_TLIGHTUSERDATA:
	case LUA_TUSERDATA:
	{
		if (lua_getmetatable(state, -1) == 0)
		{
			lua_pushfstring(state, "%s [%p] (no metatable)", luaL_typename(state, -1), lua_topointer(state, -1));
		}
		else
		{
			/* TODO: this else branch was never reached in my tests */
			lua_getfield(state, -1, "__tostring");
			if (lua_isfunction(state, -1))
			{
				if (lua_pcall(state, 0, 1, 0))
				{
					lua_pushfstring(state, "%s [%p]", luaL_typename(state, -1), lua_topointer(state, -1));
				}
				lua_pop(state, 1);
			}
			else
			{
				lua_pop(state, 1);
				lua_stackdumpvalue(state, out, -1, depth + 1, 1, indentLevel + 4, 1);
				lua_pop(state, 1);
				lua_pushfstring(state, "%s [%p]", luaL_typename(state, -1), lua_topointer(state, -1));
			}
			lua_pop(state, 1);
		}
		break;
	}
	case LUA_TBOOLEAN:
	{
		const int value = lua_toboolean(state, -1);
		if (value)
		{
			lua_pushliteral(state, "true");
		}
		else
		{
			lua_pushliteral(state, "false");
		}
		break;
	}
	case LUA_TNIL:
		lua_pushliteral(state, "nil");
		break;
	case LUA_TFUNCTION:
	{
#if 0
		/* TODO: somehow get function details. */
		lua_Debug ar;
		lua_getstack(state, -1, &ar);
		if (lua_getinfo(state, "nSlLtuf", &ar) != 0) {
			lua_pushfstring(state, "%s %s %d @%d %s", ar.namewhat, ar.name, ar.nups, ar.linedefined, ar.short_src);
		} else
#endif
		{
			lua_pushfstring(state, "%s [%p]", luaL_typename(state, -1), lua_topointer(state, -1));
		}
		break;
	}
	case LUA_TTABLE:
	{
		int len = 0;
		if (performIndent)
		{
			if (depth == 0)
			{
				out << format("%-5i | ", stackIndex);
			}
			else
			{
				out << format("       ", stackIndex);
			}
			for (int i = 0; i < indentLevel - 4; ++i)
			{
				out << format(" ");
			}
			if (indentLevel >= 4)
			{
				out << format("\\-- ");
			}
		}
		lua_pushnil(state);
		while (lua_next(state, -2))
		{
			lua_pop(state, 1);
			++len;
		}

		lua_pushfstring(state, "%s: %p (size: %d)",
						luaL_typename(state, -1),
						lua_topointer(state, -1),
						len);
		out << format("%s", lua_tostring(state, -1));
		lua_pop(state, 1);

		if (depth < luaSD_MAXDEPTH)
		{
			lua_pushnil(state);
			while (lua_next(state, -2))
			{
				out << format("\n");
				lua_stackdumpvalue(state, out, -2, depth + 1, 0, indentLevel + 4, 1);
				out << format(" = ");
				lua_stackdumpvalue(state, out, -1, depth + 1, 0, indentLevel + 4, 0);
				if (lua_type(state, -1) == LUA_TFUNCTION && lua_type(state, -2) == LUA_TSTRING)
				{
					if (!strcmp("__tostring", lua_tostring(state, -2)))
					{
						if (lua_pcall(state, 0, 1, 0))
						{
							/* TODO: push object to the stack to call __tostring from */
							if (lua_isstring(state, -1))
							{
								out << format(" (%s)", lua_tostring(state, -1));
							}
							else
							{
								out << format(" (failed to call __tostring)");
							}
						}
						else
						{
							out << format("%s", lua_tostring(state, -1));
							lua_pop(state, 1);
						}
					}
				}
				lua_pop(state, 1);
			}
		}
		/* pop the reference copy from the stack to restore the original state */
		lua_pop(state, 1);
		if (newline)
		{
			out << format("\n");
		}
		return;
	}
	default:
		lua_pushfstring(state, "%s [%p]", luaL_typename(state, -1), lua_topointer(state, -1));
		break;
	}

	{
		int width = 20;
		if (performIndent)
		{
			if (depth == 0)
			{
				out << format("%-5i | ", stackIndex);
			}
			else
			{
				out << format("       ", stackIndex);
			}
			for (int i = 0; i < indentLevel - 4; ++i)
			{
				out << format(" ");
			}
			if (indentLevel >= 4)
			{
				out << format("\\-- ");
			}
			width -= indentLevel;
		}
		out << format("%-*s", width, lua_tostring(state, -1));
	}
	if (newline)
	{
		out << format("\n");
	}
	/* pop the string and the reference copy from the stack to restore the original state */
	lua_pop(state, 2);
}

std::string lua_stackdump(lua_State *state)
{
	std::ostringstream out;
	const int top = lua_gettop(state);
	out << format("\n--------------------start-of-stacktrace----------------\n");
	out << format("index | details (%i entries)\n", top);
	int i;
	for (i = -1; i >= -top; --i)
	{
		lua_stackdumpvalue(state, out, i, 0, 1, 0, 1);
	}
	out << format("----------------------end-of-stacktrace----------------\n\n");
	return out.str();
}
