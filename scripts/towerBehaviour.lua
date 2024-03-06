local vector = require("scripts/vector")

local towerPos = vector.new(0,0,0)
local towerRange = 0
local firingRate = 0

local hasShot = 0

local allEnemies = {}

towerBehaviour = {}

function towerBehaviour:OnCreate()
	print("Tower created!", self.ID)
end

function towerBehaviour:OnUpdate(delta)
	-- Get range and firing rate of tower
	towerRange, firingRate = scene.GetComponent(self.ID, "tower")
	-- Get Position of tower entity
	towerPos = scene.GetComponent(self.ID, "position")
	
	-- Get a table of all existing enemies and assign the first enemy as closest
	allEnemies = scene.GetEnemies()
	local closest = allEnemies[1]

	if closest then -- If at least one enemy exist
		for k,v in ipairs( allEnemies ) do	-- Compare all enemies to find the closest one
			if vector.distance(towerPos, scene.GetComponent(v, "position")) < vector.distance(towerPos, scene.GetComponent(closest, "position")) then
				closest = v
			end
		end

		
		if delta > firingRate then -- if enough time has passed to shoot again
			if vector.distance(towerPos, scene.GetComponent(closest, "position")) < towerRange then -- if the closest enemy is within range
				if hasShot == 0 then -- if tower has not shot yet while delta > firingRate, then create projectile
						projectile = scene.CreateEntity()
						scene.SetComponent(projectile, "projectile", towerPos, towerPos, vector.new(1, 1, 1), closest, towerRange)
						scene.SetComponent(projectile, "behaviour", "scripts/projectileBehaviour.lua")
						hasShot = 1
				end
			end
		else
			if hasShot == 1 then hasShot = 0 end
		end
	end
end

return towerBehaviour