local ejoy3d = require "ejoy3d"
local shader = require "ejoy3d.shader"
local texture = require "ejoy3d.texture"

local math3d = require "ejoy3d.math3d"


---- camera

local function projmat(fov, aspect, near, far)
	local ymax = near * math.tan(fov * math.pi / 360)
	local xmax = ymax * aspect
	local mat = math3d.matrix()
	return mat:perspective(-xmax, xmax, -ymax, ymax, near, far)
end

local modelViewMatrix = math3d.vector3(0,0,3):transmat(math3d.matrix())
local rot = math3d.matrix(1,1,1)
modelViewMatrix:mul(rot, modelViewMatrix)

local c_projmat = projmat(45, 4/3, 0.1, 100)
local c_viewmat = math3d.matrix():inverted(modelViewMatrix)

local modelViewProjectionMatrix = math3d.matrix()
modelViewProjectionMatrix:mul(c_projmat, c_viewmat)
local normalMatrix = math3d.matrix(modelViewMatrix)
normalMatrix:transposed()

------------

local game = {
	asset_path = "./test/",
}

function game.init()
	local prog = shader.load {
		vs = "test.vs",
		fs = "test.fs",
		uniform = {
			modelViewProjectionMatrix = "matrix",
			normalMatrix = "matrix33",
		},
	}
	prog:bind()

------------- todo
	local R = ejoy3d.R

	texture.default:bind(0)

	-- 0 vertexbuffer 1 indexbuffer
	local index_buffer = shader.index_buffer {
		0, 1, 2,
		2, 1, 3,

		4, 5, 6,
		6, 5, 7,

		8, 9, 10,
		10, 9, 11,

		12, 13, 14,
		14, 13, 15,

		16, 17, 18,
		18, 17, 19,

		20, 21, 22,
		22, 21, 23,
	}
	index_buffer:bind()

	local vertex_buffer = shader.vertex_buffer {
-- positionX, positionY, positionZ,     normalX, normalY, normalZ
		0.5, -0.5, -0.5,        1.0, 0.0, 0.0,        0,0,
		0.5, 0.5, -0.5,         1.0, 0.0, 0.0,        0xffff,0,
		0.5, -0.5, 0.5,         1.0, 0.0, 0.0,        0,0xffff,
		0.5, 0.5, 0.5,          1.0, 0.0, 0.0,        0xffff,0xffff,

		0.5, 0.5, -0.5,         0.0, 1.0, 0.0,        0,0,
		-0.5, 0.5, -0.5,        0.0, 1.0, 0.0,        0xffff,0,
		0.5, 0.5, 0.5,          0.0, 1.0, 0.0,        0,0xffff,
		-0.5, 0.5, 0.5,         0.0, 1.0, 0.0,        0xffff,0xffff,

		-0.5, 0.5, -0.5,        -1.0, 0.0, 0.0,        0,0,
		-0.5, -0.5, -0.5,       -1.0, 0.0, 0.0,        0xffff,0,
		-0.5, 0.5, 0.5,         -1.0, 0.0, 0.0,        0,0xffff,
		-0.5, -0.5, 0.5,        -1.0, 0.0, 0.0,        0xffff,0xffff,

		-0.5, -0.5, -0.5,       0.0, -1.0, 0.0,        0,0,
		0.5, -0.5, -0.5,        0.0, -1.0, 0.0,        0xffff,0,
		-0.5, -0.5, 0.5,        0.0, -1.0, 0.0,        0,0xffff,
		0.5, -0.5, 0.5,         0.0, -1.0, 0.0,        0xffff,0xffff,

		0.5, 0.5, 0.5,          0.0, 0.0, 1.0,        0,0,
		-0.5, 0.5, 0.5,         0.0, 0.0, 1.0,        0xffff,0,
		0.5, -0.5, 0.5,         0.0, 0.0, 1.0,        0,0xffff,
		-0.5, -0.5, 0.5,        0.0, 0.0, 1.0,        0xffff,0xffff,

		0.5, -0.5, -0.5,        0.0, 0.0, -1.0,        0,0,
		-0.5, -0.5, -0.5,       0.0, 0.0, -1.0,        0xffff,0,
		0.5, 0.5, -0.5,         0.0, 0.0, -1.0,        0,0xffff,
		-0.5, 0.5, -0.5,        0.0, 0.0, -1.0,        0xffff,0xffff,
	}

	vertex_buffer:bind()

	prog.normalMatrix = normalMatrix
	prog.modelViewProjectionMatrix = modelViewProjectionMatrix
	--render.depthmask(R, false)
	--render.setdepth(R,0)
	shader.setcull "BACK"
end

function game.update()
	shader.clear 'cd'
	shader.draw(36)
end

ejoy3d.start(game)
