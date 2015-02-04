local render = require "ejoy3d.render"
local enum = require "ejoy3d.enum"
local asset = require "ejoy3d.asset"

local R
local shader_path

local shader = {}

function shader.init(r , path)
	R = r
	shader_path = path
end

function shader.load(fx)
	local vs = assert(fx.vs)
	local fs = assert(fx.fs)
	-- todo: load fx file
	local vs_src = asset.load(shader_path .. vs)
	local fs_src = asset.load(shader_path .. fs)
	local prog = render.shader_create (R,{
		vs = vs_src,
		fs = fs_src,
		texture = fx.texture,
	})
	-- todo: put it into fx
	local uniform = fx.uniform
	if uniform then
		render.bind(R, prog)
		for k in pairs(uniform) do
			uniform[k] = assert(render.locuniform(R, k))
		end
	end
	return prog
end

function shader.register_layout(layout)
	--todo: move to fx
	return render.register_layout(R, {
		{ name = "position", vbslot = 0, n = 3, size = 4, offset = 0 },
		{ name = "normal", vbslot = 0, n = 3, size = 4, offset = 12 },
		{ name = "texcoord", vbslot = 0, n = 2, size = 2, offset = 24 },
	})
end

do
	local set = render.set
	local VERTEXLAYOUT = enum.type.VERTEXLAYOUT
	function shader.set_layout(layout)
		set(R, VERTEXLAYOUT, layout)
	end
end

do
	local bind = render.bind
	function shader.bind(prog)
		return bind(R, prog)
	end
end

return shader
