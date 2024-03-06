#ifndef LUAVECTOR4_H
#define LUAVECTOR4_H
#include "lua.hpp"
#include "..\raylibrary\raylib.h"

Vector4 lua_tovector4(lua_State* L, int index);
void lua_pushvector4(lua_State* L, const Vector4& vector);
#endif