#include "TTLPCH.h"
#include "OSI/OSI.hpp"
#include "frontend/inputdriver.h"
#include "screen_clan_main.h"
#include "data/screens/Multiplayer.h"
#include "frontend/frontend.h"
#include "GameHelper/Translator.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when the screen is first loaded
///////////////////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ void ScreenClanMain::OnInitialize(void)
{
    UseHighlighting(true);

	SetHelpRect(ID_CLAN_HELPRECT);
	SetInitialButton(ID_CLAN_FEEDBACK);
	SetNextButton(ID_CLAN_MEMBER_LIST);
	SetNextButton(ID_CLAN_CREATE);
	SetNextButton(ID_CLAN_INVITES);
	SetNextButton(ID_CLAN_LEAVE);
	SetNextButton(ID_CLAN_VIEW_NOTICES);
	SetNextButton(ID_CLAN_POST_NOTICE);
	SetNextButton(ID_CLAN_BACK);
	SetCursor(ID_CLAN_CURSOR);

#ifndef _XBOX
	EnableButton(ID_CLAN_FEEDBACK, false);
	GetSprite(ID_CLAN_FEEDBACK)->HideAll();
	GetSprite(ID_CLAN_LOWCURSOR1)->HideAll();
#endif

	SelectInitialButton();

	ClearScreen();

		// Hide PS2 help bar on PC
//#ifdef DIRECTX_PC
//	GetSprite(ID_MAINMENU_HELPGROUP)->HideAll();
//#endif
	
	ScreenBaseClass::OnInitialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every time when the screen is displayed
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenClanMain::OnScreenIntro( void )
{
    _status = RUNNING;

    ScreenBaseClass::OnScreenIntro();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when a popup window closes (on this screen, the virtual keyboard)
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenClanMain::OnKeyboardInput( const char* keyboardStr )
{
	if( keyboardStr == NULL || keyboardStr[0] == 0 ) return;

    // Save off our clan post text for sending off to OSI
	OSIstrncpy(_clanNotice, GetKeyboardString(), OSI_STRING_LENGTH_LONG);
	_status = NOTICE_POSTED;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when user selects a button
///
/// \param Command ID of the selected button
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenClanMain::OnCommand( int Command )
{
    // Set the status flag
    switch (Command)
    {
#ifdef _XBOX
		case ID_CLAN_FEEDBACK:
			_status = FEEDBACK;
			break;
#endif

        case ID_CLAN_MEMBER_LIST:
            _status = MEMBERS;
            break;

        case ID_CLAN_CREATE:
            _status = CREATE;
            break;

        case ID_CLAN_INVITES:
            _status = INVITATIONS;
            break;

        case ID_CLAN_LEAVE:
            _status = LEAVE;
            break;

		case ID_CLAN_VIEW_NOTICES:
            _status = VIEW_NOTICES;
            break;

        case ID_CLAN_POST_NOTICE:
            _status = POST_NOTICE;
            break;

        case ID_CLAN_BACK:
            _status = BACK;
            break;
    }

    ScreenBaseClass::OnCommand(Command);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called to set the notice text on screen
///
/// \param noticeList List of the notices to display
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenClanMain::SetNotices(OSIClanNoticeList *noticeList)
{

	OSIchar buffer[ 2056 ];
	buffer[0] = 0;

	if( noticeList != NULL)
	{
		noticeList->begin();
		OSIClanNotice* notice = noticeList->getCurrent();
		// Only display first notice on this screen
		if( m_pScreen && notice )
		{
			OSIsnprintf( buffer, NUM_ARRAY_ELEMENTS(buffer), "%s: %s", g_translator.Translate( "From" ), notice->posterName );
			m_pScreen->GetTextSprite( ID_CLAN_NOTICE_NAME )->SetTextNoTranslate( buffer );
			m_pScreen->GetTextSprite( ID_CLAN_NOTICE_BODY )->SetTextNoTranslate( notice->noticeText );
			return;
		}
	}

	m_pScreen->GetTextSprite( ID_CLAN_NOTICE_NAME )->SetText( "" );
	m_pScreen->GetTextSprite( ID_CLAN_NOTICE_BODY )->SetText( "" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called to set the access level of the menu based on users clan role.
///
/// \param Clan role of the user of the menu.
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenClanMain::SetAccessLevel(const OSIClanRole &clanRole)
{
#ifdef _XBOX
	OSIPlayerList& players = OSI::getSingletonPtr()->getRecentPlayerList();
	bool isPlayerListEmpty = (0 == players.count());
	m_pScreen->EnableButton( ID_CLAN_FEEDBACK, !isPlayerListEmpty );
#endif

	switch(clanRole)
	{
		case OSI_CLAN_ROLE_UNKNOWN:
			// User not in clan, can create a clan and view invitations
			m_pScreen->EnableButton(ID_CLAN_MEMBER_LIST, false);
			m_pScreen->EnableButton(ID_CLAN_CREATE, true);
			m_pScreen->EnableButton(ID_CLAN_INVITES, true);
			m_pScreen->EnableButton(ID_CLAN_LEAVE, false);
			m_pScreen->EnableButton(ID_CLAN_VIEW_NOTICES, false);
			m_pScreen->EnableButton(ID_CLAN_POST_NOTICE, false);
			break;
		case OSI_CLAN_ROLE_LEADER:
		case OSI_CLAN_ROLE_OFFICER:
			// Clan members can't create a clan, but officers and leaders can post a notice
			m_pScreen->EnableButton(ID_CLAN_MEMBER_LIST, true);
			m_pScreen->EnableButton(ID_CLAN_CREATE, false);
			m_pScreen->EnableButton(ID_CLAN_INVITES, true);
			m_pScreen->EnableButton(ID_CLAN_LEAVE, true);
			m_pScreen->EnableButton(ID_CLAN_VIEW_NOTICES, true);
			m_pScreen->EnableButton(ID_CLAN_POST_NOTICE, true);
			break;
		case OSI_CLAN_ROLE_SOLDIER:
			// Soldiers can't post notices but can do everything else leaders and officers can
			m_pScreen->EnableButton(ID_CLAN_MEMBER_LIST, true);
			m_pScreen->EnableButton(ID_CLAN_CREATE, false);
			m_pScreen->EnableButton(ID_CLAN_INVITES, true);
			m_pScreen->EnableButton(ID_CLAN_LEAVE, true);
			m_pScreen->EnableButton(ID_CLAN_VIEW_NOTICES, true);
			m_pScreen->EnableButton(ID_CLAN_POST_NOTICE, false);
			break;
		default:
			ASSERTS(0, "Invalid clan role when setting clan main menu access level.");
			break;
	}
	m_pScreen->SelectInitialButton();
}

void ScreenClanMain::ClearScreen()
{
	m_pScreen->GetTextSprite( ID_CLAN_NAME_TEXT )->SetText( "" );
	m_pScreen->GetTextSprite( ID_CLAN_LEADER_TEXT )->SetText( "" );
	m_pScreen->GetTextSprite( ID_CLAN_MEMBER_COUNT_TEXT )->SetText( "" );
	m_pScreen->GetTextSprite( ID_CLAN_ABBR_TEXT )->SetText( "" );
	m_pScreen->GetTextSprite( ID_CLAN_ROLE_TEXT )->SetText( "" );
	m_pScreen->GetTextSprite( ID_CLAN_RANK_TEXT)->SetTextNoTranslate( "" );

	m_pScreen->GetTextSprite( ID_CLAN_NOTICE_NAME )->SetText( "" );
	m_pScreen->GetTextSprite( ID_CLAN_NOTICE_BODY )->SetText( "" );

#ifdef _XBOX
	m_pScreen->EnableButton(ID_CLAN_FEEDBACK, false );
#endif

	m_pScreen->EnableButton(ID_CLAN_MEMBER_LIST, false);
	m_pScreen->EnableButton(ID_CLAN_CREATE, false);
	m_pScreen->EnableButton(ID_CLAN_INVITES, false);
	m_pScreen->EnableButton(ID_CLAN_LEAVE, false);
	m_pScreen->EnableButton(ID_CLAN_VIEW_NOTICES, false);
	m_pScreen->EnableButton(ID_CLAN_POST_NOTICE, false);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenClanMain::OnDialogCommand( int Command )
///////////////////////////////////////////////////////////////////////////////////////////////////
{
	switch( Command )
	{
	case COMMAND_DIALOG_OK:
		g_FrontEnd.EndPopup( 0 );
	}
}

