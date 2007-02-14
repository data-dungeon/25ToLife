///////////////////////////////////////////////////////////////////////////////////////
///  \file UseCaseMainMenu.cpp
///
///  \brief Implementation of the MainMenu class, which is responsible 
/// for presenting the Main Menu and allowing the player to pick what
/// they want to do.
///////////////////////////////////////////////////////////////////////////////////////

#include "TTLPCH.h"

#include "UseCaseEORConfig.hpp"
#include "UseCaseDisplayDialog.hpp"
#include "frontend/frontend.h"
#include "../screens/screen_EORConnect.h"
#include "../screens/screen_Sandbox.h"
#include "UseCaseEOA.hpp"
#include "OSI/PS2/OSIPS2.hpp"
#include <netz.h>
#include "Network/NetZ/NetZConnection.h"
#include "frontend/savegame.h"

#ifndef DIRECTX_PC
#include "platform/PS2/CDllLoader.h"
#endif // DIRECTX_PC

// Extern data from OSIPS2.cpp
#if defined(PS2) || defined(DIRECTX_PC)
extern char			g_rendez_vous_server[256];
extern unsigned int	g_rendez_vous_port;
#endif //PS2

//PS2ProfileList OnlineUI::EORConfig::_profileList;

extern const char* g_RemoteIPAddress;

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Constructor: Initialize member variables
///////////////////////////////////////////////////////////////////////////////////////////////////
OnlineUI::EORConfig::EORConfig() : 
    _stateMachine(NET_CONFIG)
{
    _stateMachine.setStateUpdate(NET_CONFIG, &OnlineUI::EORConfig::_updateNetConfig);
    _stateMachine.setStateUpdate(CONNECT, &OnlineUI::EORConfig::_updateConnect);
    _stateMachine.setStateUpdate(SANDBOX, &OnlineUI::EORConfig::_updateSandbox);
    _stateMachine.setStateUpdate(DNAS, &OnlineUI::EORConfig::_updateDnas);
    _stateMachine.setStateUpdate(TERMINATE, &OnlineUI::EORConfig::_updateTerminate);

    _stateMachine.setStateEnter(DNAS, &OnlineUI::EORConfig::_dnasEntry);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called before use case is started.  Show the first screen for the use case.
///
/// \return true if succeeded, false otherwise
///////////////////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ bool OnlineUI::EORConfig::_init( void )
{
	_dnasRunning = false;
	_stateMachine.setCurrentState( this, NET_CONFIG );
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called after use case is ended.  Clean up the screens and other objects.
///
/// \return true if succeeded, false otherwise
///////////////////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ bool OnlineUI::EORConfig::_deinit( void )
{
#ifndef DIRECTX_PC
	if( _dnasRunning )
	{
		// A fatal error occured during DNAS, need to abort
		OSIPS2 *pPS2 = reinterpret_cast<OSIPS2 *>(OSI::getSingletonPtr());

		pPS2->cancelDNASAuth();
	}
#endif
	_dnasRunning = false;

	g_FrontEnd.RemoveAllPopups();
	if( UseCase::IsScreenActive< ScreenEORConnect >() )
	{
		g_FrontEnd.EndScreen( 0 );
	}
	g_FrontEnd.PopScreenHistory( "SCREEN_ID_CONNECTING_2_10" );
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the NET_CONFIG state.
///
/// \return true to continue use case processing, false to return to application main menu.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::EORConfig::_updateNetConfig()
{
//	g_FrontEnd.EndScreen(0);

	// Bypassing the Network config screen and going straight to the sandbox selection
	// since Avalanche has done that screen already.

	// For final consumer build, we don't want a Sandbox choice.
#ifdef CONSUMER_BUILD
	g_FrontEnd.GotoScreen("SCREEN_ID_CONNECTING_2_10");
	_stateMachine.setCurrentState(this, CONNECT);

	// Set the sandbox we're using
	strcpy(g_rendez_vous_server, "ttlliveps2.quazal.net");
	g_rendez_vous_port = 30120;

#else
	// For internal testing builds, we do.
	g_FrontEnd.GotoScreen("SCREEN_ID_SANDBOX_SELECT");
	_stateMachine.setCurrentState(this, SANDBOX);
#endif

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the CONNECT state.
///
/// \return true to continue use case processing, false to return to application main menu.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::EORConfig::_updateConnect()
{
    if (!g_FrontEnd.GetActiveScreen()) return true;

    // Execute DNAS.
#ifdef CDROM
    // NOTE: Since we're using a non-standard crt0.s for development builds,
    // DNAS will crash the app.  It will only work properly for CDROM
    // builds since we're using a standard crt0.s for those.
    _stateMachine.setCurrentState(this, DNAS);
#else
	g_FrontEnd.EndScreen(0);
    // Terminate us the next time we come back here
    _stateMachine.setCurrentState(this, TERMINATE);

    EOREOA::getSingletonPtr()->start();
#endif
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the SANDBOX state.
///
/// \return true to continue use case processing, false to return to application main menu.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::EORConfig::_updateSandbox()
{
    if (!g_FrontEnd.GetActiveScreen()) return true;

    ScreenSandbox *pScreen = UseCase::GetActiveScreen<ScreenSandbox>();
	if ( !pScreen ) return true ;

    bool next = false;
    switch (pScreen->GetScreenStatus())
    {
        case ScreenSandbox::TTL1:
            strcpy(g_rendez_vous_server, "ttl1.quazal.net");
            g_rendez_vous_port = 10100;
            next = true;
            break;

        case ScreenSandbox::TTL2:
            strcpy(g_rendez_vous_server, "ttl2.quazal.net");
            g_rendez_vous_port = 10105;
            next = true;
            break;

        case ScreenSandbox::TTL3:
            strcpy(g_rendez_vous_server, "ttl3.quazal.net");
            g_rendez_vous_port = 10110;
            next = true;
            break;
    }

    if (next)
    {
        g_FrontEnd.EndScreen(0);
        g_FrontEnd.GotoScreen("SCREEN_ID_CONNECTING_2_10");
        _stateMachine.setCurrentState(this, CONNECT);
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the TERMINATE state.
///
/// \return true to continue use case processing, false to return to application main menu.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::EORConfig::_updateTerminate()
{
    stop();
    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called upon entering the DNAS state.  Start DNAS authorization
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::EORConfig::_dnasEntry()
{
#ifndef DIRECTX_PC
    OSIPS2 *pPS2 = reinterpret_cast<OSIPS2 *>(OSI::getSingletonPtr());

    if (!pPS2->beginDNASAuth())
    {
        _stateMachine.setCurrentState(this, TERMINATE);
        DisplayDialog::getSingletonPtr()->start("DNAS Failed", pPS2->getErrorString() );
    }
	else
	{
		_dnasRunning = true;
	}
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the DNAS state.
///
/// \return true to continue use case processing, false to return to application main menu.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::EORConfig::_updateDnas()
{
#ifndef DIRECTX_PC
    OSIPS2 *pPS2 = reinterpret_cast<OSIPS2 *>(OSI::getSingletonPtr());
    ScreenEORConnect *pScreen = expectScreen<ScreenEORConnect>();
	if ( !pScreen )
	{
		pPS2->pauseDNASAuth();
		return true ;
	}

    // See if the user wants to cancel DNAS
    if (pScreen->CancelDnas())
    {
       if (pPS2->abortDNASAuth() == 0)
       {
            _dnasRunning = false;
       }
        _stateMachine.setCurrentState(this, TERMINATE);
        return true;
    }

    int dnasResult = pPS2->updateDNASAuth();
    
    // DNAS hasn't finished yet, keep processing
    if (dnasResult == 0)
    {
        return true;
    }
    // DNAS completed successfully
    else if (dnasResult == 1)
    {
		_dnasRunning = false;
        _stateMachine.setCurrentState(this, TERMINATE);
		g_FrontEnd.EndScreen( 0 );
        EOREOA::getSingletonPtr()->start();
    }
    // DNAS error, bail.
    else
    {
		_dnasRunning = false;
        // For CD builds, we want to stop executing.
#ifdef CDROM
		_stateMachine.setCurrentState(this, TERMINATE);
        DisplayDialog::getSingletonPtr()->start("DNAS Failed", pPS2->getErrorString() );
#else
        // For non-CD builds, we want to keep running since we don't have DNAS
        _stateMachine.setCurrentState(this, TERMINATE);
        EOREOA::getSingletonPtr()->start();
        DisplayDialog::getSingletonPtr()->start("DNAS Failed", pPS2->getErrorString());
#endif
    }
#endif
    return true;
}
