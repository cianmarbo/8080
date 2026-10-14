SDL_CFLAGS := -I$(shell sdl2-config --prefix)/include $(shell sdl2-config --cflags)
SDL_LIBS   := $(shell sdl2-config --libs)

ifeq ($(shell uname -s),Darwin)
GL_LIBS := -framework OpenGL
endif

build:
	gcc -std=c99 -Wall $(SDL_CFLAGS) -o 8080 src/*.c $(SDL_LIBS) $(GL_LIBS)

exec:
	./8080

clean:
	rm 8080
