local render = require "ejoy3d.render"
local glfw = require "ejoy3d.glfw"
local math3d = require "ejoy3d.math3d"

--local a = math3d.vector3(1,2,3)
--local b = math3d.vector3(4,5,6)
--print(math3d.vector3():cross(a,b):normalize())
--print(math3d.quaternion())
--print(math3d.matrix(0,0,0))

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

FS = [[
varying vec2 v_texcoord;
varying vec4 v_colorVarying;
uniform sampler2D texture0;

void main() {
	vec4 tmp = texture2D(texture0, v_texcoord);
	gl_FragColor = tmp * v_colorVarying;
}
]]

VS = [[
attribute vec4 position;
attribute vec3 normal;
attribute vec2 texcoord;

uniform mat4 modelViewProjectionMatrix;
uniform mat3 normalMatrix;

varying vec4 v_colorVarying;
varying vec2 v_texcoord;

void main()
{
	vec3 eyeNormal = normalize(normalMatrix * normal);
	vec3 lightPosition = vec3(0.0, 0.0, 1.0);
	vec4 diffuseColor = vec4(0.4, 0.4, 1.0, 1.0);
	float nDotVP = max(0.0, dot(eyeNormal, normalize(lightPosition)));
	v_colorVarying = diffuseColor * nDotVP;
	gl_Position = modelViewProjectionMatrix * position;
	v_texcoord = texcoord;
}
]]


glfw.SetErrorCallback()
glfw.Init()
local window = glfw.CreateWindow(640,480, "测试")
glfw.MakeContextCurrent(window)
glfw.SwapInterval(1)

R = render.init {
	buffer = 128,
	layout = 8,
	target = 16,
	texture = 128,
	shader = 16,
	log = function(msg)
		print(msg)
	end
}
render.viewport(R, 0,0,640,480)

local type = {
	VERTEXLAYOUT = 1,
	VERTEXBUFFER = 2,
	INDEXBUFFER = 3,
	TEXTURE = 4,
	TARGET = 5,
	SHADER = 6,
}

local layout = render.register_layout(R, {
	{ name = "position", vbslot = 0, n = 3, size = 4, offset = 0 },
	{ name = "normal", vbslot = 0, n = 3, size = 4, offset = 12 },
	{ name = "texcoord", vbslot = 0, n = 2, size = 2, offset = 24 },
})

render.set(R, type.VERTEXLAYOUT, layout)

local prog = render.shader_create (R,{
	vs = VS,
	fs = FS,
})

render.bind(R, prog)
local loc_modelViewProjectionMatrix = assert(render.locuniform(R, "modelViewProjectionMatrix"))
local loc_normalMatrix = assert(render.locuniform(R, "normalMatrix"))

---- camera

local function projmat(fov, aspect, near, far)
	local ymax = near * math.tan(fov * math.pi / 360)
	local xmax = ymax * aspect
	local mat = math3d.matrix()
	return mat:perspective(-xmax, xmax, -ymax, ymax, near, far)
end

--[[
	union matrix44 modelViewMatrix;
	matrix44_trans(&modelViewMatrix, 0.0f, 0.0f, 3.0f);
	union matrix44 rot;
	matrix44_rot(&rot, 1.0f, 1.0f, 1.0f);
	matrix44_mul(&modelViewMatrix, &rot, &modelViewMatrix);

	camera_update(&C, &modelViewMatrix);
]]

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

-- create 8x8 grid
local texdata = (("\xff\0\xff\xff\xff\xff"):rep(4) .. ("\xff\xff\xff\xff\0\xff"):rep(4)):rep(4)

local format = {
	TEXTURE_RGBA8 = 1,
	TEXTURE_RGBA4 = 2,
	TEXTURE_RGB = 3,
	TEXTURE_RGB565 = 4,
	TEXTURE_A8 = 5,
	TEXTURE_DEPTH = 6,
	TEXTURE_PVR2 = 7,
	TEXTURE_PVR4 = 8,
	TEXTURE_ETC1 = 9,
}

local tex = render.texture_create(R, 8,8, format.TEXTURE_RGB, false, false, "nN")
render.texture_update(R,tex,8,8,texdata)
render.set(R,type.TEXTURE,tex,0)	-- set texture 0

-- 0 vertexbuffer 1 indexbuffer
local index_buffer = render.buffer_create(R, type.INDEXBUFFER, cube_ib())
render.set(R, type.INDEXBUFFER, index_buffer)
local vertex_buffer = render.buffer_create(R, type.VERTEXBUFFER, cube_vb())
render.set(R, type.VERTEXBUFFER, vertex_buffer)


render.setuniform_matrix33(R, loc_normalMatrix, normalMatrix)
render.setuniform_matrix(R, loc_modelViewProjectionMatrix, modelViewProjectionMatrix)

while not glfw.WindowShouldClose(window) do
	render.clear(R, 'cd', 0)

	render.draw(R,0,36)

	glfw.SwapBuffers(window)
	glfw.PollEvents()
end

render.exit(R)
glfw.DestroyWindow(window)
glfw.Terminate()

