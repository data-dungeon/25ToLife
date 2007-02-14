///////////////////////////////////////////////////////////////////////////////////////
///  \file UseCaseMainMenu.cpp
///
///  \brief Implementation of the MainMenu class, which is responsible 
/// for presenting the Main Menu and allowing the player to pick what
/// they want to do.
///////////////////////////////////////////////////////////////////////////////////////

#include "TTLPCH.h"

#include "UseCaseFriends.hpp"
#include "UseCaseYesNoDialog.hpp"
#include "UseCaseDisplayDialog.hpp"
#include "UseCasePlayers.hpp"
#include "UseCaseJoinMatch.hpp"
#include "UseCaseMultiplayerMenu.hpp"
#include "frontend/frontend.h"
#include "frontend/screens/screen_friends.h"
//#include "frontend/screens/screen_FriendRequested.h"
//#include "frontend/screens/screen_friendConfirmed.h"
#include "frontend/screens/screen_FriendsPopup.h"
#include "EORAccount.hpp"
#include "UseCaseUtility.hpp"
#include "UseCaseWaitForOSIRequest.hpp"
#include "GameHelper/Translator.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Constructor: Initialize member variables
///////////////////////////////////////////////////////////////////////////////////////////////////
OnlineUI::Friends::Friends() : 
    _stateMachine( REQUEST_CLAN_INFO ),
	_selectedPlayer( NULL ),
	_askTitle( NULL ),
	_askMessage( NULL ),
	_askYes( DISPLAY_FRIENDS_LIST_MENU ),
	_askNo( DISPLAY_FRIENDS_LIST_MENU ),
	_cleanUpScreens( true )
{
	_stateMachine.setStateEnter( REQUEST_CLAN_INFO,				&OnlineUI::Friends::_enterRequestClanInfo );
	_stateMachine.setStateUpdate(REQUEST_CLAN_INFO,				&OnlineUI::Friends::_updateRequestClanInfo );

	_stateMachine.setStateEnter( REQUEST_FRIENDS_LIST,			&OnlineUI::Friends::_enterRequestFriendsList );
	_stateMachine.setStateUpdate(REQUEST_FRIENDS_LIST,			&OnlineUI::Friends::_updateRequestFriendsList );

	_stateMachine.setStateEnter( REQUEST_ADD_FRIEND,			&OnlineUI::Friends::_enterRequestAddFriend );
	_stateMachine.setStateUpdate(REQUEST_ADD_FRIEND,			&OnlineUI::Friends::_updateRequestAddFriend );

	_stateMachine.setStateEnter( REQUEST_ACCEPT_FRIENDSHIP,		&OnlineUI::Friends::_enterRequestAcceptFriendship );
	_stateMachine.setStateUpdate(REQUEST_ACCEPT_FRIENDSHIP,		&OnlineUI::Friends::_updateRequestAcceptFriendship );

	_stateMachine.setStateEnter( REQUEST_DENY_FRIENDSHIP,		&OnlineUI::Friends::_enterRequestDenyFriendship );
	_stateMachine.setStateUpdate(REQUEST_DENY_FRIENDSHIP,		&OnlineUI::Friends::_updateRequestDenyFriendship );

	_stateMachine.setStateEnter( REQUEST_BLOCK_FRIENDSHIP,		&OnlineUI::Friends::_enterRequestBlockFriendship );
	_stateMachine.setStateUpdate(REQUEST_BLOCK_FRIENDSHIP,		&OnlineUI::Friends::_updateRequestBlockFriendship );

	_stateMachine.setStateEnter( REQUEST_CANCEL_FRIEND_REQUEST,	&OnlineUI::Friends::_enterRequestCancelFriendRequest );
	_stateMachine.setStateUpdate(REQUEST_CANCEL_FRIEND_REQUEST,	&OnlineUI::Friends::_updateRequestCancelFriendRequest );

	_stateMachine.setStateEnter( REQUEST_END_FRIENDSHIP,		&OnlineUI::Friends::_enterRequestEndFriendship );
	_stateMachine.setStateUpdate(REQUEST_END_FRIENDSHIP,		&OnlineUI::Friends::_updateRequestEndFriendship );

	_stateMachine.setStateEnter( REQUEST_INVITE_FRIEND_TO_MATCH,&OnlineUI::Friends::_enterRequestInviteFriendToMatch );
	_stateMachine.setStateUpdate(REQUEST_INVITE_FRIEND_TO_MATCH,&OnlineUI::Friends::_updateRequestInviteFriendToMatch );

	_stateMachine.setStateUpdate(REQUEST_JOIN_MATCH_OF_FRIEND,	&OnlineUI::Friends::_updateRequestJoinMatchOfFriend );

	_stateMachine.setStateEnter( REQUEST_SEND_CLAN_INVITE,		&OnlineUI::Friends::_enterRequestSendClanInvite );
	_stateMachine.setStateUpdate(REQUEST_SEND_CLAN_INVITE,		&OnlineUI::Friends::_updateRequestSendClanInvite );

	_stateMachine.setStateUpdate(DISPLAY_FRIENDS_MENU,			&OnlineUI::Friends::_updateDisplayFriendsMenu );

	_stateMachine.setStateEnter( DISPLAY_FRIENDS_LIST_MENU_START, &OnlineUI::Friends::_enterDisplayFriendsListMenuStart );
	_stateMachine.setStateEnter( DISPLAY_FRIENDS_LIST_MENU, &OnlineUI::Friends::_enterDisplayFriendsListMenu );
	_stateMachine.setStateUpdate(DISPLAY_FRIENDS_LIST_MENU,	&OnlineUI::Friends::_updateDisplayFriendsListMenu );
	
	_stateMachine.setStateEnter( DISPLAY_FRIENDS_MENU_POPUP,	&OnlineUI::Friends::_enterDisplayFriendsMenuPopup );
	_stateMachine.setStateUpdate(DISPLAY_FRIENDS_MENU_POPUP,	&OnlineUI::Friends::_updateDisplayFriendsMenuPopup );

	_stateMachine.setStateEnter( DISPLAY_ASK_MENU,				&OnlineUI::Friends::_enterDisplayAskMenu );
	_stateMachine.setStateUpdate(DISPLAY_ASK_MENU,				&OnlineUI::Friends::_updateDisplayAskMenu );

	_newFriendName[ 0 ] = 0 ; // ugh
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called before use case is started.  Show the first screen for the use case.
///
/// \return true if succeeded, false otherwise
///////////////////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ bool OnlineUI::Friends::_init( void )
{
    g_FrontEnd.GotoScreen("SCREEN_ID_FRIENDS_5_30P");
	_stateAfterRequestingFriendsList = DISPLAY_FRIENDS_MENU;
	_stateMachine.setCurrentState( this, REQUEST_CLAN_INFO );
	_cleanUpScreens = true ;
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called after use case is ended.  Clean up the screens and other objects.
///
/// \return true if succeeded, false otherwise
///////////////////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ bool OnlineUI::Friends::_deinit( void )
{
	//if ( _cleanUpScreens )
	{
		g_FrontEnd.RemoveAllPopups();
		if( UseCase::IsScreenActive< ScreenFriendsList >() )
			g_FrontEnd.EndScreen(0);
		g_FrontEnd.PopScreenHistory( "SCREEN_ID_FRIENDS_LIST" );
		if( UseCase::IsScreenActive< ScreenFriends >() )
			g_FrontEnd.EndScreen(0);
		g_FrontEnd.PopScreenHistory( "SCREEN_ID_FRIENDS_5_30P" );
	}

	OSI::getSingletonPtr()->getFriendList().clear();
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the OSI_REQUEST_CLAN_INFO state.
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::Friends::_enterRequestClanInfo()
{
	const bool requestOK = OSI::getSingletonPtr()->requestClanUpdateMyClanInformation();

	WaitForOSIRequest::getSingletonPtr()->start( requestOK, "Friends", "Please Wait, retrieving Clan information" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the OSI_REQUEST_CLAN_INFO state.
///
/// \return true to continue use case processing
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::Friends::_updateRequestClanInfo()
{
	_stateMachine.setCurrentState( this, REQUEST_FRIENDS_LIST );

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called each time the REQUEST_FRIENDS_LIST state is entered..
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::Friends::_enterRequestFriendsList( void )
{
	const bool requestOK = OSI::getSingletonPtr()->requestFriendList();

	WaitForOSIRequest::getSingletonPtr()->start( requestOK, "Friends", "Please wait, retrieving friends list." );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the REQUEST_FRIENDS_LIST state.
///
/// \return true to continue use case processing, false to return to application main menu.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::Friends::_updateRequestFriendsList( void )
{
	OSIRequestStatus requestStatus = OSI::getSingletonPtr()->getLastRequestStatus();
	if ( requestStatus == STATUS_SUCCEEDED )
	{
		_stateMachine.setCurrentState( this, _stateAfterRequestingFriendsList );
	}
	else if ( requestStatus == STATUS_FAILED )
	{
		stop();
	}
	return true ;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called each time the REQUEST_ADD_FRIEND state is entered..
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::Friends::_enterRequestAddFriend( void )
{
	bool requestOK = false;
	if ( _newFriendName[ 0 ] )
	{
		requestOK = OSI::getSingletonPtr()->requestOfferMyFriendship( _newFriendName );
	}
	else
	{
		requestOK = OSI::getSingletonPtr()->requestOfferMyFriendship( _newFriendID );
	}

	WaitForOSIRequest::getSingletonPtr()->start(
		requestOK, "Friends", "Please wait, sending friendship request."
	);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the REQUEST_ADD_FRIEND state.
///
/// \return true to continue use case processing, false to return to application main menu.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::Friends::_updateRequestAddFriend( void )
{
	OSIRequestStatus requestStatus = OSI::getSingletonPtr()->getLastRequestStatus();
	if ( requestStatus == STATUS_SUCCEEDED )
	{
		OSIchar buffer[ OSI_STRING_LENGTH_LONG ];
		if( _newFriendName[ 0 ] )
		{
			OSIsnprintf( buffer, sizeof( buffer ), g_translator.Translate( "You have successfully invited %s to be your friend." ), _newFriendName );
		}
		else
		{
			OSIstrcpy( buffer, "Your friend invite has been sent successfully." );
		}
		buffer[ sizeof( buffer ) - 1 ] = 0;
		DisplayDialog::getSingletonPtr()->start( "Friend Request Sent", buffer );
		_stateMachine.setCurrentState( this, REQUEST_FRIENDS_LIST );
	}
	else if ( requestStatus == STATUS_FAILED )
	{
		_stateMachine.setCurrentState( this, DISPLAY_FRIENDS_MENU );
	}
	return true ;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called each time the REQUEST_ACCEPT_FRIENDSHIP state is entered..
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::Friends::_enterRequestAcceptFriendship( void )
{
	const bool requestOK = OSI::getSingletonPtr()->requestAuthorizeTheirFriendship( _selectedPlayer->id );

	WaitForOSIRequest::getSingletonPtr()->start(
		requestOK, "Friends", "Please wait, accepting friend offer."
	);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the REQUEST_ACCEPT_FRIENDSHIP state.
///
/// \return true to continue use case processing, false to return to application main menu.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::Friends::_updateRequestAcceptFriendship( void )
{
	_stateMachine.setCurrentState( this, REQUEST_FRIENDS_LIST );
	return true ;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called each time the REQUEST_DENY_FRIENDSHIP state is entered..
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::Friends::_enterRequestDenyFriendship( void )
{
	const bool requestOK = OSI::getSingletonPtr()->requestDenyTheirFriendship( _selectedPlayer->id );

	WaitForOSIRequest::getSingletonPtr()->start(
		requestOK, "Friends", "Please wait, denying friend offer."
	);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the REQUEST_DENY_FRIENDSHIP state.
///
/// \return true to continue use case processing, false to return to application main menu.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::Friends::_updateRequestDenyFriendship( void )
{
	_stateMachine.setCurrentState( this, REQUEST_FRIENDS_LIST );
	return true ;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called each time the REQUEST_BLOCK_FRIENDSHIP state is entered..
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::Friends::_enterRequestBlockFriendship( void )
{
	const bool requestOK = OSI::getSingletonPtr()->requestBlockTheirFriendship( _selectedPlayer->id );

	WaitForOSIRequest::getSingletonPtr()->start(
		requestOK, "Friends", "Please wait, blocking friend offer."
	);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the REQUEST_BLOCK_FRIENDSHIP state.
///
/// \return true to continue use case processing, false to return to application main menu.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::Friends::_updateRequestBlockFriendship( void )
{
	_stateMachine.setCurrentState( this, REQUEST_FRIENDS_LIST );
	return true ;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called each time the REQUEST_CANCEL_FRIEND_REQUEST state is entered..
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::Friends::_enterRequestCancelFriendRequest( void )
{
	const bool requestOK = OSI::getSingletonPtr()->requestCancelMyFriendshipOffer( _selectedPlayer->id );

	WaitForOSIRequest::getSingletonPtr()->start(
		requestOK, "Friends", "Please wait, canceling friend offer."
	);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the REQUEST_CANCEL_FRIEND_REQUEST state.
///
/// \return true to continue use case processing, false to return to application main menu.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::Friends::_updateRequestCancelFriendRequest( void )
{
	_stateMachine.setCurrentState( this, REQUEST_FRIENDS_LIST );
	return true ;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called each time the REQUEST_END_FRIENDSHIP state is entered..
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::Friends::_enterRequestEndFriendship( void )
{
	const bool requestOK = OSI::getSingletonPtr()->requestEndFriendship( _selectedPlayer->id );

	WaitForOSIRequest::getSingletonPtr()->start(
		requestOK, "Friends", "Please wait, ending friendship."
	);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the REQUEST_END_FRIENDSHIP state.
///
/// \return true to continue use case processing, false to return to application main menu.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::Friends::_updateRequestEndFriendship( void )
{
	_stateMachine.setCurrentState( this, REQUEST_FRIENDS_LIST );
	return true ;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called each time the REQUEST_INVITE_FRIEND_TO_MATCH state is entered..
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::Friends::_enterRequestInviteFriendToMatch( void )
{
	const bool requestOK = OSI::getSingletonPtr()->requestInvitePlayerToMatch( _selectedPlayer->id );

	WaitForOSIRequest::getSingletonPtr()->start(
		requestOK, "Friends", "Please wait, inviting friend to match."
	);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the REQUEST_INVITE_FRIEND_TO_MATCH state.
///
/// \return true to continue use case processing, false to return to application main menu.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::Friends::_updateRequestInviteFriendToMatch( void )
{
	OSIRequestStatus requestStatus = OSI::getSingletonPtr()->getLastRequestStatus();
	if ( requestStatus == STATUS_SUCCEEDED )
	{
		OSIchar buffer[ OSI_STRING_LENGTH_LONG ];
		OSIchar nameBuffer[ OSI_STRING_LENGTH_LONG ];
		OSIsnprintf( buffer, sizeof( buffer ), g_translator.Translate( "You have successfully invited %s to your match." ),
			_selectedPlayer->GetFullName( nameBuffer ) );
		buffer[ NUM_ARRAY_ELEMENTS( buffer ) - 1 ] = 0;
		DisplayDialog::getSingletonPtr()->start( "Match Invitation Sent", buffer );
		_stateMachine.setCurrentState( this, REQUEST_FRIENDS_LIST );
	}
	else
	{
		_stateMachine.setCurrentState( this, DISPLAY_FRIENDS_LIST_MENU );
	}
	return true ;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the REQUEST_JOIN_MATCH_OF_FRIEND state.
///
/// \return true to continue use case processing, false to return to application main menu.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::Friends::_updateRequestJoinMatchOfFriend( void )
{
	if (!g_FrontEnd.GetActiveScreen()) return true;

	_selectedPlayer->currentMatchList->begin();
	OSIMatch*	match	= _selectedPlayer->currentMatchList->getCurrent();
	if ( match )
	{
		OSIMatch* currentMatch = OSI::getSingletonPtr()->getCurrentMatch();
		JoinMatch::getSingletonPtr()->setMatchToJoin( match->id );
		if ( currentMatch )
		{
			g_messageDispatcher.SendMessageToAll("Quit", NULL, INVALID_OBJECT_HANDLE); // Quits the current match
			_cleanUpScreens = false ;
			stop();
			MultiplayerMenu::getSingletonPtr()->SetAutomatedUseCaseLaunch( JoinMatch::getSingletonPtr() ); // multiplayer menu will autorun join match usecase.
		}
		else
		{
			_cleanUpScreens = false ;
			stop();
			JoinMatch::getSingletonPtr()->start();
			_stateMachine.setCurrentState( this, REQUEST_FRIENDS_LIST );
		}
	}
	else
	{
		DisplayDialog::getSingletonPtr()->start( "FRIENDS MENU", "That player is no longer in a match" );
		_stateMachine.setCurrentState( this, REQUEST_FRIENDS_LIST );
	}

	return true ;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called each time the REQUEST_SEND_CLAN_INVITE state is entered..
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::Friends::_enterRequestSendClanInvite( void )
{
	const OSIPlayer* const player = OSI::getSingletonPtr()->getActivePlayer();
	const bool requestOK = OSI::getSingletonPtr()->requestClanSendInvitation( player->clanInfo.clanID, _selectedPlayer->name );

	WaitForOSIRequest::getSingletonPtr()->start(
		requestOK, "Friends", "Please wait, inviting friend to your clan."
	);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the REQUEST_SEND_CLAN_INVITE state.
///
/// \return true to continue use case processing, false to return to application main menu.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::Friends::_updateRequestSendClanInvite( void )
{
	OSIRequestStatus requestStatus = OSI::getSingletonPtr()->getLastRequestStatus();
	if( requestStatus == STATUS_SUCCEEDED )
	{
		OSIchar buffer[ OSI_STRING_LENGTH_LONG ];
		OSIchar nameBuffer[ OSIClanInfo::MAX_FULL_NAME_LENGTH ];
		OSIsnprintf( buffer, OSI_STRING_LENGTH_LONG, g_translator.Translate( "Clan Invitation successfully sent to %s." ),
			_selectedPlayer->GetFullName( nameBuffer ) );
		DisplayDialog::getSingletonPtr()->start( "Clan Invite", buffer );
	}

	_stateMachine.setCurrentState( this, DISPLAY_FRIENDS_LIST_MENU );
	return true ;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the DISPLAY_FRIENDS_MENU state.
///
/// \return true to continue use case processing, false to return to application main menu.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::Friends::_updateDisplayFriendsMenu()
{
    if (!g_FrontEnd.GetActiveScreen()) return true;

	// Make sure keyboard is not up
    if (UseCase::IsScreenActive<ScreenKeyboard>()) return true;

    ScreenFriends *pScreen = UseCase::GetActiveScreen<ScreenFriends>();
	if ( !pScreen ) return true ;

    switch (pScreen->GetScreenStatus())
    {
		case ScreenFriends::VIEW_CONFIRMED_FRIENDS:
			_friendsListDisplayMode = ScreenFriendsList::DISPLAY_CONFIRMED_FRIENDS;
			_stateAfterRequestingFriendsList = DISPLAY_FRIENDS_LIST_MENU_START;
			_stateMachine.setCurrentState( this, REQUEST_FRIENDS_LIST );
			break;

		case ScreenFriends::VIEW_INCOMING_FRIEND_REQUESTS:
			_friendsListDisplayMode = ScreenFriendsList::DISPLAY_INCOMING_FRIEND_REQUESTS;
			_stateAfterRequestingFriendsList = DISPLAY_FRIENDS_LIST_MENU_START;
			_stateMachine.setCurrentState( this, REQUEST_FRIENDS_LIST );
			break;

		case ScreenFriends::VIEW_OUTGOING_FRIEND_REQUESTS:
			_friendsListDisplayMode = ScreenFriendsList::DISPLAY_OUTGOING_FRIEND_REQUESTS;
			_stateAfterRequestingFriendsList = DISPLAY_FRIENDS_LIST_MENU_START;
			_stateMachine.setCurrentState( this, REQUEST_FRIENDS_LIST );
			break;

        // Add friend from recent players list
        case ScreenFriends::RECENT_PLAYERS_LIST:
            // Start players list use case here...
			Players::getSingletonPtr()->start();
            break;

        // Add friend by name
        case ScreenFriends::SEND_FRIEND_REQUEST_BY_NAME:
			UseCase::ActivateKeyboardForNames("", OnlineUI::MAX_ENTRY_LENGTH);
            break;

        // Got name back from virtual keyboard
        case ScreenFriends::GOT_NAME_FOR_FRIEND_REQUEST:
		{
			// Check to make sure length of name is in proper range.
			const char* keyboardEntry = g_FrontEnd.GetKeyboard()->GetString();
			size_t keyboardEntryLength = strlen(keyboardEntry);
			if(keyboardEntryLength < EORAccount::NAME_MIN_LENGTH ||
				keyboardEntryLength > EORAccount::NAME_MAX_LENGTH)
			{
				DisplayDialog::getSingletonPtr()->start("Invalid Name", EORNewAccountErrorStringTable[ERROR_ACCOUNT_INFO_NAME_TOO_LONG]);
				break;
			}
			sprintf( _newFriendName, "%s", pScreen->GetEnteredFriendName() );
			_stateMachine.setCurrentState( this, REQUEST_ADD_FRIEND );
		}
            break;

        // Back button hit or back option selected
        case ScreenFriends::BACK:
            stop();
            break;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called each time the DISPLAY_FRIENDS_LIST_MENU_START state is entered..
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::Friends::_enterDisplayFriendsListMenuStart( void )
{
	g_FrontEnd.GotoScreen( "SCREEN_ID_FRIENDS_LIST" );

	_stateAfterRequestingFriendsList = DISPLAY_FRIENDS_LIST_MENU;

	_stateMachine.setCurrentState( this, DISPLAY_FRIENDS_LIST_MENU );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called each time the DISPLAY_FRIENDS_LIST_MENU state is entered..
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::Friends::_enterDisplayFriendsListMenu( void )
{
	ScreenFriendsList *pScreen = UseCase::GetActiveScreen<ScreenFriendsList>();
	if ( !pScreen ) return ;

	pScreen->SetFriendsList( OSI::getSingletonPtr()->getFriendList(), _friendsListDisplayMode );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the DISPLAY_FRIENDS_LIST_MENU state.
///
/// \return true to continue use case processing, false to return to application main menu.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::Friends::_updateDisplayFriendsListMenu( void )
{
	ScreenFriendsList *pScreen = UseCase::GetActiveScreen<ScreenFriendsList>();
	if ( !pScreen ) return true ;

    switch( pScreen->GetScreenStatus() )
    {
		case ScreenFriendsList::FRIEND_SELECTED:
			_selectedPlayer = OSI::getSingletonPtr()->getFriendList()[ pScreen->GetSelectedPlayer() ];
			_stateMachine.setCurrentState( this, DISPLAY_FRIENDS_MENU_POPUP );
			break;
		
		// Back button hit or back option selected
        case ScreenFriendsList::BACK:
			g_FrontEnd.EndScreen( 0 );
			_stateAfterRequestingFriendsList = DISPLAY_FRIENDS_MENU;
            _stateMachine.setCurrentState( this, DISPLAY_FRIENDS_MENU );
            break;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called each time the DISPLAY_FRIENDS_MENU_POPUP state is entered..
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::Friends::_enterDisplayFriendsMenuPopup( void )
{
	g_FrontEnd.ShowPopup( "SCREEN_ID_FRIENDS_POPUP" );

	ScreenFriendsPopup *pPopup = UseCase::GetActiveScreen<ScreenFriendsPopup>();
	if ( !pPopup ) return ;

	pPopup->SetScreenRole( _selectedPlayer );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the DISPLAY_FRIENDS_MENU_POPUP state.
///
/// \return true to continue use case processing, false to return to application main menu.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::Friends::_updateDisplayFriendsMenuPopup( void )
{
	static const char askDisplayTitles[4][50] = {
		"Deny Friend Request",
		"Block Friend Request",
		"Cancel Friend Request",
		"Remove Friend"
	};

	static const char askDisplayMessages[4][255] = {
		"Are you sure you want to deny this friend request?",
		"Are you sure you want to block this player?",
		"Are you sure you want to cancel this friend request?",
		"Are you sure you want to remove this player from your friend list?"
	};

	ScreenFriendsPopup *pScreen = expectPopup<ScreenFriendsPopup>();
	if ( !pScreen )
	{
		return true;
	}

    switch (pScreen->GetScreenState())
    {
		case ScreenFriendsPopup::ACCEPT_FRIEND_REQUEST:
			g_FrontEnd.EndPopup( 1 );
            _stateMachine.setCurrentState( this, REQUEST_ACCEPT_FRIENDSHIP );
            break;

        case ScreenFriendsPopup::DENY_FRIEND_REQUEST:
			g_FrontEnd.EndPopup( 1 );
			_askTitle = askDisplayTitles[0];
			_askMessage = askDisplayMessages[0];
			_askYes = REQUEST_DENY_FRIENDSHIP;
			_askNo = DISPLAY_FRIENDS_LIST_MENU;
			_stateMachine.setCurrentState( this, DISPLAY_ASK_MENU );
            break;

		case ScreenFriendsPopup::BLOCK_FRIEND_REQUEST:
			g_FrontEnd.EndPopup( 1 );
			_askTitle = askDisplayTitles[1];
			_askMessage = askDisplayMessages[1];
			_askYes = REQUEST_BLOCK_FRIENDSHIP;
			_askNo = DISPLAY_FRIENDS_LIST_MENU;
			_stateMachine.setCurrentState( this, DISPLAY_ASK_MENU );
			break ;

		case ScreenFriendsPopup::CANCEL_FRIEND_REQUEST:
			g_FrontEnd.EndPopup( 1 );
			_askTitle = askDisplayTitles[2];
			_askMessage = askDisplayMessages[2];
			_askYes = REQUEST_CANCEL_FRIEND_REQUEST;
			_askNo = DISPLAY_FRIENDS_LIST_MENU;
			_stateMachine.setCurrentState( this, DISPLAY_ASK_MENU );
			break ;

		case ScreenFriendsPopup::REMOVE_FRIEND:
			g_FrontEnd.EndPopup( 1 );
			_askTitle = askDisplayTitles[3];
			_askMessage = askDisplayMessages[3];
			_askYes = REQUEST_END_FRIENDSHIP;
			_askNo = DISPLAY_FRIENDS_LIST_MENU;
			_stateMachine.setCurrentState( this, DISPLAY_ASK_MENU );
			break ;

		case ScreenFriendsPopup::JOIN_FRIEND_GAME:
			g_FrontEnd.EndPopup( 1 );
			_stateMachine.setCurrentState( this, REQUEST_JOIN_MATCH_OF_FRIEND );
			break ;

		case ScreenFriendsPopup::INVITE_FRIEND_TO_GAME:
			g_FrontEnd.EndPopup( 1 );
			_stateMachine.setCurrentState( this, REQUEST_INVITE_FRIEND_TO_MATCH );
			break ;

		case ScreenFriendsPopup::SEND_CLAN_INVITE:
			g_FrontEnd.EndPopup( 1 );
			_stateMachine.setCurrentState( this, REQUEST_SEND_CLAN_INVITE );
			break ;

		// Back button hit or back option selected
        case ScreenFriendsPopup::BACK:
			g_FrontEnd.EndPopup( 0 );
            _stateMachine.setCurrentState( this, DISPLAY_FRIENDS_LIST_MENU );
            break;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called each time the DISPLAY_ASK_MENU state is entered..
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::Friends::_enterDisplayAskMenu( void )
{
	const bool defaultToNoButton = false;

	YesNoDialog::getSingletonPtr()->start(_askTitle, _askMessage, defaultToNoButton);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the DISPLAY_ASK_MENU state.
///
/// \return true to continue use case processing, false to return to application main menu.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::Friends::_updateDisplayAskMenu( void )
{
	switch(YesNoDialog::getSingletonPtr()->GetAnswer()) 
	{
		case YesNoDialog::YES:
			_stateMachine.setCurrentState( this, _askYes );
			break;
		case YesNoDialog::NO:
		case YesNoDialog::CANCEL:
		default:
			_stateMachine.setCurrentState( this, _askNo );
			break;
	}
    return true;
}
