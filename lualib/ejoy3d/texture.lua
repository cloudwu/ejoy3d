local render = require "ejoy3d.render"
local enum = require "ejoy3d.enum"
local asset = require "ejoy3d.asset"

local R
local texture_path

local M = {}

local texture = {}
texture.__index = texture

do
	local set = render.set
	local TEXTURE = enum.type.TEXTURE
	function texture:bind(slot)
		return set(R, TEXTURE, self.id, slot or 0)
	end

	function texture:release()
		render.release(R, TEXTURE, self.id)
		self.id = nil
	end

	function texture:update(data, w, h)
		return render.texture_update(R, self.id, w or self.width, h or self.height, data)
	end
end

function M.init(r , path)
	R = r
	texture_path = path
	-- create 8x8 grid
	local texdata = (("\xff\0\xff\xff\xff\xff"):rep(4) .. ("\xff\xff\xff\xff\0\xff"):rep(4)):rep(4)

	local tex = render.texture_create(R, 8,8, enum.texture.RGB, false, false, "nN")
	render.texture_update(R,tex,8,8,texdata)
	M.default = setmetatable({
		width = 8,
		height = 8,
		id = tex,
	}, texture)
end

return M
