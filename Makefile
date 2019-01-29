CC     = gcc
CFLAGS = -g3 -std=c99 -pedantic -Wall
DEPS   = headers/raytrace.h
OBJ    = raytrace.o bmp.o vec.o scene.o sim.o main.o
SRC    = src

%.o: $(SRC)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

Raytrace: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
	rm -f *.o
