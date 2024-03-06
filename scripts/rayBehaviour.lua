local vector4 = require("scripts/vector4")

local ray = {}

local FLT_MAX = 340282346638528859811704183484516925440.0
local white = vector4.new( 255, 255, 255, 255 )
local orange = vector4.new(255, 161, 0, 255) 
local blue = vector4.new(0, 121, 241, 255)
local green = vector4.new(0, 228, 48, 255)

function ray:OnCreate()
	print("Ray created! ")
	print(self.ID)
end

function ray:OnUpdate(delta)

end

function ray:HitNothing()
	return white, "None", false, FLT_MAX, 0
end

function ray:HitBox()
	return orange, "Box"
end

function ray:HitMesh()
	return blue, "Mesh"
end

function ray:HitGround()
	return green, "Ground"
end

return ray