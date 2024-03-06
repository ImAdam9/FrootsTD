#ifndef BACKGROUNDSYSTEM_H
#define BACKGROUNDSYSTEM_H

#include "System.h"

class BackgroundSystem : public System
{
	lua_State* L;
public:
	BackgroundSystem(lua_State* L) : L(L) {
		luaL_dofile(L, "scripts/backgrounds.lua");
	}

	bool OnUpdate(entt::registry& registry, GameScreen& currentState, float delta) final {
		return false;
	}

	void Draw(entt::registry& registry, GameScreen currentState) final {

		lua_getglobal(L, "background");
		switch (currentState)
		{
		case TITLE:
		{
			lua_getfield(L, -1, "Title");
			lua_pushvalue(L, -2);
			lua_pcall(L, 1, 0, 0);
			lua_pop(L, 1);

		}break;
		case GAMEPLAY:
		{
			lua_getfield(L, -1, "Gameplay");
			lua_pushvalue(L, -2);
			lua_pcall(L, 1, 0, 0);
			lua_pop(L, 1);

		}break;
		case ENDING:
		{
			lua_getfield(L, -1, "Ending");
			lua_pushvalue(L, -2);
			lua_pcall(L, 1, 0, 0);
			lua_pop(L, 1);

		}break;
		case EDITOR:
		{
			lua_getfield(L, -1, "Editor");
			lua_pushvalue(L, -2);
			lua_pcall(L, 1, 0, 0);
			lua_pop(L, 1);

		} break;
		default: break;
		}	
	}

	void OnEnd(entt::registry& registry) final {}
};
#endif