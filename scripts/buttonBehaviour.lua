local vector = require("scripts/vector")
local vector4 = require("scripts/vector4")

local myState = 0
local rectangle = vector4.new(0,0,0,0)
local text
local textData = vector.new(0,0,0)
local color = vector4.new(0,0,0,0)

local black = vector4.new( 0, 0, 0, 255 )
local gray = vector4.new( 130, 130, 130, 255)

local buttonBehaviour = {}

function buttonBehaviour:OnCreate()
	myState, rectangle = scene.GetComponent(self.ID, "button")
	print("Button created!", self.ID, myState)
end

function buttonBehaviour:OnUpdate(delta)
	
end

function buttonBehaviour:Draw(state)
	if myState == state then
		text, textData = scene.GetComponent(self.ID, "text")
		color = scene.GetComponent(self.ID, "colordata")
		scene.DrawRectangle(rectangle.x, rectangle.y, rectangle.z, rectangle.w, color)
		scene.DrawText(text, textData.x, textData.y, textData.z, gray)
		
	end
end

function buttonBehaviour:Selected()
	scene.DrawOutline(rectangle, black)
end

return buttonBehaviour