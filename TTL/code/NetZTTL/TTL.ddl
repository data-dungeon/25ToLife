//
//
//  File:   TTL.ddl
//  Date:   October 1, 2003
//  By:     Bryant Collard
//  Desc:   TTL DDL definitions.
//
//  Copyright (c) 2003, Avalanche Software Inc.
//  All Rights Reserved.
//
//

// Use the general DDL definitions.
#use <Network/NetZ/Avalanche.ddl>

// Get shared constants
#include <Main/TTLConstants.h>

// Participant

dataset DSParticipant
{
	byte d_id;
	byte d_config;
} upon_request_filter;

// Player Agent

dataset DSProfile
{
	byte d_accountId[ACCOUNT_ID_SIZE];
	string d_accountName;
	string d_displayName;
	byte d_tagParams[TAG_PARAMS_SIZE];
} constant;

dataset DSAppearance
{
	string d_character;
	string d_loadout;
} upon_request_filter;

dataset DSAgent
{
	byte d_mode;
	byte d_lobbyStatus;
} upon_request_filter;

dataset DSStats
{
	int32 d_rating;
	int32 d_rank;
} upon_request_filter;

dataset DSLevel
{
	uint32 d_crc;
	int32 d_distributedActorCount;
	float d_centerX;
	float d_centerY;
	float d_centerZ;
	float d_extentX;
	float d_extentY;
	float d_extentZ;
} upon_request_filter;

doclass DOPlayerAgent : DOTalker
{
	DSParticipant d_participant;
	DSProfile d_profile;
	DSAppearance d_appearance;
	DSAgent d_agent;
	DSStats d_stats;
	DSLevel d_level;
};

// Combatant

dataset DSCombatantMode
{
	byte d_flags;
	byte d_health;
	byte d_weapon;
	int32 d_painState;
} upon_request_filter;

dataset DSCombatantState
{
	float d_sightX;
	float d_sightY;
	float d_sightZ;
} unreliable, extrapolation_filter;

doclass DOCombatant : DOActor
{
	DSParticipant d_participant;
	DSCombatantMode d_mode;
	DSCombatantState d_state;
	DSGroundFollower d_coordinates;
	DSPathFollower d_path;
};

// Glass

dataset DSGlassState
{
	bool d_shattered;
} upon_request_filter;

doclass DOGlass : DOActor
{
	DSGlassState d_state;
};

// Lobbed

dataset DSLobbedProps
{
	int32		d_type;
} constant;

doclass DOLobbed : DOActor
{
	DSSixDOF d_state;
	DSVector d_acceleration;
	DSLobbedProps d_lobbedProps;
};

// Subsonic

dataset DSSubsonicProps
{
	int32		d_type;
} constant;

doclass DOSubsonic : DOActor
{
	DSSixDOF d_state;
	DSVector d_acceleration;
	DSSubsonicProps d_subsonicProps;
};

// Referee

dataset DSRefereePhase
{
	byte d_value;
	int32 d_changeTime;
	byte d_assignmentId;
} upon_request_filter;

dataset DSRefereeId
{
	dohandle d_handle[MAX_PARTICIPANTS];
} upon_request_filter;

dataset DSRefereeAssignment
{
	byte d_config[MAX_PARTICIPANTS];
	byte d_assignmentId;
} upon_request_filter;

dataset DSRefereePopulate
{
	uint32 d_crc;
	uint32 d_numPlayers;
	int32 d_start[MAX_PLAYERS];
	int32 d_end[MAX_PLAYERS];
} upon_request_filter;

dataset DSRefereeRound
{
	byte d_roundId[ROUND_ID_SIZE];
	byte d_status;
} upon_request_filter;

dataset DSRefereeRules
{
	string d_desc;
	byte d_teamRole[MAX_TEAMS];
} upon_request_filter;

dataset DSRefereeMatch
{
	byte d_directPhaseExit[NUM_REFEREE_PHASES];
	int32 d_phaseEndTime[NUM_REFEREE_PHASES];
	int32 d_phaseLifetime[NUM_REFEREE_PHASES];
	byte d_roundsInMatch;
	byte d_roundCount;
} upon_request_filter;

dataset DSScoringPlayerInfo
{
	string d_name[MAX_PLAYERS];
	byte d_team[MAX_PLAYERS];
	byte d_isAlphaBlue;
} upon_request_filter;

dataset DSSession
{
	int32 d_alphaWins;
	int32 d_betaWins;
} upon_request_filter;

dataset DSTeams
{
	uint16 d_spawnTickets[MAX_TEAMS];
} upon_request_filter;

doclass DOReferee : DOMessageOwner
{
	DSRefereePhase d_nextPhase;
	DSRefereeId d_id;
	DSRefereeAssignment d_assignment;
	DSRefereePopulate d_populate;
	DSRefereeRound d_round;
	DSRefereeRules d_rules;
	DSRefereeMatch d_match;
	DSBuffer d_score;
	DSScoringPlayerInfo d_scoringPlayerInfo;
	DSSession d_session;
	DSTeams d_teams;
};

wellknown DOReferee g_doReferee;
