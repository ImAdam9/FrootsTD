local vector = require("scripts/vector")
local vector4 = require("scripts/vector4")

local white = vector4.new(255, 255, 255, 255)

createTower = {
	LongRange = function(tilePos)
		if g_money >= 5 then
			g_money = g_money - 5
			local towerLong = scene.CreateEntity()
			local modelPath = "resources/models/obj/turret.obj"
			local texturePath = "resources/models/obj/turret_diffuse.png"

			-- ID, C++ component, Range, FiringRate --
			scene.SetComponent(towerLong, "tower", 24, 1.0)
			scene.SetComponent(towerLong, "position", tilePos)
			scene.SetComponent(towerLong, "behaviour", "scripts/towerBehaviour.lua")
			-- ID, C++ component, model path, texture path --
			scene.SetComponent(towerLong, "renderable", modelPath, texturePath, 1, white, tilePos)
			return towerLong
		else
			return -1
		end
	end,

	ShortRange = function(tilePos)
		if g_money >= 10 then
			g_money = g_money - 10
			local towerShort = scene.CreateEntity()
			local modelPath = "resources/models/obj/well.obj"
			local texturePath = "resources/models/obj/well_diffuse.png"

			-- ID, C++ component, Range, FiringRate --
			scene.SetComponent(towerShort, "tower", 12, 0.2)
			scene.SetComponent(towerShort, "position", tilePos)
			scene.SetComponent(towerShort, "behaviour", "scripts/towerBehaviour.lua")
			-- ID, C++ component, model path, texture path --
			scene.SetComponent(towerShort, "renderable", modelPath, texturePath, 1, white, tilePos)
			return towerShort
		else
			return -1
		end
	end,

	Remove = function(tileID)

		if scene.IsEntity(tileID) then
			g_money = g_money + 5
			scene.RemoveComponent(tileID, "tower")
			scene.RemoveComponent(tileID, "position")
			scene.RemoveComponent(tileID, "behaviour")
			scene.RemoveComponent(tileID, "renderable")
			scene.RemoveEntity(tileID)
			print("\nRemoved Tower", tileID)
		end
	end,
}