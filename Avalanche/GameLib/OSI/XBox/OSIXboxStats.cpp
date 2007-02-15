
#include "pch.hpp"
#include "OSIXbox.hpp"
#include "OSIXboxStatsUtility.hpp"
#include "OSIXboxClansUtility.hpp"

///------------------------------------------------------------------------------------------------
//! \brief Take a list of players and retrieve stats for all of them.
//! \return True if the *request* (not the thing being requested) was successfully created.
//! \param playerList The players for which stats should be retrieved.
///------------------------------------------------------------------------------------------------
bool OSIXbox::requestStatsPlayerList( OSIPlayerStatsList& playerStatsList )
///------------------------------------------------------------------------------------------------
{
	if( !isOkToRequest() ) { return false; }

	ASSERT( REQUESTTYPE_NONE == _currentRequestType );

	if( playerStatsList.count() == 0 )
	{
		setLastRequestStatus( STATUS_SUCCEEDED );
		return true;
	}

	// Yes, I realize that it seems retarded to have one list named "statsListPlayers" and the other
	// named "playerStatsList".  The difference is context: the argument to this function is a
	// generic "list of player stats", thus "player-stats list".  The _statsListPlayers member
	// of OSI is one of a few specific "lists that are part of the stats subsystem" and it happens
	// to be the list that "contains players", thus "stats-list players".
	// Really the whole problem is due to the fact that OSI is a monolithic turd.  If it were 
	// better organized then there would be a stats interface that would change this line of code
	// to something like, "_playerList = playerList" or "getStatsForPlayers( playerList )" etc.
	// Note that half the time &playerStatsList==&_statsListPlayers, but the operator= should
	// handle it correctly.

	_statsListPlayers = playerStatsList;
	
	ASSERT( _scratchPlayerList.count() == 0 );
	_scratchPlayerList.clear();

	// Build a scratch list of players so clan info can be retrieved.  This sucks.
	OSIPlayerStats* stats = 0;
	for( playerStatsList.begin(); stats = playerStatsList.getCurrent(); playerStatsList.next() )
	{
		OSIPlayer player;
		player.id = stats->playerID;
		_scratchPlayerList.push_back( player );
	}

	// First have to get the clan info for everybody so the clan stats can be downloaded.
	ClansAdapter::OSICreateSingleton();
	ClansAdapter* clanIO = ClansAdapter::OSIGetSingleton();
	clanIO->BeginDownloadClanInfo( &_scratchPlayerList );
	
	setLastRequestStatus( STATUS_WORKING );
	_setCurrentRequestType( REQUESTTYPE_STATS_FOR_PLAYERS );
	_requestState = REQUESTSTATE_STATS_FOR_PLAYERS_GET_CLAN_INFO;
	return true;
}


///------------------------------------------------------------------------------------------------
void OSIXbox::_processStatsForPlayers()
///------------------------------------------------------------------------------------------------
{
	switch( _requestState )
	{
	case REQUESTSTATE_STATS_FOR_PLAYERS_GET_CLAN_INFO:	_processStatsForPlayersGetClanInfo(); break;
	case REQUESTSTATE_STATS_FOR_PLAYERS_GET_STATS:		_processStatsForPlayersGetStats(); break;
	default: ASSERT( 0 );
	}
}

///------------------------------------------------------------------------------------------------
void OSIXbox::_processStatsForPlayersGetClanInfo()
///------------------------------------------------------------------------------------------------
{
	ASSERT( REQUESTTYPE_STATS_FOR_PLAYERS == _currentRequestType );
	ASSERT( REQUESTSTATE_STATS_FOR_PLAYERS_GET_CLAN_INFO == _requestState );

	// Process clan info downloads.
	ClansAdapter* clanIO = ClansAdapter::OSIGetSingleton();
	if( !clanIO->IsDownloadClanInfoComplete() )
	{
		clanIO->ContinueDownloadClanInfo();
		return;
	}
	clanIO->FinishDownloadClanInfo();
	ClansAdapter::OSIDestroySingleton();

	_statsListClans.clear();

	// All players in _scratchPlayerList who are in a clan now have valid .clanInfo structures.
	// Iterate through _scratchPlayerList and copy any clan info out of the OSIPlayer struct and into
	// the OSIPlayerStats struct.  The clan's stats have not been retrieved yet so that info is
	// not used yet.
	OSIPlayerStats* playerStats = 0;
	for( _statsListPlayers.begin(); playerStats = _statsListPlayers.getCurrent(); _statsListPlayers.next() )
	{
		OSIPlayer* player = _scratchPlayerList.findPlayerByID( playerStats->playerID );
		if( !player )
		{
			ASSERT( player );
			continue;
		}

		// All that work just for a 3-character string...
		OSIstrncpy( playerStats->clanAbbr, player->clanInfo.tag, NUM_ARRAY_ELEMENTS( playerStats->clanAbbr ) );
		
	// UPDATE - Dont download stats for clan - this may need to change again so I'm leaving it here.
	// Originally this request did not retrieve any clan info or clan stats.
	// I changed it to get clan info, and for some reason also changed to to get clan stats.
	// In hindsight I can't remember why I made it download clan *stats* in addition to clan info, because
	// there's no reason this request needs to download clan stats.  And, in fact, doing so is a TCR violation
	// because this request is called at the end of a round and when the following lines are uncommented it
	// results in OSI downloading stats *twice* - once for the player and again for the clan.  TCRs say that
	// we are allowed only one stat download at the end of a round.  And the clan stats are not used for anything
	// anyway, so there's no reason for it that I can remember.
	// The question is whether or not there's any other code that calls requestStatsForPlayers and expects the
	// clan stats to be updated also.  If there is (and I think there is - when viewing player stats from the 
	// stats menu) - then that code needs to be updated to retrieve clan stats separately.

		/*
		// Put the clan into the _statsListClans so that the StatsAdapter will go get
		// stats for it.
		if( player->clanInfo.clanID.xBox.clanId != 0UI64 )
		{
			_statsListClans.push_back( player->clanInfo );
		}
		*/
	}

	// Begin stats download.
	StatsAdapter::OSICreateSingleton();
	StatsAdapter* statIO = StatsAdapter::OSIGetSingleton();
	HRESULT hr = statIO->BeginDownload( _currentOnlineTaskHandle, &_statsListPlayers, &_statsListClans );
	if( FAILED( hr ) )
	{
		_scratchPlayerList.clear();
		StatsAdapter::OSIDestroySingleton();
		setLastRequestStatus( STATUS_FAILED );
		_setError( hr );
	}
	else
	{
		setLastRequestStatus( STATUS_WORKING );
		_requestState = REQUESTSTATE_STATS_FOR_PLAYERS_GET_STATS;
	}
}

///------------------------------------------------------------------------------------------------
void OSIXbox::_processStatsForPlayersGetStats()
///------------------------------------------------------------------------------------------------
{
	ASSERT( REQUESTTYPE_STATS_FOR_PLAYERS == _currentRequestType );
	ASSERT( REQUESTSTATE_STATS_FOR_PLAYERS_GET_STATS == _requestState );
	ASSERT( _currentOnlineTaskHandle );

	const XONLINETASK_HANDLE taskHandle = _commonXOnlineTaskProcessing();
	const OSIRequestStatus status = getLastRequestStatus();
	if( STATUS_WORKING == status )
	{
		return;
	}

	else if( STATUS_FAILED == status )
	{
		StatsAdapter::OSIDestroySingleton();
		_scratchPlayerList.clear();
		return;
	}

	// task succeeded

	StatsAdapter* statIO = StatsAdapter::OSIGetSingleton();
	HRESULT hr = statIO->ProcessDownloadResult( taskHandle );
	if( FAILED( hr ) )
	{
		StatsAdapter::OSIDestroySingleton();
		setLastRequestStatus( STATUS_FAILED );
		_setCurrentRequestType( REQUESTTYPE_NONE );
		_setError( hr );
		_scratchPlayerList.clear();
		return;
	}

	if( !statIO->IsDownloadComplete() )
	{
		hr = statIO->ContinueDownload( _currentOnlineTaskHandle );
        if( FAILED( hr ) )
		{
			StatsAdapter::OSIDestroySingleton();
			setLastRequestStatus( STATUS_FAILED );
			_setError( hr );
			_scratchPlayerList.clear();
		}
		else
		{
			setLastRequestStatus( STATUS_WORKING );
			_setCurrentRequestType( REQUESTTYPE_STATS_FOR_PLAYERS );
			_requestState = REQUESTSTATE_STATS_FOR_PLAYERS_GET_STATS;
		}
		return;
	}

	// download is complete

	// _commonXonlineTaskProcessing should have set up OSI for a success state already
	// At this point, every player in _statsListPlayers should have valid stats and
	// every clan in _statsListClans should have valid stats.
	StatsAdapter::OSIDestroySingleton();
	
	// Fill in the clan-stat-related fields of OSIPlayerStats using the scratch list.
	// I use the scratch list because it's the only way I can associate a player with
	// a clan since OSIPLayerStats doesn't have a clanID member.
	OSIPlayer* player;
	for( _scratchPlayerList.begin()
		; player = _scratchPlayerList.getCurrent()
		; _scratchPlayerList.next() )
	{
		OSIPlayerStats* playerStats 
			= _statsListPlayers.findPlayerByID( player->id );

		OSIClanInfo* clanInfo
			= _statsListClans.findClanInfoByID( player->clanInfo.clanID );
		if( !clanInfo )
		{
			playerStats->clanKills = 0;
			playerStats->clanRating = 0;
			// Not in a clan or stats not found.
			continue;
		}

		playerStats->clanKills 	= clanInfo->stats.statsArray[ OSIClanStats::CLANSTAT_KILLS ];
		playerStats->clanRating = clanInfo->stats.statsArray[ OSIClanStats::CLANSTAT_RATING ];
		// There is no clanRank member of playerStats yet, but I'm guessing it will come soon.
		//playerStats->clanRank = clanInfo->stats.statsArray[ OSIClanStats::CLANSTAT_RANK ];
	}

	// free the scratch list
	_scratchPlayerList.clear();
}

//------------------------------------------------------------------------------------------------
bool OSIXbox::requestCreateRound()
///------------------------------------------------------------------------------------------------
{
	_currentRoundID = 5;
	setLastRequestStatus( STATUS_SUCCEEDED );
	return REQUEST_SUCCEEDED;
}

//------------------------------------------------------------------------------------------------
bool OSIXbox::requestJoinRound()
///------------------------------------------------------------------------------------------------
{
	setLastRequestStatus( STATUS_SUCCEEDED );
	return REQUEST_SUCCEEDED;
}

//------------------------------------------------------------------------------------------------
bool OSIXbox::requestStartRound()
///------------------------------------------------------------------------------------------------
{
	setLastRequestStatus( STATUS_SUCCEEDED );
	return REQUEST_SUCCEEDED;
}

//------------------------------------------------------------------------------------------------
bool OSIXbox::requestEndRound( OSIRoundStats& roundStats )
///------------------------------------------------------------------------------------------------
{
	if( !isOkToRequest() ) { return false; }

	ASSERT( REQUESTTYPE_NONE == _currentRequestType );

	// Don't report stats if the game is not ranked.
	if( !_currentMatch.options.isRanked || roundStats.playerStatsList.count() <= 1 )
	{
		setLastRequestStatus( STATUS_SUCCEEDED );
		return REQUEST_SUCCEEDED;
	}

	// Each Xbox only reports stats for the local player since there is no arbitration.
	OSIPlayer* player = getActivePlayer();
	ASSERT( player );

	// Find that player's new stat information.
 	OSIPlayerStats* playerStats = roundStats.playerStatsList.findPlayerByID( player->id );

	if( !playerStats || playerStats->playerID != player->id )
	{
		ASSERT( playerStats );
		ASSERT( playerStats->playerID == player->id );
		_setError( E_FAIL );
		setLastRequestStatus( STATUS_FAILED );
		return REQUEST_FAILED;
	}

	// Derive the clan's new stat information from the player's stats.  Note that this code will
	// still execute if clanID is invalid (meaning the player is not in a clan) but the 
	// StatsAdapter will know to ignore the information anyway.
	OSIClanStats clanStats;
	clanStats.clanID = player->clanInfo.clanID;
	
	if( playerStats->statsArray[ OSIPlayerStats::PLAYERSTAT_USER_RATING ] > 0 )
	{
		clanStats.statsArray[ OSIClanStats::CLANSTAT_RATING ] = 1;
	}
	else if( playerStats->statsArray[ OSIPlayerStats::PLAYERSTAT_USER_RATING ] < 0 )
	{
		clanStats.statsArray[ OSIClanStats::CLANSTAT_RATING ] = -1;
	}
	else
	{
		clanStats.statsArray[ OSIClanStats::CLANSTAT_RATING ] = 0;
	}

	clanStats.statsArray[ OSIClanStats::CLANSTAT_KILLS ] 
		= playerStats->statsArray[ OSIPlayerStats::PLAYERSTAT_KILLS_AS_COP ]
		+ playerStats->statsArray[ OSIPlayerStats::PLAYERSTAT_KILLS_AS_GANGSTA ];

	StatsAdapter::OSICreateSingleton();
	StatsAdapter* statIO = StatsAdapter::OSIGetSingleton();
	HRESULT beginHR = statIO->BeginUpload( _currentOnlineTaskHandle, *playerStats, clanStats );
	if( FAILED( beginHR ) )
	{
		StatsAdapter::OSIDestroySingleton();
		setLastRequestStatus( STATUS_FAILED );
		_setError( beginHR );
		return REQUEST_FAILED;
	}
	else
	{
		setLastRequestStatus( STATUS_WORKING );
		_setCurrentRequestType( REQUESTTYPE_END_ROUND );
		return REQUEST_SUCCEEDED;
	}
}

///------------------------------------------------------------------------------------------------
void OSIXbox::_processEndRound()
///------------------------------------------------------------------------------------------------
{
	ASSERT( REQUESTTYPE_END_ROUND == _currentRequestType );
	XONLINETASK_HANDLE taskHandle = _commonXOnlineTaskProcessing();
	if( STATUS_WORKING != getLastRequestStatus() )
	{
		StatsAdapter* statIO = StatsAdapter::OSIGetSingleton();
		HRESULT hr = statIO->FinishUpload( taskHandle );
		StatsAdapter::OSIDestroySingleton();
		if( FAILED( hr ) )
		{
			setLastRequestStatus( STATUS_FAILED );
			_setError( hr );
		}
	}
}

///------------------------------------------------------------------------------------------------
bool OSIXbox::requestStatsPlayerLadder( OSIPlayerID& pivotPlayer, const int numRanks )
///------------------------------------------------------------------------------------------------
{
	if( !isOkToRequest() ) { return false; }

	if( !pivotPlayer.isValid() || !numRanks )
	{
		_statsListPlayers.clear();
		setLastRequestStatus( STATUS_SUCCEEDED );
		return true;
	}

	StatsAdapter::OSICreateSingleton();
	StatsAdapter* statIO = StatsAdapter::OSIGetSingleton();
	HRESULT beginHR = statIO->BeginDownloadPlayerLadder( _currentOnlineTaskHandle, pivotPlayer, numRanks, &_statsListPlayers );
	if( FAILED( beginHR ) )
	{
		StatsAdapter::OSIDestroySingleton();
		setLastRequestStatus( STATUS_FAILED );
		_setError( beginHR );
		return REQUEST_FAILED;
	}
	else
	{
		setLastRequestStatus( STATUS_WORKING );
		_setCurrentRequestType( REQUESTTYPE_STATS_FOR_PLAYER_LADDER );
		return REQUEST_SUCCEEDED;
	}
}

///------------------------------------------------------------------------------------------------
bool OSIXbox::requestStatsTopPlayers( const int numRanks )
///------------------------------------------------------------------------------------------------
{
	if( !isOkToRequest() ) { return false; }

	if( !numRanks )
	{
		_statsListPlayers.clear();
		setLastRequestStatus( STATUS_SUCCEEDED );
		return true;
	}

	StatsAdapter::OSICreateSingleton();
	StatsAdapter* statIO = StatsAdapter::OSIGetSingleton();
	HRESULT beginHR = statIO->BeginDownloadPlayerLadder( _currentOnlineTaskHandle, 1, numRanks, &_statsListPlayers );
	if( FAILED( beginHR ) )
	{
		StatsAdapter::OSIDestroySingleton();
		setLastRequestStatus( STATUS_FAILED );
		_setError( beginHR );
		return REQUEST_FAILED;
	}
	else
	{
		setLastRequestStatus( STATUS_WORKING );
		_setCurrentRequestType( REQUESTTYPE_STATS_FOR_PLAYER_LADDER );
		return REQUEST_SUCCEEDED;
	}
}

///------------------------------------------------------------------------------------------------
void OSIXbox::_processStatsForPlayerLadder()
///------------------------------------------------------------------------------------------------
{
	ASSERT( REQUESTTYPE_STATS_FOR_PLAYER_LADDER == _currentRequestType );
	ASSERT( _currentOnlineTaskHandle );

	const XONLINETASK_HANDLE taskHandle = _commonXOnlineTaskProcessing();
	const OSIRequestStatus status = getLastRequestStatus();
	if( STATUS_WORKING == status )
	{
		return;
	}

	else if( STATUS_FAILED == status )
	{
		XOnlineTaskClose( taskHandle );
		_statsListPlayers.clear();
		_statsListClans.clear();

		// nobody on the ladder yet :)
		if( XONLINE_E_STAT_USER_NOT_FOUND == _errorHR )
		{
			setLastRequestStatus( STATUS_SUCCEEDED );
		}

		StatsAdapter::OSIDestroySingleton();
		return;
	}

	// task succeeded
	StatsAdapter* statIO = StatsAdapter::OSIGetSingleton();
	HRESULT hr = statIO->FinishDownloadPlayerLadder( taskHandle );

	// _statsListPlayers is now populated with people from the ladder, but their stats
	// and clan info are invalid.  Destroy the StatsAdapter and transition to 
	// requestStatsPlayerList, which will create the ClansAdapter, then later it will
	// create the StatsAdapter again.
	StatsAdapter::OSIDestroySingleton();

	if( FAILED( hr ) )
	{
		setLastRequestStatus( STATUS_FAILED );
		_setError( hr );
		return;
	}

	// nobody in the ladder :)
	if( 0 == _statsListPlayers.count() )
	{
		setLastRequestStatus( STATUS_SUCCEEDED );
		return;
	}

	requestStatsPlayerList( _statsListPlayers );
}

///------------------------------------------------------------------------------------------------
bool OSIXbox::requestStatsTopClans( int numClans )
///------------------------------------------------------------------------------------------------
{
	if( !isOkToRequest() ) { return false; }

	if( !numClans )
	{
		_statsListClans.clear();
		setLastRequestStatus( STATUS_SUCCEEDED );
		return true;
	}

	StatsAdapter::OSICreateSingleton();
	StatsAdapter* statIO = StatsAdapter::OSIGetSingleton();
	HRESULT beginHR = statIO->BeginDownloadClanLadder( _currentOnlineTaskHandle, 1, numClans, &_statsListClans );
	if( FAILED( beginHR ) )
	{
		StatsAdapter::OSIDestroySingleton();
		setLastRequestStatus( STATUS_FAILED );
		_setError( beginHR );
		return REQUEST_FAILED;
	}
	else
	{
		setLastRequestStatus( STATUS_WORKING );
		_setCurrentRequestType( REQUESTTYPE_STATS_FOR_CLAN_LADDER );
		_requestState = REQUESTSTATE_STATS_FOR_CLAN_LADDER_GET_STATS;
		return REQUEST_SUCCEEDED;
	}
}

///------------------------------------------------------------------------------------------------
void OSIXbox::_processStatsForClanLadder()
///------------------------------------------------------------------------------------------------
{
	switch( _requestState )
	{
	case REQUESTSTATE_STATS_FOR_CLAN_LADDER_GET_CLAN_INFO:	
		_processStatsForClanLadderGetClanInfo(); 
		break;

	case REQUESTSTATE_STATS_FOR_CLAN_LADDER_GET_STATS:		
		_processStatsForClanLadderGetStats(); 
		break;

	default: ASSERT( 0 );
	};
}

///------------------------------------------------------------------------------------------------
void OSIXbox::_processStatsForClanLadderGetClanInfo()
///------------------------------------------------------------------------------------------------
{
	ASSERT( REQUESTTYPE_STATS_FOR_CLAN_LADDER == _currentRequestType );
	ASSERT( REQUESTSTATE_STATS_FOR_CLAN_LADDER_GET_CLAN_INFO == _requestState );

	// Process clan info downloads.
	ClansAdapter* clanIO = ClansAdapter::OSIGetSingleton();
	if( !clanIO->IsDownloadClanInfoComplete() )
	{
		clanIO->ContinueDownloadClanInfo();
		return;
	}
	clanIO->FinishDownloadClanInfo();
	ClansAdapter::OSIDestroySingleton();

	setLastRequestStatus( STATUS_SUCCEEDED );
	_setCurrentRequestType( REQUESTTYPE_NONE );
}
 
///------------------------------------------------------------------------------------------------
void OSIXbox::_processStatsForClanLadderGetStats()
///------------------------------------------------------------------------------------------------
{
	ASSERT( REQUESTTYPE_STATS_FOR_CLAN_LADDER == _currentRequestType );
	ASSERT( _currentOnlineTaskHandle );
	ASSERT( REQUESTSTATE_STATS_FOR_CLAN_LADDER_GET_STATS == _requestState );

	const XONLINETASK_HANDLE taskHandle = _commonXOnlineTaskProcessing();
	const OSIRequestStatus status = getLastRequestStatus();
	if( STATUS_WORKING == status )
	{
		return;
	}

	else if( STATUS_FAILED == status )
	{
		XOnlineTaskClose( taskHandle );
		_statsListClans.clear();

		// nobody on the ladder yet :)
		if( XONLINE_E_STAT_USER_NOT_FOUND == _errorHR )
		{
			setLastRequestStatus( STATUS_SUCCEEDED );
		}

		StatsAdapter::OSIDestroySingleton();
		return;
	}

	// task succeeded
	StatsAdapter* statIO = StatsAdapter::OSIGetSingleton();
	HRESULT hr = statIO->FinishDownloadClanLadder( taskHandle );
	StatsAdapter::OSIDestroySingleton();

	// filter out invalid entries
	for( _statsListClans.begin(); _statsListClans.getCurrent(); )
	{
		OSIClanInfo* clanInfo = _statsListClans.getCurrent();
		if( clanInfo->stats.statsArray[ OSIClanStats::CLANSTAT_RATING ] < 0 )
		{
			_statsListClans.erase();
		}
		else
		{
			 _statsListClans.next();
		}
	}

	if( _statsListClans.count() > 0  )
	{
		// download clan info for display
		ClansAdapter::OSICreateSingleton();
		ClansAdapter* clanIO = ClansAdapter::OSIGetSingleton();
		clanIO->BeginDownloadClanInfo( &_statsListClans );

		setLastRequestStatus( STATUS_WORKING );
		_setCurrentRequestType( REQUESTTYPE_STATS_FOR_CLAN_LADDER );
		_requestState = REQUESTSTATE_STATS_FOR_CLAN_LADDER_GET_CLAN_INFO;
	}
	else
	{
		// nobody left to display, simulate an empty ladder
		setLastRequestStatus( STATUS_SUCCEEDED );
		_setCurrentRequestType( REQUESTTYPE_NONE );
		_setError( XONLINE_E_STAT_USER_NOT_FOUND );
	}
}

///------------------------------------------------------------------------------------------------
bool OSIXbox::requestUpdateMyRankAndRating()
///------------------------------------------------------------------------------------------------
{
	if( !isOkToRequest() ) { return false; }

	ASSERT( REQUESTTYPE_NONE == _currentRequestType );

	XONLINE_STAT stats[2];
	stats[0].type = XONLINE_STAT_LONGLONG;
	stats[0].wID = XONLINE_STAT_RATING;
	stats[0].llValue = 0;
	
	stats[1].type = XONLINE_STAT_LONG;
	stats[1].wID = XONLINE_STAT_RANK;
	stats[1].llValue = 0;

	XONLINE_STAT_SPEC spec;
	spec.dwLeaderBoardID = 1;
	spec.dwNumStats = 2;
	spec.pStats = stats;
	OSIPlayerID_to_XUID( getActivePlayer()->id, spec.xuidUser );

	HRESULT hr = XOnlineStatRead( 1, &spec, 0, &_currentOnlineTaskHandle );
	if( FAILED( hr ) )
	{
		setLastRequestStatus( STATUS_FAILED );
		_setError( hr );
		_setCurrentRequestType( REQUESTTYPE_NONE );
		return false;
	}
	else
	{
		setLastRequestStatus( STATUS_WORKING );
		_setCurrentRequestType( REQUESTTYPE_UPDATE_RANK_AND_RATING );
		return true;
	}
}
                          
///------------------------------------------------------------------------------------------------
void OSIXbox::_processUpdateMyRankAndRating()
///------------------------------------------------------------------------------------------------
{
	ASSERT( REQUESTTYPE_UPDATE_RANK_AND_RATING == _currentRequestType );
	XONLINETASK_HANDLE taskHandle = _commonXOnlineTaskProcessing();
	if( STATUS_WORKING != getLastRequestStatus() )
	{
		OSIPlayer* p = getActivePlayer();
		if( STATUS_FAILED == getLastRequestStatus() )
		{
			XOnlineTaskClose( taskHandle );
			p->statRating = 0;
			p->statRanking = 0;
		}
		else 
		{
			XONLINE_STAT stats[2];
			stats[0].type = XONLINE_STAT_LONGLONG;
			stats[0].wID = XONLINE_STAT_RATING;
			stats[0].llValue = 0;
			
			stats[1].type = XONLINE_STAT_LONG;
			stats[1].wID = XONLINE_STAT_RANK;
			stats[1].llValue = 0;

			XONLINE_STAT_SPEC spec;
			spec.dwLeaderBoardID = 1;
			spec.dwNumStats = 2;
			spec.pStats = stats;
			OSIPlayerID_to_XUID( getActivePlayer()->id, spec.xuidUser );

			HRESULT hr = XOnlineStatReadGetResult( taskHandle, 1, &spec, 0, 0 );
			XOnlineTaskClose( taskHandle );
			if( FAILED( hr ) )
			{
				setLastRequestStatus( STATUS_FAILED );
			}
			else
			{
				setLastRequestStatus( STATUS_SUCCEEDED );
				// see StatsAdapter::ConvertPlayerXboxStatToOSI
				p->statRating = (int) stats[0].llValue + 1600;
				p->statRanking = (int) stats[1].lValue;
			}
		}
	}
}
