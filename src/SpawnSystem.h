#ifndef SPAWNSYSTEM_H
#define SPAWNSYSTEM_H
#include "System.h"
#include "Structs.h"
#include <string>

class SpawnSystem : public System
{
	lua_State* L;
	int frameCounter;
	int waveNumber;
	std::string waveStr;
	int waitTime;
public:
	SpawnSystem(lua_State* L) : L(L), frameCounter(0), waveNumber(0), waveStr(""), waitTime(0) {
		luaL_dofile(L, "scripts/baseHealthBar.lua");

		lua_getglobal(L, "createBase");
		lua_getfield(L, -1, "HealthBar");
		lua_pushvalue(L, -2);
		if (lua_pcall(L, 1, 0, 0) != LUA_OK)
			DumpError(L);
		lua_pop(L, 1);
		
		SetWave();
	}

	bool OnUpdate(entt::registry& registry, GameScreen& currentState, float delta) final {

		if (currentState == GAMEPLAY) {
		
			if (SetWave()) {				
				lua_getglobal(L, waveStr.c_str());
				lua_pushinteger(L, frameCounter);
				lua_pushvalue(L, -2);
				if (lua_pcall(L, 2, 1, 0) != LUA_OK)
					DumpError(L);
				frameCounter = lua_tointeger(L, -1);
				lua_pop(L, lua_gettop(L));		
			}
			frameCounter++;
		}

		return false;
	}

	void Draw(entt::registry& registry, GameScreen currentState) final {
		if (currentState == GAMEPLAY) {
			lua_getglobal(L, "WaveDraw");
			lua_pushinteger(L, frameCounter);
			lua_pushvalue(L, -2);
			if (lua_pcall(L, 2, 0, 0) != LUA_OK)
				DumpError(L);
			if(lua_gettop(L) > 0)
				lua_pop(L, lua_gettop(L));
		}
	
	}

	void OnEnd(entt::registry& registry) final {}

private:
	bool SetWave() {
		lua_getglobal(L, "g_wave");
		waveNumber = lua_tonumber(L, -1);	
		lua_pop(L, 1);
		waveStr = "wave" + std::to_string(waveNumber);
		if (waveNumber > 0)
			return true;
		else
			return false;
	}
};
#endif