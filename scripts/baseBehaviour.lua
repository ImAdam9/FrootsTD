local vector = require("scripts/vector")
local vector4 = require("scripts/vector4")

local data = vector4.new(0,0,0,0)
local color = vector4.new(0,0,0,0)

baseBehaviour = {}

function baseBehaviour:OnCreate()
	print("Base created! ID:", self.ID)
end

function baseBehaviour:OnUpdate(delta)
	
	if g_baseHP <= 0 then
		g_money = 0
		g_gameResult = 1
		print("Base lost! ")
	end
end

function baseBehaviour:Draw()
	data = scene.GetComponent(self.ID, "basebar")
	color = scene.GetComponent(self.ID, "colordata")
	scene.DrawRectangle(data.x, data.y, data.z * g_baseHP, data.w, color)	
end

return baseBehaviour