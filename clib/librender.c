#include <lua.h>
#include <lauxlib.h>
#include "render.h"
#include "array.h"
#include "log.h"
#include "math3d.h"

static struct render *
render(lua_State *L) {
	struct render * R = lua_touserdata(L,1);
	luaL_argcheck(L, R != NULL, 1, "Need render device");
	return R;
}

static int
getint(lua_State *L, const char *key) {
	lua_getfield(L, -1, key);
	int v = luaL_checkinteger(L, -1);
	lua_pop(L, 1);
	return v;
}

static const char *
getstring(lua_State *L, const char *key) {
	lua_getfield(L, -1, key);
	const char *v = luaL_checkstring(L, -1);
	lua_pop(L, 1);
	return v;
}

static void
log_callback(void *ud, const char *msg) {
	lua_State *L = ud;
	lua_getfield(L, LUA_REGISTRYINDEX, "ejoy3d_log");
	lua_pushstring(L, msg);
	lua_pcall(L, 1, 0, 0);
}

static int
linit(lua_State *L) {
	struct render_init_args arg;
	luaL_checktype(L, 1, LUA_TTABLE);
	lua_settop(L, 1);
	arg.log_ud = L;
	arg.max_buffer = getint(L, "buffer");
	arg.max_layout = getint(L, "layout");
	arg.max_target = getint(L, "target");
	arg.max_texture = getint(L, "texture");
	arg.max_shader = getint(L, "shader");
	if (lua_getfield(L, 1, "log") == LUA_TFUNCTION) {
		lua_setfield(L, LUA_REGISTRYINDEX, "ejoy3d_log");
		log_setcallback(log_callback);
	} else {
		lua_pop(L, 1);
	}

	int sz = render_size(&arg);
	void * buffer = lua_newuserdata(L, sz);
	struct render * R = render_init(&arg, buffer, sz);
	if (R == NULL) {
		return luaL_error(L, "init render device failed");
	}

	return 1;
}

static int
lsetdepth(lua_State *L) {
	struct render *R = render(L);
	int d = luaL_optinteger(L, 2, 0);
	render_setdepth(R, d);

	return 0;
}

static int
ldepthmask(lua_State *L) {
	struct render *R = render(L);
	int d = lua_toboolean(L, 2);
	render_enabledepthmask(R, d);

	return 0;
}

static int
lsetcull(lua_State *L) {
	struct render *R = render(L);
	int c = luaL_optinteger(L, 2, 0);
	render_setcull(R, c);

	return 0;
}

static int
lsetblend(lua_State *L) {
	struct render *R = render(L);
	if (lua_gettop(L) == 1) {
		render_setblend(R, BLEND_DISABLE, BLEND_DISABLE);
	} else {
		int a = luaL_checkinteger(L, 2);
		int b = luaL_checkinteger(L, 3);
		render_setblend(R, a, b);
	}
	return 0;
}

static int
lexit(lua_State *L) {
	struct render *R = render(L);
	render_exit(R);

	return 0;
}

static int
lset(lua_State *L) {
	struct render *R = render(L);
	int what = luaL_checkinteger(L, 2);
	RID id = luaL_checkinteger(L, 3);
	int slot = luaL_optinteger(L, 4, 0);
	render_set(R, what, id, slot);

	return 0;
}

static int
lrelease(lua_State *L) {
	struct render *R = render(L);
	int what = luaL_checkinteger(L, 2);
	RID id = luaL_checkinteger(L, 3);
	render_release(R, what, id);
	return 0;
}

static int
lregister_layout(lua_State *L) {
	struct render *R = render(L);
	luaL_checktype(L, 2, LUA_TTABLE);
	int n = lua_rawlen(L, 2);
	ARRAY(struct vertex_attrib, va, n);
	int i;
	for (i=0;i<n;i++) {
		if (lua_rawgeti(L, 2, i+1) != LUA_TTABLE) {
			return luaL_error(L, "Index %d of argument 2 should be a table", i);
		}
		struct vertex_attrib *v = &va[i];
		v->name = getstring(L, "name");
		v->vbslot = getint(L, "vbslot");
		v->n = getint(L, "n");
		v->size = getint(L, "size");
		v->offset = getint(L, "offset");
	}
	RID id = render_register_vertexlayout(R, n, va);
	if (id == 0) {
		return luaL_error(L, "Register layout failed");
	}
	lua_pushinteger(L, id);

	return 1;
}

static int
lshader_create(lua_State *L) {
	struct render *R = render(L);
	luaL_checktype(L, 2, LUA_TTABLE);
	struct shader_init_args arg;
	arg.vs = getstring(L, "vs");
	arg.fs = getstring(L, "fs");
	arg.texture = 0;
	if (lua_getfield(L, -1, "texture") == LUA_TTABLE) {
		arg.texture = lua_rawlen(L, -1);
		if (arg.texture == 0) {
			lua_pop(L, 1);
		}
	} else {
		lua_pop(L, 1);
	}
	ARRAY(const char *, texture_uniform, arg.texture);
	if (arg.texture) {
		arg.texture_uniform = texture_uniform;
		int i;
		for (i=0;i<arg.texture;i++) {
			if (lua_rawgeti(L, -1, i+1) != LUA_TSTRING) {
				return luaL_error(L, "texture uniform %d should be string", i+1);
			}
			texture_uniform[i] = lua_tostring(L, -1);
			lua_pop(L, 1);
		}
		lua_pop(L, 1);
	}
	RID id = render_shader_create(R, &arg);
	if (id == 0) {
		return luaL_error(L, "Create shader failed");
	}
	lua_pushinteger(L, id);
	return 1;
}

static int
ltexture_create(lua_State *L) {
	struct render *R = render(L);
	int width = luaL_checkinteger(L,2);
	int height = luaL_checkinteger(L,3);
	int format = luaL_optinteger(L, 4, TEXTURE_RGBA8);
	int cubmap = lua_toboolean(L, 5);
	int mipmap = lua_toboolean(L, 6);
	RID id = render_texture_create(R, width, height, format, cubmap ? TEXTURE_CUBE: TEXTURE_2D, mipmap);
	if (id == 0) {
		return luaL_error(L, "Can't create texture (width=%d height=%d format=%d mipmap=%d)",
			width, height, format, mipmap);
	}
	const char * param = lua_tostring(L, 7);
	// l : MIN LINEAR, L : MAG LINEAR
	// n : MIN NEAREST, N : MAG NEAREST
	// c : S CLAMP_TO_EDGE , C : T CLAMP_TO_EDGE
	// r : S REPEAT, R T REPEAT
	// m : S MIRROR_REPEAT, M T MIRROR_REPEAT
	if (param) {
		int i;
		for (i=0;param[i];i++) {
			switch(param[i]) {
			case 'l':
				render_texture_setparam(R, id, TEXTURE_MIN_FILTER, TEXTURE_LINEAR);
				break;
			case 'L':
				render_texture_setparam(R, id, TEXTURE_MAG_FILTER, TEXTURE_LINEAR);
				break;
			case 'n':
				render_texture_setparam(R, id, TEXTURE_MIN_FILTER, TEXTURE_NEAREST);
				break;
			case 'N':
				render_texture_setparam(R, id, TEXTURE_MAG_FILTER, TEXTURE_NEAREST);
				break;
			case 'c':
				render_texture_setparam(R, id, TEXTURE_WRAP_S, TEXTURE_CLAMP_TO_EDGE);
				break;
			case 'C':
				render_texture_setparam(R, id, TEXTURE_WRAP_T, TEXTURE_CLAMP_TO_EDGE);
				break;
			case 'r':
				render_texture_setparam(R, id, TEXTURE_WRAP_S, TEXTURE_REPEAT);
				break;
			case 'R':
				render_texture_setparam(R, id, TEXTURE_WRAP_T, TEXTURE_REPEAT);
				break;
			case 'm':
				render_texture_setparam(R, id, TEXTURE_WRAP_S, TEXTURE_MIRRORED_REPEAT);
				break;
			case 'M':
				render_texture_setparam(R, id, TEXTURE_WRAP_T, TEXTURE_MIRRORED_REPEAT);
				break;
			default:
				return luaL_error(L, "Texture param invalid %s", param);
			}
		}
	}
	lua_pushinteger(L, id);
	return 1;
}

static const void *
topointer(lua_State *L, int index) {
	if (lua_isuserdata(L, index)) {
		return lua_touserdata(L, index);
	} else if (lua_isstring(L, index)) {
		return lua_tostring(L, index);
	} else {
		luaL_argerror(L, index, "Need string or userdata");
		return NULL;
	}
}

//void render_texture_update(struct render *R, RID id, int width, int height, const void *pixels, int slice, int miplevel);
static int
ltexture_update(lua_State *L) {
	struct render *R = render(L);
	RID id = luaL_checkinteger(L, 2);
	int width = luaL_checkinteger(L, 3);
	int height = luaL_checkinteger(L, 4);
	const void * pixels = topointer(L, 5);
	int slice = luaL_optinteger(L, 6, 0);
	int mipmap = lua_toboolean(L, 7);
	render_texture_update(R, id, width, height, pixels, slice, mipmap);

	return 0;
}

static int
lbuffer_create(lua_State *L) {
	struct render *R = render(L);
	enum RENDER_OBJ what = luaL_checkinteger(L, 2);
	const void * data = NULL;
	if (!lua_isnil(L, 3)) {
		data = topointer(L, 3);
	}
	int n = luaL_checkinteger(L, 4);
	int stride = luaL_checkinteger(L, 5);
	
	RID id = render_buffer_create(R, what, data, n, stride);
	if (id == 0) {
		return luaL_error(L, "Create buffer failed");
	}
	lua_pushinteger(L, id);

	return 1;
}

static int
lbuffer_update(lua_State *L) {
	struct render *R = render(L);
	RID id = luaL_checkinteger(L, 2);
	const void * data = topointer(L, 3);
	int n = luaL_checkinteger(L, 4);
	render_buffer_update(R, id, data, n);

	return 0;
}

static int
lclear(lua_State *L) {
	struct render *R = render(L);
	const char * mask = luaL_checkstring(L, 2);
	int maskbits = 0;
	int i;
	for (i=0;mask[i];i++) {
		switch (mask[i]) {
		case 'c':
			maskbits |= MASKC;
			break;
		case 'd':
			maskbits |= MASKD;
			break;
		case 's':
			maskbits |= MASKS;
			break;
		}
	}
	unsigned long argb = (unsigned long)luaL_optinteger(L, 3, 0);
	render_clear(R, maskbits, argb);

	return 0;
}

static int
ldraw(lua_State *L) {
	struct render *R = render(L);
	int from = luaL_checkinteger(L, 2);
	int n = luaL_checkinteger(L, 3);
	render_draw(R, DRAW_TRIANGLE, from, n);

	return 0;
}

static int
lviewport(lua_State *L) {
	struct render *R = render(L);
	int x = luaL_checkinteger(L, 2);
	int y = luaL_checkinteger(L, 3);
	int w = luaL_checkinteger(L, 4);
	int h = luaL_checkinteger(L, 5);

	render_setviewport(R, x,y,w,h);

	return 0;
}

static int
lbind(lua_State *L) {
	struct render *R = render(L);
	RID id = luaL_checkinteger(L, 2);
	render_shader_bind(R, id);

	return 0;
}

static int
llocuniform(lua_State *L) {
	struct render *R = render(L);
	const char *name = luaL_checkstring(L,2);
	int loc = render_shader_locuniform(R, name);
	if (loc >= 0) {
		lua_pushinteger(L, loc);
		return 1;
	} else {
		return 0;
	}
}

static int
lsetuniform_float(lua_State *L) {
	enum UNIFORM_FORMAT format;
	int top = lua_gettop(L);
	switch(top) {
	case 3:
		format = UNIFORM_FLOAT1;
		break;
	case 4:
		format = UNIFORM_FLOAT2;
		break;
	case 5:
		format = UNIFORM_FLOAT3;
		break;
	case 6:
		format = UNIFORM_FLOAT4;
		break;
	default:
		return luaL_error(L, "Only support 1,2,3,4 floats");
	}
	struct render *R = render(L);
	int loc = luaL_checkinteger(L, 2);
	float v[4];
	int i;
	for (i=3;i<=top;i++) {
		v[i-3] = luaL_checknumber(L, i);
	}
	render_shader_setuniform(R, loc, format, v);

	return 0;
}

static int
lsetuniform_vector3(lua_State *L) {
	struct render *R = render(L);
	int loc = luaL_checkinteger(L, 2);
	float *v = lua_touserdata(L,3);
	render_shader_setuniform(R, loc, UNIFORM_FLOAT3, v);
	return 0;
}

static int
lsetuniform_vector4(lua_State *L) {
	struct render *R = render(L);
	int loc = luaL_checkinteger(L, 2);
	float *v = lua_touserdata(L,3);
	render_shader_setuniform(R, loc, UNIFORM_FLOAT4, v);
	return 0;
}

static int
lsetuniform_matrix(lua_State *L) {
	struct render *R = render(L);
	int loc = luaL_checkinteger(L, 2);
	float *v = lua_touserdata(L,3);
	render_shader_setuniform(R, loc, UNIFORM_FLOAT44, v);
	return 0;
}

static int
lsetuniform_matrix33(lua_State *L) {
	struct render *R = render(L);
	int loc = luaL_checkinteger(L, 2);
	union matrix44 *mat = lua_touserdata(L,3);
	float n33[9];
	render_shader_setuniform(R, loc, UNIFORM_FLOAT33, matrix44_to33(mat, n33));
	return 0;
}

static int
lresetstate(lua_State *L) {
	struct render *R = render(L);
	render_state_reset(R);

	return 0;
}

static int
lscissor(lua_State *L) {
	struct render *R = render(L);
	if (lua_isboolean(L, 2)) {
		int e = lua_toboolean(L, 2);
		render_enablescissor(R, e);
	} else {
		int x = luaL_checkinteger(L, 2);
		int y = luaL_checkinteger(L, 3);
		int w = luaL_checkinteger(L, 4);
		int h = luaL_checkinteger(L, 5);
		render_setscissor(R, x, y, w, h);
	}
	return 0;
}

static int
ltarget_create(lua_State *L) {
	struct render *R = render(L);
	int width = luaL_checkinteger(L, 2);
	int height = luaL_checkinteger(L, 3);
	int format = luaL_checkinteger(L, 4);
	RID targetid = render_target_create(R, width, height, format);
	if (targetid == 0) {
		return luaL_error(L, "Create render targer %d * %d (%d) failed", width, height, format);
	}
	RID tex = render_target_texture(R, targetid);
	lua_pushinteger(L, targetid);
	lua_pushinteger(L, tex);

	return 2;
}

int
luaopen_ejoy3d_render(lua_State *L) {
	luaL_Reg l[] = {
		{ "init", linit },
		{ "exit", lexit },
		{ "set", lset },
		{ "release", lrelease },
		{ "register_layout", lregister_layout },
		{ "shader_create", lshader_create },
		{ "texture_create", ltexture_create },
		{ "texture_update", ltexture_update },
		{ "buffer_create", lbuffer_create },
		{ "buffer_update", lbuffer_update },
		{ "target_create", ltarget_create },
		{ "clear", lclear },
		{ "draw", ldraw },
		{ "viewport", lviewport },
		{ "bind", lbind },
		{ "locuniform", llocuniform },
		{ "setuniform_float", lsetuniform_float },
		{ "setuniform_vector3", lsetuniform_vector3 },
		{ "setuniform_vector4", lsetuniform_vector4 },
		{ "setuniform_matrix33", lsetuniform_matrix33 },
		{ "setuniform_matrix", lsetuniform_matrix },
		{ "setdepth", lsetdepth },
		{ "depthmask", ldepthmask },
		{ "setcull", lsetcull },
		{ "setblend", lsetblend },
		{ "resetstate", lresetstate },
		{ "scissor", lscissor },
		{ NULL, NULL },
	};

	luaL_checkversion(L);
	luaL_newlib(L, l);

	return 1;
}
