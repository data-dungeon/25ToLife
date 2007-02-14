#include "TTLPCH.h"
#include "platform/MemoryCard/MemoryCard.h"
#include "frontend/savegame.h"
#include "frontend/screens/screen_MySettings.h"
#include "data/screens/OnlineScreens.h"
#include "main/PlayerDB.h"
#include "GameHelper/Translator.h"

void ScreenMySettings::SetDescription( const char* pDescription )
{
	SetText( ID_MYSETTINGS_DESCRIPTION, pDescription );
}

//=============================================================================
// ScreenMySettings::OnInitialize
//=============================================================================

void ScreenMySettings::OnInitialize( void )
{
	ScreenBaseClass::OnInitialize();

	UseHighlighting( true );
	
	SetHelpRect( ID_MYSETTINGS_HELPRECT );
	SetInitialButton( ID_MYSETTINGS_MYCHARACTER );
	SetNextButton( ID_MYSETTINGS_MYTAG );
	SetNextButton( ID_MYSETTINGS_VOICESETTINGS );
	SetNextButton( ID_MYSETTINGS_GAMESETTINGS );

	SetNextScreen( ID_MYSETTINGS_MYCHARACTER, "SCREEN_ID_CHARSELECT2" );
	SetNextScreen( ID_MYSETTINGS_MYTAG, "SCREEN_ID_TAGEDITOR" );
	SetNextScreen( ID_MYSETTINGS_VOICESETTINGS, "SCREEN_ID_VOICESETTING" );
#ifdef DIRECTX_PC
	SetNextScreen( ID_MYSETTINGS_GAMESETTINGS, "SCREEN_ID_OPTIONS_PC" );   //
#else
	SetNextScreen( ID_MYSETTINGS_GAMESETTINGS, "SCREEN_ID_OPTIONS" );
#endif

	SetCursor( ID_MYSETTINGS_CURSOR );

	m_MemCardState = MYSETTINGS_IDLE;

	// Hide PS2 help bar on PC
#ifdef DIRECTX_PC
	GetSprite(ID_MYSETTINGS_HELPRECT)->HideAll();
#endif
}

void ScreenMySettings::OnScreenIntro( void )
{
	ScreenBaseClass::OnScreenIntro();

	Printf( ID_MYSETTINGS_PROFILENAME, g_translator.Translate( "Current Profile: %s" ), g_playerProfile.GetAccountName() );
}

void ScreenMySettings::OnButtonSelect( Button* pButton, Sprite* pSprite )
{
	ScreenBaseClass::OnButtonSelect( pButton, pSprite );

	const char* pDescription = 0;

	if( pButton )
	{
		switch( pButton->GetButtonId() )
		{
		case ID_MYSETTINGS_MYCHARACTER:
			pDescription = "Customize your character.";
			break;

		case ID_MYSETTINGS_MYTAG:
			pDescription = "Create a personalized tag.";
			break;

		case ID_MYSETTINGS_VOICESETTINGS:
			pDescription = "Customize your voice parameters.";
			break;

		case ID_MYSETTINGS_GAMESETTINGS:
			pDescription = "Change your game options, button configuration and analog stick settings.";
			break;
		}
	}

	SetDescription( pDescription );
}

void ScreenMySettings::OnButtonCancel( void )
{
	SaveProfile();
}

void ScreenMySettings::OnSaveGameResult( int Result )
{
	g_MemCardManager->ResetLoadSaveBuffer();
	EndScreen(0);
	g_messageDispatcher.SendMessageToAll( "ExitSettings", 0, INVALID_OBJECT_HANDLE );
}

void ScreenMySettings::OnCommand( int Command )
{
	switch( Command ) 
	{
	case ID_MYSETTINGS_MYCHARACTER:
		g_messageDispatcher.SendMessageToAll( "StartCustomPlayer", 0, INVALID_OBJECT_HANDLE );
		break;
	}

	return;
}