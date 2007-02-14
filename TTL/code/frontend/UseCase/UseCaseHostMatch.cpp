///////////////////////////////////////////////////////////////////////////////////////
///  \file UseCaseMainMenu.cpp
///
///  \brief Implementation of the MainMenu class, which is responsible 
/// for presenting the Main Menu and allowing the player to pick what
/// they want to do.
///////////////////////////////////////////////////////////////////////////////////////

#include "TTLPCH.h"

#include "Network/SessionSpec.h"
#include "UseCaseHostMatch.hpp"
#include "UseCaseDisplayDialog.hpp"
#include "frontend/frontend.h"
#include "../screens/screen_wait.h"
#include "frontend/screens/screen_CreateSession.h"
#include "frontend/screens/screen_GameSetting.h"
#include "UseCaseUtility.hpp"
#include "UseCaseReadyUp.hpp"
#include "UseCaseWaitForOSIRequest.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Constructor: Initialize member variables
///////////////////////////////////////////////////////////////////////////////////////////////////
OnlineUI::HostMatch::HostMatch() : 
	d_messageOwner("UseCaseHostMatch"),
    _stateMachine(CONFIGURING)
{
	_stateMachine.setStateEnter(VALIDATE_NAME,	&OnlineUI::HostMatch::_enterValidateMatchName );
	_stateMachine.setStateUpdate(VALIDATE_NAME,	&OnlineUI::HostMatch::_updateValidateMatchName );

    _stateMachine.setStateUpdate(CONFIGURING,	&OnlineUI::HostMatch::_updateConfiguring );
    _stateMachine.setStateUpdate(HOSTING,		&OnlineUI::HostMatch::_updateHosting );
	_stateMachine.setStateUpdate(PLAYING,		&OnlineUI::HostMatch::_updatePlaying );
	_stateMachine.setStateUpdate(CANCELLING,	&OnlineUI::HostMatch::_updateCancelling );

	_matchOptions.levelIndex = CGameEnv::GetByType( 0, CGameEnv::TYPE_MULTIPLAYER );
	OSIstrncpy( _matchOptions.levelName, CGameEnv::GetWorldDisplayName( _matchOptions.levelIndex ), sizeof( _matchOptions.levelName ) / sizeof( _matchOptions.levelName[0] ) );
	_matchOptions.timeLimit = 3;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called before use case is started.  Show the first screen for the use case.
///
/// \return true if succeeded, false otherwise
///////////////////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ bool OnlineUI::HostMatch::_init( void )
{
#ifdef _XBOX
	// This is only on xbox because when you are voicebanned you skip the ScreenCreateSession part
	// where you pick a session name. 
	if( !XOnlineIsUserVoiceAllowed( OSI::getSingletonPtr()->getActivePlayer()->id.xBox.userFlags ) )
	{
		g_FrontEnd.GotoScreen( "SCREEN_ID_GAMESETTING" );
		g_sessionSpec.SetName( g_playerProfile.GetAccountName() );
	}
	else
#endif 
	{
		g_FrontEnd.GotoScreen("SCREEN_ID_CREATESESSION");
		ScreenCreateSession* pScreen = UseCase::GetActiveScreen<ScreenCreateSession>();
		if ( !pScreen ) return false;
		pScreen->SetValidateMatchName( true );
	}

    // Catch messages from the configure screen
    _creatingMatchHandler.Initialize("SetupCreateSession", MESSAGE_HANDLE_IF_MASTER, d_messageOwner, this);
    _validateMatchNameHandler.Initialize("ValidateMatchName", MESSAGE_HANDLE_IF_MASTER, d_messageOwner, this);
    _gameRulesHandler.Initialize("GameOptionsChanged", MESSAGE_HANDLE_IF_MASTER, d_messageOwner, this);
    _cancelConfigHandler.Initialize("CancelConfigMatch", MESSAGE_HANDLE_IF_MASTER, d_messageOwner, this);

	_stateMachine.setCurrentState( this, CONFIGURING );
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called after use case is ended.  Clean up the screens and other objects.
///
/// \return true if succeeded, false otherwise
///////////////////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ bool OnlineUI::HostMatch::_deinit( void )
{
	d_messageOwner.UnregisterAll();

	g_FrontEnd.RemoveAllPopups();
	if( UseCase::IsScreenActive< ScreenGameSetting >() )
	{
		g_FrontEnd.EndScreen(0);
	}
	g_FrontEnd.PopScreenHistory( "SCREEN_ID_GAMESETTING" );

	if( UseCase::IsScreenActive< ScreenCreateSession >() )
	{
		g_FrontEnd.EndScreen(0);
	}
	g_FrontEnd.PopScreenHistory( "SCREEN_ID_CREATESESSION" );
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the CONFIGURE state.
///
/// \return true to continue use case processing, false to return to application main menu.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::HostMatch::_updateConfiguring()
{
#ifdef _XBOX
	// This block of code is a hack to make the UseCase stop when the GameSetting screen goes away
	// because you backed out.
	// This is only on xbox because when you are voicebanned you skip the ScreenCreateSession part
	// where you pick a session name.  And for some reason doing that breaks the whole world.
	ScreenGameSetting* pScreenGameSetting = UseCase::GetActiveScreen<ScreenGameSetting>();
	ScreenCreateSession* pScreenCreateSession = UseCase::GetActiveScreen<ScreenCreateSession>();
	ScreenKeyboard* pScreenKeyboard = UseCase::GetActiveScreen<ScreenKeyboard>();
	if( !pScreenCreateSession && !pScreenGameSetting && !pScreenKeyboard )
	{
		stop();
	}
#endif

    d_messageOwner.HandlePostedMessages();
    return true;
}

void OnlineUI::HostMatch::_enterValidateMatchName()
{
	const bool requestOK = OSI::getSingletonPtr()->requestValidateMatchName( g_sessionSpec.GetName() );
	
	WaitForOSIRequest::getSingletonPtr()->start( requestOK, "Please Wait", "Validating match name..." );
}

bool OnlineUI::HostMatch::_updateValidateMatchName()
{
	switch( OSI::getSingletonPtr()->getLastRequestStatus() )
	{
		case STATUS_SUCCEEDED:
			g_FrontEnd.GotoScreen( "SCREEN_ID_GAMESETTING" );
			_stateMachine.setCurrentState( this, CONFIGURING );
			break;
		case STATUS_FAILED:
		{
			ScreenCreateSession* pScreen = UseCase::GetActiveScreen<ScreenCreateSession>();
			if ( !pScreen ) return true ;

			pScreen->SetMatchName( "" ); // don't save the bad word
			g_FrontEnd.ShowScreen( g_FrontEnd.GetActiveScreen() );
			g_FrontEnd.ShowKeyboard();

        	pScreen->SetValidateMatchName( true );
			_stateMachine.setCurrentState( this, CONFIGURING );
		}
			break;
	}
	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the HOSTING state.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::HostMatch::_updateHosting()
{
	// Clean up UI
    //g_FrontEnd.EndScreen(0); // Shuts down the configuration screen

	stop();

	// Set up for transition to Ready-Up
	g_messageDispatcher.SendMessageToAll( "BeginSimLoopFromHostSetup", 0, INVALID_OBJECT_HANDLE );
	ReadyUp::getSingletonPtr()->start();

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the UseCase when in the PLAYING state.
///
/// This stops the use case, unregisters the message handlers, and resets to the
/// CONFIGURING state.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::HostMatch::_updatePlaying( void )
{
	return true ;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the UseCase when in the CANCELLING state.
///
/// This stops the use case, unregisters the message handlers, and resets to the
/// CONFIGURING state.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::HostMatch::_updateCancelling( void )
{
	stop();
	return true ;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Callback for when the CreatingMatch message is receieved.  Switch to HOSTING state.
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::HostMatch::CreatingMatch::HandleMessage( void* i_data, ActorHandle i_sender, bool i_posted )
{
	HostMatch::getSingletonPtr()->_stateMachine.setCurrentState( HostMatch::getSingletonPtr(), HOSTING );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// Tell the use case to validate the match name
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::HostMatch::ValidateMatchName::HandleMessage( void* i_data, ActorHandle i_sender, bool i_posted )
{
	HostMatch::getSingletonPtr()->_stateMachine.setCurrentState( HostMatch::getSingletonPtr(), VALIDATE_NAME );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Callback for when the GameOptionsChanged message is receieved.
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::HostMatch::GameRules::HandleMessage( void* i_data, ActorHandle i_sender, bool i_posted )
{
    // TODO: React to rules change
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Callback for when the CancelConfigMatch message is receieved.  Switch to CANCELLING state.
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::HostMatch::CancelConfig::HandleMessage( void* i_data, ActorHandle i_sender, bool i_posted )
{
	HostMatch::getSingletonPtr()->_stateMachine.setCurrentState( HostMatch::getSingletonPtr(), CANCELLING );
}
