local vector = require("scripts/vector")
local vector4 = require("scripts/vector4")

local white = vector4.new(255, 255, 255, 255)
local black = vector4.new( 0, 0, 0, 255 )

local spawned = 0 
local maxSpawned = 10

local waitFlag = 0 -- 0 = don't wait, 1 = wait
local waitTime = 30

function Restart()
	g_wave = 1
	g_killedEnemies = 0
	g_baseHP = 10
	g_money = 15

	spawned = 0
	maxSpawned = 5
	waitFlag = 0
	waitTime = 30

	return 0
end

createFruit = {
	Apple = function()
		local apple = scene.CreateEntity()
		local modelPath = "resources/models/obj/LowPolyApple.obj"
		local texturePath = "resources/models/obj/LowPolyApple.png"
		scene.SetComponent(apple, "enemy")
		scene.SetComponent(apple, "position", g_startTile)
		scene.SetComponent(apple, "behaviour", "scripts/appleBehaviour.lua")
		scene.SetComponent(apple, "renderable", modelPath, texturePath, 1, white, g_startTile)
	end,

	Orange = function()
		local orange = scene.CreateEntity()
		local modelPath = "resources/models/obj/LowPolyOrange.obj"
		local texturePath = "resources/models/obj/LowPolyOrange.png"
		scene.SetComponent(orange, "enemy")
		scene.SetComponent(orange, "position", g_startTile)
		scene.SetComponent(orange, "behaviour", "scripts/orangeBehaviour.lua")
		scene.SetComponent(orange, "renderable", modelPath, texturePath, 1, white, g_startTile)
	end,

	Melon = function()
		local melon = scene.CreateEntity()
		local modelPath = "resources/models/obj/LowPolyMelon.obj"
		local texturePath = "resources/models/obj/watermelon.png"
		scene.SetComponent(melon, "enemy")
		scene.SetComponent(melon, "position", g_startTile)
		scene.SetComponent(melon, "behaviour", "scripts/melonBehaviour.lua")
		scene.SetComponent(melon, "renderable", modelPath, texturePath, 1, white, g_startTile)
	end,
}

function wave1(framesCounter)

	if framesCounter >= waitTime and waitFlag == 0 then
		if spawned < maxSpawned  then
			createFruit.Apple()
			spawned = spawned + 1			
		else
			if g_killedEnemies == maxSpawned then
				waitTime = 500
				waitFlag = 1
				g_wave = 2
				print("wave one cleared")
			end
		end
		return 0
	end
	return framesCounter
end

function wave2(framesCounter)
	if framesCounter >= waitTime and waitFlag == 1 then
		spawned = 0
		maxSpawned = 11				
		g_killedEnemies = 0		
		waitFlag = 0
		waitTime = 30
		print("wave two")
		return 0
	end

	if framesCounter >= waitTime and waitFlag == 0 then
		if spawned < maxSpawned  then
			if spawned < maxSpawned/2 then
				createFruit.Apple()
			else
				createFruit.Orange()
			end
			spawned = spawned + 1			
		else
			if g_killedEnemies == maxSpawned then
				waitTime = 500
				waitFlag = 1
				g_wave = 3
				print("wave two cleared")
			end
		end
		return 0
	end

	return framesCounter	
end

function wave3(framesCounter)
	if framesCounter >= waitTime and waitFlag == 1 then
		spawned = 0
		maxSpawned = 12				
		g_killedEnemies = 0		
		waitFlag = 0
		waitTime = 30
		print("wave three")
		return 0
	end

	if framesCounter >= waitTime and waitFlag == 0 then
		if spawned < maxSpawned  then
			if spawned < maxSpawned/4 then
				createFruit.Orange()
			else
				createFruit.Melon()
			end
			spawned = spawned + 1			
		else
			if g_killedEnemies == maxSpawned then
				waitTime = 500
				waitFlag = 1
				g_wave = 0
				g_gameResult = 2
				print("wave three cleared")
			end
		end
		return 0
	end

	return framesCounter	
end

function WaveDraw(framesCounter)
	if waitFlag == 1 and g_wave > 0 then
		local time = (waitTime - framesCounter)/100
		scene.DrawText("Next wave in ", 350, 500, 50, black)
		scene.DrawText(time, 700, 500, 50, black)
	end
end