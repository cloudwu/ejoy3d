#ifndef ejoy3d_lua_entry_h
#define ejoy3d_lua_entry_h

#include <lua.h>

int luaopen_ejoy3d_render(lua_State *L);
int luaopen_ejoy3d_glfw(lua_State *L);
int luaopen_ejoy3d_math3d(lua_State *L);

#endif
