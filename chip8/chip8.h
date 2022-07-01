#ifndef CHIP_8
#define CHIP_8

#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <stdint.h>
#include <dirent.h>

#include <orbis/Keyboard.h>
#include <orbis/libkernel.h>

#include "log.h"
#include "notifi.h"
#include "keyboard.h"
#include "graphics.h"
#include "controller.h"
#include "print.h"

#define MEMSIZE 4096
#define WIDTH 640
#define HEIGHT 320
std::stringstream debugLogStream;

// CHIP8

uint16_t opcode;
uint8_t memory[(MEMSIZE)];
uint8_t v[16];
uint16_t I;
uint16_t PC;

uint8_t gfx[64 * 32];
uint8_t chip8_fontset[80] =
	{
		0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		0x20, 0x60, 0x20, 0x20, 0x70, // 1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

uint8_t delay_timer;
uint8_t sound_timer;

uint16_t stack[16];
uint16_t sp;

uint8_t keyboard[16];

uint_fast8_t drawflag;

void initChip8();
void execute();
void draw();
uint32_t loadRom(char *file);

// SDL

SDL_Renderer *renderer;
SDL_Window *window;
SDL_Texture *screen;
SDL_Event event;

int initSDL();

void CleanUp_SDL();

// File selection menu

DIR *p;
struct dirent *pp;
int arrowY = 200;
int pos = 0;

char filenames[50][50];
char listedFiles[50];
char selectedRom[50];
int fileCount = 0;

void listFiles();
void arrowMenu(int realPos, int arrowPos, int Y);
void getFiles();

// sceVideo , used for the file selection menu
#define FRAME_WIDTH 1920
#define FRAME_HEIGHT 1080
#define FRAME_DEPTH 4

int frameID = 0;
Scene2D *scene;

Color red = {255, 0, 0};
Color black = {0, 0, 0};

// Other
uint8_t keys[32];

#endif
