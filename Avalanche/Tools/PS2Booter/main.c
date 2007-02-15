/////////////////////////////////////////////////////////////////////
//
// PS2 Bootstrap elf.. displays splash screen, and HZ options if PAL
//
// Author: Paul Blagay
//
/////////////////////////////////////////////////////////////////////

#include <eekernel.h>
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <math.h>
#include <libdev.h>
#include <eeregs.h>
#include <libgraph.h>
#include <libdma.h>
#include <libpkt.h>
#include <sifdev.h>
#include <sifrpc.h>
#include <libvu0.h>
#include <stdbool.h>
#include <string.h>
#include <libpad.h>
#include <sifcmd.h>
#include <libcdvd.h>

#include "Draw.h"
#include "Rdtarga.h"

#define PAL			0
#define DVD			1

#define BOOT_CD	1

// -------------------------------------------------------------------
// Constants
// -------------------------------------------------------------------

#if BOOT_CD
	#define	SIO2MAN_MOD	"cdrom0:\\DATA_PS2\\IRX\\SIO2MAN.IRX;1"
	#define	PADIRX_MOD	"cdrom0:\\DATA_PS2\\IRX\\PADMAN.IRX;1"
	#define	IOPIMAGE		"cdrom0:\\DATA_PS2\\IOP\\DNAS300.IMG;1"
	#define	SPLASH		"cdrom0:\\DATA_PS2\\BOOTSTRP\\SPLASH.TGA;1"
	#define	HERTZ			"cdrom0:\\DATA_PS2\\BOOTSTRP\\HERTZ.TGA;1"
	#define	EXE			"cdrom0:\\TTL.ELF;1"
#else
	#define	SIO2MAN_MOD	"host0:\\DATA_PS2\\IRX\\SIO2MAN.IRX"
	#define	PADIRX_MOD	"host0:\\DATA_PS2\\IRX\\PADMAN.IRX"
	#define	IOPIMAGE		"host0:\\DATA_PS2\\IOP\\DNAS300.IMG"
	#define	SPLASH		"host0:\\DATA_PS2\\BOOTSTRP\\SPLASH.TGA"
	#define	HERTZ			"host0:\\DATA_PS2\\BOOTSTRP\\HERTZ.TGA"
	#define	EXE			"host0:\\TTL.ELF"
#endif

#define BUTTON_X		1
#define BUTTON_TRI	2

// ============================================================================
// Load an IRX module
// ============================================================================

void loadModule(char *module)
{
	if (sceSifLoadModule(module,0,"") < 0)
	{
		printf("Failed to load module %s\n",module);
		exit(1);
	}
}

// ============================================================================
// Initialises everything
// ============================================================================

void init(void) 
{
	int	mode;

	sceSifInitRpc(0);

#if BOOT_CD
	sceCdInit(SCECdINIT);
	sceCdMmode(DVD ? SCECdDVD : SCECdCD);
	sceCdDiskReady(0);
	while (!sceSifRebootIop(IOPIMAGE)); //for use in the debug
	while (!sceSifSyncIop());			//rem for use in the T10K
	sceSifInitRpc(0);
	sceSifInitIopHeap();
	sceCdInit(SCECdINIT);
	sceCdMmode(DVD ? SCECdDVD : SCECdCD);
	sceCdDiskReady(0);
	sceFsReset();
	sceSifLoadFileReset();
#endif

	// set up display
	mode = (PAL ? PAL_MODE : NTSC_MODE);
	DRAW_init(mode);
}

// ============================================================================
// Display TGA bitmap
// ============================================================================

void DisplayTGA( char* filename, int Time )
{
	int width, height;
	int x, y, maxX, maxY;
	u_char *tga;

	tga = loadTGAfile(filename, &width, &height);

	maxX = ( width  < SCREEN_WIDTH  ) ? width  : SCREEN_WIDTH;
	maxY = ( height < SCREEN_HEIGHT ) ? height : SCREEN_HEIGHT;

	DRAW_clearScreen();

	for ( y = 0 ; y < maxY ; y++ )
	{
		uint* col = (uint*) &tga[ y * width * 4 ];
		for ( x = 0 ; x < maxX ; x++ )
		{
			DRAW_pixel( col[x], x, y );
		}
	}

	DRAW_flipScreen();
	DRAW_flipScreen();

	while ( Time-- >= 0 )
		DRAW_flipScreen();

	free(tga);
}

// ============================================================================
// Init controllers
// ============================================================================

typedef struct
{
	u_long128		buff[scePadDmaBufferMax];
	unsigned char 	actDirect[6];
	unsigned char 	actAlign[6];
} padDMA;

padDMA padBuffer[1][1]	__attribute__((aligned(64)));

void initControllers()
{
	u_long128 *dmaBuff;
	int got = 0;

	loadModule(SIO2MAN_MOD);
	loadModule(PADIRX_MOD);

	scePadInit(0);

	dmaBuff = padBuffer[0][0].buff;
 	if ((scePadPortOpen(0,0, dmaBuff) != 1))
		got = 1;
}

// ============================================================================
// Get controller input
// ============================================================================

int waitForButton()
{
	static unsigned char data[32] __attribute__((aligned(64)));
	while ( 1 )
	{
		int padStatus = scePadGetState( 0,0 );

		if (padStatus == scePadStateFindCTP1 || padStatus == scePadStateStable)
		{
			scePadRead(0, 0, data);
			switch ( data[3] )
			{
				case 191:	return BUTTON_X;
				case 239:	return BUTTON_TRI;
			}
		}

		DRAW_flipScreen();
	}
}

// ============================================================================
// Main entry point
// ============================================================================

int main(int argc, char *argv[])
{
	int	hertz = 60;
	char*	args60[] = { "-hertz60", "-noiop" };
	char*	args50[] = { "-noiop" };

	// init video etc
	init();
	initControllers();

	// load and display the first bitmap
	DisplayTGA(SPLASH, 120);
	
#if PAL
	// load and display the 2nd bitmap
	DisplayTGA(HERTZ, 1);
	if (waitForButton() == BUTTON_TRI)
		hertz = 50;
#endif

	// close down some stuff
	scePadEnd();
	sceSifExitCmd();

	// boot the main elf
	if (hertz == 60)
		LoadExecPS2(EXE, sizeof(args60)/sizeof(args60[0]), args60);
	else
		LoadExecPS2(EXE, sizeof(args50)/sizeof(args50[0]), args50);

	return 0;
}

