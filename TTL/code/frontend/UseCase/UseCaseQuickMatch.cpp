///////////////////////////////////////////////////////////////////////////////////////
///  \file UseCaseMainMenu.cpp
///
///  \brief Implementation of the MainMenu class, which is responsible 
/// for presenting the Main Menu and allowing the player to pick what
/// they want to do.
///////////////////////////////////////////////////////////////////////////////////////

#include "TTLPCH.h"

#include "UseCaseQuickMatch.hpp"
#include "UseCaseHostMatch.hpp"
#include "UseCaseJoinMatch.hpp"
#include "frontend/frontend.h"
#include "UseCaseDisplayDialog.hpp"
#include "UseCaseYesNoDialog.hpp"
#include "../screens/screen_wait.h"
#include "../screens/screen_Quickmatch.h"
#include "Network/SessionSpec.h"
#include "main/Lobby.h"
#include "UseCaseUtility.hpp"
#include "UseCaseWaitForOSIRequest.hpp"
#include "GameHelper/DebugOutput.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Constructor: Initialize member variables
///////////////////////////////////////////////////////////////////////////////////////////////////
OnlineUI::QuickMatch::QuickMatch() : 
	_cleanUpScreens( true ),
    _stateMachine(GO_TO_EXECUTE_FIND)
{
	_stateMachine.setStateUpdate(GO_TO_EXECUTE_FIND, &OnlineUI::QuickMatch::_updateGoToExecuteFind);

	_stateMachine.setStateEnter(EXECUTE_FIND,        &OnlineUI::QuickMatch::_enterExecuteFind);
	_stateMachine.setStateUpdate(EXECUTE_FIND,       &OnlineUI::QuickMatch::_updateExecuteFind);

    _stateMachine.setStateEnter(NO_GAMES_FOUND,      &OnlineUI::QuickMatch::_enterNoGamesFound);
    _stateMachine.setStateUpdate(NO_GAMES_FOUND,     &OnlineUI::QuickMatch::_updateNoGamesFound);

    _stateMachine.setStateEnter(SELECT_MATCH,        &OnlineUI::QuickMatch::_enterSelectMatch);
    _stateMachine.setStateUpdate(SELECT_MATCH,       &OnlineUI::QuickMatch::_updateSelectMatch);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called before use case is started.  Show the first screen for the use case.
///
/// \return true if succeeded, false otherwise
///////////////////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ bool OnlineUI::QuickMatch::_init( void )
{
	_cleanUpScreens = true;
	_stateMachine.setCurrentState( this, GO_TO_EXECUTE_FIND );
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called after use case is ended.  Clean up the screens and other objects.
///
/// \return true if succeeded, false otherwise
///////////////////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ bool OnlineUI::QuickMatch::_deinit( void )
{
	g_FrontEnd.RemoveAllPopups();
	//if( _cleanUpScreens )
	{
		if( UseCase::IsScreenActive< ScreenQuickmatch >() )
			g_FrontEnd.EndScreen( 0 );
		g_FrontEnd.PopScreenHistory( "SCREEN_ID_QUICKMATCH" );
	}
    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the GO_TO_EXECUTE_FIND state.
///
/// \return true if succeeded, false otherwise
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::QuickMatch::_updateGoToExecuteFind()
{
	// This state is currently here to bypass the fact that you can't shut down
	// in the initial state without causing the enter function to not get called.
	_stateMachine.setCurrentState(this, EXECUTE_FIND);
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when entering the EXECUTE_FIND state.
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::QuickMatch::_enterExecuteFind()
{
    // HACK until configure screens are working
	unsigned int flags = OSIMatchOptions::MATCHOPTIONFLAG_QUICKMATCH_MASK;
	_matchOptions.gameTypeIndex			= -1;
	_matchOptions.levelIndex			= 0;
	_matchOptions.timeLimit				= 2;
	_matchOptions.setWildcardFlags( flags );
	OSIstrcpy(_matchOptions.levelName, "Any" );

	bool result = OSI::getSingletonPtr()->requestMatchList( _matchOptions );

	WaitForOSIRequest::getSingletonPtr()->start( result, "Quickmatch", "Finding game..." );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called while waiting for the match find results.
///
/// \return true if succeeded, false otherwise
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::QuickMatch::_updateExecuteFind()
{
	int size = 0;
	OSIRequestStatus requestStatus = OSI::getSingletonPtr()->getLastRequestStatus();
	if ( requestStatus == STATUS_SUCCEEDED )
	{
		_matchList = &OSI::getSingletonPtr()->getMatchList();
		size = _matchList->count();
		if ( size > 0 )
		{
			// Pump matches into search object
			COnlineSessionSearch &search = (COnlineSessionSearch &)g_lobby.SessionSearch();
			search.SetResults(*_matchList);

            // Start the quickmatch screen
            _stateMachine.setCurrentState(this, SELECT_MATCH);
		}
		else
		{
			// No games found, try to host
			_stateMachine.setCurrentState(this, NO_GAMES_FOUND);
		}

	}
	else
	{	
       stop();
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when entering the NO_GAMES_FOUND state.
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::QuickMatch::_enterNoGamesFound()
{
    // OUITODO: Make sure this text is TCR compliant
	 // Dan Silver - Testers are saying "Quickmatch" has a space and the M is capitalized. So I changed it.
	 // See bug 7778.
    YesNoDialog::getSingletonPtr()->start("Quick Match", "No games found.  Would you like to create one?");
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the NO_GAMES_FOUND state.
///
/// \return true to continue use case processing, false to return to application main menu.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::QuickMatch::_updateNoGamesFound()
{
	stop();
    if (YesNoDialog::getSingletonPtr()->GetAnswer() == YesNoDialog::YES)
    {
		HostMatch::getSingletonPtr()->start();
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::QuickMatch::_enterSelectMatch()
{
    g_FrontEnd.GotoScreen("SCREEN_ID_QUICKMATCH");

    // Send match data to screen
    _matchList->begin();
    const OSIMatch* match = _matchList->getCurrent();
    ASSERT(match);

    _currentMatchNum = 1;
	ScreenQuickmatch *pScreen = UseCase::GetActiveScreen<ScreenQuickmatch>();
	if ( !pScreen ) return ;

    pScreen->SetMatchNum(_currentMatchNum, _matchList->count());
    pScreen->SetGameType(COnlineSessionSearch::GameTypeIndexToStringLUT[match->options.gameTypeIndex]);
    pScreen->SetNumPlayers(match->options.totalConnectedPlayers, match->maximumPlayers);
    pScreen->SetHostName(match->options.description);
    pScreen->SetMapName(match->options.levelName);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::QuickMatch::_updateSelectMatch()
{
    const OSIMatch* match = _matchList->getCurrent();
    ASSERT(match);

	ScreenQuickmatch* pScreen = UseCase::GetActiveScreen<ScreenQuickmatch>();
	if ( !pScreen ) return true ;

    switch ( pScreen->GetStatus())
    {
        case ScreenQuickmatch::JOIN:
            if ( match )
	        {
				g_debugOutput.Print(g_debugOutput.Category("NET"), "QuickMatch: Join '%s'", match->options.description);
				_cleanUpScreens = false;
		        JoinMatch::getSingletonPtr()->setMatchToJoin( match->id );
				stop();
		        JoinMatch::getSingletonPtr()->start();
	        }
			else
			{
				stop();
			}
            break;

        case ScreenQuickmatch::NEXT:
		{
            _matchList->next();
            ++_currentMatchNum;

            // Hit the end of the list, restart
            if (!_matchList->getCurrent())
            {
                _matchList->begin();
                _currentMatchNum = 1;
            }

            match = _matchList->getCurrent();
            ASSERT(match);

            pScreen->SetMatchNum(_currentMatchNum, _matchList->count());
            pScreen->SetGameType(COnlineSessionSearch::GameTypeIndexToStringLUT[match->options.gameTypeIndex]);
            pScreen->SetNumPlayers(match->options.totalConnectedPlayers, match->maximumPlayers);
            pScreen->SetHostName(match->options.description);
            pScreen->SetMapName(match->options.levelName);
		}
            break;

        case ScreenQuickmatch::BACK:
            g_FrontEnd.EndScreen(0);
            stop();
            break;
    }

    return true;
}
