#include "TTLPCH.h"
#include "frontend/inputdriver.h"
#include "screen_Sandbox.h"
#include "data/screens/EOR.h"
#include "frontend/frontend.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when the screen is first loaded
///////////////////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ void ScreenSandbox::OnInitialize(void)
{
    Screen* pScreen = m_pScreen;

    UseHighlighting(true);

	if(pScreen)
	{
		pScreen->SetInitialButton(ID_SANDBOX_TTL1);
		pScreen->SetNextButton(ID_SANDBOX_TTL2);
        pScreen->SetNextButton(ID_SANDBOX_TTL3);
		pScreen->SetCursor(ID_SANDBOX_CURSOR);

        pScreen->SelectButton(ID_SANDBOX_TTL3);
	}

	ScreenBaseClass::OnInitialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every time when the screen is displayed
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenSandbox::OnScreenIntro( void )
{
    _status = RUNNING;

    ScreenBaseClass::OnScreenIntro();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when user selects a button
///
/// \param Command ID of the selected button
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenSandbox::OnCommand( int Command )
{
    // Set the status flag
    switch (Command)
    {
        case ID_SANDBOX_TTL1:
            _status = TTL1;
            break;

        case ID_SANDBOX_TTL2:
            _status = TTL2;
            break;

        case ID_SANDBOX_TTL3:
            _status = TTL3;
            break;
    }

    ScreenBaseClass::OnCommand(Command);
}
