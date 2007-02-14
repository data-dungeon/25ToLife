#include "TTLPCH.h"

#include "UseCaseInvitePlayerToMatch.hpp"
#include "frontend/frontend.h"
#include "../screens/screen_wait.h"
#include "UseCaseDisplayDialog.hpp"
#include "UseCaseWaitForOSIRequest.hpp"
#include "GameHelper/Translator.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Constructor: Initialize member variables
///////////////////////////////////////////////////////////////////////////////////////////////////
OnlineUI::InvitePlayerToMatch::InvitePlayerToMatch() : 
    _stateMachine( LISTEN_FOR_MESSAGES )
{
	_stateMachine.setStateUpdate(LISTEN_FOR_MESSAGES, &OnlineUI::InvitePlayerToMatch::_updateListenForMessages);

	_stateMachine.setStateUpdate(CHECK_NAME_WITH_CURRENT_PLAYERS, &OnlineUI::InvitePlayerToMatch::_updateCheckCurrentPlayers);

	_stateMachine.setStateEnter(INVITE_PLAYER_BY_NAME, &OnlineUI::InvitePlayerToMatch::_enterInvitePlayerByName);
	_stateMachine.setStateUpdate(INVITE_PLAYER_BY_NAME, &OnlineUI::InvitePlayerToMatch::_updateInvitePlayerByName);

	_stateMachine.setStateUpdate(SHUT_DOWN, &OnlineUI::InvitePlayerToMatch::_updateShutDown);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called before use case is started.  Show the first screen for the use case.
///
/// \return true if succeeded, false otherwise
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::InvitePlayerToMatch::_init( void )
{
	_inviteByNameHandler.Initialize( "InviteByName", MESSAGE_HANDLE_IF_MASTER, d_messageOwner, this );
	_cancelInviteByNameHandler.Initialize( "CancelInviteByName", MESSAGE_HANDLE_IF_MASTER, d_messageOwner, this );

	UseCase::ActivateKeyboardForNames( "" );

	_stateMachine.setCurrentState( this, LISTEN_FOR_MESSAGES );
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called after use case is ended.  Clean up the screens and other objects.
///
/// \return true if succeeded, false otherwise
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::InvitePlayerToMatch::_deinit( void )
{
	d_messageOwner.UnregisterAll();
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when we're in the LISTEN_FOR_MESSAGES state
///
/// \return true to continue use case processing
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::InvitePlayerToMatch::_updateListenForMessages()
{
    d_messageOwner.HandlePostedMessages();

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the CHECK_NAME_WITH_CURRENT_PLAYERS state.
///
/// \return true to continue use case processing, false to return to application main menu.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::InvitePlayerToMatch::_updateCheckCurrentPlayers()
{
	if( OSI::getSingletonPtr()->isPlayerInMatch( _inviteeName ) )
	{
		stop();
		DisplayDialog::getSingletonPtr()->start( "Invalid Invitation", "Player is already in the match." );
	}
	else
	{
		_stateMachine.setCurrentState( this, INVITE_PLAYER_BY_NAME );
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called each time the INVITE_PLAYER_BY_NAME state is entered.
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::InvitePlayerToMatch::_enterInvitePlayerByName()
{
	bool requestOK = OSI::getSingletonPtr()->requestInvitePlayerToMatch( _inviteeName );

	WaitForOSIRequest::getSingletonPtr()->start( requestOK, "Please wait", "Sending match invitation..." );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the INVITE_PLAYER_BY_NAME state.
///
/// \return true to continue use case processing, false to return to application main menu.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::InvitePlayerToMatch::_updateInvitePlayerByName()
{
	stop();
	OSIRequestStatus requestStatus = OSI::getSingletonPtr()->getLastRequestStatus();
	if( requestStatus == STATUS_SUCCEEDED )
	{
		OSIchar buffer[ OSI_STRING_LENGTH_LONG ];
		OSIsnprintf( buffer, sizeof( buffer ), g_translator.Translate( "You have successfully invited %s to your match." ), _inviteeName );
		DisplayDialog::getSingletonPtr()->start( "Match Invitation Sent", buffer );
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the INVITE_PLAYER_BY_NAME state.
///
/// \return true to continue use case processing, false to return to application main menu.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::InvitePlayerToMatch::_updateShutDown()
{
	stop();
	return true;
}

/******************************************************************************/
/******************************************************************************/
void OnlineUI::InvitePlayerToMatch::InviteByName::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
{
	OSIstrncpy( InvitePlayerToMatch::getSingletonPtr()->_inviteeName, reinterpret_cast<char*>(i_data), OSI_PLAYER_NAME_LENGTH );
	InvitePlayerToMatch::getSingletonPtr()->_stateMachine.setCurrentState(
		InvitePlayerToMatch::getSingletonPtr(), CHECK_NAME_WITH_CURRENT_PLAYERS
	);
}

/******************************************************************************/
/******************************************************************************/
void OnlineUI::InvitePlayerToMatch::CancelInviteByName::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
{
	InvitePlayerToMatch::getSingletonPtr()->_stateMachine.setCurrentState( InvitePlayerToMatch::getSingletonPtr(), SHUT_DOWN );
}