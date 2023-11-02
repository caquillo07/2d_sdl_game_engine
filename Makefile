CCFLAGS = $$(pkg-config --cflags lua SDL2_ttf)
LDFLAGS = $$(pkg-config --libs lua)

build:
	g++ -Wall -std=c++17 -I"./libs/" $(CCFLAGS) src/*.cpp -lSDL2 -lSDL2_image -lSDL2_mixer $(LDFLAGS) -o game_engine

run: build
	./game_engine

clean:
	rm game_engine

