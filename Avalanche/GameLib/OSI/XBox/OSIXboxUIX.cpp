
#include "pch.hpp"
#include "OSIXbox.hpp"

#include "Network/NetZ/DOHost.h"
#include "Network/NetZ/DOTalker.h"
#include "platform/xbox/xboxlive.h"

#include "GameHelper/ErrorStack.h"
#include "platform/systemi.h"
#include "network/session.h"
#include "voice/netz/xbox/XHVVoiceManager.h"
#include "voice/netz/cvoicechat.h"

///------------------------------------------------------------------------------------------------
void OSIXbox::uixStartup()
///------------------------------------------------------------------------------------------------
{
	static bool networkStarted = false;
	if( !networkStarted )
	{
		networkStarted = true;

		// tell NetZ that we're doing our own network initializing.
		//XBoxNetworking::SetInitialized( true ); 

		// initialize the network manually since netz won't let up configure XNetStartParams
		XNetStartupParams xnsp;
		memset( &xnsp, 0, sizeof(xnsp) );
		xnsp.cfgSizeOfStruct				= sizeof( XNetStartupParams );
		xnsp.cfgFlags						= 0; // default 0
		xnsp.cfgPrivatePoolSizeInPages		= 16; // 16x4k=64k for network stack, default 12 pages (48k)
		xnsp.cfgEnetReceiveQueueLength		= 8; // default 8
		xnsp.cfgIpFragMaxSimultaneous		= 4; // default 4
		xnsp.cfgIpFragMaxPacketDiv256		= 8; // default 8
		xnsp.cfgSockMaxSockets				= 64; // default 64;
		xnsp.cfgSockDefaultRecvBufsizeInK	= 16; // default 16k
		xnsp.cfgSockDefaultSendBufsizeInK	= 16; // default 16k
		xnsp.cfgKeyRegMax					= 16; // default 4
		xnsp.cfgSecRegMax					= 128; // default 32
		xnsp.cfgQosDataLimitDiv4			= 64; // default 64
		INT err = XNetStartup( &xnsp );
		if( err )
		{
			_setError( err );
			return;
		}

		WSADATA wsaData;
		err = WSAStartup( MAKEWORD( 2, 2 ), &wsaData );
		if( err )
		{
			_setError( err );
			return;
		}

		if( !XBoxNetworking::Initialize() )
		{
			_setError( E_FAIL );
			return;
		}
		
		/*const HRESULT startupHR = XOnlineStartup( NULL );
		if( FAILED( startupHR ) ) 
		{ 
			_setError( startupHR );
			return; 
		}
		*/
	}

	// Create UI plugin
	ASSERT( _fontPlugin );
	const HRESULT createUIHR = UIXCreateUIPlugin( _fontPlugin, &_uiPlugin );
	if( FAILED( createUIHR ) ) 
	{ 
		_setError( createUIHR );
		return; 
	}

	// Set UI support plugin if possible
	if( _supportPlugin )
	{
		const HRESULT supportHR = _uiPlugin->SetPluginSupport( _supportPlugin );
		if( FAILED( supportHR ) )
		{
			_setError( supportHR );
			return;
		}
	}

	// Create UIX
	//FIXME: where should this file actually live?
	const HRESULT createEngineHR = UIXCreateLiveEngine( "z:\\ttl.uix", CXboxLive::GetFilteredLanguage(), &_uix );
	if( FAILED( createEngineHR ) ) 
	{ 
		_setError( createEngineHR );
		return; 
	}

	// Set audio plugin in possible
	if( _audioPlugin ) 
	{ 
		_uix->SetAudioPlugin( _audioPlugin ); 
	}

	// Set UI plugin
	_uix->SetUIPlugin( _uiPlugin );

	// enable features and flags on UIX
	_uix->EnableFeature( UIX_LOGON_FEATURE );
	_uix->EnableFeature( UIX_FRIENDS_FEATURE );
	_uix->EnableFeature( UIX_PLAYERS_FEATURE );
	_uix->SetProperty( UIX_PROPERTY_DISPLAY_CONNECTION_ERRORS, FALSE );
	_uix->SetProperty( UIX_PROPERTY_DISPLAY_NOTIFICATIONS, TRUE );
	_uix->SetProperty( UIX_PROPERTY_ALLOW_GAME_INVITES, TRUE );
	_uix->SetProperty( UIX_PROPERTY_HOST_NO_MIGRATION, FALSE );

	if( _voicemailPlugin )
	{
		_uix->UseVoiceMail( UIX_VOICE_MAIL );
		_uix->SetProperty( UIX_PROPERTY_VOICE_MAIL_ENGINE, (DWORD) _voicemailPlugin );
		_uix->SetProperty( UIX_PROPERTY_VOICE_MAIL_TO_SPEAKERS, TRUE );
	}
	else
	{
		_uix->SetProperty( UIX_PROPERTY_VOICE_MAIL_ENGINE, 0 );
		_uix->SetProperty( UIX_PROPERTY_VOICE_MAIL_TO_SPEAKERS, TRUE );
		_uix->UseVoiceMail( 0 );
	}
}

///------------------------------------------------------------------------------------------------
//! Use this to specify input each frame, or UIX will think the controller has been unplugged.
//! \param controllerPort Which controller are you setting input data for?
//! \param input The input data, as retrieved from XInputGetState.
///------------------------------------------------------------------------------------------------
void OSIXbox::uixSetInputForControllerPort( const int controllerPort, XINPUT_STATE* const input )
///------------------------------------------------------------------------------------------------
{
	input->Gamepad.wButtons &= (~XINPUT_GAMEPAD_BACK);
	_uix->SetInput( getActivePlayer()->state.currentControllerIndex, input );
}

///------------------------------------------------------------------------------------------------
//! Keep UIX happy and let it work.  Disaterous things will happen if this isn't called on a 
//! regular basis.  Network requests will timeout, login will fail, etc.
///------------------------------------------------------------------------------------------------
bool OSIXbox::uixDoWorkEveryFrame()
///------------------------------------------------------------------------------------------------
{
	if( !_uix )
	{
		// For some reason this is called sometime between when uixShutdown is called and when
		// OSI is deleted.
		return true;
	}

	// Assume no feature exited this frame.  This is re-set in feature-specific work functions
	// when the feature reports that it has exited.
	_didAnyFeatureExitThisFrame = false;

	const HRESULT doworkHR = _uix->DoWork( &_lastUIXWorkFlags );
	ASSERT( SUCCEEDED( doworkHR ) );

	if( FAILED( doworkHR ) )
	{
		_setError( doworkHR );
		return false;
	}

	if( _lastUIXWorkFlags & UIX_DOWORK_FEATURE_EXIT )
	{
		UIX_EXIT_INFO exitInfo;
		_uix->GetExitInfo( &exitInfo );
		
		if( UIX_EXIT_LOGON_FAILED == exitInfo.ExitCode )
		{
			if( OSI_UIX_LOGON == _currentFeature )	
			{ 
				// this is a normal part of login feature
				return true; 
			}	
			else 
			{ 
				_setError( exitInfo.hr );
				return false; 
			}
		}
	}

	return true;
}

///------------------------------------------------------------------------------------------------
void OSIXbox::uixShutdown()
///------------------------------------------------------------------------------------------------
{
	if( !_uix ) { return; }

	uixSetVoicemailPlugin( 0 );

	ULONG releaseResult = _uix->Release();
	ASSERT( releaseResult == 0 );
	_uix = 0;
}

///------------------------------------------------------------------------------------------------
void OSIXbox::uixSetSupportPlugin( IPluginSupport* p )
///------------------------------------------------------------------------------------------------
{
	_supportPlugin = p;
}

///------------------------------------------------------------------------------------------------
void OSIXbox::uixSetTitleAudioPlugin( ITitleAudioPlugin* p )
///------------------------------------------------------------------------------------------------
{
	_audioPlugin = p;
}

///------------------------------------------------------------------------------------------------
void OSIXbox::uixSetTitleFontRendererPlugin( ITitleFontRenderer* p )
///------------------------------------------------------------------------------------------------
{
	_fontPlugin = p;
}

///------------------------------------------------------------------------------------------------
void OSIXbox::uixSetVoicemailPlugin( XHVEngine* p )
///------------------------------------------------------------------------------------------------
{
	_voicemailPlugin = p;

	if( _uix )
	{
		if( _voicemailPlugin )
		{
			_uix->UseVoiceMail( UIX_VOICE_MAIL );
			_uix->SetProperty( UIX_PROPERTY_VOICE_MAIL_ENGINE, (DWORD) _voicemailPlugin );
		}
		else
		{
			_uix->SetProperty( UIX_PROPERTY_VOICE_MAIL_ENGINE, 0 );
			_uix->UseVoiceMail( 0 );
		}
	}
}

///------------------------------------------------------------------------------------------------
//! If this is true, you should do whatever fast cleanup is necessary (if anything) and immediately 
//! call uixRebootNow.
//! \return True if uix must reboot.  Call uixRebootNow ASAP.
///------------------------------------------------------------------------------------------------
bool OSIXbox::uixMustReboot()
///------------------------------------------------------------------------------------------------
{
	return (_lastUIXWorkFlags & UIX_DOWORK_NEED_TO_REBOOT) != 0;
}

///------------------------------------------------------------------------------------------------
void OSIXbox::uixRebootNow()
///------------------------------------------------------------------------------------------------
{
	_uix->Reboot( 0 );
}

///------------------------------------------------------------------------------------------------
//! \return True if uix must render.  Call uixRenderNow before you next call uixDoWorkEveryFrame.
///------------------------------------------------------------------------------------------------
bool OSIXbox::uixMustRender()
///------------------------------------------------------------------------------------------------
{
	extern int g_ControllerRemoved;
	if (g_ControllerRemoved == 1)
		return false;

	return (_lastUIXWorkFlags & UIX_DOWORK_NEED_TO_RENDER) != 0;
}

///------------------------------------------------------------------------------------------------
//! \param pSurface The D3D surface that UIX will render on to.  Could be any valid surface.
///------------------------------------------------------------------------------------------------
void OSIXbox::uixRenderNow( IDirect3DSurface8* pSurface )
///------------------------------------------------------------------------------------------------
{
	ASSERT( pSurface );
	const HRESULT hr = _uix->Render( pSurface );
	if( S_OK == hr )
	{
		setLastRequestStatus( STATUS_SUCCEEDED );
	}
	else
	{
		setLastRequestStatus( STATUS_FAILED );
	}
}

///------------------------------------------------------------------------------------------------
//! If this is true, you may need to display an icon on the screen or something.  HOWEVER,
//! if you are letting uix do its work correctly, then it will display icons for notifications 
//! for you, so this function is more or less useless.
//! \return True if uix is reporting that any logged on user has a pending game invite notification
//! \return from a friend.
///------------------------------------------------------------------------------------------------
bool OSIXbox::uixIsReportingGameInviteNotification()
///------------------------------------------------------------------------------------------------
{
	if( (_lastUIXWorkFlags & UIX_DOWORK_NOTIFICATIONS) != 0 )
	{
		DWORD n;
		for( int i = 0; i < XGetPortCount(); i++ )
		{
			_uix->GetNotifications( i, UIX_NOTIFICATION_IN_GAME_FLASH, &n );
			if( (n & UIX_DOWORK_NOTIFY_GAME_INVITE) != 0 )
			{
				return true;
			}
		}
	}
	
	return false;
}

///------------------------------------------------------------------------------------------------
//! If this is true, you may need to display an icon on the screen or something.  HOWEVER,
//! if you are letting uix do its work correctly, then it will display icons for notifications 
//! for you, so this function is more or less useless.
//! \return True if uix is reporting that any logged on user has a pending new friend request.
///------------------------------------------------------------------------------------------------
bool OSIXbox::uixIsReportingFriendRequestNotification()
///------------------------------------------------------------------------------------------------
{
	if( (_lastUIXWorkFlags & UIX_DOWORK_NOTIFICATIONS) != 0 )
	{
		DWORD n;
		for( int i = 0; i < XGetPortCount(); i++ )
		{
			// found this in samples, can't find in docs
			if( ( ((UIX_PORT_0) << i) & _lastUIXWorkFlags ) != 0 )
			{
				_uix->GetNotifications( i, UIX_NOTIFICATION_IN_GAME_FLASH, &n );
				if( (n & UIX_DOWORK_NOTIFY_FRIEND_REQUEST) != 0 )
				{
					return true;
				}
			}
		}
	}
	
	return false;
}

///------------------------------------------------------------------------------------------------
//! \return True if the game should ignore all input and just let uix handle it.
///------------------------------------------------------------------------------------------------
bool OSIXbox::uixIsProcessingInput()
///------------------------------------------------------------------------------------------------
{
	extern int g_ControllerRemoved;
	if (g_ControllerRemoved == 1)
		return false;

	return (_lastUIXWorkFlags & UIX_DOWORK_PROCESSING_INPUT) != 0;
}

///------------------------------------------------------------------------------------------------
//! What it says.  Displays the UIX logon UI.
//! \param logonType Values are: UIX_LOGON_TYPE_NORMAL, UIX_LOGON_TYPE_SILENT,
//!		UIX_LOGON_TYPE_RETRIEVED_STATE, or UIX_LOGON_TYPE_RETRIEVED_GAME_INVITE.
//! \param optionalRetrievedLogonState See XOnlineRetrieveLogonState.
///------------------------------------------------------------------------------------------------
void OSIXbox::uixBeginLogon( const UIX_LOGON_TYPE logonType, 
	const XONLINE_LOGON_STATE* const optionalRetrievedLogonState,
	XONLINE_ACCEPTED_GAMEINVITE* const optionalCrossTitleInvite )
///------------------------------------------------------------------------------------------------
{
	ASSERT( _currentFeature != OSI_UIX_LOGON );

	// do logon
	UIX_LOGON_PARAMS logonParams;
	ZeroMemory( &logonParams, sizeof( logonParams ) );
	logonParams.StructSize = sizeof( logonParams );
	logonParams.LogonType = logonType;
	logonParams.LogonUserCount = 1;
	DWORD* serviceID = logonParams.LogonServiceIDs;

	*serviceID++ = XONLINE_FEEDBACK_SERVICE;
	*serviceID++ = XONLINE_MATCHMAKING_SERVICE;
	*serviceID++ = XONLINE_STATISTICS_SERVICE;
	//*serviceID++ = XONLINE_ARBITRATION_SERVICE;
	*serviceID++ = XONLINE_TEAM_SERVICE;				// for clan support
	*serviceID++ = XONLINE_SIGNATURE_SERVICE;			// for signing uploads to storage

	logonParams.pLogonState = const_cast< XONLINE_LOGON_STATE* >( optionalRetrievedLogonState );

	if( optionalCrossTitleInvite )
	{
		logonParams.pGameInvite = optionalCrossTitleInvite;
	}

	// start the feature
	CXboxLive::ResetLoginState();

	const HRESULT hr = _uix->StartFeature( UIX_LOGON_FEATURE, &logonParams );
	if( S_OK == hr )
	{
		_currentFeature = OSI_UIX_LOGON;
		setLastRequestStatus( STATUS_WORKING );
	}
	else
	{
		setLastRequestStatus( STATUS_FAILED );
	}
}

///------------------------------------------------------------------------------------------------
//! Must be called every frame, AFTER calling uixDoWorkEveryFrame.
///------------------------------------------------------------------------------------------------
bool OSIXbox::uixUpdateLogon()
///------------------------------------------------------------------------------------------------
{
	// verify that logon is running
	ASSERT( OSI_UIX_LOGON == _currentFeature );
	if( _currentFeature != OSI_UIX_LOGON )
	{
		return false;
	}

	if( !(_lastUIXWorkFlags & UIX_DOWORK_FEATURE_EXIT) ) { return true; }

	_uixCommonFeatureExit();
    
	UIX_LOGON_EXIT_DATA* logonExitData = (UIX_LOGON_EXIT_DATA*) _lastFeatureExitInfo.pExitData;
	if (!logonExitData)
	{
		if( UIX_EXIT_LOGON_USER_EXIT == _lastFeatureExitInfo.ExitCode ) { return true; }
		ASSERT( UIX_EXIT_LOGON_FAILED == _lastFeatureExitInfo.ExitCode ); // why is logonExitData NULL if the logon didn't fail?
		return false;
	}

	if( !g_input.ControllerLocked( 0 ) ) 
	{ 
		g_errorStack.SetError( " ", "Please reconnect the controller and try again.", CErrorStack::ERROR_FATAL, 7000 );
	}

//	int controllerPortIndex = g_input.GetController( 0 )->GetHeadSetPort();

	XONLINE_USER* users = XOnlineGetLogonUsers();
	if( !users ) { return false; }
	XONLINE_USER* localUser = 0;
	int controllerPortIndex = 0;
	for( int i = 0; i < XGetPortCount(); i++ )
	{
		if( users[ i ].xuid.qwUserID != 0 )
		{
			controllerPortIndex = i;
			localUser = &users[ i ];
			break;
		}
	}
	if( !localUser ) { return false; }

	// update the local player data
	OSIPlayer* localPlayer = getActivePlayer();
	ASSERT( localPlayer ); // can't be null on xbox because it points to static data initialized in constructor
	XUID_to_OSIPlayerID( localUser->xuid, localPlayer->id );
	OSIstrncpy( localPlayer->name, localUser->szGamertag, NUM_ARRAY_ELEMENTS( localPlayer->name ) );
	localPlayer->state.currentControllerIndex	= controllerPortIndex;
	localPlayer->state.isCurrentGameJoinable	= false;
	localPlayer->state.isOnline					= true;
	localPlayer->state.isPlayingGame			= false;
	localPlayer->state.isVoiceAvailable			= false;
	_setNotificationState( localPlayer->state ); // not actually async with uix, don't need to pump the task

	//{ //FIXME: REMOVE THIS
	//	ASSERT( !"Don't forget to remove stats reset call" );
	//	XUID xuid = {0,0};
	//	XONLINETASK_HANDLE handle;
	//	XOnlineStatReset( xuid, 0, 0, &handle ); // reset all stats for everyone everyhwere
	//	do{} while( XONLINETASK_S_RUNNING == XOnlineTaskContinue( handle ) );
	//	XOnlineTaskClose( handle );
	//}

#ifdef ENABLE_OSI_XBOX_LOG
	XNADDR titleAddr;
	DWORD titleAddrFlags = XNetGetTitleXnAddr( &titleAddr );

	char hwVersionString[ 256 ];
#ifdef _DEBUG
	DWORD hwVersionID = XDebugGetSystemVersion( hwVersionString, 256 );
#else
	DWORD hwVersionID = 0;
	strcpy( hwVersionString, "!_DEBUG" );
#endif

	char libVersionString[ 256 ];
#ifdef _DEBUG
	DWORD libVersionID = XDebugGetXTLVersion( libVersionString, 256 );
#else
	DWORD libVersionID = 0;
	strcpy( libVersionString, "!_DEBUG" );
#endif

	TIME_ZONE_INFORMATION tz;
	DWORD tzID = GetTimeZoneInformation( &tz );
    
	Log( "XNetGetTitleXnAddr Return flag = 0x%X\n", titleAddrFlags );
	Log( "XNetGetTitleXnAddr XNADDR::abEnet = 0x" );
	{ 
		for( int i = 0; i < 6; i++ ) 
		{	
			LogNoStamp( "%02X", (int)titleAddr.abEnet[i] ); 
		} 
	}
	LogNoStamp( "\n" );
	Log( "XNetGetTitleXnAddr XNADDR::abOnline = 0x" );
	{ 
		for( int i = 0; i < 20; i++ ) 
		{
			LogNoStamp( "%02X", (int)titleAddr.abOnline[i] ); 
		} 
	}
	LogNoStamp( "\n" );
	Log( "XNetGetTitleXnAddr XNADDR::ina = %i.%i.%i.%i\n",
		titleAddr.ina.S_un.S_un_b.s_b1, 
		titleAddr.ina.S_un.S_un_b.s_b2, 
		titleAddr.ina.S_un.S_un_b.s_b3, 
		titleAddr.ina.S_un.S_un_b.s_b4 );
	Log( "XNetGetTitleXnAddr XNADDR::inaOnline = %i.%i.%i.%i\n",
		titleAddr.inaOnline.S_un.S_un_b.s_b1, 
		titleAddr.inaOnline.S_un.S_un_b.s_b2, 
		titleAddr.inaOnline.S_un.S_un_b.s_b3, 
		titleAddr.inaOnline.S_un.S_un_b.s_b4 );
	Log( "XNetGetTitleXnAddr XNADDR::wPortOnline = %u\n", (unsigned)titleAddr.wPortOnline );
	Log( "XAutoPowerDownTimeRemaining = %lu\n", XAutoPowerDownTimeRemaining() );
	Log( "XGetAudioFlags = 0x%X\n", XGetAudioFlags() );
	Log( "XGetAVPack = 0x%X\n", XGetAVPack() );
	Log( "XGetGameRegion = 0x%X\n", XGetGameRegion() );
	Log( "XGetLanguage = 0x%X\n", XGetLanguage() );
	Log( "XGetParentalControlSetting = 0x%X\n", XGetParentalControlSetting() );
	Log( "XGetVideoFlags = 0x%X\n", XGetVideoFlags() );
	Log( "XGetVideoStandard = 0x%X\n", XGetVideoStandard() );
	Log( "XDebugGetSystemVersion = 0x%X / %s\n", hwVersionID, hwVersionString );
	Log( "XDebugGetXTLVersion = 0x%X / %s\n", libVersionID, libVersionString );
	Log( "GetTimeZoneInformation = 0x%X\n", tzID );
#endif // ENABLE_OSI_XBOX_LOG

/*
	// Get logged on user info
	XONLINE_USER* logonUsers = XOnlineGetLogonUsers();
	ASSERT( logonUsers );
	for( int userIndex = 0; userIndex < 4; userIndex++ ) 
	{ 
		if( logonUsers[ userIndex ].xuid.qwUserID != 0 ) { break; }
	}
	ASSERT( userIndex < 4 );

	XONLINETASK_HANDLE task = 0;
	HRESULT hr = 0;
	DWORD teamCount = 0;
	XUID teamXUID = {0,0};

	// Get team info for logged on user
	hr = XOnlineTeamEnumerateByUserXUID( userIndex, logonUsers[userIndex].xuid, 0, &task );
	ASSERT( SUCCEEDED( hr ) && !FAILED( hr ) );
	for(;;)
	{
		HRESULT hr = XOnlineTaskContinue( task );
		if( FAILED( hr ) ) 
		{ 
			ASSERT( SUCCEEDED( hr ) && !FAILED( hr ) );
			break; 
		}
		else if( XONLINETASK_S_SUCCESS == hr )
		{
			hr = XOnlineTeamEnumerateGetResults( task, &teamCount, &teamXUID );
			break;
		}
	}
	XOnlineTaskClose( task );	

	// report a stat for logged on user's team
	XONLINE_STAT stat;
	stat.wID = XONLINE_STAT_RATING;
	stat.type = XONLINE_STAT_LONGLONG;
	stat.llValue = 1000;
	XONLINE_STAT_PROC proc;
	proc.wProcedureID = XONLINE_STAT_PROCID_UPDATE_INCREMENT;
	proc.Update.dwConditionalIndex = 0;
	proc.Update.dwLeaderBoardID = 3;
	proc.Update.dwNumStats = 1;
	proc.Update.pStats = &stat;
	proc.Update.xuid = logonUsers[userIndex].xuid;
	hr = XOnlineStatWriteEx( 1, &proc, 0, &task );
	ASSERT( SUCCEEDED( hr ) && !FAILED( hr ) );
	for(;;)
	{
		HRESULT hr = XOnlineTaskContinue( task );
		if( FAILED( hr ) )
		{
			ASSERT( SUCCEEDED( hr ) && !FAILED( hr ) );
			break;
		}
		else if( XONLINETASK_S_SUCCESS == hr )
		{
			break;
		}
	}
	XOnlineTaskClose( task );	
*/



	return true;
}

///------------------------------------------------------------------------------------------------
bool OSIXbox::uixDidLogonFinishThisFrame()
///------------------------------------------------------------------------------------------------
{
	return _didAnyFeatureExitThisFrame && (OSI_UIX_LOGON == _lastExitedFeature);
}

///------------------------------------------------------------------------------------------------
bool OSIXbox::uixWasLogonRejectedByServer()
///------------------------------------------------------------------------------------------------
{
	ASSERT( OSI_UIX_LOGON == _lastExitedFeature );
	if( OSI_UIX_LOGON == _lastExitedFeature )
	{
		return _lastFeatureExitInfo.ExitCode == UIX_EXIT_LOGON_FAILED;
	}
	return false;
}

///------------------------------------------------------------------------------------------------
bool OSIXbox::uixWasLogonCancelledByUser()
///------------------------------------------------------------------------------------------------
{
	ASSERT( OSI_UIX_LOGON == _lastExitedFeature );
	if( OSI_UIX_LOGON == _lastExitedFeature )
	{
		return _lastFeatureExitInfo.ExitCode == UIX_EXIT_LOGON_USER_EXIT;
	}
	return false;
}

///------------------------------------------------------------------------------------------------
void OSIXbox::uixLogoff()
///------------------------------------------------------------------------------------------------
{
	// purge the local player data
	OSIPlayer* localPlayer = getActivePlayer();
	ASSERT( localPlayer ); // can't be null on xbox
	HRESULT logoffHR = _uix->LogOff(); // causes active features to exit automatically
	_currentFeature = OSI_UIX_NO_FEATURE_ACTIVE;
}

///------------------------------------------------------------------------------------------------
void OSIXbox::uixBeginFriendList( const OSIPlayerID* const optionalInitialSelectedFriend )
///------------------------------------------------------------------------------------------------
{
	if( !isOkToRequest() ) { return; }

	UIX_FRIENDS_PARAMS friendsParams;
	friendsParams.SelectedFriendXUID.qwUserID		= 0;
	friendsParams.SelectedFriendXUID.dwUserFlags	= 0;
	friendsParams.SignOutEnabled					= FALSE;
	friendsParams.StructSize						= sizeof( friendsParams );
	friendsParams.UserPort							= getActivePlayer()->state.currentControllerIndex;

	_uix->SetProperty( UIX_PROPERTY_ALLOW_GAME_INVITES, _allowPlayerToInviteFriends );

	const HRESULT hr = _uix->StartFeature( UIX_FRIENDS_FEATURE, &friendsParams );
	if( S_OK == hr )
	{
		_currentFeature = OSI_UIX_FRIENDS;
		setLastRequestStatus( STATUS_WORKING );
	}
	else
	{
		setLastRequestStatus( STATUS_FAILED );
	}
}

///------------------------------------------------------------------------------------------------
void OSIXbox::uixUpdateFriendList()
///------------------------------------------------------------------------------------------------
{
	if( !isOkToRequest() ) 
	{ 
		_uix->EndFeature();
	}

	// verify that logon is running
	ASSERT( OSI_UIX_FRIENDS == _currentFeature );
	if( _currentFeature != OSI_UIX_FRIENDS )
	{
		return;
	}

	if( _lastUIXWorkFlags & UIX_DOWORK_FEATURE_EXIT )
	{
		_uixCommonFeatureExit();
	}
}

///------------------------------------------------------------------------------------------------
bool OSIXbox::uixDidFriendListFinishThisFrame()
///------------------------------------------------------------------------------------------------
{
	return _didAnyFeatureExitThisFrame && (OSI_UIX_FRIENDS == _lastExitedFeature);
}

///------------------------------------------------------------------------------------------------
//! It's only valid to call this after the friend list feature has exited.  It will always return
//! false for an invalid call.
///------------------------------------------------------------------------------------------------
bool OSIXbox::uixWasFriendListInviteAcceptedForThisTitle()
///------------------------------------------------------------------------------------------------
{
	const XONLINE_FRIEND* friendInfo;
	if( _uixCommonGetFriendExitInvite( friendInfo ) && friendInfo )
	{
		bool sameSession = false;
		if( getCurrentMatch() )
		{
			XNKID session;
			OSIMatchID_to_XNKID( getCurrentMatch()->id, session );
			if( !memcmp( &session, &friendInfo->sessionID, sizeof(session) ) )
			{
				sameSession = true;
			}
		}
		return !sameSession && (friendInfo->dwTitleID == s_ThisTitleID);
	}
	else
	{
		return false;
	}
}

///------------------------------------------------------------------------------------------------
//! It's only valid to call this after the friend list feature has exited.  It will always return
//! false for an invalid call.
///------------------------------------------------------------------------------------------------
bool OSIXbox::uixWasFriendListInviteAcceptedForOtherTitle()
///------------------------------------------------------------------------------------------------
{
	const XONLINE_FRIEND* friendInfo;
	if( _uixCommonGetFriendExitInvite( friendInfo ) )
	{
		return friendInfo->dwTitleID != s_ThisTitleID;
	}
	else
	{
		return false;
	}
}

///------------------------------------------------------------------------------------------------
bool OSIXbox::uixGetFriendForAcceptedInvite( OSIPlayerID& getFriendForAcceptedInvite )
///------------------------------------------------------------------------------------------------
{
	const XONLINE_FRIEND* friendInfo;
	if( _uixCommonGetFriendExitInvite( friendInfo ) )
	{
		XUID_to_OSIPlayerID( friendInfo->xuid, getFriendForAcceptedInvite );
		return true;
	}
	else
	{
		return false;
	}
}

///------------------------------------------------------------------------------------------------
void OSIXbox::uixSetAllowPlayerToInviteFriends( const bool allowPlayerToInviteFriends )
///------------------------------------------------------------------------------------------------
{
	_allowPlayerToInviteFriends = allowPlayerToInviteFriends;
}

///------------------------------------------------------------------------------------------------
bool OSIXbox::uixGetSelectedFriendInFriendList( OSIPlayerID& id )
///------------------------------------------------------------------------------------------------
{
	ASSERT( OSI_UIX_FRIENDS == _currentFeature );
	if( _currentFeature != OSI_UIX_FRIENDS )
	{
		return false;
	}

	UIX_SELECTION_INFO selectionInfo;
	const HRESULT getSelectionHR = _uix->GetSelectionInfo( &selectionInfo );
	if( FAILED( getSelectionHR ) )
	{
		_setError( getSelectionHR );
		return false;
	}

	XUID_to_OSIPlayerID( selectionInfo.SelectedFriend.xuid, id );
	return true;
}

///------------------------------------------------------------------------------------------------
void OSIXbox::uixBeginPlayerList( const OSIPlayerID* const optionalInitialSelectedPlayer )
///------------------------------------------------------------------------------------------------
{
	if( !isOkToRequest() ) { return; }

	UIX_PLAYERS_PARAMS playersParams;
	playersParams.DisplayType = UIX_PLAYERS_DISPLAY_DEPARTED_PLAYERS;
	if( _showCurrentPlayersInPlayersList )
	{
		playersParams.DisplayType |= UIX_PLAYERS_DISPLAY_CURRENT_PLAYERS;
	}
	playersParams.FilterFlags = 0;
	playersParams.pPlayerControllingScreen = 0;
	playersParams.SortCurrentPlayersList = FALSE;
	playersParams.StructSize = sizeof( playersParams );
	playersParams.UserPort = getActivePlayer()->state.currentControllerIndex;

	XUID selected;
	if( optionalInitialSelectedPlayer )
	{
		OSIPlayerID_to_XUID( *optionalInitialSelectedPlayer, selected );
		playersParams.pSelectedPlayerXUID = &selected;
	}
	else
	{
		playersParams.pSelectedPlayerXUID = NULL;
	}

	const HRESULT hr = _uix->StartFeature( UIX_PLAYERS_FEATURE, &playersParams );
	if( S_OK == hr )
	{
		_currentFeature = OSI_UIX_PLAYERS;
		setLastRequestStatus( STATUS_WORKING );
	}
	else
	{
		setLastRequestStatus( STATUS_FAILED );
	}
}

///------------------------------------------------------------------------------------------------
void OSIXbox::uixSetShowCurrentPlayersInPlayersList( const bool showCurrentPlayers )
///------------------------------------------------------------------------------------------------
{
	_showCurrentPlayersInPlayersList = showCurrentPlayers;
}

///------------------------------------------------------------------------------------------------
void OSIXbox::uixUpdatePlayerList()
///------------------------------------------------------------------------------------------------
{
	if( !isOkToRequest() ) 
	{ 
		_uix->EndFeature();
	}

	// verify that logon is running
	ASSERT( OSI_UIX_PLAYERS == _currentFeature );
	if( _currentFeature != OSI_UIX_PLAYERS )
	{
		return;
	}

	if( _lastUIXWorkFlags & UIX_DOWORK_FEATURE_EXIT )
	{
		_uixCommonFeatureExit();
	}
}

///------------------------------------------------------------------------------------------------
bool OSIXbox::uixDidPlayerListFinishThisFrame()
///------------------------------------------------------------------------------------------------
{
	return _didAnyFeatureExitThisFrame && (OSI_UIX_PLAYERS == _lastExitedFeature);
}

///------------------------------------------------------------------------------------------------
bool OSIXbox::uixGetSelectedPlayerInPlayerList( OSIPlayerID& id )
///------------------------------------------------------------------------------------------------
{
	return false;
}

///------------------------------------------------------------------------------------------------
void OSIXbox::_uixCommonFeatureExit()
///------------------------------------------------------------------------------------------------
{
	_lastExitedFeature = _currentFeature;
	_currentFeature = OSI_UIX_NO_FEATURE_ACTIVE;
	_uix->GetExitInfo( &_lastFeatureExitInfo);
	_didAnyFeatureExitThisFrame = true;

	if( FAILED( _lastFeatureExitInfo.hr ) )
	{
		_setError( _lastFeatureExitInfo.hr );
		setLastRequestStatus( STATUS_FAILED );
	}
	else
	{
		setLastRequestStatus( STATUS_SUCCEEDED );
	}
}


///------------------------------------------------------------------------------------------------
bool OSIXbox::_uixCommonGetFriendExitInvite( const XONLINE_FRIEND*& friendInfo )
///------------------------------------------------------------------------------------------------
{
	if( _currentFeature != OSI_UIX_NO_FEATURE_ACTIVE || _lastExitedFeature != OSI_UIX_FRIENDS )
	{
		return false;
	}

	ASSERT( _lastFeatureExitInfo.FeatureID == UIX_FRIENDS_FEATURE );
	if( _lastFeatureExitInfo.FeatureID != UIX_FRIENDS_FEATURE )
	{
		return false;
	}

	// this will point to XONLINE_FRIEND if an invite was accepted, it will be NULL otherwise
	if( _lastFeatureExitInfo.pExitData )
	{
		friendInfo = (XONLINE_FRIEND*) _lastFeatureExitInfo.pExitData;
		return true;
	}
	else
	{
		return false;
	}
}

///------------------------------------------------------------------------------------------------
void OSIXbox::uixEndFeature()
///------------------------------------------------------------------------------------------------
{
	_uix->EndFeature();
}

///------------------------------------------------------------------------------------------------
void OSIXbox::uixSetPlayVoicemailOnSpeakers( bool voicemailOnSpeakers )
///------------------------------------------------------------------------------------------------
{
	if( !_uix ) { return; }
	
	// TRUE is the default value for this property as specified in the SDK docs.
	static bool s_currentValue = true;

	if( voicemailOnSpeakers == s_currentValue ) { return; }

	s_currentValue = voicemailOnSpeakers;
	_uix->SetProperty( UIX_PROPERTY_VOICE_MAIL_TO_SPEAKERS, voicemailOnSpeakers );
}
