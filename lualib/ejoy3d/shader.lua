local render = require "ejoy3d.render"
local enum = require "ejoy3d.enum"
local asset = require "ejoy3d.asset"

local R
local shader_path

local shader = {}

function shader.init(r , path)
	R = r
	shader_path = path

	local layout = render.register_layout(R, {
		{ name = "position", vbslot = 0, n = 3, size = 4, offset = 0 },
		{ name = "normal", vbslot = 0, n = 3, size = 4, offset = 12 },
		{ name = "texcoord", vbslot = 0, n = 2, size = 2, offset = 24 },
	})
	render.set(R, enum.type.VERTEXLAYOUT, layout)
	shader.default_layout = layout
end

local program = {}
program.__index = program

do
	local bind = render.bind
	function program:bind()
		return bind(R, self._id)
	end

	function program:release()
		render.release(R, enum.type.SHADER, self._id)
		self.id = nil
	end
end

function program:__newindex(uniform, value)
	return self._uniform[uniform](value)
end

local function gen_uniform_setter(uniform, type)
	local f = assert(render["setuniform_" .. type], type)
	local loc = assert(render.locuniform(R, uniform), uniform)
	return function(value)
		return f(R, loc, value)
	end
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

	local obj = {
		_vs = fx.vs,
		_fs = fx.fs,
		_id = prog,
		_uniform = {},
	}

	local uniform = fx.uniform
	if uniform then
		render.bind(R, prog)
		for k, v in pairs(uniform) do
			obj._uniform[k] = gen_uniform_setter(k,v)
		end
	end
	return setmetatable(obj, program)
end

function shader.register_layout(layout)
	--todo: move to fx
	return render.register_layout(R, layout)
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

-------------render state---------------

do
	local setcull = render.setcull
	local e = enum.cull
	function shader.setcull(mode)
		setcull(R, e[mode])
	end
end

do
	local setdepth = render.setdepth
	local setdepthmask = render.depthmask
	local e  = enum.depth
	function shader.setdepth(mode, writing)
		setdepthmask(R, writing)
		setdepth(R, e[mode])
	end
end

do
	local draw = render.draw
	function shader.draw(n)
		draw(R, 0, n)
	end
end

do
	local clear = render.clear
	function shader.clear(mode, argb)
		clear(R, mode, argb)
	end
end

------------index & vertex buffer--------
do
	local index_buffer = {}
	index_buffer.__index = index_buffer
	local vertex_buffer = {}
	vertex_buffer.__index = vertex_buffer

	local INDEXBUFFER = enum.type.INDEXBUFFER
	local VERTEXBUFFER = enum.type.VERTEXBUFFER
	local buffer_create = render.buffer_create
	local buffer_update = render.buffer_update

	local function pack_indexdata(data)
		local n = 0
		if data then
			n = #data
			data = ("H"):rep(n):pack(table.unpack(data))
		end
		return data, n
	end

	function index_buffer:update(data)
		local data, n = pack_indexdata(data)
		self.n = n
		return buffer_update(R, self.id, data, n)
	end

	function index_buffer:release()
		render.release(R, INDEXBUFFER, self.id)
		self.id = nil
	end

	function index_buffer:bind()
		return render.set(R, INDEXBUFFER, self.id)
	end

	function shader.index_buffer(data)
		local data , n = pack_indexdata(data)
		local ib = buffer_create(R, INDEXBUFFER, data, n , 2)
		local obj = {
			id = ib,
			n = n,
		}
		return setmetatable(obj, index_buffer)
	end

	local function pack_vertexdata(data)
		local n = 0
		if data then
			n = #data // 8
			data = ("ffffffHH"):rep(n):pack(table.unpack(data))
		end
		return data, n
	end

	function vertex_buffer:update(data)
		local data, n = pack_vertexdata(data)
		self.n = n
		return buffer_update(R, self.id, data, n)
	end

	function vertex_buffer:release()
		render.release(R, VERTEXBUFFER, self.id)
		self.id = nil
	end

	function vertex_buffer:bind(slot)
		return render.set(R, VERTEXBUFFER, self.id, slot)
	end

	function shader.vertex_buffer(data)
		local data, n = pack_vertexdata(data)
		-- default layout stride = 28, float * 6 + ushort * 2
		local vb = buffer_create(R, VERTEXBUFFER, data, n, 28)
		local obj = {
			id = vb,
			n = n,
		}
		return setmetatable(obj, vertex_buffer)
	end
end

return shader
