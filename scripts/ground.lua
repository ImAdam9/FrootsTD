local vector = require("scripts/vector")
local vector4 = require("scripts/vector4")

local darkGreen = vector4.new(0, 117, 44, 255 )
local black = vector4.new( 0, 0, 0, 255 )

local cellSize = 10
local center = vector.new(0,0,0)
local tileAmount = vector.new(10, 0, 10)
local origo = vector.new(-tileAmount.x*cellSize/2, 0, -tileAmount.z*cellSize/2)

local function left(center)
	return center.x - (cellSize/2)
end

local function right(center)
	return center.x + (cellSize/2)
end

local function up(center)
	return center.z + (cellSize/2)
end

local function down(center)
	return center.z - (cellSize/2)
end

for i = 0, tileAmount.x-1 do
	for j = 0, tileAmount.z-1 do
		center.x = (cellSize/2) + cellSize * i + origo.x
		center.z = (cellSize/2) + cellSize* j + origo.z
		local topLeft = vector.new(left(center), 0, up(center))
		local bottomLeft = vector.new(left(center), 0, down(center))
		local bottomRight = vector.new(right(center), 0, down(center))
		local topRight = vector.new(right(center), 0, up(center))

		local entity = scene.CreateEntity()
		scene.SetComponent(entity, "groundtile", topLeft, bottomLeft, bottomRight, topRight, center, cellSize, darkGreen)
		scene.SetComponent(entity, "behaviour", "scripts/pathBehaviour.lua")
		scene.SetComponent(entity, "pathnode", i, j, 1000, 0, 1000, 0, 1)
		scene.SetComponent(entity, "colordata", black)
	end
end

function GroundValues()
	return tileAmount, 40
end