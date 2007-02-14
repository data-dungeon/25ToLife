#include "TTLPCH.h"
#include "UseCaseDisplayDialog.hpp"
#include "UseCaseEORLogin.hpp"
#include "UseCaseEOA.hpp"
#include "UseCaseEORPatcher.hpp"
#include "UseCaseYesNoDialog.hpp"
#include "../screens/screen_EOA.h"
#include "frontend/inputdriver.h"
#include "frontend/frontend.h"
#include "UseCaseWaitForOSIRequest.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Constructor: Initialize member variables
///////////////////////////////////////////////////////////////////////////////////////////////////
OnlineUI::EOREOA::EOREOA() : 
    _stateMachine(INIT)
{
    _stateMachine.setStateUpdate(INIT, &OnlineUI::EOREOA::_updateInit);
	_stateMachine.setStateEnter(DNS_LOOKUP, &OnlineUI::EOREOA::_enterDNSLookup);
	_stateMachine.setStateUpdate(DNS_LOOKUP, &OnlineUI::EOREOA::_updateDNSLookup);
	_stateMachine.setStateEnter(ASK_TO_DISCO, &OnlineUI::EOREOA::_enterAskToDisco);
	_stateMachine.setStateUpdate(ASK_TO_DISCO, &OnlineUI::EOREOA::_updateAskToDisco);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called before use case is started.  Show the first screen for the use case.
///
/// \return true if succeeded, false otherwise
///////////////////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ bool OnlineUI::EOREOA::_init( void )
{
    g_FrontEnd.GotoScreen("SCREEN_ID_EOA_2_00");

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called after use case is ended.  Clean up the screens and other objects.
///
/// \return true if succeeded, false otherwise
///////////////////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ bool OnlineUI::EOREOA::_deinit( void )
{
	g_FrontEnd.RemoveAllPopups();
	if( UseCase::IsScreenActive< ScreenEOA >() )
		g_FrontEnd.EndScreen(0);
	g_FrontEnd.PopScreenHistory( "SCREEN_ID_EOA_2_00" );

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the INIT state.
///
/// \return true to continue use case processing, false to return to application main menu.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::EOREOA::_updateInit()
{
	if (!g_FrontEnd.GetActiveScreen()) return true;

	ScreenEOA *pScreen = UseCase::GetActiveScreen<ScreenEOA>();
	if ( !pScreen ) return true ;

	switch (pScreen->GetScreenStatus())
	{
		case ScreenEOA::AGREE:
			_stateMachine.setCurrentState( this, DNS_LOOKUP );
			break;

		case ScreenEOA::DISAGREE:
		case ScreenEOA::BACK:
#ifdef DIRECTX_PC
			stop();
#else
			_stateMachine.setCurrentState(this, ASK_TO_DISCO);
#endif
			return true;
	}

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when entering the DNS_LOOKUP state.
///////////////////////////////////////////////////////////////////////////////////////////////////
extern char g_rendez_vous_server[];
void OnlineUI::EOREOA::_enterDNSLookup()
{
	const bool requestStatus = OSI::getSingletonPtr()->requestDNSLookup( g_rendez_vous_server );

	WaitForOSIRequest::getSingletonPtr()->start( requestStatus, "Please wait", "Looking up server name." );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the DNS_LOOKUP state.
///
/// \return true to continue use case processing, false to return to application main menu.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::EOREOA::_updateDNSLookup()
{
	// We don't care about success or failure because if we fail this, we
	// will most definitely fail logging in.
	stop();
#if defined CDROM && !defined DIRECTX_PC
	EORPatcher::getSingletonPtr()->start();
#else
	EORLogin::getSingletonPtr()->start();
#endif

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when entering the ASK_TO_DISCO state.
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::EOREOA::_enterAskToDisco()
{
	YesNoDialog::getSingletonPtr()->start( "", "Are you sure you want to disconnect from the network?", false );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the ASK_TO_DISCO state.
///
/// \return true to continue use case processing, false to return to application main menu.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::EOREOA::_updateAskToDisco()
{
	switch( YesNoDialog::getSingletonPtr()->GetAnswer() )
	{
	case YesNoDialog::YES:
		stop();
		break;
	case YesNoDialog::NO:
	case YesNoDialog::CANCEL:
	default:
		_stateMachine.setCurrentState( this, INIT );
		break;
	}

    return true;
}
