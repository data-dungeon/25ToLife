/***************************************************************************/
// GCN Input driver
/***************************************************************************/
#ifndef GCNINPUT_H
#define GCNINPUT_H

#include <dolphin/pad.h>

#include "platform/systemi.h"
#include "platform/GCN/gcnctrol.h"

// Maximum number of controllers we want to support (just 1 for now)
#define MAX_CONTROLLERS 	PAD_MAX_CONTROLLERS

class GCNSystemInputDriver : public SystemInputDriver
{
public:
	// Basic initialization, but remember to call Initialize
	GCNSystemInputDriver( void);

	// Clean up (remember to call Terminate!!)
	~GCNSystemInputDriver( void);

	// Call this at the very beginning to really intialize stuff
	bool Initialize( void);

	// Call this at the end to release system resources
	void Terminate( void);

	// Call this to beigin using input
	void BeginInput( void);

	// Clear the key input (call at the end of processing)
	void DoneInput( void);

	// How many controlles are there?
	uint Controllers( void)
		{ return( controllers); }

	// Get a controller interface (these pointers are "static", you don't delete them)
	ControllerInputDriver *GetController( uint player)
		{ return( player < MAX_CONTROLLERS ? controller[player] : 0); }

	PADStatus * GetControllerData( int iPort) { return aPADStatus + iPort; }

	void SetMotorCommand( int iPort, u32 u32MotorCommand) { au32MotorCommands[iPort] = u32MotorCommand; bSendMotorCommands = true; }

	void SetResetMask( int iPort)
	{
		m_u32ResetMask |= ( ( PAD_CHAN0_BIT) >> iPort);
	}

private:
	// Controller stuff
	uint							controllers;

	ControllerInputDriver	*controller[MAX_CONTROLLERS];

	PADStatus					aPADStatus[PAD_MAX_CONTROLLERS];

	u32							au32MotorCommands[PAD_MAX_CONTROLLERS];

	bool							bSendMotorCommands;

	u32							m_u32ResetMask;
};

#endif
