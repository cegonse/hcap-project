CC = gcc
LFLAGS = -Wall -lm
CFLAGS = -Wall -c -DDEBUG -DPOSIX -DC11 -g -std=c11 -march=native
OBJ = bin/ppm.o bin/image.o bin/image_fast.o

all : main.o ppm.o image.o image_fast.o
	$(CC) $(LFLAGS) $(OBJ) bin/main.o -o bin/sharpen

main.o :
	$(CC) $(CFLAGS) src/main.c -o bin/main.o

ppm.o :
	$(CC) $(CFLAGS) src/ppm.c -o bin/ppm.o

image.o :
	$(CC) $(CFLAGS) src/image.c -o bin/image.o

image_fast.o :
	$(CC) $(CFLAGS) src/image_fast.c -o bin/image_fast.o
	
clean :
	rm bin/*
