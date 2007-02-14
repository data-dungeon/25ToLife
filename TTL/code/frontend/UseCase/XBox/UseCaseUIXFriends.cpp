///////////////////////////////////////////////////////////////////////////////////////
///  \file UseCaseUIXFriends.cpp
///
///  \brief Implementation of the UIXFriends class, which is responsible 
///   for presenting the UIX Friends screen on XBox.
///////////////////////////////////////////////////////////////////////////////////////

#include "TTLPCH.h"
#include "frontend/inputdriver.h"
#include "UseCaseUIXFriends.hpp"
#include "../../screens/screen_UIX.h"
#include "frontend/frontend.h"
#include "platform/Xbox/xboxctrol.h"
#include "frontend/UseCase/UseCaseDisplayDialog.hpp"
#include "frontend/UseCase/UseCaseMultiplayerMenu.hpp"
#include "XboxUIXFontRenderer.hpp"
#include "frontend/UseCase/UseCaseJoinMatch.hpp"
#include "frontend/UseCase/UseCaseReadyUp.hpp"
#include "main/TTL.h"
#include <uix.h>


///////////////////////////////////////////////////////////////////////////////////////////////////
/// Constructor: Initialize member variables
///////////////////////////////////////////////////////////////////////////////////////////////////
OnlineUI::UIXFriends::UIXFriends() : 
    _inMatchLobby(false),
    _stateMachine(NORMAL)
{
	_stateMachine.setStateUpdate( NORMAL, &OnlineUI::UIXFriends::_updateNormal );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called before use case is started.  Show the first screen for the use case.
///
/// \return true if succeeded, false otherwise
///////////////////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ bool OnlineUI::UIXFriends::_init( void )
{
    // Start friends screen
    OSIXbox* osix = reinterpret_cast< OSIXbox* >( OSI::getSingletonPtr() );
	ASSERT( osix );
	osix->uixSetAllowPlayerToInviteFriends( _inMatchLobby );
	osix->uixBeginFriendList( NULL );
	if( osix->getLastRequestStatus() == STATUS_FAILED )
	{
		DisplayDialog::getSingletonPtr()->start( "",  OSI::getSingletonPtr()->getErrorString() );
		return false;
	}

    // Start black UIX screen
    g_FrontEnd.GotoScreen("SCREEN_ID_UIX");

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called after use case is ended.  Clean up the screens and other objects.
///
/// \return true if succeeded, false otherwise
///////////////////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ bool OnlineUI::UIXFriends::_deinit( void )
{
    OSIXbox* osix = reinterpret_cast< OSIXbox* >( OSI::getSingletonPtr() );
	ASSERT( osix );

	osix->uixEndFeature(); // make sure friends interface is gone

	g_FrontEnd.RemoveAllPopups();
	if( UseCase::IsScreenActive< ScreenUIX >() )
		g_FrontEnd.EndScreen(0);
	//g_FrontEnd.PopScreenHistory( "SCREEN_ID_UIX" );

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::UIXFriends::_updateNormal()
{
    OSIXbox* osix = reinterpret_cast< OSIXbox* >( OSI::getSingletonPtr() );
	ASSERT( osix );

	// update the feature
	osix->uixUpdateFriendList();
	if( osix->getLastRequestStatus() == STATUS_FAILED )
	{
		stop();
		DisplayDialog::getSingletonPtr()->start( "",  osix->getErrorString() );
		return true;
	}

	// see if UIX wants to reboot for some reason
	if( osix->uixMustReboot() )
	{
		//FIXME: do any last-minute things here before launching the dash
		osix->launchExternalAccountMaintenanceApp();
	}

	// did feature complete?
	if( osix->uixDidFriendListFinishThisFrame() )
	{
		stop();
		if( osix->uixWasFriendListInviteAcceptedForThisTitle() )
		{
			if( osix->getCurrentMatch() )
			{
				g_messageDispatcher.SendMessageToAll("Quit", NULL, INVALID_OBJECT_HANDLE);
			}

			// and set up the joinmatch usecase to join the friend's match
			OSIPlayerID playerToJoin;
			osix->uixGetFriendForAcceptedInvite( playerToJoin );
			JoinMatch::getSingletonPtr()->setPlayerToJoin( playerToJoin );

			// set up the multiplayer menu to automatically launch the joinmatch usecase
			MultiplayerMenu::getSingletonPtr()->SetAutomatedUseCaseLaunch( JoinMatch::getSingletonPtr() );
		}
		else if( osix->uixWasFriendListInviteAcceptedForOtherTitle() )
		{
			osix->uixRebootNow();
		}
	}

    return true;
}
