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

	// initialise maxmod with soundbank and 8 channels
    mmInitDefault( (mm_addr)soundbank_bin, 4 );

	// Start playing module
	mmStart( MOD_MUSIC, MM_PLAY_LOOP );

	int i;
	short previousLine = 0;
	char donutBuffer[COLUMNS + 1];
	iprintf("\x1b[14;0H"
			".----------------------------." \
			"| GBA Donut v1.1             |" \
			"| Ported (???) by Korbo      |" \
			"| Music by Jogeir Liljedahl  |" \
			"|    (Turn off GBA to exit.) |" \
			"'----------------------------'");
	do {
		for (i = 0; i < FRAMES * ROWS; ++i) {
			memcpy(donutBuffer, donut + i * COLUMNS, COLUMNS + 1);
			donutBuffer[COLUMNS] = '\0';
			if (i % (ROWS + 1) == 0)
				iprintf("\x1b[0;0H");
			iprintf("        %s", donutBuffer);
			if (CURRENT_LINE < previousLine)
				mmFrame();
			previousLine = CURRENT_LINE;
		}
	} while( 1 );
}
