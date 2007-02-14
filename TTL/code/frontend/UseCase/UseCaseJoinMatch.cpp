///////////////////////////////////////////////////////////////////////////////////////
///  \file UseCaseMainMenu.cpp
///
///  \brief Implementation of the MainMenu class, which is responsible 
/// for presenting the Main Menu and allowing the player to pick what
/// they want to do.
///////////////////////////////////////////////////////////////////////////////////////

#include "TTLPCH.h"

#include "UseCaseJoinMatch.hpp"
#include "UseCaseDisplayDialog.hpp"
#include "Network/OnlineSessionSearch.h"
#include "UseCaseUtility.hpp"
#include "frontend/frontend.h"
#include "main/Lobby.h"
#include "frontend/screens/screen_wait.h"
#include "frontend/screens/screen_title_only.h"
#include "frontend/screens/screen_PopupText.h"
#include "environ/ConfigMgr.h"
#include "Network/SessionSpec.h"
#include "UseCaseUtility.hpp"
#include "UseCaseReadyUp.hpp"
#include "UseCaseWaitForOSIRequest.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Constructor: Initialize member variables
///////////////////////////////////////////////////////////////////////////////////////////////////
OnlineUI::JoinMatch::JoinMatch() : 
	d_messageOwner("UseCaseJoinMatch"),
    _stateMachine(DECIDE_JOIN_METHOD)
{
	_stateMachine.setStateEnter( DECIDE_JOIN_METHOD, &OnlineUI::JoinMatch::_enterDecideJoinMethod );

	_stateMachine.setStateEnter( FIND_MATCH_OF_FRIEND, &OnlineUI::JoinMatch::_enterFindMatchOfFriend );
	_stateMachine.setStateUpdate( FIND_MATCH_OF_FRIEND, &OnlineUI::JoinMatch::_updateFindMatchOfFriend );

	_stateMachine.setStateEnter( REQUEST_STATION_URLS_FOR_MATCH, &OnlineUI::JoinMatch::_enterRequestStationURLSForMatch );
	_stateMachine.setStateUpdate( REQUEST_STATION_URLS_FOR_MATCH, &OnlineUI::JoinMatch::_updateRequestStationURLSForMatch );
	
	_stateMachine.setStateEnter( REQUEST_JOIN_MATCH, &OnlineUI::JoinMatch::_enterRequestJoinMatch );
	_stateMachine.setStateUpdate( REQUEST_JOIN_MATCH, &OnlineUI::JoinMatch::_updateRequestJoinMatch );

	_stateMachine.setStateUpdate( BREAK_TO_NETWORK_LOOP, &OnlineUI::JoinMatch::_updateBreakToNetworkLoop );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called before use case is started.  Show the first screen for the use case.
///
/// \return true if succeeded, false otherwise
///////////////////////////////////////////////////////////////////////////////////////////////////
/*virtual*/
bool OnlineUI::JoinMatch::_init( void )
{
    _setupLaunchHandler.Initialize("SetupLaunch", MESSAGE_HANDLE_IF_MASTER, d_messageOwner, this);

	g_FrontEnd.GotoScreen( "SCREEN_ID_TITLE_ONLY" );

	ScreenTitleOnly* pScreen = UseCase::GetActiveScreen< ScreenTitleOnly >();
	if( pScreen == NULL ) return false;
	pScreen->SetTitle( "Joining Match..." );

	ShowPopupTextOnly( "", "Joining match, please wait..." );

	_stateMachine.setCurrentState( this, DECIDE_JOIN_METHOD );
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called after use case is ended.  Clean up the screens and other objects.
///
/// \return true if succeeded, false otherwise
///////////////////////////////////////////////////////////////////////////////////////////////////
/*virtual*/
bool OnlineUI::JoinMatch::_deinit( void )
{
    d_messageOwner.UnregisterAll();


	COnlineSessionSearch &search = (COnlineSessionSearch &)g_lobby.SessionSearch();
	search.MatchJoinResult(false);
	search.End();

	// We don't clean up the screens here because either the loop transition wipes the screen
	// stack or if we fail the UseCaseMultiplayerMenu cleans up the stack down to it's screen.
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Sets entry point for use case as joining a given match.
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::JoinMatch::setMatchToJoin( const OSIMatchID& match )
{
	_joinMethod = JOIN_MATCH;
	_matchToJoin = match;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Sets entry point for use case as joining a match of a player.
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::JoinMatch::setPlayerToJoin( const OSIPlayerID& player )
{
	_joinMethod = JOIN_PLAYER;
	_playerToJoin = player;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called each time the DECIDE_JOIN_METHOD state is entered.
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::JoinMatch::_enterDecideJoinMethod()
{
	switch( _joinMethod )
	{
		case JOIN_PLAYER:
			_stateMachine.setCurrentState( this, FIND_MATCH_OF_FRIEND );
			break;
		case JOIN_MATCH:
			_stateMachine.setCurrentState( this, REQUEST_STATION_URLS_FOR_MATCH );
			break;
		default:
			ASSERTS( 0, "Unknown JoinMatch method" );
	};
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called each time the FIND_MATCH_OF_FRIEND state is entered.
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::JoinMatch::_enterFindMatchOfFriend()
{
	bool result = OSI::getSingletonPtr()->requestMatchForFriend( _playerToJoin );

	WaitForOSIRequest::getSingletonPtr()->start( result, "Joining Match", "Please wait, joining match..." );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the FIND_MATCH_OF_FRIEND state.
///
/// \return true to continue use case processing
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::JoinMatch::_updateFindMatchOfFriend()
{
	OSIRequestStatus requestStatus = OSI::getSingletonPtr()->getLastRequestStatus();
	if ( requestStatus == STATUS_SUCCEEDED )
	{
		OSIMatchList& matches = OSI::getSingletonPtr()->getMatchList();
		if( matches.count() > 0 )
		{
			OSIMatchID id = matches[0]->id;
			setMatchToJoin( id );
			_stateMachine.setCurrentState( this, REQUEST_STATION_URLS_FOR_MATCH );
			return true;
		}
	}

	COnlineSessionSearch &search = (COnlineSessionSearch &)g_lobby.SessionSearch();
	search.MatchJoinResult(false);

	stop();
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called each time the REQUEST_STATION_URLS_FOR_MATCH state is entered.
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::JoinMatch::_enterRequestStationURLSForMatch()
{
	bool result = OSI::getSingletonPtr()->requestStationURLListForMatch( _matchToJoin );

	WaitForOSIRequest::getSingletonPtr()->start( result, "Joining Match", "Please wait, joining match..." );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the REQUEST_STATION_URLS_FOR_MATCH state.
///
/// \return true to continue use case processing, false to return to application main menu.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::JoinMatch::_updateRequestStationURLSForMatch()
{
	OSIRequestStatus requestStatus = OSI::getSingletonPtr()->getLastRequestStatus();
	if ( requestStatus == STATUS_SUCCEEDED )
	{
		OSIStringList& stationURLList = OSI::getSingletonPtr()->getStationURLList();
		g_sessionSpec.Clear();
		int urlCount = 0;
		for ( stationURLList.begin(); stationURLList.getCurrent() != NULL; stationURLList.next() )
		{
			OSIchar*	sessionURL	= stationURLList.getCurrent();
			bool		addResult	= g_sessionSpec.AddEntry( sessionURL );
			g_debugOutput.Print(g_debugOutput.Category("NET"), "JoinMatch: URL %d: %s", urlCount++, sessionURL);
		}
		
		_stateMachine.setCurrentState( this, REQUEST_JOIN_MATCH );
	}
	else
	{
		COnlineSessionSearch &search = (COnlineSessionSearch &)g_lobby.SessionSearch();
		search.MatchJoinResult(false);

		stop();
	}

	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called each time the REQUEST_STATION_URLS_FOR_MATCH state is entered.
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::JoinMatch::_enterRequestJoinMatch()
{
	bool result = OSI::getSingletonPtr()->requestJoinMatch( _matchToJoin );
	WaitForOSIRequest::getSingletonPtr()->start( result, "Joining Match", "Please wait, joining match..." );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the REQUEST_STATION_URLS_FOR_MATCH state.
///
/// \return true to continue use case processing, false to return to application main menu.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::JoinMatch::_updateRequestJoinMatch()
{
	OSIRequestStatus requestStatus = OSI::getSingletonPtr()->getLastRequestStatus();
	if ( requestStatus == STATUS_SUCCEEDED )
	{
		COnlineSessionSearch &search = (COnlineSessionSearch &)g_lobby.SessionSearch();
		search.SpecifyFoundSession( 0, g_sessionSpec );
		search.MatchJoinResult(true);

		g_messageDispatcher.SendMessageToAll( "SetupLaunch", NULL, INVALID_OBJECT_HANDLE );
	}
	else
	{
		COnlineSessionSearch &search = (COnlineSessionSearch &)g_lobby.SessionSearch();
		search.MatchJoinResult(false);

		stop();
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the BREAK_TO_NETWORK_LOOP state.
/// This state exists strictly so that it can return false and break the usecase processing loop.
///
/// \return true to continue use case processing, false to return to application main menu.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::JoinMatch::_updateBreakToNetworkLoop()
{
	g_messageDispatcher.SendMessageToAll( "BeginSimLoopFromFindSetup", 0, INVALID_OBJECT_HANDLE );

	stop();
	ReadyUp::getSingletonPtr()->start();

	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Callback for when the SetupLaunch message is receieved.  Start the Match Lobby and stop
/// finding.
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::JoinMatch::SetupLaunch::HandleMessage( void* i_data, ActorHandle i_sender, bool i_posted )
{
	JoinMatch::getSingletonPtr()->_stateMachine.setCurrentState( JoinMatch::getSingletonPtr(), BREAK_TO_NETWORK_LOOP );
}


