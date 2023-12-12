CC=g++
LANG_STD=-std=c++17
INCLUDES=-I"./libs/" src/*.cpp src/**/*.cpp
CCFLAGS = -Wall -Wfatal-errors $$(pkg-config --cflags lua SDL2_ttf)
#CCFLAGS = -Wall $$(pkg-config --cflags lua SDL2_ttf)
LDFLAGS = $$(pkg-config --libs lua) -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf
BIN=build/game_engine

.PHONY: build

build:
	$(CC) $(LANG_STD) $(CCFLAGS) $(INCLUDES) $(LDFLAGS) -o $(BIN)

run: build
	./$(BIN)

clean:
	rm -rf build/*
