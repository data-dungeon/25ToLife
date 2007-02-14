///////////////////////////////////////////////////////////////////////////////////////
///  \file UseCaseUIXPlayers.cpp
///
///  \brief Implementation of the UIXPlayers class, which is responsible 
///   for presenting the UIX Players screen on XBox.
///////////////////////////////////////////////////////////////////////////////////////

#include "TTLPCH.h"
#include "frontend/inputdriver.h"
#include "UseCaseUIXPlayers.hpp"
#include "../../screens/screen_UIX.h"
#include "frontend/frontend.h"
#include "platform/Xbox/xboxctrol.h"
#include "frontend/UseCase/UseCaseDisplayDialog.hpp"
#include "frontend/UseCase/UseCaseMultiplayerMenu.hpp"
#include "XboxUIXFontRenderer.hpp"
#include "Voice/NetZ/CVoiceChat.h"
#include "Voice/NetZ/XBox/XHVVoiceManager.h"


///////////////////////////////////////////////////////////////////////////////////////////////////
/// Constructor: Initialize member variables
///////////////////////////////////////////////////////////////////////////////////////////////////
OnlineUI::UIXPlayers::UIXPlayers() : 
    _inMatchLobby(false),
    _stateMachine(NORMAL)
{
	_stateMachine.setStateUpdate( NORMAL, &OnlineUI::UIXPlayers::_updateNormal );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called before use case is started.  Show the first screen for the use case.
///
/// \return true if succeeded, false otherwise
///////////////////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ bool OnlineUI::UIXPlayers::_init( void )
{
	CXHVVoiceManager* pXHVManager = NULL;
	pXHVManager = ( CXHVVoiceManager * ) g_CVoiceChat.getPlatformVoiceManager();
	if( pXHVManager && pXHVManager->GetXHVEnginePtr() )
	{
		pXHVManager->MutelistSuspend();
	}

    // Start Players screen
    OSIXbox* osix = reinterpret_cast< OSIXbox* >( OSI::getSingletonPtr() );
	ASSERT( osix );
   	osix->uixSetShowCurrentPlayersInPlayersList( _inMatchLobby );
	osix->uixBeginPlayerList( NULL );
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
/*virtual*/ bool OnlineUI::UIXPlayers::_deinit( void )
{
    OSIXbox* osix = reinterpret_cast< OSIXbox* >( OSI::getSingletonPtr() );
	ASSERT( osix );

	osix->uixEndFeature(); // make sure friends interface is gone
	g_FrontEnd.RemoveAllPopups();
    if( UseCase::IsScreenActive< ScreenUIX >() )
		g_FrontEnd.EndScreen(0);
	//g_FrontEnd.PopScreenHistory( "SCREEN_ID_UIX" );

	CXHVVoiceManager* pXHVManager = NULL;
	pXHVManager = ( CXHVVoiceManager * ) g_CVoiceChat.getPlatformVoiceManager();
	if( pXHVManager && pXHVManager->GetXHVEnginePtr() )
	{
		pXHVManager->MutelistResume();
	}

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case.
///
/// \return true to continue use case processing, false to return to application main menu.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::UIXPlayers::_updateNormal()
{
    OSIXbox* osix = reinterpret_cast< OSIXbox* >( OSI::getSingletonPtr() );
	ASSERT( osix );

	// update the feature
	osix->uixUpdatePlayerList();
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
	if( osix->uixDidPlayerListFinishThisFrame() )
	{
		stop();
	}

    return true;
}
