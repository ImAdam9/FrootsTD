local vector = require("scripts/vector")
g_screenWidth = 1200
g_screenHeight = 1000
g_startTile = vector.new(0,0,0)
g_endGame = 0 -- 0 = false 1 = true
g_gameResult = 0 -- 0 = keep playing, 1 = lost, 2 = win
g_wave = 1
g_baseHP = 10
g_killedEnemies = 0
g_money = 15