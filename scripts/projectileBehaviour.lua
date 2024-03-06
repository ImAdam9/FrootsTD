local vector = require("scripts/vector")

local moveDir = vector.new(0,0,0)

projectileBehaviour = {}

function projectileBehaviour:OnCreate()
	print("\nProjectile created!", self.ID)
end

local function calcMovement(bulletPos, targetPos)

	moveDir.x = targetPos.x - bulletPos.x
	moveDir.y = targetPos.y - bulletPos.y
	moveDir.z = targetPos.z - bulletPos.z

	moveDir = vector.normalize(moveDir)

	bulletPos.x = bulletPos.x + (moveDir.x * 1)
	bulletPos.y = bulletPos.y + (moveDir.y * 1)
	bulletPos.z = bulletPos.z + (moveDir.z * 1)
	
	return bulletPos
end

function projectileBehaviour:OnUpdate(delta)
	-- Get the current position, start position, transformation, target ID and projectile range
	bulletPos, bulletStart, bulletTrans, bulletTargetID, range = scene.GetComponent(self.ID, "projectile")

	if bulletTargetID > 0 then -- if projectile was given a target
		if scene.IsEntity(bulletTargetID) then -- if target exist
			
			-- Get the targets position
			local targetPos = scene.GetComponent(bulletTargetID, "position")
		
			-- Move projectile towards target
			bulletPos = calcMovement(bulletPos, targetPos)

			if vector.distance(bulletPos, targetPos) < 1 then -- if close enough to hit target, damage target and remove projectile
				scene.DamageEnemy(bulletTargetID);
				scene.RemoveComponent(self.ID, "projectile")
				scene.RemoveComponent(self.ID, "behaviour")
				scene.RemoveEntity(self.ID)
			
			elseif vector.distance(bulletStart, bulletPos) > range  then -- if projectile out moving out of range remove it
				scene.RemoveComponent(self.ID, "projectile")
				scene.RemoveComponent(self.ID, "behaviour")
				scene.RemoveEntity(self.ID)
			else -- Update position of projectile
				scene.SetComponent(self.ID, "projectile", bulletPos, bulletStart, bulletTrans, bulletTargetID, range)
			end
		else -- remove projectile if target does not exist
			scene.RemoveComponent(self.ID, "projectile")
			scene.RemoveComponent(self.ID, "behaviour")
			scene.RemoveEntity(self.ID)
		end
	end
end

return projectileBehaviour