#include <stdio.h>
#include "glad/glad.h"
#include <glfw/glfw3.h>
#include <lua.h>
#include <lauxlib.h>

static int
lVersion(lua_State *L) {
	float v = GLVersion.major + (float)GLVersion.minor / 10.0f;
	lua_pushnumber(L, v);

	return 1;
}

static int
lInit(lua_State *L) {
	if (!glfwInit()) {
		return luaL_error(L, "glfwInit failed");
	}
	return 0;
}

static int
lTerminate(lua_State *L) {
	glfwTerminate();
	return 0;
}

static void
error_callback(int error, const char* description) {
	fputs(description, stderr);
}

static int
lSetErrorCallback(lua_State *L) {
	glfwSetErrorCallback(error_callback);

	return 0;
}

static int
lCreateWindow(lua_State *L) {
	int w = luaL_checkinteger(L, 1);
	int h = luaL_checkinteger(L, 2);
	const char * title = luaL_checkstring(L, 3);
	GLFWwindow * window = glfwCreateWindow(w,h,title, NULL, NULL);
	if (window == NULL)
		return luaL_error(L, "glfwCreateWindow failed");
	if (!lua_pushthread(L)) {
		return luaL_error(L, "Should be call in main thread");
	}
	glfwSetWindowUserPointer(window, L);
	lua_pushlightuserdata(L, window);

	return 1;
}

static GLFWwindow *
getwindow(lua_State *L) {
	GLFWwindow * window = lua_touserdata(L, 1);
	if (window == NULL) {
		luaL_error(L, "Need GLFWwindow");
		// never here
	}
	void * p = glfwGetWindowUserPointer(window);
	if (p != L) {
		luaL_error(L, "Invalid window or Invalid call out side of main thread");
		// never here
	}
	return window;
}

static int
lMakeContextCurrent(lua_State *L) {
	GLFWwindow * window = getwindow(L);
	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
		return luaL_error(L, "Can't load GL");
	}

	return 0;
}

static int
lSwapInterval(lua_State *L) {
	int v = luaL_checkinteger(L, 1);
	glfwSwapInterval(v);

	return 0;
}

static int
lWindowShouldClose(lua_State *L) {
	GLFWwindow * window = getwindow(L);
	lua_pushboolean(L, glfwWindowShouldClose(window));

	return 1;
}

static int
lSwapBuffers(lua_State *L) {
	GLFWwindow * window = getwindow(L);
	glfwSwapBuffers(window);

	return 0;
}

static int
lPollEvents(lua_State *L) {
	glfwPollEvents();

	return 0;
}

static int
lDestroyWindow(lua_State *L) {
	GLFWwindow * window = getwindow(L);
	glfwDestroyWindow(window);

	return 0;
}

int
luaopen_ejoy3d_glfw(lua_State *L) {
	luaL_Reg l[] = {
		{ "Version", lVersion },
		{ "Init", lInit },
		{ "Terminate", lTerminate },
		{ "SetErrorCallback", lSetErrorCallback },
		{ "CreateWindow", lCreateWindow },
		{ "MakeContextCurrent", lMakeContextCurrent },
		{ "SwapInterval", lSwapInterval },
		{ "WindowShouldClose", lWindowShouldClose },
		{ "SwapBuffers", lSwapBuffers },
		{ "PollEvents", lPollEvents },
		{ "DestroyWindow", lDestroyWindow },
		{ NULL, NULL },
	};
	luaL_checkversion(L);
	luaL_newlib(L, l);

	return 1;
}
