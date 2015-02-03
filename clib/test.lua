render = require "ejoy3d.render"
glfw = require "ejoy3d.glfw"

FS = [[
varying vec2 v_texcoord;
uniform sampler2D texture0;

void main() {
	vec4 tmp = texture2D(texture0, v_texcoord);
	gl_FragColor = tmp;
}
]]

VS = [[
attribute vec4 position;
attribute vec2 texcoord;

varying vec2 v_texcoord;

void main() {
	gl_Position = position;
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
	{ name = "position", vbslot = 0, n = 2, size = 4, offset = 0 },
	{ name = "texcoord", vbslot = 0, n = 2, size = 2, offset = 8 },
})

render.set(R, type.VERTEXLAYOUT, layout)

local prog = render.shader_create (R,{
	vs = VS,
	fs = FS,
})

render.bind(R, prog)

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
local index_buffer = render.buffer_create(R, type.INDEXBUFFER, ("HHH"):pack(0,1,2), 3, 2)
render.set(R, type.INDEXBUFFER, index_buffer)
local vertex_buffer = render.buffer_create(R, type.VERTEXBUFFER, ("ffHHffHHffHH"):pack(
	0,1, 0,0,
	-1,-1, 0,0xffff,
	1,-1, 0xffff, 0
), 3, 12)
render.set(R, type.VERTEXBUFFER, vertex_buffer)

while not glfw.WindowShouldClose(window) do
	render.clear(R, 'c', 0)
	render.draw(R,0,3)

	glfw.SwapBuffers(window)
	glfw.PollEvents()
end

render.exit(R)
glfw.DestroyWindow(window)
glfw.Terminate()

