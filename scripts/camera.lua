local vector = require("scripts/vector")
local position = vector.new(0.0, 95.0, 90.0)
local target = vector.new(0.0, 0.0, 0.0)
local up = vector.new(0.0, 1.0, 0.0)
local fov = 45.0
local projection = 0 -- 0 = PERSPECTIVE, 1 = ORTHOGRAPHIC
local mode = 1 -- 0 = CUSTOM, 1 = FREE, 2 = ORBITAL, 3 = FIRST_PERSON, 4 = THIRD_PERSON  

local entity = scene.CreateEntity()
scene.SetComponent(entity, "camera", position, target, up, fov, projection, mode) 
print("Camera Created! ID: ", entity)