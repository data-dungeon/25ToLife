///////////////////////////////////////////////////////////////////////////////////////
///  \file UseCaseMultiplayerMenu.cpp
///
///  \brief Implementation of the MultiplayerMenu class, which is responsible 
///   for presenting the Multiplayer Menu and allowing the player to pick online
///   game options.
///////////////////////////////////////////////////////////////////////////////////////

#include "TTLPCH.h"

#include "UseCaseClans.hpp"
#include "UseCaseDisplayDialog.hpp"
#include "UseCaseYesNoDialog.hpp"
#include "UseCaseFindMatch.hpp"
#include "UseCaseHostMatch.hpp"
#include "UseCaseMultiplayerMenu.hpp"
#include "UseCaseQuickMatch.hpp"
#include "UseCaseStats.hpp"
#include "frontend/frontend.h"
#include "frontend/screens/screen_CreateSession.h"
#include "../screens/screen_MultiplayerMenu.h"
#include "data/screens/Multiplayer.h"

#include "UseCaseWaitForOSIRequest.hpp"
#include "GameHelper/Translator.h"
#include "GameHelper/ErrorStack.h"

#if defined(PS2) || defined(DIRECTX_PC)
#include "UseCaseFriends.hpp"
#include "UseCasePlayers.hpp"
#endif

#ifdef _XBOX
#include "OSI/Xbox/OSIXbox.hpp"
#include "Xbox/UseCaseUIXFriends.hpp"
#include "Xbox/UseCaseUIXPlayers.hpp"
#endif

using namespace ::OnlineUI;

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Constructor: Initialize member variables
///////////////////////////////////////////////////////////////////////////////////////////////////
OnlineUI::MultiplayerMenu::MultiplayerMenu() : 
    _stateMachine(GET_MESSAGE_OF_THE_DAY),
	_automatedUseCaseLaunch( 0 ),
	_onOpenCalled( false )
{
	_stateMachine.setStateEnter(GET_MESSAGE_OF_THE_DAY, &OnlineUI::MultiplayerMenu::_enterMessageOfTheDay);
	_stateMachine.setStateUpdate(GET_MESSAGE_OF_THE_DAY, &OnlineUI::MultiplayerMenu::_updateMessageOfTheDay);
    _stateMachine.setStateUpdate(MULTIPLAYER, &OnlineUI::MultiplayerMenu::_updateMultiplayer);
	_stateMachine.setStateEnter(ASK_TO_LOGOUT, &OnlineUI::MultiplayerMenu::_enterAskToLogout);
    _stateMachine.setStateUpdate(ASK_TO_LOGOUT, &OnlineUI::MultiplayerMenu::_updateAskToLogout);
	_stateMachine.setStateEnter(LOGOUT, &OnlineUI::MultiplayerMenu::_enterLogout);
    _stateMachine.setStateUpdate(LOGOUT, &OnlineUI::MultiplayerMenu::_updateLogout);

#ifdef _XBOX
	_stateMachine.setStateEnter( UPDATE_MY_CLAN_INFO, &OnlineUI::MultiplayerMenu::_enterUpdateMyClanInfo );
    _stateMachine.setStateUpdate( UPDATE_MY_CLAN_INFO, &OnlineUI::MultiplayerMenu::_updateUpdateMyClanInfo );
	_stateMachine.setStateEnter( UPDATE_MY_RANK_AND_RATING, &OnlineUI::MultiplayerMenu::_enterUpdateMyRankAndRating );
    _stateMachine.setStateUpdate( UPDATE_MY_RANK_AND_RATING, &OnlineUI::MultiplayerMenu::_updateUpdateMyRankAndRating );
#endif // _XBOX
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called before use case is started.  Show the first screen for the use case.
///
/// \return true if succeeded, false otherwise
///////////////////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ bool OnlineUI::MultiplayerMenu::_init( void )
{
    g_FrontEnd.GotoScreen("SCREEN_ID_MULTIPLAYER_MENU_5_00");

	ScreenMultiplayerMenu *pScreen = UseCase::GetActiveScreen<ScreenMultiplayerMenu>();
	if ( !pScreen ) return false;

	pScreen->SetOnlineName(OSI::getSingletonPtr()->getActiveAccount()->player.name);
    _updateDescription();
	// FIXME: take this out when we get MOTD done
	pScreen->SetMessageOfTheDay( "" );

#ifdef _XBOX
	_stateMachine.setCurrentState( this, UPDATE_MY_CLAN_INFO );
#else
	_stateMachine.setCurrentState( this, GET_MESSAGE_OF_THE_DAY );
#endif

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called after use case is ended.  Clean up the screens and other objects.
///
/// \return true if succeeded, false otherwise
///////////////////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ bool OnlineUI::MultiplayerMenu::_deinit( void )
{
	g_FrontEnd.RemoveAllPopups();

	if( UseCase::IsScreenActive< ScreenMultiplayerMenu >() )
		g_FrontEnd.EndScreen(0);
	g_FrontEnd.PopScreenHistory( "SCREEN_ID_MULTIPLAYER_MENU_5_00" );

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called each time use case becomes active on the stack.
///////////////////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ void OnlineUI::MultiplayerMenu::_onOpen( void )
{
	_onOpenCalled = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in multiplayer mode.
///
/// \return true to continue use case processing, false to return to application main menu.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::MultiplayerMenu::_updateMultiplayer()
{
	if( _automatedUseCaseLaunch )
	{
		_automatedUseCaseLaunch->start();
		_automatedUseCaseLaunch = 0;
		return true;
	}

    if( _onOpenCalled )
	{
		_ensureScreenIntegrity();
		_onOpenCalled = false;
	}

    ScreenMultiplayerMenu *pScreen = UseCase::GetActiveScreen<ScreenMultiplayerMenu>();
	if ( !pScreen ) return true ;

	pScreen->SetOnlineName(OSI::getSingletonPtr()->getActiveAccount()->player.name);
    _updateDescription();

	OSIPlayer* localPlayer = OSI::getSingletonPtr()->getActivePlayer();
	if( localPlayer )
	{
        pScreen->SetOnlineStatus(localPlayer->state.isOnline);
    }


    switch (pScreen->GetScreenStatus())
    {
        case ScreenMultiplayerMenu::QUICKMATCH:
            QuickMatch::getSingletonPtr()->start();
            break;

        case ScreenMultiplayerMenu::FINDMATCH:
            FindMatch::getSingletonPtr()->start();
            break;

        case ScreenMultiplayerMenu::HOSTMATCH:
            HostMatch::getSingletonPtr()->start();
            break;

        case ScreenMultiplayerMenu::FRIENDS:
#ifdef _XBOX
            UIXFriends::getSingletonPtr()->setAllowGameInvites(false);
            UIXFriends::getSingletonPtr()->start();
#elif defined(PS2) || defined(DIRECTX_PC)
            Friends::getSingletonPtr()->start();
#endif
            break;

        case ScreenMultiplayerMenu::PLAYERLIST:
#ifdef _XBOX
            UIXPlayers::getSingletonPtr()->setShowCurrentPlayers(false);
            UIXPlayers::getSingletonPtr()->start();
#elif defined(PS2) || defined(DIRECTX_PC)
            Players::getSingletonPtr()->start();
#endif
            break;

        case ScreenMultiplayerMenu::STATS:
			OnlineUI::Stats::getSingletonPtr()->start();
            break;

        case ScreenMultiplayerMenu::SETTINGS:
			g_FrontEnd.EndScreen(0); // kill the Multiplayer Menu, because we will put it back upon return
            g_messageDispatcher.SendMessageToAll( "BeginSettings", 0, INVALID_OBJECT_HANDLE );
            break;

        case ScreenMultiplayerMenu::CLAN:
            Clans::getSingletonPtr()->start();
            break;

        case ScreenMultiplayerMenu::ONLINE_STATUS:
#ifdef _XBOX
            {
				OSIXbox* osix = (OSIXbox*) OSI::getSingletonPtr();
                OSIPlayer* localPlayer = OSI::getSingletonPtr()->getActivePlayer();
			    if( localPlayer )
			    {
				    localPlayer->state.isOnline = !localPlayer->state.isOnline;
					osix->quietPublishOnlineState( localPlayer->state );
                    pScreen->SetOnlineStatus(localPlayer->state.isOnline);
                }
            }
#endif
            break;

        case ScreenMultiplayerMenu::LOGOUT:
            _stateMachine.setCurrentState(this, ASK_TO_LOGOUT);
            break;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when entering the GET_MESSAGE_OF_THE_DAY state.
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::MultiplayerMenu::_enterMessageOfTheDay()
{
    bool requestInitiatedSuccessfully = OSI::getSingletonPtr()->requestMessageOfTheDay( g_translator.GetLanguage() );

	WaitForOSIRequest::getSingletonPtr()->start(
		requestInitiatedSuccessfully, "Message of the Day", "Checking for new message of the day, please wait..."
	);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the GET_MESSAGE_OF_THE_DAY state.
///
/// \return true to continue use case processing, false to return to application main menu.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::MultiplayerMenu::_updateMessageOfTheDay()
{
	OSIRequestStatus requestStatus = OSI::getSingletonPtr()->getLastRequestStatus();
	
	if( requestStatus == STATUS_SUCCEEDED )
	{
		ScreenMultiplayerMenu *pScreen = UseCase::GetActiveScreen<ScreenMultiplayerMenu>();
		if ( !pScreen ) return true ;

		pScreen->SetMessageOfTheDay( OSI::getSingletonPtr()->getMessageOfTheDay() );
	}

	_stateMachine.setCurrentState(this, MULTIPLAYER);

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when entering the ASK_TO_LOGOUT state.
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::MultiplayerMenu::_enterAskToLogout()
{
#ifdef _XBOX
	YesNoDialog::getSingletonPtr()->start( "Sign out", "Are you sure you want to sign out of Xbox Live?", false );
#else
	YesNoDialog::getSingletonPtr()->start( "Logout", "Are you sure you want to logout?", false );
#endif 
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the ASK_TO_LOGOUT state.
///
/// \return true to continue use case processing, false to return to application main menu.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::MultiplayerMenu::_updateAskToLogout()
{
	switch( YesNoDialog::getSingletonPtr()->GetAnswer() )
	{
	case YesNoDialog::YES:
		_stateMachine.setCurrentState( this, LOGOUT );
		break;
	case YesNoDialog::NO:
	case YesNoDialog::CANCEL:
	default:
		_stateMachine.setCurrentState( this, MULTIPLAYER );
		break;
	}

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when entering the LOGOUT state.
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::MultiplayerMenu::_enterLogout()
{
    bool requestInitiatedSuccessfully = OSI::getSingletonPtr()->requestLogout();

	WaitForOSIRequest::getSingletonPtr()->start( requestInitiatedSuccessfully, "", "" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the LOGOUT state.
///
/// \return true to continue use case processing, false to return to application main menu.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::MultiplayerMenu::_updateLogout()
{
	g_playerProfile.SetAccountName( "" );
	g_playerProfile.SetDisplayName( "" );

	OSIRequestStatus requestStatus = OSI::getSingletonPtr()->getLastRequestStatus();
	
	ScreenCreateSession* pScreen = reinterpret_cast<ScreenCreateSession*>(g_FrontEnd.GetScreenHandler( "SCREEN_ID_CREATESESSION" ));
	pScreen->SetMatchName( "" );
	// Back out to login menu regardless of whether you logged out succesfully or not
	stop();

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Update the description field based on the current selection on the screen
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::MultiplayerMenu::_updateDescription()
{
    ScreenMultiplayerMenu *pScreen = UseCase::GetActiveScreen<ScreenMultiplayerMenu>();
    if (!pScreen) return;

    // NOTE: The reason that these strings aren't in an array is that the IDs are not
    // guaranteed to be sequential.  Even if they are right now, they may change
    // and break the entire thing.  So we have to use a switch and manually set the
    // string.  Boo.
    switch (pScreen->GetCurrentButtonId())
    {
        case ID_MULTIPLAYER_QUICKMATCH:
            pScreen->SetDescription("Find the first available match, or host if no matches are available.");
            break;

        case ID_MULTIPLAYER_FINDMATCH:
            pScreen->SetDescription("Find a match based on criteria you select.");
            break;

        case ID_MULTIPLAYER_HOSTMATCH:
            pScreen->SetDescription("Host a match to throw down with other players.");
            break;

        case ID_MULTIPLAYER_FRIENDS:
#ifdef _XBOX
			pScreen->SetDescription("See if your homies are on Xbox Live and join them in throwing down.");
#else
			pScreen->SetDescription("See if your homies are online and join them in throwing down.");
#endif 
            break;

        case ID_MULTIPLAYER_PLAYERLIST:
            pScreen->SetDescription("Check out who you've been hangin' with and invite them to be your friend.");
            break;

        case ID_MULITPLAYER_STATS:
            pScreen->SetDescription("See how well you stack up against other players.");
            break;

        case ID_MULTIPLAYER_SETTINGS:
            pScreen->SetDescription("Customize your character and game settings for the current profile.");
            break;

        case ID_MULTIPLAYER_CLAN:
            pScreen->SetDescription("Check out what's going down with your gang.");
            break;

        case ID_MULTIPLAYER_XBOX_STATUS:
            pScreen->SetDescription("Toggle your online status.");
            break;

        case ID_MULTIPLAYER_LOGOUT:
#ifdef _XBOX
			pScreen->SetDescription("Sign out.");
#else
			pScreen->SetDescription("Log off playing online.");
#endif 
            break;
    }
}

void OnlineUI::MultiplayerMenu::_ensureScreenIntegrity()
{
	g_FrontEnd.RemoveAllPopups();

	while( g_FrontEnd.GetActiveScreen() && !UseCase::IsScreenActive< ScreenMultiplayerMenu >() )
	{
		g_FrontEnd.EndScreen( 0 );
	}
	if( !UseCase::IsScreenActive< ScreenMultiplayerMenu >() )
	{
		// We are in a really bad state, throw up an error
		g_errorStack.SetError("Multiplayer Fatal Error", "Unknown system error has occurred.",
				CErrorStack::ERROR_FATAL, 2000);
	}
}

#ifdef _XBOX

///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::MultiplayerMenu::_enterUpdateMyClanInfo()
{
	bool requestInitiatedSuccessfully = OSI::getSingletonPtr()->requestClanUpdateMyClanInformation();
	WaitForOSIRequest::getSingletonPtr()->start( requestInitiatedSuccessfully, "", "" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::MultiplayerMenu::_updateUpdateMyClanInfo()
{
	_stateMachine.setCurrentState(this, UPDATE_MY_RANK_AND_RATING);

	OSIPlayer* activePlayer = OSI::getSingletonPtr()->getActivePlayer();
	if( activePlayer )
	{
		char fullname[OSI_PLAYER_FULL_NAME_LENGTH];
		activePlayer->GetFullName( fullname );

		g_playerProfile.SetAccountName( activePlayer->name );
		g_playerProfile.SetDisplayName( fullname );
	}
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::MultiplayerMenu::_enterUpdateMyRankAndRating()
{
	OSIXbox* osix = (OSIXbox*) OSI::getSingletonPtr();
	bool requestOK = osix->requestUpdateMyRankAndRating();
	WaitForOSIRequest::getSingletonPtr()->start( requestOK, "", "" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::MultiplayerMenu::_updateUpdateMyRankAndRating()
{
	_stateMachine.setCurrentState( this, MULTIPLAYER );

	OSIPlayer* activePlayer = OSI::getSingletonPtr()->getActivePlayer();
	if( activePlayer )
	{
		g_playerProfile.SetRank( activePlayer->statRanking );
		g_playerProfile.SetRating( activePlayer->statRating );
	}
	return true;
}



#endif // _XBOX

