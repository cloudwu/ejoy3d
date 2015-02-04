local ejoy3d = require "ejoy3d"
local shader = require "ejoy3d.shader"

local render = require "ejoy3d.render"
local math3d = require "ejoy3d.math3d"
local enum = require "ejoy3d.enum"

local function cube_vb()
-- positionX, positionY, positionZ,     normalX, normalY, normalZ
	local cube = {
		0.5, -0.5, -0.5,        1.0, 0.0, 0.0,        0,0,
		0.5, 0.5, -0.5,         1.0, 0.0, 0.0,        1,0,
		0.5, -0.5, 0.5,         1.0, 0.0, 0.0,        0,1,
		0.5, 0.5, 0.5,          1.0, 0.0, 0.0,        1,1,

		0.5, 0.5, -0.5,         0.0, 1.0, 0.0,        0,0,
		-0.5, 0.5, -0.5,        0.0, 1.0, 0.0,        1,0,
		0.5, 0.5, 0.5,          0.0, 1.0, 0.0,        0,1,
		-0.5, 0.5, 0.5,         0.0, 1.0, 0.0,        1,1,

		-0.5, 0.5, -0.5,        -1.0, 0.0, 0.0,        0,0,
		-0.5, -0.5, -0.5,       -1.0, 0.0, 0.0,        1,0,
		-0.5, 0.5, 0.5,         -1.0, 0.0, 0.0,        0,1,
		-0.5, -0.5, 0.5,        -1.0, 0.0, 0.0,        1,1,

		-0.5, -0.5, -0.5,       0.0, -1.0, 0.0,        0,0,
		0.5, -0.5, -0.5,        0.0, -1.0, 0.0,        1,0,
		-0.5, -0.5, 0.5,        0.0, -1.0, 0.0,        0,1,
		0.5, -0.5, 0.5,         0.0, -1.0, 0.0,        1,1,

		0.5, 0.5, 0.5,          0.0, 0.0, 1.0,        0,0,
		-0.5, 0.5, 0.5,         0.0, 0.0, 1.0,        1,0,
		0.5, -0.5, 0.5,         0.0, 0.0, 1.0,        0,1,
		-0.5, -0.5, 0.5,        0.0, 0.0, 1.0,        1,1,

		0.5, -0.5, -0.5,        0.0, 0.0, -1.0,        0,0,
		-0.5, -0.5, -0.5,       0.0, 0.0, -1.0,        1,0,
		0.5, 0.5, -0.5,         0.0, 0.0, -1.0,        0,1,
		-0.5, 0.5, -0.5,        0.0, 0.0, -1.0,        1,1,
	}
	local tmp = {}
	for i=1,24*8,8 do
		table.insert(tmp, string.pack("ffffffHH",
			cube[i],cube[i+1],cube[i+2],
			cube[i+3],cube[i+4],cube[i+5],
			cube[i+6]*0xffff,cube[i+7]*0xffff))
	end
	return table.concat(tmp), 24, 28
end

local function cube_ib()
	local index = {
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
	local tmp = {}
	for _,v in ipairs(index) do
		table.insert(tmp, string.pack("H",v))
	end
	return table.concat(tmp), 36, 2
end

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
	shader_path = "./test/",
}

local uniform = {
	modelViewProjectionMatrix = true,
	normalMatrix = true,
}

function game.init()
	local layout = shader.register_layout {
		{ name = "position", vbslot = 0, n = 3, size = 4, offset = 0 },
		{ name = "normal", vbslot = 0, n = 3, size = 4, offset = 12 },
		{ name = "texcoord", vbslot = 0, n = 2, size = 2, offset = 24 },
	}
	shader.set_layout(layout)

	local prog = shader.load {
		vs = "test.vs",
		fs = "test.fs",
		uniform = uniform,
	}
	shader.bind(prog)

------------- todo
	local R = ejoy3d.R

	-- create 8x8 grid
	local texdata = (("\xff\0\xff\xff\xff\xff"):rep(4) .. ("\xff\xff\xff\xff\0\xff"):rep(4)):rep(4)

	local tex = render.texture_create(R, 8,8, enum.texture.RGB, false, false, "nN")
	render.texture_update(R,tex,8,8,texdata)
	render.set(R, enum.type.TEXTURE,tex,0)	-- set texture 0

	-- 0 vertexbuffer 1 indexbuffer
	local index_buffer = render.buffer_create(R, enum.type.INDEXBUFFER, cube_ib())
	render.set(R, enum.type.INDEXBUFFER, index_buffer)
	local vertex_buffer = render.buffer_create(R, enum.type.VERTEXBUFFER, cube_vb())
	render.set(R, enum.type.VERTEXBUFFER, vertex_buffer)

	render.setuniform_matrix33(R, uniform.normalMatrix, normalMatrix)
	render.setuniform_matrix(R, uniform.modelViewProjectionMatrix, modelViewProjectionMatrix)
	--render.depthmask(R, false)
	--render.setdepth(R,0)
	render.setcull(R,2)
end

function game.update()
	render.clear(ejoy3d.R, 'cd', 6)
	render.draw(ejoy3d.R,0,36)
end

ejoy3d.start(game)
