#include <gba.h>
#include <gba_video.h>
#include <gba_systemcalls.h>
#include <maxmod.h>

#include <string.h>
#include <stdio.h>

#define CURRENT_LINE (*(volatile unsigned short *)0x4000006)

#define COLUMNS 15
#define ROWS 13
#define FRAMES 340

#include "../data/donut.h"
#include "soundbank.h"
#include "soundbank_bin.h"

int main(void) {
	irqInit();

	// Maxmod requires the vblank interrupt to reset sound DMA.
	// Link the VBlank interrupt to mmVBlank, and enable it. 
	irqSet( IRQ_VBLANK, mmVBlank );
	irqEnable(IRQ_VBLANK);

	consoleInit(0, 4, 0, NULL, 0, 15);
	BG_COLORS[0]=RGB8(0,0,0);
	BG_COLORS[241]=RGB5(31,31,31);
	SetMode(MODE_0 | BG0_ON);
	// ansi escape sequence to clear screen and home cursor
	// /x1b[line;columnH
	// iprintf("\x1b[2J");

	// initialise maxmod with soundbank and 8 channels
    mmInitDefault( (mm_addr)soundbank_bin, 4 );

	// Start playing module
	mmStart( MOD_MUSIC, MM_PLAY_LOOP );

	// ansi escape sequence to clear screen and home cursor
	// /x1b[line;columnH
	// iprintf("\x1b[2J");

	// ansi escape sequence to set print co-ordinates
	// /x1b[line;columnH
	// float A = 0, B = 0;
	// float i, j;
	// int k;
	// float z[1760];
	// char b[1760];
	int i;
	short previousLine = 0;
	char donutBuffer[COLUMNS + 1];
	iprintf("\x1b[14;0H"
			".----------------------------." \
			"| GBA Donut v1.0             |" \
			"| Ported (???) by Korbo      |" \
			"| Music by Jogeir Liljedahl  |" \
			"|     (Press START to exit.) |" \
			"'----------------------------'");
	do {
		for (i = 0; i < FRAMES * ROWS; ++i) {
			memcpy(donutBuffer, donut + i * COLUMNS, COLUMNS + 1);
			donutBuffer[COLUMNS] = '\0';
			scanKeys();
			if (keysDown() && KEY_START)
				Stop();
			if (i % (ROWS + 1) == 0)
				iprintf("\x1b[0;0H");
			iprintf("        %s", donutBuffer);
			if (CURRENT_LINE < previousLine)
				mmFrame();
			previousLine = CURRENT_LINE;
		}
	} while( 1 );
}
