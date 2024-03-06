#ifndef GROUNDSYSTEM_H
#define GROUNDSYSTEM_H
#include "System.h"
#include "Structs.h"

class GroundSystem : public System
{
private:
	lua_State* L;

public:

	GroundSystem(lua_State* L) : L(L) {
		luaL_dofile(L, "scripts/ground.lua");
	}

	bool OnUpdate(entt::registry& registry, GameScreen& currentState, float delta) final {
		if (lua_gettop(L) > 0)
			printf("GROUND");
		return false;
	}

	void Draw(entt::registry& registry, GameScreen currentState) final {
		if (currentState == GAMEPLAY || currentState == EDITOR) {

			BeginMode3D(registry.get<CameraComponent>((entt::entity)0).camera);
			
			auto groundView = registry.view<GroundTile>();
			groundView.each([&](GroundTile& tile) {
	
				DrawPlane(tile.center, { tile.cellSize , tile.cellSize }, tile.color);
				if (tile.gotHit)
				{
					DrawLine3D(tile.center, { tile.center.x, tile.center.y + 5.f, tile.center.z }, BLACK);
					DrawCubeWires({ tile.center.x, tile.center.y + 0.1f, tile.center.z }, tile.cellSize, 0.f, tile.cellSize, BLACK);
				}
				});
			EndMode3D();
		}
	}

	void OnEnd(entt::registry& registry) final {

	}
};
#endif