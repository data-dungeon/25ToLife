//***************************************************************************/
// The game variations setup
/***************************************************************************/
#ifndef GAME_VERSION_H
#define GAME_VERSION_H

#define TTL_PATCH_VERSION 0x000 //Increment to get TTL to do a patch

//#define EENET

//#define CONSUMER_BUILD	  	// This is for the "real deal" consumer build

#ifndef CONSUMER_BUILD
#define ALLOW_FREE_CAM			// Define this to allow L1+R1 camera cycling
#define ALLOW_DEBUG_MENU		// whether the debug menu is supported or not
#endif

#ifndef CDROM
#define ALLOW_SCREEN_SNAPS		// allow screen snaps, along with right dpad clears pause menu, and up dpad allows single step
#endif

#endif GAME_VERSION_H


