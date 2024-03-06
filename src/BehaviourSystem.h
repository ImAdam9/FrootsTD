#ifndef BEHAVIOURSYSTEM_H
#define BEHAVIOURSYSTEM_H
#include "System.h"
#include "Structs.h"
#include "LuaConsole.h"

class BehaviourSystem : public System
{
	lua_State* L;
public:
	BehaviourSystem(lua_State* L) : L(L) {}

	bool OnUpdate(entt::registry& registry, GameScreen& currentState, float delta) final {
		auto viewB = registry.view<Behaviour>();

		viewB.each([&](Behaviour& script) {
			lua_rawgeti(L, LUA_REGISTRYINDEX, script.LuaTableRef);
			lua_getfield(L, -1, "OnUpdate");
			lua_pushvalue(L, -2);
			lua_pushnumber(L, delta);
			if (lua_pcall(L, 2, 0, 0) != LUA_OK) { 
				DumpError(L);
			}
			lua_pop(L, 1);
		});
		return false;
	}

	void Draw(entt::registry& registry, GameScreen currentState) final {}

	void OnEnd(entt::registry& registry) final {}
};
#endif
