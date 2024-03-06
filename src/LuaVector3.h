#ifndef LUAVECTOR3_H
#define LUAVECTOR3_H
#include "lua.hpp"
#include "..\raylibrary\raylib.h"
Vector3 lua_tovector(lua_State* L, int index);
void lua_pushvector(lua_State* L, const Vector3& vector);


#endif