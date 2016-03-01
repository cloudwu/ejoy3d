local ejoy3d = require "ejoy3d"
local shader = require "ejoy3d.shader"
local texture = require "ejoy3d.texture"
local matrix = require "ejoy3d.matrix"
local asset = require "util.asset"
local scene = require "ejoy3d.scene"

------------

local game = {
	asset_path = "./test/",
}

local prog

function game.init()
	prog = shader.load {
		vs = "test.vs",
		fs = "test.fs",
		uniform = {
			viewProjMat = "matrix",
			worldMat = "matrix",
			worldNormalMat = "matrix33",
			lightDir = "vector3",
		},
	}
	prog:bind()
	texture.default:bind(0)

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

	vertex_buffer:bind(0)	-- bind vb to slot 0

	shader.setcull "BACK"
end

local t = 5
local d = 0

function game.update()
	shader.clear 'cd'
	prog.viewProjMat = scene.camera(t, 30)
	local m = matrix.new()
	prog.worldMat = m:rot(0,d,0)
	prog.worldNormalMat = m:inverted():transposed()
	matrix.drop(m)
	d = d + 0.01
	prog.lightDir = scene.lightdir(90, 60)
	shader.draw(36)
end

ejoy3d.start(game)
