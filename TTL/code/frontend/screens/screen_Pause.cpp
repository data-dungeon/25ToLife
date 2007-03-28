#include "TTLPCH.h"
#include "frontend/frontend.h"
#include "frontend/screens/screen_Pause.h"
#include "OSI/OSI.hpp"
#include "avatar/sobj.h"
#if defined( _XBOX )
#include "frontend/UseCase/Xbox/UseCaseUIXPlayers.hpp"
#include "frontend/UseCase/Xbox/UseCaseUIXFriends.hpp"
#include "platform/xbox/xboxlive.h"
#elif defined( PS2 ) || defined( DIRECTX_PC )
#include "frontend/UseCase/UseCasePlayers.hpp"
#include "frontend/UseCase/UseCaseFriends.hpp"
#endif

#ifdef DIRECTX_PC
#include "data/screens/pcscreens.h"
#else
#include "data/screens/Pause.h"
#include "data/screens/NetworkGames.h"
#endif

#include "frontend/frontend.h"


bool	g_PauseOnScreen = false;

enum
{
	COMMAND_QUITGAME_YES = 0x100,
	COMMAND_QUITGAME_NO,
};

void ScreenPause::SetPrimaryObjective( int n, const char* pObjective )
{
	Text* pText = GetTextSprite( GetPrimaryId(n) );

	if( pText )
	{
		pText->SetText( pObjective );

		if( !pObjective || 0 == strlen(pObjective) )
			pText->m_state.familycolor.a = 0;
		else
			pText->m_state.familycolor.a = 255;
	}

	return;
}


void ScreenPause::SetSecondaryObjective( int n, const char* pObjective, const char* pProgress )
{
	Text* pText = GetTextSprite( GetSecondaryId(n) );

	if( pText )
	{
		pText->SetText( pObjective );

		if( !pObjective || 0 == strlen(pObjective) )
			pText->m_state.familycolor.a = 0;
		else
			pText->m_state.familycolor.a = 255;
	}

	SetText( GetProgressId(n), pProgress );
}

void ScreenPause::SetPrimaryObjectiveState( int n, OBJECTIVE_STATE State )
{
	SetObjectiveState( GetTextSprite( GetPrimaryId(n) ), State );
}

void ScreenPause::SetSecondaryObjectiveState( int n, OBJECTIVE_STATE State )
{
	SetObjectiveState( GetTextSprite( GetSecondaryId(n) ), State );
}

void ScreenPause::SetObjectiveState( Text* pObjective, OBJECTIVE_STATE State )
{
	if( pObjective )
	{
		bool Checked = false;
		bool CrossedOff = false;

		switch( State )
		{
		case OBJECTIVE_INCOMPLETE:
			break;

		case OBJECTIVE_COMPLETED:
			Checked = true;
			break;

		case OBJECTIVE_FAILED:
			CrossedOff = true;
			break;
		}

		Sprite* pCheck = GetSprite( ID_PAUSE_CHECK, pObjective );

		Sprite* pCross = GetSprite( ID_PAUSE_X, pObjective );
		if( pCheck )
		{
			if( Checked )
				pCheck->m_state.familycolor.a = 255;
			else
				pCheck->m_state.familycolor.a = 0;
		}

		if( pCross )
		{
			if( CrossedOff )
				pCross->m_state.familycolor.a = 255;
			else
				pCross->m_state.familycolor.a = 0;
		}

	}
	return;
}

int ScreenPause::GetPrimaryId( int n )
{
	switch(n)
	{
	case 0:
		return ID_PAUSE_PRIMARY1;
	case 1:
		return ID_PAUSE_PRIMARY2;
	case 2:
		return ID_PAUSE_PRIMARY3;
	}

	return 0;
}

int ScreenPause::GetSecondaryId( int n )
{
	switch(n)
	{
	case 0:
		return ID_PAUSE_SECONDARY1;
	case 1:
		return ID_PAUSE_SECONDARY2;
	case 2:
		return ID_PAUSE_SECONDARY3;
	}

	return 0;
}

int ScreenPause::GetProgressId( int n )
{
	switch(n)
	{
	case 0:
		return ID_PAUSE_PROGRESS1;
	case 1:
		return ID_PAUSE_PROGRESS2;
	case 2:
		return ID_PAUSE_PROGRESS3;
	}

	return 0;
}


void ScreenPause::OnInitialize( void )
{
	g_PauseOnScreen = false;
	ScreenBaseClass::OnInitialize();

	UseHighlighting( true );

#if defined(DIRECTX_PC)
	SetInitialButton( ID_PAUSEPC_CONTINUE );
	SetNextButton( ID_PAUSEPC_OPTIONS );
	SetNextButton( ID_PAUSEPC_QUIT );
	SetCursor( ID_PAUSEPC_CURSOR );
#elif defined(_XBOX)
	SetInitialButton( ID_PAUSE_CONTINUE );
	SetNextButton( ID_PAUSE_OPTIONS );
	SetNextButton( ID_PAUSE_FRIENDS );
	SetNextButton( ID_PAUSE_QUIT );
	SetCursor( ID_PAUSE_CURSOR );
#else
	SetInitialButton( ID_PAUSE_CONTINUE );
	SetNextButton( ID_PAUSE_OPTIONS );
	SetNextButton( ID_PAUSE_QUIT );
	SetCursor( ID_PAUSE_CURSOR );
#endif // DIRECTX_PC

#ifdef DIRECTX_PC
	SetNextScreen( ID_PAUSEPC_OPTIONS, "SCREEN_ID_OPTIONS_PC" );
#else
	SetNextScreen( ID_PAUSE_OPTIONS, "SCREEN_ID_OPTIONS" );
#endif

	SetPrimaryObjective( 0, 0 );
	SetPrimaryObjective( 1, 0 );
	SetPrimaryObjective( 2, 0 );

	SetPrimaryObjectiveState( 0 , OBJECTIVE_INCOMPLETE );
	SetPrimaryObjectiveState( 1 , OBJECTIVE_INCOMPLETE );
	SetPrimaryObjectiveState( 2 , OBJECTIVE_INCOMPLETE );

	SetSecondaryObjective( 0, 0, 0 );
	SetSecondaryObjective( 1, 0, 0 );
	SetSecondaryObjective( 2, 0, 0 );

	SetSecondaryObjectiveState( 0 , OBJECTIVE_INCOMPLETE );
	SetSecondaryObjectiveState( 1 , OBJECTIVE_INCOMPLETE );
	SetSecondaryObjectiveState( 2 , OBJECTIVE_INCOMPLETE );

	return;
}

void ScreenPause::OnScreenIntro( void )
{
	Sprite* pSprite = m_pScreen->SpriteHost();
	char		s[ 255 ];
	char		v[ 20 ];
	bool		done = false;
	int		i , o; 

	g_PauseOnScreen = true;

	if( pSprite )
	{
		pSprite->InitAnim();

		SpriteAnim &anim = *pSprite->m_anim;

		anim.offset.x.SetFrame( 0.000f,-1000.0f );
		anim.offset.x.SetFrame( 0.200f, 0000.0f );
		anim.offset.x.Deccelerate( true );
		anim.offset.x.Play( false );

		anim.familycolor.a.SetFrame( 0.000f, 0.0f );
		anim.familycolor.a.SetFrame( 0.050f, 1.0f );
		anim.familycolor.a.Play( false );
	}
	
	PlaySound( "menu_entry" );

	// Show mouse
#ifdef DIRECTX_PC
	g_MenuCursor.SetHidden(false);
	QuitButton.Show(false);
	BackButton.Show(false);
#endif

	// primary objectives
	g_secondaryObjective.GetFirstPrimaryObjective( o );
	done = false;
	i = 0;
	while( !done )
	{
		g_secondaryObjective.GetObjectiveString( o , s );
		if( g_secondaryObjective.IsObjVisible( o ) )
		{
			SetPrimaryObjective( i , s );
			if( g_secondaryObjective.GetObjState( o ) == O_COMPLETE )
				SetPrimaryObjectiveState( i , OBJECTIVE_COMPLETED );
			else if( g_secondaryObjective.GetObjState( o ) == O_FAIL )
				SetPrimaryObjectiveState( i , OBJECTIVE_FAILED );
			else
				SetPrimaryObjectiveState( i , OBJECTIVE_INCOMPLETE );
		}
		else
		{
			SetPrimaryObjective( i , "" );
			SetPrimaryObjectiveState( i , OBJECTIVE_INCOMPLETE );
		}
		done = g_secondaryObjective.GetNextPrimaryObjective( o );
		i++;
	}

	// bonus objectives
	g_secondaryObjective.GetFirstBonusObjective( o );
	i = 0;
	if( o!=-1 )
	{
		done = false;
		while( !done )
		{
			g_secondaryObjective.GetObjectiveString( o , s );
			g_secondaryObjective.GetObjectiveValue( o , v );
			SetSecondaryObjective( i , s , v );
			if( g_secondaryObjective.GetObjState( o ) == O_COMPLETE )
				SetSecondaryObjectiveState( i , OBJECTIVE_COMPLETED );
			else if( g_secondaryObjective.GetObjState( o ) == O_FAIL )
				SetSecondaryObjectiveState( i , OBJECTIVE_FAILED );
			else
				SetSecondaryObjectiveState( i , OBJECTIVE_INCOMPLETE );

			done = g_secondaryObjective.GetNextBonusObjective( o );
			i++;
		}
	}
	else
	{
		SetSecondaryObjective( i , 0 , 0 );
		i++;
	}

	// stat objectives
	g_secondaryObjective.GetFirstStatObjective( o );
	if( o != -1 )
	{
		done = false;
		while( !done )
		{
			g_secondaryObjective.GetObjectiveString( o , s );
			g_secondaryObjective.GetObjectiveValue( o , v );
			SetSecondaryObjective( i , s , v );
			if( g_secondaryObjective.GetObjState( o ) == O_COMPLETE )
			{
				SetSecondaryObjectiveState( i , OBJECTIVE_COMPLETED );
			}
			if( g_secondaryObjective.GetObjState( o ) == O_FAIL )
			{
				SetSecondaryObjectiveState( i , OBJECTIVE_FAILED );
			}
			done = g_secondaryObjective.GetNextStatObjective( o );
			i++;
		}
	}
	else
	{
		SetSecondaryObjective( i , 0 , 0 );
	}

	SetText( ID_PAUSE_LEVELNAME, g_gameEnvironment.GetWorldDisplayName( g_gameEnvironment.Get() ) );
	
	g_secondaryObjective.GetTimeString( s );
	SetText( ID_PAUSE_TIME, s );

	ScreenHandler::OnScreenIntro();
}

void ScreenPause::OnScreenExit( void )
{
	Sprite* pSprite = m_pScreen->SpriteHost();

	g_PauseOnScreen = false;

	// Hide mouse
#ifdef DIRECTX_PC
	g_MenuCursor.SetHidden(true);
	QuitButton.Show(false);
	BackButton.Show(false); 
#endif

	if( pSprite )
	{
		pSprite->InitAnim();

		SpriteAnim &anim = *pSprite->m_anim;

		anim.offset.x.SetFrame( 0.000f, 0000.0f );
		anim.offset.x.SetFrame( 0.200f, 1000.0f );
		anim.offset.x.Deccelerate( true );
		anim.offset.x.Play( false );

		anim.familycolor.a.SetFrame( 0.000f, 1.0f );
		anim.familycolor.a.SetFrame( 0.200f, 1.0f );
		anim.familycolor.a.SetFrame( 0.201f, 0.0f );
		anim.familycolor.a.Play( false );
	}

	UnselectButton();
}

void ScreenPause::OnScreenUpdate( void )
{
	ScreenBaseClass::OnScreenUpdate();

#ifdef DIRECTX_PC
	g_MenuCursor.Update();
	float x, x1, x2, y, y1, y2;
	x = g_MenuCursor.GetX();
	y = g_MenuCursor.GetY();
	Button* button = GetDefaultButton();
	// functionality for button based menus
	if (!button)
	{
		if (g_MenuCursor.CursorButtonPressed(0))
			OnButtonStart();
	}
	// search through the buttons to see if we're over one
	// if so, check to see if we're clicking the mouse
	while(button) 
	{
		button->SpriteHost()->GetFamilyRect(x1, y1, x2, y2);
		if ( ( x > x1 && x < x2 ) && ( y > y1 && y < y2 ) )
		{
			if (!button->Enabled()) break;
			SelectButton(button);
			if ( g_MenuCursor.CursorButtonPressed(0) )
			{
					button->m_Selected = true;
					g_MenuCursor.ForceButtonUp(0);
					OnButtonOK(); 
			}
			else
			{
					button->m_Selected = false;
			}
			break;
		}
		button = button->GetNextButton();
	}
#endif

#ifdef _XBOX
	bool Enabled = CXboxLive::AreWeLoggedIn();
	EnableButton( ID_PAUSE_FRIENDS, Enabled );
	ShowSprite( ID_PAUSE_LOWCURSOR3, Enabled );
#endif
}

void ScreenPause::OnButtonStart( void )
{
#ifndef DIRECTX_PC
	OnCommand( ID_PAUSE_CONTINUE );
#endif
}

void ScreenPause::OnCommand( int Command )
{
	switch( Command )
	{
#ifdef DIRECTX_PC
	case ID_PAUSEPC_CONTINUE:
		OnCommandContinue();
		break;
	case ID_PAUSEPC_QUIT:
		OnCommandQuit();
		break;
#else
	case ID_PAUSE_CONTINUE:
		OnCommandContinue();
		break;
	case ID_PAUSE_FRIENDS:
		OnCommandFriends();
		break;
	case ID_PAUSE_QUIT:
		OnCommandQuit();
		break;
#endif // DIRECTX_PC
	}
	return;
}

void ScreenPause::OnCommandContinue( void )
{
	GotoScreen(0);

	g_FrontEnd.ReleaseInputFocus();

	PlaySound( "menu_invalid" );

	SPause pauseMessage;
	pauseMessage.d_type = SPause::UNPAUSE;
	pauseMessage.d_menu = NULL;
	g_messageDispatcher.PostMessageToAll("Pause", (void*)&pauseMessage,
			INVALID_OBJECT_HANDLE);
}

void ScreenPause::OnCommandFriends( void )
{
	#ifdef _XBOX
	GotoScreen("SCREEN_ID_UIXFRIENDS");
	#endif
}

void ScreenPause::OnCommandQuit( void )
{
	ScreenPopupDialog* pDialog = GetPopupDialog();

	if( pDialog )
	{
		pDialog->Clear();
		pDialog->SetTitle( "Attention!" );
		pDialog->SetMessage( "Any progress you've made since your last save will be lost.\nWould you like to quit now?" );
		pDialog->AddMenuItem( "Yes", COMMAND_QUITGAME_YES, 0 );
		pDialog->AddMenuItem( "No", COMMAND_QUITGAME_NO, 0 );
		pDialog->SelectMenuItem( COMMAND_QUITGAME_NO );
		ShowPopupDialog();
	}

	return;
}

void ScreenPause::OnDialogCommand( int Command )
{
	switch( Command )
	{
	case COMMAND_QUITGAME_YES:
		g_messageDispatcher.SendMessageToAll("Quit", NULL, INVALID_OBJECT_HANDLE);
		g_audio.dialog.Stop();  
		g_audio.dialog.Clear();
		break;

	case COMMAND_QUITGAME_NO:
		EndScreen(0);
		break;
	}

	return;
}

#ifdef DIRECTX_PC
void ScreenPause::OnButtonPressed( int button )
{
	switch( button )
	{
	case FluffControlMapper::TRIANGLE:
		OnCommandContinue();
		break;
	default:
		ScreenBaseClass::OnButtonPressed(button);
		break;
	}
}
#endif
#if defined(NETWORKING)
//////////////////////////////////////////////////////////////////////////

void ScreenPauseOnline::OnInitialize( void )
{
	ScreenBaseClass::OnInitialize();

	UseHighlighting( true );

	SetInitialButton( ID_STARTMENU_CONTINUE );
#ifdef DIRECTX_PC
	SetNextButton( ID_STARTMENU_OPTIONS );
	SetNextButton( ID_STARTMENU_FRIENDS );
	SetNextButton( ID_STARTMENU_PLAYERS );
	SetNextButton( ID_STARTMENU_VOICE);
	SetNextButton( ID_STARTMENU_QUIT );
	SetNextScreen( ID_STARTMENU_OPTIONS, "SCREEN_ID_OPTIONS_PC" );
#else
	SetNextButton( ID_STARTMENU_FRIENDS );
	SetNextButton( ID_STARTMENU_PLAYERS );
	SetNextButton( ID_STARTMENU_OPTIONS );
	SetNextButton( ID_STARTMENU_VOICE);
	SetNextButton( ID_STARTMENU_QUIT );
	SetNextScreen( ID_STARTMENU_OPTIONS, "SCREEN_ID_OPTIONS" );
#endif
	SetCursor( ID_STARTMENU_CURSOR );
	SetNextScreen( ID_STARTMENU_VOICE, "SCREEN_ID_VOICESETTING" );

	EnableButton( ID_STARTMENU_FRIENDS, true );
	EnableButton( ID_STARTMENU_PLAYERS, true );
}

void ScreenPauseOnline::OnScreenIntro( void )
{
	Sprite*	pSprite = m_pScreen->SpriteHost();

	if( pSprite )
	{
		pSprite->InitAnim();

		SpriteAnim &anim = *pSprite->m_anim;

		anim.offset.x.SetFrame( 0.000f,-1000.0f );
		anim.offset.x.SetFrame( 0.200f, 0000.0f );
		anim.offset.x.Deccelerate( true );
		anim.offset.x.Play( false );

		anim.familycolor.a.SetFrame( 0.000f, 0.0f );
		anim.familycolor.a.SetFrame( 0.050f, 1.0f );
		anim.familycolor.a.Play( false );
	}

	#ifdef DIRECTX_PC
	g_MenuCursor.SetHidden(false);
	QuitButton.Show(false);
	BackButton.Show(false);
	#endif

	PlaySound( "menu_entry" );

	ScreenHandler::OnScreenIntro();
}

void ScreenPauseOnline::OnScreenExit( void )
{
	Sprite* pSprite = m_pScreen->SpriteHost();

	if( pSprite )
	{
		pSprite->InitAnim();

		SpriteAnim &anim = *pSprite->m_anim;

		anim.offset.x.SetFrame( 0.000f, 0000.0f );
		anim.offset.x.SetFrame( 0.200f, 1000.0f );
		anim.offset.x.Deccelerate( true );
		anim.offset.x.Play( false );

		anim.familycolor.a.SetFrame( 0.000f, 1.0f );
		anim.familycolor.a.SetFrame( 0.200f, 1.0f );
		anim.familycolor.a.SetFrame( 0.201f, 0.0f );
		anim.familycolor.a.Play( false );
	}

	#ifdef DIRECTX_PC
	g_MenuCursor.SetHidden(true);
	QuitButton.Show(false);
	BackButton.Show(false);
	#endif

	UnselectButton();
}

void ScreenPauseOnline::OnCommand( int Command )
{
	switch( Command )
	{
	case ID_STARTMENU_CONTINUE:
		OnCommandContinue();
		break;
	case ID_STARTMENU_QUIT:
		OnCommandQuit();
		break;
	case ID_STARTMENU_FRIENDS:
		OnCommandFriends();
		break;
	case ID_STARTMENU_PLAYERS:
		OnCommandPlayers();
		break;
	}

	return;
}

void ScreenPauseOnline::OnButtonStart( void )
{
#ifdef DIRECTX_PC
	OnCommand( ID_PAUSEPC_CONTINUE );
#else
	OnCommand( ID_STARTMENU_CONTINUE );
#endif
}

void ScreenPauseOnline::OnButtonPressed( int Button )
{
	switch( Button )
	{
	case FluffControlMapper::SQUARE:
		GotoScreen( "SCREEN_ID_CHATINGAME" );
		break;
#ifdef DIRECTX_PC
	case FluffControlMapper::TRIANGLE:
		OnCommandContinue();
#endif
	default:
		ScreenBaseClass::OnButtonPressed( Button );
		break;
	}

	return;
}

void ScreenPauseOnline::OnCommandContinue( void )
{
	GotoScreen(0);
}

void ScreenPauseOnline::OnCommandFriends( void )
{
	switch( g_FrontEnd.GetGameType() )
	{
	case FrontEnd::GAMETYPE_NETWORKGAME:
		GotoScreen( "SCREEN_ID_UIXFRIENDS" );
		break;

	case FrontEnd::GAMETYPE_ONLINEGAME:
	#if defined( _XBOX )
		OnlineUI::UIXFriends::getSingletonPtr()->setAllowGameInvites( true );
		OnlineUI::UIXFriends::getSingletonPtr()->start();
	#elif defined( PS2 ) || defined( DIRECTX_PC )
		OnlineUI::Friends::getSingletonPtr()->start();
		//OnlineUI::Friends::getSingletonPtr()->setDisplayPopup( true ); // display popup of options
	#endif
		break;
	}
}

void ScreenPauseOnline::OnCommandPlayers( void )
{
#if defined( _XBOX )
	OnlineUI::UIXPlayers::getSingletonPtr()->setShowCurrentPlayers( true );
	OnlineUI::UIXPlayers::getSingletonPtr()->start();
#elif defined( PS2 ) || defined( DIRECTX_PC )
	OnlineUI::Players::getSingletonPtr()->start(true);
	//OnlineUI::Players::getSingletonPtr()->setDisplayPopup( true ); // display popup of options
#endif // !_XBOX
}

#ifdef DIRECTX_PC
void ScreenPauseOnline::OnButtonCancel()
{
	OnScreenExit();
	GotoScreen(0);
}
#endif

void ScreenPauseOnline::OnCommandQuit( void )
{
	ScreenPopupDialog* pDialog = GetPopupDialog();

	if( pDialog )
	{
		pDialog->Clear();
		pDialog->SetTitle(0);
		pDialog->SetMessage( "Do you want to Quit?" );
		pDialog->AddMenuItem( "Yes", COMMAND_QUITGAME_YES, 0 );
		pDialog->AddMenuItem( "No", COMMAND_QUITGAME_NO, 0 );
		pDialog->SelectMenuItem( COMMAND_QUITGAME_NO );
		ShowPopupDialog();
	}

	return;
}

void ScreenPauseOnline::OnDialogCommand( int Command )
{
	switch( Command )
	{
	case COMMAND_QUITGAME_YES:
		g_messageDispatcher.SendMessageToAll("Quit", NULL, INVALID_OBJECT_HANDLE);
		g_audio.dialog.Stop();  
		g_audio.dialog.Clear();
		break;

	case COMMAND_QUITGAME_NO:
		EndScreen(0);
		break;
	}

	return;
}
#endif
void ScreenQuitGame::OnInitialize( void )
{
	g_DisableControllerRemoved = false;

	ScreenBaseClass::OnInitialize();

	m_TimeToQuit = 0;
}

void ScreenQuitGame::OnScreenIntro( void )
{
	g_DisableControllerRemoved = true;

	ScreenBaseClass::OnScreenIntro();

	ScreenManager* pScreenManager = GetScreenManager();

	if( pScreenManager )
		pScreenManager->ClearScreenHistory();

	m_TimeToQuit = g_timer.GetOSTime() + 2500;
}

void ScreenQuitGame::OnScreenExit( void )
{
	g_DisableControllerRemoved = false;
	m_TimeToQuit = 0;
}

bool ScreenQuitGame::IsScreenReadyToExit( void )
{
	return ( g_timer.GetOSTime() >= m_TimeToQuit );
}
