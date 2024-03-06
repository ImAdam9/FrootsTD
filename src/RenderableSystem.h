#ifndef RENDERABLESYSTEM_H
#define RENDERABLESYSTEM_H
#include "System.h"
#include "Structs.h"
#include "..\raylibrary\raymath.h"

class RenderableSystem : public System
{
	lua_State* L;
public:
	RenderableSystem(lua_State* L) : L(L) {
		luaL_dofile(L, "scripts/tower.lua");
		luaL_dofile(L, "scripts/waves.lua");
	}

	bool OnUpdate(entt::registry& registry, GameScreen& currentState, float delta) final { return false; }

	void Draw(entt::registry& registry, GameScreen currentState) final {
		if (currentState == GAMEPLAY) {

			BeginMode3D(registry.get<CameraComponent>((entt::entity)0).camera);

			auto viewTE = registry.view<Position, Renderable>();
			viewTE.each([](Position& pos, Renderable& model) {
				DrawModel(model.renderableModel, pos.position, (float)model.scale, model.color);
			});

			auto viewRange = registry.view<Position, Renderable, Tower>();
			viewRange.each([](Position& pos, Renderable& model, Tower& tower) {
				if (model.gotHit) {
					DrawBoundingBox(model.renderBBox, model.color);
					DrawCircle3D({ pos.position.x, pos.position.y + 1.f, pos.position.z }, tower.range, { 1, 0, 0 }, 90.f, WHITE);
				}
			});

			auto viewP = registry.view<Projectile>();
			viewP.each([](Projectile& bullet) {
				DrawCube(bullet.position, bullet.transform.x, bullet.transform.y, bullet.transform.z, WHITE);
				});

			EndMode3D();
		}
	}

	void OnEnd(entt::registry& registry) final {
		auto view = registry.view<Renderable>();
		view.each([&](Renderable& model) {
			UnloadTexture(model.renderableTexture);
			UnloadModel(model.renderableModel);
			});
	}

};
#endif
