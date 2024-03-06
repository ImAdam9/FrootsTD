#ifndef LUACONSOLE_H
#define LUACONSOLE_H
#include "lua.hpp"

void ConsoleThreadFunction(lua_State* L);
void DumpError(lua_State* L);
void DumpStack(lua_State* L);

#endif