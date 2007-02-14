#include "TTLPCH.h"
#include "frontend/inputdriver.h"
#include "UseCaseYesNoDialog.hpp"
#include "../screens/screen_YesNoDialog.h"
#include "../screens/screen_Dialog.h"
#include "frontend/frontend.h"

//XBox use cases
#ifdef _XBOX
#include "frontend/UseCase/Xbox/UseCaseUIXLogin.hpp"
#endif


///////////////////////////////////////////////////////////////////////////////////////////////////
/// Constructor: Initialize member variables
///////////////////////////////////////////////////////////////////////////////////////////////////
OnlineUI::YesNoDialog::YesNoDialog() : 
    _stateMachine(INIT)
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called before use case is started.  Show the first screen for the use case.
///
/// \return true if succeeded, false otherwise
///////////////////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ bool OnlineUI::YesNoDialog::_init( void )
{
    g_FrontEnd.ShowPopup("SCREEN_ID_YES_NO_DIALOG");
    _answer = RUNNING;

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called after use case is ended.  Clean up the screens and other objects.
///
/// \return true if succeeded, false otherwise
///////////////////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ bool OnlineUI::YesNoDialog::_deinit( void )
{
	if( UseCase::IsScreenActive< ScreenYesNoDialog >() )
		g_FrontEnd.RemoveActivePopup();
	g_FrontEnd.ClosePopup( "SCREEN_ID_YES_NO_DIALOG" );

	if( _answer == RUNNING )
		_answer = CANCEL; // can't stop without an answer, choose no

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case.
///
/// \return true to continue use case processing, false to return to application main menu.
///////////////////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ bool OnlineUI::YesNoDialog::update()
{
    ScreenYesNoDialog *pScreen = expectPopup<ScreenYesNoDialog>();
	if ( !pScreen )
	{
		return true ;
	}

    switch (pScreen->GetScreenStatus())
    {
        case ScreenYesNoDialog::YES:
            _answer = YES;
            stop();
            break;

        case ScreenYesNoDialog::NO:
            _answer = NO;
            stop();
            break;

		case ScreenYesNoDialog::CANCEL:
			_answer = CANCEL;
			stop();
			break;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Show the dialog with the given title and text
///
/// \param pTitle Dialog title
///
/// \param pText Dialog text (is centered and will word wrap automatically)
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::YesNoDialog::start(const char *pTitle, const char *pText, bool defaultYesButton)
{
    UseCase::start();

    if (!g_FrontEnd.GetActiveScreen()) return;

    ScreenYesNoDialog *pScreen = expectPopup<ScreenYesNoDialog>();
	if ( ! pScreen )
	{
		return;
	}

    pScreen->SetTitleText(pTitle);
    pScreen->SetDialogText(pText);

	pScreen->SetDefaultButtonToYes(defaultYesButton);

	g_FrontEnd.ShowScreen( g_FrontEnd.GetActiveScreen() );
}
