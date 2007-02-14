#include "TTLPCH.h"
#include "frontend/frontend.h"
#include "frontend/inputdriver.h"
#include "screen_MultiplayerMenu.h"
#include "data/screens/Multiplayer.h"
#include "GameHelper/Translator.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when the screen is first loaded
///////////////////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ void ScreenMultiplayerMenu::OnInitialize(void)
{
	UseHighlighting(true);

	SetHelpRect(ID_MULTIPLAYER_HELPRECT);
	SetInitialButton(ID_MULTIPLAYER_QUICKMATCH);
	SetNextButton(ID_MULTIPLAYER_FINDMATCH);
	SetNextButton(ID_MULTIPLAYER_HOSTMATCH);
	SetNextButton(ID_MULTIPLAYER_SETTINGS);
	SetNextButton(ID_MULTIPLAYER_FRIENDS);
	SetNextButton(ID_MULTIPLAYER_CLAN);
	SetNextButton(ID_MULITPLAYER_STATS);
	SetNextButton(ID_MULTIPLAYER_PLAYERLIST);

#ifdef _XBOX
	SetNextButton(ID_MULTIPLAYER_XBOX_STATUS);
#else
	GetTextSprite(ID_MULTIPLAYER_XBOX_STATUS)->Hide();
	GetSprite(ID_MULTIPLAYER_LOWCURSORX)->Hide();
#endif

	SetNextButton(ID_MULTIPLAYER_LOGOUT);
	SetCursor(ID_MULTIPLAYER_CURSOR);

	SelectButton(ID_MULTIPLAYER_QUICKMATCH);

	// On XBox, Find Match is called Optimatch and Quickmatch doesn't have a space
#ifdef _XBOX
	// Dan Silver - Testers are saying "Quickmatch" has a space and the M is capitalized. So I changed it.
	// They also said Optimatch has a capital M so I changed that too.
	// See bug 7778.
	SetText( ID_MULTIPLAYER_FINDMATCH, "OptiMatch" );
	SetText( ID_MULTIPLAYER_QUICKMATCH, "Quick Match" );
	SetText( ID_MULTIPLAYER_TITLE, "Xbox Live" );
	SetText( ID_MULTIPLAYER_LOGOUT, "Sign Out" );
	SetText( ID_MULTIPLAYER_HELPTEXT3, "Sign Out" );
#endif

	EnableButton( ID_MULTIPLAYER_FRIENDS, true );
	EnableButton( ID_MULTIPLAYER_PLAYERLIST, true );
	EnableButton( ID_MULTIPLAYER_SETTINGS, true );
	EnableButton( ID_MULTIPLAYER_CLAN, true );

	SetMessageOfTheDay( "" );

	// Hide PS2 help bar on PC
#ifdef DIRECTX_PC
	GetSprite(ID_MULTIPLAYER_HELPRECT)->HideAll();
#endif

	// Disable stuff not on this milestone for PC
//#ifdef DIRECTX_PC
//	EnableButton(ID_MULTIPLAYER_SETTINGS, false);
//	EnableButton(ID_MULTIPLAYER_CLAN, false);
//	EnableButton(ID_MULITPLAYER_STATS, false);
//#endif

	ScreenBaseClass::OnInitialize();
}

void ScreenMultiplayerMenu::OnScreenIntro( void )
{
    _status = RUNNING;

    ScreenBaseClass::OnScreenIntro();
#ifdef DIRECTX_PC
	g_MenuCursor.SetHidden(false);
#endif
}


void ScreenMultiplayerMenu::OnCommand( int Command )
{
    // Set the status flag

    switch (Command)
    {
        case ID_MULTIPLAYER_QUICKMATCH:
            _status = QUICKMATCH;
            break;

        case ID_MULTIPLAYER_FINDMATCH:
            _status = FINDMATCH;
            break;

        case ID_MULTIPLAYER_HOSTMATCH:
            _status = HOSTMATCH;
            break;

        case ID_MULTIPLAYER_FRIENDS:
            _status = FRIENDS;
            break;

        case ID_MULTIPLAYER_PLAYERLIST:
            _status = PLAYERLIST;
            break;

        case ID_MULITPLAYER_STATS:
            _status = STATS;
            break;

        case ID_MULTIPLAYER_SETTINGS:
            _status = SETTINGS;
            break;

        case ID_MULTIPLAYER_CLAN:
            _status = CLAN;
            break;

        case ID_MULTIPLAYER_XBOX_STATUS:
            _status = ONLINE_STATUS;
            break;

        case ID_MULTIPLAYER_LOGOUT:
            _status = LOGOUT;
            break;
    }

    ScreenBaseClass::OnCommand(Command);
}

void ScreenMultiplayerMenu::SetOnlineStatus(bool status)
{
	if (status)
		SetText(ID_MULTIPLAYER_XBOX_STATUS, "Appear Offline" );
	else
		SetText(ID_MULTIPLAYER_XBOX_STATUS, "Appear Online" );

	return;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Set the online name field on the screen
///
/// \param pName The name to set in the field
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenMultiplayerMenu::SetOnlineName(const char *pName)
{
    Screen* pScreen = m_pScreen;
    char name[256];

	if(pScreen)
	{
#ifndef _XBOX
		_snprintf( name, 256, "%s: %s", g_translator.Translate( "Online name" ), pName );
#else
		sprintf( name, "%s: %s", g_translator.Translate( "Online name" ), pName );
#endif
        name[ 255 ] = 0;
        pScreen->GetTextSprite(ID_MULTIPLAYER_PROFILENAME)->SetTextNoTranslate( name );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Set the description for the currently selected item on the screen
///
/// \param pDescription The description to set in the field
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenMultiplayerMenu::SetDescription(const char *pDescription)
{
    Screen* pScreen = m_pScreen;

	if(pScreen)
	{
        pScreen->GetTextSprite(ID_MULTIPLAYER_DESCRIPTION)->SetText(pDescription);
    }
}

void ScreenMultiplayerMenu::SetMessageOfTheDay(const char* message)
{
	if( message == NULL || message[0] == 0 )
	{
		m_pScreen->GetTextSprite(ID_MULTIPLAYER_MOTD_TITLE)->SetText( "" );
		m_pScreen->GetTextSprite(ID_MULTIPLAYER_MOTD)->SetText( "" );
		return;
	}

	m_pScreen->GetTextSprite(ID_MULTIPLAYER_MOTD_TITLE)->SetText( "Message of the Day" );
	m_pScreen->GetTextSprite(ID_MULTIPLAYER_MOTD)->SetText( message );
}







