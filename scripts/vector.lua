local vector = {}

vector.__index = vector

local function new(x, y, z)
	local t = {
		x = x or 0,
		y = y or 0,
		z = z or 0
	}
	return setmetatable(t, vector)
end

local function isvector(t)
	return getmetatable(t) == vector
end

local function __newindex (t , k , v )
	print (" vector - not possible to assign new fields ")
end

local function __tostring ( t )
	return "(" .. t.x .. ", " .. t.y .. ", " .. t.z .. ")"
end

local function __unm(t)
	return new(-t.x, -t.y, -t.z)
end

local function __add(a, b)
	assert(isvector(a) and isvector(b), "vector add - expected args: vector, vector")
	return new(a.x+b.x, a.y+b.y, a.z+b.z)
end

local function __mul(a, b)
	-- Check for scalar multiplication
	if type(a) == "number" then
		return new(a*b.x, a*b.y, a*b.z)
	elseif type(b) == "number" then
		return new(a.x*b, a.y*b, a.z*b)
	-- None of a or b is number from here on
	else
		assert(isvector(a) and isvector(b), "vector mul - expected args: vector or number")
		return new(a.x*b.x, a.y*b.y, a.z*b.z)
	end
end

local function __eq(a, b)
	assert(isvector(a) and isvector(b), "vector eq - expected args: vector, vector")
	return a.x==b.x, a.y==b.y, a.z==b.z
end

function vector.length(a)
	return math.sqrt((a.x*a.x + a.y*a.y + a.z*a.z))
end

function vector.distance(a, b)
	local d = math.sqrt((b.x-a.x)*(b.x-a.x) + (b.y-a.y)*(b.y-a.y) + (b.z-a.z)*(b.z-a.z))
	return d
end

function vector.normalize(a)
	local d = vector.length(a)
	if d == 0 then
		return new(0, 0, 0)
	else
		return new(a.x/d, a.y/d, a.z/d)
	end
end

--function vector.distanceArr(a, bArr, cArr) -- a = playerPos, bArr = enemyPos[], cArr = enemyID[] 
--	local distTable = {}
--	local IDTable = {}
--	local len = # bArr

--	for i = 1, len do  
--		distTable[bArr[i]] = vector.distance(a, bArr[i + 1])
--		IDTable[cArr[i]] = cArr[i + 1]
--	end

--	local furthestDistance = distTable[bArr[1]]
	--local furthestID = IDTable[cArr[1]]
--	for i = 2, len do
--		if distTable[bArr[i]] < furthestDistance then
--			furthestDistance = distTable[bArr[i]]
--			furthestID = IDTable[cArr[i]]
--		end
--	end

--	return furthestID
--end

vector.new = new
vector.isvector = isvector
vector.__newindex = __newindex
vector.__tostring = __tostring
vector.__unm = __unm
vector.__add = __add
vector.__mul = __mul
vector.__eq = __eq

return vector
