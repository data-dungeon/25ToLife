#include "TTLPCH.h"
#include "frontend/inputdriver.h"
#include "screen_friends.h"
#include "data/screens/FriendsAndPlayers.h"
#include "frontend/frontend.h"
#include "OSI/OSIMatchList.hpp"
#include "OSI/OSI.hpp"
#include "GameHelper/Translator.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when the screen is first loaded
///////////////////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ void ScreenFriends::OnInitialize(void)
{
	UseHighlighting(true);

	SetHelpRect(ID_FRIENDS_HELPRECT);
	SetInitialButton(ID_FRIENDS_CONFIRMED);
	SetNextButton(ID_FRIENDS_INCOMING);
	SetNextButton(ID_FRIENDS_OUTGOING);
	SetNextButton(ID_FRIENDS_SEND_REQ);
	SetNextButton(ID_FRIENDS_RECENT);
	SetCursor(ID_FRIENDS_CURSOR);

	SelectInitialButton();

	OnCursorUpdate();

	// Hide PS2 help bar on PC
#ifdef DIRECTX_PC
	GetSprite(ID_FRIENDS_HELPRECT)->HideAll();
#endif

	ScreenBaseClass::OnInitialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every time when the screen is displayed
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenFriends::OnScreenIntro(void)
{
    _status = RUNNING;

    ScreenBaseClass::OnScreenIntro();

	OnCursorUpdate();

	char buff[ 256 ];
	OSIsnprintf( buff, 256, "%s: %s", g_translator.Translate( "Online name" ), OSI::getSingletonPtr()->getActiveAccount()->player.name );
	buff[ 255 ] = 0;
	GetTextSprite( ID_FRIENDS_PROFILENAME )->SetTextNoTranslate( buff );
#ifdef DIRECTX_PC
	BackButton.Show(true);
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when user selects a button
///
/// \param Command ID of the selected button
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenFriends::OnCommand(int Command)
{
    // Set the status flag
    switch (Command)
    {
        case ID_FRIENDS_CONFIRMED:
            _status = VIEW_CONFIRMED_FRIENDS;
            break;

		case ID_FRIENDS_INCOMING:
            _status = VIEW_INCOMING_FRIEND_REQUESTS;
            break;

		case ID_FRIENDS_OUTGOING:
            _status = VIEW_OUTGOING_FRIEND_REQUESTS;
            break;

		case ID_FRIENDS_SEND_REQ:
            _status = SEND_FRIEND_REQUEST_BY_NAME;
            break;

        case ID_FRIENDS_RECENT:
            _status = RECENT_PLAYERS_LIST;
            break;
    }

    ScreenBaseClass::OnCommand(Command);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when up is pressed: move the selection
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenFriends::OnCursorUp()
{
	ScreenBaseClass::OnCursorUp();

	OnCursorUpdate();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when down is pressed: move the selection
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenFriends::OnCursorDown()
{
	ScreenBaseClass::OnCursorDown();

	OnCursorUpdate();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Update the cursor position
/////////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenFriends::OnCursorUpdate(void)
{
    switch( m_pScreen->GetCurrentButtonId() )
	{
		case ID_FRIENDS_CONFIRMED:
            m_pScreen->GetTextSprite( ID_FRIENDS_DESCRIPTION )->SetText(
				"View a list of all your current friends."
			);
            break;

		case ID_FRIENDS_INCOMING:
            m_pScreen->GetTextSprite( ID_FRIENDS_DESCRIPTION )->SetText(
				"View a list of players who have invited you to be their friend."
			);
            break;

		case ID_FRIENDS_OUTGOING:
            m_pScreen->GetTextSprite( ID_FRIENDS_DESCRIPTION )->SetText(
				"View a list of players who you have sent a friend request to."
			);
            break;

		case ID_FRIENDS_SEND_REQ:
            m_pScreen->GetTextSprite( ID_FRIENDS_DESCRIPTION )->SetText(
				"Send a friend request to a player by name."
			);
            break;

        case ID_FRIENDS_RECENT:
            m_pScreen->GetTextSprite( ID_FRIENDS_DESCRIPTION )->SetText(
				"View a list of recent players you have been in a match with and then send a friend request to them."
			);
            break;
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// Get the friend's name entered on the virtual keyboard
///
/// \return The friend's name entered on the virtual keyboard
///////////////////////////////////////////////////////////////////////////////////////////////////
const char *ScreenFriends::GetEnteredFriendName()
{
    return GetKeyboardString();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when the virtual keyboard closes.
///////////////////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ void ScreenFriends::OnKeyboardInput( const char* pString )
{
	_status = GOT_NAME_FOR_FRIEND_REQUEST;
}

#ifdef DIRECTX_PC
///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when the screen updates.  Necessary to update button descriptions when selected with mouse.
///////////////////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ void ScreenFriends::OnScreenUpdate()
{
	ScreenBaseClass::OnScreenUpdate();

	OnCursorUpdate();
}
#endif