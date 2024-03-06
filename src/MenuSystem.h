#ifndef MENUSYSTEM_H
#define MENUSYSTEM_H

#include "System.h"
#include "Structs.h"

class MenuSystem : public System
{
	lua_State* L;
	std::string spawnObjectName;
public:
	MenuSystem(lua_State* L) : L(L), spawnObjectName("Remove") {
		luaL_dofile(L, "scripts/button.lua");
	}

	bool OnUpdate(entt::registry& registry, GameScreen& currentState, float delta) final {

		if (currentState == ENDING) {
			spawnObjectName = "Remove";
		}

		if (currentState == GAMEPLAY || currentState == EDITOR ) {
			if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
				auto view = registry.view<Button, SpawnButton>(entt::exclude_t<StateButton>());
				view.each([&](Button& button, SpawnButton& spawn) {
					if ((GameScreen)button.state == currentState && CheckCollisionPointRec(GetMousePosition(), button.rec))
						spawnObjectName = spawn.objectName;
					});
			}
		}

		if (currentState == GAMEPLAY && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {

			auto groundNodeView = registry.view<GroundTile, PathNode>();
			groundNodeView.each([&](GroundTile& tile, PathNode& node) {
				if (tile.gotHit && node.blocking == 1) {
					if (tile.towerID == -1 && spawnObjectName != "Remove") {
						lua_getglobal(L, "createTower");
						lua_getfield(L, -1, spawnObjectName.c_str());
						lua_pushvector(L, tile.center);
						lua_pushvalue(L, -2);
						if (lua_pcall(L, 2, 1, 0) != LUA_OK)
							DumpError(L);
						tile.towerID = lua_tointeger(L, -1);
						lua_pop(L, 1);
						lua_pop(L, lua_gettop(L));
					}
					else if (tile.towerID != -1 && spawnObjectName == "Remove") {
						lua_getglobal(L, "createTower");
						lua_getfield(L, -1, spawnObjectName.c_str());
						lua_pushinteger(L, tile.towerID);
						lua_pushvalue(L, -2);
						if (lua_pcall(L, 2, 0, 0) != LUA_OK)
							DumpError(L);
						tile.towerID = -1;
						lua_pop(L, lua_gettop(L));							
					}
				}
				});
		}

		if (lua_gettop(L) > 0)
			printf("MENU");


		return false;
	}

	void Draw(entt::registry& registry, GameScreen currentState) final {
		auto view = registry.view<Button, Behaviour>();
		view.each([&](Button& button, Behaviour& script) {		
			lua_rawgeti(L, LUA_REGISTRYINDEX, script.LuaTableRef);
			lua_getfield(L, -1, "Draw");
			lua_pushvalue(L, -2);	
			lua_pushinteger(L, (int)currentState);
			if (lua_pcall(L, 2, 0, 0) != LUA_OK)
				DumpError(L);
			lua_pop(L, 1);							
		});
			

		if (currentState == GAMEPLAY) {
			auto barView = registry.view<BaseBar, Behaviour>();
			barView.each([&](BaseBar& bar, Behaviour& script) {
				lua_rawgeti(L, LUA_REGISTRYINDEX, script.LuaTableRef);
				lua_getfield(L, -1, "Draw");
				lua_pushvalue(L, -2);
				if (lua_pcall(L, 1, 0, 0) != LUA_OK)
					DumpError(L);
				lua_pop(L, 1);
			});
		}

		if (currentState == GAMEPLAY || currentState == EDITOR) {
			auto view2 = registry.view<Button, SpawnButton, Behaviour>(entt::exclude_t<StateButton>());
			view2.each([&](Button& button, SpawnButton& spawn, Behaviour& script) {
				if ((GameScreen)button.state == currentState) {
					if (spawnObjectName == spawn.objectName) {
						lua_rawgeti(L, LUA_REGISTRYINDEX, script.LuaTableRef);
						lua_getfield(L, -1, "Selected");
						lua_pushvalue(L, -2);
						if (lua_pcall(L, 1, 0, 0) != LUA_OK)
							DumpError(L);
						lua_pop(L, 1);
					}

				}
				});
		}
		
	}

	void OnEnd(entt::registry& registry) final {}
};
#endif