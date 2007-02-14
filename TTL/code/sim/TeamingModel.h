/*
**
**  File:   TeamingModel.h
**  Date:   October 5, 2004
**  By:     Bryant Collard
**  Desc:   The base class of a module that installs in the referee and that
**          manages team assignments.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/sim/TeamingModel.h $
**  $Revision: #17 $
**  $DateTime: 2005/08/08 15:39:00 $
**    $Author: Bryce $
**
*/

#ifndef TEAMING_MODEL_H
#define TEAMING_MODEL_H

#include "main/ParticipantConfig.h"
#include "main/TTLConstants.h"
#include "GameHelper/Creator.h"
#include "main/PlayerProfile.h"

class CPlayerAgent;
class CActor;

//////////////////////////////// CTeamingModel /////////////////////////////////

class CTeamingModel
{
  public:
	// Groups of teams
	enum EGroup
	{
		ALPHA = 0,
		BETA,
		NUM_GROUPS
	};

	// Team roles
	enum ERole
	{
		INVALID_ROLE = -1,
		BLUE = 0,
		RED,
		ENEMY,
		ENEMY2,
		FRIEND,
		AMBIENT,
		NUM_ROLES
	};

	CTeamingModel(NParticipantConfig::TConfig* i_config, uint8* i_role,
			uint8 i_maxPlayers, uint8 i_maxParticipants, uint8 i_maxTeams);

	virtual ~CTeamingModel() {}

	// Setup teams
	virtual void ValidateAssignments(void);
	virtual void AssignTeam(uint8 i_id, CPlayerAgent* i_playerAgent);
	virtual void AssignTeam(uint8 i_id, CActor* i_actor);
	virtual void ProcessTeamChanges(void);

	// Validity check for teams
	bool ValidTeams();
	bool TeamHasOpenSlot(uint8 i_id);

	// Query for new team and/or group
	virtual uint8 FindTeamInGroupAlpha() const;
	virtual uint8 FindTeamInGroupBeta() const;
	virtual uint8 FindTeamInSameGroup(uint8 i_id) const;
	virtual uint8 FindTeamInNewGroup(uint8 i_id) const;
	virtual void RequestTeamInSameGroup(uint8 i_id);
	virtual void RequestTeamInNewGroup(uint8 i_id);

	// Query player/team group affiliation
	virtual bool IsPlayerInGroupAlpha(uint8 i_id) const;
	virtual bool IsPlayerInGroupBeta(uint8 i_id) const;

	// Query team's affiliation
	virtual bool IsTeamInGroupAlpha(uint8 i_id) const;
	virtual bool IsTeamInGroupBeta(uint8 i_id) const;

	// Query roles
	ERole GetPlayerRole(uint8 i_id) const;
	ERole GetTeamRole(uint8 i_id) const;

	// Find the role of each group
	ERole GetAlphasRole() const;
	ERole GetBetasRole() const;
	bool IsAlphaBlue() const;
	bool IsBetaBlue() const;
	bool IsAlphaRed() const;
	bool IsBetaRed() const;

	// Assign the role of each group
	void SetAlphasRole(ERole i_eRole);
	void SetBetasRole(ERole i_eRole);
	void AssignAlphaToBlueTeam();
	void AssignBetaToBlueTeam();
	void AssignAlphaToRedTeam();
	void AssignBetaToRedTeam();

	// Query player role
	virtual CPlayerProfile::EVariant GetPlayerProfileVariant(uint8 i_id) const;
	virtual CPlayerProfile::EVariant GetTeamProfileVariant(uint8 i_id) const;

	// Query team's name
	virtual const char *GetTeamName(uint8 i_teamId) const;
	virtual const char *GetBlueTeamName(void) const;
	virtual const char *GetRedTeamName(void) const;

	// Query player/teams's team assignment. updated at beginning of round.
	virtual bool IsPlayerBlue(uint8 i_id) const;
	virtual bool IsPlayerRed(uint8 i_id) const;
	virtual bool IsTeamBlue(uint8 i_id) const;
	virtual bool IsTeamRed(uint8 i_id) const;

	// Query general roles
	virtual bool IsAmbient(uint8 i_id) const;

	// Query non-player relationships to players (only true if tested against
	// non-players)
	virtual bool IsPlayerFriend(uint8 i_id) const;
	virtual bool IsPlayerEnemy(uint8 i_id) const;
	virtual bool IsPlayerNeutral(uint8 i_id) const;

	// Query team relationships
	bool AreTeammateTeams(uint8 i_teamA, uint8 i_teamB) const
		{return(i_teamA == i_teamB);}
	virtual bool AreFriendTeams(uint8 i_teamA, uint8 i_teamB) const;
	virtual bool AreEnemyTeams(uint8 i_teamA, uint8 i_teamB) const;
	virtual bool AreNeutralTeams(uint8 i_teamA, uint8 i_teamB) const;

	// Team status
	bool IsTeamAllAlive(uint8 i_team) const;
	bool IsBlueAllAlive(void) const;
	bool IsRedAllAlive(void) const;
	bool IsTeamAllDead(uint8 i_team) const;
	bool IsBlueAllDead(void) const;
	bool IsRedAllDead(void) const;
	bool IsTeamAllPermanentlyDead(uint8 i_team) const;
	bool IsBlueAllPermanentlyDead(void) const;
	bool IsRedAllPermanentlyDead(void) const;
	int GetTeamPlayerCount(uint8 i_team) const;
	int GetBluePlayerCount() const;
	int GetRedPlayerCount() const;
	int GetTeamAliveCount(uint8 i_team) const;
	int GetBlueAliveCount() const;
	int GetRedAliveCount() const;

	// Query participant relationships
	bool AreTeammates(uint8 i_idA, uint8 i_idB,
			bool i_includeGhosts = false) const;
	bool AreFriends(uint8 i_idA, uint8 i_idB,
			bool i_includeGhosts = false) const;
	bool AreEnemies(uint8 i_idA, uint8 i_idB,
			bool i_includeGhosts = false) const;
	bool AreNeutral(uint8 i_idA, uint8 i_idB,
			bool i_includeGhosts = false) const;

	// Iterate
	uint8 GetNextTeammate(uint8 i_idQuery, uint8 i_idLast,
			bool i_includeQuery = true, bool i_wrap = true,
			bool i_includeGhosts = false) const;
	uint8 GetNextFriend(uint8 i_idQuery, uint8 i_idLast,
			bool i_includeQuery = true, bool i_wrap = true,
			bool i_includeGhosts = false) const;
	uint8 GetNextEnemy(uint8 i_idQuery, uint8 i_idLast,
			bool i_includeQuery = true, bool i_wrap = true,
			bool i_includeGhosts = false) const;
	uint8 GetNextNeutral(uint8 i_idQuery, uint8 i_idLast,
			bool i_includeQuery = true, bool i_wrap = true,
			bool i_includeGhosts = false) const;
	uint8 GetNextNonTeammate(uint8 i_idQuery, uint8 i_idLast,
			bool i_includeQuery = true, bool i_wrap = true,
			bool i_includeGhosts = false) const;
	uint8 GetNextNonFriend(uint8 i_idQuery, uint8 i_idLast,
			bool i_includeQuery = true, bool i_wrap = true,
			bool i_includeGhosts = false) const;
	uint8 GetNextNonEnemy(uint8 i_idQuery, uint8 i_idLast,
			bool i_includeQuery = true, bool i_wrap = true,
			bool i_includeGhosts = false) const;
	uint8 GetNextNonNeutral(uint8 i_idQuery, uint8 i_idLast,
			bool i_includeQuery = true, bool i_wrap = true,
			bool i_includeGhosts = false) const;
	uint8 GetPrevTeammate(uint8 i_idQuery, uint8 i_idLast,
			bool i_includeQuery = true, bool i_wrap = true,
			bool i_includeGhosts = false) const;
	uint8 GetPrevFriend(uint8 i_idQuery, uint8 i_idLast,
			bool i_includeQuery = true, bool i_wrap = true,
			bool i_includeGhosts = false) const;
	uint8 GetPrevEnemy(uint8 i_idQuery, uint8 i_idLast,
			bool i_includeQuery = true, bool i_wrap = true,
			bool i_includeGhosts = false) const;
	uint8 GetPrevNeutral(uint8 i_idQuery, uint8 i_idLast,
			bool i_includeQuery = true, bool i_wrap = true,
			bool i_includeGhosts = false) const;
	uint8 GetPrevNonTeammate(uint8 i_idQuery, uint8 i_idLast,
			bool i_includeQuery = true, bool i_wrap = true,
			bool i_includeGhosts = false) const;
	uint8 GetPrevNonFriend(uint8 i_idQuery, uint8 i_idLast,
			bool i_includeQuery = true, bool i_wrap = true,
			bool i_includeGhosts = false) const;
	uint8 GetPrevNonEnemy(uint8 i_idQuery, uint8 i_idLast,
			bool i_includeQuery = true, bool i_wrap = true,
			bool i_includeGhosts = false) const;
	uint8 GetPrevNonNeutral(uint8 i_idQuery, uint8 i_idLast,
			bool i_includeQuery = true, bool i_wrap = true,
			bool i_includeGhosts = false) const;

  protected:
	const uint8 d_maxPlayers;
	const uint8 d_maxParticipants;
	const uint8 d_maxTeams;

  private:
	NParticipantConfig::TConfig* d_config;
	uint8* d_role;

	CTeamingModel();
	bool GetTeams(uint8 i_idA, uint8 i_idB, uint8 &o_teamA, uint8 &o_teamB,
			bool i_includeGhosts) const;
};

/******************************************************************************/
/******************************************************************************/
inline bool CTeamingModel::AreTeammates(uint8 i_idA, uint8 i_idB,
		bool i_includeGhosts) const
{
	uint8 teamA, teamB;
	return(GetTeams(i_idA, i_idB, teamA, teamB, i_includeGhosts) &&
			(teamA == teamB));
}

/******************************************************************************/
/******************************************************************************/
inline bool CTeamingModel::AreFriends(uint8 i_idA, uint8 i_idB,
		bool i_includeGhosts) const
{
	uint8 teamA, teamB;
	return(GetTeams(i_idA, i_idB, teamA, teamB, i_includeGhosts) &&
			AreFriendTeams(teamA, teamB));
}

/******************************************************************************/
/******************************************************************************/
inline bool CTeamingModel::AreEnemies(uint8 i_idA, uint8 i_idB,
		bool i_includeGhosts) const
{
	uint8 teamA, teamB;
	return(GetTeams(i_idA, i_idB, teamA, teamB, i_includeGhosts) &&
			AreEnemyTeams(teamA, teamB));
}

/******************************************************************************/
/******************************************************************************/
inline bool CTeamingModel::AreNeutral(uint8 i_idA, uint8 i_idB,
		bool i_includeGhosts) const
{
	uint8 teamA, teamB;
	return(GetTeams(i_idA, i_idB, teamA, teamB, i_includeGhosts) &&
			AreNeutralTeams(teamA, teamB));
}

///////////////////////////// CTeamingModelCreator /////////////////////////////

class CTeamingModelCreator : public CCreator
{
  public:
	static CTeamingModel* Create(const char* i_name,
			NParticipantConfig::TConfig* &i_config, uint8* &i_role,
			uint8 i_maxPlayers, uint8 i_maxParticipants, uint8 i_maxTeams);

  protected:
	virtual CTeamingModel* Create(NParticipantConfig::TConfig* &i_config,
			uint8* &i_role, uint8 i_maxPlayers, uint8 i_maxParticipants,
			uint8 i_maxTeams) = 0;
	static CTeamingModelCreator* s_head;
};

// A macro for defining a teaming model creator. The arguments are:
//   teamClass - the class to be created.
//   teamName  - the name (not quoted) used to find the creator.
//
// This macro implements the Create() function which simply uses new to create
// the object.
//
// NOTE: The constuctor is implemented here because Add() calls Name() to
// check for multiple creators for a single name and Name() is not a valid
// function until its constructor is called.
#define TEAMING_MODEL_CREATOR(teamClass, teamName)                             \
static class teamClass##Creator : public CTeamingModelCreator                  \
{                                                                              \
  public:                                                                      \
	teamClass##Creator() {Add((CCreator**)&s_head);}                            \
                                                                               \
  protected:                                                                   \
	virtual const char* Name(void) {return(#teamName);}                         \
	virtual CTeamingModel* Create(NParticipantConfig::TConfig* &i_config,       \
			uint8* &i_role, uint8 i_maxPlayers, uint8 i_maxParticipants,          \
			uint8 i_maxTeams)                                                     \
		{return(new teamClass(i_config, i_role, i_maxPlayers,                    \
				i_maxParticipants, i_maxTeams));}                                  \
} l_##teamClass##Creator

#endif // TEAMING_MODEL_H
