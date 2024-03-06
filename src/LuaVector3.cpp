#include "LuaVector3.h"

Vector3 lua_tovector(lua_State* L, int index) {

	if (!lua_istable(L, index)) {
		throw "lua_tovector: table expected.";
	}

	Vector3 vector;

	lua_getfield(L, index, "x");
	vector.x = lua_tonumber(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, index, "y");
	vector.y = lua_tonumber(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, index, "z");
	vector.z = lua_tonumber(L, -1);
	lua_pop(L, 1);

	return vector;
}

void lua_pushvector(lua_State* L, const Vector3& vector)
{
	lua_newtable(L);
	
	lua_pushnumber(L, vector.x);
	lua_setfield(L, -2, "x");

	lua_pushnumber(L, vector.y);
	lua_setfield(L, -2, "y");

	lua_pushnumber(L, vector.z);
	lua_setfield(L, -2, "z");
}