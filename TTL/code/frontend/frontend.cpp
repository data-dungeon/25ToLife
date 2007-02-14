#include "TTLPCH.h"

//=============================================================================
// Engine Includes
//
//		The front end depends on these engine components.
//=============================================================================

#include "Platform/MemoryCard/MemoryCard.h"
#include "Main/PlayerAgent.h"
#include "GameHelper/Translator.h"

//=============================================================================
// Front End Includes
//=============================================================================

#include "frontend/frontend.h"
#include "frontend/keyboardchat.h"
#include "frontend/components.h"
#include "frontend/inputdriver.h"
#include "frontend/savegame.h"
#include "main/TTL.h"
#include "fmv/fmv.h"
#include "EngineHelper/LoadScreen.h"

#ifdef _XBOX
#include "platform/Xbox/XboxLive.h"
#include "frontend/UseCase/Xbox/UseCaseUIXFriends.hpp"
#include "frontend/UseCase/Xbox/UseCaseUIXLogin.hpp"
#include "frontend/UseCase/Xbox/UseCaseUIXPlayers.hpp"
#include "OSI/XBox/OSIXBox.hpp"
#endif

//=============================================================================
// Screen Handler Includes
//
//		You must include all header files for every screen handler here.
//		Please keep it alphabetical.
//=============================================================================

#include "frontend/screens/screen_BaseClass.h"
#include "frontend/screens/screen_BasePopup.h"
#include "frontend/screens/screen_Briefing.h"
#include "frontend/screens/screen_CreateProfile.h"
#include "frontend/screens/screen_CreateSession.h"
#include "frontend/screens/screen_CustomPlayer.h"
#include "frontend/screens/screen_DevMenu.h"
#include "frontend/screens/screen_GamePadSettings.h"
#include "frontend/screens/screen_GameSetting.h"
#include "frontend/screens/screen_Invitations.h"
#include "frontend/screens/screen_Keyboard.h"
#include "frontend/screens/screen_MainMenu.h"
#include "frontend/screens/screen_MemCard.h"
#include "frontend/screens/screen_MySettings.h"
#include "frontend/screens/screen_NetworkError.h"
#include "frontend/screens/screen_Options.h"
#include "frontend/screens/screen_OnlineChat.h"
#include "frontend/screens/screen_OnlineLobby.h"
#include "frontend/screens/screen_OnlineMenu.h"
#include "frontend/screens/screen_OnlineProfile.h"
#include "frontend/screens/screen_PalMode.h"
#include "frontend/screens/screen_Pause.h"
#include "frontend/screens/screen_PlayersList.h"
#include "frontend/screens/screen_Popups.h"
#include "frontend/screens/screen_ScoreBoard.h"
#include "frontend/screens/screen_SearchResult.h"
#include "frontend/screens/screen_TagEditor.h"
#include "frontend/screens/screen_Title.h"
#include "frontend/screens/screen_VoiceSetting.h"
#include "frontend/screens/screen_Weapons.h"
#include "frontend/screens/screen_XboxLive.h"

//#ifdef PS2
#if defined(PS2) || defined(DIRECTX_PC)
#include "frontend/screens/screen_NetworkConfig.h"
#endif

//=============================================================================
// Ritual screens
//=============================================================================

#if defined PS2 || defined _XBOX || defined(DIRECTX_PC)
#include "frontend/screens/screen_Dialog.h"
#include "frontend/screens/screen_MultiplayerMenu.h"
#include "frontend/screens/screen_MultiplayerGameSettings.h"
#include "frontend/screens/screen_Quickmatch.h"
#include "frontend/screens/screen_stats.h"
#include "frontend/screens/screen_wait.h"
#include "frontend/screens/screen_YesNoDialog.h"
#include "frontend/screens/screen_clan_main.h"
#include "frontend/screens/screen_clan_list.h"
#include "frontend/screens/screen_clan_create.h"
#include "frontend/screens/screen_clan_invite.h"
#include "frontend/screens/screen_clan_popup.h"
#include "frontend/screens/screen_clan_invite_popup.h"
#include "frontend/screens/screen_clan_notices.h"
#include "frontend/screens/screen_CountryList.h"
#include "frontend/screens/screen_title_only.h"
#include "frontend/screens/screen_PopupText.h"
#endif

//=============================================================================
// Ritual (PS2 or PC)
//=============================================================================

#if defined(PS2) || defined(DIRECTX_PC)
#include "frontend/screens/screen_EOA.h"
#include "frontend/screens/screen_EORConnect.h"
#include "frontend/screens/screen_EORCreateAccount1.h"
#include "frontend/screens/screen_EORCreateAccount2.h"
#include "frontend/screens/screen_EORDeleteAccount.h"
#include "frontend/screens/screen_EOREditAccount1.h"
#include "frontend/screens/screen_EOREditAccount2.h"
#include "frontend/screens/screen_EORLogin.h"
#include "frontend/screens/screen_EORRecoverAccount.h"
#include "frontend/screens/screen_friends.h"
#include "frontend/screens/screen_FriendsList.h"
#include "frontend/screens/screen_FriendRequested.h"
#include "frontend/screens/screen_friendConfirmed.h"
#include "frontend/screens/screen_FriendsPopup.h"
#include "frontend/screens/screen_players.h"
#include "frontend/screens/screen_Sandbox.h"
#include "frontend/screens/screen_AskPassword.h"
#endif

#include "sim/InitialLoop.h"

#include "hud/imarquee.h"

//=============================================================================
// Ritual (PC-only)
//=============================================================================
#ifdef DIRECTX_PC
#include "frontend/screens/screen_PCKeyBindings.h"
#include "frontend/screens/Screen_PC_LevelSelect.h"
#endif // DIRECTX_PC

//=============================================================================
// Ritual (XBox-only)
//=============================================================================
#ifdef _XBOX
#include "frontend/screens/screen_UIX.h"
#include "frontend/screens/screen_XboxClanFeedback.h"
#include "frontend/screens/screen_XboxClanFeedbackPopup.h"
#endif


int	g_ControllerRemoved = 0;
bool	g_DisableControllerRemoved = false;

extern	bool	g_PalModeDone;
extern	bool	g_ObjectivesOnScreen;
extern	bool	g_PauseOnScreen;
extern	bool	g_OptionsOnScreen;
extern	bool	g_MainMenuSignOut;
extern	bool	m_TitleScreenOn;

//=============================================================================
// FrontEndUIHandler::RefreshInput
//
//		Not used.  Doesn't do anything.
//=============================================================================

void FrontEndUIHandler::RefreshInput( void )
{
}

//=============================================================================
// FrontEndUIHandler::ButtonPressed
//
//		Returns true if the button is pressed.
//=============================================================================

bool FrontEndUIHandler::ButtonPressed( DIGITALBUTTON Button )
{
	int DeviceButton = GetDeviceButton( Button );

	if( DeviceButton == -1 )
		return false;

	return DeviceButtonPressed( DeviceButton );
}

//=============================================================================
// FrontEndUIHandler::ButtonDown
//
//		Returns true if the button is held down.
//=============================================================================

bool FrontEndUIHandler::ButtonDown( DIGITALBUTTON Button )
{
	int DeviceButton = GetDeviceButton( Button );

	if( DeviceButton == -1 )
		return false;

	return DeviceButtonDown( DeviceButton );
}

//=============================================================================
// FrontEndUIHandler::DeviceButtonPressed
//
//		Returns true if the button is pressed.
//=============================================================================

bool FrontEndUIHandler::DeviceButtonPressed( int Button )
{
	FluffControlMapper* pMapper = GetMapper();

	if( !pMapper )
		return false;

	return pMapper->ButtonPressed( (FluffControlMapper::DIGITALBUTTON) Button );
}

//=============================================================================
// FrontEndUIHandler::DeviceButtonPressed
//
//		Returns true if the button is pressed.
//=============================================================================

bool FrontEndUIHandler::DeviceButtonReleased( int Button )
{
	FluffControlMapper* pMapper = GetMapper();

	if( !pMapper )
		return false;

	return pMapper->ButtonReleased( (FluffControlMapper::DIGITALBUTTON) Button );
}

//=============================================================================
// FrontEndUIHandler::DeviceButtonDown
//
//		Returns true if the button is held down.
//=============================================================================

bool FrontEndUIHandler::DeviceButtonDown( int Button )
{
	FluffControlMapper* pMapper = GetMapper();

	if( !pMapper )
		return false;

	return pMapper->ButtonDown( (FluffControlMapper::DIGITALBUTTON) Button );
}

//=============================================================================
// FrontEndUIHandler::PlaySound
//
//		The front end always goes through this function to play ALL sounds.
//=============================================================================

void FrontEndUIHandler::PlaySound( const char* pSound )
{
	Sfx::Play2D( pSound, 1.0f, 0.0f );
}

//=============================================================================
// FrontEndUIHandler::GetMapper
//
//		Returns a pointer to the fluff control mapper.
//=============================================================================

FluffControlMapper* FrontEndUIHandler::GetMapper( void )
{
	return (FluffControlMapper*) g_controlFocus.Find("Fluff");
}

//=============================================================================
// FrontEndUIHandler::GetDeviceButton
//
//		Returns the device button mapped to the specified input.
//=============================================================================

int FrontEndUIHandler::GetDeviceButton( DIGITALBUTTON Button )
{
	switch( Button )
	{
	case CURSOR_UP:
		return FluffControlMapper::UP;
	case CURSOR_DOWN:
		return FluffControlMapper::DOWN;
	case CURSOR_LEFT:
		return FluffControlMapper::LEFT;
	case CURSOR_RIGHT:
		return FluffControlMapper::RIGHT;
	case BUTTON_OK:
		return FluffControlMapper::X;
	case BUTTON_CANCEL:
#ifndef DIRECTX_PC
		return FluffControlMapper::TRIANGLE;
#else
		return FluffControlMapper::ESCAPE;
#endif
	case BUTTON_START:
		return FluffControlMapper::START;
	case BUTTON_SELECT:
		return FluffControlMapper::SELECT;
	}

	return -1;
}

//=============================================================================
// FrontEnd::FrontEnd
//=============================================================================

FrontEnd* FrontEnd::m_pPopupHandler = 0;

FrontEnd::FrontEnd( void )
{
	m_ControlsActive = false;

	if( m_pPopupHandler == 0 )
		m_pPopupHandler = this;

	m_IconImages = INVALID_HANDLE;
	m_ScoreBoardTime = 0.0f;
	m_ControlsActive = false;
	m_ShowObjectives = false;
	m_ShowDebugOutput = false;
	m_ShowScoreBoard = false;
	m_ControllerDisconnected = false;

	SetGameType( GAMETYPE_UNDEFINED );

	// declare screen uber files
	//
	//		uber files contain the textures which should be loaded for that game type

	DeclareUberFile( "screens/uber_allscreens.dbl", GAMETYPE_ANY );
	DeclareUberFile( "screens/uber_mainmenu.dbl", GAMETYPE_MAINMENU );
	DeclareUberFile( "screens/uber_singleplayer.dbl", GAMETYPE_SINGLEPLAYERGAME );
	DeclareUberFile( "screens/uber_network.dbl", GAMETYPE_NETWORKGAME );
	DeclareUberFile( "screens/uber_online.dbl", GAMETYPE_ONLINEGAME );

	// declare screen files

	DeclareScreenFile( "screens/Briefing.sc", GAMETYPE_SINGLEPLAYERGAME );
	DeclareScreenFile( "screens/Buttonmap.sc", GAMETYPE_ANY );
	DeclareScreenFile( "screens/CustomPlayer.sc", GAMETYPE_MAINMENU );
	DeclareScreenFile( "screens/Keyboard2.sc", GAMETYPE_ANY );
	DeclareScreenFile( "screens/MainMenu.sc", GAMETYPE_MAINMENU );
	DeclareScreenFile( "screens/NetworkGames.sc", GAMETYPE_MAINMENU|GAMETYPE_NETWORKGAME|GAMETYPE_ONLINEGAME );
	DeclareScreenFile( "screens/OnlineScreens.sc", GAMETYPE_MAINMENU );
	DeclareScreenFile( "screens/Options.sc", GAMETYPE_ANY );
	DeclareScreenFile( "screens/Pause.sc", GAMETYPE_SINGLEPLAYERGAME|GAMETYPE_NETWORKGAME|GAMETYPE_ONLINEGAME );
	DeclareScreenFile( "screens/Popups.sc", GAMETYPE_ANY );
	DeclareScreenFile( "screens/ScoreBoard.sc", GAMETYPE_NETWORKGAME|GAMETYPE_ONLINEGAME );
	DeclareScreenFile( "screens/TagEditor.sc", GAMETYPE_MAINMENU );


	// online screens by ritual
#if defined PS2 || defined _XBOX || defined(DIRECTX_PC)
	DeclareScreenFile( "screens/Multiplayer.sc", GAMETYPE_MAINMENU );
	DeclareScreenFile( "screens/MultiplayerPopups.sc", GAMETYPE_MAINMENU|GAMETYPE_ONLINEGAME );
#endif

#if defined(PS2) || defined(DIRECTX_PC)
	// eidos online registration screens by ritual
	DeclareScreenFile( "screens/EOR.sc", GAMETYPE_MAINMENU );
	// friends and players screens for OSI
	DeclareScreenFile( "screens/FriendsAndPlayers.sc", GAMETYPE_ANY );
#endif

	// only on xbox
#ifdef _XBOX
	DeclareScreenFile( "screens/UIX.sc", GAMETYPE_ANY );
#endif

	// directx_pc only screens
#ifdef DIRECTX_PC
	DeclareScreenFile( "screens/PcScreens.sc", GAMETYPE_ANY );
	DeclareScreenFile( "screens/PCKeyBindings.sc", GAMETYPE_ANY );
#endif


	return;
}

//=============================================================================
// FrontEnd::~FrontEnd
//=============================================================================

FrontEnd::~FrontEnd( void )
{
	if( m_pPopupHandler == this )
		m_pPopupHandler = 0;

	return;
}

//=============================================================================
// FrontEnd::DeclareUberFile
//=============================================================================

void FrontEnd::DeclareUberFile( const char* pFileName, GAMETYPE GameType )
{
	ScreenFile* pScreenFile = new ScreenFile( pFileName, GameType );

	if( pScreenFile )
		m_UberFiles.AddChild( pScreenFile );

	return;
}

//=============================================================================
// FrontEnd::DeclareScreenFile
//=============================================================================

void FrontEnd::DeclareScreenFile( const char* pFileName, GAMETYPE GameType )
{
	ScreenFile* pScreenFile = new ScreenFile( pFileName, GameType );

	if( pScreenFile )
		m_ScreenFiles.AddChild( pScreenFile );

	return;
}

//=============================================================================
// FrontEnd::SetGameType
//=============================================================================

void FrontEnd::SetGameType( GAMETYPE GameType )
{
	m_GameType = GameType;
}

//=============================================================================
// FrontEnd::GetGameType
//=============================================================================

FrontEnd::GAMETYPE FrontEnd::GetGameType( void )
{
	return m_GameType;
}

//=============================================================================
// FrontEnd::Init
//
//		Call this function during startup to initilaize the front end. This takes
//		care of everything including registration of screen handlers, sprite
//		interfaces and loading the screens files from disk.
//=============================================================================

bool FrontEnd::Init( void )
{
	m_CinematicInProgress = false;

	if( !ScreenManager::Init() )
		return false;

	if( 0 == strcmp( "frontend", g_gameEnvironment.GetWorldDisplayName() ) )
		m_FrontEndWorld = true;
	else
		m_FrontEndWorld = false;

	InitDeviceDrivers();
	RegisterScreenHandlers();
	ReleaseInputFocus();

	// preload some images
	PreloadImages();

	// Init the debug window
	InitDebugOutput();

	// init the screens
	InitScreens();

	m_MemCardState = STARTUPCHECK_IDLE;

	g_KeyboardChat.Initialize();

	return true;
}

//=============================================================================
// memcard state engine

void	FrontEnd::MemCardState()
{
	switch (m_MemCardState)
	{
		case	STARTUPCHECK_IDLE:
			break;

		case	STARTUPCHECK_CHECK_CONTROLLER:
			g_MemCardManager->StartupControllerCheck(0);
			m_MemCardState = STARTUPCHECK_CHECK_CONTROLLER_RESULT;
			break;

		case	STARTUPCHECK_CHECK_CONTROLLER_RESULT:
			if (g_MemCardManager->GetState() == g_MemCardManager->CMEMORYCARD_IDLE)
				m_MemCardState = STARTUPCHECK_GO;
			break;

		case	STARTUPCHECK_GO:
			g_MemCardManager->SetUsePopups(true);
			g_MemCardManager->StartupCheck(0, sizeof(g_GlobalProfile), MC_SAVED_PROFILE);
			m_MemCardState = STARTUPCHECK_RESULT;
			break;

		case	STARTUPCHECK_RESULT:
			if (g_MemCardManager->GetState() == g_MemCardManager->CMEMORYCARD_IDLE)
			{
				if ( g_input.ControllerLocked(0) )
					g_input.UnlockPlayerFromController(0);

				g_MemCardManager->ResetLoadSaveBuffer();
				m_MemCardState = STARTUPCHECK_IDLE;
			}
			break;

		default:
			break;
	}
}

//=============================================================================
// FrontEnd::Update
//
//		Call this function once every frame to update the front end.  Processes
//		input, updates the active screen and also checks for disconnected
//		controllers and opened cd-trays.
//=============================================================================

void FrontEnd::Update( void )
{
	int				i;
	static	bool	StartupLock = false;
	char				Buffer[192];

	// memcard space startup check
	#if !defined (_XBOX)
	if (g_MemCardManager->GetStartupCheckDone() == false)
	{
		if ( !CInitialLoop::s_skipFluff)
		{
			if (g_PalModeDone == true)
			{
				#if defined (PS2)
				m_MemCardState = STARTUPCHECK_CHECK_CONTROLLER;
				#else
				m_MemCardState = STARTUPCHECK_GO;
				#endif
				g_MemCardManager->SetStartupCheckDone(true);
			}
		}
	}
	#endif

	// update memcard
	MemCardState();
#ifndef DIRECTX_PC
	#if defined (WIN32) && !defined (_XBOX)
	// check for keys to simulate nocard, nospace etc
	if (g_input.ButtonPressed(BC_F1))
		g_MemCardMgr.ToggleNoCard();
	if (g_input.ButtonPressed(BC_F2))
		g_MemCardMgr.ToggleNoSpace();
	#endif

	if( GetActiveScreen() && !m_ControlsActive )
		SetInputFocus();
	else
	if( !GetActiveScreen() && m_ControlsActive )
		ReleaseInputFocus();
#endif // DIRECTX_PC
	SetTime( (float) g_timer.GetOSTime() / 1000.0f );
#ifdef _XBOX
	bool ShowSilentLogin = false;

	if( m_FrontEndWorld )
		if( !OnlineUI::UIXLogin::isSingletonCreated() || !OnlineUI::UIXLogin::getSingletonPtr()->isRunning() )
			ShowSilentLogin = true;

	if( ShowSilentLogin )
	{
		ShowScreen( "SCREEN_ID_LIVELOGIN" );
	}
	else
	{
		HideScreen( "SCREEN_ID_LIVELOGIN" );
	}
#endif // _XBOX

	UpdateDebugOutput();
	UpdateInvite();
	UpdateScreens();

	ControllerInputDriver* pDriver = NULL;

#if defined (_XBOX)
	if (m_TitleScreenOn == false)
#endif
	{
		// need to check for all controllers for startup memory card check.. if we find
		// none locked, then lock it, and release after we leave this screen
		if ((g_MemCardManager->GetStartupCheckDone() == false && StartupLock == false) || g_MainMenuSignOut == true)

		{
			for (i=0; i<MAX_CONTROLLERS; i++)
			{
				if ( g_input.ControllerLocked(i) )
				{
					pDriver = g_input.GetController(i);
					break;
				}
			}

			// startup check
			if (pDriver == NULL)
			{
				uint	i;
				for ( i = 0; i < g_input.Controllers(); i++)
				{
					if ( !g_input.UnmappedControllerLocked(i) && g_input.GetUnlockedController(i)->Connected() )
					{
						g_input.LockPlayerToController(0, i);
						pDriver = g_input.GetController(0);
						StartupLock = true;
						g_MainMenuSignOut = false;
						break;
					}
				}
			}
		}
		else
		{
			if ( g_input.ControllerLocked(0) )
				pDriver = g_input.GetController(0);
		}
	}

	bool				ControllerOK = true;

#if !defined (_XBOX)
	if( !pDriver || !pDriver->DeviceName() )
		ControllerOK = FALSE;
#else
	if (pDriver)
	{
		if( !pDriver->Connected() )
			ControllerOK = FALSE;
	}
#endif

#ifndef DIRECTX_PC
	if( g_input.ButtonDown( BC_PAGEDOWN ) )
		ControllerOK = FALSE;
#endif

	bool	SkipControllerCheck = false;

#if defined(PS2)
	// hack, we're sometimes getting a bogus "controller not detected" message, possibly
	// due to IOP flooding.  so let's wait a few frames before we declare it so.
	static int successiveControllerBad = 0;
	if (!ControllerOK)
	{
		successiveControllerBad++;
		if (successiveControllerBad < 3)
			ControllerOK = true;
	}
	else
	{
		successiveControllerBad = 0;
	}
#endif

	//==========================================================================
	// Whenever the controller is removed, a message is displayed asking the
	// user to reinsert the controller and press the START button.  If for some
	// reason the screen flow magically transitions to another screen and kills
	// our error message, re-enable the controller so it doesn't get stuck in
	// the disconnected state.
	//==========================================================================

	bool RenableController = false;

	if( m_ControllerDisconnected && !IsScreenAlreadyActive( "SCREEN_ID_ERRORMESSAGE" ) )
		RenableController = true;

	if( !ControllerOK )
	{
		if(	!m_ControllerDisconnected &&
				g_MemCardManager->GetState() == g_MemCardManager->CMEMORYCARD_IDLE && 
				FMVPlayer::IsPlaying() == false &&
				g_DisableControllerRemoved == false &&
				LoadScreen::IsRunning() == false
			  )
		{
			if (g_ttl->IsSinglePlayer() || g_ttl->IsMultiPlayer() )
			{
				 if (m_CinematicInProgress == true)
					SkipControllerCheck = true;
			}

#ifndef _XBOX
			// lets skip the controller check if we're in the front end
			if( GetActiveScreen() )
				SkipControllerCheck = true;
#endif // _XBOX
			
			if (RenderMgr__IsFadedIn() == false)
				SkipControllerCheck = true;

			if (SkipControllerCheck == false)
			{
				#if defined (_XBOX)
				sprintf (Buffer, g_translator.Translate("Please re-connect the Controller in controller port %d and press START"), g_input.GetLockedControllerIndex(0) + 1);
				#elif defined (PS2)
				sprintf (Buffer, g_translator.Translate("The DUALSHOCK®2 analog controller is not detected. Please attach the DUALSHOCK®2 analog controller to controller port %d, and press %c"), g_input.GetLockedControllerIndex(0) + 1, 17);
				#else
				sprintf (Buffer, g_translator.Translate("Please re-connect the Controller"));
				#endif

				#if defined (_XBOX) || defined (PS2)
				g_ControllerRemoved = 1;

				// send message to pause
				if (g_ttl->IsSinglePlayer())
				{
					if (g_ObjectivesOnScreen == false && g_PauseOnScreen == false && g_OptionsOnScreen == false)
					{
						SPause pause;
						pause.d_type = SPause::PAUSE;
						pause.d_menu = NULL;

						pause.d_affectAudio = true;
						g_messageDispatcher.PostMessageToAll("Pause", (void*)&pause,
								INVALID_OBJECT_HANDLE);
					}
				}
				else
				{
					if (ValidSingleton(CPlayers))
					{
						AvatarControlMapper	*mapper = g_players.GetMapper(0, false);
						if (mapper)
							mapper->Enable(false);
					}
				}
				#endif

				SetErrorMessage( Buffer );
				ShowErrorMessage( true );
				m_ControllerDisconnected = true;
			}
		}
	}
	else
	{
		if( m_ControllerDisconnected )
		{
			#if defined (_XBOX)
			// jm - this needs to be "released" instead of "pressed" so that UIX won't get confused by
			// the start button suddenly being "down" without ever being "pressed".
			if( pDriver && pDriver->ButtonReleased(CB_START) )
				RenableController = true;
			#endif

			#if defined (PS2)
			if (pDriver && pDriver->ButtonReleased(CB_X) )
				RenableController = true;
			#endif

			if( RenableController )
			{
				ShowErrorMessage( false );
				m_ControllerDisconnected = false;
				g_ControllerRemoved = 0;

				if (g_ttl->IsSinglePlayer())
				{
					if (g_ObjectivesOnScreen == false && SkipControllerCheck == false && g_PauseOnScreen == false && g_OptionsOnScreen == false)
					{

						// send message to unpause
						SPause pause;
						pause.d_type = SPause::UNPAUSE;
						pause.d_menu = NULL;

						pause.d_affectAudio = true;
						g_messageDispatcher.PostMessageToAll("Pause", (void*)&pause,
								INVALID_OBJECT_HANDLE);
					}
				}

				#if defined (_XBOX) || defined (PS2)
				else
				{
					if (ValidSingleton(CPlayers))
					{
						AvatarControlMapper	*mapper = g_players.GetMapper(0, false);
						if (mapper)
							mapper->Enable(true);
					}
				}
				#endif
			} 
		}
	}

	return;
}

//=============================================================================
// FrontEnd::Shutdown
//
//		Call this function to terminate the front end and cleanup everything.
//=============================================================================

void FrontEnd::Shutdown( void )
{
	g_KeyboardChat.Terminate();
	g_controlFocus.Pop("Fluff");
	g_controlFocus.Remove("Fluff");

	UnregisterScreenHandlers();

	ScreenManager::Shutdown();
	ScreenHandler::UnregisterAllClasses();

	SetGameType( GAMETYPE_UNDEFINED );
}

//=============================================================================
// FrontEnd::GotoScreen
//
//		Call this function to activate a specific screen.
//=============================================================================

bool FrontEnd::GotoScreen( const char* pScreenName )
{
	if( !ScreenManager::GotoScreen( pScreenName ) )
	{
		ReleaseInputFocus();

		return false;
	}

	SetInputFocus();

	return true;
}

//=============================================================================
// InitDeviceDrivers
//
//		This function is called during initialization to init the device drivers.
//=============================================================================


void FrontEnd::InitDeviceDrivers( void )
{
	SetUIHandler( &m_UIHandler );

	FluffControlMapper *fluffControl = new FluffControlMapper;

	ASSERT(fluffControl);

	g_controlFocus.Add( fluffControl );

	return;
}

//==========================================================================
// FontEnd::RegisterScreenHandlers
//
//		This function is called during startup to register screen handlers.
//==========================================================================

void FrontEnd::RegisterScreenHandlers( bool TrueOrFalse )
{
	//==========================================================================
	//						 !!!!! KEEP IT ALPHABETIZED !!!!!
	//==========================================================================

	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenBaseClass ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenBasePopup ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenBriefing ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenButtonMap ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenCharSelect2 ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenCharSelect3 ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenConnectingToSession ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenCreateProfile ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenCreateSession ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenCustomEdit ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenCustomMenu ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenCustomName ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenCustomPlayer ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenCustomWeapons ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenDevMenu ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenDifficulty ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenErrorMessage ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenErrorDialog ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenGameSetting ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenKeyboard ), TrueOrFalse );
#ifndef DIRECTX_PC
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenLevelSelect ), TrueOrFalse );
#endif
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenLobbyMenu ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenMainMenu ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenMemCard ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenMySettings ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenChatInGame ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenChatRoom ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenNetworkError ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenOnlineLobby ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenOnlineMenu ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenOnlineProfile ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenOptions ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenObjectiveSummary ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenPalMode ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenPause ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenPauseOnline ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenPlayersList ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenPlayersPopup ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenPleaseWait ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenPopupDialog ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenQuitGame ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenScoreBoard ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenSearchResult ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenStatsSummary ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenTagEditor ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenTestPal60 ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenThumbSticks ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenTitle ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenVoiceSetting ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenWeapons ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenWeaponsInGame ), TrueOrFalse );

    // Online screens by Ritual
#if defined PS2 || defined _XBOX || defined(DIRECTX_PC)
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenClanCreate ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenClanInvite ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenClanInvitePopup ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenClanList ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenClanMain ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenClanListPopup ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenClanNotices ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenDialog ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenInvitations ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenMultiplayerMenu ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenMultiplayerGameSettings ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenQuickmatch ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenStats ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenWait ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenYesNoDialog ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenTitleOnly ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenPopupText ), TrueOrFalse );
#endif

   // PS2 and PC screens
#if defined(PS2) || defined(DIRECTX_PC)
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenCountryList ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenEOA ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenEORConnect ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenEORCreateAccount1 ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenEORCreateAccount2 ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenEORDeleteAccount ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenEOREditAccount1 ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenEOREditAccount2 ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenEORLogin ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenEORRecoverAccount ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenFriends ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenFriendsList ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenFriendRequested ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenFriendConfirmed ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenFriendsPopup ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenPlayers ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenSandbox ), TrueOrFalse );

#ifndef DIRECTX_PC
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenNetworkConfig ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenNetworkConnect ), TrueOrFalse );
#endif // DIRECTX_PC

	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenAskPasswordPopup ), TrueOrFalse );
#endif

   // XBox-only screens
#ifdef _XBOX
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenUIX ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenXboxClanFeedback ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenXboxClanFeedbackPopup ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenXboxLiveLogin ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenUIXFriends ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenUIXPlayers ), TrueOrFalse );
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenUIXLogin ), TrueOrFalse );
#endif

   // PC-only screens
#ifdef DIRECTX_PC
   ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenPCKeyBindings ), TrueOrFalse );
   ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenPCLevelSelect ), TrueOrFalse );
#endif
}

//==========================================================================
// UnregisterScreenHandlers
//
//		Calling this function will unregister all the screen handlers that
//		were registered when RegisterScreenHandlers was called.
//==========================================================================

void FrontEnd::UnregisterScreenHandlers( void )
{
	RegisterScreenHandlers( true );
}

//==========================================================================
// FrontEnd::PreloadImages
//==========================================================================

void FrontEnd::PreloadImages( void )
{
	// TODO: this function allows you to preload some files
}

//==========================================================================
// FrontEnd::InitScreens
//
//		Load the screens and init the screen handlers.
//==========================================================================

bool FrontEnd::InitScreens( void )
{
	GAMETYPE GameType = m_GameType;

	if( GameType == GAMETYPE_UNDEFINED )
		GameType = GAMETYPE_ANY;

	// load the uber files first
	ScreenFile* pScreenFile = m_UberFiles.Child();

	while( pScreenFile )
	{
		if( pScreenFile->m_GameType == GameType )
			dblLoad( pScreenFile->m_FileName.get() );

		pScreenFile = pScreenFile->Next();
	}

	// now load the screens and whatever wasn't found in the uber
	pScreenFile = m_ScreenFiles.Child();

	while( pScreenFile )
	{
		if( pScreenFile->m_GameType & GameType )
			LoadScreen( pScreenFile->m_FileName.get() );

		pScreenFile = pScreenFile->Next();
	}

	//__________________________________________________________________________
	//
	// PC-only
	//__________________________________________________________________________

#ifdef DIRECTX_PC
	SetScreenHandler( "SCREEN_ID_LEVELSELECT_PC",		ScreenPCLevelSelect::ClassnameStatic() );
	SetScreenHandler( "SCREEN_ID_ONLINEPROFILE_PC",		ScreenOnlineProfile::ClassnameStatic() );
	SetScreenHandler( "SCREEN_ID_OPTIONS_PC",			ScreenOptions::ClassnameStatic() );
	SetScreenHandler( "SCREEN_ID_PAUSE_PC",				ScreenPause::ClassnameStatic() );
	SetScreenHandler( "SCREEN_ID_PCKEYBINDINGS",		ScreenPCKeyBindings::ClassnameStatic() );
	SetScreenHandler( "SCREEN_ID_STARTMENUNETWORK_PC",	ScreenPauseOnline::ClassnameStatic() );
	SetScreenHandler( "SCREEN_ID_STARTMENUONLINE_PC",	ScreenPauseOnline::ClassnameStatic() );
#endif

	//__________________________________________________________________________
	//
	// Register memcard popup functions...
	//__________________________________________________________________________

	if( g_MemCardManager )
	{
		g_MemCardManager->RegisterWaitUntilIdleUpdate( (void*) WaitUntilIdleCallBack );
		g_MemCardManager->RegisterPopupFunction( CMemoryCard::SHOWPOPUP, (void*)ShowPopupCallBack );
		g_MemCardManager->RegisterPopupFunction( CMemoryCard::HIDEPOPUP, (void*)HidePopupCallBack );
		g_MemCardManager->RegisterPopupFunction( CMemoryCard::POPUPRESULT, (void*)PopupResultCallBack );
		g_MemCardManager->RegisterPopupFunction( CMemoryCard::CLEARRESULT, (void*)PopupClearResultCallback );
	}

	return true;
}

//==========================================================================
// FrontEnd::SetInputFocus
//
//		Activate the controls and push the mapper onto the stack.
//==========================================================================

void FrontEnd::SetInputFocus( void )
{
	if( !m_ControlsActive )
	{
		m_ControlsActive = true;
		g_controlFocus.Push("Fluff");
	}

	return;
}

//==========================================================================
// FrontEnd::ReleaseInputFocus
//
//		Deactivate the controls and remove the mapper from the stack.
//==========================================================================

void FrontEnd::ReleaseInputFocus( void )
{
	if( m_ControlsActive )
	{
		m_ControlsActive = false;
		g_controlFocus.Pop("Fluff");
	}

	return;
}

//==========================================================================
// FrontEnd::GetScoreBoard
//
//		Returns a pointer to the scoreboard handler.
//==========================================================================

ScreenScoreBoard* FrontEnd::GetScoreBoard( void )
{
	return (ScreenScoreBoard*) GetScreenHandler( "SCREEN_ID_SCOREBOARD", ScreenScoreBoard::ClassnameStatic() );
}

//==========================================================================
// FrontEnd::GetPopupScreen
//
//		Returns a pointer to the specified popup handler.
//==========================================================================

ScreenBasePopup* FrontEnd::GetPopupScreen( const char* pScreenName )
{
	return (ScreenBasePopup*) GetScreenHandler( pScreenName );
}

//==========================================================================
// FrontEnd::GetPopupScreen
//
//		Returns a pointer to the memory card popup screen.
//==========================================================================

ScreenMemCard* FrontEnd::GetMemCardScreen( void )
{
	ScreenHandler* pScreenHandler = GetScreenHandler( "SCREEN_ID_MEMCARD" );

	if( pScreenHandler && 0 == strcmp( pScreenHandler->Classname(), ScreenMemCard::ClassnameStatic() ) )
		return (ScreenMemCard*) pScreenHandler;

	return 0;
}

//==========================================================================
// FrontEnd::GetPopupDialog
//
//		Returns a pointer to the dialog popup screen.
//==========================================================================

ScreenPopupDialog* FrontEnd::GetPopupDialog( void )
{
	ScreenHandler* pScreenHandler = GetScreenHandler( "SCREEN_ID_POPUPDIALOG" );

	if( pScreenHandler && 0 == strcmp( pScreenHandler->Classname(), ScreenPopupDialog::ClassnameStatic() ) )
		return (ScreenPopupDialog*) pScreenHandler;

	return 0;
}


//==========================================================================
// FrontEnd::GetErrorDialog
//
//		Returns a pointer to the dialog popup screen.
//==========================================================================

ScreenErrorDialog* FrontEnd::GetErrorDialog( void )
{
	ScreenHandler* pScreenHandler = GetScreenHandler( "SCREEN_ID_ERRORDIALOG" );

	if( pScreenHandler && 0 == strcmp( pScreenHandler->Classname(), ScreenErrorDialog::ClassnameStatic() ) )
		return (ScreenErrorDialog*) pScreenHandler;

	return 0;
}

//==========================================================================
// FrontEnd::ShowPopupDialog
//==========================================================================

void FrontEnd::ShowPopupDialog( bool Show )
{
	if( Show )
		ShowPopup( "SCREEN_ID_POPUPDIALOG" );
	else
		ClosePopup( "SCREEN_ID_POPUPDIALOG" );

	return;
}

//==========================================================================
// FrontEnd::ShowErrorDialog
//==========================================================================

void FrontEnd::ShowErrorDialog( bool Show )
{
	if( Show )
		ShowPopup( "SCREEN_ID_ERRORDIALOG" );
	else
		ClosePopup( "SCREEN_ID_ERRORDIALOG" );

	return;
}


//==========================================================================
// FrontEnd::GotoScoreBoard
//
//		Bring up the scoreboard and set input focus.
//==========================================================================

void FrontEnd::GotoScoreBoard( void )
{
	ShowScoreBoardBk( true );
	GotoScreen( "SCREEN_ID_SCOREBOARD" );
	ClearScreenHistory();
}

//==========================================================================
// FrontEnd::ShowScoreBoard
//
//		Displays the scoreboard.  The scoreboard has 2 modes, the "in-game"
//		score board which does not take input focus and has a transparent
//		background, and the regular mode which steals input focus and has an
//		opaque background.
//==========================================================================

void FrontEnd::ShowScoreBoard( bool Show )
{
	if( Show != m_ShowScoreBoard )
	{
		ScreenScoreBoard* pScoreBoard = GetScoreBoard();

		if( pScoreBoard )
		{
			if( Show )
				pScoreBoard->Show();
			else
				pScoreBoard->Hide();

			pScoreBoard->ShowBk( false );
		}

		m_ShowScoreBoard = Show;
	}

	return;
}

//==========================================================================
//==========================================================================
bool FrontEnd::ScoreBoardUp( void )
{
	ScreenScoreBoard* pScoreBoard = GetScoreBoard();
	return (pScoreBoard && pScoreBoard->GetVisible());
}

//==========================================================================
// FrontEnd::ShowScoreboardBk
//
//		Specifies whether the Scoreboard Background should be visible or not.
//==========================================================================

void FrontEnd::ShowScoreBoardBk( bool Show )
{
	ScreenScoreBoard* pScoreBoard = GetScoreBoard();

	if( pScoreBoard )
		pScoreBoard->ShowBk( Show );

	return;
}

//==========================================================================
// FrontEnd::ShowKeyboard
//
//		Call this function to display the virtual keyboard.
//==========================================================================

void FrontEnd::ShowKeyboard( void )
{
	ShowPopup( "SCREEN_ID_KEYBOARD" );
}

//==========================================================================
// FrontEnd::GetKeyboard
//
//		Returns a pointer to the keyboard screen handler.
//==========================================================================

ScreenKeyboard* FrontEnd::GetKeyboard( void )
{
	return (ScreenKeyboard*) GetScreenHandler( "SCREEN_ID_KEYBOARD" );
}

//==========================================================================
// FrontEnd::ShowObjectives
//==========================================================================

void FrontEnd::ShowObjectives( bool Show )
{
	if( Show )
		ShowScreen( "SCREEN_ID_OBJECTIVES" );
	else
		HideScreen( "SCREEN_ID_OBJECTIVES" );

	m_ShowObjectives = Show;
}

//==========================================================================
// FrontEnd::ToggleObjectives
//==========================================================================

void FrontEnd::ToggleObjectives( void )
{
	ShowObjectives( !m_ShowObjectives );
}

//==========================================================================
// FrontEnd::ShowMissionBriefing
//==========================================================================

void FrontEnd::ShowMissionBriefing( void )
{
	ScreenHandler* pHandler = GetScreenHandler( "SCREEN_ID_BRIEFING" );
	ScreenBriefing* pBriefing = 0;

	if( pHandler && 0 == strcmp( pHandler->Classname(), ScreenBriefing::ClassnameStatic() ) )
		pBriefing = (ScreenBriefing*) pHandler;

	if( pBriefing )
	{
		const char* pMessage = "No mission objectives defined for this mode.";

		switch( g_gameEnvironment.Get() )
		{
		case 5: // subway
			pMessage = "Prevent Shaun from getting away by using the subway system.";
			break;
		case 10: // mall
			pMessage = "Find a change of clothes and escape the cops looking for you in the mall.";
			break;
		}

		pBriefing->SetPrimaryObjective( 0, pMessage );
		pBriefing->SetSecondaryObjective( 0, "None" );
	}

//	SPause pauseMessage;
//	pauseMessage.d_type = SPause::PAUSE;
//	pauseMessage.d_menu = "SCREEN_ID_BRIEFING";
//	g_messageDispatcher.PostMessageToAll("Pause", (void*)&pauseMessage, INVALID_OBJECT_HANDLE);

	GotoScreen( "SCREEN_ID_BRIEFING" );
}

//==========================================================================
// FrontEnd::GetInvite
//==========================================================================

ScreenInvitations* FrontEnd::GetInvite( void )
{
	return (ScreenInvitations*) GetScreenHandler( "SCREEN_ID_INVITATIONS", ScreenInvitations::ClassnameStatic() );
}
//==========================================================================
// FrontEnd::SetInvite
//==========================================================================

void FrontEnd::SetInvite( Invitation::Icon Icon, const char *pMessage )
{
	ScreenInvitations* pInvite = GetInvite();

	if( pInvite )
		pInvite->SetInvite( Icon, pMessage );

	return;
}

//==========================================================================
// FrontEnd::ShowInviteIcon
//==========================================================================

void FrontEnd::ShowInviteIcon( unsigned char Color )
{
	ScreenInvitations* pInvite = GetInvite();

	if( pInvite )
		pInvite->SetIconOpacity( Color );

	return;
}

//==========================================================================
// FrontEnd::ShowInviteMessage
//==========================================================================

void FrontEnd::ShowInviteText( unsigned char Color )
{
	ScreenInvitations* pInvite = GetInvite();

	if( pInvite )
		pInvite->SetMessageOpacity( Color );

	return;
}


//==========================================================================
// FrontEnd::UpdateInvite
//==========================================================================

void FrontEnd::UpdateInvite( void )
{
#if defined(PS2) || defined(DIRECTX_PC)
    // NOTE: All times are in seconds.
	 static unsigned int TimeIconStarted = 0;            // Clock for keeping track of icon life cycle
    const float FadeIn = 1.0f;            // Amount of time to fade in icon
    const float TextFadeOut = 4.0f;       // Time to start fading out text
    const float TextFadeOutDone = 5.0f;   // When the text should be done fading out
    const float FadeOut = 10.0f;           // When to start fading out the icon
    const float FadeOutDone = 11.0f;       // When the icon should be done fading out
    static bool Active = false;           // Is an icon currently active?

    // See if we have any icons to activate
    if (OSI::getSingletonPtr() && !Active)
    {
        bool Activate = false;
        if (OSI::getSingletonPtr()->isNewClanInvitationWaiting())
        {
            SetInvite(Invitation::ClanInvite_Received, "Clan invite");
            Activate = true;
        }
        else if (OSI::getSingletonPtr()->isNewFriendInvitationWaiting())
        {
            SetInvite(Invitation::FriendInvite_Received, "Friend invite");
            Activate = true;
        }
        else if (OSI::getSingletonPtr()->isNewMatchInvitationWaiting())
        {
            SetInvite(Invitation::GameInvite_Received, "Match invite");
            Activate = true;
        }

        // If we activated an icon, set parameters for displaying it
        if (Activate)
        {
            Active = true;
				TimeIconStarted = g_timer.GetOSTime();
        }
    }

    // If we have an icon currently active, take it through its lifecycle
    else if (Active)
    {
        float Clock = (g_timer.GetOSTime() - TimeIconStarted) * 0.001f;

        // Doing the initial fade in
        if (Clock <= FadeIn)
        {
            unsigned char Color = static_cast<unsigned char>((Clock / FadeIn) * 255.0f);
            ShowInviteIcon(Color);
            ShowInviteText(Color);
        }
        // Done fading in, do full brightness
        else if (Clock > FadeIn && Clock < TextFadeOut)
        {
            ShowInviteIcon(255);
            ShowInviteText(255);
        }
        // Still displaying icon, wanting to fade the text out
        else if (Clock >= TextFadeOut && Clock <= TextFadeOutDone)
        {
            unsigned char Color = static_cast<unsigned char>((TextFadeOutDone - Clock) / (TextFadeOutDone - TextFadeOut) * 255.0f);
            ShowInviteText(Color);
        }
        // Done fading text, completely hide it
        else if (Clock > TextFadeOutDone && Clock < FadeOut)
        {
            ShowInviteText(0);
        }
        // Done displaying icon, fade the whole thing out
        else if (Clock >= FadeOut && Clock <= FadeOutDone)
        {
            unsigned char Color = static_cast<unsigned char>((FadeOutDone - Clock) / (FadeOutDone - FadeOut) * 255.0f);
            ShowInviteIcon(Color);
        }
        // We're done, reset and wait for another icon
        else if (Clock >= FadeOutDone)
        {
            ShowInviteIcon(0);
            Active = false;
        }
    }

	 if( Active )
		 ShowScreen( "SCREEN_ID_INVITATIONS" );
	 else
		 HideScreen( "SCREEN_ID_INVITATIONS" );
#endif

	return;
}

//==========================================================================
// FrontEnd::SetErrorMessage
//==========================================================================

void FrontEnd::SetErrorMessage( const char* pMessage )
{
	ScreenHandler* pHandler = GetScreenHandler( "SCREEN_ID_ERRORMESSAGE" );

	if( pHandler && 0 == stricmp( pHandler->Classname(), ScreenErrorMessage::ClassnameStatic() ) )
	{
		ScreenErrorMessage* pErrorScreen = (ScreenErrorMessage*) pHandler;

		pErrorScreen->SetErrorMessage( pMessage );
	}

	return;
}

//==========================================================================
// FrontEnd::ShowErrorMessage
//==========================================================================

void FrontEnd::ShowErrorMessage( bool Show )
{
	Screen* pErrorMessage = GetScreen( "SCREEN_ID_ERRORMESSAGE" );

	if( pErrorMessage )
	{
		if( Show )
			ShowPopup( pErrorMessage );
		else
			ClosePopup( pErrorMessage );
	}

	return;
}

//==========================================================================
// FrontEnd::InitDebugOutput
//==========================================================================

#define DEBUG_SCALE     0.85f
#define REFERENCE_CHAR  'M'
#define MIN_TEXT_HEIGHT 8.0f
#define MAX_TEXT_HEIGHT 25.0f

void FrontEnd::InitDebugOutput( void )
{
	Font* pFont = Font::GetFirstFont();
	float cx = Sprite::SimulatedScreenWidth();
	float cy = Sprite::SimulatedScreenHeight();

	int rows = GetDebugOutputRows();
	float th = Math::Min( cy / (float)rows, MAX_TEXT_HEIGHT );
	if( th < MIN_TEXT_HEIGHT )
	{
		th = MIN_TEXT_HEIGHT;
		rows = int( cy / th );
	}
	float ay = th * rows;

	m_DebugRect.Reset();
	m_DebugRect.Create( pFont->m_texture.handle, TS_NO_TEXTURE );
	m_DebugRect.SetColor( 0, 0, 0, 128 );
	m_DebugRect.SetSize( cx, ay );
	m_DebugRect.ScreenPosition( 0.5f, 0.5f * ( 1.0f - DEBUG_SCALE * ( 1.0f - ay / cy ) ) );
	m_DebugRect.SetOrigin( 0.5f, 0.5f );
	m_DebugRect.SetPriority( 0x7eff );
	m_DebugRect.SetScale( DEBUG_SCALE, DEBUG_SCALE );

	int i;
	float x = 0.0f;
	float y = 0.0f;
	float cw = pFont->GetCharWidth( REFERENCE_CHAR );
	int cols = int( cx / ( cw * th / pFont->GetFontHeight() ) );

	for( i = 0; i < rows; i++ )
	{
		m_DebugText[i].SetParent( &m_DebugRect );
		m_DebugText[i].SetFont( pFont );
		m_DebugText[i].SetPosition( x, y );
		m_DebugText[i].SetTextHeight( th );
		m_DebugText[i].SetOrigin( 0.0f, 0.0f );
		m_DebugText[i].SetTextFixedSpacing( cw );

		y = y + th;
	}

	m_DebugDisplay.SetOutput( &g_debugOutput );
	m_DebugDisplay.DescribeDisplay( rows, cols );

	ShowDebugOutput( false );

#if 0
	g_debugOutput.Print(false,
			"Debug font: name = %s, width(%c) = %f, height = %f",
			pFont->Name(), REFERENCE_CHAR, pFont->GetCharWidth(REFERENCE_CHAR),
			pFont->GetFontHeight());
	g_debugOutput.Print(false, "Debug screen: %f wide x %f high", cx, cy);
	g_debugOutput.Print(false, "Debug display: %d cols x %d rows", cols, rows);
	g_debugOutput.Print(false, "Debug adjust: scale = %f, char width = %f",
			DEBUG_SCALE, cw);
	g_debugOutput.Print(false, "Debug height: text = %f, rect = %f", th, ay);
#endif
}

//==========================================================================
// FrontEnd::ShowDebugOutput
//==========================================================================

void FrontEnd::ShowDebugOutput( bool Show )
{
	m_ShowDebugOutput = Show;
}

//==========================================================================
// FrontEnd::ToggleDebugOutput
//==========================================================================

void FrontEnd::ToggleDebugOutput( void )
{
	if( !m_ShowDebugOutput )
		ShowDebugOutput( true );
	else
		ShowDebugOutput( false );

	return;
}

//==========================================================================
// FrontEnd::UpdateDebugOutput
//==========================================================================

void FrontEnd::UpdateDebugOutput( void )
{
	u8 alpha = 0;

	if( m_ShowDebugOutput )
	{
		alpha = 255;

		m_DebugDisplay.LockBuffer();
		m_DebugDisplay.GotoTop();

		int i = 0;
		while( !m_DebugDisplay.PastEnds() )
		{
			m_DebugText[i].SetTextNoTranslate( m_DebugDisplay.GetLine() );
			++m_DebugDisplay;
			++i;
		}

		m_DebugDisplay.UnlockBuffer();
	}

	m_DebugRect.m_state.familycolor.a = alpha;
	m_DebugRect.Update();
}

//==========================================================================
// FrontEnd::GetDebugOutputRows
//==========================================================================

int FrontEnd::GetDebugOutputRows( void )
{
	return sizeof(m_DebugText)/sizeof(m_DebugText[0]);
}

//==========================================================================
// FrontEnd::SetCinematicInProgress
//
//		Front End set flag cinematic in progress
//==========================================================================

void FrontEnd::SetCinematicInProgress(bool Flag)
{
	m_CinematicInProgress = Flag;
}

//==========================================================================
// FrontEnd::WaitUntilIdleCallBack
//
//		This function is registered with the memory card manager and is called
//		while waiting for a memory card operation to complete.
//==========================================================================

void FrontEnd::WaitUntilIdleCallBack( void )
{
	g_input.UpdateInput();
	g_controlFocus.Update();
	g_FrontEnd.Update();
}

//==========================================================================
// FrontEnd::ShowPopupCallBack
//
//		This function is registered with the memory card manager and is called
//		when a popup must be displayed.
//==========================================================================

void FrontEnd::ShowPopupCallBack( void )
{
	ScreenMemCard* pMemCard = g_FrontEnd.GetMemCardScreen();

//		if( pMemCard )
//			pMemCard->SetResult( MC_NONE );

	if( g_MemCardManager->GetAutoSaveIcon())
//	if( g_MemCardManager->GetState() == CMemoryCard::CMEMORYCARD_SAVE && g_MemCardManager->GetAutosave() )
	{
		g_FrontEnd.ShowScreen( "SCREEN_ID_AUTOSAVE" );
	}
	else
	{
		ScreenMemCard* pMemCard = g_FrontEnd.GetMemCardScreen();

		if( pMemCard )
		{
			int n = g_MemCardManager->PopupInfo.NumChoices;
			char* pChoiceText;
			int ChoiceResult;

			pMemCard->Reset();
			pMemCard->SetTitle( g_MemCardManager->PopupInfo.Title );
			pMemCard->SetMessage( g_MemCardManager->PopupInfo.String );

			for( int i = 0; i < n; i++ )
			{
				pChoiceText = g_MemCardManager->PopupInfo.Choices[i].ChoiceText;
				ChoiceResult = g_MemCardManager->PopupInfo.Choices[i].ChoiceResult;
				pMemCard->AddMenuItem( pChoiceText, ChoiceResult );
			}

			pMemCard->SelectMenuItem( g_MemCardManager->PopupInfo.Default );
			pMemCard->SetResult( MC_NONE );
		}

		if( g_FrontEnd.GetActiveScreen() != g_FrontEnd.GetScreen( "SCREEN_ID_MEMCARD" ) )
			g_FrontEnd.ShowPopup( "SCREEN_ID_MEMCARD" );
	}

	return;
}

//==========================================================================
// FrontEnd::HidePopupCallBack
//
//		This function is registered with the memory card manager and is called
//		when the active popup should be removed.
//==========================================================================

void FrontEnd::HidePopupCallBack( void )
{
	if( g_MemCardManager->GetInProgress() == CMemoryCard::CMEMORYCARD_NOTHING_IN_PROGRESS )
	{
		g_FrontEnd.ClosePopup( "SCREEN_ID_MEMCARD" );
	}
	else
	{
		ScreenMemCard* pMemCard = g_FrontEnd.GetMemCardScreen();

		if( pMemCard )
			pMemCard->Reset();
	}

	return;
}

//==========================================================================
// FrontEnd::PopupResultCallBack
//
//		This function is registered with the memory card manager and is called
//		to retrieve the result of a popup window.
//==========================================================================

int FrontEnd::PopupResultCallBack( void )
{
	int Result = 0xff;

	if( g_MemCardManager->GetAutoSaveIcon() )
		Result = g_MemCardManager->PopupInfo.PassThrough;
	else
	{
		ScreenMemCard* pMemCard = g_FrontEnd.GetMemCardScreen();

		if( pMemCard )
		{
			Result = pMemCard->GetResult();

//			if (pMemCard->GetActive() == false)
//				Result = MC_CANCEL_CURRENT_OPERATION;
		}
	}

	return Result;
}

//==========================================================================
// FrontEnd::PopupClearResult
//
// resets the result
//==========================================================================

void FrontEnd::PopupClearResultCallback( void )
{
	ScreenMemCard* pMemCard = g_FrontEnd.GetMemCardScreen();

	if( pMemCard )
		pMemCard->SetResult( MC_NONE );
}

FrontEnd g_FrontEnd;


