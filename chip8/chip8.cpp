#include "chip8.h"
#include "notifi.h"
#include <orbis/Keyboard.h>
#include <orbis/libkernel.h>
#include <dirent.h>

#define FRAME_WIDTH 1920
#define FRAME_HEIGHT 1080
#define FRAME_DEPTH 4

void initChip8();
int initSDL();
void draw();
void execute();
void CleanUp_SDL();
void listFiles();
void arrowMenu(int realPos, int arrowPos, int Y);
void getFiles();

uint32_t loadRom(char *file);
uint32_t quit = 0;
SDL_Renderer *renderer;
SDL_Window *window;
SDL_Texture *screen;
SDL_Event event;
int frameID = 0;
int Y = 200;
int pos = 0;
int quitMain = 0;
int fileCount = 0;
DIR *p;
struct dirent *pp;

Scene2D *scene;
Color red = {255, 0, 0};
char filenames[50][50];
char selectedRom[35];
int main(int argc, char **argv)
{

	// No buffering
	setvbuf(stdout, NULL, _IONBF, 0);

	scene = new Scene2D(FRAME_WIDTH, FRAME_HEIGHT, FRAME_DEPTH);
	if (!scene->Init(0xC000000, 2))
	{
		DEBUGLOG << "[ERROR] Can't init scene2D!!";
	}
	auto sceKeyboard = new Keyboard();
	if (!sceKeyboard->Init(-1))
	{

		DEBUGLOG << "[ERROR] Can't init SceKeyboard!!";
	}

	auto controller = new Controller();
	if (!controller->Init(-1))
	{
		DEBUGLOG << "[ERROR] Can't init the controller!!";
	}

	if (!scene->InitFont(&fontSize, font, FONT_SIZE))
	{
		DEBUGLOG << "Failed to initialize large font '" << font << "'";
	}
	getFiles();

	while (!quitMain)
	{
		scene->FrameBufferFill({0, 0, 0});

		if (controller->DpadDownPressed())
		{

			if (pos > fileCount)
			{

				Y = (200 + (fileCount * 40));
				pos = fileCount;
			}
			else
			{
				pos++;
				Y += 40;
			}
		}
		else if (controller->DpadUpPressed())
		{
			if (pos < 0)
			{
				Y = 200;
				pos = 0;
			}
			else
			{
				pos--;
				Y -= 40;
			}
		}

		else if (controller->XPressed())
		{

			initChip8();
			sprintf(selectedRom, "/app0/assets/misc/%s", filenames[pos]);

			if (loadRom(selectedRom) != 0)
			{
				DEBUGLOG << "Something failed trying to load the ROM";

				return 1;
			}
			scene->FrameBufferFill({0, 0, 0});

			sceVideoOutClose(scene->videoID());
			free(scene);
			free(controller);
			quitMain = 1;
		}
		listFiles();

		scene->SubmitFlip(frameID);
		scene->FrameWait(frameID);
		scene->FrameBufferSwap();
		frameID++;
	}

	if (initSDL() != 0)
	{
		notifi(NULL, "%s", SDL_GetError());
		return 1;
	}

	uint8_t keys[32];
	memset(keys, 0, 32);

	while (!quit)
	{

		sceKeyboard->Poll();
		// thanks znullptr for this help
		for (int ix = 0; ix < 32; ix++)
		{
			uint8_t found = 0;
			for (int ik = 0; ik < (int)sizeof(sceKeyboard->data); ik++)
			{
				uint8_t kc = sceKeyboard->data.keycodes[ik];
				if (kc && ix == kc)
					found = true;
			}
			bool press = found && !(keys[ix]);
			bool release = !found && (keys[ix]);
			keys[ix] = found;

			if (press || release)
			{
				char key = (char)sceKeyboard->Key2Char(ix);
				// TODO: Optimize this shit
				// this is so disgusting and it deserves a special place in hell

				switch (key)
				{
				case 'x':
				case 'X':
					press ? keyboard[0] = 1 : keyboard[0] = 0;
					break;
				case '1':
					press ? keyboard[1] = 1 : keyboard[1] = 0;
					break;
				case '2':
					press ? keyboard[2] = 1 : keyboard[2] = 0;
					break;
				case 'g':
				case 'G':
					press ? keyboard[3] = 1 : keyboard[3] = 0;
					break;
				case 'T':
				case 't':

					press ? keyboard[0xC] = 1 : keyboard[0xC] = 0;
					break;
				case 'q':
				case 'Q':
					press ? keyboard[4] = 1 : keyboard[4] = 0;
					break;
				case 'w':
				case 'W':
					press ? keyboard[5] = 1 : keyboard[5] = 0;
					break;
				case 'e':
				case 'E':
					press ? keyboard[6] = 1 : keyboard[6] = 0;
					break;
				case 'a':
				case 'A':
					press ? keyboard[7] = 1 : keyboard[7] = 0;
					break;
				case 's':
				case 'S':
					press ? keyboard[8] = 1 : keyboard[8] = 0;
					break;
				case 'd':
				case 'D':
					press ? keyboard[9] = 1 : keyboard[9] = 0;
					break;
				case 'z':
				case 'Z':
					press ? keyboard[0xA] = 1 : keyboard[0xA] = 0;
					break;
				case 'c':
				case 'C':
					press ? keyboard[0xB] = 1 : keyboard[0xB] = 0;
					break;
				case 'r':
				case 'R':
					press ? keyboard[0xD] = 1 : keyboard[0xD] = 0;
					break;
				case 'f':
				case 'F':
					press ? keyboard[0xE] = 1 : keyboard[0xE] = 0;
					break;
				case 'v':
				case 'V':
					press ? keyboard[0xF] = 1 : keyboard[0xF] = 0;
					break;

				case 'K':
				case 'k':
					initChip8();
					if (loadRom(selectedRom) == 0)
					{
						CleanUp_SDL();
						return 0;
					}
					break;

				default:
					break;
				}
			}
		}

		SDL_Delay(1);

		if (delay_timer > 0)
		{
			--delay_timer;
		}
		execute();
		draw();
	}

	CleanUp_SDL();

	return 0;
}

void arrowMenu(int realPos, int arrowPos, int Y)
{

	char positionArrow[10];
	sprintf(positionArrow, "%d->", pos);

	if (realPos == arrowPos)
	{
		screenPrint(scene, positionArrow, Y, 90, red);
	}
	else
	{
		;
	}
}

void getFiles()
{

	p = opendir("/app0/assets/misc/");
	if (p != NULL)
	{

		int i = 0;

		while ((pp = readdir(p)) != NULL)
		{
			if (strcmp(pp->d_name, ".") == 0 || strcmp(pp->d_name, "..") == 0)
			{
				;
			}
			else
			{
				strncpy(filenames[i], pp->d_name, 35);
				i++;
				fileCount++;
			}
		}
	}

	closedir(p);
}
char listedFiles[50];
void listFiles()
{
	int Ylist = 200;

	for (int i = 0; i < fileCount; i++)
	{
		sprintf(listedFiles, "%d | %s", i, filenames[i]);
		screenPrint(scene, listedFiles, Ylist, 180, red);
		arrowMenu(i, pos, Y);
		Ylist += 40;
	}
}

// Initialize everything
void initChip8()
{
	delay_timer = 0;
	sound_timer = 0;
	opcode = 0;
	PC = 0x200;
	I = 0;
	sp = 0;
	memset(stack, 0, 16);
	memset(memory, 0, 4096);
	memset(v, 0, 16);
	memset(gfx, 0, 2048);
	memset(keyboard, 0, 16);
	// Load fonts
	memcpy(memory, chip8_fontset, 80 * sizeof(int8_t));
}

int initSDL()
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		DEBUGLOG << "[SDL ERROR] Failed to initialize SDL: " << SDL_GetError();
		return 1;
	}

	window = SDL_CreateWindow("CHIP-8", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, 0);
	if (window == NULL)
	{
		DEBUGLOG << "[SDL ERROR] Failed trying to create window: " << SDL_GetError();
		return 1;
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
	if (renderer == NULL)
	{
		DEBUGLOG << "[SDL ERROR] Failed trying to create renderer: " << SDL_GetError();
		return 1;
	}
	if (SDL_RenderSetLogicalSize(renderer, 64, 32) < 0)
	{
		DEBUGLOG << "[SDL ERROR] Failed trying to set logical rendering size: " << SDL_GetError();
		return 1;
	}
	if (SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255))
	{
		DEBUGLOG << "[SDL ERROR] Failed trying to set draw color rendering: " << SDL_GetError();
		return 1;
	}
	if (SDL_RenderClear(renderer))
	{
		DEBUGLOG << "[SDL ERROR] Failed trying to clear renderer: " << SDL_GetError();
		return 1;
	}

	screen = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);
	if (screen == NULL)
	{
		DEBUGLOG << "[SDL ERROR] Failed trying to create texture: " << SDL_GetError();
		return 1;
	}
	return 0;
}

// Load rom file into memory
uint32_t loadRom(char *file)
{
	FILE *fp = fopen(file, "rb");

	if (fp == NULL)
	{
		notifi(NULL, "%s", "[ERROR] Cannot open file rom!");

		return 1;
	}

	fseek(fp, 0, SEEK_END);
	int size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	fread(memory + 0x200, sizeof(uint16_t), size, fp);

	return 0;
}

// Draw function
void draw()
{

	SDL_Rect r;
	int x, y;
	r.x = 0;
	r.y = 0;
	r.w = 1;
	r.h = 1;

	if (drawflag)
	{
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 0);
		for (x = 0; x < 64; x++)
		{
			for (y = 0; y < 32; y++)
			{
				if (gfx[(x) + ((y)*64)] == 1)
				{
					r.x = x;
					r.y = y;
					SDL_RenderFillRect(renderer, &r);
				}
			}
		}
		SDL_RenderPresent(renderer);
	}
	drawflag = false;
}

// Emulate cycle
void execute()
{
	uint8_t X, Y, kk, n;
	uint16_t nnn;
	uint32_t i, key_pressed;

	opcode = memory[PC] << 8 | memory[PC + 1];
	PC += 2;
	X = (opcode & 0x0F00) >> 8;
	Y = (opcode & 0x00F0) >> 4;
	nnn = (opcode & 0x0FFF);
	kk = (opcode & 0x00FF);
	n = (opcode & 0x000F);

	switch (opcode & 0xF000)
	{
	case 0x0000:
	{

		switch (opcode & 0x00FF)
		{
		// 00E0
		case 0x00E0:
		{
			memset(gfx, 0, 2048);
			drawflag = true;
			break;
		}
		// 00E0
		case 0x00EE:
		{
			--sp;
			PC = stack[sp];
			break;
		}
		default:
			DEBUGLOG << "[ERROR|OPCODE] 0xxx -> " << opcode;
		}
		break;
	}

	// 1nnn
	case 0x1000:
	{
		PC = nnn;
		break;
	}
	// 2nnn
	case 0x2000:
	{
		stack[sp] = PC;
		++sp;
		PC = nnn;
		break;
	}
	// 3xkk
	case 0x3000:
	{
		if (v[X] == kk)
			PC += 2;
		break;
	}
	// 4xkk
	case 0x4000:
	{
		if (v[X] != kk)
			PC += 2;
		break;
	}
	// 5xy0
	case 0x5000:
	{
		if (v[X] == v[Y])
			PC += 2;
		break;
	}
	// 6xkk
	case 0x6000:
	{
		v[X] = kk;
		break;
	}
	// 7xkk
	case 0x7000:
	{
		v[(X)] += kk;
		break;
	}
	// 8xyn
	case 0x8000:
	{
		switch (n)
		{
		// 8xy0
		case 0x0000:
			v[X] = v[Y];
			break;
		// 8xy1
		case 0x0001:
			v[X] |= v[Y];
			break;
		// 8xy2
		case 0x0002:
			v[X] &= v[Y];
			break;
		// 8xy3
		case 0x0003:
			v[X] ^= v[Y];
			break;
		// 8xy4
		case 0x0004:
		{
			int i;
			i = (int)(v[X]) + (int)(v[Y]);
			if (i > 255)
				v[0xF] = 1;
			else
				v[0xF] = 0;
			v[X] = i & 0xFF;
		}
		break;
		// 8xy5
		case 0x0005:
			if (v[X] > v[Y])
				v[0xF] = 1;
			else
				v[0xF] = 0;
			v[X] -= v[Y];
			break;
		// 8xy6
		case 0x0006:
			v[0xF] = v[X] & 1;
			v[X] >>= 1;
			break;
		// 8xy7
		case 0x0007:
			if (v[Y] > v[X])
				v[0xF] = 1;
			else
				v[0xF] = 0;
			v[X] = v[Y] - v[X];
			break;
		// 8xyE
		case 0x000E:
			v[0xF] = v[X] >> 7;
			v[X] <<= 1;
			break;

		default:
			DEBUGLOG << "[ERROR|OPCODE] 8xxx -> " << opcode;
		}
		break;
	}

	// 9xy0
	case 0x9000:
		if (v[X] != v[Y])
			PC += 2;
		break;

	// Annn
	case 0xA000:
		I = nnn;
		break;

	// Bnnn
	case 0xB000:
		PC = (nnn) + v[0x0];
		break;

	// Cxkk
	case 0xC000:
		v[X] = (rand() % 0x100) & (kk);
		break;

	// Dxyn
	case 0xD000:
	{
		uint16_t x = v[X];
		uint16_t y = v[Y];
		uint16_t height = n;
		uint8_t pixel;

		v[0xF] = 0;
		for (int yline = 0; yline < height; yline++)
		{
			pixel = memory[I + yline];
			for (int xline = 0; xline < 8; xline++)
			{
				if ((pixel & (0x80 >> xline)) != 0)
				{
					if (gfx[(x + xline + ((y + yline) * 64))] == 1)
					{
						v[0xF] = 1;
					}
					gfx[x + xline + ((y + yline) * 64)] ^= 1;
				}
			}
		}
		drawflag = true;
		break;
	}
	// Exkk
	case 0xE000:
	{
		switch (kk)
		{
		// Ex9E
		case 0x009E:
			if (keyboard[v[X]] != 0)
				PC += 2;
			break;
		// ExA1
		case 0x00A1:
			if (keyboard[v[X]] == 0)
				PC += 2;
			break;
		}
		break;
	}
	// Fxkk
	case 0xF000:
	{

		switch (kk)
		{
		// Fx07
		case 0x0007:

			v[X] = delay_timer;
			break;
		// Fx0A
		case 0x000A:
			key_pressed = 0;
			for (i = 0; i < 16; i++)
			{
				if (keyboard[i])
				{
					key_pressed = 1;
					v[X] = i;
				}
			}

			if (key_pressed == 0)
			{
				PC -= 2;
			}

			break;
		// Fx15
		case 0x0015:
			delay_timer = v[X];
			break;
		// Fx18
		case 0x0018:
			sound_timer = v[X];
			break;
		// Fx1E
		case 0x001E:
			I = I + v[X];
			break;
		// Fx29
		case 0x0029:
			I = v[X] * 5;
			break;
		// Fx33
		case 0x0033:
		{
			int vX;
			vX = v[X];
			memory[I] = (vX - (vX % 100)) / 100;
			vX -= memory[I] * 100;
			memory[I + 1] = (vX - (vX % 10)) / 10;
			vX -= memory[I + 1] * 10;
			memory[I + 2] = vX;
		}
		break;

		// Fx55
		case 0x0055:

			for (uint8_t i = 0; i <= X; ++i)
			{
				memory[I + i] = v[i];
			}
			break;
		// Fx65
		case 0x0065:

			for (uint8_t i = 0; i <= X; ++i)
			{
				v[i] = memory[I + i];
			}
			break;
		}
	}
	break;

	default:
		DEBUGLOG << "[ERROR|OPCODE] Other opcode -> " << opcode;
		break;
	}
}

void CleanUp_SDL()
{
	SDL_DestroyTexture(screen);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
