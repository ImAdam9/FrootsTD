local vector4 = {}

vector4.__index = vector4

local function new(x, y, z, w)
	local t = {
		x = x or 0,
		y = y or 0,
		z = z or 0,
		w = w or 0
	}
	return setmetatable(t, vector4)
end

local function isvector(t)
	return getmetatable(t) == vector4
end

local function __newindex (t , k , v , w)
	print (" vector4 - not possible to assign new fields ")
end

local function __tostring ( t )
	return "(" .. t.x .. ", " .. t.y .. ", " .. t.z .. ", " .. t.w ..")"
end

local function __unm(t)
	return new(-t.x, -t.y, -t.z, -t.w)
end

local function __add(a, b)
	assert(isvector(a) and isvector(b), "vector add - expected args: vector, vector")
	return new(a.x+b.x, a.y+b.y, a.z+b.z, a.w+b.w)
end

local function __mul(a, b)
	-- Check for scalar multiplication
	if type(a) == "number" then
		return new(a*b.x, a*b.y, a*b.z, a*b.w)
	elseif type(b) == "number" then
		return new(a.x*b, a.y*b, a.z*b, a.w*b)
	-- None of a or b is number from here on
	else
		assert(isvector(a) and isvector(b), "vector mul - expected args: vector or number")
		return new(a.x*b.x, a.y*b.y, a.z*b.z, a.w*b.w)
	end
end

local function __eq(a, b)
	assert(isvector(a) and isvector(b), "vector eq - expected args: vector, vector")
	return a.x==b.x, a.y==b.y, a.z==b.z, a.w==b.w
end

function vector4:length()
	return math.sqrt(self.x*self.x + self.y*self.y + self.z*self.z + self.w*self.w)
end


vector4.new = new
vector4.isvector = isvector
vector4.__newindex = __newindex
vector4.__tostring = __tostring
vector4.__unm = __unm
vector4.__add = __add
vector4.__mul = __mul
vector4.__eq = __eq

return vector4