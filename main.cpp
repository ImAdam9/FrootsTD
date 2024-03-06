#include <iostream>
#include <thread>
#include <string>
#include <chrono>
#include "raylibrary\raylib.h"
#include "raylibrary\raymath.h"
#include "src\LuaConsole.h"
#include "src\Scene.h"

int main(void)
{
	// Initialize all required variables and load all required data here!
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);
	std::thread consoleThread(ConsoleThreadFunction, L);
	using Clock = std::chrono::high_resolution_clock;
	Clock::time_point lastTick = Clock::now();
	float deltaInSeconds = 0.0;

	GameScreen currentScreen = TITLE;

	luaL_dofile(L, "scripts/globals.lua");
	lua_getglobal(L, "g_screenWidth");
	int startScreenWidth = lua_tonumber(L, -1);
	lua_pop(L, 1);
	lua_getglobal(L, "g_screenHeight");
	int startScreenHeight = lua_tonumber(L, -1);
	lua_pop(L, 1);

	InitWindow(startScreenWidth, startScreenHeight, "FrootsTD");

	Scene scene;
	Scene::lua_openscene(L, &scene);

	scene.CreateSystem<CameraSystem>(L);	
	scene.CreateSystem<RaySystem>(L);
	scene.CreateSystem<BackgroundSystem>(L);
	scene.CreateSystem<RenderableSystem>(L);
	scene.CreateSystem<GroundSystem>(L);
	scene.CreateSystem<PathFinding>(scene.GetRegistry(), L);
	scene.CreateSystem<BehaviourSystem>(L);
	scene.CreateSystem<SpawnSystem>(L);
	scene.CreateSystem<StateSystem>(L);
	scene.CreateSystem<MenuSystem>(L);
	scene.CreateSystem<CleanupSystem>();
	DumpStack(L);

	SetTargetFPS(60); 
	
	while (!WindowShouldClose())
	{
		// ------------------ UPDATE ------------------ //
		Clock::time_point thisTick = Clock::now();
		Clock::duration delta = thisTick - lastTick;
		lastTick = thisTick;
		deltaInSeconds += delta.count() / 1000000000.f;

		scene.UpdateSystems(currentScreen, deltaInSeconds);
		if (deltaInSeconds >= 1.f) {
			deltaInSeconds = 0.f;
		}

		// ------------------ DRAW ------------------ //	
		scene.DrawSystems(currentScreen);
		if(currentScreen == GAMEPLAY)	
			DrawFPS(10, 10);
		
		if(lua_gettop(L) > 0)
			DumpStack(L);

		EndDrawing();
	}

	consoleThread.detach();
	lua_close(L);
	CloseWindow();
	return 0;
}