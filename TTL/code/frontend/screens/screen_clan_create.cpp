#include "TTLPCH.h"
#include "OSI/OSI.hpp"
#include "frontend/inputdriver.h"
#include "screen_clan_create.h"
#include "data/screens/Multiplayer.h"
#include "frontend/frontend.h"
#include "../UseCase/UseCaseUtility.hpp"

void ScreenClanCreate::ClearName()
{
	_clanName[0] = 0;
	SetText( ID_CLAN_CREATE_NAME_TEXT, "" );
}

void ScreenClanCreate::ClearTag()
{
	_clanTag[0] = 0;
	SetText( ID_CLAN_CREATE_ABBREVIATION_TEXT, "" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when the screen is first loaded
///////////////////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ void ScreenClanCreate::OnInitialize(void)
{
	UseHighlighting(true);

	SetHelpRect(ID_CLAN_CREATE_HELPGROUP);
	SetInitialButton(ID_CLAN_CREATE_NAME);
	SetNextButton(ID_CLAN_CREATE_ABBREVIATION);
	SetNextButton(ID_CLAN_CREATE_DONE);
	SelectButton(ID_CLAN_CREATE_NAME);
	SetCursor(ID_CLAN_CREATE_CURSOR);

		// Hide PS2 help bar on PC
	#ifdef DIRECTX_PC
	GetSprite(ID_CLAN_CREATE_HELPGROUP)->HideAll();
	#endif

	ScreenBaseClass::OnInitialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every time when the screen is displayed
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenClanCreate::OnScreenIntro( void )
{
	if(m_pScreen)
	{
		m_pScreen->GetTextSprite( ID_CLAN_CREATE_NAME_TEXT )->SetText( "" );
		m_pScreen->GetTextSprite( ID_CLAN_CREATE_ABBREVIATION_TEXT )->SetText( "" );
	}

	memset(_clanName, 0, sizeof(_clanName));
	memset(_clanTag, 0, sizeof(_clanTag));

    _status = RUNNING;

    ScreenBaseClass::OnScreenIntro();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when a popup window closes (on this screen, the virtual keyboard)
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenClanCreate::OnKeyboardInput( const char* keyboardEntry )
{
	size_t keyboardEntryLength = OSIstrlen(keyboardEntry);

    // Fill in the correct text field and value for the value returned by the keyboard.
    switch( _activeKBField )
    {
        case CLAN_CREATE_NAME:
			if(keyboardEntryLength < OnlineUI::MIN_ENTRY_LENGTH ||
				keyboardEntryLength > OnlineUI::MAX_ENTRY_LENGTH)
			{
				_status = INVALID_NAME_LENGTH;
				_clanName[0] = 0;
			}
			else
			{
				OSIstrncpy( _clanName, keyboardEntry, NUM_ARRAY_ELEMENTS(_clanName) );
			}
            m_pScreen->GetTextSprite( ID_CLAN_CREATE_NAME_TEXT )->SetTextNoTranslate( _clanName );
            break;

		case CLAN_CREATE_TAG:
			if(keyboardEntryLength >= OSIClanInfo::MAX_TAG_LENGTH ||
				keyboardEntryLength <= 0)
			{
				_status = INVALID_TAG_LENGTH;
				_clanTag[0] = 0;
			}
			else
			{
				OSIstrncpy( _clanTag, keyboardEntry, NUM_ARRAY_ELEMENTS(_clanTag) );
			}
            m_pScreen->GetTextSprite( ID_CLAN_CREATE_ABBREVIATION_TEXT )->SetTextNoTranslate( _clanTag );
            break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when user selects a button
///
/// \param Command ID of the selected button
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenClanCreate::OnCommand( int Command )
{
    // Set the status flag
    switch (Command)
    {
        case ID_CLAN_CREATE_NAME:
            _status = ENTER_NAME;
			_activeKBField = CLAN_CREATE_NAME;
            break;

        case ID_CLAN_CREATE_ABBREVIATION:
            _status = ENTER_TAG;
			_activeKBField = CLAN_CREATE_TAG;
            break;

        case ID_CLAN_CREATE_DONE:
            _status = DONE;
            break;
    }

    ScreenBaseClass::OnCommand(Command);
}
