CC = gcc
LFLAGS = -Wall -lm
CFLAGS = -Wall -c -DDEBUG -DPX -g -march=native
OBJ = bin/objs/ppm.o bin/objs/image.o bin/objs/image_fast.o bin/objs/compintrin.o
C11 = -std=c11

all : clean ppm.o image.o image_fast.o compintrin.o sharpen.o combined.o
	$(CC) $(LFLAGS) $(OBJ) bin/objs/sharpen.o -o bin/sharpen
	$(CC) $(LFLAGS) $(OBJ) bin/objs/combined.o -o bin/combined

sharpen.o :
	$(CC) $(CFLAGS) $(C11) demos/sharpen/main.c -o bin/objs/sharpen.o
	
combined.o :
	$(CC) $(CFLAGS) $(C11) demos/combined/main.c -o bin/objs/combined.o

ppm.o :
	$(CC) $(CFLAGS) $(C11) src/ppm.c -o bin/objs/ppm.o

image.o :
	$(CC) $(CFLAGS) $(C11) src/image.c -o bin/objs/image.o

image_fast.o :
	$(CC) $(CFLAGS) $(C11) src/image_fast.c -o bin/objs/image_fast.o

compintrin.o :
	$(CC) $(CFLAGS) $(C11) src/compintrin.c -o bin/objs/compintrin.o
	
clean :
	rm -rf bin
	mkdir bin
	mkdir bin/objs
