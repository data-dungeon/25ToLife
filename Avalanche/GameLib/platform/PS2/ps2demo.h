/***************************************************************************/
// PS2 demo
/***************************************************************************/
#ifndef PS2DEMO_H
#define PS2DEMO_H

//define PS2DEMO to configure the app to run as a demo (i.e., it exits back to some demo browser)
//#define PS2DEMO

#ifdef PS2DEMO

#include <libscedemo.h>

//info passed into demo via sceDemoStart()
typedef struct
{
	unsigned short language;
	unsigned short aspect;
	unsigned short playmode;
	unsigned short to_inactive;
	unsigned short to_gameplay;
	unsigned short mediatype;
	unsigned short masterVolumeScale;
	unsigned int dataDirectorySectorNumber;
	sceDemoEndReason demoEndReason;
} PS2Demo;

//global to allow access to this information
extern PS2Demo ps2Demo;

#endif

#endif
