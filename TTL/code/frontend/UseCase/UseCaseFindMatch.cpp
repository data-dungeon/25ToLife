///////////////////////////////////////////////////////////////////////////////////////
///  \file UseCaseMainMenu.cpp
///
///  \brief Implementation of the MainMenu class, which is responsible 
/// for presenting the Main Menu and allowing the player to pick what
/// they want to do.
///////////////////////////////////////////////////////////////////////////////////////

#include "TTLPCH.h"

#include "UseCaseFindMatch.hpp"
#include "UseCaseDisplayDialog.hpp"
#include "UseCaseJoinMatch.hpp"
#include "frontend/frontend.h"
#include "Network/OnlineSessionSearch.h"
#include "main/Lobby.h"
#include "../screens/screen_wait.h"
#include "../screens/screen_MultiplayerGameSettings.h"
#include "../screens/screen_SearchResult.h"
#include "environ/ConfigMgr.h"
#include "UseCaseUtility.hpp"
#include "UseCaseWaitForOSIRequest.hpp"
#include "GameHelper/DebugOutput.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Constructor: Initialize member variables
///////////////////////////////////////////////////////////////////////////////////////////////////
OnlineUI::FindMatch::FindMatch() : 
	d_messageOwner("UseCaseFindMatch"),
	_refresh( false ),
    _stateMachine(LISTEN_FOR_MESSAGES),
	_cleanUpScreens( true )
{
    _stateMachine.setStateUpdate(LISTEN_FOR_MESSAGES, &OnlineUI::FindMatch::_updateListenForMessages);

	_stateMachine.setStateEnter(FIND_MATCHES, &OnlineUI::FindMatch::_enterFindMatches);
	_stateMachine.setStateUpdate(FIND_MATCHES, &OnlineUI::FindMatch::_updateFindMatches);
    
	_stateMachine.setStateUpdate(JOIN_SELECTED_MATCH, &OnlineUI::FindMatch::_updateJoinSelectedMatch );

    _stateMachine.setStateUpdate(SHUT_DOWN, &OnlineUI::FindMatch::_updateShutDown);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called before use case is started.  Show the first screen for the use case.
///
/// \return true if succeeded, false otherwise
///////////////////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ bool OnlineUI::FindMatch::_init( void )
{
	// initialize
	g_FrontEnd.GotoScreen("SCREEN_ID_SEARCH_5_11");
	_stateMachine.setCurrentState(this, LISTEN_FOR_MESSAGES);
	_mapNamePtr		= 0;
    _gameTypePtr	= 0;
	_cleanUpScreens	= true ;

    // Add values to the settings screen
    _createGameSettings();

    // Catch messages from screens
    _configureMatchHandler.Initialize("SearchForMatches", MESSAGE_HANDLE_IF_MASTER, d_messageOwner, this);
    _cancelConfigHandler.Initialize("CancelConfigMatch", MESSAGE_HANDLE_IF_MASTER, d_messageOwner, this);
    _cancelResultsHandler.Initialize("CancelSearchResults", MESSAGE_HANDLE_IF_MASTER, d_messageOwner, this);
    _sessionSelectHandler.Initialize("SessionSelected", MESSAGE_HANDLE_IF_MASTER, d_messageOwner, this);

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called after use case is ended.  Clean up the screens and other objects.
///
/// \return true if succeeded, false otherwise
///////////////////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ bool OnlineUI::FindMatch::_deinit( void )
{
    d_messageOwner.UnregisterAll();

	g_FrontEnd.RemoveAllPopups();
	//if( _cleanUpScreens )
	{
		if( UseCase::IsScreenActive< ScreenSearchResult >() )
		{
			g_FrontEnd.EndScreen(0);
		}
		g_FrontEnd.PopScreenHistory( "SCREEN_ID_SEARCHRESULT" );

		if( UseCase::IsScreenActive< ScreenMultiplayerGameSettings >() )
		{
			g_FrontEnd.EndScreen(0);
		}
		g_FrontEnd.PopScreenHistory( "SCREEN_ID_SEARCH_5_11" );
	}

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when we're in the LISTEN_FOR_MESSAGES state
///
/// \return true to continue use case processing
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::FindMatch::_updateListenForMessages()
{
    d_messageOwner.HandlePostedMessages();

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called each time the FIND_MATCHES state is entered.
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::FindMatch::_enterFindMatches()
{
	unsigned int flags = OSIMatchOptions::MATCHOPTIONFLAG_QUICKMATCH_MASK; // everything wildcard
	
	// see if map name and index should be wildcard or specific
	const char *name = _mapNamePtr->getStringValue();
	if ( name && (!strcmp(name, "Any")) )
	{
		OSIstrcpy(_matchOptions.levelName, "Any" );
	}
	else
	{
		flags &= ~(OSIMatchOptions::MATCHOPTIONFLAG_LEVELNAME | OSIMatchOptions::MATCHOPTIONFLAG_LEVELINDEX);
		_matchOptions.levelIndex = _mapNamePtr->getIndexForValue( name ) - 1; // subtract 1 because the list starts with "Any"
		if( _matchOptions.levelIndex >= 0 )
		{
			_matchOptions.levelIndex = g_gameEnvironment.GetByType( _matchOptions.levelIndex , CGameEnv::TYPE_MULTIPLAYER );
		}
		OSIstrcpy(_matchOptions.levelName, name );
	}
		
	// see if gametype should be wildcard of specific
	int type = _gameTypePtr->getIntegerValue();
	if ( type == -1 )
	{
		_matchOptions.gameTypeIndex = -1;
	}
	else
	{
		flags &= ~OSIMatchOptions::MATCHOPTIONFLAG_GAMETYPE;
		_matchOptions.gameTypeIndex = type;
	}

	// set the wildcard flags and start the match list request
	_matchOptions.setWildcardFlags( flags );

	// Request our match list from OSI that match our search options
	bool result = OSI::getSingletonPtr()->requestMatchList( _matchOptions );

	WaitForOSIRequest::getSingletonPtr()->start( result, "Find Match", "Please wait, finding matches..." );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when we're in the FIND_MATCHES state
///
/// \return true to continue use case processing
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::FindMatch::_updateFindMatches()
{
	int size = 0;
	OSIRequestStatus requestStatus = OSI::getSingletonPtr()->getLastRequestStatus();
	if ( requestStatus == STATUS_SUCCEEDED )
	{
		// We have found a set of matches that match our search criteria.
        // Display the search results screen and listen to messages from the screen.
		if( !_refresh )
		{
			g_FrontEnd.GotoScreen("SCREEN_ID_SEARCHRESULT");
		}
		_stateMachine.setCurrentState(this, LISTEN_FOR_MESSAGES);

		_matchList = &OSI::getSingletonPtr()->getMatchList();
		size = _matchList->count();

        // Pump matches into search object
        COnlineSessionSearch &search = (COnlineSessionSearch &)g_lobby.SessionSearch();

		printf("-- OSI -- Sending %d match resuls to the results table.\n",_matchList->count());
		search.End(); // reset from previous searches
        search.SetResults(*_matchList);
		// Delete all the items from the search result table on the screen.
		ScreenSearchResult* pScreen = UseCase::GetActiveScreen< ScreenSearchResult >();
		if( pScreen )
		{
			GuiTable* pTable = pScreen->GetTable();
			if( pTable )
			{
				pTable->DeleteAllItems();
			}
		}
	}
	else
	{	
        stop();
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when we're in the JOIN_SELECTED_MATCH state
////////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::FindMatch::_updateJoinSelectedMatch( void )
{
	const OSIMatch*	match	= (*_matchList)[ _nSession ];
	OSIMatchID		matchID	= match->id ;
	
	_cleanUpScreens = false;
	stop();

	g_debugOutput.Print(g_debugOutput.Category("NET"), "FindMatch: Join '%s'", match->options.description);

	JoinMatch::getSingletonPtr()->setMatchToJoin( matchID );
	JoinMatch::getSingletonPtr()->start();
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called by the SessionSelect message handler to join the given match.
///
/// \param matchIndex Offset of the match to join.
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::FindMatch::joinMatch( int matchIndex )
{
	_nSession				= matchIndex ;

	const OSIMatch*	match	= (*_matchList)[ _nSession ];
	if( match->ping != 9999 )
	{
		_stateMachine.setCurrentState( this, JOIN_SELECTED_MATCH );
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Stop the use case.  Used by the CancelConfigMatch message handler, since it can't stop
/// the use case itself.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::FindMatch::_updateShutDown()
{
    stop();
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Callback for when the SearchForMatches message is receieved.  Switch into FIND_MATCHES state.
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::FindMatch::ConfigureMatch::HandleMessage( void* i_data, ActorHandle i_sender, bool i_posted )
{
	FindMatch::getSingletonPtr()->_refresh = i_data == 0 ? false : true;
	FindMatch::getSingletonPtr()->_stateMachine.setCurrentState(FindMatch::getSingletonPtr(), FIND_MATCHES);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Callback for when the CancelConfigMatch message is receieved.  Stop the find match use case.
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::FindMatch::CancelConfig::HandleMessage( void* i_data, ActorHandle i_sender, bool i_posted )
{
	// This message is only handled from the MP Game Settings screen.
	FindMatch::getSingletonPtr()->_stateMachine.setCurrentState(FindMatch::getSingletonPtr(), SHUT_DOWN);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Callback for when the CancelSearchResults message is receieved.  Go back a screen.
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::FindMatch::CancelResults::HandleMessage( void* i_data, ActorHandle i_sender, bool i_posted )
{
	// This message is only handled when the search results screen is active.
	g_FrontEnd.EndScreen(0); // Kill search results screen
	// Go back to listening to messages for the MP Game Settings screen
    FindMatch::getSingletonPtr()->_stateMachine.setCurrentState(FindMatch::getSingletonPtr(), LISTEN_FOR_MESSAGES);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Callback for when the SessionSelect message is receieved.
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::FindMatch::SessionSelect::HandleMessage( void* i_data, ActorHandle i_sender, bool i_posted )
{
    FindMatch::getSingletonPtr()->joinMatch( int(i_data) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Create the game settings and give them to the screen
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::FindMatch::_createGameSettings()
{
    int iMap, numMaps;
    int iMode;
    ScreenMultiplayerGameSettings *pScreen = UseCase::GetActiveScreen<ScreenMultiplayerGameSettings>();
	if ( !pScreen )
	{
		stop();
		return;
	}

	const int levelIndex = g_configmgr.GetOptionIndexByName("gameMap");

    // Clear any existing options
    pScreen->ClearConfig();

	g_configmgr.Reset(false);

    // Create the map type.  NOTE: _mapNamePtr is deleted by the screen.
    numMaps = g_gameEnvironment.Environments(CGameEnv::TYPE_MULTIPLAYER);
    _mapNamePtr = new StringType("Map Name");
	_mapNamePtr->addStringValue( "Any", "Find matches with any map being played." );
    for (iMap = 0; iMap < numMaps; ++iMap)
    {
        int index = g_gameEnvironment.GetByType(iMap, CGameEnv::TYPE_MULTIPLAYER);
        _mapNamePtr->addStringValue(g_gameEnvironment.GetWorldDisplayName(index), g_gameEnvironment.GetWorldDescription(index));
    }

    pScreen->AddConfigObject(_mapNamePtr);

    const int modeIndex = g_configmgr.GetOptionIndexByName("gameModes");

    // Create the game type.  NOTE: _gameTypePtr is deleted by the screen.
    _gameTypePtr = new EnumType("Gametype");
	_gameTypePtr->addValue( -1, "Any", "Find matches with any game type selected." );

    if (modeIndex >= 0)
    {
		 char param1[50];
		 iMode = 0;

		 const char* modeString = g_configmgr.GetValue(modeIndex);
		 strncpy(param1, modeString, sizeof(param1));
		 do
		 {
			 _gameTypePtr->addValue(iMode++, modeString, g_configmgr.GetDisplayDescription(modeIndex));
			 g_configmgr.Increment(modeIndex);
			 modeString = g_configmgr.GetValue(modeIndex);		 
		 }
		 while (strcmp(modeString, param1) != 0);
    }
    pScreen->AddConfigObject(_gameTypePtr);
	//pScreen->SetToAny();
}
