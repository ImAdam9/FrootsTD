local vector4 = require("scripts/vector4")


local black = vector4.new( 0, 0, 0, 255 )
local raywhite = vector4.new( 245, 245, 245, 255)
local lightgray = vector4.new(210, 210, 210, 255)
local gray = vector4.new( 130, 130, 130, 255)
local darkgray = vector4.new(80, 80, 80, 255 )


background = {
	Title = function()
		if g_gameResult > 0 then
			g_gameResult = 0
		end
		scene.DrawRectangle(0, 0, g_screenWidth, g_screenHeight, gray)
		scene.DrawText("FROOTS TD", g_screenWidth/4, g_screenHeight/4, 100, lightgray)
	end,

	Gameplay = function()
		scene.DrawRectangle(0, 0, g_screenWidth, g_screenHeight, gray)
		scene.DrawText("Money: ", g_screenWidth - 200, 0, 40, black)
		scene.DrawText(g_money, g_screenWidth - 50, 0, 40, black)
	end,

	Ending = function()
		scene.DrawRectangle(0, 0, g_screenWidth, g_screenHeight, gray)
		if g_gameResult > 0 then
			if g_gameResult == 1 then
				scene.DrawText("YOU LOST", g_screenWidth/2 - 50, g_screenHeight/2, 40, black)
			else
				scene.DrawText("YOU WIN", g_screenWidth/2- 50, g_screenHeight/2, 40, black)
			end
		else
			scene.DrawText("DEBUG - HELLO :)", g_screenWidth/2, g_screenHeight/2, 40, black)
		end

	end,

	Editor = function()
		scene.DrawRectangle(0, 0, g_screenWidth, g_screenHeight, gray)
	end,
}