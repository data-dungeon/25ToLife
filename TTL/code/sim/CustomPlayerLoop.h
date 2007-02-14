/********************************************************************
	$File: //depot/TTL/code/sim/CustomPlayerLoop.h $
	$Revision: #7 $
	$DateTime: 2005/05/03 16:18:03 $
	$Author: Bryant_Collard $
	purpose: Implement the initial game loop.
*********************************************************************/

#pragma once

#ifndef _CUSTOMPLAYER_LOOP_H
#define _CUSTOMPLAYER_LOOP_H

#include "main/TTLLoop.h"

class CCustomPlayerLoop : public GameLoop
{
public:
	CCustomPlayerLoop();
	virtual ~CCustomPlayerLoop();

	virtual void RequestExit(void);

protected:
	virtual int DetermineState( int i_state );

	// Some debugging stuff
	virtual const char *LoopName() {return("CustomPlayerLoop");}

private:
	// The main loop states and associated functions.
	enum ELoopStates
	{
		UNINITIALIZED = UNDEFINED_STATE,
		INITIALIZE = 0,
		RUN_SETTINGS,
		SHUTDOWN_CHARACTER,
		INITIALIZE_CHARACTER,
		RUN_CHARACTER,
		TERMINATE,
		NUM_STATES
	};

	static SLoopEntry s_stateArray[ NUM_STATES ]; // Array of state functions
	void Initialize();
	void Run();
	void InitializeCharacter();
	void ShutdownCharacter();
	void Terminate();

	// Message handling
	CMessageOwner d_messageOwner;
	REF_SEND_MESSAGE_HANDLER( ExitSettings_Handler, CCustomPlayerLoop )      m_ExitSettings_Handler;
	REF_SEND_MESSAGE_HANDLER( StartCustomPlayer_Handler, CCustomPlayerLoop ) m_StartCustomPlayer_Handler;
	REF_SEND_MESSAGE_HANDLER( EndCustomPlayer_Handler, CCustomPlayerLoop )   m_EndCustomPlayer_Handler;
	REF_SEND_MESSAGE_HANDLER( AcceptInvitation_Handler, CCustomPlayerLoop )  m_AcceptInvitation_Handler;

	// Loop control
	enum
	{
		MODE_SETTINGS,
		MODE_CHARACTER,
		MODE_EXIT
	} d_mode;

};

#endif // _CUSTOMPLAYER_LOOP_H
