#include "TTLPCH.h"

#include "UseCaseWaitForOSIRequest.hpp"
#include "frontend/frontend.h"
#include "../screens/screen_wait.h"
#include "UseCaseDisplayDialog.hpp"
#include "GameHelper/ErrorStack.h"
#include "main/Lobby.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Constructor: Initialize member variables
///////////////////////////////////////////////////////////////////////////////////////////////////
OnlineUI::WaitForOSIRequest::WaitForOSIRequest() : 
    _stateMachine( OSI_REQUEST_PUMP )
{
	_stateMachine.setStateUpdate(OSI_REQUEST_PUMP, &OnlineUI::WaitForOSIRequest::_updateOSIRequestPump);
	_stateMachine.setStateUpdate(FATAL_ERROR, &OnlineUI::WaitForOSIRequest::_updateFatalError);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called before use case is started.  Show the first screen for the use case.
///
/// \return true if succeeded, false otherwise
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::WaitForOSIRequest::_init( void )
{
	if( _displayWait )
	{
		g_FrontEnd.ShowPopup("SCREEN_ID_WAIT");
	}
	_stateMachine.setCurrentState( this, OSI_REQUEST_PUMP );
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called after use case is ended.  Clean up the screens and other objects.
///
/// \return true if succeeded, false otherwise
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::WaitForOSIRequest::_deinit( void )
{
	if( _displayWait )
	{
		if( UseCase::IsScreenActive< ScreenWait >() )
			g_FrontEnd.RemoveActivePopup();
		g_FrontEnd.ClosePopup( "SCREEN_ID_WAIT" );
	}
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Wait for an OSI request to complete
///
/// This function takes in the boolean of the OSI request last called and a title and text
/// to display as a wait screen while the request completes.  It will also display an error
/// screen if it encounters one during the processing.  If you do not want a wait screen
/// set the title to "".
///
/// \param requestStatus Whether the request succeeded or failed
/// \param pTitle Wait screen title ("" for no wait screen)
/// \param pText Wait screen text (is centered and will word wrap automatically)
/// \param displayError True to display a dialog error message on request failure (default true)
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::WaitForOSIRequest::start(
	bool requestStatus, const char *pTitle, const char *pText, bool displayError
) {
	_displayError = displayError;
	if( pTitle == NULL || pTitle[0] == 0 )
	{
		_displayWait = false;
	}
	else
	{
		_displayWait = true;
	}

	if( !requestStatus )
	{
		if ( OSI::getSingletonPtr()->isLastErrorFatal() )
		{
			OSIchar errorMsg[ 512 ];
			if( g_lobby.GetDisconnectMessage() )
			{
				OSIsnprintf( errorMsg, NUM_ARRAY_ELEMENTS(errorMsg), "%s %s",
					OSI::getSingletonPtr()->getErrorString(), g_lobby.GetDisconnectMessage()
				);
			}
			else
			{
				OSIstrncat( errorMsg, OSI::getSingletonPtr()->getErrorString(), NUM_ARRAY_ELEMENTS(errorMsg) );
			}
			g_errorStack.SetError( "Request Failed", errorMsg, CErrorStack::ERROR_FATAL, 5000 );
		}
		else if( _displayError )
		{
			DisplayDialog::getSingletonPtr()->start( "Request Failed", OSI::getSingletonPtr()->getErrorString() );
		}
		return;
	}

    UseCase::start();

	//if( _displayWait )
	//{
	//	ScreenWait *pScreen = UseCase::GetActiveScreen<ScreenWait>();
	//	if ( !pScreen ) return ;

	//	pScreen->SetTitleText( pTitle );
	//	pScreen->SetDialogText( pText );

	//	g_FrontEnd.ShowScreen( g_FrontEnd.GetActiveScreen() );
	//}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called each time the OSI_REQUEST_PUMP state is entered.
///////////////////////////////////////////////////////////////////////////////////////////////////
//void OnlineUI::WaitForOSIRequest::_enterOSIRequestPump()
//{
//
//}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the OSI_REQUEST_PUMP state.
///
/// \return true to continue use case processing, false to return to application main menu.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::WaitForOSIRequest::_updateOSIRequestPump()
{
	OSI* osi = OSI::getSingletonPtr();
	if( osi->processLastRequest() )
	{
		if( osi->getLastRequestStatus() == STATUS_FAILED )
		{
			if ( OSI::getSingletonPtr()->isLastErrorFatal() )
			{
				_displayWait = 0 ; // don't try to hide the wait popup a second time, we've already hidden it.
				g_FrontEnd.EndPopup(0);
				OSIchar errorMsg[ 512 ];
				if( g_lobby.GetDisconnectMessage() )
				{
					OSIsnprintf( errorMsg, NUM_ARRAY_ELEMENTS(errorMsg), "%s %s",
						OSI::getSingletonPtr()->getErrorString(), g_lobby.GetDisconnectMessage()
					);
				}
				else
				{
					OSIstrncat( errorMsg, OSI::getSingletonPtr()->getErrorString(), NUM_ARRAY_ELEMENTS(errorMsg) );
				}
				g_errorStack.SetError( "Request Failed", errorMsg, CErrorStack::ERROR_FATAL, 5000 );
				_stateMachine.setCurrentState( this, FATAL_ERROR );
			}
			else
			{
				stop();
				if( _displayError )
				{
					DisplayDialog::getSingletonPtr()->start( "Request Failed", OSI::getSingletonPtr()->getErrorString() );
				}
			}
		}
		else
		{
			stop();
		}
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the FATAL_ERROR state.
///
/// This state basically does nothing.  Once WaitForOSIRequest gets in this
/// state, the only way out is for an external force (OnlineSetupLoop) to
/// explicitly call stop on ever UseCase in the stack.  The top most use case
/// will be this one, and it will terminate.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::WaitForOSIRequest::_updateFatalError()
{
	return true;
}




