#include "TTLPCH.h"
#include "frontend/frontend.h"
#include "frontend/savegame.h"
#include "frontend/inputdriver.h"
#include "frontend/texteffects.h"
#include "frontend/screens/screen_MainMenu.h"
#include "frontend/screens/screen_Popups.h"
#include "data/screens/mainmenu.h"
#include "data/screens/OnlineScreens.h"
#include "frontend/savegame.h"
#include "environ/ConfigMgr.h"
#include "Network/NetZ/NetZConnection.h"
#include "GameHelper/Translator.h"
#include "main/cheats.h"

#include "main/TTL.h"

#if defined (_XBOX)
#include "platform/xbox/xboxlive.h"
#include "main/TTL.h"
#endif
#include "Voice/NetZ/CVoiceChat.h"

#include "frontend\screens\screen_NetworkError.h"
extern const char* g_RemoteIPAddress;

int g_mainMenuSelection;
Screen*	g_LastScreen;
bool		g_MainMenuSignOut = false;

enum
{
	COMMAND_SIGNOUT_CANCEL = 0x100,
	COMMAND_SIGNOUT_OK,
	COMMAND_NETWORKMSG_OK,
	COMMAND_INVITE_NO,
	COMMAND_INVITE_YES,
};

//=============================================================================
// ScreenMainMenu::ScreenMainMenu - The default constructor
//=============================================================================

ScreenMainMenu::ScreenMainMenu( void )
{
}

//=============================================================================
// ScreenMainMenu::~ScreenMainMenu - The default destructor
//=============================================================================

ScreenMainMenu::~ScreenMainMenu( void )
{
}


//=============================================================================
// ScreenMainMenu::SetDescription
//=============================================================================

void ScreenMainMenu::SetDescription( const char *pDescription )
{
	SetText( ID_MAINMENU_DESCRIPTION, pDescription );
}

//=============================================================================
// ScreenMainMenu::SignOut
//=============================================================================

void ScreenMainMenu::SignOut( void )
{
	ScreenPopupDialog* pDialog = GetPopupDialog();

	if( pDialog )
	{
		pDialog->Clear();
		pDialog->SetTitle(0);
		pDialog->SetMessage( "Would you like to return to the Title screen?" );
		pDialog->AddMenuItem( "OK", COMMAND_SIGNOUT_OK, 0 );
		pDialog->AddMenuItem( "Cancel", COMMAND_SIGNOUT_CANCEL, 0 );
		pDialog->SelectMenuItem( COMMAND_SIGNOUT_CANCEL );
	}

	ShowPopupDialog();
}

//=============================================================================
// ScreenMainMenu::OnInitialize
//=============================================================================

void ScreenMainMenu::OnInitialize( void )
{
	g_MainMenuSignOut = false;

	ScreenBaseClass::OnInitialize();

	UseHighlighting( true );

	SetHelpRect( ID_MAINMENU_HELPRECT );

#ifdef _XBOX
	SetInitialButton( ID_MAINMENU_SINGLEPLAYER );
	SetNextButton( ID_MAINMENU_ONLINE );
	SetNextButton( ID_MAINMENU_SYSTEMLINK );
	SetNextButton( ID_MAINMENU_SETTINGS );
	SetNextButton( ID_MAINMENU_FRIENDS );
	SetNextButton( ID_MAINMENU_APPEAROFFLINE );
	SetNextButton( ID_MAINMENU_SIGNIN );
#else
	SetInitialButton( ID_MAINMENU_SINGLEPLAYER );
	SetNextButton( ID_MAINMENU_ONLINE );
	SetNextButton( ID_MAINMENU_SYSTEMLINK );
	SetNextButton( ID_MAINMENU_SETTINGS );
#ifdef DIRECTX_PC
	GetSprite(ID_MAINMENU_HELPRECT)->HideAll();
#endif
#endif

	SetCursor( ID_MAINMENU_CURSOR );

#ifdef DIRECTX_PC
	SetNextScreen( ID_MAINMENU_SETTINGS, "SCREEN_ID_OPTIONS_PC" );
#else
	SetNextScreen( ID_MAINMENU_SETTINGS, "SCREEN_ID_OPTIONS" );
#endif

#if defined (WIN32) && !defined (_XBOX) && !defined (DIRECTX_PC)
	EnableButton( ID_MAINMENU_ONLINE, false );
#endif

#ifdef _XBOX
	m_XboxLiveEnabled = true;
#endif

	return;
}

//=============================================================================
// ScreenMainMenu::OnScreenIntro
//=============================================================================

void ScreenMainMenu::OnScreenIntro( void )
{
	g_MainMenuSignOut = false;

	ScreenManager* pScreenManager = GetScreenManager();

	if( pScreenManager )
		pScreenManager->ClearScreenHistory();

	Printf( ID_MAINMENU_CURRENTPROFILE, g_translator.Translate( "Current Profile: %s" ), g_playerProfile.GetAccountName() );

	ScreenBaseClass::OnScreenIntro();
	m_state = STATE_IDLE;
	g_mainMenuSelection = 0;

#ifdef _XBOX
	if (CXboxLive::GetGameInvite() == true)
	{
		g_acceptingInvitation = true;
	}

	//g_FrontEnd.ShowScreen( "SCREEN_ID_LIVELOGIN" );
#endif

#ifdef DIRECTX_PC
	g_MenuCursor.SetHidden(false);
	StartButton.Show(true);
	BackButton.Show(true);
#endif

	// --- we must always tear down our network connection
	CNetZConnection::TearDown( );
	return;
}

//=============================================================================
// ScreenDifficulty::OnScreeenUpdate
//=============================================================================

void ScreenMainMenu::OnScreenUpdate( void )
{
	if( g_acceptingInvitation )
	{
		g_messageDispatcher.SendMessageToAll("AcceptInvitation", NULL, INVALID_OBJECT_HANDLE );
		GotoScreen("SCREEN_ID_LANCONNECT");
		return;
	}

#ifdef DIRECTX_PC
	// CALL THE BASE CLASS SCREEN UPDATE FUNCTION FOR MOUSE
	// UI FUNCTIONALITY
	ScreenBaseClass::OnScreenUpdate();
#endif
	switch( m_state )
	{
		case STATE_IDLE:
			break;

		// --- read the network configuration and come back
		case STATE_GET_NETWORK_CONFIG:
#ifdef PS2
			if( !PS2App::IAm()->HasNetworkAdapter() )
			{
				m_connectionTimeOut = g_timer.GetOSTime() + ( 4000 );

				ScreenPopupDialog* pDialog = GetPopupDialog();

				if( pDialog )
				{
					pDialog->Clear();
					pDialog->SetTitle( "Network Adaptor" );
					pDialog->SetMessage( "A network adaptor (Ethernet/modem) (for PlayStation®2) is required for multiplayer games. Power down, install your network adaptor, and try again." );
					pDialog->AddMenuItem( "OK", COMMAND_NETWORKMSG_OK, 0 );
					pDialog->SelectMenuItem( COMMAND_NETWORKMSG_OK );
				}

				m_state = STATE_WAIT;
				ShowPopupDialog();
				break;
			}
			GotoScreen( "SCREEN_ID_LANCONFIG" );
			break;
#endif
			m_state = STATE_START_CONNECTION;
			break;


		// --- if we have a valid configuration start the connection process
		case STATE_START_CONNECTION:
			m_state = STATE_INITIATE_CONNECTION;
			break;

		// --- initiate the connection if we don't already have one
		case STATE_INITIATE_CONNECTION:
#ifndef PS2
			if( !CNetZConnection::InitiateConnection( false ) )
			{
				m_state = STATE_CONNECTION_FAILED;
				break;
			}
#endif
			m_state = STATE_WAIT_FOR_CONNECTION;
			break;

		// --- wait for the network to come up, and show progress
		case STATE_WAIT_FOR_CONNECTION:
			m_state = STATE_CONNECTION_VALID;
			break;

		// --- network is up, we can now proceed with multiplayer
		case STATE_CONNECTION_VALID:
			if( g_mainMenuSelection == ID_MAINMENU_ONLINE )
				OnCommandMultiPlayerOnline();
			else if( g_mainMenuSelection == ID_MAINMENU_SYSTEMLINK )
				OnCommandMultiPlayerNetwork();
			m_state = STATE_IDLE;
			break;

		// --- connection failed, tear down and show failure dialog
		case STATE_CONNECTION_FAILED:
			CNetZConnection::TearDown( );

#ifdef _XBOX
			if( g_mainMenuSelection == ID_MAINMENU_ONLINE && !CXboxLive::AreWeLoggedIn() )
			{
				GotoScreen("SCREEN_ID_UIXLOGIN");
				break;
			}
#endif
			ScreenNetworkError::SetNetworkError( ScreenNetworkError::ERROR_CONNECTION_FAILED );
			GotoScreen( "SCREEN_ID_LANERROR" );
			m_state = STATE_WAIT;
			break;

		// --- wait a tick before going back to the idle state
		case STATE_WAIT:
			m_state = STATE_IDLE;
			break;
	}

#ifdef _XBOX
	bool AppearOffline = CXboxLive::GetOnlineStatus();

	if( AppearOffline )
		SetText( ID_MAINMENU_APPEAROFFLINE, "Appear Offline" );
	else
		SetText( ID_MAINMENU_APPEAROFFLINE, "Appear Online" );

	if( CXboxLive::AreWeLoggedIn() )
		SetText( ID_MAINMENU_SIGNIN, "Sign Out of Xbox Live" );
	else
		SetText( ID_MAINMENU_SIGNIN, "Sign In to Xbox Live" );

	if( m_XboxLiveEnabled != CXboxLive::AreWeLoggedIn() )
	{
		m_XboxLiveEnabled = CXboxLive::AreWeLoggedIn();
		int id = GetCurrentButtonId();
		if( id == ID_MAINMENU_FRIENDS || id == ID_MAINMENU_APPEAROFFLINE )
		{
			SelectInitialButton();
		}
		EnableButton( ID_MAINMENU_FRIENDS, m_XboxLiveEnabled );
		EnableButton( ID_MAINMENU_APPEAROFFLINE, m_XboxLiveEnabled );
	}

#endif

	switch( GetCurrentButtonId() )
	{
	case ID_MAINMENU_SINGLEPLAYER:
		SetDescription( "Play a single player game." );
		break;

	case ID_MAINMENU_ONLINE:
	#ifdef _XBOX // bug 7777
		SetDescription( "Play a multiplayer game over Xbox Live." );
	#else
		SetDescription( "Play a multiplayer game over the internet." );
	#endif
		break;

	case ID_MAINMENU_SYSTEMLINK:
		SetDescription( "Play a multiplayer game on your local network." );
		break;

	case ID_MAINMENU_SETTINGS:
		SetDescription( "Customize your game settings for the current profile." );
		break;

#ifdef _XBOX
	case ID_MAINMENU_FRIENDS:
		SetDescription( "View your friends list." );
		break;

	case ID_MAINMENU_APPEAROFFLINE:
		if( CXboxLive::GetOnlineStatus() )
			SetDescription( "Appear Offline." );
		else
			SetDescription( "Appear Online." );
		break;

	case ID_MAINMENU_SIGNIN:
		if( CXboxLive::AreWeLoggedIn() )
			SetDescription( "Sign out of Xbox Live." );
		else
			SetDescription( "Sign in to Xbox Live." );
		break;
#endif

	default:
		SetDescription(0);
		break;
	}


	return;
}

//=============================================================================
// ScreenMainMenu::OnButtonCancel
//=============================================================================

void ScreenMainMenu::OnButtonCancel( void )
{
	SignOut();
}

//=============================================================================
// ScreenMainMenu::OnButtonPressed
//=============================================================================

void ScreenMainMenu::OnButtonPressed( int Button )
{
	ScreenBaseClass::OnButtonPressed( Button );

#ifndef CONSUMER_BUILD
	switch( Button )
	{
	case FluffControlMapper::SQUARE:
		// We are in single player
		g_configmgr.SetOptionByName("gameModes", "dev");
		g_configmgr.AcceptChanges();
		g_messageDispatcher.SendMessageToAll( "NextLoop", ( void* )"Developer", INVALID_OBJECT_HANDLE);
		break;

	case FluffControlMapper::SELECT:
		g_configmgr.SetOptionByName("gameModes", "war");
		g_configmgr.AcceptChanges();
		g_messageDispatcher.SendMessageToAll( "NextLoop", ( void* )"MultiPlayer_Internet_NewUI", INVALID_OBJECT_HANDLE);
		break;

	default:
		ScreenBaseClass::OnButtonPressed( Button );
		break;
	}
#endif // CONSUMER_BUILD

	return;
}

//=============================================================================
// ScreenMainMenu::OnPopupShow
//=============================================================================

void ScreenMainMenu::OnPopupShow( void )
{
	ScreenBaseClass::OnPopupShow();
	ShowScreenExit();
}

//=============================================================================
// ScreenMainMenu::OnPopupHide
//=============================================================================

void ScreenMainMenu::OnPopupHide( void )
{
	ScreenBaseClass::OnPopupHide();
	ShowScreenIntro();
}

//=============================================================================
// ScreenMainMenu::OnDialogCommand
//=============================================================================

void ScreenMainMenu::OnDialogCommand( int Command )
{
	switch( Command )
	{
	case COMMAND_SIGNOUT_OK:
		g_MainMenuSignOut = true;
		GotoScreen( "SCREEN_ID_TITLE" );
	#ifdef _XBOX
		if( CXboxLive::AreWeLoggedIn() )
			CXboxLive::LogOutUIX();
	#endif
		break;

	case COMMAND_SIGNOUT_CANCEL:
		EndScreen(0);
		break;

	case COMMAND_NETWORKMSG_OK:
		EndScreen(0);
		break;

	}

	return;
}

//=============================================================================
// ScreenMainMenu::OnCommand
//=============================================================================

void ScreenMainMenu::OnCommand( int Command )
{
	if( m_state != STATE_IDLE ) return;

	switch( Command )
	{
	case ID_MAINMENU_SINGLEPLAYER:
		OnCommandSinglePlayer();
		break;

	case ID_MAINMENU_SYSTEMLINK:
		g_mainMenuSelection = m_modeSelected = ID_MAINMENU_SYSTEMLINK;
		m_state = STATE_GET_NETWORK_CONFIG;
		break;

	case ID_MAINMENU_ONLINE:
		g_mainMenuSelection = m_modeSelected = ID_MAINMENU_ONLINE;
		m_state = STATE_GET_NETWORK_CONFIG;
		break;

#ifdef _XBOX
	case ID_MAINMENU_FRIENDS:
		OnCommandFriends();
		break;

	case ID_MAINMENU_APPEAROFFLINE:
		OnCommandAppearOffline();
		break;

	case ID_MAINMENU_SIGNIN:
		OnCommandSignIn();
		break;
#endif
	}

	return;
}

//=============================================================================
// ScreenMainMenu::OnCommandSinglePlayer
//=============================================================================

void ScreenMainMenu::OnCommandSinglePlayer( void )
{
	// We are in single player
	g_configmgr.SetOptionByName("gameModes", "sp");
	g_configmgr.AcceptChanges();

	g_messageDispatcher.SendMessageToAll( "NextLoop", ( void* )"SinglePlayer", INVALID_OBJECT_HANDLE );
}

//=============================================================================
// ScreenMainMenu::OnCommandMultiPlayerOnline
//=============================================================================

void ScreenMainMenu::OnCommandMultiPlayerOnline( void )
{
	// We are in single player
#ifdef _XBOX
	g_CVoiceChat.setMultiPlayMode( CVoiceChat::ONLINE_PLAY );
#endif
	g_configmgr.Initialize();
	g_configmgr.SetOptionByName("gameModes", "war");
	g_configmgr.AcceptChanges();

	g_messageDispatcher.SendMessageToAll( "NextLoop", ( void* )"MultiPlayer_Internet", INVALID_OBJECT_HANDLE);
}

//=============================================================================
// ScreenMainMenu::OnCommandMultiPlayerNetwork
//=============================================================================

void ScreenMainMenu::OnCommandMultiPlayerNetwork( void )
{
	// We are in single player
#ifdef _XBOX
	g_CVoiceChat.setMultiPlayMode( CVoiceChat::LAN_PLAY );
#endif

	g_configmgr.Initialize();
	g_configmgr.SetOptionByName("gameModes", "war");
	g_configmgr.AcceptChanges();

	g_messageDispatcher.SendMessageToAll( "NextLoop", (void*)"MultiPlayer_LAN", INVALID_OBJECT_HANDLE );
}

#ifdef _XBOX
//=============================================================================
// ScreenMainMenu::OnCommandFriends
//=============================================================================

void ScreenMainMenu::OnCommandFriends( void )
{
	GotoScreen("SCREEN_ID_UIXFRIENDS");
}

//=============================================================================
// ScreenMainMenu::OnCommandAppearOffline
//=============================================================================

void ScreenMainMenu::OnCommandAppearOffline( void )
{
	CXboxLive::ToggleOnlineStatus();
}

//=============================================================================
// ScreenMainMenu::OnCommandSignIn
//=============================================================================

void ScreenMainMenu::OnCommandSignIn( void )
{
	if( CXboxLive::AreWeLoggedIn() )
		CXboxLive::LogOutUIX();
	else
		GotoScreen("SCREEN_ID_UIXLOGIN");

	return;
}
#endif

//=============================================================================
// ScreenDifficulty::OnInitialize
//=============================================================================

void ScreenDifficulty::OnInitialize( void )
{
	ScreenBaseClass::OnInitialize();

	UseHighlighting( true );

	SetHelpRect( ID_DIFFICULTY_HELPRECT );
	SetInitialButton( ID_DIFFICULTY_EASY );
	SetNextButton( ID_DIFFICULTY_NORMAL );
	SetNextButton( ID_DIFFICULTY_HARD );
	SetNextButton( ID_DIFFICULTY_ULTIMATE );

	SetCursor( ID_DIFFICULTY_CURSOR );
#ifdef DIRECTX_PC
	SetNextScreen( ID_DIFFICULTY_EASY,     "SCREEN_ID_LEVELSELECT_PC" );
	SetNextScreen( ID_DIFFICULTY_NORMAL,   "SCREEN_ID_LEVELSELECT_PC" );
	SetNextScreen( ID_DIFFICULTY_HARD,     "SCREEN_ID_LEVELSELECT_PC" );
	SetNextScreen( ID_DIFFICULTY_ULTIMATE, "SCREEN_ID_LEVELSELECT_PC" );
#else
	SetNextScreen( ID_DIFFICULTY_EASY,     "SCREEN_ID_LEVELSELECT" );
	SetNextScreen( ID_DIFFICULTY_NORMAL,   "SCREEN_ID_LEVELSELECT" );
	SetNextScreen( ID_DIFFICULTY_HARD,     "SCREEN_ID_LEVELSELECT" );
	SetNextScreen( ID_DIFFICULTY_ULTIMATE, "SCREEN_ID_LEVELSELECT" );
#endif

	SetText(ID_DIFFICULTY_ULTIMATE,"OMFG!");
}

//=============================================================================
// ScreenDifficulty::OnScreenIntro
//=============================================================================

void ScreenDifficulty::OnScreenIntro( void )
{
	ScreenBaseClass::OnScreenIntro();

	Printf( ID_DIFFICULTY_PROFILENAME, g_translator.Translate( "Current Profile: %s" ), g_playerProfile.GetAccountName() );

	SelectButton( ID_DIFFICULTY_NORMAL );

#ifdef DIRECTX_PC
	GetSprite(ID_DIFFICULTY_HELPRECT)->HideAll();
#endif
	return;
}

//=============================================================================
// ScreenDifficulty::OnScreenExit
//=============================================================================

void ScreenDifficulty::OnScreenExit( void )
{
	ScreenBaseClass::OnScreenExit();
}

//=============================================================================
// ScreenDifficulty::OnButtonCancel
//=============================================================================

void ScreenDifficulty::OnButtonCancel( void )
{
	// go back to the InitialLoop
	g_messageDispatcher.SendMessageToAll( "SetupBackup", NULL, INVALID_OBJECT_HANDLE );

	ScreenBaseClass::OnButtonCancel();
}
//=============================================================================
// ScreenDifficulty::OnCommand
//=============================================================================

void ScreenDifficulty::OnCommand( int Command )
{
	switch( Command )
	{
	case ID_DIFFICULTY_EASY:
		g_GlobalProfile.GameSettings.Difficulty = DIFFICULTY_EASY;
		break;
	case ID_DIFFICULTY_NORMAL:
		g_GlobalProfile.GameSettings.Difficulty = DIFFICULTY_NORMAL;
		break;
	case ID_DIFFICULTY_HARD:
		g_GlobalProfile.GameSettings.Difficulty = DIFFICULTY_HARD;
		break;
	case ID_DIFFICULTY_ULTIMATE:
		g_GlobalProfile.GameSettings.Difficulty = DIFFICULTY_ULTIMATE;
		break;
	}

	return;
}

//=============================================================================
// ScreenDifficulty::OnPopupShow
//=============================================================================

void ScreenDifficulty::OnPopupShow( void )
{
	ScreenBaseClass::OnPopupShow();
	ShowScreenExit();
}

//=============================================================================
// ScreenDifficulty::OnPopupHide
//=============================================================================

void ScreenDifficulty::OnPopupHide( void )
{
	ScreenBaseClass::OnPopupHide();
	ShowScreenIntro();
}

#ifndef DIRECTX_PC
//=============================================================================
// ScreenLevelSelect::GetListBox
//=============================================================================

GuiListBox* ScreenLevelSelect::GetListBox( void )
{
	return m_pListBox;
}

//=============================================================================
// ScreenLevelSelect::LaunchSinglePlayer
//=============================================================================

void ScreenLevelSelect::LaunchSinglePlayer( int Level )
{
	// get rid of screen
	GotoScreen(0);

	// start game using the specified level
	g_gameEnvironment.Set( Level );

	// go!
	g_messageDispatcher.SendMessageToAll( "SetupLaunch", NULL, INVALID_OBJECT_HANDLE );
}

//=============================================================================
// ScreenLevelSelect::OnInitialize
//=============================================================================

void ScreenLevelSelect::OnInitialize( void )
{
	ScreenBaseClass::OnInitialize();

	SetHelpRect( ID_LEVELSELECT_HELPRECT );

	m_pListBox = CreateListBox( ID_LEVELSELECT_MENU );

	if( m_pListBox )
	{
		m_pListBox->InitControl( this );

		m_pListBox->InitItem( GetTextSprite( ID_LEVELSELECT_ITEM1 ) );
		m_pListBox->InitItem( GetTextSprite( ID_LEVELSELECT_ITEM2 ) );
		m_pListBox->InitItem( GetTextSprite( ID_LEVELSELECT_ITEM3 ) );
		m_pListBox->InitItem( GetTextSprite( ID_LEVELSELECT_ITEM4 ) );
		m_pListBox->InitItem( GetTextSprite( ID_LEVELSELECT_ITEM5 ) );
		m_pListBox->InitItem( GetTextSprite( ID_LEVELSELECT_ITEM6 ) );
		m_pListBox->InitCursor( GetSprite( ID_LEVELSELECT_CURSOR ) );
	}

	Text* pBigProfile = GetTextSprite( ID_LEVELSELECT_BIGPROFILE );

	if( pBigProfile )
		pBigProfile->SetTextEffect( new BassPumpEffect );

	GuiScrollBar* pScrollBar = CreateScrollBar( ID_LEVELSELECT_SCROLLBAR );

	if( pScrollBar )
	{
		Sprite* pSprite = pScrollBar->SpriteHost();

		pScrollBar->InitPart( GuiScrollBar::Shaft, pSprite );
		pScrollBar->InitPart( GuiScrollBar::ArrowUp, GetSprite( ID_LEVELSELECT_UPARROW, pSprite ) );
		pScrollBar->InitPart( GuiScrollBar::ArrowDown, GetSprite( ID_LEVELSELECT_DOWNARROW, pSprite ) );

		pScrollBar->SetRange( 0.0f, 10.0f );
		pScrollBar->SetPosition( 5.0f );
		pScrollBar->SetSteps( 10 );
	}

	return;
}

//=============================================================================
// ScreenLevelSelect::OnScreenIntro
//
//		When we go to this screen, the first thing we do is check to see if any
//		of the maps have been unlocked.  If not, we immediately skip the screen
//		and go directly into the game, otherwise a list of all the unlocked maps
//		are displayed and the user can choose to play on any of them.
//=============================================================================

// Sorry dan for this ugliness
bool GetLevelUnlocked(int level, bool &weaponMaster)
{
	// can't unlock beatdown (yet)
	weaponMaster = g_gameEnvironment.IsWeaponMaster(level);
	if (g_gameEnvironment.BonusLevel(level))
	{
		for (int l = 0; l < 12; l++)
		{
			if (!g_GlobalProfile.GetCompletedLevel(l))
				return false;
		}
		return true;
	}
	if (Cheats::Enabled(Cheats::UNLOCKLEVELS))
		return true;
	else
		return g_GlobalProfile.GetLevelUnlocked( level );
}

void ScreenLevelSelect::OnScreenIntro( void )
{
	ScreenBaseClass::OnScreenIntro();

	//
	// Populate the list box with unlocked levels...
	//

	GuiListBox* pListBox = GetListBox();

	if( pListBox )
	{
		// start fresh
		pListBox->ClearItems();

		// Add a back option
#ifdef DIRECTX_PC
		pListBox->AddItem("Back", ~0, 0);
#endif

		// populate
		int firstMap = g_gameEnvironment.GetByType( 0, CGameEnv::TYPE_SINGLEPLAYER );
		int map      = firstMap;
		int lastMap  = -1;
		do
		{
			// add if unlocked
			bool weaponMaster = false;
			if ( GetLevelUnlocked( map, weaponMaster ) )
			{
				if (!weaponMaster)
					pListBox->AddItem( g_gameEnvironment.GetWorldDisplayName( map ), map, 0 );
				else
				{
					char levelName[64];
					sprintf(levelName, "%s +", g_gameEnvironment.GetWorldDisplayName( map ));
					pListBox->AddItem( levelName, map, 0 );
				}
				lastMap = map;
			}

			// next map
			map = g_gameEnvironment.GetNextByType( map, CGameEnv::TYPE_SINGLEPLAYER );
		}
		while ( map != firstMap );

		// Add a back option
#ifdef DIRECTX_PC
		pListBox->AddItem("Back", ~0, 0);
#endif

		// Set the initial selection
		pListBox->SelectItem( lastMap );
	}

	Printf( ID_LEVELSELECT_PROFILENAME, g_translator.Translate( "Current Profile: %s" ), g_playerProfile.GetAccountName() );

	OnCursorUpdate();
}

//=============================================================================
// ScreenLevelSelect::OnScreenExit
//=============================================================================

void ScreenLevelSelect::OnScreenExit( void )
{
	ScreenBaseClass::OnScreenExit();
}

//=============================================================================
// ScreenLevelSelect::OnCursorUp
//=============================================================================

void ScreenLevelSelect::OnCursorUp( void )
{
	GuiListBox* pListBox = GetListBox();

	if( pListBox )
	{
		int OldPos = pListBox->CurrentItem();

		pListBox->OnCursorUp();

		int NewPos = pListBox->CurrentItem();

		if( OldPos != NewPos )
		{
			PlaySoundEvent( Screen::SOUNDEVENT_PREVBUTTON );

			OnCursorUpdate();
		}
	}

	return;
}

//=============================================================================
// ScreenLevelSelect::OnCursorDown
//=============================================================================

void ScreenLevelSelect::OnCursorDown( void )
{
	GuiListBox* pListBox = GetListBox();

	if( pListBox )
	{
		int OldPos = pListBox->CurrentItem();

		pListBox->OnCursorDown();

		int NewPos = pListBox->CurrentItem();
#ifndef DIRECTX_PC
		if( OldPos != NewPos )
		{
			PlaySoundEvent( Screen::SOUNDEVENT_NEXTBUTTON );

			OnCursorUpdate();
		}
#endif
	}

	return;
}

//=============================================================================
// ScreenLevelSelect::OnCursorUpdate
//=============================================================================

void ScreenLevelSelect::OnCursorUpdate( void )
{
	GuiListBox* pListBox = GetListBox();

	if( pListBox )
	{
		GuiListBox::Item* pItem = pListBox->GetItem( pListBox->CurrentItem() );

		if( pItem )
		{
			Text* pBigProfile = GetTextSprite( ID_LEVELSELECT_BIGPROFILE );

			if( pBigProfile )
			{
				pBigProfile->SetText( pItem->GetName() );
				pBigProfile->InitAnim();

				SpriteAnim& anim = *pBigProfile->m_anim;

				anim.offset.x.SetFrame( 0.0f, -100.0f );
				anim.offset.x.SetFrame( 0.2f,  -10.0f );
				anim.offset.x.SetFrame( 1.0f,    0.0f );
				anim.offset.x.Deccelerate( true );
				anim.offset.x.Play( false );

				anim.scale.x.SetFrame( 0.0f, 3.5f );
				anim.scale.x.SetFrame( 1.0f, 1.8f );
				anim.scale.x.Deccelerate( true );
				anim.scale.x.Play( false );

				anim.familycolor.a.SetFrame( 0.0f, 0.0f );
				anim.familycolor.a.SetFrame( 0.3f, 1.0f );
				anim.familycolor.a.Deccelerate( true );
				anim.familycolor.a.Play( false );
			}
		}
	}

	return;
}


//=============================================================================
// ScreenLevelSelect::OnButtonOK
//=============================================================================

void ScreenLevelSelect::OnButtonOK( void )
{
	GuiListBox* pListBox = GetListBox();

	if( pListBox )
	{
		int GameEnv = pListBox->CurrentSelection();

		// Check for back
#ifdef DIRECTX_PC
		if (GameEnv == ~0)
		{
			OnButtonCancel();
			return;
		}
#endif

		g_gameEnvironment.Set( GameEnv );
		g_messageDispatcher.SendMessageToAll( "SetupLaunch", NULL, INVALID_OBJECT_HANDLE );
	}

	GotoScreen(0);
}


//=============================================================================
// ScreenLevelSelect::OnPopupShow
//=============================================================================

void ScreenLevelSelect::OnPopupShow( void )
{
	ScreenBaseClass::OnPopupShow();
	ShowScreenExit();
}

//=============================================================================
// ScreenLevelSelect::OnPopupHide
//=============================================================================

void ScreenLevelSelect::OnPopupHide( void )
{
	ScreenBaseClass::OnPopupHide();
	ShowScreenIntro();
}

#endif // DIRECTX_PC
