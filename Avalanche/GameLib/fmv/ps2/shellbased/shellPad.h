//-----------------------------------------------------------------------------
//                           PS2 Shell System Core
//
// shellPad header file
//
// $Workfile: shellPad.h $
// $Author: Tyler Colbert $
// $Date: 8/15/03 11:17a $
// $Revision: 1 $
//
// Copyright (c) 2002-3 Sony Computer Entertainment Europe.
// All Rights Reserved.
//
// Description: Simple functions to handle all types of standard pad
//              1. Returns pad type:
//                         0=none connected
//                         1=digital
//                         2=analog (PlayStation1 dualshock)
//                         3=pressure sensitive(PS2 dualshock2)
//              2. Locks analog mode for sticks
//              3. Removes dead zone from sticks
//              4. Provides sticky and toggling buttons
//              5. Provides pressure sensitivity
//              6. Provides velocity on both analogue sticks and buttons
//              7. Enables easy motor setting for vibration
//              8. Works multiple pads & with LIBMTAP if necessary
//-----------------------------------------------------------------------------

#ifndef _SHELLPAD_H
#define _SHELLPAD_H

//-----------------------------------------------------------------------------
// T Y P E D E F S
//-----------------------------------------------------------------------------

typedef enum
{
	SHELLPAD_DISCONNECT = 0,
	SHELLPAD_DIGITAL    = 1,
	SHELLPAD_DUALSHOCK  = 2,
	SHELLPAD_DUALSHOCK2 = 3
} EShellPadType;

// These definitions are required when reading the pressure values
typedef enum
{
	SHELLPAD_PRESSURE_LRIGHT =0,
	SHELLPAD_PRESSURE_LLEFT  =1,
	SHELLPAD_PRESSURE_LUP    =2,
	SHELLPAD_PRESSURE_LDOWN  =3,

	SHELLPAD_PRESSURE_RUP	=4,
	SHELLPAD_PRESSURE_RRIGHT	=5,
	SHELLPAD_PRESSURE_RDOWN	=6,
	SHELLPAD_PRESSURE_RLEFT	=7,

	SHELLPAD_PRESSURE_L1	=8,
	SHELLPAD_PRESSURE_R1	=9,
	SHELLPAD_PRESSURE_L2	=10,
	SHELLPAD_PRESSURE_R2	=11
} EShellPadPressureButton;


typedef struct _ShellPadData
{
	u_long128 pad_dma_buf[scePadDmaBufferMax] __attribute__((aligned(64)));  
	
	// For buttons
	u_int button;
	u_int stickyBtn;
	u_int toggleBtn;
	u_int previousToggleBtn;
	u_int toggledBtn;
	u_int previousBtn;
	
	// pad state info
	int port;
	int slot;
	unsigned short rpad;
	int phase;
	int term_id;
	int pad_type;
	int deadZone;
	int frameCount;
	
	char leftStickX;
	char leftStickY;
	char rightStickX;
	char rightStickY;
	
	char oldLeftStickX;
	char oldLeftStickY;
	char oldRightStickX;
	char oldRightStickY;

	short leftStickVelX;
	short leftStickVelY;
	short rightStickVelX;
	short rightStickVelY;

	unsigned short buttonVelocities[12];
	unsigned char pressures[12];
	unsigned char oldPressure[12];
	
	// For motors
	unsigned char act_direct[6];
	unsigned char act_align[6];
	
}
ShellPadData __attribute__((aligned(64)));


//-----------------------------------------------------------------------------
// P R O T O T Y P E S
//-----------------------------------------------------------------------------

#if defined (__LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
extern "C" {
#endif

void  shellPadInit(int enableMultitaps);
void  shellPadInitPort(ShellPadData *padStruct, 
                      int port, int slot, int deadzone);
EShellPadType shellPadRead(ShellPadData *padStruct);
void  shellPadClearToggleBtn(ShellPadData *padStruct);
void  shellPadSetMotors(ShellPadData *padStruct,
			     int motor1,   
			     int motor2 ); 

#if defined (__LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
}
#endif

#endif
