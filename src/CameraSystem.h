#ifndef CAMERASYSTEM_H
#define CAMERASYSTEM_H
#include "System.h"
#include "Structs.h"

class CameraSystem : public System
{
	lua_State* L;
public:
	CameraSystem(lua_State* L) : L(L) {
		luaL_dofile(L, "scripts/camera.lua");
	}
	
	bool OnUpdate(entt::registry& registry, GameScreen& currentState, float delta) final {
		auto viewC = registry.view<CameraComponent>();
		viewC.each([](CameraComponent& camera) {
			UpdateCamera(&camera.camera);
		});
		return false;
	}

	void Draw(entt::registry& registry, GameScreen currentState) final {}

	void OnEnd(entt::registry& registry) final {}
};
#endif