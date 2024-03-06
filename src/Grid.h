#ifndef GRID_H
#define GRID_H

#include <string>
#include <cmath>
#include "Structs.h"

class Grid {
private:

	Vector3 tileAmount;
	int cellSize;
	int** gridArray;

public:

	Grid(entt::registry* registry, lua_State* L) {
		
		lua_getglobal(L, "GroundValues");
		lua_pushvalue(L, -2);
		if (lua_pcall(L, 1, 2, 0) != LUA_OK)
			DumpError(L);
		this->cellSize = lua_tointeger(L, -1);
		lua_pop(L, 1);
		this->tileAmount = lua_tovector(L, -1);
		lua_pop(L, 1);

		printf("cellSize %i \n", cellSize);
		printf("tileAmount %f %f %f\n", tileAmount.x, tileAmount.y, tileAmount.z);

		this->gridArray = new int* [tileAmount.x];
		for (int i = 0; i < tileAmount.x; i++)
			this->gridArray[i] = new int[tileAmount.z];

		for (int i = 0; i < tileAmount.x; i++) {
			for (int j = 0; j < tileAmount.z; j++) 
				this->gridArray[i][j] = 0;
		}

		// put entity id into gridArray
		int i = 0, j = 0;
		auto view = registry->view<GroundTile, PathNode>();
		view.each([&](GroundTile& tile, PathNode& node) {
			if (i < tileAmount.x && j < tileAmount.z) {
				node.x = i;
				node.y = j;
				this->gridArray[i][j++] = tile.entity;
			}
			if (j > 9) {
				i++;
				j = 0;
			}
		});
	}

	~Grid() {
		for(int i = 0; i < tileAmount.x; i++)
			delete[] this->gridArray[i];
		delete this->gridArray;
	}

	void SetValue(int x, int z, int value) {
		if (x >= 0 && z >= 0 && x < tileAmount.x && z < tileAmount.z)
			gridArray[x][z] = value;
	}

	void SetValue(Vector2 worldPos, int value) {
		Vector2 gridPos = WorldToGrid(worldPos);
		SetValue(gridPos.x, gridPos.y, value);
	}

	int GetValue(int x, int z) {
		if (x >= 0 && z >= 0 && x < tileAmount.x && z < tileAmount.z)
			return gridArray[x][z];
		else
			return 0;
	}

	int GetValue(Vector2 worldPos) {
		worldPos = WorldToGrid(worldPos);
		return GetValue(worldPos.x, worldPos.y);
	}

	int GetWidth() { return this->tileAmount.x; }
	int GetHeight() { return this->tileAmount.z; }

	void Print() {
		for (int i = 0; i < tileAmount.x; i++) {
			for (int j = 0; j < tileAmount.z; j++) {
				printf("%i ", this->gridArray[i][j]);
			}
			printf("\n");
		}
	}

	void Draw(entt::registry& registry) {
		Vector3 drawPos = { 0.f, 0.f, 0.f };
		auto view = registry.view<PathNode, ColorData>();
		view.each([&](PathNode& node, ColorData& color) {
			drawPos = GridToWorld(node.x, node.y);
			std::string text = std::to_string(node.x) + "," + std::to_string(node.y);

			DrawText(text.c_str(), drawPos.x, drawPos.y, 20, color.color);
		
		});
	}

	Vector3 GridToWorld(int xPos, int yPos) {
		Vector3 worldPosition = { (float)xPos * this->cellSize, (float)yPos * this->cellSize, 0.f };
		return worldPosition;
	}

	Vector2 WorldToGrid(Vector2 worldPos) {
		Vector2 gridPos;	
		gridPos.x = std::floor(worldPos.x / this->cellSize);
		gridPos.y = std::floor(worldPos.y / this->cellSize);
		return gridPos;
	}
};
#endif