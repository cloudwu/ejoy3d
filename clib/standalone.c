#include <stdio.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include "ejoyclib.h"

static void
error_message(const char *prog, const char *msg) {
	fprintf(stderr, "%s : %s\n", prog, msg);
}

static void
require_ejoy3d(lua_State *L) {
	luaL_requiref(L, "ejoy3d.render", luaopen_ejoy3d_render,0);
	luaL_requiref(L, "ejoy3d.glfw", luaopen_ejoy3d_glfw,0);
	luaL_requiref(L, "ejoy3d.math3d", luaopen_ejoy3d_math3d,0);
}

static void
disable_cloader(lua_State *L) {
	lua_getglobal(L, "package");
	lua_pushnil(L);
	lua_setfield(L, -2, "cpath");
	lua_getfield(L, -1, "searchers");
	lua_pushnil(L);
	lua_rawseti(L, -2, 4);
	lua_pushnil(L);
	lua_rawseti(L, -2, 3);
	lua_pop(L, 2);
}

static int
pmain (lua_State *L) {
	int argc = (int)lua_tointeger(L, 1);
	char **argv = (char **)lua_touserdata(L, 2);
	if (argc <= 1) {
		return luaL_error(L, "Need filename");
	}
	luaL_openlibs(L);
	disable_cloader(L);
	require_ejoy3d(L);
	lua_createtable(L, argc - 2, 2);
	int i;
	for (i=0;i<=argc;i++) {
		lua_pushstring(L, argv[i]);
		lua_rawseti(L, -2, i-1);
	}
	lua_setglobal(L, "arg");
	return luaL_dofile(L, argv[1]);
}

int
main(int argc, char *argv[]) {
	int status;
	lua_State *L = luaL_newstate();  /* create state */
	if (L == NULL) {
		error_message(argv[0], "cannot create state: not enough memory");
		return 1;
	}
	lua_pushcfunction(L, &pmain);  /* to call 'pmain' in protected mode */
	lua_pushinteger(L, argc);  /* 1st argument */
	lua_pushlightuserdata(L, argv); /* 2nd argument */
	status = lua_pcall(L, 2, 0, 0);  /* do the call */
	if (status != LUA_OK) {
		const char *msg = lua_tostring(L, -1);
		error_message(argv[0], msg);
		return 1;
	}
	lua_close(L);
	return (status == LUA_OK) ? 0 : 1;
}

