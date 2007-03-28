/*
**
**  File:   OnlineSetup.h
**  Date:   November 2, 2004
**  By:     Chris Stark
**  Desc:   A loop for doing online setup.
**
*/
#if defined(NETWORKING)
#ifndef _ONLINE_SETUP_LOOP_H
#define _ONLINE_SETUP_LOOP_H

////////////////////////////////////////////////////////////////////////////////
#include "Loop/gameloop.h"
#include "main/OSILobby.h"

class COnlineSetupLoop : public GameLoop
{
public:
	enum OnlineSetupLoopStates
	{
		UNINITIALIZED	= UNDEFINED_STATE,
		STARTING		= 0,				// starting up
		STOPPING,							// shutting down
		RUNNING,							// running
		NUM_STATES
	};

	enum ENextLoop
	{
		NEXTLOOP_NONE,
		NEXTLOOP_NETWORK_LOOP,
        NEXTLOOP_MAIN_MENU,
        NEXTLOOP_CUSTOM_PLAYER,
	};
	ENextLoop GetNextLoop() {return(d_nextLoop);}

	/** \name Constructor */
	//\{
	COnlineSetupLoop();
	//\}

	/** \name Destructor */
	//\{
	virtual ~COnlineSetupLoop();
	//\}

	/** \name State machine methods */
	//\{
	void Reset();
	void StartingUpdate();
	void StoppingUpdate();
	void RunningUpdate();
	//\}

	void TerminateEarly();

protected:
	/** \name Base class overrides */
	//\{
	virtual int DetermineState( int i_state );
#ifdef DIRECTX_PC
	virtual void RequestExit();
#endif
	//\}

	// Some debugging stuff
	virtual const char *LoopName() {return("OnlineSetupLoop");}

	/** \name Utility methods */
	//\{
	void Shutdown( void );
	void StartUp( void );
	bool HasFatalErrorAndShouldTerminate( void );
	//\}

	/** \name Message Handlers */
	//\{
	CMessageOwner d_messageOwner;  // this allows this class to send/recieve messages
	REF_SEND_MESSAGE_HANDLER(BeginSimLoopFromHostSetup, COnlineSetupLoop)		d_beginSimLoopFromHostSetupHandler;		///< Probably sent from the HostMatch UseCase
	REF_SEND_MESSAGE_HANDLER(BeginSimLoopFromFindSetup, COnlineSetupLoop)		d_beginSimLoopFromFindSetupHandler;		///< Probably sent from the FindMatch UseCase
    REF_SEND_MESSAGE_HANDLER(BeginSettings, COnlineSetupLoop)		            d_beginCharacterLoopHandler;		    ///< Sent from the MultiplayerMenu UseCase
	//\}

	/** \name Loop transition functionality, Avalanche style */
	//\{
	ENextLoop d_nextLoop;
    OnlineSetupLoopStates d_startingState; ///< State to come back to (defaults to INITIALIZE)
	//\}

	static SLoopEntry stateArray[ NUM_STATES ]; /// < Array of state functions

	COSILobby d_lobby;
};

#endif // _ONLINE_SETUP_LOOP_H
#endif
