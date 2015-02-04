BUILD=build
STANDALONE=ej3d.exe

CFLAGS=-g -Wall
CC=gcc
AR=ar rc

CSRC=\
 carray.c\
 log.c\
 render.c

LUASRC=\
 librender.c\
 libglfw.c\
 libmath.c

GLAD=\
 3rd/glad/src/glad.c

LUAINC=/usr/local/include

LIBSRCS:= $(CSRC) $(LUASRC)
LIBOBJS:= $(addprefix $(BUILD)/,$(LIBSRCS:.c=.o))
LIBS:=-L/usr/local/lib -lglfw3 -llua -lopengl32 -lgdi32

.PHONY : all clean

all : $(BUILD)/$(STANDALONE)

$(BUILD) :
	mkdir $@

$(LIBOBJS): $(BUILD)/%.o : clib/%.c | $(BUILD)
	$(CC) $(CFLAGS) -I$(LUAINC) -I3rd/glad/include -c -o $@ $<

$(BUILD)/glad.o : 3rd/glad/src/glad.c | $(BUILD)
	$(CC) $(CFLAGS) -I3rd/glad/include -c -o $@ $<

$(BUILD)/libejoy3d.a : $(LIBOBJS) $(BUILD)/glad.o
	$(AR) $@ $^

$(BUILD)/$(STANDALONE) : SRCS=clib/standalone.c
$(BUILD)/$(STANDALONE) : $(BUILD)/libejoy3d.a
	$(CC) $(CFLAGS) -I$(LUAINC) -Iinclude -o $@ $(SRCS) -L$(BUILD) -lejoy3d $(LIBS)

clean :
	rm -rf build
