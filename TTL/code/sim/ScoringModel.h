/*
**
**  File:   ScoringModel.h
**  Date:   October 5, 2004
**  By:     Bryant Collard
**  Desc:   The base class of a module that installs in the referee and that
**          manages scoring.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/sim/ScoringModel.h $
**  $Revision: #31 $
**  $DateTime: 2005/08/18 13:23:21 $
**    $Author: Bryce $
**
*/

#ifndef SCORING_MODEL_H
#define SCORING_MODEL_H

#include "main/TTLConstants.h"
#include "GameHelper/Creator.h"
#include "container/Darray.h"
#include "netzttl/DSScoringPlayerInfo.h"

#define MIN_UPDATE_TIME_DELAY   (0.08f)	// Min time between network updates of scoring buffer.
#define INFINITE_SPAWN_TICKETS  0xFFFF		// Value used by spawn ticket limit to denote no limit.
#define INVALID_SCORING_TEAM    0xFF		// Value used to imply an invalid team

//////////////////////////////// CScoringModel /////////////////////////////////

class CScoringModel
{
public:
	// Construct
	CScoringModel(uint8 i_maxPlayers, uint8 i_maxParticipants);
	virtual ~CScoringModel();

	// The scoring model does not store its data internally. A buffer is provided
	// to it. Return the size of the buffer.  This size must be fixed for the
	// lifetime of the model.
	virtual uint GetBufferSize(void);

	// Set pointer to the scoring data buffer.
	void SetBuffer(void* i_buffer);

	// Get at the scoring data buffer
	void* GetBuffer(void) {return(d_buffer);}

	// Called right after SetBuffer() is called to give the model a chance to
	// initialize the data area. If used in a networked game, this is only called
	// on the DuplicationMaster.
	virtual void InitializeBuffer(void);

	// Return true if the scoring data was changed by the scoring model since the
	// last time this routine was called. If used in a networked game, this is
	// only called on the DuplicationMaster which is the only place the scoring
	// model should be changing the data.
	virtual bool HasDataChanged(void);

	// Called whenever the scoring data has changed outside control of the
	// scoring model. This is only called in a networked game and will only be
	// called on a Duplica. Duplicas should only respond to external changes to
	// the data.
	virtual void DataHasChanged(void);

	// Set pointer to the referee's scoring player info.
	virtual void SetPlayerInfo(DSScoringPlayerInfo* i_pPlayerInfo);

	enum EScoringType
	{
		ALL_KILLS,
		ENEMY_KILLS,
		TEAM_KILLS,
		NPC_KILLS,
		ALL_DEATHS,
		DEATHS,
		TEAM_KILLED,
		SUICIDES,
		APPREHENDS,
		OBJECTIVES,
		TOTAL_SCORE,
		ROUNDS_WON,
		ROUNDS_LOST
	};

	enum ETeamStatus
	{
		WINNING,
		LOSING,
		TIED,
		WON,
		LOST
	};

	static const uint8 INVALID_PLAYER = (uint8)~0;

	// Add to player/team counts
	virtual void AddToPlayerCount(EScoringType i_eType, uint8 i_playerId, uint8 i_targetId, int i_count);
	virtual void AddToTeamCount(EScoringType i_eType, uint8 i_teamId, uint8 i_targetId, int i_count);

	// Get player/team counts
	virtual int GetPlayerCount(EScoringType i_eType, uint8 i_playerId);
	virtual int GetTeamCount(EScoringType i_eType, uint8 i_teamId);

	// Get player/team using specific requirements
	virtual uint8 GetBestPlayer(EScoringType i_eType, int &o_total);
	virtual uint8 GetBestTeam(EScoringType i_eType, int &o_total);
	virtual uint8 GetBestPlayerOnTeam(EScoringType i_eType, uint8 i_teamId, int &o_total);
	virtual uint8 GetNextBestPlayerOnTeam(EScoringType i_eType, uint8 i_playerId, int &o_total);

	// Get ranking of players overall or by team
	virtual uint8 GetPlayerRanking(EScoringType i_eType, uint8 i_iRank) {return(0);}
	virtual uint8 GetPlayerCopRanking(EScoringType i_eType, uint8 i_playerId) {return(0);}
	virtual uint8 GetPlayerGangRanking(EScoringType i_eType, uint8 i_playerId) {return(0);}

	// Get MVP for specified instance
	virtual float GetPlayerMVPRating(uint8 i_playerId) {return(0);}
	virtual int GetPlayerCopMVPScore(uint8 i_playerId) {return(0);}
	virtual int GetPlayerGangMVPScore(uint8 i_playerId) {return(0);}
	uint8 GetTeamMVP(uint8 i_teamId);
	uint8 GetOverallMVP();

	// Get the competitive status of a team
	virtual CScoringModel::ETeamStatus GetTeamStatus(uint8 i_teamId);
	CScoringModel::ETeamStatus GetBlueTeamStatus();
	CScoringModel::ETeamStatus GetRedTeamStatus();

	// Team wins data
	uint8 GetAlphaWins();
	uint8 GetBetaWins();

	// Current state of the scoring system
	bool IsRecording();
	bool IsTabulated();

	// Reset info as needed for new round
	void ResetForNewRound();

	// Get at scoring player info
	int GetPlayerInfoCount();
	int GetTeamPlayerCount(uint8 i_teamId);
	const char* GetPlayerName(uint8 i_playerId);
	uint8 GetPlayerTeam(uint8 i_playerId);
	bool IsPlayerBlue(uint8 i_playerId);
	bool IsPlayerRed(uint8 i_playerId);
	bool IsTeamBlue(uint8 i_teamId);
	bool IsTeamRed(uint8 i_teamId);
	bool IsAlphaBlue();
	bool IsAlphaRed();
	bool IsBetaBlue();
	bool IsBetaRed();
	CPlayerProfile::EVariant GetPlayerProfileVariant(uint8 i_playerId);
	CPlayerProfile::EVariant GetTeamProfileVariant(uint8 i_teamId);

protected:
	// Parse the score data by player.
	int GetAllKills(uint8 i_id);
	int GetEnemyKills(uint8 i_id);
	int GetTeamKills(uint8 i_id);
	int GetNonPlayerKills(uint8 i_id);
	int GetAllDeaths(uint8 i_id);
	int GetDeathByEnemy(uint8 i_id);
	int GetDeathByTeammates(uint8 i_id);
	int GetSuicides(uint8 i_id);
	int GetApprehensions(uint8 i_id);
	int GetObjectives(uint8 i_id);
	virtual int GetTotalScore(uint8 i_id);

	// Change stuff
	bool HasChanged() { return( d_hasChanged ); }
	void HasChanged(bool change) { d_hasChanged = change; }

	// Team wins data
	void SetAlphaWins(uint8 i_count);
	void IncrementAlphaWins();
	void SetBetaWins(uint8 i_count);
	void IncrementBetaWins();
	void ResetTeamWins() {SetAlphaWins(0); SetBetaWins(0);}

	// Team bonus points data
	uint16 GetTeamBonus(uint8 i_teamId);
	void IncrementTeamBonus(uint8 i_teamId, uint16 i_count);
	void SetAlphaBonus(uint16 i_count);
	uint16 GetAlphaBonus();
	void IncrementAlphaBonus(uint16 i_count) {SetAlphaBonus(GetAlphaBonus() + i_count);}
	void SetBetaBonus(uint16 i_count);
	uint16 GetBetaBonus();
	void IncrementBetaBonus(uint16 i_count) {SetBetaBonus(GetBetaBonus() + i_count);}
	void ResetTeamBonus() {SetAlphaBonus(0); SetBetaBonus(0);}

	// Objective data
	void SetObjectiveAchieved(uint8 i_obj);
	bool IsObjectiveAchieved(uint8 i_obj);
	void ResetObjectiveAchieved();

	// Eliminate team objective
	bool IsTeamEliminated(uint8 i_teamId);

	// Current state of the scoring system
	void SetState(uint8 i_state);

	// Hud messages for scoring
	void ShowKillHudMessage(uint8 i_victim, uint8 i_attacker);
	void ShowApprehendHudMessage(uint8 i_victim, uint8 i_attacker);
	void ShowTagHudMessage(uint8 i_player);

	enum EObjectives
	{
		BLUE_CAPTURED_OBJ = 0x01,
		RED_CAPTURED_OBJ  = 0x02,
		BLUE_ELIMINATED   = 0x04,
		RED_ELIMINATED    = 0x08
	};

	enum EScoringStatus
	{
		RECORDING = 0,		// Accepting updates to the buffer
		TABULATED			// Calculated final round scores
	};

	enum EColumnType
	{
		NPC_DEATHS = 0,
		APPREHENSIONS,
		PRI_OBJECTIVES,
		NUM_COLUMNS
	};

	struct SMatchScore
	{
		uint8 d_alphaWins;
		uint8 d_betaWins;
		uint16 d_alphaBonus;
		uint16 d_betaBonus;
		uint8 d_objAchieved;
		uint8 d_state;
	};

	// Access buffer as 2-D array.
	static const int NUM_SUPPLEMENTARY_ROWS = 0;
	static const int NUM_SUPPLEMENTARY_COLUMNS = NUM_COLUMNS;
	typedef uint8 TScore;
	TScore &Score(int i_killer, int i_victim);
	SMatchScore& MatchScore(void);

	// Holds player info for displaying results on score board
	void PopulatePlayerInfo();
	void SetPlayerName(uint8 i_playerId, const char* i_name);
	void SetPlayerTeam(uint8 i_playerId, uint8 i_teamId);
	bool AreEnemies(uint8 i_alphaId, uint8 i_betaId);
	bool AreTeammates(uint8 i_alphaId, uint8 i_betaId);

	const uint8 d_maxPlayers;
	const uint8 d_maxParticipants;

private:
	// What we store our scoring stuff in.
	void* d_buffer;
	SMatchScore d_localMatchScore;

	// Our dataset of players' info.
	DSScoringPlayerInfo* d_pPlayerInfo;

	// Flag data changes.
	bool d_hasChanged;
	float d_timeLastUpdated;

	bool d_isAlphaTeamLeading; // both false on a tie
	bool d_isBetaTeamLeading;
};

///////////////////////////// CScoringModelCreator /////////////////////////////

class CScoringModelCreator : public CCreator
{
  public:
	static CScoringModel* Create(const char* i_name, uint8 i_maxPlayers,
			uint8 i_maxParticipants);

  protected:
	virtual CScoringModel* Create(uint8 i_maxPlayers, uint8 i_maxParticipants) = 0;
	static CScoringModelCreator* s_head;
};

// A macro for defining a scoring model creator. The arguments are:
//   scoreClass    - the class to be created.
//   scoreName     - the name (not quoted) used to find the creator.
//
// This macro implements the Create() function which simply uses new to create
// the object.
//
// NOTE: The constructor is implemented here because Add() calls Name() to
// check for multiple creators for a single name and Name() is not a valid
// function until its constructor is called.
#define SCORING_MODEL_CREATOR(scoreClass, scoreName)                           \
static class scoreClass##Creator : public CScoringModelCreator                 \
{                                                                              \
  public:                                                                      \
	scoreClass##Creator() {Add((CCreator**)&s_head);}                           \
                                                                               \
  protected:                                                                   \
	virtual const char* Name(void) {return(#scoreName);}                        \
	virtual CScoringModel* Create(uint8 i_maxPlayers, uint8 i_maxParticipants)  \
			{return(new scoreClass(i_maxPlayers, i_maxParticipants));}            \
} l_##scoreClass##Creator

inline CScoringModel::TScore &CScoringModel::Score(int i_killer,
																				 int i_victim)
{
	ASSERT(d_buffer != NULL);
	return(*(((TScore*)d_buffer) +
		(i_killer * (d_maxPlayers + NUM_SUPPLEMENTARY_COLUMNS)) + i_victim));
}

inline CScoringModel::SMatchScore &CScoringModel::MatchScore(void)
{
	if (d_buffer == NULL)
		return(d_localMatchScore);

	return(*(SMatchScore*)((char*)d_buffer +
			(d_maxPlayers + NUM_SUPPLEMENTARY_ROWS) *
			(d_maxPlayers + NUM_SUPPLEMENTARY_COLUMNS) * sizeof(TScore)));
}

#endif // SCORING_MODEL_H
