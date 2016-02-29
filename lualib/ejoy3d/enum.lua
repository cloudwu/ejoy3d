local E = {}

E.type = {
	VERTEXLAYOUT = 1,
	VERTEXBUFFER = 2,
	INDEXBUFFER = 3,
	TEXTURE = 4,
	TARGET = 5,
	SHADER = 6,
}

E.texture = {
	RGBA8 = 1,
	RGBA4 = 2,
	RGB = 3,
	RGB565 = 4,
	A8 = 5,
	DEPTH = 6,
	PVR2 = 7,
	PVR4 = 8,
	ETC1 = 9,
}

E.cull = {
	DISABLE = 0,
	FRONT = 1,
	BACK = 2,
}

E.depth = {
	DISABLE = 0,
	LESS_EQUAL = 1,
	LESS = 2,
	EQUAL = 3,
	GREATER = 4,
	GREATER_EQUAL = 5,
	ALWAYS = 6,
}

return E
