///////////////////////////////////////////////////////////////////////////////////////
///  \file UseCaseUIXLogin.cpp
///
///  \brief Implementation of the UIXLogin class, which is responsible 
///   for presenting the UIX login screen on XBox.
///////////////////////////////////////////////////////////////////////////////////////

#include "TTLPCH.h"
#include "frontend/inputdriver.h"
#include "UseCaseUIXLogin.hpp"
#include "../../screens/screen_UIX.h"
#include "frontend/frontend.h"
#include "platform/Xbox/xboxctrol.h"
#include "platform/Xbox/XboxLive.h"
#include "frontend/UseCase/UseCaseDisplayDialog.hpp"
#include "frontend/UseCase/UseCaseMultiplayerMenu.hpp"
#include "frontend/UseCase/UseCaseJoinMatch.hpp"
#include "frontend/UseCase/UseCaseYesNoDialog.hpp"
#include "main/PlayerProfile.h"
#include "Voice/NetZ/CVoiceChat.h"
#include "Voice/NetZ/XBox/XHVVoiceManager.h"
#include "../../../main/ttl.h"
#include <uix.h>


///////////////////////////////////////////////////////////////////////////////////////////////////
/// Constructor: Initialize member variables
///////////////////////////////////////////////////////////////////////////////////////////////////
OnlineUI::UIXLogin::UIXLogin() : 
    _uixFontRenderer(0),
	_uixAudioPlugin(0),
    _stateMachine(WAITING_LOGIN)
{
	_stateMachine.setStateUpdate(WAITING_LOGIN, updateWaitingLogin);
	_stateMachine.setStateUpdate(REQUEST_LOGOUT, updateRequestLogout);
    _stateMachine.setStateUpdate(WAITING_LOGOUT, updateWaitingLogout);
	_stateMachine.setStateEnter( VERIFY_REBOOT, enterVerifyReboot );
	_stateMachine.setStateUpdate( VERIFY_REBOOT, updateVerifyReboot );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called before use case is started.  Show the first screen for the use case.
///
/// \return true if succeeded, false otherwise
///////////////////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ bool OnlineUI::UIXLogin::_init( void )
{
    g_FrontEnd.GotoScreen("SCREEN_ID_UIX");

	XONLINE_LOGON_STATE logonState;
	XONLINE_LOGON_STATE* validLogonState = 0;

	// if we silently logged in, log us out now
	if (CXboxLive::AreWeLoggedIn() == true)
	{
		if( SUCCEEDED( XOnlineSaveLogonState( &logonState ) ))
		{
			validLogonState = &logonState;
		}
		CXboxLive::LogOutUIX();
	}

	if (CXboxLive::IsUIXStarted())
		CXboxLive::KillUIX();

	_startUIXLogin( validLogonState );
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called after use case is ended.  Clean up the screens and other objects.
///
/// \return true if succeeded, false otherwise
///////////////////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ bool OnlineUI::UIXLogin::_deinit( void )
{
	g_FrontEnd.RemoveAllPopups();
	if( UseCase::IsScreenActive< ScreenUIX >() )
		g_FrontEnd.EndScreen(0);
	//g_FrontEnd.PopScreenHistory( "SCREEN_ID_UIX" );

	OSIXbox* osix = reinterpret_cast< OSIXbox* >( OSI::getSingletonPtr() );
	ASSERT( osix );
	// hack - requestLogout is actually instantaneous and doesn't require pumping.
	// This is called here because the fatal error code path doesn't properly call the logout
	// state.
	osix->requestLogout();

	osix->uixShutdown();
	_uixFontRenderer = 0; // deleted by UIX
	
	delete _uixAudioPlugin; // NOT deleted by UIX for some reason
	_uixAudioPlugin = 0; 

	g_CVoiceChat.releasePlatformSpecific();

	// try to get back to silent login for single player
	CXboxLive::InitUIX();
	//CXboxLive::LoginUIX(false);

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case.
///
/// \return true to continue use case processing, false to return to application main menu.
///////////////////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ bool OnlineUI::UIXLogin::update()
{
    return _stateMachine.update(this);
}

inline void ShowLoginDialog()
{
	g_FrontEnd.ShowPopup("SCREEN_ID_WAIT");
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called to start logging in to UIX.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::UIXLogin::_startUIXLogin( XONLINE_LOGON_STATE* logonState, bool allowSilent )
{
    OSIXbox* osix = reinterpret_cast< OSIXbox* >( OSI::getSingletonPtr() );
	ASSERT( osix );

	osix->uixSetVoicemailPlugin( 0 ); // paranoia hack - try to make sure XHV is properly shut down before trying to log in

	osix->uixShutdown();
	_uixFontRenderer = 0; // deleted by UIX
	
	delete _uixAudioPlugin; // NOT deleted by UIX for some reason
	_uixAudioPlugin = 0; 

	_uixFontRenderer = new UIXFontRenderer();
	_uixAudioPlugin = new UIXAudioPlugin();
    osix->uixSetTitleFontRendererPlugin( _uixFontRenderer );
	osix->uixSetTitleAudioPlugin( _uixAudioPlugin );
	osix->uixStartup();

	// prime uix with input so it won't display "insert controller" on the first frame.
	if( g_input.ControllerLocked( 0 ) )
	{
		XboxControllerInputDriver* pDriver = reinterpret_cast< XboxControllerInputDriver* >( g_input.GetController(0) );
		if( pDriver && pDriver->Connected() && pDriver->GetHandle() )
		{
			XINPUT_STATE state;
			XInputGetState( pDriver->GetHandle(), &state );
			int controllerPortIndex = g_input.GetController( 0 )->GetHeadSetPort();
			osix->uixSetInputForControllerPort( controllerPortIndex, &state );
		}
	}


	g_CVoiceChat.releasePlatformSpecific(); // paranoia hack - try to make sure XHV is properly shut down before trying to log in

	// Try retrieved game invite
	XNKID* xnkidMatchID = CXboxLive::GetInviterSessionID();
	if( xnkidMatchID && g_acceptingInvitation )
	{
		_lastAttemptedLoginType = UIX_LOGON_TYPE_RETRIEVED_GAME_INVITE;
		osix->uixBeginLogon( _lastAttemptedLoginType, 0, CXboxLive::GetBootInvite() );
		if( STATUS_FAILED != osix->getLastRequestStatus() ) { ShowLoginDialog(); return true; }
	}

	// Try retrieved logon state
	if( logonState )
	{
		_lastAttemptedLoginType = UIX_LOGON_TYPE_RETRIEVED_STATE;
		osix->uixBeginLogon( _lastAttemptedLoginType, logonState, 0 );
		if( STATUS_FAILED != osix->getLastRequestStatus() ) { ShowLoginDialog();return true; }
	}

	// Try silent logon
	if( allowSilent )
	{
		_lastAttemptedLoginType = UIX_LOGON_TYPE_SILENT;
		osix->uixBeginLogon( _lastAttemptedLoginType, 0, 0 );
		if( STATUS_FAILED != osix->getLastRequestStatus() ) { ShowLoginDialog(); return true; }
	}

	// Try normal
	_lastAttemptedLoginType = UIX_LOGON_TYPE_NORMAL;
	osix->uixBeginLogon( _lastAttemptedLoginType, 0, 0 );
	if( STATUS_FAILED != osix->getLastRequestStatus() ) { return true; }

	// Nothing worked - fail!
	stop();
	DisplayDialog::getSingletonPtr()->start("", osix->getErrorString());
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Update processing method for WAITING_LOGIN state.
///
/// \return true to continue use case processing, false to return to application main menu.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::UIXLogin::updateWaitingLogin()
{
    // Update UIX screen
    OSIXbox* osix = reinterpret_cast< OSIXbox* >( OSI::getSingletonPtr() );
	ASSERT( osix );

    // Update the login feature
	if (!osix->uixUpdateLogon())
    {
		g_FrontEnd.EndPopup(0);
		if( UIX_LOGON_TYPE_NORMAL != _lastAttemptedLoginType )
		{
			// Try normal
			if( _startUIXLogin( 0, false ) ) { return true; }
		}

		stop();
		DisplayDialog::getSingletonPtr()->start("",  osix->getErrorString());
		return false;
    }

    // See if UIX wants to reboot for some reason
	if( osix->uixMustReboot() )
	{
		_stateMachine.setCurrentState( this, VERIFY_REBOOT );
		return true;
	}

	// Return here if the login screen is still processing.
	if( !osix->uixDidLogonFinishThisFrame() ){ return true; }
    //------------
	
	// I don't know why this is here...
	g_FrontEnd.EndPopup(0);

    // Clean up text
    _uixFontRenderer->BeginFrame();

	if( osix->uixWasLogonRejectedByServer() )
	{
        stop();
	}
	else if( osix->uixWasLogonCancelledByUser() )
	{
        stop();
	}
	else // login successful
	{
		// Set the online state for the player
		OSIPlayer* localPlayer = OSI::getSingletonPtr()->getActivePlayer();
		if( localPlayer && !osix->uixWasLogonCancelledByUser() )
		{
			localPlayer->state.isOnline = true;
			osix->quietPublishOnlineState( localPlayer->state );
		}

		// save state
		CXboxLive::SaveLoginState();

		// initialize UIX voice support
		g_CVoiceChat.setMultiPlayMode( CVoiceChat::ONLINE_PLAY );
		if( g_CVoiceChat.createPlatformSpecific() )
		{
			CXHVVoiceManager* pXHVManager = ( CXHVVoiceManager * ) g_CVoiceChat.getPlatformVoiceManager();
			ASSERT( pXHVManager );
			IXHVEngine* pIXHVEngine = (IXHVEngine*) pXHVManager->GetXHVEnginePtr();
			ASSERT( pIXHVEngine );
			osix->uixSetVoicemailPlugin( pIXHVEngine );
		}
		else
		{
			osix->uixSetVoicemailPlugin( 0 );
		}

		// If we come back, it will be to logout
		_stateMachine.setCurrentState(this, REQUEST_LOGOUT);

		OSIPlayer* activePlayer = osix->getActivePlayer();
		if( activePlayer )
		{
			char fullname[OSI_PLAYER_FULL_NAME_LENGTH];
			activePlayer->GetFullName( fullname );

			g_playerProfile.SetAccountName( activePlayer->name );
			g_playerProfile.SetDisplayName( fullname );
		}

		// Check for game invite
		XNKID* xnkidMatchID = CXboxLive::GetInviterSessionID();
		if( xnkidMatchID && g_acceptingInvitation )
		{
			g_acceptingInvitation = false;
			OSIMatchID matchID;
			XNKID_to_OSIMatchID( *xnkidMatchID, matchID );
			JoinMatch::getSingletonPtr()->setMatchToJoin( matchID );
			MultiplayerMenu::getSingletonPtr()->SetAutomatedUseCaseLaunch( JoinMatch::getSingletonPtr() );
		}

		MultiplayerMenu::getSingletonPtr()->start();
	}

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Update processing method for REQUEST_LOGOUT state.
///
/// \return true to continue use case processing, false to return to application main menu.
///////////////////////////////////////////////////////////////////////////////////////////////////

bool OnlineUI::UIXLogin::updateRequestLogout()
{
    bool logoutRequest = OSI::getSingletonPtr()->requestLogout();

    if (!logoutRequest)
    {
        stop();
        return false;
    }

    _stateMachine.setCurrentState(this, WAITING_LOGOUT);

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Update processing method for WAITING_LOGOUT state.
///
/// \return true to continue use case processing, false to return to application main menu.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::UIXLogin::updateWaitingLogout()
{
    if ( OSI::getSingletonPtr()->processLastRequest() )
	{
		OSIRequestStatus requestStatus = OSI::getSingletonPtr()->getLastRequestStatus();
		if ( requestStatus == STATUS_SUCCEEDED )
		{
			g_playerProfile.SetAccountName( "" );
			g_playerProfile.SetDisplayName( "" );
			
			_startUIXLogin( 0, false );
			_stateMachine.setCurrentState( this, WAITING_LOGIN );
		}
		else
		{
			stop();
            DisplayDialog::getSingletonPtr()->start( "",  OSI::getSingletonPtr()->getErrorString() );
			//_setState( STATE_REQUEST_ACCOUNT_LIST ); // FIXME: If we failed to logout, we'll probably fail to log in!
		}
	}

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::UIXLogin::enterVerifyReboot()
{
	OSIXbox* osix = (OSIXbox*) OSI::getSingletonPtr();
	osix->uixEndFeature();
	YesNoDialog::getSingletonPtr()->start( "", "The Xbox console will restart so that you can use the Xbox Dashboard.  Any unsaved data will be lost.  Are you sure?" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::UIXLogin::updateVerifyReboot()
{
    if( YesNoDialog::getSingletonPtr()->GetAnswer() == YesNoDialog::YES )
    {
		OSIXbox* osix = (OSIXbox*) OSI::getSingletonPtr();
		osix->uixRebootNow();
    }
	else
	{
		stop();
	}
    return true;
}