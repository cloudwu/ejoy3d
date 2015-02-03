CSRC=\
 carray.c\
 log.c\
 render.c

LUASRC=\
 librender.c\
 libglfw.c

GLAD=\
 ../3rd/glad/src/glad.c

GLADINC=\
 -I../3rd/glad/include

LUALIB=-L/usr/local/bin -llua53

CFLAGS=-g -I/usr/local/include -Wall $(GLADINC)
LDFLAGS=-L/usr/local/lib -lglfw3 -lopengl32 -lgdi32 $(LUALIB)

ejoy3d.dll : $(CSRC) $(LUASRC) $(GLAD)
	gcc $(CFLAGS) --shared -o $@ $^ $(LDFLAGS)

test : test.c log.c
	gcc -Wall -o $@ $^ -I/usr/local/include -L/usr/local/lib -lglfw3 -lopengl32 -lgdi32

