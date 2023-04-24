LIBS = -lSDL2 -lm
CFLAGS = -Wall -Wpedantic -march=native -mtune=native -O3
CC = gcc
OBJECTS = src/main.o src/3D/camera.o src/3D/material.o src/3D/model.o src/3D/object.o src/3D/renderer.o src/game.o src/scene.o src/road.o src/player.o src/npc.o src/entity.o src/prop.o src/text.o src/ui.o

%.o: %.c %.h
	$(CC) -c $(CFLAGS) $< -o $@

spyhunter: $(OBJECTS)
	$(CC) $(OBJECTS) $(STATIC_LIBS) $(LIBS) -o $@ 

.PHONY: clean run

clean:
	-rm $(OBJECTS)
	-rm spyhunter

run: spyhunter
	./spyhunter
