CC=g++
CFLAGS=-Wall
LIBS=-lSDL2 -lGL
INCLUDES=-I/home/mevil/OpenGL/Dependencies/include

SRC_FILES=src/main.cpp src/glad.c
OUTPUT=main

all: ${OUTPUT}

${OUTPUT}: ${SRC_FILES} 
	$(CC) $(CFLAGS) ${INCLUDES} $^ -o $@ ${LIBS}


