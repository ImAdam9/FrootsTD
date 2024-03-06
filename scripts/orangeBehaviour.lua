local vector = require("scripts/vector")
local vector4 = require("scripts/vector4")

local position 
local moveDir = vector.new(0,0,0)

local orangeBehaviour = {}

function orangeBehaviour:OnCreate()
	print("Orange created!")
	self.HP = 2
	print("ID: ", self.ID)
	print("HP: ", self.HP)
end

function orangeBehaviour:OnUpdate(delta)
	if self.HP <= 0 then
		--print("Orange died! ID: ", self.ID)
		g_money = g_money + 1
		local position = scene.GetComponent(self.ID, "position")
		scene.RemoveComponent(self.ID, "behaviour");
		scene.RemoveComponent(self.ID, "renderable")
		local modelPath = "resources/models/obj/LowPolyApple.obj"
		local texturePath = "resources/models/obj/LowPolyApple.png"
		scene.SetComponent(self.ID, "behaviour", "scripts/appleBehaviour.lua")
		scene.SetComponent(self.ID, "renderable", modelPath, texturePath, 1, vector4.new(255, 255, 255, 255), position)
	end
end

function orangeBehaviour:TakeDamage()
		--print("Orange took damage! ID: ", self.ID)
		self.HP = self.HP - 1
end

function orangeBehaviour:DamageBase()
	--print("Orange died! ID: ", self.ID)
	scene.RemoveComponent(self.ID, "enemy")
	scene.RemoveComponent(self.ID, "position")
	scene.RemoveComponent(self.ID, "behaviour")
	scene.RemoveComponent(self.ID, "renderable")
	scene.RemoveEntity(self.ID)
	if g_baseHP > 0 then
		g_baseHP = g_baseHP - 1
	end
	g_killedEnemies = g_killedEnemies + 1
end

function orangeBehaviour:CalcMovement(targetPos)
	position = scene.GetComponent(self.ID, "position")

	moveDir.x = targetPos.x - position.x 
	moveDir.y = targetPos.y - position.y 
	moveDir.z = targetPos.z - position.z 
	moveDir = vector.normalize(moveDir)

	position.x = position.x + (moveDir.x * 0.2)
	position.y = position.y + (moveDir.y * 0.2)
	position.z = position.z + (moveDir.z * 0.2)

	scene.SetComponent(self.ID, "position", position)
end

function orangeBehaviour:GetDist(towerPos)
	position = scene.GetComponent(self.ID, "position")
	local closest = vector.distance(towerPos, position)
	return closest, self.ID
end

function orangeBehaviour:CompareID(otherID)
	if otherID == self.ID then
		return true
	else
		return false
	end
end

return orangeBehaviour