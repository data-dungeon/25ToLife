///////////////////////////////////////////////////////////////////////////////////////
///  \file UseCaseMainMenu.cpp
///
///  \brief Implementation of the MainMenu class, which is responsible 
/// for presenting the Main Menu and allowing the player to pick what
/// they want to do.
///////////////////////////////////////////////////////////////////////////////////////

#include "TTLPCH.h"

#include "UseCasePlayers.hpp"
#include "frontend/frontend.h"
#include "../screens/screen_PlayersList.h"
#include "UseCaseDisplayDialog.hpp"
#include "UseCaseWaitForOSIRequest.hpp"
#include "GameHelper/Translator.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Constructor: Initialize member variables
///////////////////////////////////////////////////////////////////////////////////////////////////
OnlineUI::Players::Players() : 
    _stateMachine(OSI_REQUEST_CLAN_INFO),
	_selectedPlayer( NULL ),
	_useCurrentPlayers( false )
{
	_stateMachine.setStateUpdate( MENU_PLAYERS_LIST, &OnlineUI::Players::_updateDisplayPlayersMenu );

	_stateMachine.setStateEnter( MENU_PLAYERS_LIST_POPUP, &OnlineUI::Players::_enterPlayerPopup );
	_stateMachine.setStateUpdate( MENU_PLAYERS_LIST_POPUP, &OnlineUI::Players::_updatePlayerPopup );

	_stateMachine.setStateEnter( OSI_REQUEST_CLAN_INFO, &OnlineUI::Players::_enterOSIClanInfo );
	_stateMachine.setStateUpdate( OSI_REQUEST_CLAN_INFO, &OnlineUI::Players::_updateOSIClanInfo );

	_stateMachine.setStateEnter( OSI_REQUEST_SEND_MATCH_INVITE,&OnlineUI::Players::_enterRequestInviteToMatch );
	_stateMachine.setStateUpdate( OSI_REQUEST_SEND_MATCH_INVITE,&OnlineUI::Players::_updateRequestInviteToMatch );

	_stateMachine.setStateEnter( OSI_REQUEST_FRIENDSHIP, &OnlineUI::Players::_enterOSIInviteFriend );
	_stateMachine.setStateUpdate( OSI_REQUEST_FRIENDSHIP, &OnlineUI::Players::_updateOSIInviteFriend );

	_stateMachine.setStateEnter( OSI_REQUEST_SEND_CLAN_INVITE, &OnlineUI::Players::_enterOSIInviteClan );
	_stateMachine.setStateUpdate( OSI_REQUEST_SEND_CLAN_INVITE, &OnlineUI::Players::_updateOSIInviteClan );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called before use case is started.  Show the first screen for the use case.
///
/// \return true if succeeded, false otherwise
///////////////////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ bool OnlineUI::Players::_init( void )
{
	g_FrontEnd.GotoScreen("SCREEN_ID_PLAYERSLIST");
	_selectedPlayer = NULL ;

	ScreenPlayersList* pScreen = UseCase::GetActiveScreen<ScreenPlayersList>();
	if ( !pScreen ) return false;

	OSIPlayer* tempPlayer;
	OSIPlayerList displayList, tempList;
	tempList = OSI::getSingletonPtr()->getCurrentPlayerList();
	tempList.begin();
	while ( (tempPlayer = tempList.getCurrent()) != NULL )
	{
		displayList.push_back(*tempPlayer);
		tempList.next();
	}

	tempList = OSI::getSingletonPtr()->getRecentPlayerList();
	tempList.begin();
	while ( (tempPlayer = tempList.getCurrent()) != NULL )
	{
		displayList.push_back(*tempPlayer);
		tempList.next();
	}

	_playerList = displayList;
	//if ( _useCurrentPlayers )
	//	_playerList = OSI::getSingletonPtr()->getCurrentPlayerList();
	//else
	//	_playerList = OSI::getSingletonPtr()->getRecentPlayerList();
	
	// Tell the screen what to display
	pScreen->SetRecentPlayersList( _playerList );

	_stateMachine.setCurrentState( this, OSI_REQUEST_CLAN_INFO );

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called after use case is ended.  Clean up the screens and other objects.
///
/// \return true if succeeded, false otherwise
///////////////////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ bool OnlineUI::Players::_deinit( void )
{
	g_FrontEnd.RemoveAllPopups();
	if( UseCase::IsScreenActive<ScreenPlayersList>() )
		g_FrontEnd.EndScreen(0);
	g_FrontEnd.PopScreenHistory( "SCREEN_ID_PLAYERSLIST" );
    return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// Overloaded start function needed to set the the flag which tells what "list" to use.
///////////////////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ void OnlineUI::Players::start( void )
{
	_useCurrentPlayers = false;
	UseCase::start();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// Overloaded start function needed to set the the flag which tells what "list" to use.
///////////////////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ void OnlineUI::Players::start( bool useCurrentPlayers )
{
	_useCurrentPlayers = true;
	UseCase::start();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the MENU_PLAYERS_LIST state.
///
/// \return true to continue use case processing, false to return to application main menu.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::Players::_updateDisplayPlayersMenu()
{
	if (! UseCase::IsScreenActive<ScreenPlayersList>()) return true ; // something else is on top
	ScreenPlayersList* pScreen = UseCase::GetActiveScreen<ScreenPlayersList>();
	if ( !pScreen ) return true ;

	int playerIndex;
	
	switch ( pScreen->GetScreenState() )
	{
		case ScreenPlayersList::PLAYER_SELECTED:
			playerIndex	= pScreen->GetSelectedPlayerIndex();
			_convertSelectedPlayerIndexIntoSelectedPlayer( playerIndex );
			_stateMachine.setCurrentState( this, MENU_PLAYERS_LIST_POPUP );
			break ;
		case ScreenPlayersList::CANCEL:
			stop();
			break ;
		default: 
			break ;
	}

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the MENU_PLAYERS_LIST_POPUP state.
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::Players::_enterPlayerPopup()
{
	g_FrontEnd.ShowPopup( "SCREEN_ID_PLAYERSPOPUP" );

	// Set what options are to be displayed in the popup based on the users clan role
	ScreenPlayersPopup* pScreen = UseCase::GetActiveScreen<ScreenPlayersPopup>();
	if ( !pScreen ) return ;

	pScreen->SetOptionList(
		OSI::getSingletonPtr()->getActivePlayer()->clanRole,
		OSI::getSingletonPtr()->isPlayerInMatch( _selectedPlayer )
	);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the MENU_PLAYERS_LIST_POPUP state.
///
/// \return true to continue use case processing
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::Players::_updatePlayerPopup()
{
	ScreenPlayersPopup* pScreen = expectPopup<ScreenPlayersPopup>();
	if ( !pScreen )
	{
		return true ;
	}

	switch ( pScreen->GetScreenState() )
	{
		case ScreenPlayersPopup::SEND_FRIEND_REQUEST:
			g_FrontEnd.EndPopup(0);
			_stateMachine.setCurrentState( this, OSI_REQUEST_FRIENDSHIP );
			break;
		case ScreenPlayersPopup::SEND_CLAN_INVITE:
			g_FrontEnd.EndPopup(0);
			_stateMachine.setCurrentState( this, OSI_REQUEST_SEND_CLAN_INVITE );
			break;
		case ScreenPlayersPopup::SEND_MATCH_INVITE:
			g_FrontEnd.EndPopup(0);
			_stateMachine.setCurrentState( this, OSI_REQUEST_SEND_MATCH_INVITE );
			break;
		case ScreenPlayersPopup::CANCEL:
			g_FrontEnd.EndPopup(0);
			_stateMachine.setCurrentState( this, MENU_PLAYERS_LIST );
			break;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the OSI_REQUEST_CLAN_INFO state.
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::Players::_enterOSIClanInfo()
{
	const bool requestOK = OSI::getSingletonPtr()->requestClanUpdateMyClanInformation();

	WaitForOSIRequest::getSingletonPtr()->start(
		requestOK, "Recent Players", "Updating recent players list, please wait.", false
	);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the OSI_REQUEST_CLAN_INFO state.
///
/// \return true to continue use case processing
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::Players::_updateOSIClanInfo()
{
	// don't care whether we succeed or fail, just go to recent players list
	_stateMachine.setCurrentState( this, MENU_PLAYERS_LIST );

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the OSI_REQUEST_FRIENDSHIP state.
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::Players::_enterOSIInviteFriend()
{
	const bool requestOK = OSI::getSingletonPtr()->requestOfferMyFriendship( _selectedPlayer->name );

	WaitForOSIRequest::getSingletonPtr()->start(
		requestOK, "Friend Request", "Sending friend invitation, please wait."
	);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the OSI_REQUEST_FRIENDSHIP state.
///
/// \return true to continue use case processing
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::Players::_updateOSIInviteFriend()
{
	if( OSI::getSingletonPtr()->getLastRequestStatus() == STATUS_SUCCEEDED )
	{
		OSIchar buffer[ OSI_STRING_LENGTH_LONG ];
		OSIchar nameBuffer[ OSI_PLAYER_FULL_NAME_LENGTH ];
		OSIsnprintf( buffer, sizeof( buffer ), g_translator.Translate( "You have successfully invited %s to be your friend." ),
			_selectedPlayer->GetFullName( nameBuffer ) );
		DisplayDialog::getSingletonPtr()->start( "Invite Sent", buffer );
	}

	_stateMachine.setCurrentState( this, MENU_PLAYERS_LIST );

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the OSI_REQUEST_SEND_CLAN_INVITE state.
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::Players::_enterOSIInviteClan()
{
	const bool requestOK = OSI::getSingletonPtr()->requestClanSendInvitation(
		OSI::getSingletonPtr()->getActivePlayer()->clanInfo.clanID, _selectedPlayer->name
	);

	WaitForOSIRequest::getSingletonPtr()->start(
		requestOK, "Friend Request", "Sending friend invitation, please wait."
	);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the OSI_REQUEST_SEND_CLAN_INVITE state.
///
/// \return true to continue use case processing
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::Players::_updateOSIInviteClan()
{
	if( OSI::getSingletonPtr()->getLastRequestStatus() == STATUS_SUCCEEDED )
	{
		OSIchar buffer[ OSI_STRING_LENGTH_LONG ];
		OSIchar nameBuffer[ OSI_PLAYER_FULL_NAME_LENGTH ];
		OSIsnprintf( buffer, sizeof( buffer ), g_translator.Translate( "You have successfully invited %s to be in your clan." ),
			_selectedPlayer->GetFullName( nameBuffer ) );
		DisplayDialog::getSingletonPtr()->start( "Invite Sent", buffer );
	}

	_stateMachine.setCurrentState( this, MENU_PLAYERS_LIST );

	return true;
}



///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called each time the REQUEST_INVITE_TO_MATCH state is entered..
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::Players::_enterRequestInviteToMatch( void )
{
	const bool requestOK = OSI::getSingletonPtr()->requestInvitePlayerToMatch( _selectedPlayer->id );

	WaitForOSIRequest::getSingletonPtr()->start(
		requestOK, "Players", "Please wait, inviting player to match."
	);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the REQUEST_INVITE_TO_MATCH state.
///
/// \return true to continue use case processing, false to return to application main menu.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::Players::_updateRequestInviteToMatch( void )
{
	OSIRequestStatus requestStatus = OSI::getSingletonPtr()->getLastRequestStatus();
	if ( requestStatus == STATUS_SUCCEEDED )
	{
		OSIchar buffer[ OSI_STRING_LENGTH_LONG ];
		OSIchar nameBuffer[ OSI_STRING_LENGTH_LONG ];
		OSIsnprintf( buffer, sizeof( buffer ), g_translator.Translate( "You have successfully invited %s to your match." ),
			_selectedPlayer->GetFullName( nameBuffer ) );
		buffer[ sizeof( buffer ) - 1 ] = 0;
		DisplayDialog::getSingletonPtr()->start( "Match Invitation Sent", buffer );
		_stateMachine.setCurrentState( this, MENU_PLAYERS_LIST );
	}
	else
	{
		_stateMachine.setCurrentState( this, MENU_PLAYERS_LIST );
	}
	return true ;
}



///-------------------------------------------------------------------------------
/// Converts a selectedPlayerIndex (0-# recent players) to the OSIPlayer object
/// that it matches in the recent players list.  A selectedPlayerIndex of -1
/// indicates no selection is made (shouldn't get here, but if it does...)
///
///  This is a little messy, because it assumes the recentPlayerList hasn't changed.
/// We could store off the original, but it seems unnecessary.
///--------------------------------------------------------------------------------
void OnlineUI::Players::_convertSelectedPlayerIndexIntoSelectedPlayer( int selectedPlayerIndex )
{
	if ( selectedPlayerIndex == -1 )
	{
		_selectedPlayer = NULL ;
		return ;
	}

	int count = 0 ;
	for ( _playerList.begin(), count=0; _playerList.getCurrent() != NULL; _playerList.next(), count++ )
	{
		OSIPlayer* recentPlayer	= _playerList.getCurrent();
		if ( count == selectedPlayerIndex )
		{
			_selectedPlayer		= recentPlayer ;
			break ;
		}
	}
}