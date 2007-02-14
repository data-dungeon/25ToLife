#include "TTLPCH.h"
#include "frontend/inputdriver.h"
#include "screen_players.h"
#include "data/screens/Multiplayer.h"
#include "frontend/frontend.h"

/*virtual*/ void ScreenPlayers::OnInitialize(void)
{
    Screen* pScreen = m_pScreen;

    UseHighlighting(true);

	if(pScreen)
	{
		/*pScreen->SetInitialButton(Players_AGREE);
		pScreen->SetNextButton(Players_DISAGREE);
		pScreen->SetCursor(Players_CURSOR);

        pScreen->SelectButton(Players_AGREE);*/
	}

	ScreenBaseClass::OnInitialize();
}

void ScreenPlayers::OnScreenIntro( void )
{
    _status = RUNNING;

    ScreenBaseClass::OnScreenIntro();
}


void ScreenPlayers::OnCommand( int Command )
{
    // Set the status flag
    /*switch (Command)
    {
        case Players_AGREE:
            _status = AGREE;
            break;

        case Players_DISAGREE:
            _status = DISAGREE;
            break;
    }*/

    ScreenBaseClass::OnCommand(Command);
}
