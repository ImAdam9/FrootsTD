#ifndef CLEANUPSYSTEM_H
#define CLEANUPSYSTEM_H
#include"System.h"
#include"Structs.h"

class CleanupSystem : public System
{
public:

	bool OnUpdate(entt::registry& registry, GameScreen& currentState, float delta) final {
		if (currentState == ENDING) {
			auto enemyView = registry.view<Enemy>();
			enemyView.each([&](entt::entity entity, Enemy& enemy) {
				registry.destroy(entity);
				});

			auto towerView = registry.view<Tower>();
			towerView.each([&](entt::entity entity, Tower& tower) {
				registry.destroy(entity);
				});

			auto projecView = registry.view<Projectile>();
			projecView.each([&](entt::entity entity, Projectile& bullet) {
				registry.destroy(entity);
				});

			auto groundView = registry.view<GroundTile>();
			groundView.each([&](GroundTile& tile) {
				tile.towerID = -1;
			});
		}

		return false;
	}

	void Draw(entt::registry& registry, GameScreen currentState) final {

	}

	void OnEnd(entt::registry& registry) final {

	}
};
#endif