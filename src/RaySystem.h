#ifndef RAYSYSTEM_H
#define RAYSYSTEM_H
#include "System.h"
#include "Structs.h"

class RaySystem : public System
{
	lua_State* L;
public:
	RaySystem(lua_State* L) : L(L) {
		luaL_dofile(L, "scripts/ray.lua");
	}

	bool OnUpdate(entt::registry& registry, GameScreen& currentState, float delta) final {
		
		if (currentState == GAMEPLAY || currentState == EDITOR) {

			Vector4 colorData = Vector4();
			RayCollision meshHitInfo = { 0 };
			RayCollision groundHitInfo = { 0 };
			RayCollision boxHitInfo = { 0 };
			Camera mainCamera = registry.get<CameraComponent>((entt::entity)0).camera;
			auto rayView = registry.view<RayComponent, Behaviour>();
			auto modelView = registry.view<Renderable>();
			rayView.each([&](RayComponent& ray, Behaviour& script) {
			
				// Reset Ray
				lua_rawgeti(L, LUA_REGISTRYINDEX, script.LuaTableRef);
				lua_getfield(L, -1, "HitNothing");
				lua_pushvalue(L, -2);
				if (lua_pcall(L, 1, 5, 0) != LUA_OK) {
					DumpError(L);
				}

				ray.collider = { (bool)lua_toboolean(L, -1) };
				lua_pop(L, 1);
				ray.collider.distance = (float)lua_tonumber(L, -1);
				lua_pop(L, 1);
				ray.collider.hit = lua_toboolean(L, -1);
				lua_pop(L, 1);
				ray.strObjectName = lua_tostring(L, -1);
				lua_pop(L, 1);
				colorData = lua_tovector4(L, -1);
				lua_pop(L, 1);
				lua_pop(L, 1);
				
				Color color = { colorData.x, colorData.y, colorData.z, colorData.w };
				ray.cursorColor = color;
				ray.ray = GetMouseRay(GetMousePosition(), mainCamera);
				
				auto groundView = registry.view<GroundTile>();
				groundView.each([&](GroundTile& tile) {
					groundHitInfo = GetRayCollisionQuad(ray.ray, tile.topLeft, tile.bottomLeft, tile.bottomRight, tile.topRight);
					tile.gotHit = false;
					if ((groundHitInfo.hit) && (groundHitInfo.distance < ray.collider.distance))
					{
						lua_rawgeti(L, LUA_REGISTRYINDEX, script.LuaTableRef);
						lua_getfield(L, -1, "HitGround");
						lua_pushvalue(L, -2);

						if (lua_pcall(L, 1, 2, 0) != LUA_OK) {
							DumpError(L);
						}

						ray.collider = groundHitInfo;
						ray.strObjectName = lua_tostring(L, -1);
						lua_pop(L, 1);
						colorData = lua_tovector4(L, -1);
						lua_pop(L, 1);
						Color color = { colorData.x, colorData.y, colorData.z, colorData.w };
						ray.cursorColor = color;
						lua_pop(L, 1);
				
						tile.gotHit = true;
					}
					});

				// Check ray collision against bounding box first, before trying the full ray-mesh test
				modelView.each([&](Renderable& model) {

					model.gotHit = false;
					boxHitInfo = GetRayCollisionBox(ray.ray, model.renderBBox);
					if ((boxHitInfo.hit) && (boxHitInfo.distance < ray.collider.distance))
					{
						model.gotHit = true;
						lua_rawgeti(L, LUA_REGISTRYINDEX, script.LuaTableRef);
						lua_getfield(L, -1, "HitBox");
						lua_pushvalue(L, -2);

						if (lua_pcall(L, 1, 2, 0) != LUA_OK) {
							DumpError(L);
						}

						ray.collider = boxHitInfo;
						ray.strObjectName = lua_tostring(L, -1);
						lua_pop(L, 1);
						colorData = lua_tovector4(L, -1);
						lua_pop(L, 1);
						Color color = { colorData.x, colorData.y, colorData.z, colorData.w };
						ray.cursorColor = color;				
						lua_pop(L, 1);
			

						// Check ray collision against model meshes
						meshHitInfo = { 0 };
						for (int m = 0; m < model.renderableModel.meshCount; m++)
						{
							meshHitInfo = GetRayCollisionMesh(ray.ray, model.renderableModel.meshes[m], model.renderableModel.transform);
							if (meshHitInfo.hit)
							{
								// Save the closest hit mesh
								if ((!ray.collider.hit) || (ray.collider.distance > meshHitInfo.distance))
									ray.collider = meshHitInfo;

								break;  // Stop once one mesh collision is detected, the colliding mesh is m
							}
						}

						if (meshHitInfo.hit)
						{
							lua_rawgeti(L, LUA_REGISTRYINDEX, script.LuaTableRef);
							lua_getfield(L, -1, "HitMesh");
							lua_pushvalue(L, -2);
							if (lua_pcall(L, 1, 2, 0) != LUA_OK) {
								DumpError(L);
							}
							ray.collider = meshHitInfo;
							ray.strObjectName = lua_tostring(L, -1);
							lua_pop(L, 1);
							colorData = lua_tovector4(L, -1);
							lua_pop(L, 1);
							Color color = { colorData.x, colorData.y, colorData.z, colorData.w };
							ray.cursorColor = color;
							lua_pop(L, 1);
							
						}
					}
					});
				});
		}

		if (lua_gettop(L) > 0) {
			printf("RAY %i", lua_gettop(L));
			DumpStack(L);
			lua_pop(L, lua_gettop(L));
		}

		return false;
	}

	void Draw(entt::registry& registry, GameScreen currentState) final {

		if (currentState == GAMEPLAY || currentState == EDITOR) {
			BeginMode3D(registry.get<CameraComponent>((entt::entity)0).camera);
				auto rayView = registry.view<RayComponent>();
				rayView.each([&](RayComponent& ray) {

					if (ray.collider.hit)
					{
						DrawCube(ray.collider.point, 0.3f, 0.3f, 0.3f, ray.cursorColor);

						Vector3 normalEnd = Vector3();
						normalEnd.x = ray.collider.point.x + ray.collider.normal.x;
						normalEnd.y = ray.collider.point.y + ray.collider.normal.y;
						normalEnd.z = ray.collider.point.z + ray.collider.normal.z;

						DrawLine3D(ray.collider.point, normalEnd, RED);
					}
					DrawRay(ray.ray, ray.cursorColor);
				});
			EndMode3D();
		}


		//if (currentState == EDITOR) {
		//	auto rayView = registry.view<RayComponent>();
		//	rayView.each([&](RayComponent& ray) {

		//		// Draw some debug GUI text
		//		DrawText(TextFormat("Hit Object: %s", ray.strObjectName.c_str()), 10, 50, 10, BLACK);

		//		if (ray.collider.hit)
		//		{
		//			int ypos = 70;

		//			DrawText(TextFormat("Distance: %3.2f", ray.collider.distance), 10, ypos, 10, BLACK);

		//			DrawText(TextFormat("Hit Pos: %3.2f %3.2f %3.2f",
		//				ray.collider.point.x,
		//				ray.collider.point.y,
		//				ray.collider.point.z), 10, ypos + 15, 10, BLACK);

		//			DrawText(TextFormat("Hit Norm: %3.2f %3.2f %3.2f",
		//				ray.collider.normal.x,
		//				ray.collider.normal.y,
		//				ray.collider.normal.z), 10, ypos + 30, 10, BLACK);
		//		}
		//	});
		//}
	}

	void OnEnd(entt::registry& registry) final {}
};
#endif
