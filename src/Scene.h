#ifndef SCENE_H
#define SCHENE_H
#include <entt.hpp>
#include "lua.hpp"
#include "System.h"
#include "BehaviourSystem.h"
#include "CameraSystem.h"
#include "RaySystem.h"
#include "GroundSystem.h"
#include "StateSystem.h"
#include "SpawnSystem.h"
#include "PathFinding.h"
#include "MenuSystem.h"
#include "BackgroundSystem.h"
#include "CleanupSystem.h"
#include "RenderableSystem.h"

class Scene
{
	entt::registry m_registry;
	std::vector<System*> m_systems;
public:
	Scene();
	~Scene();

	entt::registry* GetRegistry();

	// Entity
	int CreateEntity();
	int GetEntityCount();
	bool IsEntity(int entity);
	void RemoveEntity(int entity);

	// Lua
	static void lua_openscene(lua_State* L, Scene* scene);

	// Components
	template <typename ... Args>
	bool HasComponents(int entity);

	template <typename T>
	T& GetComponent(int entity);

	template <typename T>
	void SetComponent(int entity, const T&);

	template <typename T, typename ... Args>
	void SetComponent(int entity, Args ... args);

	template <typename T>
	void RemoveComponent(int entity);

	// Systems
	template<typename T, typename ... Args>
	void CreateSystem(Args ... args);
	void UpdateSystems(GameScreen& currentState, float delta);
	void DrawSystems(GameScreen currentState);

	// Behaviour
	int RefAndPushBehaviour(lua_State* L, int entity, const char* path);

private:
	// Lua
	static Scene* lua_GetSceneUpValue(lua_State* L);

	static int lua_GetEntityCount(lua_State* L);
	static int lua_CreateEntity(lua_State* L);
	static int lua_IsEntity(lua_State* L);
	static int lua_RemoveEntity(lua_State* L);

	static int lua_HasComponent(lua_State* L);
	static int lua_GetComponent(lua_State* L);
	static int lua_SetComponent(lua_State* L);
	static int lua_RemoveComponent(lua_State* L);

	static int lua_DrawText(lua_State* L);
	static int lua_DrawRectangle(lua_State* L);
	static int lua_DrawOutline(lua_State* L);
	static int lua_DamageEnemy(lua_State* L);

	static int lua_GetEnemies(lua_State* L);
};

// Components
template <typename ... Args>
bool Scene::HasComponents(int entity) {
	return m_registry.all_of<Args ...>((entt::entity)entity);
}

template <typename T>
T& Scene::GetComponent(int entity) {
	return m_registry.get<T>((entt::entity)entity);
}

template <typename T>
void Scene::SetComponent(int entity, const T& component) {
	m_registry.emplace_or_replace<T>((entt::entity)entity, component);
}

template <typename T, typename ... Args>
void Scene::SetComponent(int entity, Args ... args) {
	m_registry.emplace_or_replace<T>((entt::entity)entity, args ...);
}

template <typename T>
void Scene::RemoveComponent(int entity) {
	m_registry.remove<T>((entt::entity)entity);
}

// Systems
template <typename T, typename ... Args>
void Scene::CreateSystem(Args ... args) {
	m_systems.emplace_back(new T(args ...));
}

#endif
