OBJS = src/chip8.c
CC = gcc
C_FLAGS = -w
L_FLAGS = -lSDL2
OBJ_NAME = chip8

all: $(OBJS)
	$(CC) $(OBJS) $(C_FLAGS) $(L_FLAGS) -o $(OBJ_NAME) 

