local asset = {}

local function insert(t, ...)
	local n = #t
	for i = 1, select("n", ...) do
		t[n+i] = select(i, ...)
	end
end

local function calc_normal(v, f, n)
	local math3d = require "ejoy3d.math3d"
	local plane = math3d.plane()
	local v1 = math3d.vector3()
	local v2 = math3d.vector3()
	local v3 = math3d.vector3()
	for i = 1, #f do
		local face = f[i]
		local vec3 = v[face[1]]
		v1:pack(vec3[1],vec3[2],vec3[3])
		table.insert(vec3,i)
		local vec3 = v[face[4]]
		v2:pack(vec3[1],vec3[2],vec3[3])
		table.insert(vec3,i)
		local vec3 = v[face[7]]
		v3:pack(vec3[1],vec3[2],vec3[3])
		table.insert(vec3,i)
		face[10], face[11], face[12] = plane:dot3(v1,v2,v3):normal()
		face[3] = i
		face[6] = i
		face[9] = i
	end
	-- calc normal of v
	for i = 1, #v do
		local x,y,z = 0,0,0
		local vec3 = v[i]
		for j = 4, #vec3 do
			local face = f[vec3[j]]
			x = x + face[10]
			y = y + face[11]
			z = z + face[12]
		end
		v1:pack(x,y,z):normalize()
		n[#n+1] = { v1:unpack() }
	end
end

local function packnumber(...)
	local tmp = table.pack(...)
	for i=1,tmp.n do
		if tmp[i] then
			tmp[i] = tonumber(tmp[i])
		end
	end
	return tmp
end

-- https://en.wikipedia.org/wiki/Wavefront_.obj_file
function asset.obj(filename)
	local vertex = {}
	local uv = {}
	local face = {}
	local normal = {}
	local f = io.open(filename)
	for line in f:lines() do
		local tag, data = line:match "^(%a+)%s*(.*)"
		if tag == "v" then
			table.insert(vertex,packnumber(data:match("([%d.-]+)%s+([%d.-]+)%s+([%d.-]+)")))
		elseif tag == "vt" then
			local u,v = data:match "([%d.]+)%s+([%d.]+)"
			table.insert(uv, tonumber(u))
			table.insert(uv, tonumber(v))
		elseif tag == "vn" then
			table.insert(normal,packnumber(data:match("([%d.-]+)%s+([%d.-]+)%s+([%d.-]+)")))
		elseif tag == "f" then
			table.insert(face, packnumber(data:match "(%d+)/?(%d*)/?(%d*)%s+(%d+)/?(%d*)/?(%d*)%s+(%d+)/?(%d*)/?(%d*)"))
		else	-- ignore
		end
	end
	f:close()
	if face[1][3] == nil then
		calc_normal(vertex, face, normal)
	end
	local ib = {}
	local vb = {}
	for i = 1 , #vertex do
		local idx = i * 8
		vb[idx-7] = vertex[i][1]
		vb[idx-6] = vertex[i][2]
		vb[idx-5] = vertex[i][3]
		vb[idx-4] = normal[i][1]
		vb[idx-3] = normal[i][2]
		vb[idx-2] = normal[i][3]
		vb[idx-1] = false
		vb[idx] = false
	end
	local function fix_uv(vindex, tindex)
		vindex = vindex - 1
		vb[vindex*8 + 7] = math.floor(uv[tindex*2 - 1] * 0xffff)
		vb[vindex*8 + 8] = math.floor(uv[tindex*2] * 0xffff)
		table.insert(ib, vindex)
	end
	for i = 1, #face do
		local f = face[i]
		fix_uv(f[1], f[2])
		fix_uv(f[4], f[5])
		fix_uv(f[7], f[8])
	end

	return ib, vb
end

return asset
