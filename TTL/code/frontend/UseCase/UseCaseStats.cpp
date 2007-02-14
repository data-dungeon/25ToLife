///////////////////////////////////////////////////////////////////////////////////////
///  \file UseCaseStats.cpp
///
///  \brief Implementation of the Stats class, which is responsible 
/// for presenting the Main Menu and allowing the player to pick what
/// they want to do.
///////////////////////////////////////////////////////////////////////////////////////

#include "TTLPCH.h"

#include "UseCaseStats.hpp"
#include "UseCaseDisplayDialog.hpp"
#include "UseCaseWaitForOSIRequest.hpp"
#include "frontend/frontend.h"
#include "../screens/screen_stats.h"

// static
const OnlineUI::Stats::DisplayStat OnlineUI::Stats::copStats[ NUM_COP_STATS ] = 
{
	USER_RATING,
	CLAN_RATING,
	SESSIONS,
	MVP_AWARDS,
	KILLS,
	HEADSHOTS,
	RAID_CAPTURES,
	DEATHS,
	SUICIDES,
	KILL_DEATH_RATIO,
	TEAM_KILLS,
	CLAN_KILLS,
	NPC_KILLS,
	ROUNDS_WON,
	ROUNDS_LOST,
	APPREHENSIONS,
	SHOTS_FIRED,
	SHOTS_HIT,
	ACCURACY,
	MOST_PLAYED_MODE
};


const OnlineUI::Stats::DisplayStat OnlineUI::Stats::gangStats[ NUM_GANG_STATS ] =
{
	USER_RATING,
	CLAN_RATING,
	SESSIONS,
	MVP_AWARDS,
	KILLS,
	HEADSHOTS,
	ROBBERY_ESCAPES,
	TAGS,
	DEATHS,
	SUICIDES,
	KILL_DEATH_RATIO,
	TEAM_KILLS,
	CLAN_KILLS,
	NPC_KILLS,
	ROUNDS_WON,
	ROUNDS_LOST,
	APPREHENDED,
	SHOTS_FIRED,
	SHOTS_HIT,
	ACCURACY,
	MOST_PLAYED_MODE
};

const OSIchar* const OnlineUI::Stats::s_statNames[ LAST_DISPLAYED_STAT ] =
{
	"User Rating (Rank)",
	"Clan Rating",
	"Sessions",
	"MVP Awards",
	"Kills",
	"Headshots",
	"Raid Captures",
	"Robbery Escapes",
	"Tags",
	"Deaths",
	"Suicides",
	"Kill Death Ratio",
	"Team Kills",
	"Clan Kills",
	"NPC Kills",
	"Rounds Won #LANG#A",	// #LANG#A prevents translation collision
	"Rounds Lost",
	"Apprehensions",
	"Apprehended",
	"Shots Fired",
	"Shots Hit",
	"Accuracy",
	"Most Played Mode"
};

const OSIchar* const OnlineUI::Stats::s_gameModeNames[ NUM_GAME_MODES ] = 
{
	"Tag", "War", "Robbery", "Raid", "Wipeout", "War"
};

const OnlineUI::Stats::State 
OnlineUI::Stats::s_stateToRunForSource[ ScreenStats::NUM_STAT_SOURCE ] =
{
	LADDER_STATS,			// STAT_SOURCE_PLAYERS,
	LADDER_STATS,			// STAT_SOURCE_TOP_PLAYERS,
	CLAN_MEMBER_PLAYERCLAN, // STAT_SOURCE_CLAN,
	CLAN_LADDER_STATS,		// STAT_SOURCE_TOP_CLANS,
	FRIEND_LIST,			// STAT_SOURCE_FRIENDS,
};

const OSIchar* const 
s_stringsForStatSources[ ScreenStats::NUM_STAT_SOURCE ] =
{
	"Player Stats",		// STAT_SOURCE_PLAYERS,
	"Top 10 Players",	// STAT_SOURCE_TOP_PLAYERS,
	"Clan Members",		// STAT_SOURCE_CLAN,
	"Top 10 Clans",		// STAT_SOURCE_TOP_CLANS,
	"Friends",			// STAT_SOURCE_FRIENDS,
};


//-------------------------------------------------------------------------------------------------
OnlineUI::Stats::Stats() : 
    _stateMachine(IDLE)
//-------------------------------------------------------------------------------------------------
{
	_stateMachine.setStateUpdate( IDLE, &OnlineUI::Stats::_updateIdle );

	_stateMachine.setStateUpdate( POPULATE_SCREEN_PLAYERS, &OnlineUI::Stats::_updatePopulateScreenPlayers );
	_stateMachine.setStateUpdate( POPULATE_SCREEN_CLANS, &OnlineUI::Stats::_updatePopulateScreenClans );

	_stateMachine.setStateEnter( LADDER_STATS, &OnlineUI::Stats::_enterLadderStats );
	_stateMachine.setStateUpdate( LADDER_STATS, &OnlineUI::Stats::_updateLadderStats );

	_stateMachine.setStateEnter( CLAN_LADDER_STATS, &OnlineUI::Stats::_enterClanLadderStats );
	_stateMachine.setStateUpdate( CLAN_LADDER_STATS, &OnlineUI::Stats::_updateClanLadderStats );

	_stateMachine.setStateEnter( FRIEND_LIST, &OnlineUI::Stats::_enterFriendList );
	_stateMachine.setStateUpdate( FRIEND_LIST,&OnlineUI::Stats:: _updateFriendList );
	_stateMachine.setStateEnter( FRIEND_STATS, &OnlineUI::Stats::_enterFriendStats );
	_stateMachine.setStateUpdate( FRIEND_STATS, &OnlineUI::Stats::_updateFriendStats );

	_stateMachine.setStateEnter( CLAN_MEMBER_PLAYERCLAN, &OnlineUI::Stats::_enterClanMemberPlayerClan );
	_stateMachine.setStateUpdate( CLAN_MEMBER_PLAYERCLAN, &OnlineUI::Stats::_updateClanMemberPlayerClan );
	_stateMachine.setStateEnter( CLAN_MEMBER_LIST, &OnlineUI::Stats::_enterClanMemberList );
	_stateMachine.setStateUpdate( CLAN_MEMBER_LIST, &OnlineUI::Stats::_updateClanMemberList );
	_stateMachine.setStateEnter( CLAN_MEMBER_STATS, &OnlineUI::Stats::_enterClanMemberStats );
	_stateMachine.setStateUpdate( CLAN_MEMBER_STATS, &OnlineUI::Stats::_updateClanMemberStats );
}

//-------------------------------------------------------------------------------------------------
//! \brief Called before use case is started.  Show the first screen for the use case.
//! \return true if succeeded, false otherwise
//-------------------------------------------------------------------------------------------------
/*virtual*/ bool OnlineUI::Stats::_init( void )
//-------------------------------------------------------------------------------------------------
{
	g_FrontEnd.GotoScreen("SCREEN_ID_STATS_5_80");
	_stateMachine.setCurrentState( this, IDLE );

    // Set current player name on stats screen
    ScreenStats *pScreen = UseCase::GetActiveScreen<ScreenStats>();
	if ( !pScreen ) return false;

    pScreen->SetUserName(OSI::getSingletonPtr()->getActiveAccount()->player.name);
	_lastStatSource = ScreenStats::NUM_STAT_SOURCE;
	_currentStatID = USER_RATING;
	_pivotPlayerID = OSI::getSingletonPtr()->getActivePlayer()->id;

	return true;
}

//-------------------------------------------------------------------------------------------------
//! \brief Called after use case is ended.  Clean up the screens and other objects.
//! \return true if succeeded, false otherwise
//-------------------------------------------------------------------------------------------------
/*virtual*/ bool OnlineUI::Stats::_deinit( void )
//-------------------------------------------------------------------------------------------------
{
    d_messageOwner.UnregisterAll();

	g_FrontEnd.RemoveAllPopups();
	if( UseCase::IsScreenActive< ScreenStats >() )
		g_FrontEnd.EndScreen( 0 );
	g_FrontEnd.PopScreenHistory( "SCREEN_ID_STATS_5_80" );
    return true;
}

//-------------------------------------------------------------------------------------------------
bool OnlineUI::Stats::_updateIdle()
//-------------------------------------------------------------------------------------------------
{
	bool			scrollLeftOrRight	= false;
    ScreenStats*	pScreen				= UseCase::GetActiveScreen<ScreenStats>();
	if ( !pScreen ) return true ;

	switch( pScreen->GetScreenStatus() )
	{
	case ScreenStats::NEXT_STAT:
		scrollLeftOrRight = true;
		_nextStat();
		break;

	case ScreenStats::PREV_STAT:
		scrollLeftOrRight = true;
		_prevStat();
		break;

	case ScreenStats::CHANGED_FILTER:
		_currentStatID = USER_RATING;
		break;

	case ScreenStats::PAGE_PLAYER_LIST_DOWN:
	{
		OSIPlayerStatsList& statsList = OSI::getSingletonPtr()->getStatsPlayerList();
		if( statsList.count() == 0 ) break;
		statsList.end();
		_pivotPlayerID = statsList.getCurrent()->playerID;
		if( ScreenStats::STAT_SOURCE_PLAYERS == _lastStatSource )
		{
			_stateMachine.setCurrentState( this, LADDER_STATS );
		}
		else if( ScreenStats::STAT_SOURCE_FRIENDS == _lastStatSource )
		{
			_stateMachine.setCurrentState( this, FRIEND_STATS );
		}
		return true;
		}

	case ScreenStats::PAGE_PLAYER_LIST_UP:
	{
		OSIPlayerStatsList& statsList = OSI::getSingletonPtr()->getStatsPlayerList();
		if( statsList.count() == 0 ) break;
		statsList.begin();
		_pivotPlayerID = statsList.getCurrent()->playerID;
		if( ScreenStats::STAT_SOURCE_PLAYERS == _lastStatSource )
		{
			_stateMachine.setCurrentState( this, LADDER_STATS );
		}
		else if( ScreenStats::STAT_SOURCE_FRIENDS == _lastStatSource )
		{
			_stateMachine.setCurrentState( this, FRIEND_STATS );
		}
		return true;
	}

	case ScreenStats::BACK:
        stop();
		break;
	};


	// populate the screen if necessary
	if( pScreen->IsWaitingForRows() )
	{
		_setStatTitle();
		// if the source hasn't changed, don't start any OSI requests
		//const bool pressedPlayerAfterScrollingUpOrDown = (_lastStatSource == ScreenStats::STAT_SOURCE_PLAYERS && !scrollLeftOrRight);
		if( _lastStatSource == pScreen->GetStatSource() ) //&& !pressedPlayerAfterScrollingUpOrDown )
		{
			if( _lastStatSource == ScreenStats::STAT_SOURCE_TOP_CLANS )
			{
				_stateMachine.setCurrentState( this, POPULATE_SCREEN_CLANS );
			}
			else
			{
				_stateMachine.setCurrentState( this, POPULATE_SCREEN_PLAYERS );
			}
		}
		else
		{
			// go to whichever state will retrieve the data from the current source
			_lastStatSource = pScreen->GetStatSource();

			if( ScreenStats::STAT_SOURCE_FRIENDS == _lastStatSource )
			{
				_pivotPlayerID.setInvalid();
			}
			else if( _lastStatSource == ScreenStats::STAT_SOURCE_PLAYERS )
			{
				_pivotPlayerID = OSI::getSingletonPtr()->getActivePlayer()->id;
			}
			_stateMachine.setCurrentState( this, s_stateToRunForSource[ _lastStatSource ] );
		}
	}

    return true;
}

//-------------------------------------------------------------------------------------------------
void OnlineUI::Stats::_setStatTitle()
//-------------------------------------------------------------------------------------------------
{
	ScreenStats* pScreen = UseCase::GetActiveScreen<ScreenStats>();
	if ( !pScreen ) return ;

	if( _lastStatSource != pScreen->GetStatSource() )
	{
		if( pScreen->GetStatSource() == ScreenStats::STAT_SOURCE_TOP_CLANS )
		{
			_currentStatID = CLAN_RATING;
		}
		else
		{
			_currentStatID = USER_RATING;
		}
	}
	
	// show the correct stat name on the screen
	pScreen->SetStatTitle( s_statNames[ _getDisplayedStatID() ] );
	pScreen->SetCurrentStatName( s_stringsForStatSources[ pScreen->GetStatSource() ] );
}

//-------------------------------------------------------------------------------------------------
void OnlineUI::Stats::OnButtonCancel()
//-------------------------------------------------------------------------------------------------
{
	stop();
}

//-------------------------------------------------------------------------------------------------
void OnlineUI::Stats::_enterLadderStats()
//-------------------------------------------------------------------------------------------------
{
	OSI* const osi = OSI::getSingletonPtr();
    ScreenStats* screen = UseCase::GetActiveScreen<ScreenStats>();
	if ( !screen) return ;
	
	bool requestOK;
	if( ScreenStats::STAT_SOURCE_TOP_PLAYERS != screen->GetStatSource() )
	{
		// get ladder around player
		requestOK = osi->requestStatsPlayerLadder( _pivotPlayerID, screen->GetStatTableDisplayRows() );
	}
	else
	{
		// get top N players
		requestOK = osi->requestStatsTopPlayers( screen->GetStatTableDisplayRows() );
	}

	WaitForOSIRequest::getSingletonPtr()->start( requestOK, "Downloading Stats", "Please wait..." );
}

//-------------------------------------------------------------------------------------------------
bool OnlineUI::Stats::_updateLadderStats()
//-------------------------------------------------------------------------------------------------
{
	OSI* const osi = OSI::getSingletonPtr();
    ScreenStats* screen = UseCase::GetActiveScreen<ScreenStats>();
	if ( !screen ) return true ;

	OSIPlayerStatsList& playerStats = osi->getStatsPlayerList();
	if ( ( STATUS_FAILED == osi->getLastRequestStatus() ) 
		|| playerStats.count() == 0 )
	{
		DisplayDialog::getSingletonPtr()->start( "No Stats", "There are no stats available for that request." );
		screen->FinishAddingRows();
		_stateMachine.setCurrentState( this, IDLE );
	}
	else
	{
		_stateMachine.setCurrentState( this, POPULATE_SCREEN_PLAYERS );
	}

	return true;
}

//-------------------------------------------------------------------------------------------------
void OnlineUI::Stats::_enterClanLadderStats()
//-------------------------------------------------------------------------------------------------
{
	OSI* const osi = OSI::getSingletonPtr();
    ScreenStats* screen = UseCase::GetActiveScreen<ScreenStats>();
	if ( !screen ) return ;

	const bool requestOK = osi->requestStatsTopClans(screen->GetStatTableDisplayRows());
	WaitForOSIRequest::getSingletonPtr()->start( requestOK, "Downloading Stats", "Please wait..." );
}

//-------------------------------------------------------------------------------------------------
bool OnlineUI::Stats::_updateClanLadderStats()
//-------------------------------------------------------------------------------------------------
{
	OSI* const osi = OSI::getSingletonPtr();
    ScreenStats* screen = UseCase::GetActiveScreen<ScreenStats>();
	if ( !screen ) return true ;

	if( ( STATUS_FAILED == osi->getLastRequestStatus() ) 
		|| osi->getStatsClanList().count() == 0 )
	{
		DisplayDialog::getSingletonPtr()->start( "Stats", "There are no clans to display." );
		screen->FinishAddingRows();
		_stateMachine.setCurrentState( this, IDLE );
	}
	else
	{
		_stateMachine.setCurrentState( this, POPULATE_SCREEN_CLANS );
	}

	return true;
}

//-------------------------------------------------------------------------------------------------
void OnlineUI::Stats::_enterFriendList()
//-------------------------------------------------------------------------------------------------
{
	OSI* const osi = OSI::getSingletonPtr();
    ScreenStats* screen = UseCase::GetActiveScreen<ScreenStats>();
	if ( !screen ) return ;
	
	const bool requestOK = osi->requestFriendList();
	WaitForOSIRequest::getSingletonPtr()->start( requestOK, "Downloading Stats", "Please wait..." );
}

//-------------------------------------------------------------------------------------------------
bool OnlineUI::Stats::_updateFriendList()
//-------------------------------------------------------------------------------------------------
{
	OSI* const osi = OSI::getSingletonPtr();
    ScreenStats* screen = UseCase::GetActiveScreen<ScreenStats>();
	if ( !screen ) return true ;

	if( STATUS_FAILED == osi->getLastRequestStatus() )
	{
		stop();
	}
	else
	{
		_stateMachine.setCurrentState( this, FRIEND_STATS );
	}
	return true;
}

//-------------------------------------------------------------------------------------------------
void OnlineUI::Stats::_enterFriendStats()
//-------------------------------------------------------------------------------------------------
{
	OSI* const osi = OSI::getSingletonPtr();
    ScreenStats* screen = UseCase::GetActiveScreen<ScreenStats>();
	if ( !screen ) return ;
	
	OSIPlayerList& friendsList = osi->getFriendList();
	OSIPlayer* player;
	OSIPlayerStatsList realFriendsList;
	friendsList.begin();
	
	// Since the friend list doesn't pivot like the leaderboard, it has to be manually searched
	// for the pivot ID.
	while( _pivotPlayerID.isValid() && (friendsList.getCurrent()->id != _pivotPlayerID) )
	{
		friendsList.next();
	}

	// Since the friend list doesn't pivot like the leaderboard, it has to be manually offset
	// to only page down half a page at a time.
	for( int i = 0; i < 5; i++ ) { friendsList.previous(); }
	if( !friendsList.getCurrent() ) { friendsList.begin(); }

	while ( (player = friendsList.getCurrent()) != NULL )
	{
		if ( player->isAFriend )
		{
			OSIPlayerStats playerStats;
			playerStats.playerID = player->id;
			OSIstrncpy(playerStats.name, player->name, OSI_PLAYER_NAME_LENGTH);
			OSIstrncpy(playerStats.clanAbbr, player->clanInfo.tag, OSI_CLAN_TAG_LENGTH);			
			realFriendsList.push_back(playerStats);

			// Only retrieve stats for a page worth of friends at a time instead of 
			// potentially all 100 at once.
			if( realFriendsList.count() >= 10 ) { break; }
		}
		friendsList.next();
	}

	_realFriendsCount = realFriendsList.count();

	const bool requestOK = osi->requestStatsPlayerList( realFriendsList );
	WaitForOSIRequest::getSingletonPtr()->start( requestOK, "Downloading Stats", "Please wait..." );
}

//-------------------------------------------------------------------------------------------------
bool OnlineUI::Stats::_updateFriendStats()
//-------------------------------------------------------------------------------------------------
{
	OSI* const osi = OSI::getSingletonPtr();
    ScreenStats* screen = UseCase::GetActiveScreen<ScreenStats>();
	if ( !screen ) return true ;

	OSIPlayerList& friendsList = osi->getFriendList();
	if ( ( STATUS_FAILED == osi->getLastRequestStatus() ) 
		|| _realFriendsCount == 0 )
	{
		DisplayDialog::getSingletonPtr()->start( "Stats", "You do not have any friends." );
		screen->FinishAddingRows();
		// Need to clear out the stats list so we don't get old data
		OSI::getSingletonPtr()->getStatsPlayerList().clear();
		_stateMachine.setCurrentState( this, IDLE );
	}
	else
	{
		_stateMachine.setCurrentState( this, POPULATE_SCREEN_PLAYERS );
	}
	return true;
}

//-------------------------------------------------------------------------------------------------
void OnlineUI::Stats::_enterClanMemberPlayerClan()
//-------------------------------------------------------------------------------------------------
{
	OSI* const osi = OSI::getSingletonPtr();
    ScreenStats* screen = UseCase::GetActiveScreen<ScreenStats>();
	if ( !screen ) return ;

	const bool requestOK = osi->requestClanUpdateMyClanInformation();
	WaitForOSIRequest::getSingletonPtr()->start( requestOK, "Downloading Stats", "Please wait..." );
}

//-------------------------------------------------------------------------------------------------
bool OnlineUI::Stats::_updateClanMemberPlayerClan()
//-------------------------------------------------------------------------------------------------
{
	OSI* const osi = OSI::getSingletonPtr();
    ScreenStats* screen = UseCase::GetActiveScreen<ScreenStats>();
	if ( !screen ) return true ;

	if ( ( STATUS_FAILED == osi->getLastRequestStatus() ) 
		|| osi->getActivePlayer()->clanRole == OSI_CLAN_ROLE_UNKNOWN )
	{
		DisplayDialog::getSingletonPtr()->start( "Stats", "You are not in a clan." );
		screen->FinishAddingRows();
		// Need to clear out the stats list so we don't get old data
		OSI::getSingletonPtr()->getStatsPlayerList().clear();
		_stateMachine.setCurrentState( this, IDLE );
	}
	else
	{
		_stateMachine.setCurrentState( this, CLAN_MEMBER_LIST );
	}
	return true;
}

//-------------------------------------------------------------------------------------------------
void OnlineUI::Stats::_enterClanMemberList()
//-------------------------------------------------------------------------------------------------
{
	OSI* const osi = OSI::getSingletonPtr();
    ScreenStats* screen = UseCase::GetActiveScreen<ScreenStats>();
	if ( !screen ) return ;

	OSIClanID& clanID = osi->getActivePlayer()->clanInfo.clanID;
	const bool requestOK = osi->requestClanMembershipList( clanID );
	WaitForOSIRequest::getSingletonPtr()->start( requestOK, "Downloading Stats", "Please wait..." );
}

//-------------------------------------------------------------------------------------------------
bool OnlineUI::Stats::_updateClanMemberList()
//-------------------------------------------------------------------------------------------------
{
	OSI* const osi = OSI::getSingletonPtr();
    ScreenStats* screen = UseCase::GetActiveScreen<ScreenStats>();
	if ( !screen ) return true ;

	if( STATUS_FAILED == osi->getLastRequestStatus() )
	{
		stop();
	}
	else
	{
		_stateMachine.setCurrentState( this, CLAN_MEMBER_STATS );
	}
	return true;
}

//-------------------------------------------------------------------------------------------------
void OnlineUI::Stats::_enterClanMemberStats()
//-------------------------------------------------------------------------------------------------
{
	OSI* const osi = OSI::getSingletonPtr();
    ScreenStats* screen = UseCase::GetActiveScreen<ScreenStats>();
	if ( !screen ) return ;

	OSIClanMembershipList& membershipList = osi->getClanMembershipList();
	OSIPlayerStatsList statsList;
	for( membershipList.begin(); membershipList.getCurrent(); membershipList.next() )
	{
		OSIClanMembership* membership = membershipList.getCurrent();

		OSIPlayerStats playerStats;
		playerStats.playerID = membership->playerID;
		OSIstrncpy(playerStats.name, membership->memberName, OSI_PLAYER_NAME_LENGTH);
		OSIstrncpy(playerStats.clanAbbr, "", OSI_CLAN_TAG_LENGTH); // Need this?
		statsList.push_back(playerStats);
	}
	
	const bool requestOK = osi->requestStatsPlayerList( statsList );
	WaitForOSIRequest::getSingletonPtr()->start( requestOK, "Downloading Stats", "Please wait..." );
}

//-------------------------------------------------------------------------------------------------
bool OnlineUI::Stats::_updateClanMemberStats()
//-------------------------------------------------------------------------------------------------
{
	OSI* const osi = OSI::getSingletonPtr();
    ScreenStats* screen = UseCase::GetActiveScreen<ScreenStats>();
	if ( !screen ) return true ;

	if( STATUS_FAILED == osi->getLastRequestStatus() )
	{
		stop();
	}
	else
	{
		_stateMachine.setCurrentState( this, POPULATE_SCREEN_PLAYERS );
	}
	return true;
}


//-------------------------------------------------------------------------------------------------
bool OnlineUI::Stats::_updatePopulateScreenPlayers()
//-------------------------------------------------------------------------------------------------
{
	OSI* const osi = OSI::getSingletonPtr();
    ScreenStats* screen = UseCase::GetActiveScreen<ScreenStats>();
	if ( !screen ) return true ;
	
	OSIPlayerStatsList& statsList = osi->getStatsPlayerList();
	for( statsList.begin(); statsList.getCurrent(); statsList.next() )
	{
		OSIPlayerStats* playerStats = statsList.getCurrent();

		OSIchar fullName[ OSIClanInfo::MAX_FULL_NAME_LENGTH ];
		GetFullPlayerName(fullName, playerStats->clanAbbr, playerStats->name);

		_populateDisplayedStats( *playerStats, screen->GetStatFilter() );

		// if player has no sessions then they have no stats
		if ( _statValues[ SESSIONS ] == 0.0f )
		{
			screen->AddRow( fullName, "N/A" );
			continue;
		}


		// handle special cases for stat display
		OSIchar buff[32];
		unsigned int displayedStatID = _getDisplayedStatID();
		switch( displayedStatID )
		{
		case USER_RATING:
			sprintf( buff, "%d (%d)", 
				(int)_statValues[ displayedStatID ], 
				(int)playerStats->getStat(OSIPlayerStats::PLAYERSTAT_NA_RANK) );
			screen->AddRow( fullName, buff );
			break;

		case MOST_PLAYED_MODE:
			screen->AddRow( fullName, s_gameModeNames[ (int) _statValues[ displayedStatID ] ] );
			break;

		default:
			screen->AddRow( fullName, _statValues[ displayedStatID ] );
			break;
		};
	}

	screen->FinishAddingRows();
	_stateMachine.setCurrentState( this, IDLE );
	return true;
}

//-------------------------------------------------------------------------------------------------
bool OnlineUI::Stats::_updatePopulateScreenClans()
//-------------------------------------------------------------------------------------------------
{
	OSI* const osi = OSI::getSingletonPtr();
    ScreenStats* screen = UseCase::GetActiveScreen<ScreenStats>();
	if ( !screen ) return true ;
	
	OSIClanInfoList& clans = osi->getStatsClanList();
	for( clans.begin(); clans.getCurrent(); clans.next() )
	{
		OSIClanInfo* clan = clans.getCurrent();
		OSIchar clanNameAndTag[ OSI_CLAN_NAME_LENGTH + OSI_CLAN_TAG_LENGTH ];
		if( clan->tag[0] && clan->name[0] )
			{ OSIsnprintf( clanNameAndTag, NUM_ARRAY_ELEMENTS( clanNameAndTag ), "<%s> %s", clan->tag, clan->name ); }
		else
			{ OSIstrcpy( clanNameAndTag, "N/A" ); }
		clanNameAndTag[ OSI_CLAN_NAME_LENGTH + OSI_CLAN_TAG_LENGTH - 1 ] = 0;
		
		if( CLAN_RATING == _getDisplayedStatID() )
		{
			OSIchar buff[ 32 ];
			OSIsnprintf( buff, 32, "%i (%i)", 
				(int)clan->stats.getStat(OSIClanStats::CLANSTAT_RATING),
				(int)clan->stats.getStat(OSIClanStats::CLANSTAT_RANK) );
			buff[31] = 0;
			screen->AddRow( clanNameAndTag, buff );
		}
		else
		{
			screen->AddRow( clanNameAndTag, (int)clan->stats.getStat(OSIClanStats::CLANSTAT_KILLS) );
		}
	}
	screen->FinishAddingRows();
	_stateMachine.setCurrentState( this, IDLE );
	return true;
}

//-------------------------------------------------------------------------------------------------
void OnlineUI::Stats::_nextStat()
//-------------------------------------------------------------------------------------------------
{
	ScreenStats* screen = UseCase::GetActiveScreen<ScreenStats>();
	if ( !screen ) return ;

	if( screen->GetStatSource() == ScreenStats::STAT_SOURCE_TOP_CLANS )
	{
		// We only have two stat's for clans, CLAN_RATING and KILLS
		if( _currentStatID == CLAN_RATING )
			_currentStatID = CLAN_KILLS;
	}
	else if ( screen->GetStatFilter() == ScreenStats::STAT_FILTER_SHOW_COP_ONLY )
	{
		if( _currentStatID < NUM_COP_STATS-1 )
			_currentStatID = (DisplayStat)((int)_currentStatID + 1);
	}
	else if ( screen->GetStatFilter() == ScreenStats::STAT_FILTER_SHOW_GANGSTA_ONLY )
	{
		if( _currentStatID < NUM_GANG_STATS-1 )
			_currentStatID = (DisplayStat)((int)_currentStatID + 1);
	}
	else
	{
		if( _currentStatID < LAST_DISPLAYED_STAT-1 )
			_currentStatID = (DisplayStat)((int)_currentStatID + 1);
	}

#ifdef _XBOX 
	if( screen->GetStatSource() != ScreenStats::STAT_SOURCE_TOP_CLANS )
	{
		// skip the clan rating on Xbox because it's not correct and making it correct
		// would result in a TCR violation because of downloading too many stats at once.
		if( CLAN_RATING == _getDisplayedStatID() ) { _currentStatID = _currentStatID = (DisplayStat)((int)_currentStatID + 1); }
		else if( CLAN_KILLS == _getDisplayedStatID() ) { _currentStatID = _currentStatID = (DisplayStat)((int)_currentStatID + 1); }
	}
#endif

}

//-------------------------------------------------------------------------------------------------
void OnlineUI::Stats::_prevStat()
//-------------------------------------------------------------------------------------------------
{
	ScreenStats* screen = UseCase::GetActiveScreen<ScreenStats>();
	if ( !screen ) return ;

	if( screen->GetStatSource() == ScreenStats::STAT_SOURCE_TOP_CLANS )
	{
		// We only have two stat's for clans, CLAN_RATING and KILLS
		if( _currentStatID == CLAN_KILLS )
			_currentStatID = CLAN_RATING;
	}
	else if( _currentStatID > 0 )
	{
		_currentStatID = (DisplayStat)((int)_currentStatID - 1);
	}

#ifdef _XBOX 
	if( screen->GetStatSource() != ScreenStats::STAT_SOURCE_TOP_CLANS )
	{
		// skip the clan rating on Xbox because it's not correct and making it correct
		// would result in a TCR violation because of downloading too many stats at once.
		if( CLAN_RATING == _getDisplayedStatID() ) { _currentStatID = (DisplayStat)((int)_currentStatID - 1); }
		else if( CLAN_KILLS == _getDisplayedStatID() ) { _currentStatID = (DisplayStat)((int)_currentStatID - 1); }
	}
#endif
}


//-------------------------------------------------------------------------------------------------
void OnlineUI::Stats::_populateDisplayedStats(OSIPlayerStats& playerStats, ScreenStats::StatFilter statFilter)
//-------------------------------------------------------------------------------------------------
{
	for (int i=0; i<LAST_DISPLAYED_STAT; i++ )
		_statValues[i] = 0;

	OSI* const osi = OSI::getSingletonPtr();
	OSIPlayerStats::StatValueType *s = playerStats.statsArray; // Shortcut! 

	// Stats in Common between all view (total, cop, gangsta)
	_statValues[RAID_CAPTURES]		= s[OSIPlayerStats::PLAYERSTAT_NA_RAID_SCORES];
	_statValues[ROBBERY_ESCAPES]	= s[OSIPlayerStats::PLAYERSTAT_NA_LOOT_SCORES];
	_statValues[TAGS]				= s[OSIPlayerStats::PLAYERSTAT_NA_TAG_SCORES];
	_statValues[APPREHENSIONS]		= s[OSIPlayerStats::PLAYERSTAT_NA_APPREHENSIONS];
	_statValues[APPREHENDED]		= s[OSIPlayerStats::PLAYERSTAT_NA_APPREHENDED];
	_statValues[CLAN_RATING]		= playerStats.clanRating;
	_statValues[CLAN_KILLS]			= playerStats.clanKills;

	float mpmScore = 0, mpm = 0;
	if ( s[OSIPlayerStats::PLAYERSTAT_NA_TAG_GAMES_PLAYED] > mpmScore )
	{
		mpmScore = s[OSIPlayerStats::PLAYERSTAT_NA_TAG_GAMES_PLAYED];
		mpm = 0;
	}
	if ( s[OSIPlayerStats::PLAYERSTAT_NA_WAR_GAMES_PLAYED] > mpmScore )
	{
		mpmScore = s[OSIPlayerStats::PLAYERSTAT_NA_WAR_GAMES_PLAYED];
		mpm = 1;
	}
	if ( s[OSIPlayerStats::PLAYERSTAT_NA_ROBBERY_GAMES_PLAYED] > mpmScore )
	{
		mpmScore = s[OSIPlayerStats::PLAYERSTAT_NA_ROBBERY_GAMES_PLAYED];
		mpm = 2;
	}
	if ( s[OSIPlayerStats::PLAYERSTAT_NA_RAID_GAMES_PLAYED] > mpmScore )
	{
		mpmScore = s[OSIPlayerStats::PLAYERSTAT_NA_RAID_GAMES_PLAYED];
		mpm = 3;
	}
	if ( s[OSIPlayerStats::PLAYERSTAT_NA_MODE5_GAMES_PLAYED] > mpmScore )
	{
		mpmScore = s[OSIPlayerStats::PLAYERSTAT_NA_MODE5_GAMES_PLAYED];
		mpm = 4;
	}
	if ( s[OSIPlayerStats::PLAYERSTAT_NA_MODE6_GAMES_PLAYED] > mpmScore )
	{
		mpmScore = s[OSIPlayerStats::PLAYERSTAT_NA_MODE6_GAMES_PLAYED];
		mpm = 5;
	}
	_statValues[MOST_PLAYED_MODE] = mpm;


	// Cop Specific Stats
	if ( statFilter == ScreenStats::STAT_FILTER_SHOW_COP_ONLY )
	{
		_statValues[USER_RATING]		= s[OSIPlayerStats::PLAYERSTAT_COP_RATING];
		
		_statValues[SESSIONS]			= s[OSIPlayerStats::PLAYERSTAT_COP_GAMES_PLAYED];
		_statValues[MVP_AWARDS]			= s[OSIPlayerStats::PLAYERSTAT_COP_MVPS];
		_statValues[KILLS]				= s[OSIPlayerStats::PLAYERSTAT_KILLS_AS_COP];
		_statValues[HEADSHOTS]			= s[OSIPlayerStats::PLAYERSTAT_NA_COP_HEADSHOTS];
		_statValues[DEATHS]				= s[OSIPlayerStats::PLAYERSTAT_DEATHS_AS_COP];
		_statValues[SUICIDES]			= s[OSIPlayerStats::PLAYERSTAT_NA_COP_SUICIDES];
		if( s[OSIPlayerStats::PLAYERSTAT_NA_COP_DEATHS] == 0 )
			_statValues[KILL_DEATH_RATIO] = s[OSIPlayerStats::PLAYERSTAT_KILLS_AS_COP];
		else
			_statValues[KILL_DEATH_RATIO] = (s[OSIPlayerStats::PLAYERSTAT_KILLS_AS_COP]) / (s[OSIPlayerStats::PLAYERSTAT_NA_COP_DEATHS]);
		_statValues[TEAM_KILLS]			= s[OSIPlayerStats::PLAYERSTAT_NA_COP_TEAM_KILLS];
		_statValues[NPC_KILLS]			= s[OSIPlayerStats::PLAYERSTAT_NA_COP_NPC_KILLS];
		_statValues[ROUNDS_WON]			= s[OSIPlayerStats::PLAYERSTAT_COP_WINS];
		_statValues[ROUNDS_LOST]		= s[OSIPlayerStats::PLAYERSTAT_NA_COP_LOSSES];
		_statValues[SHOTS_FIRED]		= s[OSIPlayerStats::PLAYERSTAT_NA_COP_SHOTS_FIRED];
		_statValues[SHOTS_HIT]			= s[OSIPlayerStats::PLAYERSTAT_NA_COP_SHOTS_HIT];
		if( s[OSIPlayerStats::PLAYERSTAT_NA_COP_SHOTS_FIRED] == 0 )
			_statValues[ACCURACY]		= s[OSIPlayerStats::PLAYERSTAT_NA_COP_SHOTS_HIT];
		else
			_statValues[ACCURACY]		= (s[OSIPlayerStats::PLAYERSTAT_NA_COP_SHOTS_HIT]) / (s[OSIPlayerStats::PLAYERSTAT_NA_COP_SHOTS_FIRED]);
	}
	else if ( statFilter == ScreenStats::STAT_FILTER_SHOW_GANGSTA_ONLY ) // Gang Specific Stats
	{
		_statValues[USER_RATING]		= s[OSIPlayerStats::PLAYERSTAT_GANG_RATING];
		_statValues[SESSIONS]			= s[OSIPlayerStats::PLAYERSTAT_GANG_GAMES_PLAYED];
		_statValues[MVP_AWARDS]			= s[OSIPlayerStats::PLAYERSTAT_GANG_MVPS];
		_statValues[KILLS]				= s[OSIPlayerStats::PLAYERSTAT_KILLS_AS_GANGSTA];
		_statValues[HEADSHOTS]			= s[OSIPlayerStats::PLAYERSTAT_NA_GANG_HEADSHOTS];
		_statValues[DEATHS]				= s[OSIPlayerStats::PLAYERSTAT_DEATHS_AS_GANGSTA];
		_statValues[SUICIDES]			= s[OSIPlayerStats::PLAYERSTAT_NA_GANG_SUICIDES];
		if( s[OSIPlayerStats::PLAYERSTAT_NA_GANG_DEATHS] == 0 )
			_statValues[KILL_DEATH_RATIO] = s[OSIPlayerStats::PLAYERSTAT_KILLS_AS_GANGSTA];
		else
			_statValues[KILL_DEATH_RATIO] = (s[OSIPlayerStats::PLAYERSTAT_KILLS_AS_GANGSTA]) / (s[OSIPlayerStats::PLAYERSTAT_NA_GANG_DEATHS]);
		_statValues[TEAM_KILLS]			= s[OSIPlayerStats::PLAYERSTAT_NA_GANG_TEAM_KILLS];
		_statValues[NPC_KILLS]			= s[OSIPlayerStats::PLAYERSTAT_NA_GANG_NPC_KILLS];
		_statValues[ROUNDS_WON]			= s[OSIPlayerStats::PLAYERSTAT_GANG_WINS];
		_statValues[ROUNDS_LOST]		= s[OSIPlayerStats::PLAYERSTAT_NA_GANG_LOSSES];
		_statValues[SHOTS_FIRED]		= s[OSIPlayerStats::PLAYERSTAT_NA_GANG_SHOTS_FIRED];
		_statValues[SHOTS_HIT]			= s[OSIPlayerStats::PLAYERSTAT_NA_GANG_SHOTS_HIT];
		if( s[OSIPlayerStats::PLAYERSTAT_NA_GANG_SHOTS_FIRED] == 0 )
			_statValues[ACCURACY]		= s[OSIPlayerStats::PLAYERSTAT_NA_GANG_SHOTS_HIT];
		else
			_statValues[ACCURACY]		= (s[OSIPlayerStats::PLAYERSTAT_NA_GANG_SHOTS_HIT]) / (s[OSIPlayerStats::PLAYERSTAT_NA_GANG_SHOTS_FIRED]);
	}
	else // Stat Totals
	{
		_statValues[USER_RATING]		= s[OSIPlayerStats::PLAYERSTAT_USER_RATING];
		_statValues[SESSIONS]			= s[OSIPlayerStats::PLAYERSTAT_COP_GAMES_PLAYED] + s[OSIPlayerStats::PLAYERSTAT_GANG_GAMES_PLAYED];
		_statValues[MVP_AWARDS]			= s[OSIPlayerStats::PLAYERSTAT_COP_MVPS] + s[OSIPlayerStats::PLAYERSTAT_GANG_MVPS];
		_statValues[KILLS]				= s[OSIPlayerStats::PLAYERSTAT_KILLS_AS_COP] + s[OSIPlayerStats::PLAYERSTAT_KILLS_AS_GANGSTA];
		_statValues[HEADSHOTS]			= s[OSIPlayerStats::PLAYERSTAT_NA_COP_HEADSHOTS] + s[OSIPlayerStats::PLAYERSTAT_NA_GANG_HEADSHOTS];
		_statValues[DEATHS]				= s[OSIPlayerStats::PLAYERSTAT_DEATHS_AS_COP] + s[OSIPlayerStats::PLAYERSTAT_DEATHS_AS_GANGSTA];
		_statValues[SUICIDES]			= s[OSIPlayerStats::PLAYERSTAT_NA_COP_SUICIDES] + s[OSIPlayerStats::PLAYERSTAT_NA_GANG_SUICIDES];
		if( s[OSIPlayerStats::PLAYERSTAT_NA_COP_DEATHS] + s[OSIPlayerStats::PLAYERSTAT_NA_GANG_DEATHS] == 0 )
			_statValues[KILL_DEATH_RATIO] = s[OSIPlayerStats::PLAYERSTAT_KILLS_AS_COP] + s[OSIPlayerStats::PLAYERSTAT_KILLS_AS_GANGSTA];
		else
			_statValues[KILL_DEATH_RATIO] = (s[OSIPlayerStats::PLAYERSTAT_KILLS_AS_COP] + s[OSIPlayerStats::PLAYERSTAT_KILLS_AS_GANGSTA]) / (s[OSIPlayerStats::PLAYERSTAT_NA_COP_DEATHS] + s[OSIPlayerStats::PLAYERSTAT_NA_GANG_DEATHS]);
		_statValues[TEAM_KILLS]			= s[OSIPlayerStats::PLAYERSTAT_NA_COP_TEAM_KILLS] +  s[OSIPlayerStats::PLAYERSTAT_NA_GANG_TEAM_KILLS];
		_statValues[NPC_KILLS]			= s[OSIPlayerStats::PLAYERSTAT_NA_COP_NPC_KILLS] + s[OSIPlayerStats::PLAYERSTAT_NA_GANG_NPC_KILLS];
		_statValues[ROUNDS_WON]			= s[OSIPlayerStats::PLAYERSTAT_COP_WINS] + s[OSIPlayerStats::PLAYERSTAT_GANG_WINS];
		_statValues[ROUNDS_LOST]		= s[OSIPlayerStats::PLAYERSTAT_NA_COP_LOSSES] + s[OSIPlayerStats::PLAYERSTAT_NA_GANG_LOSSES];
		_statValues[SHOTS_FIRED]		= s[OSIPlayerStats::PLAYERSTAT_NA_COP_SHOTS_FIRED] + s[OSIPlayerStats::PLAYERSTAT_NA_GANG_SHOTS_FIRED];
		_statValues[SHOTS_HIT]			= s[OSIPlayerStats::PLAYERSTAT_NA_COP_SHOTS_HIT] + s[OSIPlayerStats::PLAYERSTAT_NA_GANG_SHOTS_HIT];
		if( s[OSIPlayerStats::PLAYERSTAT_NA_COP_SHOTS_FIRED] + s[OSIPlayerStats::PLAYERSTAT_NA_GANG_SHOTS_FIRED] == 0 )
			_statValues[ACCURACY]		= s[OSIPlayerStats::PLAYERSTAT_NA_COP_SHOTS_HIT] + s[OSIPlayerStats::PLAYERSTAT_NA_GANG_SHOTS_HIT];
		else
			_statValues[ACCURACY]		= (s[OSIPlayerStats::PLAYERSTAT_NA_COP_SHOTS_HIT] + s[OSIPlayerStats::PLAYERSTAT_NA_GANG_SHOTS_HIT]) / (s[OSIPlayerStats::PLAYERSTAT_NA_COP_SHOTS_FIRED] + s[OSIPlayerStats::PLAYERSTAT_NA_GANG_SHOTS_FIRED]);
	}
}


OnlineUI::Stats::DisplayStat OnlineUI::Stats::_getDisplayedStatID() const
{
    ScreenStats *pScreen = UseCase::GetActiveScreen<ScreenStats>();
	if ( !pScreen ) return USER_RATING;

	switch( pScreen->GetStatFilter() )
	{ 
	case ScreenStats::STAT_FILTER_SHOW_COP_ONLY:		return copStats[ _currentStatID ];
	case ScreenStats::STAT_FILTER_SHOW_GANGSTA_ONLY:	return gangStats[ _currentStatID ];
	default:											return _currentStatID;
	};
}
