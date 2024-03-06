#ifndef STATESYSTEM_H
#define STATESYSTEM_H
#include "System.h"
#include "Structs.h"

class StateSystem : public System
{
	lua_State* L;
	int gameResult;
	int framesCounter;
public:

	StateSystem(lua_State* L) : L(L), gameResult(0), framesCounter(0) {}

	bool OnUpdate(entt::registry& registry, GameScreen& currentState, float delta) final {

		if (currentState == GAMEPLAY) {
	
			lua_getglobal(L, "g_gameResult");
			gameResult = lua_tonumber(L, -1);
			lua_pop(L, lua_gettop(L));

			if (gameResult > 0)
				framesCounter++;		
		}

		if (framesCounter >= 120)
		{
			lua_getglobal(L, "Restart");
			lua_pushvalue(L, -2);
			if (lua_pcall(L, 1, 1, 0) != LUA_OK)
				DumpError(L);
			framesCounter = lua_tointeger(L, -1);
			DumpStack(L);
			lua_pop(L, 1);
			DumpStack(L);
			currentState = ENDING;

		}
		
		// Switch state to corresponding button pressed
		auto view = registry.view<Button, StateButton>(entt::exclude_t<SpawnButton>());
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			switch (currentState)
			{
				case TITLE:
				{
					view.each([&](Button& button, StateButton& sButton) {
						if (button.state == 0 && sButton.destination > -1 && CheckCollisionPointRec(GetMousePosition(), button.rec))
							currentState = (GameScreen)sButton.destination;
						});

				} break;
				case GAMEPLAY:
				{
					view.each([&](Button& button, StateButton& sButton) {
						if (button.state == 1 && sButton.destination > -1 && CheckCollisionPointRec(GetMousePosition(), button.rec))
							currentState = (GameScreen)sButton.destination;
						});

				} break;
				case ENDING:
				{
					view.each([&](Button& button, StateButton& sButton) {
						if (button.state == 2 && sButton.destination > -1 && CheckCollisionPointRec(GetMousePosition(), button.rec))
							currentState = (GameScreen)sButton.destination;
						});
				} break;
				case EDITOR:
				{
					view.each([&](Button& button, StateButton& sButton) {
						if (button.state == 3 && sButton.destination > -1 && CheckCollisionPointRec(GetMousePosition(), button.rec))
							currentState = (GameScreen)sButton.destination;
						});

				} break;
				default: break;
			}
		}

		BeginDrawing();
		ClearBackground(RAYWHITE);
		return false;
	}

	void Draw(entt::registry& registry, GameScreen currentState) final {}

	void OnEnd(entt::registry& registry) final {}

};
#endif