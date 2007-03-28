/*
**
**  File:   OnlineSetupLoop.cpp
**  Date:   August 23, 2004
**  By:     Chris Stark
**  Desc:   A loop for doing online setup.
**
*/
#if defined(NETWORKING)
#include "TTLPCH.h"
#include "sim/OnlineSetupLoop.h"

#include "frontend/frontend.h"
#include "frontend/UseCase/UseCaseClans.hpp"
#include "frontend/UseCase/UseCaseDisplayDialog.hpp"
#include "frontend/UseCase/UseCaseFindMatch.hpp"
#include "frontend/UseCase/UseCaseHostMatch.hpp"
#include "frontend/UseCase/UseCaseJoinMatch.hpp"
#include "frontend/UseCase/UseCaseQuickMatch.hpp"
#include "frontend/UseCase/UseCaseStats.hpp"
#include "frontend/UseCase/UseCaseMultiplayerMenu.hpp"
#include "frontend/UseCase/UseCaseYesNoDialog.hpp"
#include "frontend/UseCase/UseCaseReadyUp.hpp"
#include "frontend/UseCase/UseCaseMySettings.hpp"
#include "frontend/UseCase/UseCaseWaitForOSIRequest.hpp"
#include "frontend/UseCase/UseCaseInvitePlayerToMatch.hpp"
#include "Network/SessionSpec.h"
#include "GameHelper/ErrorStack.h"
#include "main/TTL.h"

#if defined( PS2 ) || defined ( DIRECTX_PC ) 
#include "frontend/UseCase/UseCaseEOA.hpp"
#include "frontend/UseCase/UseCaseEORConfig.hpp"
#include "frontend/UseCase/UseCaseEORConnect.hpp"
#include "frontend/UseCase/UseCaseEORCreateAccount.hpp"
#include "frontend/UseCase/UseCaseEORRecoverAccount.hpp"
#include "frontend/UseCase/UseCaseEOREditAccount.hpp"
#include "frontend/UseCase/UseCaseEORDeleteAccount.hpp"
#include "frontend/UseCase/UseCaseEORLogin.hpp"
#include "frontend/UseCase/UseCaseEORPassword.hpp"
#include "frontend/UseCase/UseCaseFriends.hpp"
#include "frontend/UseCase/UseCasePlayers.hpp"
#include <netz.h>
#endif

#ifdef _XBOX
#include "frontend/UseCase/Xbox/UseCaseUIXFriends.hpp"
#include "frontend/UseCase/Xbox/UseCaseUIXLogin.hpp"
#include "frontend/UseCase/Xbox/UseCaseUIXPlayers.hpp"
#include "OSI/XBox/OSIXBox.hpp"
#endif

#ifdef DIRECTX_PC
#include "frontend/UseCase/UseCaseEOA.hpp"
//#include "frontend/UseCase/UseCaseEORConfig.hpp"
#include "frontend/UseCase/UseCaseEORConnect.hpp"
#include "frontend/UseCase/UseCaseEORCreateAccount.hpp"
#include "frontend/UseCase/UseCaseEORRecoverAccount.hpp"
#include "frontend/UseCase/UseCaseEOREditAccount.hpp"
#include "frontend/UseCase/UseCaseEORDeleteAccount.hpp"
#include "frontend/UseCase/UseCaseEORLogin.hpp"
#include "frontend/UseCase/UseCaseEORPassword.hpp"
#include "frontend/UseCase/UseCaseFriends.hpp"
#include "frontend/UseCase/UseCasePlayers.hpp"
#endif

// Namespace usage
using namespace ::OnlineUI;

// Static members
SLoopEntry COnlineSetupLoop::stateArray[ COnlineSetupLoop::NUM_STATES ] =
{
	"STARTING",	(LoopFunc)&COnlineSetupLoop::StartingUpdate,
	"STOPPING",	(LoopFunc)&COnlineSetupLoop::StoppingUpdate,
	"RUNNING",	(LoopFunc)&COnlineSetupLoop::RunningUpdate,
};

COnlineSetupLoop::COnlineSetupLoop() :
	GameLoop(stateArray, UNINITIALIZED),
	d_messageOwner( "OnlineSetupLoop" ),
	d_nextLoop( NEXTLOOP_NONE )
{
}

/*virtual*/ COnlineSetupLoop::~COnlineSetupLoop()
{
}

///-------------------------------------------------------------------------------------------------
/// This is called via COnlineSetupLoop::loopFunc when the loop is in the INITIALIZE state
void COnlineSetupLoop::StartingUpdate()
{
	if ( d_nextLoop == NEXTLOOP_NETWORK_LOOP )
	{
		// stop the ready-up usecase
		UseCase* useCase = OnlineUI::UseCase::getActiveUseCase();
		if( useCase == OnlineUI::ReadyUp::getSingletonPtr() )
		{
			useCase->stop();
		}

		#ifdef _XBOX
		g_FrontEnd.GotoScreen("SCREEN_ID_UIX");
		#else
		g_FrontEnd.GotoScreen("SCREEN_ID_EOR_LOGIN_4_00");
		#endif // !XBOX
		
		g_FrontEnd.GotoScreen("SCREEN_ID_MULTIPLAYER_MENU_5_00");
	}
	else if ( d_nextLoop == NEXTLOOP_CUSTOM_PLAYER )
	{
		// stop the my settings usecase
		UseCase* useCase = OnlineUI::UseCase::getActiveUseCase();
		if( useCase == OnlineUI::MySettings::getSingletonPtr() )
		{
			useCase->stop();
		}

		#ifdef _XBOX
		g_FrontEnd.GotoScreen("SCREEN_ID_UIX");
		#else
		g_FrontEnd.GotoScreen("SCREEN_ID_EOR_LOGIN_4_00");
		#endif // !XBOX
		
		g_FrontEnd.GotoScreen("SCREEN_ID_MULTIPLAYER_MENU_5_00");

	}

	d_nextLoop = NEXTLOOP_NONE;

	// Initialize message handlers
	d_beginSimLoopFromHostSetupHandler.Initialize(	// sent from hostmatch usecase
		"BeginSimLoopFromHostSetup", 
		MESSAGE_HANDLE_IF_MASTER,
		d_messageOwner, 
		this );

	d_beginSimLoopFromFindSetupHandler.Initialize(	// sent from findmatch usecase
		"BeginSimLoopFromFindSetup", 
		MESSAGE_HANDLE_IF_MASTER,
		d_messageOwner, 
		this );

    d_beginCharacterLoopHandler.Initialize(  // Sent from MultiplayerMenu UseCase
        "BeginSettings",
        MESSAGE_HANDLE_IF_MASTER,
        d_messageOwner,
        this);

#if defined(PS2) || defined(_XBOX) || defined(DIRECTX_PC)
	StartUp();
#endif
}


///------------------------------------------------------------------------------------------
/// StartUp-- This gets called EVERY time we enter the OnlineSetupLoop, not just
///           when we start the first time.
///------------------------------------------------------------------------------------------
void COnlineSetupLoop::StartUp( void )
{
	if ( !OSI::isCreated() )
	{
		g_FrontEnd.EndScreen( 0 ); // ends the network configuration screen...could be a problem for XBox and PC
		// 1. Start OSI
		OSI::createOSI(0);
	}

	// 2. Set the Lobby
	CLobby::Start(d_lobby);

    // 3. Start first use case
#ifdef _XBOX
	if( !UIXLogin::getSingletonPtr()->isRunning() )
	{
		UIXLogin::getSingletonPtr()->start();
	}
#elif defined PS2
	if( !EORConfig::getSingletonPtr()->isRunning() )
	{
		EORConfig::getSingletonPtr()->start();
	}
#elif defined DIRECTX_PC
	if ( !MultiplayerMenu::getSingletonPtr()->isRunning() )
	{
		EOREOA::getSingletonPtr()->start();
	}
#endif

#ifdef _XBOX
	OSIXbox* osix = reinterpret_cast< OSIXbox* >( OSI::getSingletonPtr() );
	ASSERT( osix );
	bool UIXRenderCallback( void* dataPtr );
	RenderMgr__RegisterCallback(RENDER_UIX_CALLBACK, UIXRenderCallback, (void *)osix->getFontPlugin());
#endif
}

//-------------------------------------------------------------------------------------------------
//		DeinitializeState
///
/// this is called via COnlineSetupLoop::loopFunc when the loop is in the STOPPING state
///
void COnlineSetupLoop::StoppingUpdate()
{
    // Unregister for messages
    d_messageOwner.UnregisterAll();

	if ( d_nextLoop != NEXTLOOP_NETWORK_LOOP && d_nextLoop != NEXTLOOP_CUSTOM_PLAYER )
		Shutdown();

#ifdef _XBOX
	RenderMgr__KillCallback(RENDER_UIX_CALLBACK); // TTLReset will kill it anyway, but lets be complete anyway
#endif
}

///-----------------------------------------------------------------------------------------------
/// This gets called from SetupLoop if there is an error in NetworkLoop and we're not going
/// to come back to the OnlineSetupLoop.
void COnlineSetupLoop::TerminateEarly( void )
{
	d_messageOwner.UnregisterAll();
	OnlineUI::UseCase::popThroughUseCase( NULL );
	Shutdown();
}

///-------------------------------------------------------------------------------------------------
/// this is called via COnlineSetupLoop::loopFunc when the loop is in the MENU state
void COnlineSetupLoop::RunningUpdate()
{
    if ( !UseCase::getActiveUseCase() )
	{
		d_nextLoop = NEXTLOOP_MAIN_MENU;		// If we empty the use case stack, we need to return to the main menu
    }
}

///-------------------------------------------------------------------------------------------------
/// Returns true if we have a fatal error or our NetZ connection has
/// gone away.
bool COnlineSetupLoop::HasFatalErrorAndShouldTerminate( void )
{
	if ( d_nextLoop != NEXTLOOP_NONE )			return false ; // we're transitioning already, ignore errors
	if ( g_errorStack.IsFatal() )
		return true ;

	return false ;
}


///-------------------------------------------------------------------------------------------------
/// This function returns an integer that is used to lookup into COnlineSetupLoop::loopFunc, e.g.:
///		m_loopState = DetermineState( m_loopState );
///		func = loopFunc[ m_loopState ];
///		func();
int COnlineSetupLoop::DetermineState( int i_state )
{
	if ( HasFatalErrorAndShouldTerminate() )
	{
		g_MemCardManager->SetCancelOperation(true);
		d_nextLoop	= NEXTLOOP_MAIN_MENU ;
		OnlineUI::UseCase::popThroughUseCase( NULL );
		return STOPPING;
    }

	switch( i_state )
	{
		default:								return i_state ;
		case UNINITIALIZED:						return STARTING ;
		case STARTING:							return RUNNING ;
		case WAIT_ON_STATIC_FUNC:				return i_state ;
		case RUNNING:
			switch ( d_nextLoop ) 
			{
				case NEXTLOOP_NONE:				return i_state;
				case NEXTLOOP_MAIN_MENU:		return STOPPING ;
				case NEXTLOOP_CUSTOM_PLAYER:	return STOPPING;
				case NEXTLOOP_NETWORK_LOOP:		return( WaitForTask(&RenderMgr__IsFadedOut, NULL, STOPPING) );
			}
		case STOPPING:
			g_acceptingInvitation = false;
			return UNINITIALIZED;
	};

	// code never gets here
	return UNINITIALIZED;
}


//-------------------------------------------------------------------------------------------------
//		Reset
void COnlineSetupLoop::Reset()
{
	d_nextLoop =  NEXTLOOP_NONE ;
}

//-------------------------------------------------------------------------------------------------
//		Shutdown
///
/// Unregister all messages, destroy all usecases, destroy OSI
void COnlineSetupLoop::Shutdown()
{
	// 1. Stop the Lobby
	CLobby::Stop();

	// 2. Unregister handlers
	d_messageOwner.UnregisterAll();

	// 3. Clear errors
	g_errorStack.ClearAllNonExpiringErrors();

	// 4. Destroy all our singletons (keep these alphabetical)
	Clans::destroySingleton();
	DisplayDialog::destroySingleton();
	FindMatch::destroySingleton();
	HostMatch::destroySingleton();
	JoinMatch::destroySingleton();
	MultiplayerMenu::destroySingleton();
	MySettings::destroySingleton();
	QuickMatch::destroySingleton();
	ReadyUp::destroySingleton();
	Stats::destroySingleton();
	YesNoDialog::destroySingleton();
	WaitForOSIRequest::destroySingleton();
#if defined(PS2) || defined(DIRECTX_PC)
    EOREOA::destroySingleton();
    EORConnect::destroySingleton();
    EORCreateAccount::destroySingleton();
    EORDeleteAccount::destroySingleton();
	EORRecoverAccount::destroySingleton();
	EOREditAccount::destroySingleton();
	EORPassword::destroySingleton();
    EORLogin::destroySingleton();
    Friends::destroySingleton();
	InvitePlayerToMatch::destroySingleton();
    Players::destroySingleton();
#elif defined(_XBOX)
	UIXFriends::destroySingleton();
	UIXLogin::destroySingleton();
	UIXPlayers::destroySingleton();
#endif
#if defined(PS2)
    EORConfig::destroySingleton();
#endif

	// 5. Destroy OSI
	OSI::destroyOSI();

	g_playerProfile.SetAccountName( g_GlobalProfile.GetName() );
	g_playerProfile.SetDisplayName( "" );
	g_playerProfile.SetRank(0);
	g_playerProfile.SetRating(0);
}

///-------------------------------------------------------------------------------------------------
///		BeginSimLoopFromHostSetup::HandleMessage
///
///	Handles the message that a usecase will send telling the loop to move on to the sim loop next.
void COnlineSetupLoop::BeginSimLoopFromHostSetup::HandleMessage( void* i_data, ActorHandle i_sender, bool i_posted )
{
	RenderMgr__FadeOut(FADE_TO_BLACK_FAST);
	d_ref->d_nextLoop = NEXTLOOP_NETWORK_LOOP;
}

///-------------------------------------------------------------------------------------------------
///		BeginSimLoopFromFindSetup::HandleMessage
///
///	Handles the message that a usecase will send telling the loop to move on to the sim loop next.
void COnlineSetupLoop::BeginSimLoopFromFindSetup::HandleMessage( void* i_data, ActorHandle i_sender, bool i_posted )
{
	RenderMgr__FadeOut(FADE_TO_BLACK_FAST);
	d_ref->d_nextLoop = NEXTLOOP_NETWORK_LOOP;
}

///-------------------------------------------------------------------------------------------------
///		BeginSettings::HandleMessage
///
///	Handles the message that a usecase will send telling the loop to move on to the settings (character setup) loop next.
void COnlineSetupLoop::BeginSettings::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
{
	OnlineUI::MySettings::getSingletonPtr()->start();
    d_ref->d_nextLoop = NEXTLOOP_CUSTOM_PLAYER;
}

#ifdef DIRECTX_PC
void COnlineSetupLoop::RequestExit()
{
	Shutdown();
}
#endif

#endif