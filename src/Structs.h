#ifndef STRUCTS_H
#define STRUCTS_H
#include "LuaVector3.h"
#include <cstring>

struct Renderable
{
	Model renderableModel;
	Texture2D renderableTexture;
	BoundingBox renderBBox;
	int scale;
	Color color;
	bool gotHit;
};

struct Position
{
	Vector3 position;
};

struct Tower {
	float range;
	float firingRate;
};

struct Enemy {
	int pathIndex;
	int entity;
};

struct Projectile
{
	Vector3 position;
	Vector3 startPosition;
	Vector3 transform;
	int targetID;
	float range;
};

struct Behaviour
{
	char ScriptPath[64];
	int LuaTableRef;

	// Constructor to initialize char array
	Behaviour(const char* path, int luaRef) : LuaTableRef(luaRef)
	{
		memset(ScriptPath, '\0', 64);
		strcpy_s(ScriptPath, path);
	}
};

struct CameraComponent
{
	Camera camera;
	int mode;
};

struct RayComponent
{
	Ray ray;
	RayCollision collider;
	std::string strObjectName;
	Color cursorColor;
};

struct GroundTile
{
	Vector3 topLeft;
	Vector3 bottomLeft;
	Vector3 bottomRight;
	Vector3 topRight;
	Vector3 center;
	Color color;
	float cellSize;
	bool gotHit;
	int towerID;
	int entity;
};

struct PathNode
{
	int x;
	int y;
	int gCost;
	int hCost;
	int fCost;
	int prevEnt;
	int blocking; // 0 = false, 1 = true

	bool operator==(const PathNode& other)  const
	{
		return (this->x == other.x && this->y == other.y ); 
	}

	void operator=(const PathNode& other) {
		this->x = other.x;
		this->y = other.y;
		this->gCost = other.gCost;
		this->hCost = other.hCost;
		this->fCost = other.fCost;
		this->prevEnt = other.prevEnt;
		this->blocking = other.blocking;
	}

};

struct Button 
{
	// state this button belongs to
	int state; 
	// x, y, width and height
	Rectangle rec;
};

struct StateButton
{
	// state to switch to if clicked
	int destination;
};

struct SpawnButton
{
	std::string objectName;
};

struct Text
{
	std::string text;
	// position and fontSize
	Vector3 textData;
};

struct ColorData // change so everything with color uses this one
{
	Color color;
};

struct BaseBar
{
	// posX, PosY, Width, Height
	Vector4 baseBarData;
};

#endif