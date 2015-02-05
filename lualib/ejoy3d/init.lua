local render = require "ejoy3d.render"
local shader = require "ejoy3d.shader"
local texture = require "ejoy3d.texture"

local ejoy3d = {}
local R

local function create_device(config)
	config = config or {}
	R = render.init {
		buffer = config.buffer or 128,
		layout = config.layout or 8,
		target = config.target or 16,
		texture = config.texture or 128,
		shader = config.shader or 16,
		log = config.log or function(msg)
			print(msg)
		end,
	}
	ejoy3d.R = R
	render.viewport(R,0,0,ejoy3d.WIDTH,ejoy3d.HEIGHT)
end

function ejoy3d.start(config)
	local glfw = require "ejoy3d.glfw"
	ejoy3d.WIDTH = config.width or 1024
	ejoy3d.HEIGHT = config.height or 768
	glfw.SetErrorCallback()
	glfw.Init()
	local window = glfw.CreateWindow(
		ejoy3d.WIDTH,
		ejoy3d.HEIGHT,
		config.name or "ejoy3d"
	)
	glfw.MakeContextCurrent(window)
	glfw.SwapInterval(config.sync or 1)
	create_device(config.render)
	local path = config.asset_path or "./"
	shader.init(R, config.shader_path or path)
	texture.init(R, config.texture_path or path)

	-- init (user defined)
	if config.init then
		config.init()
	end

	-- main loop

	local update_func = config.update or function() end

	while not glfw.WindowShouldClose(window) do
		update_func()

		glfw.SwapBuffers(window)
		glfw.PollEvents()
	end

	-- term

	render.exit(R)
	glfw.DestroyWindow(window)
	glfw.Terminate()
end

return ejoy3d
