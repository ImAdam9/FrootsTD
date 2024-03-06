local vector = require("scripts/vector")
local vector4 = require("scripts/vector4")

local red = vector4.new(230, 41, 55, 255)

createBase = {
	HealthBar = function()
		g_baseHP = 10
		local baseBar = scene.CreateEntity()
		local baseBarData = vector4.new(g_screenWidth/2, 0, 10, 50)
		scene.SetComponent(baseBar, "basebar", baseBarData)
		scene.SetComponent(baseBar, "behaviour", "scripts/baseBehaviour.lua")
		scene.SetComponent(baseBar, "colordata", red)
	end,
}