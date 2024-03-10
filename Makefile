all:
	g++ -I lib/sdl/include -I lib/sdl_ttf/include -L lib/sdl/lib -L lib/sdl_ttf/lib -o main main.cpp -l mingw32 -l SDL2main -l SDL2 -lSDL2_ttf