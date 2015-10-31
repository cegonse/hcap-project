CC = gcc
LFLAGS = -Wall -lm
CFLAGS = -Wall -c -DDEBUG -DPX -g -march=native
OBJ = bin/objs/ppm.o bin/objs/image.o bin/objs/image_fast.o
C11 = -std=c11

all : clean ppm.o image.o image_fast.o sharpen.o
	$(CC) $(LFLAGS) $(OBJ) bin/objs/sharpen.o -o bin/sharpen

sharpen.o :
	$(CC) $(CFLAGS) $(C11) demos/sharpen/main.c -o bin/objs/sharpen.o

ppm.o :
	$(CC) $(CFLAGS) $(C11) src/ppm.c -o bin/objs/ppm.o

image.o :
	$(CC) $(CFLAGS) $(C11) src/image.c -o bin/objs/image.o

image_fast.o :
	$(CC) $(CFLAGS) $(C11) src/image_fast.c -o bin/objs/image_fast.o
	
clean :
	rm -rf bin
	mkdir bin
	mkdir bin/objs
