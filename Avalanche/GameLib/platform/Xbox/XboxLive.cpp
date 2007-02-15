//-----------------------------------------------------------------------------
// File: XboxLive.cpp
//
// A lot of what is here is to handle things like Silent Login for Live Aware,
// starting UIX, friends lists, player lists, etc.
//-----------------------------------------------------------------------------

#include "platform/PlatformPCH.h"


#include "Math/MathUndeprecate.h"
#include <xtl.h>
#include "Math/MathDeprecate.h"
#include <xgraphics.h>
#include <tchar.h> 
#include <cstdio>
#include <cassert>
#include <xonline.h>
#include <UIX.h>
#include "Network\SessionMgr.h"
#include "Voice/NetZ/CVoiceChat.h"
#include "Voice/NetZ/XBox/XHVVoiceManager.h"
#include "EngineHelper/sfx.h"

#include "XboxLive.h"

extern LPDIRECT3DDEVICE8		DirectX_Device;
//-----------------------------------------------------------------------------
// Vars

XONLINETASK_HANDLE	g_hLogonTask = NULL;
HRESULT					g_LogonAuthenticate = XONLINETASK_S_RUNNING;
HRESULT					g_LiveStatus = XONLINE_S_LOGON_CONNECTION_ESTABLISHED;
static	bool			g_LoggedIn = false;
static	bool			g_LoginFailed = true;
static	bool			g_UIXStarted = false;
static	bool			g_LoggingIn = false;
static	int			g_LastLoginStatus = CXBOXLIVE_NOT_SIGNED_IN;
static	UIX_EXIT_CODE_TYPE	g_ExitCode = UIX_EXIT_NONE;
static	XONLINE_FRIEND*		g_FriendData = NULL;
static	PXONLINE_USER			g_User;
static	XONLINE_LOGON_STATE	*g_LoginState = NULL;
static	bool			g_InUIX = false;
static	bool			g_InviteFound = false;
static	int			g_InviteType = CXBOXLIVE_NOINVITE;
static	XONLINE_ACCEPTED_GAMEINVITE g_BootInvite;
static	XNKID*		g_SessionID = NULL;
static	bool			g_AppearOffline = false;
static	bool			g_SilentSignIn = false;
static	bool			g_LoginDisabled = false;
static	int			g_ControllerPortIndex = 0;
static	CXboxLiveSoundNameMap*	g_SoundMap = NULL;
static	int			g_SoundMapEntries = 0;

ITitleFontRenderer*		cFontPlugin;	
ITitleAudioPlugin*		cAudioPlugin;		
ITitleUIPlugin*			cUiPlugin;			
IPluginSupport*			cSupportPlugin;	
ILiveEngine*				cUix;	

CXboxLiveFont*				cFontRendererPtr;

// for now allow matchmaking
const DWORD Services[] = { XONLINE_MATCHMAKING_SERVICE,
									XONLINE_FEEDBACK_SERVICE,
									XONLINE_STATISTICS_SERVICE,
									XONLINE_TEAM_SERVICE,
									XONLINE_SIGNATURE_SERVICE };	

const DWORD dwNumServices = sizeof( Services ) / sizeof( Services[0] );
ControllerInputDriver::HeadSetStatus s_headSetStatus = ControllerInputDriver::UNINITIALIZED;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Setup Xbox Live for single player loop

bool CXboxLive::Init(void)
{
	g_LoggedIn = false;
	g_LoginFailed = true;
	g_SessionID = NULL;

	HRESULT hr = XOnlineStartup( NULL );

	if (SUCCEEDED(hr))
		return true;
	else
		return false;
}

//-----------------------------------------------------------------------------
// Go to dashboard - login errors (account management) etc

void	CXboxLive::GotoDashboard(int Type)
{
	LD_LAUNCH_DASHBOARD	ld;

	ld.dwContext = 0;
	ld.dwParameter1 = 0;
	ld.dwParameter2 = 0;

	switch	(Type)
	{
	case	CXBOXLIVE_NETWORK_CONFIGURATION:
		ld.dwReason = XLD_LAUNCH_DASHBOARD_NETWORK_CONFIGURATION;
		break;
	case	CXBOXLIVE_NEW_ACCOUNT:
		ld.dwReason = XLD_LAUNCH_DASHBOARD_NEW_ACCOUNT_SIGNUP;
		break;
	case	CXBOXLIVE_ACCOUNT_MANAGEMENT:
		ld.dwReason = XLD_LAUNCH_DASHBOARD_ACCOUNT_MANAGEMENT;
		break;
	case	CXBOXLIVE_ONLINE_MENU:
		ld.dwReason = XLD_LAUNCH_DASHBOARD_ONLINE_MENU;
		break;
	default:
		break;
	}

	DWORD	result = XLaunchNewImage(NULL, (PLAUNCH_DATA)&ld);
	return;
}

//-----------------------------------------------------------------------------
// Silent Login?

bool CXboxLive::SilentLoginOn(void)
{
	return g_SilentSignIn;
}

//-----------------------------------------------------------------------------
// Toggle Silent Login

void CXboxLive::SetSilentLogin(bool Flag)
{
	g_SilentSignIn = Flag;
}

//-----------------------------------------------------------------------------
// Are we logged in?

bool CXboxLive::AreWeLoggedIn(void)
{
	return g_LoggedIn;
}

//-----------------------------------------------------------------------------
// Get Invite Found flag

bool CXboxLive::GetInviteFound(void)
{
	return g_InviteFound;
}

//-----------------------------------------------------------------------------
// Set Invite Found flag

void CXboxLive::SetInviteFound(bool Flag)
{
	g_InviteFound = Flag;
}

//-----------------------------------------------------------------------------
// Get authenticate state

HRESULT CXboxLive::GetAuthenticate(void)
{
	return	g_LogonAuthenticate;
}

//-----------------------------------------------------------------------------
// Are we logging in?

bool CXboxLive::AreWeLoggingIn(void)
{
	return g_LoggingIn;
}

//-----------------------------------------------------------------------------
// Did sign-in fail?

bool CXboxLive::LogInFailed(void)
{
	return g_LoginFailed;
}

//-----------------------------------------------------------------------------
// Setup Xbox Live for single player loop

void CXboxLive::LogOut(void)
{
	if( g_hLogonTask )
	{
		// Finally, signoff by closing the logon task handle.
		dbgPrint("Signing Off");
		XOnlineTaskClose( g_hLogonTask );    
		g_hLogonTask = NULL;
		g_LoginFailed = true;
		ResetLoginState();
	}
    
	XOnlineCleanup();    

	g_LoggedIn = false;
}

//-----------------------------------------------------------------------------
// Setup Xbox Live for single player loop

void CXboxLive::LogOutUIX(void)
{
	if (g_UIXStarted)    
	{
		cUix->SetProperty(UIX_PROPERTY_VOICE_MAIL_ENGINE, NULL);
		cUix->LogOff();
		g_CVoiceChat.releasePlatformSpecific();
		g_LoggedIn = false;
		g_LastLoginStatus = CXBOXLIVE_NOT_SIGNED_IN;
	}
}

//-----------------------------------------------------------------------------
// Toggle online status

void CXboxLive::ToggleOnlineStatus( void )
{
	SetOnlineStatus( !GetOnlineStatus() );
}

//-----------------------------------------------------------------------------
// Get Auto Logon Status

bool CXboxLive::GetAutoLogonStatus( void )
{
	DWORD	flag = XGetAutoLogonFlag();

	if (flag == XC_AUTO_LOGON_NOT_ALLOWED)
		return (false);
	else
		return (true);
}

//-----------------------------------------------------------------------------
// Set the online status

void CXboxLive::SetOnlineStatus(bool AppearOffline)
{
	g_AppearOffline = AppearOffline;

	XNKID nkid;
	memset( &nkid, 0, sizeof( nkid ) );

	if (cUix)
		cUix->NotificationSetState(g_ControllerPortIndex, g_AppearOffline, nkid, 0, NULL);

	return;
}

//-----------------------------------------------------------------------------
// Get the online status

bool CXboxLive::GetOnlineStatus(void)
{
	return g_AppearOffline;
}

//-----------------------------------------------------------------------------
// Get Login State

int CXboxLive::GetLoginStatus(void)
{
	return (g_LastLoginStatus);
}

//-----------------------------------------------------------------------------
// Attempt Silent Login

bool CXboxLive::SilentLogin(void)
{
	bool	result = true;

	g_LoggingIn = true;

	// clear state.. only needed from Multiplayer login
	ResetLoginState();

	// attempt logon
	HRESULT hr = XOnlineSilentLogon( Services, dwNumServices, NULL, &g_hLogonTask ); 
	
	// Check for errors
	// A title can decide how to respond to errors.  For example, it might
	// decide to continue without signing on the user.

	switch( hr )
	{
	case S_OK:   
		// XOnlineSilentLogon succeeded
		dbgPrint("Logon Successful");
		g_LoginFailed = false;
		break; 
	case XONLINE_E_SILENT_LOGON_DISABLED:
		// Silent Login is not enabled on this console
		dbgPrint("Silent Logon Not Enabled");
		result = false;
		break;
	case XONLINE_E_SILENT_LOGON_NO_ACCOUNTS:
		// no user accounts found
		dbgPrint("User Account Not Found");
		result = false;
		break;
	case XONLINE_E_SILENT_LOGON_PASSCODE_REQUIRED:
		// Password required, cannot proceed
		dbgPrint("Password Required For This Account");
		result = false;
		break;
	case XONLINE_E_LOGON_NO_NETWORK_CONNECTION:
		// Sign on failed because no network connection was
		// detected.  A title must give the player the
		// option of accessing the network configuration of the online dash
		dbgPrint("No Network Connection Found");
		result = false;
		break;
	default:
		// This should never happen
		dbgPrint("OOOPPPPPSSS");
		ASSERT(FALSE);
		result = false;
    }

	if (result == false)
		g_LoggingIn = false;

	return (result);
}

//-----------------------------------------------------------------------------
// Attempt Silent Login - UIX

bool CXboxLive::LoginUIX(void)
{
	HRESULT hr;
	XONLINE_LOGON_STATE*		state;

	g_SessionID	= NULL;
	g_LoggingIn = true;

	// do we have a saved state?
	state = CXboxLive::GetLoginState();

	UIX_LOGON_PARAMS    LogonParams = {0};

	if (state == NULL)
	{
		if (g_SilentSignIn)
		{
			LogonParams.LogonType = UIX_LOGON_TYPE_SILENT;
			LogonParams.pLogonState = 0;
		}
		else
		{
			g_InUIX = true;
			LogonParams.LogonType = UIX_LOGON_TYPE_NORMAL;
			LogonParams.pLogonState = 0;
		}
	}
	else
	{
		LogonParams.LogonType = UIX_LOGON_TYPE_RETRIEVED_STATE;
		LogonParams.pLogonState = state;
	}

	LogonParams.StructSize = sizeof( LogonParams );
	LogonParams.LogonUserCount = 1;
	LogonParams.LogonServiceIDs[0] = XONLINE_MATCHMAKING_SERVICE;
	LogonParams.LogonServiceIDs[1] = XONLINE_FEEDBACK_SERVICE;
	LogonParams.LogonServiceIDs[2] = XONLINE_STATISTICS_SERVICE;
	LogonParams.LogonServiceIDs[3] = XONLINE_TEAM_SERVICE;
	LogonParams.LogonServiceIDs[4] = XONLINE_SIGNATURE_SERVICE;
	LogonParams.pGameInvite = NULL;

	// Start the process of signing in to Live
	hr = cUix->StartFeature( UIX_LOGON_FEATURE, &LogonParams );

	if (hr != S_OK)
		return false;
	else
		return true;
}

//-----------------------------------------------------------------------------
// Get Last Exit Code (clears the result)

UIX_EXIT_CODE_TYPE	CXboxLive::GetLastExitCode(void)
{
	UIX_EXIT_CODE_TYPE	code = g_ExitCode;
//	g_ExitCode = UIX_EXIT_NONE;
	return (code);
}

//-----------------------------------------------------------------------------
// Returns true and clears the last exit code if the exit condition has occured

bool CXboxLive::CheckExitCondition( UIX_EXIT_CODE_TYPE ExitCode )
{
	if( GetLastExitCode() != ExitCode )
		return false;

	ClearLastExitCode();
	return true;
}



//-----------------------------------------------------------------------------
// Set Last Exit Code (clears the result

void	CXboxLive::ClearLastExitCode(void)
{
	g_ExitCode = UIX_EXIT_NONE;
}

//-----------------------------------------------------------------------------
// Get Friend Data from Invite request

XONLINE_FRIEND*	CXboxLive::GetFriendData(void)
{
	return (g_FriendData);
}

//-----------------------------------------------------------------------------
// Get Name of inviter

char*	CXboxLive::GetInvitersName(void)
{
	return (g_FriendData->szGamertag);
}

char*	CXboxLive::GetBootInvitersName(void)
{
	return	(g_BootInvite.InvitingFriend.szGamertag );
}

XONLINE_ACCEPTED_GAMEINVITE* CXboxLive::GetBootInvite()
{
	return &g_BootInvite;
}

//-----------------------------------------------------------------------------
// Get Friend Data from Invite request

char*	CXboxLive::GetUserName(void)
{
	if( g_User )
		return (g_User->szGamertag);
	return( NULL );
}

//-----------------------------------------------------------------------------
// End UIX Feature

bool CXboxLive::EndUIXFeature(void)
{
	HRESULT	hr;

	if (cUix)
	{
		hr = cUix->EndFeature();

		if (hr == S_OK)
			return true;
		else
			return false;
	}
	else
		return false;
}

//-----------------------------------------------------------------------------
// Login Update - Called once per game loop

void CXboxLive::UIXUpdate(DWORD dwFlags)
{

	// check ethernet connection is still valid
	// if its removed, attempt logout, and set status to logged off.
	if (XNetGetEthernetLinkStatus() == 0)
	{
		// are we logged in
		if (g_LoggedIn || g_LoggingIn)
		{		
			if (g_LoggingIn)
			{
				dbgPrint("Not signed: Network Cable Disconnected");
				g_LoginFailed = true;
			}

			LogOutUIX();
			g_LoggedIn = false;
			g_LoggingIn = false;
			g_SilentSignIn = false;

		}
		g_LastLoginStatus = CXBOXLIVE_NOT_SIGNED_IN;
	}
	// Eidos doesnt want it to show ethernet status.. gay!
//pab	else
//pab	{
//pab		if (g_LastLoginStatus == CXBOXLIVE_ETHERNET_DISCONNECTED)
//pab			g_LastLoginStatus = CXBOXLIVE_NOT_SIGNED_IN;
//pab	}

	// render
	if (dwFlags & UIX_DOWORK_NEED_TO_RENDER)
	{
		IDirect3DSurface8* backbuf;

		DirectX_Device->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &backbuf );
		CXboxLive::GetLiveEngine()->Render(backbuf);
		backbuf->Release();
	}

	// reboot??
	if (dwFlags & UIX_DOWORK_NEED_TO_REBOOT)			// on accepted cross title invite
		CXboxLive::GetLiveEngine()->Reboot(0);

	// feature exit
	if ( dwFlags & UIX_DOWORK_FEATURE_EXIT )
	{
		UIX_EXIT_INFO exitInfo;
 		BOOL bCrossGameInvite = FALSE;
		cUix->GetExitInfo( &exitInfo );

		g_ExitCode = exitInfo.ExitCode;

		switch ( exitInfo.ExitCode )
		{
			case UIX_EXIT_NONE:
				break;

			// logged in!!
			case UIX_EXIT_LOGON_SUCCESSFUL:
			{
				HRESULT	hr;
				int		status = 1;

				hr = XOnlineGetServiceInfo(XONLINE_MATCHMAKING_SERVICE, NULL);
				if (hr != S_OK)
					status = 0;
				hr = XOnlineGetServiceInfo(XONLINE_FEEDBACK_SERVICE, NULL);
				if (hr != S_OK)
					status = 0;
				hr = XOnlineGetServiceInfo(XONLINE_STATISTICS_SERVICE, NULL);
				if (hr != S_OK)
					status = 0;
				hr = XOnlineGetServiceInfo(XONLINE_TEAM_SERVICE, NULL);
				if (hr != S_OK)
					status = 0;
				hr = XOnlineGetServiceInfo(XONLINE_SIGNATURE_SERVICE, NULL);
				if (hr != S_OK)
					status = 0;

				// we failed to start a feature.. we should log off live
				if (status == 0)
				{
					LogOutUIX();
					g_LoginFailed = true;
					g_LastLoginStatus = CXBOXLIVE_NOT_SIGNED_IN;
					g_InUIX = false;
					return;
				}

				XNKID nkid;
				memset( &nkid, 0, sizeof( nkid ) );
				g_User = XOnlineGetLogonUsers();

				// check for pending message or management issues on account
				if (g_User[0].hr != S_OK)
				{
					if (g_User[0].hr == XONLINE_S_LOGON_USER_HAS_MESSAGE)
					{
						g_LoginFailed = false;
						g_LoggingIn = false;
						g_LoggedIn = true;
						g_LastLoginStatus = CXBOXLIVE_SIGNED_IN_MESSAGE_PENDING;
					}
					else if (g_User[0].hr == XONLINE_E_LOGON_USER_ACCOUNT_REQUIRES_MANAGEMENT)
					{
						g_LastLoginStatus = 	CXBOXLIVE_REQUIRES_MANAGEMENT;
						g_LoginFailed = true;
						g_LoggedIn = false;		
						g_InUIX = false;
						return;
					}
				}
				else
				{
					g_LoginFailed = false;
					g_LoggingIn = false;
					g_LoggedIn = true;
					g_LastLoginStatus = CXBOXLIVE_SIGNED_IN;
				}

				// set controller port for notifications
				if (g_SilentSignIn)
					g_ControllerPortIndex = 0;
				else
				{
					XONLINE_USER* localUser = 0;
					int controllerPortIndex = 0;

					for( int i = 0; i < XGetPortCount(); i++ )
					{
						if( g_User[ i ].xuid.qwUserID != 0 )
						{
							controllerPortIndex = i;
							localUser = &g_User[ i ];
							break;
						}
					}

					if( !localUser ) 
					{
						g_LastLoginStatus = CXBOXLIVE_SIGNIN_FAILED;
						g_LoginFailed = true;
						g_LoggedIn = false;		
						g_InUIX = false;
						return;
					}

					g_ControllerPortIndex = controllerPortIndex;
				}

				CXHVVoiceManager* pXHVManager = NULL;
				IXHVEngine* pIXHVEngine = NULL;
				g_CVoiceChat.createPlatformSpecific();

				pXHVManager = ( CXHVVoiceManager * ) g_CVoiceChat.getPlatformVoiceManager();
				ASSERT( pXHVManager );
				pIXHVEngine = (IXHVEngine*) pXHVManager->GetXHVEnginePtr();
//				ASSERT( pIXHVEngine );

				if (pIXHVEngine)
				{
					cUix->UseVoiceMail( UIX_VOICE_MAIL );
					cUix->SetProperty(UIX_PROPERTY_VOICE_MAIL_ENGINE, (DWORD)pIXHVEngine);
					cUix->SetProperty(UIX_PROPERTY_VOICE_MAIL_TO_SPEAKERS, TRUE);
					s_headSetStatus = ControllerInputDriver::UNINITIALIZED;
				}

				// set us to visible online
				cUix->NotificationSetState(g_ControllerPortIndex, XONLINE_FRIENDSTATE_FLAG_ONLINE, nkid, 0, NULL);

				g_InUIX = false;
				return;
			}

			case UIX_EXIT_LOGON_FAILED:
			{
				g_LoggingIn = false;
				g_LoginFailed = true;

				switch ( exitInfo.hr )
				{
					case XONLINE_E_SILENT_LOGON_DISABLED:
						dbgPrint("Not signed in (auto-sign in disabled)");
						g_LastLoginStatus = CXBOXLIVE_SIGNIN_FAILED;
						break;

					case XONLINE_E_SILENT_LOGON_NO_ACCOUNTS:
						dbgPrint("Not signed in (auto-sign in disabled)");
						g_LastLoginStatus = CXBOXLIVE_SIGNIN_FAILED;
						break;

					case XONLINE_E_SILENT_LOGON_PASSCODE_REQUIRED:
						dbgPrint("Not signed in: passcode required");
						g_LastLoginStatus = CXBOXLIVE_PASSCODE_NEEDED;
						break;

					case XONLINE_E_LOGON_KICKED_BY_DUPLICATE_LOGON:
						dbgPrint("Not signed: Duplicate login");
						g_LastLoginStatus = CXBOXLIVE_SIGNIN_FAILED;
						break;

					case XONLINE_E_LOGON_NO_NETWORK_CONNECTION:
						dbgPrint("Not signed: No Network Connection");
						g_LastLoginStatus = CXBOXLIVE_NOT_SIGNED_IN;		// According to Eidos, show not signed, not ethernet status.. whatever!
						break;

					case	XONLINE_E_LOGON_USER_ACCOUNT_REQUIRES_MANAGEMENT:
						dbgPrint("Not signed: Account Requires Management");
						g_LastLoginStatus = CXBOXLIVE_REQUIRES_MANAGEMENT;
						break;

					case	 XONLINE_E_LOGON_SERVERS_TOO_BUSY:
						dbgPrint("Not signed: Servers are too busy");
						g_LastLoginStatus = CXBOXLIVE_SERVERS_TOO_BUSY;
						g_ExitCode = CXBOXLIVE_UIX_EXIT;
						break;

					case	XONLINE_E_LOGON_CONNECTION_LOST:
						dbgPrint("Not signed: Connection Lost");
						g_LastLoginStatus = CXBOXLIVE_CONNECTION_LOST;		// According to Eidos, show not signed, not ethernet status.. whatever!
						g_ExitCode = CXBOXLIVE_UIX_EXIT;
						break;

					default:
						dbgPrint("Not signed: Unknown Error");
						g_LastLoginStatus = CXBOXLIVE_SIGNIN_FAILED_UNKNOWN;
						break;
				}
				
				// MS has some problems with UIX, supposedly closing and re opening UIX can fix some of them... yicky
				LogOutUIX();
				KillUIX();
				InitUIX();

				g_InUIX = false;
				return;
			}
				
			case UIX_EXIT_LOGON_USER_EXIT:
				// This means the user backed out of the logon screen.
				dbgPrint("User backed out of logon");
				g_LoggingIn = false;
				g_LoginFailed = true;
				g_InUIX = false;
				return;

			case	UIX_EXIT_FRIENDS_NORMAL_EXIT:
				g_InUIX = false;
				return;

			case	UIX_EXIT_PLAYERS_NORMAL_EXIT:
				g_InUIX = false;
				return;

			case	UIX_EXIT_FRIENDS_SIGNED_OUT:
				g_InUIX = false;
				return;

			case	UIX_EXIT_FRIENDS_JOIN_GAME:
				g_InviteFound = false;			// clear icon
				g_InUIX = false;
				g_FriendData = ( XONLINE_FRIEND* )exitInfo.pExitData;  
				g_InviteType = CXBOXLIVE_INVITE;
				g_SessionID = &g_FriendData->sessionID;
				return;

			case	UIX_EXIT_FRIENDS_JOIN_GAME_CROSS_TITLE:
				g_InviteFound = false;			// clear icon
				g_InUIX = false;
				g_FriendData = ( XONLINE_FRIEND* )exitInfo.pExitData;  
				g_InviteType = CXBOXLIVE_CROSS_INVITE;
				g_SessionID = &g_FriendData->sessionID;
				return;

			default:
				break;
		}
	}


	// --- make sure headset status is updated correctly and we route voice correctly
	CXHVVoiceManager* pXHVManager = NULL;
	pXHVManager = ( CXHVVoiceManager * ) g_CVoiceChat.getPlatformVoiceManager();
	if( pXHVManager && pXHVManager->GetXHVEnginePtr() )
	{
		ControllerInputDriver *c = NULL;
		if ( g_input.ControllerLocked(0) )
			c = g_input.GetController(0);

		// --- update the communicator
		if( c )
		{
			ControllerInputDriver::HeadSetStatus headSetStatus = c->GetHeadSetStatus();
			if( s_headSetStatus != headSetStatus )
			{
				s_headSetStatus = headSetStatus;
				if( headSetStatus == ControllerInputDriver::INSERTED )
				{
					cUix->SetProperty(UIX_PROPERTY_VOICE_MAIL_TO_SPEAKERS, FALSE);
					pXHVManager->SetVoiceThroughSpeakers( 0, false );
				}
				else
				{
					cUix->SetProperty(UIX_PROPERTY_VOICE_MAIL_TO_SPEAKERS, TRUE);
					pXHVManager->SetVoiceThroughSpeakers( 0, true );
				}
			}
		}
		pXHVManager->DoWork();
	}
	return;
}

//-----------------------------------------------------------------------------
// Call Auto Update

void	CXboxLive::AutoUpdate(void)
{
	HRESULT	hr;
	hr = XOnlineTitleUpdate(0);
}

//-----------------------------------------------------------------------------
// Save Login State

void	CXboxLive::SaveLoginState(void)
{
	 static XONLINE_LOGON_STATE logonState;

	HRESULT	hr;

	hr = XOnlineSaveLogonState(&logonState);

	if (hr == S_OK)
		g_LoginState = &logonState;
	else
		g_LoginState = 0;
}

//-----------------------------------------------------------------------------
// Get Login State

XONLINE_LOGON_STATE*	CXboxLive::GetLoginState(void)
{
	return (g_LoginState);
}

//-----------------------------------------------------------------------------
// ResetLoginState

void	CXboxLive::ResetLoginState(void)
{
	g_LoginState = NULL;
}

//-----------------------------------------------------------------------------
// Login Update - Called once per game loop

bool CXboxLive::LoginUpdate(void)
{
	HRESULT hr;

	// If sucessful, an asynchronous task handle (XONLINETASK_HANDLE) will
	// be returned.  As with the other Xbox online APIs that return
	// task handles, XOnlineTaskContinue() is used to perform a "unit" of
	// work.  The HRESULT returned by calling XOnlineTaskContinue will
	// indicate if additional work is required (XONLINETASK_S_RUNNING) or if
	// the task has failed.  Some of the result codes returned will depend on 
	// the actual type of task being pumped.  However, the SUCCEEDED and
	// FAILED macros can be used for error handling purposes.
	dbgPrint("Signing In");

	if (LogInFailed() == false)
	{
		if (g_hLogonTask != NULL)
		{
			hr = XOnlineTaskContinue( g_hLogonTask );  // Do a small amount of work
			g_LogonAuthenticate = hr;

			if (hr == XONLINETASK_S_RUNNING)
				return false;
			else
				return true;
		}
		else
			return false;
	}
	else
		return false;
}

//-----------------------------------------------------------------------------
// Login Authenticate - Called once LoginUpdate completes successfully

bool CXboxLive::AuthenticateLogin(void)
{
	bool		result = true;
	HRESULT	hr = S_OK;

	// Check for system authentication errors.
	switch( g_LogonAuthenticate )
	{
	case XONLINE_S_LOGON_CONNECTION_ESTABLISHED:  
		// The Xbox has been validated and there are no system authentication
		// errors
		dbgPrint("Connection Established");
		break;	
	case XONLINE_E_LOGON_CONNECTION_LOST:
		dbgPrint("Network Connection Lost");
		result = false;
		break;	
	case XONLINE_E_LOGON_CANNOT_ACCESS_SERVICE:
		dbgPrint("Cannot Access Service");
		result = false;
		break;	
	case XONLINE_E_LOGON_UPDATE_REQUIRED:
		// XONLINE_E_LOGON_UPDATE_REQUIRED is returned when an 
		// updated version of this title is available on the
		// server.  For silent sign-in, this should be treated as a
		// failure to sign-in the user.
		dbgPrint("Auto Update Is Required To Continue Signing In");
		result = false;
		break;	
	case XONLINE_E_LOGON_INVALID_USER:
		// The user has an unrecognized Gamertag or key.
		// This should be treated as a failure to sign-in.
		dbgPrint("Account Is Not Current");
		result = false;
		break;	
	case XONLINE_E_LOGON_SERVERS_TOO_BUSY:
		dbgPrint("Xbox Live Service Too Busy");
		result = false;
		break;	
	default:
		// Some other error - title is free to allow access to dash      
		dbgPrint("Login Failed Error: 0x%x", hr);
		result = false;
		break;	
    }

	if (result == false)
	{
		g_LoggingIn = false;
		return (result);
	}

	// Check for user authentication errors.
	// To check for user authentication errors, we call XOnlineGetLogonUsers.
	// This returns a pointer to an array of XONLINE_USER structures.  The
	// array in controller order and has the 'hr' field of each XONLINE_USER
	// set with a status code indicating whether or not authentication 
	// for that user succeeded.  Since there is only one user, this
	// information is in the first entry.
	PXONLINE_USER Users = XOnlineGetLogonUsers();
	ASSERT( Users );

	// Check authentication results for this user
	switch( Users[0].hr )
	{
	case S_OK:
		dbgPrint("%S signed in", Users[0].szGamertag);
		result = true;
		break;
	case XONLINE_S_LOGON_USER_HAS_MESSAGE:
		// Ignore for silent sign-in
		result = true;
		break;
	case XONLINE_E_LOGON_USER_ACCOUNT_REQUIRES_MANAGEMENT:
		// Login failed
		dbgPrint("This %S account requires management", Users[0].szGamertag);
		result = false;
		break;
	default:
		// Should never happen
		ASSERT( FALSE );
		result = false;
		break;
	}

	if (result == false)
	{
		g_LoggingIn = false;
		return (result);
	}

	// Finally check the requested services.  A title
	// may choose to gracefully degrade functionality based
	// on service availability
	for( DWORD i = 0; i < dwNumServices; ++i )
	{
		hr = XOnlineGetServiceInfo( Services[i], NULL );
	    
		switch( hr )
		{
		case S_OK:
			dbgPrint("Service %lu Available", Services[i]);
			result = true;
			break;

		case XONLINE_E_LOGON_SERVICE_NOT_AUTHORIZED:
			// Handle access denial (e.g. Billing service lockout)
			dbgPrint("Access to service %lu is denied", Services[i]);
			result = false;
			break;
		case XONLINE_E_LOGON_SERVICE_TEMPORARILY_UNAVAILABLE:
			// A title can decide how to handle the unavailability
			// of a service.  In some cases, the title may decide
			// to temporarily disable a feature (e.g. content download)
			// for the duration of the sesssion, or may decide to not
			// allow gameplay (e.g. if the matchmaking service was
			// unavailable).
			dbgPrint("Service %lu is unavailable", Services[i]);
			result = false;
			break;
		default:
			dbgPrint("Error 0x%x signing onto service %lu", hr, Services[i] );
			result = false;
			break;
		}            
	}   

	// Everything is OK at this point.  
	// Set user online notification state so they are visible to their
	// friends. A real title would also check for the voice peripheral and 
	// specify the XONLINE_FRIENDSTATE_FLAG_VOICE if present.  
	hr = XOnlineNotificationSetState( 0,   // Controller index
		XONLINE_FRIENDSTATE_FLAG_ONLINE, XNKID(), 0, NULL );

	if (SUCCEEDED(hr))
		result = true;
	else
		result = false;

	if (result == true)
		g_LoggedIn = true;

	g_LoggingIn = false;

	return (result);
}

//-----------------------------------------------------------------------------
// Update - needs to be called once a game loop
bool	CXboxLive::Update(void)
{
	bool		result = true;
	HRESULT	hr = S_OK;

	if (g_LoggedIn == true)
	{
		// Continue pumping the logon task handle in your game loop.
		// Failure to pump the task in a timely manner will result in
		// automatic signoff from the system.  The title should always
		// check the return value to make sure the connection is still
		// established
		hr = XOnlineTaskContinue( g_hLogonTask );

		switch( hr )
		{
			case XONLINE_S_LOGON_CONNECTION_ESTABLISHED:		
				// Still signed on
				result = true;
				break;  
			case XONLINE_E_LOGON_CONNECTION_LOST:
				// The connection was lost.
				dbgPrint("Connection was lost.");
				result = false;
				break;
			case XONLINE_E_LOGON_KICKED_BY_DUPLICATE_LOGON:
				// A title is required to check for the case where a player
				// has been signed out because that same account has been
				// signed on another Xbox.  This is indicated by the
				// XONLINE_E_LOGON_KICKED_BY_DUPLICATE_LOGON error code.
				dbgPrint("You were signed out of Xbox Live because another person signed on using your account.\n");
				result = false;
				break;
			default:
				ASSERT(FALSE);
				result = false;
				break;
		}
	}

	g_LiveStatus = hr;
	return (result);
}

//-----------------------------------------------------------------------------
// CheckInvites

bool CXboxLive::CheckInvites(int Controller, XONLINE_NOTIFICATION_TYPE Flags)
{
	BOOL result;
	result = XOnlineGetNotification(Controller, Flags);
	return (TRUE == result);
}

//-----------------------------------------------------------------------------
// SessionID of inviter
XNKID*	CXboxLive::GetInviterSessionID(void)
{
	return (g_SessionID);
}

//-----------------------------------------------------------------------------
// Did we get a cross-title invite?
bool	CXboxLive::GetGameInvite(void)
{
	HRESULT	hr;

	hr = XOnlineFriendsGetAcceptedGameInvite( &g_BootInvite );

	if (hr == S_OK)
	{
		g_SessionID = &g_BootInvite.InvitingFriend.sessionID;
		return true;
	}
	else
		return false;
}

//-----------------------------------------------------------------------------
// UIX functions
//-----------------------------------------------------------------------------

bool CXboxLive::IsUIXStarted(void)
{
	return (g_UIXStarted);
}

//-----------------------------------------------------------------------------
int CXboxLive::GetFilteredLanguage()
{
	// we can't allow showing text for languages we don't support for the whole game.
	switch (XGetLanguage())
	{
		case XC_LANGUAGE_FRENCH:  return XC_LANGUAGE_FRENCH;
		case XC_LANGUAGE_ITALIAN: return XC_LANGUAGE_ITALIAN;
		default:                  return XC_LANGUAGE_ENGLISH;
	}
}

//-----------------------------------------------------------------------------
// Set Soundmap

void CXboxLive::SetSoundMap(CXboxLiveSoundNameMap* Map, int NumEntries)
{
	g_SoundMap = Map;
	g_SoundMapEntries = NumEntries;
}

//-----------------------------------------------------------------------------
// Initialize UIX

bool CXboxLive::InitUIX()
{
	HRESULT	hr;
	bool		result = true;

	// font
	cFontRendererPtr = new CXboxLiveFont();
	cFontPlugin	= cFontRendererPtr;

	// create UI plugin
	hr = UIXCreateUIPlugin( cFontPlugin, &cUiPlugin );

	if (hr != S_OK)
		return (false);

	if (g_SoundMap)
		cAudioPlugin = new CXboxLiveAudio();

	// Set UI support plugin if necessary
//	hr = cUiPlugin->SetPluginSupport( cSupportPlugin );
//	if (hr != S_OK)
//		return (false);

	// create UIX engine
	hr = UIXCreateLiveEngine( "z:\\ttl.uix", CXboxLive::GetFilteredLanguage(), &cUix );

	if (hr != S_OK)
		return (false);


	// set up features
	cUix->SetUIPlugin ( cUiPlugin );
	cUix->SetAudioPlugin ( cAudioPlugin );
	cUix->EnableFeature( UIX_LOGON_FEATURE );
	cUix->EnableFeature( UIX_FRIENDS_FEATURE );
	cUix->EnableFeature( UIX_PLAYERS_FEATURE );

	cUix->SetProperty( UIX_PROPERTY_DISPLAY_CONNECTION_ERRORS, FALSE );
	cUix->SetProperty( UIX_PROPERTY_DISPLAY_NOTIFICATIONS, TRUE );
	cUix->SetProperty( UIX_PROPERTY_ALLOW_GAME_INVITES, TRUE );
	cUix->SetProperty( UIX_PROPERTY_HOST_NO_MIGRATION, FALSE );

	g_UIXStarted = true;
	g_FriendData = NULL;
	g_InviteType = CXBOXLIVE_NOINVITE;
	return (true);
}

//-----------------------------------------------------------------------------
// Invite type
int	CXboxLive::GetInviteType(void)
{
	return (g_InviteType);
}

//-----------------------------------------------------------------------------
// In UIX?
bool CXboxLive::InUix(void)
{
	return (g_InUIX);
}

//-----------------------------------------------------------------------------
// Kill UIX

bool CXboxLive::KillUIX(void)
{
	// close UIX
	cUix->Release();
	cUix = 0;

	// UIX doesnt kill this.. 
	// jm - but it does try to call Release on it so don't delete it until after uix
	if (cAudioPlugin)
	{
		delete cAudioPlugin;
		cAudioPlugin = 0;
	}

	g_UIXStarted = false;
	return (true);
}

//-----------------------------------------------------------------------------
// Get Live Engine

ILiveEngine*	CXboxLive::GetLiveEngine(void)
{
	return (cUix);
}

//-----------------------------------------------------------------------------
// Friends List
bool CXboxLive::StartFriendsList(void)
{
	HRESULT			hr;
	UIX_EXIT_INFO	info;

	int	Cnum = g_input.GetLockedControllerIndex(0);
	if (Cnum == 0xffffffff)
		Cnum = 0;

	UIX_FRIENDS_PARAMS friendsParams;
	friendsParams.SelectedFriendXUID.qwUserID		= 0;
	friendsParams.SelectedFriendXUID.dwUserFlags	= 0;
	friendsParams.SignOutEnabled						= FALSE;
	friendsParams.StructSize							= sizeof( friendsParams );
	friendsParams.UserPort								= Cnum;

	// turn off game invites
	cUix->SetProperty( UIX_PROPERTY_ALLOW_GAME_INVITES, true );

	hr = cUix->StartFeature( UIX_FRIENDS_FEATURE, &friendsParams );
	hr = cUix->GetExitInfo(&info);

	if( S_OK == hr )
	{
		g_InUIX = true;
		return true;
	}
	else
		return false;
}

//-----------------------------------------------------------------------------
// Players List
bool CXboxLive::StartPlayerList(void)
{
	HRESULT	hr;

	UIX_PLAYERS_PARAMS playersParams;

	int	Cnum = g_input.GetLockedControllerIndex(0);
	if (Cnum == 0xffffffff)
		Cnum = 0;

	playersParams.StructSize						= sizeof( playersParams );
	playersParams.UserPort							= Cnum;
	playersParams.pPlayerControllingScreen		= NULL;
	playersParams.pSelectedPlayerXUID			= NULL;
	playersParams.SortCurrentPlayersList		= FALSE;
	playersParams.FilterFlags						= 0;
	playersParams.DisplayType						= UIX_PLAYERS_DISPLAY_CURRENT_PLAYERS;

	// turn off game invites
	cUix->SetProperty( UIX_PROPERTY_ALLOW_GAME_INVITES, false );

	hr = cUix->StartFeature( UIX_PLAYERS_FEATURE, &playersParams );
	if( S_OK == hr )
	{
		g_InUIX = true;
		return true;
	}
	else
		return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Create the Text objects for rendering and set default values for class members.
/////////////////////////////////////////////////////////////////////////////////////////////////////
CXboxLiveFont::CXboxLiveFont()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Clean up.
/////////////////////////////////////////////////////////////////////////////////////////////////////
CXboxLiveFont::~CXboxLiveFont()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Decrease the reference count.  Since this class is only created/deleted once, we clean up.
///
/// \return The new reference count.
/////////////////////////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(ULONG) CXboxLiveFont::Release()
{
	delete this;
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Called by UIX to draw a string to the screen
///
/// \param pText Pointer to a null-terminated Unicode string that specifies the text to draw.
///
/// \param X X pixel coordinate of the text. 
///
/// \param Y Y pixel coordinate of the text. 
///
/// \param MaxWidth Maximum width to draw. Text extending beyond the specified width should be clipped.
///
/// \return S_OK if successful, an error code otherwise.
/////////////////////////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CXboxLiveFont::DrawText(LPCWSTR pText, DWORD X, DWORD Y, DWORD MaxWidth)
{
	g_Font.Begin();
	g_Font.SetScaleFactors( mScale, mScale );
	g_Font.DrawText( (float)X, (float)Y, mColor, pText );
	g_Font.End();
    return S_OK;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Called by UIX to figure out the size of a string: used to space the screen properly.
///
/// \param pText Pointer to null-terminated Unicode string
///
/// \param pWidth Pointer to a DWORD variable that receives the width, in pixels, of the text. 
///
/// \param pHeight Pointer to a DWORD variable that receives the height, in pixels, of the text. 
///
/// \return S_OK if successful, an error code otherwise 
/////////////////////////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CXboxLiveFont::GetTextSize(LPCWSTR pText, DWORD *pWidth, DWORD *pHeight)
{
	float fwidth, fheight;

	g_Font.SetScaleFactors( mScale, mScale );
	g_Font.GetTextExtent( pText, &fwidth, &fheight );
	*pWidth = (DWORD) fwidth;
	*pHeight = (DWORD) fheight;
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Sets the current text color
///
/// \param The new color to use
///
/// \return S_OK if successful, an error code otherwise 
/////////////////////////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CXboxLiveFont::SetColor(D3DCOLOR Color)
{
    mColor = Color;
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Sets the current text height
///
/// \param The new text size to use
///
/// \return S_OK if successful, an error code otherwise 
/////////////////////////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CXboxLiveFont::SetHeight(DWORD Height)
{
   mHeight = (float)Height;
	mScale = mHeight / g_Font.GetFontHeight();
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Called at the end of every frame so that we know to restart at the beginning of
/// the text objects for the next frame.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void CXboxLiveFont::EndFrame()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Called at the start of every frame so that we can reset the text objects.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void CXboxLiveFont::BeginFrame()
{
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
/// UIX Audio Plugin
/////////////////////////////////////////////////////////////////////////////////////////////////////

ULONG CXboxLiveAudio::Release()
{
	return 0;
}

HRESULT CXboxLiveAudio::DoWork()
{
	return S_OK; // return value is ignored by UIX according to docs
}

HRESULT CXboxLiveAudio::PlaySound( LPCSTR soundNameUIX )
{
	const char* soundNameTTL = 0;
	for( int i = 0; i < g_SoundMapEntries; i++ )
	{
		if( !strcmp(g_SoundMap[i].uix, soundNameUIX ) )
		{
			soundNameTTL = g_SoundMap[i].ttl;
			break;
		}
	}

	if( soundNameTTL )
	{
		Sfx::Play2D( soundNameTTL, 1.0f, 0.0f );
	}

	return S_OK; // return value is ignored by UIX according to docs
}










