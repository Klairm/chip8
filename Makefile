OBJS = chip8.c
CC = gcc
C_FLAGS = -w
L_FLAGS = -lSDL
OBJ_NAME = chip8

all: $(OBJS)
	$(CC) $(OBJS) $(C_FLAGS) $(L_FLAGS) -o $(OBJ_NAME) 

