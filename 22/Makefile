cflags := $(shell pkg-config --cflags freetype2) -I/home/ryan/adventofcode/2020/
ldflags := $(shell pkg-config --libs freetype2) -lglad -lglfw -lstb_image -ldl

glcombat: main.cpp card.hpp /home/ryan/adventofcode/2020/opengl/texture.hpp /home/ryan/adventofcode/2020/opengl/character.hpp /home/ryan/adventofcode/2020/opengl/mesh.hpp
	g++ main.cpp $(cflags) -D USEGL -o glcombat $(ldflags)

combat2: main.cpp
	g++ main.cpp -o combat2
