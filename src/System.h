#ifndef SYSTEM_H
#define SYSTEM_H
#include <entt.hpp>
#include "LuaVector3.h"
#include "LuaVector4.h"

typedef enum GameScreen { TITLE = 0, GAMEPLAY = 1, ENDING = 2, EDITOR = 3 } GameScreen;

class System
{
public:
	virtual bool OnUpdate(entt::registry& registry, GameScreen &currentState, float delta) = 0; //run every tick
	virtual void Draw(entt::registry& registry, GameScreen currentState) = 0;
	virtual void OnEnd(entt::registry& registry) = 0;

};
#endif