#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<stdbool.h>
#include<SDL2/SDL.h>


#define MEMSIZE 4096
 
int main (int argc,char ** argv)
{
	if (argc < 2)
    {
        printf("Usage: ./chip8 <romFile> \n");
        return 0;
    }
	int quit=0;

	
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0){

    fprintf(stderr, "SDL failed to initialise: %s\n", SDL_GetError());
	return 1;
     }
    SDL_Event event;
   
	SDL_Window * window = SDL_CreateWindow("CHIP-8",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,640,320,0);
	SDL_Renderer * renderer = SDL_CreateRenderer(window,-1,0);
	//SDL_Surface * screen = SDL_CreateRGBSurface(SDL_SWSURFACE, 64, 64, 32, 0,0,0,0);
	//SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, screen);
	SDL_Texture *screen;
    screen = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB565, SDL_TEXTUREACCESS_STREAMING, 64, 64);
    
    


	unsigned char chip8_fontset[80] =
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
	

	unsigned short opcode; // asignamos una variable para los opcode (chip 8 tiene 35 opcodes)
	unsigned char  memory[(MEMSIZE)];// asignamos una variable para memoria, el sistema de chip 8 usaba 4K asi que asignamos 4096
	unsigned char v[16];/*Los registros de CPU, el de chip 8 tenia 15 de 8bits, 8 bits son 1 Byte asi que son 15 bytes pero usaremos 16,
			      los registros eran llamados desde V0,V1,V2,V3,V4,V5,V6,V7,V8,V9,VA,VB,VC,VD y VE, y elultimo VF se usa como flag*/

	unsigned short I; /* Tenemos el registro de indice ""I"" y un registro de programa (PC) que pueden tener un valor desde 0x000 hasta 0xFFF*/
	unsigned short PC;
	unsigned char gfx[64 * 32];/*los graficos de CHIP 8 son en blanco ynegro, y tiene 2048 pixeles, se puede usar una variable para alterar
					  el estado de los graficos entre 1 y 0 facilmente */


	// Interruptores y registros de hardware
	unsigned char delay_timer;// Un registro de tiempo que cuenta los 60HZ, cuando llega mas de 0 se pone automaticamente en 0
	unsigned char sound_timer;// El buzzer del sistema suena en cualquier momento que sound_timer llegue a 0

	unsigned short stack[16];
	unsigned short sp;
	//char[2048];
	unsigned char keyboard[16];	
	// CHIP-8 Usa un keypad basado en hexadecimal, desde 0x0 hasta 0xF, usaremos una variable para guardar el estado
	bool drawflag;

	
//Initialize everything
void initChip8(){	
	delay_timer= 0;
	sound_timer= 0;
	opcode = 0;
	PC = 0x200;
	I = 0;
	sp = 0;
	memset(stack,0,16);
	memset(memory,0,4096);
	memset(v,0,16);
	for(int i = 0; i < 80; ++i){
    memory[i] = chip8_fontset[i];}	
}

void loadRom(){
	FILE * fp = fopen(argv[1],"rb");
	if(fp == NULL)
	{
		fprintf(stderr,"Can't open the file rom \n");
		exit(1);
	}	
	fseek(fp, 0, SEEK_END);
	int size = ftell(fp); 
	fseek(fp, 0 ,SEEK_SET);
	
	fread(memory+0x200,sizeof(uint16_t),size,fp);
}
	/*for( int i  = 0; i<size;i++)
	{
		printf("%x ",memory[i]);
	}*/
void draw(){
	void *pixels;
    int pitch;
	if(drawflag){
	SDL_LockTexture(screen, NULL, &pixels, &pitch);
    // 4 for 32-bits, 2 for 16-bots colordepth etc...
    memcpy(pixels,gfx, (64*64)*2);
    SDL_UnlockTexture(screen);
    
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, screen, NULL, NULL);
    SDL_RenderPresent(renderer);
}
}
	
	void execute(){
	
		opcode = memory[PC] << 8 | memory[PC + 1];
		PC +=2;
		if(PC == (MEMSIZE)){
			PC = 0;
		}

		
		printf("opcode: %x \n", opcode);
		
		
		switch (opcode & 0xF000){
			case 0x00E0:
	          	memset(gfx, 0, 2048);
	          	break;
	     	case 0x00EE:
	     		PC =  stack[sp--];
	     		break;
			case 0x1000:
				PC = opcode & 0x0FFF;
			break;
			case 0x2000:
	      		stack[sp++] = PC;
	      		PC = opcode & 0x0FFF;
	      	break;
	      	case 0x3000: 
				if (v[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)) PC += 2;
	      	break;
	      	case 0x4000:
	      		if (v[opcode & 0x0F00 >> 8] != (opcode & 0x00FF)) PC+=2;
	      	break;
	      	case 0x5000:
	      		if (v[opcode & 0x0F00 >> 8] == v[(opcode & 0x00F0)]) PC+=2;
	      	break;
	      	case 0x6000:
	      		v[opcode & 0x0F00 >> 8] = opcode & 0x00FF;
	      	break;
	      	case 0x7000:
	      		v[opcode & 0x0F00 >> 8] = opcode & 0x0F00 + opcode & 0x00FF;
	      	break;
	      	case 0x8000:
	      		switch(opcode & 0x000F){
	      			case 0x0000:
	      			v[opcode & 0x0F00 >>8 ] = v[(opcode & 0x00F0) >> 4];
	      			break;

	      			case 0x0001:
	      			v[opcode & 0x0F00 >>8 ] |= v[(opcode & 0x00F0) >> 4];
	      			break;

	      			case 0x0002:
	      			v[(opcode & 0x0F00) >>8 ] &= v[(opcode & 0x00F0) >> 4];
	      			break;

	      			case 0x0003:
	      			v[(opcode & 0x0F00) >>8 ] ^= v[(opcode & 0x00F0) >> 4];
	      			break;

	      			case 0x0004:{
	      			int i;
	      			i = (int)(v[opcode & 0x0F00 >> 8]) + (int)(v[(opcode & 0x00F0) >> 4]);
	      			if (i > 255)
	      				v[0xF] = 1;
	      			else
	      				v[0xF] = 0;
	      			v[opcode & 0x0F00 >> 8] = i;}
	      			break;
	      			
	      			case 0x0005:
					if (v[(opcode & 0x0F00 >> 8 ) > (opcode & 0x00F0 >> 4 )]) v[0xF] = 1;
					else v[0xF] = 0;
					v[(opcode & 0x0F00 >> 8)] = (opcode & 0x0F00 >>8 ) - (opcode & 0x00F0 >> 4);
					break; 
					
					case 0x0006:
					if(v[(opcode & 0x0F00 >> 8)] & 1 == 1) v[(opcode & 0xF)] = 1;
					else v[opcode & 0xF] = 0;
					v[(opcode & 0x0F00) >> 8] = ((opcode & 0x0F00) >> 8) / 2;
					
					case 0x0007:
					if(v[(opcode & 0x00F0) >> 4] > v[(opcode & 0x0F00) >> 8]) v[0xF] = 1;
					else v[0xF] = 0;
					v[(opcode & 0x0F00) >> 8] = v[(opcode & 0x0F00) >> 8] - v[(opcode & 0x00F0)>> 4 ];
					break;

					case 0x000E:
					if(v[(opcode & 0x0F00)>>8 ] >>7 == 1) v[0x0F] = 1;
					else v[0x0F] = 0;
					v[(opcode & 0x0F00 )>> 8] = v[(opcode & 0x0F00)>>8] * 2;
					break; 				
											}
			case 0x9000:
			if(v[(opcode & 0x0F00)>>8] != v[(opcode & 0x00F0)>>4]) PC += 2;
			break;

			case 0xA000:
			I = opcode & 0xFFF;
			break;

			case 0xB000:
			PC = opcode & 0x0FFF + v[0x0];
			break;
	      	
	      	case 0xC000:
	      	v[(opcode & 0x0F00)>>8] = (rand() % 0x100) & (opcode & 0x00FF);
	      	break;

	      	case 0xD000:{
	         unsigned short x = v[(opcode & 0x0F00) >> 8];
             unsigned short y = v[(opcode & 0x00F0) >> 4];
             unsigned short height = opcode & 0x000F;
             unsigned short pixel;

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

              PC += 2;
			
			
			
		}
	      	break;
	      	case 0xE000:
	      		switch(opcode & 0x00FF){
	      			case 0x009E:/* 
	      	Ex9E - SKP Vx
			Skip next instruction if key with the value of Vx is pressed.

			Checks the keyboard, and if the key corresponding to the value of Vx is currently in the down position, PC is increased by 2.*/
					
					break;						
	      		}
	  
	      	break;
	      	default:
      		printf("Opcode error -> %x \n",opcode);
      		break;
     	}
	}
initChip8();
loadRom();	
while(!quit){
	
	SDL_Delay(5);
	SDL_PollEvent(&event);
	switch(event.type)
		{
			case SDL_QUIT:
				quit = 1;
				break;
			}
	execute();
	draw();
	
}
return 0;
}
