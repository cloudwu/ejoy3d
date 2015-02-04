local asset = {}

function asset.load(filename)
	local f = assert(io.open(filename, "rb"),filename)
	local c = f:read "a"
	f:close(f)
	return c
end

return asset
