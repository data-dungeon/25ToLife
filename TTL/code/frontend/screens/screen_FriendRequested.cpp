#include "TTLPCH.h"
#include "frontend/inputdriver.h"
#include "screen_FriendRequested.h"
#include "data/screens/FriendsAndPlayers.h"
#include "frontend/frontend.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when the screen is first loaded
///////////////////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ void ScreenFriendRequested::OnInitialize(void)
{
    UseHighlighting(true);

	SetInitialButton(ID_REQUEST_YES);
	SetNextButton(ID_REQUEST_NO);
	SetNextButton(ID_REQUEST_BLOCK);
	SetCursor(ID_REQUEST_CURSOR);
	SelectInitialButton();
		
	ScreenBaseClass::OnInitialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every time when the screen is displayed
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenFriendRequested::OnScreenIntro( void )
{
    _status = RUNNING;

    ScreenBaseClass::OnScreenIntro();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when user selects a button
///
/// \param Command ID of the selected button
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenFriendRequested::OnCommand( int Command )
{
    // Set the status flag
    switch (Command)
    {
        case ID_REQUEST_YES:
            _status = YES;
            break;

        case ID_REQUEST_NO:
            _status = NO;
            break;

        case ID_REQUEST_BLOCK:
            _status = BLOCK;
            break;
    }

    ScreenBaseClass::OnCommand(Command);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Set the name of the friend on the screen
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenFriendRequested::SetFriendName(const char *pFriendName)
{
    char message[500];

//rhn    m_pScreen->GetTextSprite(ID_REQUEST_TITLE)->SetText(pFriendName);
    strcpy(message, "You have received a friends request from ");
    strcat(message, pFriendName);
    strcat(message, ". Do you want to accept the request?");
    m_pScreen->GetTextSprite(ID_REQUEST_TEXT)->SetText(message);
}
