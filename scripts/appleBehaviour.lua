local vector = require("scripts/vector")

local position 
local moveDir = vector.new(0,0,0)
local targetInt = 0

appleBehaviour = {}

function appleBehaviour:OnCreate()
	print("Apple created!")
	self.HP = 1
	print("ID: ", self.ID)
	print("HP: ", self.HP)
end

function appleBehaviour:OnUpdate(delta)

	if self.HP <= 0 then
		--print("Apple died! ID: ", self.ID)
		scene.RemoveComponent(self.ID, "enemy")
		scene.RemoveComponent(self.ID, "position")
		scene.RemoveComponent(self.ID, "behaviour")
		scene.RemoveComponent(self.ID, "renderable")
		scene.RemoveEntity(self.ID)
		g_killedEnemies = g_killedEnemies + 1
		g_money = g_money + 1
	end
end

function appleBehaviour:TakeDamage()
		--print("Apple took damage! ID: ", self.ID)
		self.HP = self.HP - 1
end

function appleBehaviour:DamageBase()
	if g_baseHP > 0 then
		g_baseHP = g_baseHP - 1
	end
	scene.RemoveComponent(self.ID, "enemy")
	scene.RemoveComponent(self.ID, "position")
	scene.RemoveComponent(self.ID, "behaviour")
	scene.RemoveComponent(self.ID, "renderable")
	scene.RemoveEntity(self.ID)
	g_killedEnemies = g_killedEnemies + 1
end

function appleBehaviour:CalcMovement(targetPos)
	position = scene.GetComponent(self.ID, "position")

	moveDir.x = targetPos.x - position.x 
	moveDir.y = targetPos.y - position.y 
	moveDir.z = targetPos.z - position.z 
	moveDir = vector.normalize(moveDir)

	position.x = position.x + (moveDir.x * 0.3)
	position.y = position.y + (moveDir.y * 0.3)
	position.z = position.z + (moveDir.z * 0.3)

	scene.SetComponent(self.ID, "position", position)
end

--returns distance between this enemy and a tower and this enemys ID
function appleBehaviour:GetDist(towerPos)
	position = scene.GetComponent(self.ID, "position")
	local closest = vector.distance(towerPos, position)
	return closest, self.ID
end

function appleBehaviour:CompareID(otherID)
	if otherID == self.ID then
		return true
	else
		return false
	end
end

return appleBehaviour