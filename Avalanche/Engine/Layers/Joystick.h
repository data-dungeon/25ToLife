/* --------------------------------------------------
 * File    : JOYSTICK.H
 * Created : Fri Feb 26 09:35:38 1999
 * Descript: Joystick Duh!
 * Platform: EXPORTABLE
 * $Revision: 7 $
 * $Log: /Avalanche/Engine/Layers/Joystick.h $
 * 
 * 7     11/27/01 12:18p Nmartin
 * remove Processor directory, promote Math to same level as Display,
 * Game, etc. Move Assert to Layers. Move BaseType.h to Platform
 * 
 * 6     10/10/01 2:26p Nmartin
 * math include file rename and location change
 * 
 * 5     7/19/01 12:23p Nmartin
 * game include file clean up
 *
 *    Rev 1.2   Jul 06 1999 11:32:42   Food
 * c++ compile stuff
 *
 *    Rev 1.1   Jun 14 1999 14:39:44   Dave
 * Added a body from the PSX version
 *
 *    Rev 1.0   May 07 1999 17:52:32   Jon
 * Initial revision.
 *
 *    Rev 1.0   09 Mar 1999 00:39:40   Joe
 * Initial revision.
 *
 *    Rev 1.1   08 Mar 1999 23:30:40   Joe
 * Added Portability Information.
 * --------------------------------------------------*/
#ifndef __L_JOYSTICK_H
#define __L_JOYSTICK_H

/* --------------------------------------------------
 * Includes
 * --------------------------------------------------*/

/* system includes */

/* engine includes */

#include "platform/BaseType.h"

#if defined(__cplusplus)
extern "C" {
#endif

/* --------------------------------------------------
 * Defines
 * --------------------------------------------------*/
#define _MAX_ANALOG_STICKS 	2
#define _MAX_ANALOG_BUTTONS 	2
#define _MAX_CONTROLLERS 		4

struct ts_AnalogStk
{
    short	  			XThrow, YThrow;
};

struct ts_JoyStk
{
	u32  				u32CapsFlags;		// device capabilities
	u32   			u32CurData;			// current frame's digital button data
	u32   			u32LastData;		// last frame's digital button data
	u32   			u32DebData;			// debounced digital button data
	u32   			u32OpenData;		// current frame's trailing edge (just opened) buttons
	ts_AnalogStk  	AnalogStks[_MAX_ANALOG_STICKS];
	short         	AnalogBtns[_MAX_ANALOG_BUTTONS];
};

// Bits Field Defines
#define BTN_START					0x00000001
#define BTN_SELECT 				0x00000002
#define BTN_DSTK_U				0x00000010
#define BTN_DSTK_D				0x00000020
#define BTN_DSTK_L				0x00000040
#define BTN_DSTK_R				0x00000080
#define BTN_1						0x00000100
#define BTN_2						0x00000200
#define BTN_3						0x00000400
#define BTN_4						0x00000800
#define BTN_5						0x00001000
#define BTN_6						0x00002000
#define BTN_7						0x00004000
#define BTN_8						0x00008000


// Bits Custom to CapsFlags
// Which Analog Sticks are available?
#define BTN_ASTK_1_AVAILABLE	0x00010000
#define BTN_ASTK_2_AVAILABLE	0x00020000
#define BTN_ASTK_3_AVAILABLE	0x00040000
#define BTN_ASTK_4_AVAILABLE	0x00080000
#define BTN_PLUGGED_IN			0x00100000
#define BTN_IN_MULTI				0x00200000
#define BTN_ACTUATOR_ENABLED	0x00400000

// Bits Custom to Data Fields
// Which Analog Sticks are past threshold?
#define BTN_ASTK_1_PASTTHRESH	0x00010000
#define BTN_ASTK_2_PASTTHRESH	0x00020000
#define BTN_ASTK_3_PASTTHRESH	0x00040000
#define BTN_ASTK_4_PASTTHRESH	0x00080000

/* --------------------------------------------------
 * Exported Data
 * --------------------------------------------------*/
extern ts_JoyStk			Joy[_MAX_CONTROLLERS];

/* --------------------------------------------------
 * Exported Functions
 * --------------------------------------------------*/
void 			joyInit(int nJoy);
void 			joyLaunchRead( void);
void 			joyGetData( void);

#if defined(__cplusplus)
}
#endif

#endif __L_JOYSTICK_H
