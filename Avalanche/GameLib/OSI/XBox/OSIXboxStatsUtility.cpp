
#include "pch.hpp"
#include "OSIXbox.hpp"
#include "OSIXboxStatsUtility.hpp"

StatsAdapter* StatsAdapter::m_singletonPtr = 0;
#ifdef _DEBUG
BYTE StatsAdapter::s_statsAdapterInstanceNumber = 0;
#endif //_DEBUG

// see header for more documentation

//-----------------------------------------------------------------------------
HRESULT 
StatsAdapter::
BeginUpload( 
	XONLINETASK_HANDLE& handle,
	OSIPlayerStats& playerStats,
	OSIClanStats& clanStats )
//-----------------------------------------------------------------------------
{
	if( !m_upload )
	{
		m_upload = new UploadData;

		#ifdef _DEBUG
		m_upload->instanceNumberAtBegin = s_statsAdapterInstanceNumber;
		#endif //_DEBUG
	}

	XUID playerXUID;
	XUID clanXUID;
	OSIPlayerID_to_XUID( playerStats.playerID, playerXUID );

	XONLINE_STAT_PROC (&procs)[ s_NumUploadProcs ] = m_upload->procs;
	procs[ PROCINDEX_PLAYER_INC_0 ].wProcedureID = XONLINE_STAT_PROCID_UPDATE_INCREMENT;
	procs[ PROCINDEX_PLAYER_INC_64 ].wProcedureID = XONLINE_STAT_PROCID_UPDATE_INCREMENT;

	XONLINE_STAT* statPlayerInc1 = m_upload->statPlayerInc;
	const int proc1StatCount = FillPlayerStats( 
		statPlayerInc1, 
		s_LeaderboardIdForPlayerStats0to63,
		s_PlayerIncStatsToUpload,
		s_NumUploadIncStatsPerPlayer, 
		&playerStats );

	XONLINE_STAT* statPlayerInc2 = statPlayerInc1 + proc1StatCount;
	const int proc2StatCount = FillPlayerStats( 
		statPlayerInc2, 
		s_LeaderboardIdForPlayerStats64to127,
		s_PlayerIncStatsToUpload,
		s_NumUploadIncStatsPerPlayer, 
		&playerStats );

	FillUpdate( 
		procs[ PROCINDEX_PLAYER_INC_0 ].Update, 
		playerXUID, 
		s_LeaderboardIdForPlayerStats0to63, 
		proc1StatCount, 
		statPlayerInc1 );
	
	FillUpdate( 
		procs[ PROCINDEX_PLAYER_INC_64 ].Update, 
		playerXUID, 
		s_LeaderboardIdForPlayerStats64to127, 
		proc2StatCount, 
		statPlayerInc2 );

	int procCount = NUM_PROCINDEX_PLAYER;

	if( clanStats.clanID.xBox.clanId != 0 )
	{
		procCount = NUM_PROCINDEX_CLAN;

		OSIClanID_to_XUID( clanStats.clanID, clanXUID );
		procs[ PROCINDEX_CLAN_INC_0 ].wProcedureID = XONLINE_STAT_PROCID_UPDATE_INCREMENT;
		XONLINE_STAT* statClanInc1 = statPlayerInc2 + proc2StatCount;
		const int proc3StatCount = FillClanStats(
			statClanInc1,
			s_LeaderboardIdForClanStats0to63,
			s_ClanIncStatsToUpload,
			s_NumUploadIncStatsPerClan,
			&clanStats );

		FillUpdate(
			procs[ PROCINDEX_CLAN_INC_0 ].Update,
			clanXUID,
			s_LeaderboardIdForClanStats0to63,
			proc3StatCount,
			statClanInc1 );

		m_upload->resetClanRatingStat.wID = ConvertClanOsiStatIDToXbox( OSIClanStats::CLANSTAT_RATING );
		m_upload->resetClanRatingStat.type = XONLINE_STAT_LONGLONG;
		m_upload->resetClanRatingStat.llValue = -1600;

		procs[ PROCINDEX_CLAN_RESET_RATING ].wProcedureID = XONLINE_STAT_PROCID_UPDATE_REPLACE;
		procs[ PROCINDEX_CLAN_RESET_RATING ].Update.dwConditionalIndex = 1 + PROCINDEX_CONDITIONAL_RATING_LESS_NEG_1600;
		procs[ PROCINDEX_CLAN_RESET_RATING ].Update.dwLeaderBoardID = s_LeaderboardIdForClanStats0to63;
		procs[ PROCINDEX_CLAN_RESET_RATING ].Update.dwNumStats = 1;
		procs[ PROCINDEX_CLAN_RESET_RATING ].Update.pStats = &m_upload->resetClanRatingStat;
		procs[ PROCINDEX_CLAN_RESET_RATING ].Update.xuid = clanXUID;
		
		procs[ PROCINDEX_CONDITIONAL_RATING_LESS_NEG_1600 ].wProcedureID = XONLINE_STAT_PROCID_CONDITIONAL;
		procs[ PROCINDEX_CONDITIONAL_RATING_LESS_NEG_1600 ].Conditional.xuid = clanXUID;
		procs[ PROCINDEX_CONDITIONAL_RATING_LESS_NEG_1600 ].Conditional.dwLeaderBoardID = s_LeaderboardIdForClanStats0to63;
		procs[ PROCINDEX_CONDITIONAL_RATING_LESS_NEG_1600 ].Conditional.bComparisonType = XONLINE_STAT_COMPTYPE_LESS;
		procs[ PROCINDEX_CONDITIONAL_RATING_LESS_NEG_1600 ].Conditional.StatToCompare.wID = ConvertClanOsiStatIDToXbox( OSIClanStats::CLANSTAT_RATING );
		procs[ PROCINDEX_CONDITIONAL_RATING_LESS_NEG_1600 ].Conditional.StatToCompare.type = XONLINE_STAT_LONGLONG;
		procs[ PROCINDEX_CONDITIONAL_RATING_LESS_NEG_1600 ].Conditional.StatToCompare.llValue = -1600 + (LONGLONG) clanStats.getStat( OSIClanStats::CLANSTAT_RATING );

	}
    
	return XOnlineStatWriteEx( procCount, procs, 0, &handle );
}

//-----------------------------------------------------------------------------
HRESULT 
StatsAdapter::
FinishUpload( 
	const XONLINETASK_HANDLE& handle )
//-----------------------------------------------------------------------------
{
	XOnlineTaskClose( handle );
	return S_OK;
}

//-----------------------------------------------------------------------------
HRESULT 
StatsAdapter::
BeginDownload( 
	XONLINETASK_HANDLE& handle, 
	OSIPlayerStatsList* playerList, 
	OSIClanInfoList* clanList )
//-----------------------------------------------------------------------------
{
	if( playerList && playerList->count() > 0  )
	{
		if( !m_playerDownload )
		{
			m_playerDownload = new DownloadPlayerData;
		}

		#ifdef _DEBUG
		m_playerDownload->instanceNumberAtBegin = s_statsAdapterInstanceNumber;
		#endif //_DEBUG

		m_playerList = playerList;
		m_playerList->begin();
		m_playerDownload->numTotal = m_playerList->count();
		m_playerDownload->numStarted = 0;
		m_playerDownload->numFinished = 0;

	}
	else
	{
		if( m_playerDownload ) 
		{
			delete m_playerDownload;
			m_playerDownload = 0;
		}
		m_playerList = 0;
	}

	if( clanList && clanList->count() > 0 )
	{
		if( !m_clanDownload ) 
		{ 
			m_clanDownload = new DownloadClanData;
		}

		#ifdef _DEBUG
		m_clanDownload->instanceNumberAtBegin = s_statsAdapterInstanceNumber;
		#endif //_DEBUG

		m_clanInfoList = clanList;
		m_clanInfoList->begin();
		m_clanDownload->numTotal = m_clanInfoList->count();
		m_clanDownload->numStarted = 0;
		m_clanDownload->numFinished = 0;
	}
	else
	{
		if( m_clanDownload ) 
		{ 
			delete m_clanDownload;
			m_clanDownload = 0;
		}
		m_clanInfoList = 0;
	}

	if( !m_playerDownload && !m_clanDownload )
	{
		return S_OK;
	}

	return ContinueDownload( handle );
}
	
//-----------------------------------------------------------------------------
HRESULT
StatsAdapter::
ContinueDownloadClans(
	XONLINETASK_HANDLE& handle )
//-----------------------------------------------------------------------------
{
	if( IsDownloadClansComplete() )
	{
		// shouldn't be calling continue if it's all complete
		ASSERT( !IsDownloadClansComplete() );
		return S_OK;
	}

	int clanIdx = 0;

	while(( (m_clanDownload->numFinished + clanIdx) < m_clanDownload->numTotal ) 
		&& ( clanIdx < s_MaxClansPerDownload ) )
	{
		OSIClanInfo* const clanInfo = m_clanInfoList->getCurrent();
		XUID clanXUID;
		OSIClanID_to_XUID( clanInfo->clanID, clanXUID );

		XONLINE_STAT_SPEC& spec1 = m_clanDownload->specs[ s_NumDownloadSpecsPerClan * clanIdx ];
		XONLINE_STAT* const stat1 = &m_clanDownload->stats[ s_NumDownloadStatsPerClan * clanIdx ];
		const int spec1StatCount = FillClanStats( 
			stat1,
			s_LeaderboardIdForClanStats0to63,
			s_ClanStatsToDownload,
			s_NumDownloadStatsPerClan,
			&clanInfo->stats );

		FillSpec( 
			spec1,
			clanXUID,
			s_LeaderboardIdForClanStats0to63,
			spec1StatCount,
			stat1 );

		m_clanInfoList->next();
		++clanIdx;
		++m_clanDownload->numStarted;
	}

	// kick off the download operation
	const int numClansInDownload = m_clanDownload->numStarted - m_clanDownload->numFinished;
	return XOnlineStatRead( 
		s_NumDownloadSpecsPerClan * numClansInDownload,
		m_clanDownload->specs,
		0,
		&handle );
}

//-----------------------------------------------------------------------------
HRESULT
StatsAdapter::
ContinueDownloadPlayers(
	XONLINETASK_HANDLE& handle )
//-----------------------------------------------------------------------------
{
	if( IsDownloadPlayersComplete() )
	{
		// shouldn't be calling continue if it's all complete
		ASSERT( !IsDownloadPlayersComplete() );
		return S_OK;
	}

	int playerIdx = 0;
	
	while(( (m_playerDownload->numFinished + playerIdx) < m_playerDownload->numTotal ) 
		&& ( playerIdx < s_MaxPlayersPerDownload ) )
	{
		OSIPlayerStats* const player = m_playerList->getCurrent();
		XUID playerXUID;
		OSIPlayerID_to_XUID( player->playerID, playerXUID );

		// make some pointers to this player's specs and stats
		XONLINE_STAT_SPEC& spec1 = m_playerDownload->specs[ s_NumDownloadSpecsPerPlayer * playerIdx ];
		XONLINE_STAT_SPEC& spec2 = m_playerDownload->specs[ s_NumDownloadSpecsPerPlayer * playerIdx + 1 ];

		XONLINE_STAT* const stat1 = &m_playerDownload->stats[ s_NumDownloadStatsPerPlayer * playerIdx ];
		const int spec1StatCount = FillPlayerStats( 
			stat1,
			s_LeaderboardIdForPlayerStats0to63,
			s_PlayerStatsToDownload,
			s_NumDownloadStatsPerPlayer,
            player );

		XONLINE_STAT* const stat2 = stat1 + spec1StatCount;
		const int spec2StatCount = FillPlayerStats( 
			stat2,
			s_LeaderboardIdForPlayerStats64to127,
			s_PlayerStatsToDownload,
			s_NumDownloadStatsPerPlayer,
            player );

		FillSpec( 
			spec1, 
			playerXUID, 
			s_LeaderboardIdForPlayerStats0to63, 
			spec1StatCount, 
			stat1 );

		FillSpec( 
			spec2, 
			playerXUID, 
			s_LeaderboardIdForPlayerStats64to127, 
			spec2StatCount, 
			stat2 );

		m_playerList->next();
		++playerIdx;
		++m_playerDownload->numStarted;
	}

	// kick off the download operation
	const int numPlayersInDownload = m_playerDownload->numStarted - m_playerDownload->numFinished;
	return XOnlineStatRead( 
		s_NumDownloadSpecsPerPlayer * numPlayersInDownload,
		m_playerDownload->specs,
		0,
		&handle );
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
HRESULT 
StatsAdapter::
ContinueDownload( 
	XONLINETASK_HANDLE& handle )
//-----------------------------------------------------------------------------
{
	ASSERT( m_playerDownload || m_clanDownload );
#ifdef _DEBUG
	ASSERT( !m_playerDownload || (m_playerDownload->instanceNumberAtBegin 
		== s_statsAdapterInstanceNumber) );
	ASSERT( !m_clanDownload || (m_clanDownload->instanceNumberAtBegin 
		== s_statsAdapterInstanceNumber) );
#endif
	ASSERT( !m_playerDownload || m_playerList );
	ASSERT( !m_clanDownload || m_clanInfoList );
	ASSERT( !m_playerDownload || (m_playerDownload->numTotal == m_playerList->count()) );
	ASSERT( !m_clanDownload || (m_clanDownload->numTotal == m_clanInfoList->count()) );

	// don't call this function if IsDownloadComplete returns true!
	if( IsDownloadComplete() )
		{ ASSERT( !IsDownloadComplete() ); }
	else if( !IsDownloadPlayersComplete() )	
		{ return ContinueDownloadPlayers( handle ); }
	else if( !IsDownloadClansComplete() )
		{ return ContinueDownloadClans( handle ); }
	else
		{ ASSERT( 0 ); } //wtf?
	
	// default to fail
	return E_FAIL;
}

//-----------------------------------------------------------------------------
HRESULT 
StatsAdapter::
ProcessDownloadResult( 
	const XONLINETASK_HANDLE& handle )
//-----------------------------------------------------------------------------
{
	ASSERT( m_playerDownload || m_clanDownload );
#ifdef _DEBUG
	ASSERT( !m_playerDownload || (m_playerDownload->instanceNumberAtBegin 
		== s_statsAdapterInstanceNumber) );
	ASSERT( !m_clanDownload || (m_clanDownload->instanceNumberAtBegin 
		== s_statsAdapterInstanceNumber) );
#endif
	ASSERT( !m_playerDownload || m_playerList );
	ASSERT( !m_clanDownload || m_clanInfoList );
	ASSERT( !m_playerDownload || (m_playerDownload->numTotal == m_playerList->count()) );
	ASSERT( !m_clanDownload || (m_clanDownload->numTotal == m_clanInfoList->count()) );

	// don't call this function if IsDownloadComplete returns true!
	// Remember that IsDownloadComplete will only return true after a call to
	// to ProcessDownloadResult that leaves no more work to do.
	if( IsDownloadComplete() )
		{ ASSERT( !IsDownloadComplete() ); }
	else if( !IsDownloadPlayersComplete() )
		{ return ProcessDownloadPlayersResult( handle ); }
	else if( !IsDownloadClansComplete() )
		{ return ProcessDownloadClansResult( handle ); }
	else
		{ ASSERT( 0 ); } //wtf?
	
	// default to fail
	return E_FAIL;
}

//-----------------------------------------------------------------------------
HRESULT 
StatsAdapter::
ProcessDownloadPlayersResult( 
	const XONLINETASK_HANDLE& handle )
//-----------------------------------------------------------------------------
{
	ASSERT( m_playerDownload && m_playerList );

	// parse the results 
	const int numPlayersInDownload 
		= m_playerDownload->numStarted 
		- m_playerDownload->numFinished;

	HRESULT hr = XOnlineStatReadGetResult( 
		handle, 
		s_NumDownloadSpecsPerPlayer * numPlayersInDownload, 
		m_playerDownload->specs, 
		0, 
		0 );
	XOnlineTaskClose( handle );
	if( FAILED( hr ) )
	{
		return hr;
	}

	// figure out how much data is expected
	const int spec1StatCount = CountPlayerStats(
		s_LeaderboardIdForPlayerStats0to63,
		s_PlayerStatsToDownload,
		s_NumDownloadStatsPerPlayer );
	ASSERT( spec1StatCount > 0 );
	ASSERT( spec1StatCount < 64 );

	const int spec2StatCount = CountPlayerStats(
		s_LeaderboardIdForPlayerStats64to127,
		s_PlayerStatsToDownload,
		s_NumDownloadStatsPerPlayer );
	ASSERT( spec2StatCount > 0 );
	ASSERT( spec2StatCount < 64 );

	// there's no way to assert that live returned as many specs as was expected (say that fast), but
	// XOnlineStatReadGetResults will fail if we call it with wrong numbers so it will be caught that way.
	for( int playerNum = 0; playerNum < numPlayersInDownload; playerNum++ )
	{
		// make some pointers to this player's specs
		XONLINE_STAT_SPEC* spec1 = &m_playerDownload->specs[ s_NumDownloadSpecsPerPlayer * playerNum ];
		XONLINE_STAT_SPEC* spec2 = spec1 + 1;

		// verify that the stats returned from live matche what is expected
		if( spec1->dwNumStats != spec1StatCount )
		{
			ASSERT( spec1->dwNumStats == spec1StatCount );
			return E_FAIL;
		}
		if( spec2->dwNumStats != spec2StatCount )
		{
			ASSERT( spec2->dwNumStats == spec2StatCount );
			return E_FAIL;
		}

		// go through each stat spec and extract the data
		for( int specNum = 0; specNum < s_NumDownloadSpecsPerPlayer; specNum++ )
		{
			XONLINE_STAT_SPEC& spec = m_playerDownload->specs[ (s_NumDownloadSpecsPerPlayer * playerNum) + specNum ];
			
			OSIPlayerID id;
			XUID_to_OSIPlayerID( spec.xuidUser, id );
			OSIPlayerStats* player = m_playerList->findPlayerByID( id );
			if( !player )
			{
				ASSERT( player );
				return E_FAIL;
			}

			OSIPlayerStats::StatValueType (&playerStatsArray)[ OSIPlayerStats::NUM_PLAYERSTATS ] = player->statsArray;

			// go through each stat and extract the data
			for( unsigned int stat = 0; stat < spec.dwNumStats; stat++ )
			{
				WORD osiStatID;
				OSIPlayerStats::StatValueType osiStatValue;
				ConvertPlayerXboxStatToOSI( spec.pStats[ stat ], spec.dwLeaderBoardID, osiStatID, osiStatValue );
				playerStatsArray[ osiStatID ] = osiStatValue;
			}
		}

		++m_playerDownload->numFinished;
	}

	return S_OK;
}

//-----------------------------------------------------------------------------
HRESULT 
StatsAdapter::
ProcessDownloadClansResult( 
	const XONLINETASK_HANDLE& handle )
//-----------------------------------------------------------------------------
{
	ASSERT( m_clanDownload && m_clanInfoList );

	// parse the results 
	const int numClansInDownload 
		= m_clanDownload->numStarted 
		- m_clanDownload->numFinished;

	HRESULT hr = XOnlineStatReadGetResult( 
		handle, 
		s_NumDownloadSpecsPerClan * numClansInDownload, 
		m_clanDownload->specs, 
		0, 
		0 );
	XOnlineTaskClose( handle );
	if( FAILED( hr ) )
	{
		return hr;
	}

	// figure out how much data is expected
	const int spec1StatCount = s_NumDownloadStatsPerClan;
	ASSERT( spec1StatCount > 0 );
	ASSERT( spec1StatCount < 64 );

	// there's no way to assert that live returned as many specs as was 
	// expected (say that fast), but XOnlineStatReadGetResults will fail 
	// if we call it with wrong numbers so it will be caught that way.
	for( int clanNum = 0; clanNum < numClansInDownload; clanNum++ )
	{
		// make some pointers to this clan's specs
		XONLINE_STAT_SPEC* spec1 
			= &m_clanDownload->specs[ s_NumDownloadSpecsPerClan * clanNum ];

		// verify that the stats returned from live matche what is expected
		if( spec1->dwNumStats != spec1StatCount )
		{
			ASSERT( spec1->dwNumStats == spec1StatCount );
			return E_FAIL;
		}

		// go through each stat spec and extract the data
		for( int specNum = 0; specNum < s_NumDownloadSpecsPerClan; specNum++ )
		{
			XONLINE_STAT_SPEC& spec 
				= m_clanDownload->specs
				[ (s_NumDownloadSpecsPerClan * clanNum) + specNum ];
			
			OSIClanID id;
			XUID_to_OSIClanID( spec.xuidUser, id );
			OSIClanInfo* clanInfo = m_clanInfoList->findClanInfoByID( id );
			if( !clanInfo )
			{
				ASSERT( clanInfo );
				return E_FAIL;
			}

			OSIClanStats::StatValueType (&clanStatsArray)
				[ OSIClanStats::NUM_CLANSTATS ] = clanInfo->stats.statsArray;

			// go through each stat and extract the data
			for( unsigned int stat = 0; stat < spec.dwNumStats; stat++ )
			{
				WORD osiStatID;
				OSIClanStats::StatValueType osiStatValue;
				ConvertClanXboxStatToOSI(
					spec.pStats[ stat ], 
					osiStatID, 
					osiStatValue );
				clanStatsArray[ osiStatID ] = osiStatValue;
			}
		}

		++m_clanDownload->numFinished;
	}

	return S_OK;
}

//-----------------------------------------------------------------------------
HRESULT 
StatsAdapter::
BeginDownloadPlayerLadder( 
	XONLINETASK_HANDLE& handle, 
	OSIPlayerID& pivotPlayer,  // overloaded
	const DWORD numRanks,
	OSIPlayerStatsList* playerList )
//-----------------------------------------------------------------------------
{
	if( !playerList )
	{
		ASSERT( playerList );
		return S_OK;
	}

	if( !m_ladderDownload )
	{
		m_ladderDownload = new DownloadLadderData;

		#ifdef _DEBUG
		m_ladderDownload->instanceNumberAtBegin = s_statsAdapterInstanceNumber;
		#endif //_DEBUG
	}

	m_playerList = playerList;
	m_playerList->clear();

	OSIPlayerID_to_XUID( pivotPlayer, m_ladderDownload->pivotXUID );
	m_ladderDownload->firstRank = -1;
	m_ladderDownload->numRanks = numRanks;
	
	if( m_ladderDownload->users ) { delete m_ladderDownload->users; }
	m_ladderDownload->users = new XONLINE_STAT_USER[ numRanks ]; // deleted here or in ~DownloadLadderData

	return XOnlineStatLeaderEnumerate( 
		&m_ladderDownload->pivotXUID,
		0, // ignored since xuid specified
		numRanks,
		s_LeaderboardIdForPlayerStats0to63,
		0,
		NULL,
		0,
		&handle );
}

//-----------------------------------------------------------------------------
HRESULT 
StatsAdapter::
BeginDownloadPlayerLadder( 
	XONLINETASK_HANDLE& handle, 
	const DWORD firstRank, // overloaded
	const DWORD numRanks,
	OSIPlayerStatsList* playerList )
//-----------------------------------------------------------------------------
{
	if( !playerList )
	{
		ASSERT( playerList );
		return S_OK;
	}

	if( !m_ladderDownload  )
	{
		m_ladderDownload = new DownloadLadderData;

		#ifdef _DEBUG
		m_ladderDownload->instanceNumberAtBegin = s_statsAdapterInstanceNumber;
		#endif //_DEBUG
	}

	m_playerList = playerList;
	m_playerList->clear();

	m_ladderDownload->firstRank = firstRank;
	m_ladderDownload->numRanks = numRanks;
	
	if( m_ladderDownload->users ) { delete m_ladderDownload->users; }
	m_ladderDownload->users = new XONLINE_STAT_USER[ numRanks ];

	return XOnlineStatLeaderEnumerate( 
		0,// ignored since firstRank specified
		m_ladderDownload->firstRank, 
		m_ladderDownload->numRanks,
		s_LeaderboardIdForPlayerStats0to63,
		0,
		NULL,
		0,
		&handle );
}

//-----------------------------------------------------------------------------
HRESULT 
StatsAdapter::
FinishDownloadPlayerLadder( 
	const XONLINETASK_HANDLE& handle )
//-----------------------------------------------------------------------------
{
	DWORD leaderboardSize;
	DWORD numResultsReturned;
	HRESULT hr = XOnlineStatLeaderEnumerateGetResults(
		handle,
		m_ladderDownload->numRanks,
		m_ladderDownload->users,
		0,
		0,
		&leaderboardSize,
		&numResultsReturned,
		0,
		0 );
    XOnlineTaskClose( handle );
	if( FAILED( hr ) )
	{
		return hr;
	}

	ASSERT( m_playerList );
	m_playerList->clear();
	for( DWORD i = 0; i < numResultsReturned; i++ )
	{
		OSIPlayerStats p;
		XONLINE_STAT_USER* u = &m_ladderDownload->users[ i ];
		XUID_to_OSIPlayerID( u->xuidUser, p.playerID );
		OSIstrncpy( p.name, u->szGamertag, NUM_ARRAY_ELEMENTS( p.name ) );
		m_playerList->push_back( p );
	}
	return S_OK;
}

//-----------------------------------------------------------------------------
HRESULT
StatsAdapter::
BeginDownloadClanLadder(
	XONLINETASK_HANDLE& handle,
	const DWORD firstRank,
	const DWORD numRanks,
	OSIClanInfoList* clanList )
//-----------------------------------------------------------------------------
{
	if( !clanList )
	{
		ASSERT( clanList );
		return S_OK;
	}

	if( !m_ladderDownload  )
	{
		m_ladderDownload = new DownloadLadderData;

		#ifdef _DEBUG
		m_ladderDownload->instanceNumberAtBegin = s_statsAdapterInstanceNumber;
		#endif //_DEBUG
	}

	m_clanInfoList = clanList;
	m_clanInfoList->clear();

	m_ladderDownload->firstRank = firstRank;
	m_ladderDownload->numRanks = numRanks;
	
	if( m_ladderDownload->users ) { delete m_ladderDownload->users; }
	m_ladderDownload->users = new XONLINE_STAT_USER[ numRanks ];

	WORD xboxStatIDs[ s_NumDownloadStatsPerClan ];
	for( int i = 0; i < s_NumDownloadStatsPerClan; ++i )
	{
		xboxStatIDs[ i ]
			= ConvertClanOsiStatIDToXbox( s_ClanStatsToDownload[ i ] );
	}

	return XOnlineStatLeaderEnumerate( 
		0,// ignored since firstRank specified
		m_ladderDownload->firstRank, 
		m_ladderDownload->numRanks,
		s_LeaderboardIdForClanStats0to63,
		s_NumDownloadStatsPerClan,
		xboxStatIDs,
		0,
		&handle );
}

//-----------------------------------------------------------------------------
HRESULT
StatsAdapter::
FinishDownloadClanLadder(
	const XONLINETASK_HANDLE& handle )
//-----------------------------------------------------------------------------
{
	DWORD leaderboardSize;
	DWORD numResultsReturned;
	
	const DWORD statArrayCount = s_NumDownloadStatsPerClan * m_ladderDownload->numRanks;
	XONLINE_STAT* statArray = new XONLINE_STAT[ statArrayCount ];

	HRESULT hr = XOnlineStatLeaderEnumerateGetResults(
		handle,
		m_ladderDownload->numRanks,
		m_ladderDownload->users,
		statArrayCount,
		statArray,
		&leaderboardSize,
		&numResultsReturned,
		0,
		0 );
    XOnlineTaskClose( handle );
	if( FAILED( hr ) )
	{
		delete [] statArray;
		return hr;
	}

	// fill the claninfolist with all the stat data
	ASSERT( m_clanInfoList );
	m_clanInfoList->clear();
	for( DWORD resultIndex = 0; resultIndex < numResultsReturned; resultIndex++ )
	{
		OSIClanInfo clan;
		XONLINE_STAT_USER* u = &m_ladderDownload->users[ resultIndex ];
		XUID_to_OSIClanID( u->xuidUser, clan.clanID );
		OSIstrncpy( clan.name, u->wszTeamName, NUM_ARRAY_ELEMENTS( clan.name ) );

		XONLINE_STAT* const statPtr = &statArray[ resultIndex * s_NumDownloadStatsPerClan ] ;
		for( int statIndex = 0; statIndex < s_NumDownloadStatsPerClan; ++statIndex )
		{
			WORD osiStatID;
			OSIClanStats::StatValueType osiStatValue;
			ConvertClanXboxStatToOSI(
				statPtr[ statIndex ], 
				osiStatID, 
				osiStatValue );
			clan.stats.statsArray[ osiStatID ] = osiStatValue;
		}

		m_clanInfoList->push_back( clan );
	}

	delete [] statArray;

	return S_OK;
}

//-----------------------------------------------------------------------------
const WORD
StatsAdapter::s_ClanIncStatsToUpload[ StatsAdapter::s_NumUploadIncStatsPerClan ] = 
{
	OSIClanStats::CLANSTAT_RATING,
	OSIClanStats::CLANSTAT_KILLS
};

//-----------------------------------------------------------------------------
const WORD
StatsAdapter::s_ClanStatsToDownload[ s_NumDownloadStatsPerClan ] =
{
	OSIClanStats::CLANSTAT_RANK,
	OSIClanStats::CLANSTAT_RATING,
	OSIClanStats::CLANSTAT_KILLS
};

//-----------------------------------------------------------------------------
const WORD
StatsAdapter::s_PlayerStatsToDownload[ StatsAdapter::s_NumDownloadStatsPerPlayer ] = 
{
	OSIPlayerStats::PLAYERSTAT_USER_RATING,
	OSIPlayerStats::PLAYERSTAT_COP_RATING,
	OSIPlayerStats::PLAYERSTAT_GANG_RATING,
	OSIPlayerStats::PLAYERSTAT_COP_WINS,
	OSIPlayerStats::PLAYERSTAT_COP_GAMES_PLAYED,
	//OSIPlayerStats::PLAYERSTAT_COP_PLAYERS_IN_ROUND,
	OSIPlayerStats::PLAYERSTAT_GANG_WINS,
	OSIPlayerStats::PLAYERSTAT_GANG_GAMES_PLAYED,
	//OSIPlayerStats::PLAYERSTAT_GANG_PLAYERS_IN_ROUND,
	OSIPlayerStats::PLAYERSTAT_KILLS_AS_COP,
	OSIPlayerStats::PLAYERSTAT_DEATHS_AS_COP,
	OSIPlayerStats::PLAYERSTAT_KILLS_AS_GANGSTA,
	OSIPlayerStats::PLAYERSTAT_DEATHS_AS_GANGSTA,
	OSIPlayerStats::PLAYERSTAT_COP_MVPS,
	OSIPlayerStats::PLAYERSTAT_GANG_MVPS,
	OSIPlayerStats::PLAYERSTAT_NA_RANK,
	OSIPlayerStats::PLAYERSTAT_NA_COP_WINS,
	OSIPlayerStats::PLAYERSTAT_NA_COP_LOSSES,
	OSIPlayerStats::PLAYERSTAT_NA_COP_TIES,
	OSIPlayerStats::PLAYERSTAT_NA_GANG_WINS,
	OSIPlayerStats::PLAYERSTAT_NA_GANG_LOSSES,
	OSIPlayerStats::PLAYERSTAT_NA_GANG_TIES,
	OSIPlayerStats::PLAYERSTAT_NA_COP_KILLS,
	OSIPlayerStats::PLAYERSTAT_NA_GANG_KILLS,
	OSIPlayerStats::PLAYERSTAT_NA_COP_DEATHS,
	OSIPlayerStats::PLAYERSTAT_NA_GANG_DEATHS,
	OSIPlayerStats::PLAYERSTAT_NA_COP_SUICIDES,
	OSIPlayerStats::PLAYERSTAT_NA_GANG_SUICIDES,
	OSIPlayerStats::PLAYERSTAT_NA_COP_HEADSHOTS,
	OSIPlayerStats::PLAYERSTAT_NA_GANG_HEADSHOTS,
	OSIPlayerStats::PLAYERSTAT_NA_COP_TEAM_KILLS,
	OSIPlayerStats::PLAYERSTAT_NA_GANG_TEAM_KILLS,
	OSIPlayerStats::PLAYERSTAT_NA_COP_TEAM_DEATHS,
	OSIPlayerStats::PLAYERSTAT_NA_GANG_TEAM_DEATHS,
	OSIPlayerStats::PLAYERSTAT_NA_COP_NPC_KILLS,
	OSIPlayerStats::PLAYERSTAT_NA_GANG_NPC_KILLS,
	OSIPlayerStats::PLAYERSTAT_NA_APPREHENSIONS,
	OSIPlayerStats::PLAYERSTAT_NA_APPREHENDED,
	OSIPlayerStats::PLAYERSTAT_NA_COP_SHOTS_FIRED,
	OSIPlayerStats::PLAYERSTAT_NA_GANG_SHOTS_FIRED,
	OSIPlayerStats::PLAYERSTAT_NA_COP_SHOTS_HIT,
	OSIPlayerStats::PLAYERSTAT_NA_GANG_SHOTS_HIT,
	//OSIPlayerStats::PLAYERSTAT_NA_COP_TIME_PLAYED,
	//OSIPlayerStats::PLAYERSTAT_NA_GANG_TIME_PLAYED,
	OSIPlayerStats::PLAYERSTAT_NA_LOOT_SCORES,
	OSIPlayerStats::PLAYERSTAT_NA_RAID_SCORES,
	OSIPlayerStats::PLAYERSTAT_NA_TAG_SCORES,
	//OSIPlayerStats::PLAYERSTAT_NA_COP_MELEE,
	//OSIPlayerStats::PLAYERSTAT_NA_COP_GRENADE,
	//OSIPlayerStats::PLAYERSTAT_NA_COP_SPECIAL,
	//OSIPlayerStats::PLAYERSTAT_NA_COP_SUB_MACHINEGUN,
	//OSIPlayerStats::PLAYERSTAT_NA_COP_HEAVY_MACHINEGUN,
	//OSIPlayerStats::PLAYERSTAT_NA_COP_SHOTGUN,
	//OSIPlayerStats::PLAYERSTAT_NA_GANG_MELEE,
	//OSIPlayerStats::PLAYERSTAT_NA_GANG_GRENADE,
	//OSIPlayerStats::PLAYERSTAT_NA_GANG_SPECIAL,
	//OSIPlayerStats::PLAYERSTAT_NA_GANG_SUB_MACHINEGUN,
	//OSIPlayerStats::PLAYERSTAT_NA_GANG_HEAVY_MACHINEGUN,
	//OSIPlayerStats::PLAYERSTAT_NA_GANG_SHOTGUN,
	//OSIPlayerStats::PLAYERSTAT_NA_COP_DEATH_HOLDING_MELEE,
	//OSIPlayerStats::PLAYERSTAT_NA_COP_DEATH_HOLDING_GRENADE,
	//OSIPlayerStats::PLAYERSTAT_NA_COP_DEATH_HOLDING_SPECIAL,
	//OSIPlayerStats::PLAYERSTAT_NA_COP_DEATH_HOLDING_SUB_MACHINEGUN,
	//OSIPlayerStats::PLAYERSTAT_NA_COP_DEATH_HOLDING_HEAVY_MACHINEGUN,
	//OSIPlayerStats::PLAYERSTAT_NA_COP_DEATH_HOLDING_SHOTGUN,
	//OSIPlayerStats::PLAYERSTAT_NA_GANG_DEATH_HOLDING_MELEE,
	//OSIPlayerStats::PLAYERSTAT_NA_GANG_DEATH_HOLDING_GRENADE,
	//OSIPlayerStats::PLAYERSTAT_NA_GANG_DEATH_HOLDING_SPECIAL,
	//OSIPlayerStats::PLAYERSTAT_NA_GANG_DEATH_HOLDING_SUB_MACHINEGUN,
	//OSIPlayerStats::PLAYERSTAT_NA_GANG_DEATH_HOLDING_HEAVY_MACHINEGUN,
	//OSIPlayerStats::PLAYERSTAT_NA_GANG_DEATH_HOLDING_SHOTGUN,
	//OSIPlayerStats::PLAYERSTAT_NA_COP_DEATH_BY_MELEE,
	//OSIPlayerStats::PLAYERSTAT_NA_COP_DEATH_BY_GRENADE,
	//OSIPlayerStats::PLAYERSTAT_NA_COP_DEATH_BY_SPECIAL,
	//OSIPlayerStats::PLAYERSTAT_NA_COP_DEATH_BY_SUB_MACHINEGUN,
	//OSIPlayerStats::PLAYERSTAT_NA_COP_DEATH_BY_HEAVY_MACHINEGUN,
	//OSIPlayerStats::PLAYERSTAT_NA_COP_DEATH_BY_SHOTGUN,
	//OSIPlayerStats::PLAYERSTAT_NA_GANG_DEATH_BY_MELEE,
	//OSIPlayerStats::PLAYERSTAT_NA_GANG_DEATH_BY_GRENADE,
	//OSIPlayerStats::PLAYERSTAT_NA_GANG_DEATH_BY_SPECIAL,
	//OSIPlayerStats::PLAYERSTAT_NA_GANG_DEATH_BY_SUB_MACHINEGUN,
	//OSIPlayerStats::PLAYERSTAT_NA_GANG_DEATH_BY_HEAVY_MACHINEGUN,
	//OSIPlayerStats::PLAYERSTAT_NA_GANG_DEATH_BY_SHOTGUN,
	//OSIPlayerStats::PLAYERSTAT_NA_TAG_WINS,
	OSIPlayerStats::PLAYERSTAT_NA_TAG_GAMES_PLAYED,
	//OSIPlayerStats::PLAYERSTAT_NA_TAG_TIME,
	//OSIPlayerStats::PLAYERSTAT_NA_WAR_WINS,
	OSIPlayerStats::PLAYERSTAT_NA_WAR_GAMES_PLAYED,
	//OSIPlayerStats::PLAYERSTAT_NA_WAR_TIME,
	//OSIPlayerStats::PLAYERSTAT_NA_ROBBERY_WINS,
	OSIPlayerStats::PLAYERSTAT_NA_ROBBERY_GAMES_PLAYED,
	//OSIPlayerStats::PLAYERSTAT_NA_ROBBERY_TIME,
	//OSIPlayerStats::PLAYERSTAT_NA_RAID_WINS,
	OSIPlayerStats::PLAYERSTAT_NA_RAID_GAMES_PLAYED,
	//OSIPlayerStats::PLAYERSTAT_NA_RAID_TIME,
	//OSIPlayerStats::PLAYERSTAT_NA_MODE5_WINS,
	OSIPlayerStats::PLAYERSTAT_NA_MODE5_GAMES_PLAYED,
	//OSIPlayerStats::PLAYERSTAT_NA_MODE5_TIME,
	//OSIPlayerStats::PLAYERSTAT_NA_MODE6_WINS,
	OSIPlayerStats::PLAYERSTAT_NA_MODE6_GAMES_PLAYED,
	//OSIPlayerStats::PLAYERSTAT_NA_MODE6_TIME,
	//OSIPlayerStats::PLAYERSTAT_NA_INCOMPLETES,
};

//-----------------------------------------------------------------------------
const WORD 
StatsAdapter::s_PlayerIncStatsToUpload[ StatsAdapter::s_NumUploadIncStatsPerPlayer ] = 
//-----------------------------------------------------------------------------
{
	OSIPlayerStats::PLAYERSTAT_USER_RATING,
	OSIPlayerStats::PLAYERSTAT_COP_RATING,
	OSIPlayerStats::PLAYERSTAT_GANG_RATING,
	OSIPlayerStats::PLAYERSTAT_COP_WINS,
	OSIPlayerStats::PLAYERSTAT_COP_GAMES_PLAYED,
	OSIPlayerStats::PLAYERSTAT_COP_PLAYERS_IN_ROUND,
	OSIPlayerStats::PLAYERSTAT_GANG_WINS,
	OSIPlayerStats::PLAYERSTAT_GANG_GAMES_PLAYED,
	OSIPlayerStats::PLAYERSTAT_GANG_PLAYERS_IN_ROUND,
	OSIPlayerStats::PLAYERSTAT_KILLS_AS_COP,
	OSIPlayerStats::PLAYERSTAT_DEATHS_AS_COP,
	OSIPlayerStats::PLAYERSTAT_KILLS_AS_GANGSTA,
	OSIPlayerStats::PLAYERSTAT_DEATHS_AS_GANGSTA,
	OSIPlayerStats::PLAYERSTAT_COP_MVPS,
	OSIPlayerStats::PLAYERSTAT_GANG_MVPS,
	//PLAYERSTAT_NA_RANK = FIRST_NONARBITRATED_STAT,  don't upload rank
	OSIPlayerStats::PLAYERSTAT_NA_COP_WINS,
	OSIPlayerStats::PLAYERSTAT_NA_COP_LOSSES,
	OSIPlayerStats::PLAYERSTAT_NA_COP_TIES,
	OSIPlayerStats::PLAYERSTAT_NA_GANG_WINS,
	OSIPlayerStats::PLAYERSTAT_NA_GANG_LOSSES,
	OSIPlayerStats::PLAYERSTAT_NA_GANG_TIES,
	OSIPlayerStats::PLAYERSTAT_NA_COP_KILLS,
	OSIPlayerStats::PLAYERSTAT_NA_GANG_KILLS,
	OSIPlayerStats::PLAYERSTAT_NA_COP_DEATHS,
	OSIPlayerStats::PLAYERSTAT_NA_GANG_DEATHS,
	OSIPlayerStats::PLAYERSTAT_NA_COP_SUICIDES,
	OSIPlayerStats::PLAYERSTAT_NA_GANG_SUICIDES,
	OSIPlayerStats::PLAYERSTAT_NA_COP_HEADSHOTS,
	OSIPlayerStats::PLAYERSTAT_NA_GANG_HEADSHOTS,
	OSIPlayerStats::PLAYERSTAT_NA_COP_TEAM_KILLS,
	OSIPlayerStats::PLAYERSTAT_NA_GANG_TEAM_KILLS,
	OSIPlayerStats::PLAYERSTAT_NA_COP_TEAM_DEATHS,
	OSIPlayerStats::PLAYERSTAT_NA_GANG_TEAM_DEATHS,
	OSIPlayerStats::PLAYERSTAT_NA_COP_NPC_KILLS,
	OSIPlayerStats::PLAYERSTAT_NA_GANG_NPC_KILLS,
	OSIPlayerStats::PLAYERSTAT_NA_APPREHENSIONS,
	OSIPlayerStats::PLAYERSTAT_NA_APPREHENDED,
	OSIPlayerStats::PLAYERSTAT_NA_COP_SHOTS_FIRED,
	OSIPlayerStats::PLAYERSTAT_NA_GANG_SHOTS_FIRED,
	OSIPlayerStats::PLAYERSTAT_NA_COP_SHOTS_HIT,
	OSIPlayerStats::PLAYERSTAT_NA_GANG_SHOTS_HIT,
	OSIPlayerStats::PLAYERSTAT_NA_COP_TIME_PLAYED,
	OSIPlayerStats::PLAYERSTAT_NA_GANG_TIME_PLAYED,
	OSIPlayerStats::PLAYERSTAT_NA_LOOT_SCORES,
	OSIPlayerStats::PLAYERSTAT_NA_RAID_SCORES,
	OSIPlayerStats::PLAYERSTAT_NA_TAG_SCORES,
	OSIPlayerStats::PLAYERSTAT_NA_COP_MELEE,
	OSIPlayerStats::PLAYERSTAT_NA_COP_GRENADE,
	OSIPlayerStats::PLAYERSTAT_NA_COP_SPECIAL,
	OSIPlayerStats::PLAYERSTAT_NA_COP_SUB_MACHINEGUN,
	OSIPlayerStats::PLAYERSTAT_NA_COP_HEAVY_MACHINEGUN,
	OSIPlayerStats::PLAYERSTAT_NA_COP_SHOTGUN,
	OSIPlayerStats::PLAYERSTAT_NA_GANG_MELEE,
	OSIPlayerStats::PLAYERSTAT_NA_GANG_GRENADE,
	OSIPlayerStats::PLAYERSTAT_NA_GANG_SPECIAL,
	OSIPlayerStats::PLAYERSTAT_NA_GANG_SUB_MACHINEGUN,
	OSIPlayerStats::PLAYERSTAT_NA_GANG_HEAVY_MACHINEGUN,
	OSIPlayerStats::PLAYERSTAT_NA_GANG_SHOTGUN,
	OSIPlayerStats::PLAYERSTAT_NA_COP_DEATH_HOLDING_MELEE,
	OSIPlayerStats::PLAYERSTAT_NA_COP_DEATH_HOLDING_GRENADE,
	OSIPlayerStats::PLAYERSTAT_NA_COP_DEATH_HOLDING_SPECIAL,
	OSIPlayerStats::PLAYERSTAT_NA_COP_DEATH_HOLDING_SUB_MACHINEGUN,
	OSIPlayerStats::PLAYERSTAT_NA_COP_DEATH_HOLDING_HEAVY_MACHINEGUN,
	OSIPlayerStats::PLAYERSTAT_NA_COP_DEATH_HOLDING_SHOTGUN,
	OSIPlayerStats::PLAYERSTAT_NA_GANG_DEATH_HOLDING_MELEE,
	OSIPlayerStats::PLAYERSTAT_NA_GANG_DEATH_HOLDING_GRENADE,
	OSIPlayerStats::PLAYERSTAT_NA_GANG_DEATH_HOLDING_SPECIAL,
	OSIPlayerStats::PLAYERSTAT_NA_GANG_DEATH_HOLDING_SUB_MACHINEGUN,
	OSIPlayerStats::PLAYERSTAT_NA_GANG_DEATH_HOLDING_HEAVY_MACHINEGUN,
	OSIPlayerStats::PLAYERSTAT_NA_GANG_DEATH_HOLDING_SHOTGUN,
	OSIPlayerStats::PLAYERSTAT_NA_COP_DEATH_BY_MELEE,
	OSIPlayerStats::PLAYERSTAT_NA_COP_DEATH_BY_GRENADE,
	OSIPlayerStats::PLAYERSTAT_NA_COP_DEATH_BY_SPECIAL,
	OSIPlayerStats::PLAYERSTAT_NA_COP_DEATH_BY_SUB_MACHINEGUN,
	OSIPlayerStats::PLAYERSTAT_NA_COP_DEATH_BY_HEAVY_MACHINEGUN,
	OSIPlayerStats::PLAYERSTAT_NA_COP_DEATH_BY_SHOTGUN,
	OSIPlayerStats::PLAYERSTAT_NA_GANG_DEATH_BY_MELEE,
	OSIPlayerStats::PLAYERSTAT_NA_GANG_DEATH_BY_GRENADE,
	OSIPlayerStats::PLAYERSTAT_NA_GANG_DEATH_BY_SPECIAL,
	OSIPlayerStats::PLAYERSTAT_NA_GANG_DEATH_BY_SUB_MACHINEGUN,
	OSIPlayerStats::PLAYERSTAT_NA_GANG_DEATH_BY_HEAVY_MACHINEGUN,
	OSIPlayerStats::PLAYERSTAT_NA_GANG_DEATH_BY_SHOTGUN,
	OSIPlayerStats::PLAYERSTAT_NA_TAG_WINS,
	OSIPlayerStats::PLAYERSTAT_NA_TAG_GAMES_PLAYED,
	OSIPlayerStats::PLAYERSTAT_NA_TAG_TIME,
	OSIPlayerStats::PLAYERSTAT_NA_WAR_WINS,
	OSIPlayerStats::PLAYERSTAT_NA_WAR_GAMES_PLAYED,
	OSIPlayerStats::PLAYERSTAT_NA_WAR_TIME,
	OSIPlayerStats::PLAYERSTAT_NA_ROBBERY_WINS,
	OSIPlayerStats::PLAYERSTAT_NA_ROBBERY_GAMES_PLAYED,
	OSIPlayerStats::PLAYERSTAT_NA_ROBBERY_TIME,
	OSIPlayerStats::PLAYERSTAT_NA_RAID_WINS,
	OSIPlayerStats::PLAYERSTAT_NA_RAID_GAMES_PLAYED,
	OSIPlayerStats::PLAYERSTAT_NA_RAID_TIME,
	OSIPlayerStats::PLAYERSTAT_NA_MODE5_WINS,
	OSIPlayerStats::PLAYERSTAT_NA_MODE5_GAMES_PLAYED,
	OSIPlayerStats::PLAYERSTAT_NA_MODE5_TIME,
	OSIPlayerStats::PLAYERSTAT_NA_MODE6_WINS,
	OSIPlayerStats::PLAYERSTAT_NA_MODE6_GAMES_PLAYED,
	OSIPlayerStats::PLAYERSTAT_NA_MODE6_TIME,
	OSIPlayerStats::PLAYERSTAT_NA_INCOMPLETES,
};


