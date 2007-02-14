#include "TTLPCH.h"
#include "frontend/inputdriver.h"
#include "screen_friendConfirmed.h"
#include "data/screens/FriendsAndPlayers.h"
#include "frontend/frontend.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when the screen is first loaded
///////////////////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ void ScreenFriendConfirmed::OnInitialize(void)
{
	SetHelpRect(ID_CONFIRMED_HELPRECT);
	SetInitialButton(ID_CONFIRMED_JOIN);
	SetNextButton(ID_CONFIRMED_INVITE);
	SetNextButton(ID_CONFIRMED_REMOVE);
	SetNextButton(ID_CONFIRMED_BACK);
	SetCursor(ID_CONFIRMED_CURSOR);

	EnableButton(ID_CONFIRMED_INVITE, false);

	SelectButton(ID_CONFIRMED_JOIN);

	// Hide PS2 help bar on PC
	//#ifdef DIRECTX_PC
	//	GetSprite(ID_CLAN_CREATE_HELPRECT)->HideAll();
	//#endif

	UseHighlighting(true);

	ScreenBaseClass::OnInitialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every time when the screen is displayed
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenFriendConfirmed::OnScreenIntro( void )
{
    _status = RUNNING;

    ScreenBaseClass::OnScreenIntro();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when user selects a button
///
/// \param Command ID of the selected button
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenFriendConfirmed::OnCommand( int Command )
{
    // Set the status flag
    switch (Command)
    {
        case ID_CONFIRMED_JOIN:
            _status = JOIN;
            break;

        case ID_CONFIRMED_INVITE:
            _status = INVITE;
            break;

        case ID_CONFIRMED_REMOVE:
            _status = REMOVE;
            break;

        case ID_CONFIRMED_BACK:
            _status = BACK;
            break;
    }

    ScreenBaseClass::OnCommand(Command);
}

