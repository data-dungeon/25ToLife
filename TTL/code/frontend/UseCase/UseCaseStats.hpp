///////////////////////////////////////////////////////////////////////////////////////
///  \file UseCaseStats.hpp
///
///  \brief Defines the Stats class
///////////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDED_STATS_
#define _INCLUDED_STATS_

#include "UseCase.hpp"
#include "OSI/OSI.hpp"
#include "StateMachine.hpp"
#include "Singleton.hpp"
#include "../screens/screen_stats.h"


///////////////////////////////////////////////////////////////////////////////////////
/// Handle display of the stats UI.
///////////////////////////////////////////////////////////////////////////////////////
namespace OnlineUI
{
class Stats : public UseCase, public Singleton<Stats>
{
public:
	enum State
	{
		IDLE,

		LADDER_STATS,

		CLAN_LADDER_STATS,

		FRIEND_LIST,
		FRIEND_STATS,

		CLAN_MEMBER_PLAYERCLAN,
		CLAN_MEMBER_LIST,
		CLAN_MEMBER_STATS,

		POPULATE_SCREEN_PLAYERS,
		POPULATE_SCREEN_CLANS,

		NUM_STATES
	};


	enum DisplayStat
	{
		USER_RATING,
		CLAN_RATING,
		SESSIONS,
		MVP_AWARDS,
		KILLS,
		HEADSHOTS,
		RAID_CAPTURES,
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
		APPREHENSIONS,
		APPREHENDED,
		SHOTS_FIRED,
		SHOTS_HIT,
		ACCURACY,
		MOST_PLAYED_MODE,
		LAST_DISPLAYED_STAT
	};

	static const OSIchar* const	s_statNames[ LAST_DISPLAYED_STAT ];
	
	static const int NUM_GAME_MODES = 6;
	static const OSIchar* const s_gameModeNames[ NUM_GAME_MODES ];

	static const int NUM_COP_STATS = 20;
	static const DisplayStat copStats[ NUM_COP_STATS ];

	static const int NUM_GANG_STATS = 21;
	static const DisplayStat gangStats[  NUM_GANG_STATS ];

	virtual bool update() { return _stateMachine.update(this); }
	CMessageOwner d_messageOwner;

protected:
	friend class Singleton<Stats>;

	static const State			s_stateToRunForSource[ ScreenStats::NUM_STAT_SOURCE ];
	DisplayStat				_currentStatID;
	ScreenStats::StatSource _lastStatSource;

	Stats();
	virtual ~Stats() { }

	virtual bool _init();
	virtual bool _deinit();
	virtual void OnButtonCancel(); // { stop(); }

	void _nextStat();
	void _prevStat();

	bool _updateIdle();
	bool _updatePopulateScreenPlayers();
	bool _updatePopulateScreenClans();

	void _enterLadderStats();
	bool _updateLadderStats();

	void _enterClanLadderPlayerClan();
	bool _updateClanLadderPlayerClan();

	void _enterClanLadderRank();
	bool _updateClanLadderRank();

	void _enterClanLadderStats();
	bool _updateClanLadderStats();

	void _enterFriendList();
	bool _updateFriendList();

	void _enterFriendStats();
	bool _updateFriendStats();

	void _enterClanMemberPlayerClan();
	bool _updateClanMemberPlayerClan();

	void _enterClanMemberList();
	bool _updateClanMemberList();

	void _enterClanMemberStats();
	bool _updateClanMemberStats();

	void _setStatTitle();
	DisplayStat _getDisplayedStatID() const;
private:
	StateMachine< State, NUM_STATES, Stats > _stateMachine;

	float _statValues[LAST_DISPLAYED_STAT];
	void _populateDisplayedStats(OSIPlayerStats& playerStats, ScreenStats::StatFilter statFilter);

	OSIPlayerID _pivotPlayerID;
	int _realFriendsCount;
};

} // namespace OnlineUI

#endif //_INCLUDED_MULTIPLAYER_MENU_
