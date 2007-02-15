
#include "pch.hpp"
#include "OSIXbox.hpp"
#include "platform/xbox/xboxlive.h"
#include "OSIXboxPlayersListItem.hpp"

///------------------------------------------------------------------------------------------------
//! \brief Find out which game a friend is in and put it in the match list.
//! \return True if the *request* (not the thing being requested) was successfully created.
//! \param id The ID of the friend who's match should be retrieved.
///------------------------------------------------------------------------------------------------
bool OSIXbox::requestMatchForFriend( OSIPlayerID& id )
///------------------------------------------------------------------------------------------------
{
#ifdef ENABLE_OSI_XBOX_LOG
	OSIXbox::Log( "### OSIXbox::requestMatchForFriend: id = 0x%016I64X+%08X\n", id.xBox.userId, id.xBox.userFlags );
#endif

	if( !isOkToRequest() ) { return false; }

	ASSERT( REQUESTTYPE_NONE == _currentRequestType );

	if( !id.isValid() )
	{
		_setError( E_FAIL ); 
		setLastRequestStatus( STATUS_FAILED );
		return false;
	}

	_matchForFriendID = id;

	// get the friend list from uix
	const HRESULT getHR = _uix->GetFriendsForUser( 0, &_uixFriendsList );
	if( FAILED( getHR ) )
	{
		_setError( getHR );
		setLastRequestStatus( STATUS_FAILED );
		_setCurrentRequestType( REQUESTTYPE_NONE );
		return REQUEST_SUCCEEDED;
	}
	else
	{
		_setCurrentRequestType( REQUESTTYPE_MATCH_FOR_FRIEND );
		_requestState = REQUESTSTATE_MATCHFORFRIEND_WAIT_FOR_FRIEND_LIST;
	}

	return REQUEST_SUCCEEDED;
}

///------------------------------------------------------------------------------------------------
bool OSIXbox::playerJoinCurrentMatch( const OSIchar* playerName, OSIPlayerID& id, int numPlayersInMatch )
///------------------------------------------------------------------------------------------------
{
#ifdef ENABLE_OSI_XBOX_LOG
	OSIXbox::Log( "### OSIXbox::playerJoinCurrentMatch: playerName = %s, id = 0x%016I64X+%08X, numPlayersInMatch = %u\n", 
		playerName, id.xBox.userId, id.xBox.userFlags, numPlayersInMatch );
#endif

	if( !isOkToRequest() ) { return false; }

	ASSERT( playerName && playerName[0] );
	ASSERT( id.xBox.userId != 0 );

	if( !id.isValid() || !playerName || !playerName[0] )
	{
		return false;
	}

	OSIPlayerID_to_XUID( id, _requestPlayerXUID );

	if( getActivePlayer()->id != id )
	{
		ILivePlayersList* uixPlayersList;
		_uix->GetFeatureInterface( UIX_PLAYERS_FEATURE, 0, (void**)&uixPlayersList );
		if( uixPlayersList )
		{
			OSIXboxPlayersListItem* newPlayer = new OSIXboxPlayersListItem( _requestPlayerXUID, playerName );
			_remotePlayersList.push_back( newPlayer );
			uixPlayersList->RegisterPlayer( newPlayer );
		}

		// Add this player to the current player list
		OSIPlayer joiningPlayer;
		joiningPlayer.id = id;
		OSIstrcpy( joiningPlayer.name, playerName );
		_addCurrentPlayer( joiningPlayer );
	}

	// update matchmaking info
	_currentMatch.numberOfPlayers = numPlayersInMatch;
	_currentMatch.options.totalConnectedPlayers = numPlayersInMatch;

	// update player state for friends
	if( OSIUpdatePlayerStateForMatch( getActivePlayer()->state, _currentMatch ) )
	{
		_setNotificationState( getActivePlayer()->state );
	}

	if( _hostedSession.Exists() )
	{
		_setHostedSessionOptions( _currentMatch.options );
		_hostedSession.Update();
	}
	
	return true;
}

///------------------------------------------------------------------------------------------------
bool OSIXbox::playerLeaveCurrentMatch( const OSIchar* playerName, OSIPlayerID& id, int numPlayersInMatch )
///------------------------------------------------------------------------------------------------
{
#ifdef ENABLE_OSI_XBOX_LOG
	OSIXbox::Log( "### OSIXbox::playerLeaveCurrentMatch: playerName = %s, id = 0x%016I64X+%08X, numPlayersInMatch = %u\n", 
		playerName, id.xBox.userId, id.xBox.userFlags, numPlayersInMatch );
#endif
	
	//	if( !isOkToRequest() ) { return false; }

	ASSERT( playerName && playerName[0] );
	ASSERT( id.xBox.userId != 0 );

	bool selfIsLeaving = (getActivePlayer()->id == id);

	if( !selfIsLeaving )
	{
		OSIPlayer leavingPlayer;
		leavingPlayer.id = id;
		OSIstrncpy( leavingPlayer.name, playerName, NUM_ARRAY_ELEMENTS( leavingPlayer.name ) );

		// update OSI current/recent player lists
		_removeCurrentPlayer( leavingPlayer );
		_addRecentPlayer( leavingPlayer );

		// update UIX player lists
		ILivePlayersList* uixPlayersList;
		_uix->GetFeatureInterface( UIX_PLAYERS_FEATURE, 0, (void**)&uixPlayersList );
		if( uixPlayersList )
		{
			XUID xuid;
			OSIPlayerID_to_XUID( id, xuid );
			
			_remotePlayersList.begin(); 
			for( OSIu32 i = 0; i < _remotePlayersList.count(); ++i, _remotePlayersList.next() )
			{
				const OSIXboxPlayersListItem* const currentPlayer = _remotePlayersList.getCurrent();
				if( currentPlayer->_xuid == xuid )
					break;
			}

			ASSERT( i <= _remotePlayersList.count() );
			if( i < _remotePlayersList.count() )
			{
				uixPlayersList->UnregisterPlayer( _remotePlayersList.getCurrent() );
				delete _remotePlayersList.getCurrent();
				_remotePlayersList.erase();
			}
		}
	}

	// update matchmaking info
	_currentMatch.numberOfPlayers = numPlayersInMatch;
	_currentMatch.options.totalConnectedPlayers = numPlayersInMatch;

	// update player state for friends
	if( OSIUpdatePlayerStateForMatch( getActivePlayer()->state, _currentMatch ) )
	{
		_setNotificationState( getActivePlayer()->state );
	}

	if( !selfIsLeaving && _hostedSession.Exists() )
	{
		_setHostedSessionOptions( _currentMatch.options );
		_hostedSession.Update();
	}
	return true;
}

///------------------------------------------------------------------------------------------------
//! \brief Attempts to create a hosted session and advertise it on Live!.
//! \param options The type of match to host.  Tells the matchmaking service what to advertise.
//! \return True if the *request* (not the thing being requested) was successfully created.
///------------------------------------------------------------------------------------------------
bool OSIXbox::requestHostMatch( OSIMatchOptions& options )
///------------------------------------------------------------------------------------------------
{
#ifdef ENABLE_OSI_XBOX_LOG
	OSIXbox::Log( "### OSIXbox::requestHostMatch\n" );
#endif

	if( !isOkToRequest() ) { return false; }

	ASSERT( REQUESTTYPE_NONE == _currentRequestType );

	options.totalConnectedPlayers = 1;
	_setHostedSessionOptions( options );

	const HRESULT createHR = _hostedSession.Create();
	if( FAILED( createHR ) )
	{
		setLastRequestStatus( STATUS_FAILED );
		_setError( createHR );
		_setCurrentRequestType( REQUESTTYPE_NONE );
		return REQUEST_FAILED;
	}
	else
	{
		// Set currentMatch to reflect what's being created
		_currentMatch.hostURLs.clear();
		XNKID_to_OSIMatchID( _hostedSession.SessionID, _currentMatch.id );
		
		//OSIstrncpy( _currentMatch.options.description, _hostedSession.GetHostTitle(), NUM_ARRAY_ELEMENTS( _currentMatch.options.description ) );
		_currentMatch.maximumPlayers = options.maxPlayers;
		_currentMatch.numberOfPlayers = options.totalConnectedPlayers;
		_currentMatch.options = options;
		_currentMatch.ping = 0;

		// transition to state that will get the game list
		_setCurrentRequestType( REQUESTTYPE_HOST_MATCH );
		return REQUEST_SUCCEEDED;
	}
}

///------------------------------------------------------------------------------------------------
//! \brief Updates matchmaking data to reflect that this xbox/player is connected to the specified match.
//! \return True if the *request* (not the thing being requested) was successfully created.
//! \param matchID The ID of the match to join.
///------------------------------------------------------------------------------------------------
bool OSIXbox::requestJoinMatch( OSIMatchID& matchID )
///------------------------------------------------------------------------------------------------
{
#ifdef ENABLE_OSI_XBOX_LOG
	OSIXbox::Log( "### OSIXbox::requestJoinMatch: matchID = 0x%I64X\n", *((ULONGLONG*)(matchID.xBox)));
#endif

	if( !isOkToRequest() ) { return false; }

	ASSERT( REQUESTTYPE_NONE == _currentRequestType );
	ASSERT( REQUESTSTATE_NONE == _requestState );
	XNKID nkid;
	OSIMatchID_to_XNKID( matchID, nkid );

	const HRESULT findHR = _specificMatchQuery.Query( *((LONGLONG*)(nkid.ab)) );
	if( FAILED( findHR ) )
	{
		_setError( findHR );
		_setCurrentRequestType( REQUESTTYPE_NONE );
		setLastRequestStatus( STATUS_FAILED );
		return REQUEST_FAILED;
	}
	else
	{
		_setCurrentRequestType( REQUESTTYPE_JOIN_MATCH );
		_requestState = REQUESTSTATE_JOINMATCH_RESOLVE_MATCH_ID;
		setLastRequestStatus( STATUS_WORKING );
		return REQUEST_SUCCEEDED;
	}
}

///------------------------------------------------------------------------------------------------
bool OSIXbox::requestUpdateMatchOptions( OSIMatchOptions& options )
///------------------------------------------------------------------------------------------------
{
#ifdef ENABLE_OSI_XBOX_LOG
	OSIXbox::Log( "### OSIXbox::requestUpdateMatchOptions\n" );
#endif

	if( !isOkToRequest() ) { return false; }

	ASSERT( REQUESTTYPE_NONE == _currentRequestType );

	// update player state for friends
	_currentMatch.options = options;
	_currentMatch.maximumPlayers = _currentMatch.options.maxPlayers;
	_currentMatch.numberOfPlayers = _currentMatch.options.totalConnectedPlayers;
	
	if( OSIUpdatePlayerStateForMatch( getActivePlayer()->state, _currentMatch ) )
	{
		_setNotificationState( getActivePlayer()->state );
	}
	
	if( !_hostedSession.Exists() )
	{
		return REQUEST_SUCCEEDED;
	}

	_setHostedSessionOptions( options );
	_hostedSession.Update();

	setLastRequestStatus( STATUS_SUCCEEDED );
	_setCurrentRequestType( REQUESTTYPE_NONE );
	return REQUEST_SUCCEEDED;
}

///------------------------------------------------------------------------------------------------
//! \brief Updates matchmaking data to reflect that this xbox/player is no longer connected to the current match.
//! \return True if the *request* (not the thing being requested) was successfully created.
///------------------------------------------------------------------------------------------------
bool OSIXbox::requestQuitMatch()
///------------------------------------------------------------------------------------------------
{
#ifdef ENABLE_OSI_XBOX_LOG
	OSIXbox::Log( "### OSIXbox::requestQuitMatch\n" );
#endif

	if( !isOkToRequest() ) { return false; }

	ASSERT( REQUESTTYPE_NONE == _currentRequestType );

	// HACK fix - if the current player list is not empty, just dump them all into the recent player list
	while( _currentPlayerList.count() > 0 )
	{
		_currentPlayerList.begin();
		OSIPlayer& p = *(_currentPlayerList.getCurrent());
		_addRecentPlayer( p );
		_removeCurrentPlayer( p );
	}

	ILivePlayersList* uixPlayersList;
	_uix->GetFeatureInterface( UIX_PLAYERS_FEATURE, 0, (void**)&uixPlayersList );
	while( uixPlayersList && _remotePlayersList.count() > 0 )
	{
		_remotePlayersList.begin();
		OSIXboxPlayersListItem* p = _remotePlayersList.getCurrent();
		uixPlayersList->UnregisterPlayer( p );
		delete p;
		_remotePlayersList.erase();
	}

	OSIClearMatch( _currentMatch );
	
	if( OSIUpdatePlayerStateForMatch( getActivePlayer()->state, _currentMatch ) )
	{
		_setNotificationState( getActivePlayer()->state );
	}

	if( _hostedSession.Exists() )
	{
		_hostedSession.Delete();
		setLastRequestStatus( STATUS_WORKING );
		_setCurrentRequestType( REQUESTTYPE_QUIT_HOSTED_MATCH );
	}
	else
	{
		setLastRequestStatus( STATUS_SUCCEEDED );
		_setCurrentRequestType( REQUESTTYPE_NONE );
	}
	return REQUEST_SUCCEEDED;
}


///------------------------------------------------------------------------------------------------
void OSIXbox::_processQuitHostedMatch()
///------------------------------------------------------------------------------------------------
{
	if( _hostedSession.IsDeleting() )
	{
		_hostedSession.Process();	
		setLastRequestStatus( STATUS_WORKING );
	}
	else
	{
		_setCurrentRequestType( REQUESTTYPE_NONE );
		_hostedSession.Reset();
		setLastRequestStatus( STATUS_SUCCEEDED );
	}
}


///------------------------------------------------------------------------------------------------
//! \brief Begins enumeration of advertised sessions that have a free public slot and fulfill the options.
//! \param options Describes the kind of match to search for.
//! \return True if the *request* (not the thing being requested) was successfully created.
///------------------------------------------------------------------------------------------------
bool OSIXbox::requestMatchList( OSIMatchOptions& options )
///------------------------------------------------------------------------------------------------
{
#ifdef ENABLE_OSI_XBOX_LOG
	OSIXbox::Log( "### OSIXbox::requestMatchList\n" );
#endif

	if( !isOkToRequest() ) { return false; }

	ASSERT( REQUESTTYPE_NONE == _currentRequestType );
	const unsigned int wildcards = options.getWildcardFlags();
	
	const ULONGLONG ArcadeDamage		= X_MATCH_NULL_INTEGER;//MatchOptionOrWildcardValue( wildcards, OSIMatchOptions::MATCHOPTIONFLAG_ARCADEDAMAGE,		options.arcadeDamage );

    const ULONGLONG ConsoleRegion			= XGetGameRegion();
    const ULONGLONG GameVersion				= 1; //FIXME: determine game version
    const ULONGLONG AdditionalContent		= 1; //FIXME: determine game additional content id
    const ULONGLONG LevelIndex			= MatchOptionOrWildcardValue( wildcards, OSIMatchOptions::MATCHOPTIONFLAG_LEVELINDEX,		options.levelIndex );
    const ULONGLONG GameType 			= MatchOptionOrWildcardValue( wildcards, OSIMatchOptions::MATCHOPTIONFLAG_GAMETYPE,			options.gameTypeIndex );
    const ULONGLONG TimeLimit			= MatchOptionOrWildcardValue( wildcards, OSIMatchOptions::MATCHOPTIONFLAG_TIMELIMIT,		options.timeLimit );
    const ULONGLONG ScoreLimit			= MatchOptionOrWildcardValue( wildcards, OSIMatchOptions::MATCHOPTIONFLAG_SCORELIMIT,		options.scoreLimit );
    const ULONGLONG TagLimit			= MatchOptionOrWildcardValue( wildcards, OSIMatchOptions::MATCHOPTIONFLAG_TAGLIMIT,			options.tagLimit );
    const ULONGLONG BestOfSeries 		= MatchOptionOrWildcardValue( wildcards, OSIMatchOptions::MATCHOPTIONFLAG_BESTOFSERIES,		options.bestOfSeries );
    const ULONGLONG PlayersRespawn		= MatchOptionOrWildcardValue( wildcards, OSIMatchOptions::MATCHOPTIONFLAG_PLAYERSRESPAWN,	options.playersRespawn );
    const ULONGLONG FriendlyFire 		= MatchOptionOrWildcardValue( wildcards, OSIMatchOptions::MATCHOPTIONFLAG_FRIENDLYFIRE,		options.friendlyFire );
    const ULONGLONG Apprehension		= MatchOptionOrWildcardValue( wildcards, OSIMatchOptions::MATCHOPTIONFLAG_APPREHENSION,		options.apprehension );
    const ULONGLONG SelfDamage			= MatchOptionOrWildcardValue( wildcards, OSIMatchOptions::MATCHOPTIONFLAG_SELFDAMAGE,		options.selfDamage );
    const ULONGLONG AlternatingTeams	= MatchOptionOrWildcardValue( wildcards, OSIMatchOptions::MATCHOPTIONFLAG_ALTERNATINGTEAMS, options.alternatingTeams );
    const ULONGLONG MaxPlayers			= MatchOptionOrWildcardValue( wildcards, OSIMatchOptions::MATCHOPTIONFLAG_MAXPLAYERS,		options.maxPlayers );
    const ULONGLONG IsRanked			= MatchOptionOrWildcardValue( wildcards, OSIMatchOptions::MATCHOPTIONFLAG_ISRANKED,			options.isRanked );
    const ULONGLONG TotalConnectedPlayers	= X_MATCH_NULL_INTEGER; //fixme: what?
    const ULONGLONG LoadOutLimit			= X_MATCH_NULL_INTEGER;
    const ULONGLONG VoiceCount				= X_MATCH_NULL_INTEGER;

	const HRESULT queryHR = _commonMatchQuery.Query(
		ConsoleRegion,			GameVersion,			AdditionalContent,		LevelIndex,
		GameType,				TimeLimit,				ScoreLimit,				TagLimit,
		BestOfSeries,			PlayersRespawn,			FriendlyFire,			Apprehension,
		SelfDamage,				AlternatingTeams,		MaxPlayers,				IsRanked,
		TotalConnectedPlayers,	LoadOutLimit,			VoiceCount );

	if( FAILED( queryHR ) )
	{
		// transition to back to nothing happening
		_setError( queryHR );
		_setCurrentRequestType( REQUESTTYPE_NONE );
		_requestState = REQUESTSTATE_NONE;
		setLastRequestStatus( STATUS_FAILED );
		return REQUEST_FAILED;
	}
	else
	{
		// transition to state that will continue pumping the query
		_setCurrentRequestType( REQUESTTYPE_COMMON_MATCH_QUERY );
		_requestState = REQUESTSTATE_FINDMATCH_GET_LIST;
		setLastRequestStatus( STATUS_WORKING );
		return REQUEST_SUCCEEDED;
	}
}

///------------------------------------------------------------------------------------------------
void OSIXbox::_processMatchForFriend()
///------------------------------------------------------------------------------------------------
{
	ASSERT( REQUESTTYPE_MATCH_FOR_FRIEND == _currentRequestType );

	switch( _requestState )
	{
	case REQUESTSTATE_MATCHFORFRIEND_WAIT_FOR_FRIEND_LIST:		_processMatchForFriendWaitForFriendList();		break;
	case REQUESTSTATE_MATCHFORFRIEND_WAIT_FOR_FRIEND_SEARCH:	_processMatchForFriendWaitForFriendSearch();	break;
	default: ASSERT( 0 ); break;
	}
}

///------------------------------------------------------------------------------------------------
void OSIXbox::_processMatchForFriendWaitForFriendList()
///------------------------------------------------------------------------------------------------
{
	if( !_uixFriendsList->IsReady() )
	{
		setLastRequestStatus( STATUS_WORKING );
		return;
	}

	XUID xuid;
	OSIPlayerID_to_XUID( _matchForFriendID, xuid );

	// find the friend in the friend list
	XONLINE_FRIEND f;
	const HRESULT findFriendHR = _uixFriendsList->GetFriendByXUID( xuid, &f );
	if( FAILED( findFriendHR ) )
	{
		_setError( findFriendHR );
		_uixFriendsList->Release();
		setLastRequestStatus( STATUS_FAILED );
		_setCurrentRequestType( REQUESTTYPE_NONE );
		return;
	}
	_uixFriendsList->Release();
	_uixFriendsList = 0;

	// start search for the match by id
	//const HRESULT findMatchHR = XOnlineMatchSessionFindFromID( f.sessionID, NULL, &_currentOnlineTaskHandle );
	const HRESULT queryHR = _specificMatchQuery.Query( *((LONGLONG*)(&f.sessionID.ab)) );
	if( FAILED( queryHR ) )
	{
		_setError( queryHR );
		setLastRequestStatus( STATUS_FAILED );
		_requestState = REQUESTSTATE_NONE;
		_setCurrentRequestType( REQUESTTYPE_NONE );
	}
	else
	{
		setLastRequestStatus( STATUS_WORKING );
		_requestState = REQUESTSTATE_MATCHFORFRIEND_WAIT_FOR_FRIEND_SEARCH;
	}
}

///------------------------------------------------------------------------------------------------
void OSIXbox::_processMatchForFriendWaitForFriendSearch()
///------------------------------------------------------------------------------------------------
{
	//ASSERT( _currentOnlineTaskHandle );
	//const HRESULT continueHR = XOnlineTaskContinue( _currentOnlineTaskHandle );

	const HRESULT processHR = _specificMatchQuery.Process();
	if( FAILED( processHR ) )
	{
		_requestState = REQUESTSTATE_NONE;
		_setCurrentRequestType( REQUESTTYPE_NONE );
		_setError( processHR );
		setLastRequestStatus( STATUS_FAILED );
	}
	else if( XONLINETASK_S_SUCCESS == processHR )
	{
		_requestState = REQUESTSTATE_NONE;
		_setCurrentRequestType( REQUESTTYPE_NONE );
		setLastRequestStatus( STATUS_SUCCEEDED );
		_matchList.setNetworkDataReady( true );
	}
	else // not finished
	{
		setLastRequestStatus( STATUS_WORKING );
	}
}

///------------------------------------------------------------------------------------------------
void OSIXbox::_processJoinMatch()
///------------------------------------------------------------------------------------------------
{
	ASSERT( REQUESTTYPE_JOIN_MATCH == _currentRequestType );
	ASSERT( _requestState != REQUESTSTATE_NONE );
	//ASSERT( _currentOnlineTaskHandle );

	switch( _requestState )
	{
	case REQUESTSTATE_JOINMATCH_RESOLVE_MATCH_ID:	_processJoinMatchResolveMatchID(); break;
	default: ASSERT( 0 );
	};
}

///------------------------------------------------------------------------------------------------
void OSIXbox::_processJoinMatchResolveMatchID()
///------------------------------------------------------------------------------------------------
{
	ASSERT( REQUESTTYPE_JOIN_MATCH == _currentRequestType );
	ASSERT( REQUESTSTATE_JOINMATCH_RESOLVE_MATCH_ID == _requestState );

	const HRESULT queryHR = _specificMatchQuery.Process();
	if( FAILED( queryHR ) )
	{
		_setCurrentRequestType( REQUESTTYPE_NONE );
		_setError( queryHR );
		setLastRequestStatus( STATUS_FAILED );
		_requestState = REQUESTSTATE_NONE;
	}
	else if( XONLINETASK_S_SUCCESS == queryHR )
	{
		// the request type has to be set to NONE here because 
		// _populateMatchList uses _lastRequestType to determine where
		// to get search results from.
		_requestState = REQUESTSTATE_NONE;
		_setCurrentRequestType( REQUESTTYPE_NONE );

		_populateMatchList();
		_matchList.begin();
		OSIMatch* match = _matchList.getCurrent();

		if( match && (match->numberOfPlayers != match->maximumPlayers) )
		{
			setLastRequestStatus( STATUS_SUCCEEDED );
			_currentMatch = *match;

			// update player state for friends
			if( OSIUpdatePlayerStateForMatch( getActivePlayer()->state, _currentMatch ) )
			{
				_setNotificationState( getActivePlayer()->state );
			}
		}
		else
		{
			setLastRequestStatus( STATUS_FAILED );
			_setError( XONLINE_E_MATCH_INVALID_SESSION_ID );
		}
	}
	else // not finished
	{
		setLastRequestStatus( STATUS_WORKING );
	}
}

///------------------------------------------------------------------------------------------------
void OSIXbox::_processCommonMatchQuery()
///------------------------------------------------------------------------------------------------
{
	ASSERT( REQUESTTYPE_COMMON_MATCH_QUERY == _currentRequestType );
	ASSERT( _requestState != REQUESTSTATE_NONE );

	// process results
	switch( _requestState )
	{
	case REQUESTSTATE_FINDMATCH_GET_LIST:	_processCommonMatchQueryGetList(); break;
	case REQUESTSTATE_FINDMATCH_QOS_PROBE:	_processCommonMatchQueryQosProbe(); break;
	default: ASSERT( 0 );
	};
}

///------------------------------------------------------------------------------------------------
void OSIXbox::_processCommonMatchQueryGetList()
///------------------------------------------------------------------------------------------------
{
	ASSERT( REQUESTTYPE_COMMON_MATCH_QUERY == _currentRequestType );
	ASSERT( REQUESTSTATE_FINDMATCH_GET_LIST == _requestState );

	const HRESULT processHR = _commonMatchQuery.Process();
	if( FAILED( processHR ) )
	{
		_setError( processHR );
		setLastRequestStatus( STATUS_FAILED );
		_setCurrentRequestType( REQUESTTYPE_NONE );
		_requestState = REQUESTSTATE_NONE;
	}
	else if( XONLINETASK_S_SUCCESS == processHR )
	{
		// transition to state that will start pinging hosts
		const HRESULT probeHR = _commonMatchQuery.Probe();
		if( FAILED( probeHR ) )
		{
			_setError( probeHR );
			setLastRequestStatus( STATUS_FAILED );
			_setCurrentRequestType( REQUESTTYPE_NONE );
			_requestState = REQUESTSTATE_NONE;
		}
		else // switch into qos_probe state
		{
			setLastRequestStatus( STATUS_WORKING );
			_requestState = REQUESTSTATE_FINDMATCH_QOS_PROBE;
		}
	}
	else // not finished
	{
		setLastRequestStatus( STATUS_WORKING );
	}
}

///------------------------------------------------------------------------------------------------
void OSIXbox::_processCommonMatchQueryQosProbe()
///------------------------------------------------------------------------------------------------
{
	ASSERT( REQUESTTYPE_COMMON_MATCH_QUERY == _currentRequestType );
	ASSERT( REQUESTSTATE_FINDMATCH_QOS_PROBE == _requestState );

	const HRESULT processHR = _commonMatchQuery.Process();
	if( FAILED( processHR ) )
	{
		_setError( processHR );
		setLastRequestStatus( STATUS_FAILED );
		_setCurrentRequestType( REQUESTTYPE_NONE );
		_requestState = REQUESTSTATE_NONE;
	}
	else if( XONLINETASK_S_SUCCESS == processHR )
	{
		// list is complete, update the UI and transition back to nothing happening
		_requestState = REQUESTSTATE_NONE;
		_setCurrentRequestType( REQUESTTYPE_NONE );
		_matchList.setNetworkDataReady( true ); // let the matchlist know to update itself
		setLastRequestStatus( STATUS_SUCCEEDED );
	}
	else // not finished
	{
		setLastRequestStatus( STATUS_WORKING );
	}
}

///------------------------------------------------------------------------------------------------
void OSIXbox::_populateMatchList()
///------------------------------------------------------------------------------------------------
{
	if( REQUESTTYPE_COMMON_MATCH_QUERY == _lastRequestType )
	{
		_matchList.clear();
		const DWORD dwResults = _commonMatchQuery.Results.Size();
		for( DWORD i = 0; i < dwResults; ++i )
		{
			OSIMatch match;
			const CCommonMatchResult& result = _commonMatchQuery.Results[i];
			CCommonMatchSearchResult_to_OSIMatch( result, match );
			_matchList.push_back( match );
		}
	}
	else if( REQUESTTYPE_MATCH_FOR_FRIEND == _lastRequestType
		|| REQUESTTYPE_JOIN_MATCH == _lastRequestType )
	{
		_matchList.clear();
		if( _specificMatchQuery.Results.Size() == 0 ) { return; }
		OSIMatch match;
		const CSpecificMatchResult& result = _specificMatchQuery.Results[0];
		CSpecificMatchSearchResult_to_OSIMatch( result, match );
		_matchList.push_back( match );
	}
	else
	{
		ASSERT( !"trying to populate match list with no source" );
	}
}

///------------------------------------------------------------------------------------------------
void OSIXbox::_processHostMatch()
///------------------------------------------------------------------------------------------------
{
	ASSERT( REQUESTTYPE_HOST_MATCH == _currentRequestType );
	ASSERT( _hostedSession.Exists() || _hostedSession.IsCreating() );

	const HRESULT processHR = _hostedSession.Process();
	if( FAILED( processHR ) )
	{
		_setError( processHR );
		setLastRequestStatus( STATUS_FAILED );
	}
	else if( XONLINETASK_S_SUCCESS == processHR )
	{
		if( !_hostedSession.Exists() )
		{
			ASSERT( 0 ); // how could this happen?
			setLastRequestStatus( STATUS_FAILED );
			_setCurrentRequestType( REQUESTTYPE_NONE );
			return;
		}
		else
		{
			setLastRequestStatus( STATUS_SUCCEEDED );
			XNKID_to_OSIMatchID( _hostedSession.SessionID, _currentMatch.id );
			//FIXME; need to fill in hosturls?

			// update player state for friends
			if( OSIUpdatePlayerStateForMatch( getActivePlayer()->state, _currentMatch ) )
			{
				_setNotificationState( getActivePlayer()->state );
			}
		}
	}
	else
	{
		setLastRequestStatus( STATUS_WORKING );
		return;
	}

	_setCurrentRequestType( REQUESTTYPE_NONE );
}

///------------------------------------------------------------------------------------------------
bool OSIXbox::requestStationURLListForMatch( OSIMatchID& matchID )
///------------------------------------------------------------------------------------------------
{
#ifdef ENABLE_OSI_XBOX_LOG
	OSIXbox::Log( "### OSIXbox::requestStationURLListForMatch: matchID = 0x%I64X\n", *((ULONGLONG*)(matchID.xBox)));
#endif

	if( !isOkToRequest() ) { return false; }

	ASSERT( REQUESTTYPE_NONE == _currentRequestType );

	XNKID sessionID;
	OSIMatchID_to_XNKID( matchID, sessionID );

	_stationURLList.clear();

	const HRESULT queryHR = _specificMatchQuery.Query( *((LONGLONG*)(&sessionID.ab)) );
	if( FAILED( queryHR ) )
	{
		_setError( queryHR );
		setLastRequestStatus( STATUS_FAILED );
		_setCurrentRequestType( REQUESTTYPE_NONE );
	}
	else
	{
		setLastRequestStatus( STATUS_WORKING );
		_setCurrentRequestType( REQUESTTYPE_STATION_URL_LIST_FOR_MATCH );
	}
	return true;
}

///------------------------------------------------------------------------------------------------
void OSIXbox::_processStationURLListForMatch()
///------------------------------------------------------------------------------------------------
{
	const HRESULT processHR = _specificMatchQuery.Process();
	if( FAILED( processHR ) )
	{
		_requestState = REQUESTSTATE_NONE;
		_setCurrentRequestType( REQUESTTYPE_NONE );
		_setError( processHR );
		setLastRequestStatus( STATUS_FAILED );
	}
	else if( XONLINETASK_S_SUCCESS == processHR )
	{
		_requestState = REQUESTSTATE_NONE;
		_setCurrentRequestType( REQUESTTYPE_NONE );
		setLastRequestStatus( STATUS_SUCCEEDED );

		if( _specificMatchQuery.Results.Size() == 0 )
		{
			return;
		}

		ASSERT( _specificMatchQuery.Results.Size() == 1 );
		const DWORD dwResults = _specificMatchQuery.Results.Size();

		OSIMatch match;
		CSpecificMatchResult& result = _specificMatchQuery.Results[0];

		StationURL url;
		url.SetXBoxURL( &result.HostAddress, &result.SessionID, &result.KeyExchangeKey );
		_stationURLList.push_back( url.CStr() );

	}
	else // not finished
	{
		setLastRequestStatus( STATUS_WORKING );
	}
}

///------------------------------------------------------------------------------------------------
bool OSIXbox::requestHostMigration( OSIPlayerID& newHostID, int numPlayersInMatch )
///------------------------------------------------------------------------------------------------
{
#ifdef ENABLE_OSI_XBOX_LOG
	OSIXbox::Log( "### OSIXbox::requestHostMigration: newHostID = %016I64X+%08X, numPlayersInMatch = %u\n",
		newHostID.xBox.userId, newHostID.xBox.userFlags, numPlayersInMatch );
#endif

	if( !isOkToRequest() ) { return false; }

	ASSERT( REQUESTTYPE_NONE == _currentRequestType );
	ASSERT( !_currentOnlineTaskHandle );

	if( !newHostID.isValid() )
	{
		_setError( E_FAIL ); 
		setLastRequestStatus( STATUS_FAILED );
		return false;
	}


	// can't join me until migration is complete
	getActivePlayer()->state.isCurrentGameJoinable = false;
	_setNotificationState( getActivePlayer()->state );

	if( newHostID == getActivePlayer()->id )
	{
		// This machine is the new host - do a modified requestHostMatch
		// this is not the cleanest way to do this, but it gets the job done
		ASSERT( !_hostedSession.Exists() );

		// update the match variables
		OSIMatchOptions& options = getCurrentMatch()->options;
		OSIMatch* match = getCurrentMatch();
		options.totalConnectedPlayers = numPlayersInMatch;
		match->maximumPlayers = options.maxPlayers;
		match->numberOfPlayers = numPlayersInMatch;
		match->ping = 0;
		
		// build a HostURL for this machine and register it
		StationURL url;
		XNADDR xnaddr;
		XNetGetTitleXnAddr( &xnaddr );
		url.SetXBoxURL( &xnaddr, XBoxNetworking::GetSessionID(), XBoxNetworking::GetSessionKey() );
		getCurrentMatch()->hostURLs.clear();
		getCurrentMatch()->hostURLs.push_back( url.CStr() );
		//XBoxNetworking::RegisterSession( *XBoxNetworking::GetSessionID(), *XBoxNetworking::GetSessionKey() );

		// set up the matchmaking data
		_setHostedSessionOptions( options );

		const HRESULT createHR = _hostedSession.Create();
		if( FAILED( createHR ) )
		{
			setLastRequestStatus( STATUS_FAILED );
			_setError( createHR );
			_setCurrentRequestType( REQUESTTYPE_NONE );
			return REQUEST_FAILED;
		}
		else
		{
			_setCurrentRequestType( REQUESTTYPE_HOST_MATCH );
			return REQUEST_SUCCEEDED;
		}
	}
	else
	{
		// Someone else is the new host
		// just stay unjoinable until the new matchid is discovered
		// that will be handled in another request, where XBoxNetworking::RegisterSession and _setNotificationState will be updated
		return REQUEST_SUCCEEDED;
	}
}

///------------------------------------------------------------------------------------------------
bool OSIXbox::requestValidateMatchName( const OSIchar* matchName )
///------------------------------------------------------------------------------------------------
{
#ifdef ENABLE_OSI_XBOX_LOG
	OSIXbox::Log( "### OSIXbox::requestValidateMatchName: matchName = %s\n", matchName );
#endif

	if( !isOkToRequest() ) { return false; }

	ASSERT( REQUESTTYPE_NONE == _currentRequestType );
	ASSERT( !_currentOnlineTaskHandle );
	ASSERT( matchName );
	
	if( !matchName || !matchName[0] )
	{
		_setError( E_FAIL ); 
		setLastRequestStatus( STATUS_FAILED );
		return false;
	}

	// The string that comes in to this function comes from a call to g_sessionSpec.GetName().
    // That function returns CSessionSpecInstance::Entry(0).
	// That function returns CSessionSpecInstance::d_entry[i_index].
	// That array is sized by a template parameter that is hard-coded in the TTL class in TTL.h:
	//  CSessionSpecInstance<256, MAX_PLAYERS> d_sessionSpec;
	// I decided to just use 256 since I don't want to include all of that crap.
	WCHAR matchNameWideCopy[ 256 ]; 
	const WCHAR* matchNameWideCopyPtr = matchNameWideCopy;
	OSIstrncpy( matchNameWideCopy, matchName, NUM_ARRAY_ELEMENTS( matchNameWideCopy ) );
	HRESULT hr = XOnlineStringVerify( 
		1,
		&matchNameWideCopyPtr,
		CXboxLive::GetFilteredLanguage(),
		NULL,
		&_currentOnlineTaskHandle );
	if( FAILED( hr ) )
	{
		// something failed
		_setError( hr );
		setLastRequestStatus( STATUS_FAILED );
		return REQUEST_FAILED;
	}
	else
	{
		setLastRequestStatus( STATUS_WORKING );
		_setCurrentRequestType( REQUESTTYPE_VERIFY_STRING );
		return REQUEST_SUCCEEDED;
	}
}

///------------------------------------------------------------------------------------------------
void OSIXbox::_processVerifyString()
///------------------------------------------------------------------------------------------------
{
	ASSERT( REQUESTTYPE_VERIFY_STRING == _currentRequestType );
	ASSERT( _currentOnlineTaskHandle );

	XONLINETASK_HANDLE taskHandle = _currentOnlineTaskHandle;
	OSIRequestStatus status;

	const HRESULT continueHR = XOnlineTaskContinue( _currentOnlineTaskHandle );

	if( FAILED( continueHR ) || (S_FALSE == continueHR) )
	{
		status = STATUS_FAILED;
		_setCurrentRequestType( REQUESTTYPE_NONE );
		_setError( continueHR );
		_currentOnlineTaskHandle = 0;
		_requestState = REQUESTSTATE_NONE;
	}
	else if( (XONLINETASK_S_SUCCESS == continueHR) ) //|| (S_OK == continueHR) )
	{
		status = STATUS_SUCCEEDED;
		_setCurrentRequestType( REQUESTTYPE_NONE );
		_currentOnlineTaskHandle = 0;
		_requestState = REQUESTSTATE_NONE;
	}
	else
	{
		status = STATUS_WORKING;
	}

	setLastRequestStatus( status );

	if( STATUS_WORKING == status )
	{
		return;
	}

	// task completed

	HRESULT verifyHR = E_FAIL;
	HRESULT getResultsHR = XOnlineStringVerifyGetResults(
		taskHandle,
		1, 
		&verifyHR );

	XOnlineTaskClose( taskHandle );

	if( FAILED( verifyHR ) )
	{
		_setError( verifyHR );
		setLastRequestStatus( STATUS_FAILED );
		_setCurrentRequestType( REQUESTTYPE_NONE );
		return;
	}

	if( FAILED( getResultsHR ) )
	{
		_setError( getResultsHR );
		setLastRequestStatus( STATUS_FAILED );
		_setCurrentRequestType( REQUESTTYPE_NONE );
		return;
	}

	setLastRequestStatus( STATUS_SUCCEEDED );
	_setCurrentRequestType( REQUESTTYPE_NONE );
}
