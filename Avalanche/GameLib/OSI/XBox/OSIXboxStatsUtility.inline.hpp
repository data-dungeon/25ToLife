#pragma once


//-----------------------------------------------------------------------------
StatsAdapter::DownloadLadderData::DownloadLadderData()
:	firstRank( -1 ),
	numRanks( 0 ),
	users( 0 )
{
	pivotXUID.qwUserID = (ULONGLONG) 0;
	pivotXUID.dwUserFlags = 0;
}

//-----------------------------------------------------------------------------
StatsAdapter::DownloadLadderData::~DownloadLadderData()
{
	if( users ) { delete [] users; }
}


//-----------------------------------------------------------------------------
StatsAdapter::StatsAdapter()
:	m_upload( 0 ),
	m_playerDownload( 0 ),
	m_clanDownload( 0 ),
	m_ladderDownload( 0 ),
	m_playerList( 0 ),
	m_clanInfoList( 0 )
{
#ifdef _DEBUG
	++s_statsAdapterInstanceNumber;
#endif
}

//-----------------------------------------------------------------------------
StatsAdapter::~StatsAdapter()
{
	if( m_upload )			{ delete m_upload; }
	if( m_playerDownload )	{ delete m_playerDownload; }
	if( m_clanDownload )	{ delete m_clanDownload; }
	if( m_ladderDownload )	{ delete m_ladderDownload; }
}




//-----------------------------------------------------------------------------
//static 
void StatsAdapter::OSICreateSingleton() 
{ 
	ASSERT( !StatsAdapter::m_singletonPtr ); 
	m_singletonPtr = new StatsAdapter; 
}

//-----------------------------------------------------------------------------
//static 
void StatsAdapter::OSIDestroySingleton() 
{ 
	ASSERT( StatsAdapter::m_singletonPtr ); 
	delete m_singletonPtr; 
	m_singletonPtr = 0; 
}

//-----------------------------------------------------------------------------
//static
StatsAdapter* StatsAdapter::OSIGetSingleton() 
{ 
	return m_singletonPtr; 
}

//-----------------------------------------------------------------------------
//static 
WORD
StatsAdapter::ConvertPlayerOsiStatIDToXbox( 
	const WORD osiStatID )
{
	if( ((DWORD) osiStatID) >= OSIPlayerStats::NUM_PLAYERSTATS )
	{
		ASSERT( ((DWORD) osiStatID) < OSIPlayerStats::NUM_PLAYERSTATS );
		return s_DummyStatID; // return something that isn't displayed in any way
	}

	switch( osiStatID )
	{
	case OSIPlayerStats::PLAYERSTAT_USER_RATING:	return XONLINE_STAT_RATING;
	case OSIPlayerStats::PLAYERSTAT_NA_RANK:		return XONLINE_STAT_RANK;
	default:										return (WORD) ((osiStatID % 64) + 1); // 0 is an invalid id on Live.
	};
}

//-----------------------------------------------------------------------------
//static 
WORD 
StatsAdapter::ConvertPlayerXboxStatIDToOsi( 
	const WORD xboxStatID,
	const DWORD leaderboardID )
{
	switch( xboxStatID )
	{
	case XONLINE_STAT_RATING:	return OSIPlayerStats::PLAYERSTAT_USER_RATING;
	case XONLINE_STAT_RANK:		return OSIPlayerStats::PLAYERSTAT_NA_RANK;
	default:					
		if( ((DWORD)xboxStatID) == 0 || ((DWORD)xboxStatID) > OSIPlayerStats::NUM_PLAYERSTATS )
		{
			ASSERT( ((DWORD) xboxStatID) <= OSIPlayerStats::NUM_PLAYERSTATS );
			return (WORD) s_DummyStatID; // return something that isn't displayed in any way
		}
		else
		{
			return (WORD) (((leaderboardID - 1) * 64) + (xboxStatID - 1)); // 0 is an invalid id on Live.
		}
	};
}

//-----------------------------------------------------------------------------
//static 
void 
StatsAdapter::ConvertPlayerOsiStatToXbox( 
	XONLINE_STAT& stat, 
	const WORD osiStatID, 
	const OSIPlayerStats::StatValueType value )
{
	WORD xboxStatID = ConvertPlayerOsiStatIDToXbox( osiStatID );
	switch( xboxStatID )
	{
	case XONLINE_STAT_RATING:
		FillStatLongLong( stat, xboxStatID, (LONGLONG) value ); 
		break;
	default:
		FillStatLong( stat, xboxStatID, (LONG) value ); 
		break;
	};
}

//-----------------------------------------------------------------------------
//static 
OSIPlayerStats::StatValueType
StatsAdapter::GetDefaultPlayerStatValue( const WORD& id )
{
	//switch( id )
	//{
	//case OSIPlayerStats::PLAYERSTAT_USER_RATING:	return 1600;
	//case OSIPlayerStats::PLAYERSTAT_NA_RANK:		return 9999;
	//default:										return 0;
	//};
	return 0;
}

//-----------------------------------------------------------------------------
//static 
void 
StatsAdapter::ConvertPlayerXboxStatToOSI( 
	XONLINE_STAT& stat,
	const DWORD leaderboardID,
	WORD& osiStatID,
	OSIPlayerStats::StatValueType& value )
{
	osiStatID = ConvertPlayerXboxStatIDToOsi( stat.wID, leaderboardID );
	switch( stat.type )
	{
	case XONLINE_STAT_LONG:
		value = (OSIPlayerStats::StatValueType) stat.lValue;
		break;
	case XONLINE_STAT_LONGLONG:
		value = (OSIPlayerStats::StatValueType) stat.llValue;
		break;
	case XONLINE_STAT_NONE:
		value = GetDefaultPlayerStatValue( osiStatID );
		break;
	default:
		ASSERT( stat.type == XONLINE_STAT_LONG || stat.type == XONLINE_STAT_LONGLONG );
		value = 0;
		break;
	};

	if( (osiStatID == OSIPlayerStats::PLAYERSTAT_GANG_RATING)
		|| (osiStatID == OSIPlayerStats::PLAYERSTAT_COP_RATING)
		|| (osiStatID == OSIPlayerStats::PLAYERSTAT_USER_RATING) )
	{
		value += 1600;
	}
}

//-----------------------------------------------------------------------------
//static 
DWORD
StatsAdapter::FillPlayerStats( 
	XONLINE_STAT* statArray,
	const DWORD leaderboardId,
	const WORD* const statOsiIdArray,
	const DWORD statOsiIdArrayCount,
	const OSIPlayerStats* playerStats )
{
	DWORD statCopyCount = 0; // counts stats that match the leaderboard and are copied
	const int statIdMin = (leaderboardId - 1) * s_MaxStatsPerLeaderboard; // stats on the requested leaderboard have (max >= id >= min)
	const int statIdMax = statIdMin + s_MaxStatsPerLeaderboard - 1;
	for( DWORD i = 0; i < statOsiIdArrayCount; i++ )
	{
		WORD osiStatId = statOsiIdArray[ i ];
		if( ! ((osiStatId >= statIdMin) && (osiStatId <= statIdMax)) )
		{
			continue;
		}
		
		XONLINE_STAT& xstat = statArray[ statCopyCount ];
		OSIPlayerStats::StatValueType value;
		if( playerStats )
		{
			value = playerStats->statsArray[ osiStatId ];
		}
		else
		{
			value = 0;
		}
		ConvertPlayerOsiStatToXbox( xstat, osiStatId, value );
		++statCopyCount;
	}
	return statCopyCount;
}


//-----------------------------------------------------------------------------
//static 
DWORD
StatsAdapter::CountPlayerStats(
	const DWORD leaderboardId,
	const WORD* statOsiIdArray,
	const DWORD statOsiIdArrayCount )
{
	DWORD count = 0;
	const int statIdMin = (leaderboardId - 1) * s_MaxStatsPerLeaderboard; // stats on the requested leaderboard have (max >= id >= min)
	const int statIdMax = statIdMin + s_MaxStatsPerLeaderboard - 1;
	for( DWORD i = 0; i < statOsiIdArrayCount; i++ )
	{
		WORD osiStatId = statOsiIdArray[ i ];
		if( ! ((osiStatId >= statIdMin) && (osiStatId <= statIdMax)) )
		{
			continue;
		}
		++count;
	}
	return count;
}









//-----------------------------------------------------------------------------
//static 
OSIClanStats::StatValueType
StatsAdapter::GetDefaultClanStatValue( const WORD& id )
{
	//switch( id )
	//{
	//case OSIClanStats::CLANSTAT_RATING:		return 1600;
	//case OSIClanStats::CLANSTAT_RANK:		return 9999;
	//default:								return 0;
	//};
	return 0;
}

//-----------------------------------------------------------------------------
//static 
WORD
StatsAdapter::ConvertClanOsiStatIDToXbox( 
	const WORD osiStatID )
{
	if( ((DWORD) osiStatID) >= OSIClanStats::NUM_CLANSTATS )
	{
		ASSERT( ((DWORD) osiStatID) < OSIClanStats::NUM_CLANSTATS );
		return s_DummyStatID;
	}

	switch( osiStatID )
	{
	case OSIClanStats::CLANSTAT_RATING:		return XONLINE_STAT_RATING;
	case OSIClanStats::CLANSTAT_RANK:		return XONLINE_STAT_RANK;
	default:								return (WORD) (osiStatID + 1); // 0 is an invalid id on Live.
	};
}

//-----------------------------------------------------------------------------
//static 
WORD 
StatsAdapter::ConvertClanXboxStatIDToOsi( 
	const WORD xboxStatID )
{
	switch( xboxStatID )
	{
	case XONLINE_STAT_RATING:	return OSIClanStats::CLANSTAT_RATING;
	case XONLINE_STAT_RANK:		return OSIClanStats::CLANSTAT_RANK;
	default:					
		if( ((DWORD)xboxStatID) == 0 || ((DWORD)xboxStatID) > OSIClanStats::NUM_CLANSTATS )
		{
			ASSERT( ((DWORD) xboxStatID) <= OSIClanStats::NUM_CLANSTATS );
			return (WORD) s_DummyStatID;
		}
		else
		{
			return (WORD) (xboxStatID - 1); // 0 is an invalid id on Live.
		}
	};
}

//-----------------------------------------------------------------------------
//static 
void 
StatsAdapter::ConvertClanOsiStatToXbox( 
	XONLINE_STAT& stat, 
	const WORD osiStatID, 
	const OSIClanStats::StatValueType value )
{
	WORD xboxStatID = ConvertClanOsiStatIDToXbox( osiStatID );
	switch( xboxStatID )
	{
	case XONLINE_STAT_RATING:
		FillStatLongLong( stat, xboxStatID, (LONGLONG) value ); 
		break;
	default:
		FillStatLong( stat, xboxStatID, (LONG) value ); 
		break;
	};
}

//-----------------------------------------------------------------------------
//static 
void 
StatsAdapter::ConvertClanXboxStatToOSI( 
	XONLINE_STAT& stat,
	WORD& osiStatID,
	OSIClanStats::StatValueType& value )
{
	osiStatID = ConvertClanXboxStatIDToOsi( stat.wID );
	switch( stat.type )
	{
	case XONLINE_STAT_LONG:
		value = (OSIClanStats::StatValueType) stat.lValue;
		break;
	case XONLINE_STAT_LONGLONG:
		value = (OSIClanStats::StatValueType) stat.llValue;
		break;
	case XONLINE_STAT_NONE:
		value = GetDefaultClanStatValue( osiStatID );
		break;
	default:
		ASSERT( stat.type == XONLINE_STAT_LONG || stat.type == XONLINE_STAT_LONGLONG );
		value = 0;
		break;
	};

	if( osiStatID == OSIClanStats::CLANSTAT_RATING )
	{
		value += 1600;
	}
}

//-----------------------------------------------------------------------------
//static 
DWORD
StatsAdapter::FillClanStats( 
	XONLINE_STAT* statArray,
	const DWORD leaderboardId,
	const WORD* const statOsiIdArray,
	const DWORD statOsiIdArrayCount,
	const OSIClanStats* clanStats )
{
	DWORD statCopyCount = 0; // counts stats that match the leaderboard and are copied
	const int statIdMin = (leaderboardId - s_LeaderboardIdForClanStats0to63) * s_MaxStatsPerLeaderboard; // stats on the requested leaderboard have (max >= id >= min)
	const int statIdMax = statIdMin + s_MaxStatsPerLeaderboard - 1;
	for( DWORD i = 0; i < statOsiIdArrayCount; i++ )
	{
		WORD osiStatId = statOsiIdArray[ i ];
		if( ! ((osiStatId >= statIdMin) && (osiStatId <= statIdMax)) )
		{
			continue;
		}
		
		XONLINE_STAT& xstat = statArray[ statCopyCount ];
		OSIClanStats::StatValueType value;
		if( clanStats )
		{
			value = clanStats->statsArray[ osiStatId ];
		}
		else
		{
			value = 0;
		}
		ConvertClanOsiStatToXbox( xstat, osiStatId, value );
		++statCopyCount;
	}
	return statCopyCount;
}






//-----------------------------------------------------------------------------
//static 
void 
StatsAdapter::FillUpdate( 
	XONLINE_STAT_UPDATE& update, 
	const XUID xuid, 
	const DWORD leaderboardID, 
	const DWORD statCount, 
	XONLINE_STAT* const statPtr )
{
	ASSERT( statCount > 0 );
	ASSERT( statPtr );
	ASSERT( leaderboardID > 0 );
	ASSERT( xuid.qwUserID != 0 );
	update.xuid = xuid;
	update.dwLeaderBoardID = leaderboardID;
	update.dwConditionalIndex = 0;
	update.dwNumStats = statCount;
	update.pStats = statPtr;
}

//-----------------------------------------------------------------------------
//static 
void
StatsAdapter::FillSpec(
	XONLINE_STAT_SPEC& spec,
	const XUID xuid,
	const DWORD leaderboardID,
	const DWORD statCount,
	XONLINE_STAT* const statPtr )
{
	spec.xuidUser = xuid;
	spec.dwNumStats = statCount;
	spec.dwLeaderBoardID = leaderboardID;
	spec.pStats = statPtr;
}


//-----------------------------------------------------------------------------
//static 
void 
StatsAdapter::FillStatLong( 
	XONLINE_STAT& stat, 
	const WORD id, 
	const LONG value ) 
{
	stat.wID = id;
	stat.type = XONLINE_STAT_LONG;
	stat.lValue = value;
}

//-----------------------------------------------------------------------------
//static 
void 
StatsAdapter::FillStatLongLong( 
	XONLINE_STAT& stat, 
	const WORD id, 
	const LONGLONG value )
{
	stat.wID = id;
	stat.type = XONLINE_STAT_LONGLONG;
	stat.llValue = value;
}

//-----------------------------------------------------------------------------
bool
StatsAdapter::IsDownloadPlayersComplete() 
const
{
	if( !m_playerDownload ) 
	{ 
		return true; 
	}
	else 
	{ 
		#ifdef _DEBUG
		ASSERT( m_playerDownload->instanceNumberAtBegin 
			== s_statsAdapterInstanceNumber );
		#endif 
		return (m_playerDownload->numTotal == m_playerDownload->numFinished); 
	}
}

//-----------------------------------------------------------------------------
bool
StatsAdapter::IsDownloadClansComplete() 
const
{
	if( !m_clanDownload ) 
	{ 
		return true; 
	}
	else 
	{ 
		#ifdef _DEBUG
		ASSERT( m_clanDownload->instanceNumberAtBegin 
			== s_statsAdapterInstanceNumber );
		#endif 
		return (m_clanDownload->numTotal == m_clanDownload->numFinished); 
	}
}


//-----------------------------------------------------------------------------
bool 
StatsAdapter::IsDownloadComplete() 
const
{
	return IsDownloadPlayersComplete() && IsDownloadClansComplete();
}

