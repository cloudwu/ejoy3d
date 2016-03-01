local matrix = require "ejoy3d.matrix"
local math3d = require "ejoy3d.math3d"

local scene = {}

local c_projmat = matrix.projmat(45, 4/3, 0.1, 100000)

local t_camera = matrix.new()
function scene.camera(height, pitch)
	local alpha = pitch * math.pi / 180
	local tmp = matrix.new():rot(alpha,0,0):trans(0,0,-height / math.tan(alpha))
	t_camera:mul(c_projmat, tmp)
	matrix.drop(tmp)
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
