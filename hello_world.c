#include "lib/neslib.h"
#include "lib/boilerplate.h"
#include "src/rom_0.h"
#include "src/rom_1.h"

// Suggestion: Define smart names for your banks and use defines like this. 
// This is just to make a clear example, and I didn't want to suggest using bank #s directly.
#define ROM_0 0
#define ROM_1 1

#define DUMMY_SONG 0
#define SFX_BOING 0 

// Globals! Defined as externs in src/globals.h
unsigned char currentPadState;
unsigned char i;
char currentMessage[16];

// Local to this file.
static unsigned char showMessageA;
static unsigned char playMusic;
static char screenBuffer[20];

// Put a string on the screen at X/Y coordinates given in adr.
void put_str(unsigned int adr, const char *str) {
	vram_adr(adr);
	while(1) {
		if(!*str) break;
		vram_put((*str++)-0x20);//-0x20 because ASCII code 0x20 is placed in tile 0 of the CHR
	}
}

void write_screen_buffer(unsigned char x, unsigned char y, char* data) {
	screenBuffer[0] = MSB(NTADR_A(x, y)) | NT_UPD_HORZ;
	screenBuffer[1] = LSB(NTADR_A(x, y));
	screenBuffer[2] = 16u;
	for (i = 0; data[i] != '\0'; ++i) 
		screenBuffer[i+3u] = data[i]-0x20;
	screenBuffer[19] = NT_UPD_EOF;
	set_vram_update(screenBuffer);
}

// Main entry point for the application.
void main(void) {

	showMessageA = 0;
	playMusic = 0;
	// Queue up our dummy song.. but don't play it until the user asks (Because it stinks)
	music_play(DUMMY_SONG);
	music_pause(playMusic);

	pal_col(1,0x19);//set dark green color
	pal_col(17,0x19);


	// Show a message to the user.
	put_str(NTADR_A(2,8), "Hello world!");
	put_str(NTADR_A(2,12), "Press A to toggle");
	put_str(NTADR_A(2,13),"the message below.");
	put_str(NTADR_A(2,20), "Start to toggle music");
	ppu_on_all();


	// Now we wait for input from the user, and do dumb things!
	while(1) {
		currentPadState = pad_trigger(0);
		if (currentPadState & PAD_A) {
			showMessageA = !showMessageA;
			if (showMessageA) {
				set_prg_bank(ROM_0);
				set_message_from_rom_0();
			} else {
				set_prg_bank(ROM_1);
				set_message_from_rom_1();
			}
			write_screen_buffer(2, 15, currentMessage);
			sfx_play(SFX_BOING, 0);
		}

		if (currentPadState & PAD_START) {
			playMusic = !playMusic;
			music_pause(playMusic);
		}
		ppu_wait_nmi();
	}
}
