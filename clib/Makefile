all : ej3d.exe

CSRC=\
 carray.c\
 log.c\
 render.c

LUASRC=\
 librender.c\
 libglfw.c\
 libmath.c

GLAD=\
 ../3rd/glad/src/glad.c

GLADINC=\
 -I../3rd/glad/include

LUALIB=-L/usr/local/bin -llua53
LUASTATIC=-L/usr/local/lib -llua

CFLAGS=-g -I/usr/local/include -Wall $(GLADINC)
LDFLAGS=-L/usr/local/lib -lglfw3 -lopengl32 -lgdi32

ejoy3d.dll : $(CSRC) $(LUASRC) $(GLAD)
	gcc $(CFLAGS) --shared -o $@ $^ $(LDFLAGS) $(LUALIB)

ej3d.exe : $(CSRC) $(LUASRC) $(GLAD) standalone.c
	gcc $(CFLAGS) -I../include -o $@ $^ $(LDFLAGS) $(LUASTATIC)

clean :
	rm -f ejoy3d.dll ej3d.exe

