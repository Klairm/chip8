#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "chip8.h"

void initChip8();
void draw();
void execute();
void CleanUp_SDL();
uint32_t loadRom(char* file);

SDL_Renderer * renderer;
SDL_Window * window;
SDL_Texture * screen;

int main (int argc, char ** argv)
{
	uint32_t quit=0;
	
	if (argc < 2)
	{
		printf("Usage: ./chip8 <rom> \n");
		return 0;
	}

	
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		fprintf(stderr, "SDL failed to initialise: %s\n", SDL_GetError());
		return 0;
	}
	SDL_Event event;

	window = SDL_CreateWindow(("CHIP-8:  %s",argv[1]),SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,640,320,0);
	renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	SDL_RenderSetLogicalSize(renderer, 64, 32);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);	

	screen = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,64,32);

	initChip8();
	if (loadRom(argv[1]) == 0)
	{
		CleanUp_SDL();
		return 0;
	}
	int32_t speed=5;
	
	while(!quit)
	{
		printf("Speed: %d \n",speed);
		while(SDL_PollEvent(&event))
		{
				switch(event.type)
				{
					case SDL_QUIT:
					quit = 1;
					break;

					case SDL_KEYDOWN:

					switch (event.key.keysym.sym)
					{
						case SDLK_ESCAPE:quit = 1;break;
						case SDLK_F1:
							initChip8();
							if (loadRom(argv[1]) == 0)
							{
								CleanUp_SDL();
								return 0;
							}
						break;
						case SDLK_F2:speed -= 1;break;
						case SDLK_F3:speed += 1;break;
						case SDLK_x:keyboard[0] = 1;break;
						case SDLK_1:keyboard[1] = 1;break;
						case SDLK_2:keyboard[2] = 1;break;
						case SDLK_3:keyboard[3] = 1;break;
						case SDLK_q:keyboard[4] = 1;break;
						case SDLK_w:keyboard[5] = 1;break;
						case SDLK_e:keyboard[6] = 1;break;
						case SDLK_a:keyboard[7] = 1;break;
						case SDLK_s:keyboard[8] = 1;break;
						case SDLK_d:keyboard[9] = 1;break;
						case SDLK_z:keyboard[0xA] = 1;break;
						case SDLK_c:keyboard[0xB] = 1;break;
						case SDLK_4:keyboard[0xC] = 1;break;
						case SDLK_r:keyboard[0xD] = 1;break;
						case SDLK_f:keyboard[0xE] = 1;break;
						case SDLK_v:keyboard[0xF] = 1;break;
					}
					break;
					
					case SDL_KEYUP:
					
					switch (event.key.keysym.sym)
					{
						case SDLK_x:keyboard[0] = 0;break;
						case SDLK_1:keyboard[1] = 0;break;
						case SDLK_2:keyboard[2] = 0;break;
						case SDLK_3:keyboard[3] = 0;break;
						case SDLK_q:keyboard[4] = 0;break;
						case SDLK_w:keyboard[5] = 0;break;
						case SDLK_e:keyboard[6] = 0;break;
						case SDLK_a: keyboard[7] = 0;break;
						case SDLK_s:keyboard[8] = 0;break;
						case SDLK_d:keyboard[9] = 0;break;
						case SDLK_z:keyboard[0xA] = 0;break;
						case SDLK_c:keyboard[0xB] = 0;break;
						case SDLK_4:keyboard[0xC] = 0;break;
						case SDLK_r:keyboard[0xD] = 0;break;
						case SDLK_f:keyboard[0xE] = 0;break;
						case SDLK_v:keyboard[0xF] = 0;break;
					}
					break;
				}
				break;
			}
			
		if(speed<0)
		{
			speed = 0;
		}
		else
		{
			SDL_Delay(speed);
		}
		if (delay_timer > 0)
			--delay_timer;
			
		execute();
		draw();

	}
	
	CleanUp_SDL();
	
	return 0;
}

//Initialize everything
void initChip8()
{	
	delay_timer= 0;
	sound_timer= 0;
	opcode = 0;
	PC = 0x200;
	I = 0;
	sp = 0;
	memset(stack,0,16);
	memset(memory,0,4096);
	memset(v,0,16);
	memset(gfx,0,2048);
	memset(keyboard,0,16);
	// Load fonts
	memcpy(memory,chip8_fontset,80*sizeof(int8_t));
}

	// Load rom file into memory
uint32_t loadRom(char* file)
{
	FILE * fp = fopen(file,"rb");
	
	if(fp == NULL)
	{
		fprintf(stderr,"Can't open the file rom \n");
		return 0;
	}	
	
	fseek(fp, 0, SEEK_END);
	int size = ftell(fp); 
	fseek(fp, 0 ,SEEK_SET);

	fread(memory+0x200,sizeof(uint16_t),size,fp);
	
	return 1;
}

// Draw function
void draw()
{
	void *pixels;
	int pitch;
	SDL_Rect r;
	int x, y;
	r.x = 0;
	r.y = 0;
	r.w = 1;
	r.h = 1;
		
	if (drawflag)
	{
		SDL_SetRenderDrawColor( renderer, 0, 0, 0, 255 );
		SDL_RenderClear(renderer);
		SDL_SetRenderDrawColor( renderer, 255, 0, 0, 0 );
		for(x=0;x<64;x++)
		{
			for(y=0;y<32;y++)
			{
				if (gfx[(x) + ((y) * 64)] == 1)
				{
					r.x = x;
					r.y = y;
					SDL_RenderFillRect( renderer, &r );
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
	PC +=2;
	X = (opcode & 0x0F00) >> 8;
	Y = (opcode & 0x00F0) >> 4;
	nnn = (opcode & 0x0FFF);
	kk = (opcode & 0x00FF);
	n = (opcode & 0x000F);
	printf("opcode: %x \n", opcode);
	printf("program counter: %x \n",PC);
	printf("I: %x \n",I);
	
	switch (opcode & 0xF000)
	{
			case 0x0000:

			switch(opcode & 0x00FF){
				// 00E0 
				case 0x00E0:
				memset(gfx, 0, 2048);
				drawflag = true;
				break;

				//00E0 
				case 0x00EE:
				--sp;
				PC = stack[sp];
				break;

				default: printf("Opcode error 0xxx -> %x\n",opcode ); 
			}break;

			//1nnn 
			case 0x1000:
			PC = nnn;
			break;
			
			//2nnn 
			case 0x2000:
			stack[sp] = PC;
			++sp;
			PC = nnn;
			break;

			case 0x3000: 
			if (v[X] == kk) PC += 2;
			break;

			case 0x4000:
			if (v[X] != kk) PC+=2;
			break;

			case 0x5000:
			if (v[X] == v[Y]) PC+=2;
			break;

			case 0x6000:
			v[X] = kk;
			break;

			case 0x7000:
			v[(X)] += kk;
			break;

			case 0x8000:
			switch(n){
				case 0x0000:
				v[X ] = v[Y];
				break;

				case 0x0001:
				v[X ] |= v[Y];
				break;

				case 0x0002:
				v[X ] &= v[Y];
				break;

				case 0x0003:
				v[X ] ^= v[Y];
				break;

				case 0x0004:{
					int i;
					i = (int)(v[X]) + (int)(v[Y]);
					if (i > 255)
						v[0xF] = 1;
					else
						v[0xF] = 0;
					v[X] = i & 0xFF;
				}
				break;

				case 0x0005:
				if (v[X]> v[Y] ) v[0xF] = 1;
				else v[0xF] = 0;
				v[X] -= v[Y];
				break; 

				case 0x0006:
				v[0xF] = v[X] &1;
				v[X] >>= 1;
				break;

				case 0x0007:
				if(v[Y] > v[X]) v[0xF] = 1;
				else v[0xF] = 0;
				v[X] = v[Y] - v[X];
				break;

				case 0x000E:
				v[0xF] = v[X] >> 7;
				v[X] <<= 1;
				break; 	

				default: printf("Opcode error 8xxx -> %x\n",opcode );			
			}
			break;

			case 0x9000:
			if(v[X] != v[Y]) PC += 2;
			break;

			case 0xA000:
			I = nnn;
			break;

			case 0xB000:
			PC = (nnn) + v[0x0];
			break;

			case 0xC000:
			v[X] = (rand() % 0x100) & (kk);
			break;

			case 0xD000:;
			uint16_t x = v[X];
			uint16_t y = v[Y];
			uint16_t height = n;
			uint8_t pixel;

			v[0xF] = 0;
			for (int yline = 0; yline < height; yline++) {
				pixel = memory[I + yline];
				for(int xline = 0; xline < 8; xline++) {
					if((pixel & (0x80 >> xline)) != 0) {
						if(gfx[(x + xline + ((y + yline) * 64))] == 1){
							v[0xF] = 1;                                   
						}
						gfx[x + xline + ((y + yline) * 64)] ^= 1;
					}

				}

			}
			drawflag = true;
			break;

			case 0xE000:
			switch(kk){

				case 0x009E:
				if(keyboard[v[X]] != 0)PC += 2;
				break;						
				case 0x00A1:
				if(keyboard[v[X]]==0)PC+=2;
				break;

			}
			break;
			case 0xF000:

			switch(kk){
				case 0x0007:

				v[X] = delay_timer;
				break;
				case 0x000A:
				key_pressed = 0;
				for(i=0;i<16;i++)
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
				case 0x0015:
				delay_timer = v[X];
				break;
				case 0x0018:
				sound_timer = v[X];
				break;
				case 0x001E:
				I = I + v[X];
				break;

				case 0x0029:
				I = v[X] * 5;
				break;

				case 0x0033:{
					int vX;
					vX = v[X];
					memory[I]     = (vX - (vX % 100)) / 100;
					vX -= memory[I] * 100;
					memory[I + 1] = (vX - (vX % 10)) / 10;
					vX -= memory[I+1] * 10;
					memory[I + 2] = vX;
				}
				break;


				case 0x0055:

				for (uint8_t i = 0; i <= X; ++i){
					memory[I+ i] = v[i];	
				}
				break;

				case 0x0065:

				for (uint8_t i = 0; i <= X; ++i){
					v[i] = memory[I+ i];	
				}
				break;

			}
			break;	
			default:
			
			printf("Opcode error -> %x \n",opcode);
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


