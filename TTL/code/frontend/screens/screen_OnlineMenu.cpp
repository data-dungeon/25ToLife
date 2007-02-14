#include "TTLPCH.h"
#include "GameHelper/Translator.h"
#include "frontend/screens/screen_Popups.h"
#include "frontend/screens/screen_OnlineMenu.h"
#include "data/screens/OnlineScreens.h"

//=============================================================================
// ScreenOnlineMenu::ScreenOnlineMenu - The default constructor
//=============================================================================

ScreenOnlineMenu::ScreenOnlineMenu( void ) :
	d_messageOwner( "ScreenOnlineMenu" )
{
}

//=============================================================================
// ScreenOnlineMenu::~ScreenOnlineMenu - The default destructor
//=============================================================================

ScreenOnlineMenu::~ScreenOnlineMenu( void )
{
}

//=============================================================================
// ScreenOnlineMenu::SetDescription
//=============================================================================

void ScreenOnlineMenu::SetDescription( const char *pDescription )
{
	SetText( ID_ONLINEMENU_DESCRIPTION, pDescription );
}

//=============================================================================
// ScreenOnlineMenu::OnInitialize
//=============================================================================

void ScreenOnlineMenu::OnInitialize( void )
{
	ScreenBaseClass::OnInitialize();

	UseHighlighting( true );

	SetHelpRect( ID_ONLINEMENU_HELPRECT );
	SetInitialButton( ID_ONLINEMENU_JOINSESSION );
	SetNextButton( ID_ONLINEMENU_CREATESESSION );
	SetNextButton( ID_ONLINEMENU_MYSETTINGS );
	SetCursor( ID_ONLINEMENU_CURSOR );

	SetNextScreen( ID_ONLINEMENU_JOINSESSION,   "SCREEN_ID_SEARCHRESULT" );
	SetNextScreen( ID_ONLINEMENU_CREATESESSION, "SCREEN_ID_CREATESESSION" );

	m_MemCardState = ONLINE_IDLE;

	// Hide PS2 help bar on PC
#ifdef DIRECTX_PC
	GetSprite(ID_ONLINEMENU_HELPRECT)->HideAll();
#endif
}

//=============================================================================
// ScreenOnlineMenu::OnCommand
//=============================================================================

void ScreenOnlineMenu::OnCommand( int Command )
{
	switch( Command )
	{
	case ID_ONLINEMENU_JOINSESSION:
		OnCommandJoinSession();
		break;
	case ID_ONLINEMENU_MYSETTINGS:
		OnCommandMySettings();
		break;
	}

	return;
}

//=============================================================================
// ScreenOnlineMenu::OnButtonCancel
//=============================================================================

void ScreenOnlineMenu::OnButtonCancel( void )
{
	ScreenBaseClass::OnButtonCancel();
	g_messageDispatcher.SendMessageToAll( "SetupBackup", NULL, INVALID_OBJECT_HANDLE );
}

//=============================================================================
// ScreenOnlineMenu::OnButtonSelect
//=============================================================================

void ScreenOnlineMenu::OnButtonSelect( Button* pButton, Sprite* pSprite )
{
	ScreenBaseClass::OnButtonSelect( pButton, pSprite );

	if( pButton )
	{
		switch( pButton->GetButtonId() )
		{
		case ID_ONLINEMENU_JOINSESSION:
			SetDescription( "Join your homies in a network game." );
			break;

		case ID_ONLINEMENU_CREATESESSION:
			SetDescription( "Host a match to throw down with other players." );
			break;

		case ID_ONLINEMENU_MYSETTINGS:
			SetDescription( "Customize your character and game settings for the current profile." );
			break;

		default:
			SetDescription(0);
			break;
		}
	}

	return;
}

//=============================================================================
// ScreenOnlineMenu::OnScreenIntro
//=============================================================================

void ScreenOnlineMenu::OnScreenIntro( void )
{
	ScreenManager* pScreenManager = GetScreenManager();

	if( pScreenManager )
		pScreenManager->ClearScreenHistory() ;

	SelectInitialButton();

	Printf( ID_ONLINEMENU_CURRENTPROFILE, g_translator.Translate( "Current Profile: %s" ), g_playerProfile.GetAccountName() );
	
	ScreenBaseClass::OnScreenIntro();
}

//=============================================================================
// ScreenOnlineMenu::OnCommandJoinSession
//
//		When the user selects "Optimatch" from the OnlineMenu
//=============================================================================

void ScreenOnlineMenu::OnCommandJoinSession( void )
{
}

//=============================================================================
// ScreenOnlineMenu::OnCommandMySettings
//
//		When the user selects "My Settings" from the OnlineMenu
//=============================================================================

void ScreenOnlineMenu::OnCommandMySettings( void )
{
	g_messageDispatcher.SendMessageToAll( "SetupSettings", 0, INVALID_OBJECT_HANDLE );
}



