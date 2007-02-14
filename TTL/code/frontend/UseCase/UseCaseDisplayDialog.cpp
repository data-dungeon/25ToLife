#include "TTLPCH.h"
#include "frontend/inputdriver.h"
#include "UseCaseDisplayDialog.hpp"
#include "../screens/screen_Dialog.h"
#include "frontend/frontend.h"

//XBox use cases
#ifdef _XBOX
#include "frontend/UseCase/Xbox/UseCaseUIXLogin.hpp"
#endif


///////////////////////////////////////////////////////////////////////////////////////////////////
/// Constructor: Initialize member variables
///////////////////////////////////////////////////////////////////////////////////////////////////
OnlineUI::DisplayDialog::DisplayDialog() : 
    _stateMachine(INIT)
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called before use case is started.  Show the first screen for the use case.
///
/// \return true if succeeded, false otherwise
///////////////////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ bool OnlineUI::DisplayDialog::_init( void )
{
    g_FrontEnd.ShowPopup("SCREEN_ID_DIALOG");

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called after use case is ended.  Clean up the screens and other objects.
///
/// \return true if succeeded, false otherwise
///////////////////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ bool OnlineUI::DisplayDialog::_deinit( void )
{
	if( UseCase::IsScreenActive< ScreenDialog >() )
	{
		g_FrontEnd.RemoveActivePopup();
	}
	g_FrontEnd.ClosePopup( "SCREEN_ID_DIALOG" );
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case.
///
/// \return true to continue use case processing, false to return to application main menu.
///////////////////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ bool OnlineUI::DisplayDialog::update()
{
    ScreenDialog *pScreen = expectPopup<ScreenDialog>();
	if ( !pScreen )
	{
		return true ;
	}

    if (pScreen->GetStatus() == ScreenDialog::DONE)
    {
        stop();
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
void OnlineUI::DisplayDialog::start(const char *pTitle, const char *pText)
{
    UseCase::start();

    ScreenDialog *pScreen = UseCase::GetActiveScreen<ScreenDialog>();
	if ( !pScreen ) return ;

    pScreen->SetTitleText(pTitle);
    pScreen->SetDialogText(pText);

	g_FrontEnd.ShowScreen( g_FrontEnd.GetActiveScreen() );
}
