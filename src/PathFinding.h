#ifndef PATHFINDING_H
#define PATHFINDING_H

#include <list>
#include <algorithm>
#include <fstream>
#include "..\raylibrary\raymath.h"
#include "System.h"
#include "Structs.h"
#include "Grid.h"

class PathFinding : public System {
private:
	const int STRAIGHT_COST = 10;
	const int DIAGONAL_COST = 14;

	Grid grid;
	lua_State* L;
	std::list<PathNode> openList;
	std::list<PathNode> closedList;
	std::vector<PathNode> finalPath;
	std::vector<Vector3> positionList;
	Vector2 startPoint;
	Vector2 endPoint;
	std::string buttonChoice;
public:
	PathFinding(entt::registry* registry, lua_State* L) : L(L), grid(registry, L) {
		startPoint = {0,0};
		endPoint = { 9,9 };
		buttonChoice = "startPoint";
		LoadLevel(*registry, "default.txt");
	}

	bool OnUpdate(entt::registry& registry, GameScreen& currentState, float delta) final {
		
		if (currentState == EDITOR && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
			// find which button was pressed, if any
			auto view = registry.view<Button, SpawnButton>(entt::exclude_t<StateButton>());
			view.each([&](Button& button, SpawnButton& spawn) {
				if ((GameScreen)button.state == currentState && CheckCollisionPointRec(GetMousePosition(), button.rec)) 
					buttonChoice = spawn.objectName;
			});
			
			int tempId = grid.GetValue(GetMousePosition());
			if (tempId > 0) 
			{			
				auto view2 = registry.view<PathNode, Behaviour>();
				view2.each([&](PathNode& node, Behaviour& script) {
					if(tempId == grid.GetValue(node.x, node.y))
					{
						lua_rawgeti(L, LUA_REGISTRYINDEX, script.LuaTableRef);
						if (buttonChoice == "startPoint") {
							startPoint = grid.WorldToGrid(GetMousePosition());
							
							lua_getfield(L, -1, "StartTile");
							lua_pushvalue(L, -2);
							if (lua_pcall(L, 1, 0, 0) != LUA_OK)
								DumpError(L);
							lua_pop(L, 1);
						}
						else if (buttonChoice == "endPoint") {
							endPoint = grid.WorldToGrid(GetMousePosition());
	
							lua_getfield(L, -1, "EndTile");
							lua_pushvalue(L, -2);
							if (lua_pcall(L, 1, 0, 0) != LUA_OK)
								DumpError(L);
							lua_pop(L, 1);
						}
						else if (buttonChoice == "blocking") {
							if (node.blocking == 0) {		
								lua_getfield(L, -1, "BlockTile");
								lua_pushvalue(L, -2);
								if (lua_pcall(L, 1, 0, 0) != LUA_OK)
									DumpError(L);
								lua_pop(L, 1);
							}
							else {
								lua_getfield(L, -1, "UnblockTile");
								lua_pushvalue(L, -2);
								if (lua_pcall(L, 1, 0, 0) != LUA_OK)
									DumpError(L);
								lua_pop(L, 1);
							}
						}
					}
				});
			}
			
			if (buttonChoice == "build") {
				ResetFinalPath(registry);
				finalPath = FindPath(registry, startPoint.x, startPoint.y, endPoint.x, endPoint.y);
				BuildFinalPath(registry);
				buttonChoice = "startPoint";
			}
			else if (buttonChoice == "clear") {
				ResetFinalPath(registry);
				positionList.clear();
				buttonChoice = "startPoint";
				printf("Cleared path\n");
			}
			else if (buttonChoice == "save") {
				SaveLevel("level one.txt");
				buttonChoice = "startPoint";
			}
			else if (buttonChoice == "load") {
				LoadLevel(registry, "level one.txt");
				buttonChoice = "startPoint";
				
			}
			
			
						
		}
		else if (currentState == GAMEPLAY) {
			if (positionList.size() > 0) {
				Vector3 targetPos = Vector3();
				Vector3 moveDir = Vector3();
				auto view = registry.view<Enemy, Position, Behaviour>();
				view.each([&](Enemy& enemy, Position& pos, Behaviour& script) {
					if (enemy.pathIndex < positionList.size()) {
						targetPos = positionList[enemy.pathIndex];					

						lua_rawgeti(L, LUA_REGISTRYINDEX, script.LuaTableRef);
						lua_getfield(L, -1, "CalcMovement");
						lua_pushvalue(L, -2);
						lua_pushvector(L, targetPos);
	
						if (lua_pcall(L, 2, 0, 0) != LUA_OK)
							DumpError(L);

						lua_pop(L, lua_gettop(L));
						if (Vector3Distance(pos.position, targetPos) < 1)
							enemy.pathIndex = enemy.pathIndex + 1;
				
					}
					else {
						lua_rawgeti(L, LUA_REGISTRYINDEX, script.LuaTableRef);
						lua_getfield(L, -1, "DamageBase");
						lua_pushvalue(L, -2);
						if (lua_pcall(L, 1, 0, 0) != LUA_OK)
							DumpError(L);
						lua_pop(L, 1);
					}
					});
			}
		}		
		return false;
	}

	void Draw(entt::registry& registry, GameScreen currentState) final {
		if (currentState == EDITOR) {
			grid.Draw(registry);
		}
	}

	void OnEnd(entt::registry& registry) final {  
		openList.clear();
		closedList.clear();
		finalPath.clear();
		positionList.clear();
	}

	std::vector<PathNode> FindPath(entt::registry& registry, int startX, int startZ, int endX, int endZ) {
		PathNode startNode = GetNode(registry, startX, startZ);
		PathNode endNode = GetNode(registry, endX, endZ);

		startNode.gCost = 0;
		startNode.hCost = CalculateDistanceCost(startNode, endNode);
		CalculateFCost(startNode);

		openList = std::list<PathNode>{ startNode };
		closedList.clear();

		std::list<PathNode> neighbourList = std::list<PathNode>();
		int tentativeGCost = 0;

		while (openList.size() > 0) {
			PathNode currentNode = GetLowestFCostNode(openList);
			
			if (currentNode == endNode) {
				// Reached final node
				closedList.push_back(currentNode);
				return CalculatePath(registry, currentNode, closedList);
			}

			openList.remove(currentNode);
			closedList.push_back(currentNode);
			
			neighbourList = GetNeighbourList(registry, currentNode);
			for (auto it : neighbourList) {
				if (!Contains(it, closedList)) {
					if (it.blocking == 0) {
						tentativeGCost = currentNode.gCost + CalculateDistanceCost(currentNode, it);
						if (tentativeGCost < it.gCost) {
							it.prevEnt = grid.GetValue(currentNode.x, currentNode.y);
							it.gCost = tentativeGCost;
							it.hCost = CalculateDistanceCost(it, endNode);
							CalculateFCost(it);

							if (!Contains(it, openList))
								openList.push_back(it);
						}
					}
					else
						closedList.push_back(it);
					
				}
			}
		}

		// Out of nodes on openList
		return std::vector<PathNode>();
	}

private:
	void SaveLevel(std::string levelName) {
		if (finalPath.size() > 0) {
			std::ofstream pathFile;
			pathFile.open(levelName, std::ofstream::out | std::ofstream::trunc);
			if (pathFile.is_open()) {
				for (int i = 0; i < finalPath.size(); i++) {
					std::string tile = std::to_string(finalPath[i].x) + " " + std::to_string(finalPath[i].y) + " " + std::to_string(finalPath[i].blocking);
					pathFile << tile << "\n";
				}
				pathFile.close();
			}
			printf("Saved Level\n");
		}
		else
			printf("Could not save level\n");
		
	}

	void LoadLevel(entt::registry& registry, std::string levelName) {
		ResetFinalPath(registry);

		std::ifstream infile(levelName);
		if (infile.is_open()) {
			int x, y, blocking;
			while (infile >> x >> y >> blocking) {
				PathNode node = PathNode();
				node.x = x;
				node.y = y;
				node.blocking = blocking;
				finalPath.push_back(node);
			}
			infile.close();
		}

		BuildFinalPath(registry);

		if (finalPath.size() > 0) 			
			printf("Loaded level\n");	
		else
			printf("Could not load level\n");
	}

	void FillPositionList(entt::registry& registry) {
		this->positionList.clear();
		int id = 0;
		for (int i = 0; i < finalPath.size(); i++) {
			id = grid.GetValue(finalPath[i].x, finalPath[i].y);
			positionList.push_back(registry.get<GroundTile>((entt::entity)id).center);
		}
	}

	void ResetFinalPath(entt::registry& registry) {
		if (finalPath.size() > 0) {
			auto view = registry.view<PathNode, ColorData>();
			view.each([&](PathNode& node, ColorData& color) {
				node.gCost = 1000;
				CalculateFCost(node);
				node.prevEnt = 0;
				node.blocking = 1;
				color.color = BLACK;
				int id = grid.GetValue(node.x, node.y);
				registry.get<GroundTile>((entt::entity)id).color = DARKGREEN;
			});

			finalPath.clear();
		}
	}

	void BuildFinalPath(entt::registry& registry) {
		if (finalPath.size() > 0) {
			auto view = registry.view<PathNode, Behaviour>();
			for (int i = 0; i < finalPath.size(); i++) {
				view.each([&](PathNode& node, Behaviour& script) {
					if (node.x == finalPath[i].x && node.y == finalPath[i].y) {
				
						lua_rawgeti(L, LUA_REGISTRYINDEX, script.LuaTableRef);
						if (i == 0) {
							lua_getfield(L, -1, "StartTile");
							lua_pushvalue(L, -2);
							if (lua_pcall(L, 1, 0, 0) != LUA_OK)
								DumpError(L);
							lua_pop(L, 1);
						}
						else if (i == finalPath.size() - 1) {
							lua_getfield(L, -1, "EndTile");
							lua_pushvalue(L, -2);
							if (lua_pcall(L, 1, 0, 0) != LUA_OK)
								DumpError(L);
							lua_pop(L, 1);
						}
						else {							
							lua_getfield(L, -1, "BuildTile");
							lua_pushvalue(L, -2);
							if (lua_pcall(L, 1, 0, 0) != LUA_OK)
								DumpError(L);
							lua_pop(L, 1);
						}
						if(lua_gettop(L) > 0)
							lua_pop(L, lua_gettop(L));
					}
					});
			}
			FillPositionList(registry);
	
			lua_pushvector(L, positionList.front());
			lua_setglobal(L, "g_startTile");	

			printf("Built Path\n");
		}
		else
			printf("Could not build path\n");		
	}

	bool Contains(const PathNode& currentNode, const std::list<PathNode>& list) {
		for (auto const it : list) {
			if (it == currentNode)
				return true;
		}
		return false;
	}

	PathNode GetNode(entt::registry& registry, int x, int y) { 
		return registry.get<PathNode>((entt::entity)grid.GetValue(x, y));
	}

	std::list<PathNode> GetNeighbourList(entt::registry& registry, PathNode currentNode) {
		std::list<PathNode> neighbourList = std::list<PathNode>();

		if (currentNode.x - 1 >= 0) {
			// Left Up
			if (currentNode.y - 1 >= 0)
				neighbourList.push_back(GetNode(registry, currentNode.x - 1, currentNode.y - 1));
			// Left
			neighbourList.push_back(GetNode(registry, currentNode.x - 1, currentNode.y));
			// Left Down
			if (currentNode.y + 1 < grid.GetHeight())
				neighbourList.push_back(GetNode(registry, currentNode.x - 1, currentNode.y + 1));
		}
		// Down
		if (currentNode.y + 1 < grid.GetHeight()) 
			neighbourList.push_back(GetNode(registry, currentNode.x, currentNode.y + 1));
		if (currentNode.x + 1 < grid.GetWidth()) {
			// Right Down
			if (currentNode.y + 1 < grid.GetHeight())
				neighbourList.push_back(GetNode(registry, currentNode.x + 1, currentNode.y + 1));
			// Right
			neighbourList.push_back(GetNode(registry, currentNode.x + 1, currentNode.y));
			// Right Up
			if (currentNode.y - 1 >= 0)
				neighbourList.push_back(GetNode(registry, currentNode.x + 1, currentNode.y - 1));
		}
		// Up
		if (currentNode.y - 1 >= 0) 
			neighbourList.push_back(GetNode(registry, currentNode.x, currentNode.y - 1));
		
		return neighbourList;
	}

	std::vector<PathNode> CalculatePath(entt::registry& registry, PathNode endNode, std::list<PathNode> closedList) {
		std::list<PathNode> path = std::list<PathNode>();
		std::vector<PathNode> road = std::vector<PathNode>(); 

		PathNode currentNode = endNode;

		while (Contains(currentNode, closedList)) {
			for (auto const it : closedList) {			
				if (it == currentNode) {
					path.push_back(it);
					if (it.prevEnt != 0) {
						currentNode = registry.get<PathNode>((entt::entity)it.prevEnt);
					}
					else {
						path.reverse();
						for (auto const jt : path)
							road.push_back(jt);
						return road;
					}			
				}				
			}
		}

		return road;
	}

	void CalculateFCost(PathNode& node) { node.fCost = node.gCost + node.hCost; }

	int CalculateDistanceCost(PathNode a, PathNode b) {
		int xDistance = abs(a.x - b.x);
		int yDistance = abs(a.y - b.y);
		int remaining = abs(xDistance - yDistance);
		return DIAGONAL_COST * std::min(xDistance, yDistance) + STRAIGHT_COST * remaining;
	}

	PathNode GetLowestFCostNode(std::list<PathNode> nodeList) {
		PathNode lowestNode = PathNode();
		lowestNode.fCost = 1000;
		for (auto const it : nodeList) {
			if (it.fCost < lowestNode.fCost) 
				lowestNode = it;
		}
		return lowestNode;		
	}
};
#endif