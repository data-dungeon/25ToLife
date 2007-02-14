#include "TTLPCH.h"
#include "frontend/inputdriver.h"
#include "screen_AskPassword.h"
#include "data/screens/EOR.h"
#include "frontend/frontend.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every time the screen is brought up.
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenAskPasswordPopup::OnScreenIntro( void )
{
    _status = RUNNING;

    ScreenBaseClass::OnScreenIntro();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when the screen is created.
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenAskPasswordPopup::OnInitialize( void )
{
	UseHighlighting( true );

    if (m_pScreen)
    {
		m_pScreen->SetInitialButton(ID_ASKPWD_ENTER_PWD);
        m_pScreen->SetNextButton(ID_ASKPWD_CANCEL);

		m_pScreen->SetCursor(ID_ASKPWD_CURSOR);

		m_pScreen->SelectInitialButton();
    }

    ScreenBaseClass::OnInitialize();

	return;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when a button is selected on the screen
///
/// \param Command The button pressed
///////////////////////////////////////////////////////////////////////////////////////////////////
/* virtual */ void ScreenAskPasswordPopup::OnCommand(int Command)
{
    switch (Command)
    {
        case ID_ASKPWD_ENTER_PWD:
            _status = ENTER_PASSWORD;
            break;

        case ID_ASKPWD_CANCEL:
            _status = BACK;
            break;

		default:
			ScreenBasePopup::OnCommand( Command );
			break;
    }
}