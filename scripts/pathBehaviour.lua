local vector = require("scripts/vector")
local vector4 = require("scripts/vector4")

local brown = vector4.new(127, 106, 79, 255)
local darkbrown = vector4.new(76, 63, 47, 255)
local red = vector4.new(230, 41, 55, 255)
local black = vector4.new(0, 0, 0, 255)
local blue = vector4.new(0, 121, 241, 255)
local darkgreen = vector4.new(0, 117, 44, 255)
local color = vector4.new(0,0,0,0)

local blocking = 0

local pathBehaviour = {}

function pathBehaviour:OnCreate()
	--print("Path created!", self.ID)
end

function pathBehaviour:OnUpdate(delta)
	
end

function SetBlocking(ID, blocking)
	x, y, gCost, hCost, fCost, prevEnt = scene.GetComponent(ID, "pathnode")
	scene.SetComponent(ID, "pathnode", x, y, gCost, hCost, fCost, prevEnt, blocking)
end

function SetGroundColor(ID, color)
	topL, botL, botR, topR, center, cellSize = scene.GetComponent(ID, "groundtile")
	scene.SetComponent(ID, "groundtile", topL, botL, botR, topR, center, cellSize, color)
	print("set color")
end

function pathBehaviour:BuildTile()
	SetBlocking(self.ID, 0)
	SetGroundColor(self.ID, darkbrown)
	scene.SetComponent(self.ID, "colordata", darkbrown)
end

function pathBehaviour:StartTile()
	SetBlocking(self.ID, 0)
	SetGroundColor(self.ID, blue)
	scene.SetComponent(self.ID, "colordata", blue)
end

function pathBehaviour:EndTile()
	SetBlocking(self.ID, 0)
	SetGroundColor(self.ID, red)
	scene.SetComponent(self.ID, "colordata", red)
end

function pathBehaviour:BlockTile()
	SetBlocking(self.ID, 1)
	SetGroundColor(self.ID, darkgreen)
	scene.SetComponent(self.ID, "colordata", black)
end

function pathBehaviour:UnblockTile()
	SetBlocking(self.ID, 0)
	SetGroundColor(self.ID, brown)
	scene.SetComponent(self.ID, "colordata", brown)
end


return pathBehaviour