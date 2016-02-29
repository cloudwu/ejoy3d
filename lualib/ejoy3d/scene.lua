local matrix = require "ejoy3d.matrix"
local math3d = require "ejoy3d.math3d"

local scene = {}

local c_projmat = matrix.projmat(45, 4/3, 0.1, 100000)

local t_camera = matrix.new()
function scene.camera(height, pitch)
	local alpha = pitch * math.pi / 180
	local tmat = matrix.transmat(0,0,-height / math.tan(alpha))
	local rmat = matrix.rotmat(alpha, 0,0)
	t_camera:mul(tmat,rmat)
	t_camera:mul(c_projmat, t_camera)
	matrix.drop(rmat)
	matrix.drop(tmat)
	return t_camera
end


local t_dir = math3d.vector3()
function scene.lightdir(yaw, pitch)
	yaw = yaw * math.pi / 180
	pitch = pitch * math.pi / 180
	local x = math.cos(yaw)
	local z = math.sin(yaw)
	local y = math.tan(pitch)
	return t_dir:pack(x,y,z):normalize()
end

return scene
