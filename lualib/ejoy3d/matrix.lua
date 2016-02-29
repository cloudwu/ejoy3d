local math3d = require "ejoy3d.math3d"

local matrix = {}

local weak_mode = { __mode = "kv" }
local matcache = setmetatable({}, weak_mode)
local veccache = setmetatable({}, weak_mode)

local get = table.remove
function matrix.new()
	local tmp = get(matcache)
	if tmp then
		return tmp:identity()
	else
		return math3d.matrix()
	end
end

function matrix.clone(m)
	local tmp = get(matcache)
	if tmp then
		return tmp:copy(m)
	else
		return math3d.matrix(m)
	end
end

local set = table.insert
function matrix.drop(m)
	set(matcache, m)
end

function matrix.projmat(fov, aspect, near, far)
	local ymax = near * math.tan(fov * math.pi / 360)
	local xmax = ymax * aspect
	local mat = matrix.new()
	return mat:perspective(-xmax, xmax, -ymax, ymax, near, far)
end

local function vecnew(...)
	local tmp = get(veccache)
	if tmp then
		return tmp:pack(...)
	else
		return math3d.vector3(...)
	end
end

function matrix.transmat(x,y,z)
	local tmp = vecnew(x,y,z)
	local ret =  tmp:transmat(matrix.new())
	set(veccache, tmp)
	return ret
end

function matrix.scalemat(x,y,z)
	local tmp = vecnew(x,y,z)
	local ret = tmp:scalemat(matrix.new())
	set(veccache, tmp)
	return ret
end

function matrix.rotmat(x,y,z)
	local tmp = vecnew(x,y,z)
	local ret = tmp:rotmat(matrix.new())
	set(veccache, tmp)
	return ret
end

function matrix.invmat(m)
	return matrix.new():inverted(m)
end

return matrix
