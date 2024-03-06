local vector4 = require("scripts/vector4")
local ray = 0
local collider = 0
local defaultName = "None"
local color = vector4.new( 255, 255, 255, 255 )

local entity = scene.CreateEntity()
scene.SetComponent(entity, "ray", ray, collider, defaultName, color)
scene.SetComponent(entity, "behaviour", "scripts/rayBehaviour.lua")