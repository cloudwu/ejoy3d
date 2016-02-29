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

local N
local prog

function game.init()
	prog = shader.load {
		vs = "test.vs",
		fs = "test.fs",
		uniform = {
			viewProjMat = "matrix",
			worldMat = "matrix",
			lightDir = "vector3",
		},
	}
	prog:bind()
	texture.default:bind(0)

	local ib, vb = asset.obj "test/wolf.obj"
	N = #ib

	local index_buffer = shader.index_buffer(ib)
	index_buffer:bind()

	local vertex_buffer = shader.vertex_buffer(vb)
	vertex_buffer:bind(0)	-- bind vb to slot 0

	shader.setdepth("LESS", true)
	shader.setcull "BACK"
end

local t = 500
local light = 0
local d = 0

function game.update()
	shader.clear 'cd'
	prog.viewProjMat = scene.camera(t, 30)
	prog.worldMat = matrix.rotmat(0,d,0)
	d = d + 0.001
	prog.lightDir = scene.lightdir(light, 60)
	light = light + 1
	shader.draw(N)
end

ejoy3d.start(game)
