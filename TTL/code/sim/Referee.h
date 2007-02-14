/*
**
**  File:   Referee.h
**  Date:   October 5, 2004
**  By:     Bryant Collard
**  Desc:   The referee basically has three responsibilities; it controls
**          progression through a round, it manages participants, and it is
**          the installation site for game modes.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
*/

#ifndef REFEREE_H
#define REFEREE_H

#include "main/TTLConstants.h"
#include "main/ParticipantConfig.h"
#include "EngineHelper/Singleton.h"
#include "GameHelper/Message.h"
#include "GameHelper/Handle.h"
#include "sim/TeamingModel.h"
#include "sim/ScoringModel.h"
#include "environ/ModeScript.h"

class CPlayerAgent;
class WorldActorLoader;
handledef(CActor) ActorHandle;

/////////////////////////////////// Referee ////////////////////////////////////

#define PARAM_STRING_LEN  512

class Referee
{
  public:
	enum EPhase
	{
		READY,
		SETUP,
		POPULATING,
		REPOPULATE,
		PLAYING,
		REPORTING,
		NUM_PHASES
	};

	// Destroy
	virtual ~Referee();

	// Update
	virtual const char* Validate(void) = 0;
	virtual void Update(void);

	// Assignment
	virtual bool IsParticipationClosed(bool i_player) = 0;
	virtual void RegisterParticipant(CPlayerAgent* i_playerAgent) {}
	virtual void RegisterParticipant(CActor* i_actor) {}
	virtual void RemoveParticipant(CPlayerAgent* i_playerAgent) {}
	virtual void RemoveParticipant(CActor* i_actor) {}
	virtual void RequestTeam(uint8 i_id, uint8 i_requestedTeam) = 0;
	virtual void SetActive(uint8 i_id, bool i_active) = 0;

	// Phase.
	void LocalExitting(void) {d_localActive = false;}
	void EnablePhaseChanges(bool i_enable = true) {d_enablePhase = i_enable;}
	void DisablePhaseChanges(void) {EnablePhaseChanges(false);}
	EPhase GetPhase(void) {return(d_phase);}
	void PreventSetupPhaseExit(void)
		{DirectPhaseExit(SETUP, NO_CHANGE);}
	void PreventPopulatingPhaseExit(void)
		{DirectPhaseExit(POPULATING, NO_CHANGE);}
	void PreventPlayingPhaseExit(void)
		{DirectPhaseExit(PLAYING, NO_CHANGE);}
	void PreventReportingPhaseExit(void)
		{DirectPhaseExit(REPORTING, NO_CHANGE);}
	void AllowSetupPhaseExit(void)
		{DirectPhaseExit(SETUP, CHANGE_ALLOWED);}
	void AllowPopulatingPhaseExit(void)
		{DirectPhaseExit(POPULATING, CHANGE_ALLOWED);}
	void AllowPlayingPhaseExit(void)
		{DirectPhaseExit(PLAYING, CHANGE_ALLOWED);}
	void AllowReportingPhaseExit(void)
		{DirectPhaseExit(REPORTING, CHANGE_ALLOWED);}
	void CommandSetupPhaseExit(void)
		{DirectPhaseExit(SETUP, CHANGE_REQUIRED);}
	void CommandPopulatingPhaseExit(void)
		{DirectPhaseExit(POPULATING, CHANGE_REQUIRED);}
	void CommandPlayingPhaseExit(void)
		{DirectPhaseExit(PLAYING, CHANGE_REQUIRED);}
	void CommandReportingPhaseExit(void)
		{DirectPhaseExit(REPORTING, CHANGE_REQUIRED);}
	void SetSetupPhaseEndTime(int32 i_time)
		{SetPhaseEndTime(SETUP, i_time);}
	void SetPopulatingPhaseEndTime(int32 i_time)
		{SetPhaseEndTime(POPULATING, i_time);}
	void SetPlayingPhaseEndTime(int32 i_time)
		{SetPhaseEndTime(PLAYING, i_time);}
	void SetReportingPhaseEndTime(int32 i_time)
		{SetPhaseEndTime(REPORTING, i_time);}
	void SetSetupPhaseLifetime(int32 i_time)
		{SetPhaseLifetime(SETUP, i_time);}
	void SetPopulatingPhaseLifetime(int32 i_time)
		{SetPhaseLifetime(POPULATING, i_time);}
	void SetPlayingPhaseLifetime(int32 i_time)
		{SetPhaseLifetime(PLAYING, i_time);}
	void SetReportingPhaseLifetime(int32 i_time)
		{SetPhaseLifetime(REPORTING, i_time);}
	void ForceReadyPhaseExit(void);
	void ResetReadyPhaseExit(void);

	void SetRemainingPhaseTime(int32 i_time, bool i_exceedLifetime);

	void ComputeCurrentPhaseEndTime(void);
	int32 GetElapsedPhaseTime(void);
	int32 GetRemainingPhaseTime(void);
	int32 GetCurrentPhaseStartTime(void) {return(d_currentPhaseStartTime);}
	int32 GetPhaseCountdown(void);
	bool IsTimedPhase(void);

	// Control slots.
	bool ConfigureSlots(void);
	bool SetMaxSlots(uint8 i_maxSlots);
	uint8 GetMaxSlots(void) {return(d_maxSlots);}

	// Is this slot used?
	virtual bool IsParticipant(uint8 i_id) = 0;
	bool IsPlayer(uint8 i_id);
	bool IsNonPlayer(uint8 i_id);
	uint8 GetMaxPlayers(void) {return(d_maxPlayers);}
	uint8 GetMaxNonPlayers(void) {return(d_maxParticipants - d_maxPlayers);}
	uint8 GetMaxParticipants(void) {return(d_maxParticipants);}
	virtual uint8 GetNumStartPlayers(void) = 0;
	uint8 GetNumPlayers(void);
	uint8 GetNumActivePlayers(void);
	uint8 GetNumNonPlayers(void);
	uint8 GetNumParticipants(void);
	uint8 GetNextPlayer(uint8 i_lastId);
	uint8 GetNextNonPlayer(uint8 i_lastId);
	uint8 GetNextParticipant(uint8 i_lastId);

	// Parse the configuration.
	bool IsGhost(uint8 i_id);
	bool IsReady(uint8 i_id);
	bool IsAlive(uint8 i_id);
	bool IsAccepted(uint8 i_id);
	bool IsActivePlayer(uint8 i_id);

	// Get the assigned team.
	uint8 GetTeam(uint8 i_id);

	// Get the assigned team. Returns true if it matches the requested team.
	bool GetTeam(uint8 i_id, uint8 &o_team);

	// Spawn tickets.
	virtual void SetUnlimitedSpawnTickets(uint8 i_teamId) = 0;
	virtual void SetUnlimitedSpawnTicketsForAll(void) = 0;
	virtual void SetNumSpawnTickets(uint8 i_teamId, uint16 d_numSpawnTickets) = 0;
	virtual void SetNumSpawnTicketsForAll(uint16 d_numSpawnTickets) = 0;
	virtual bool DoesTeamLimitSpawnTickets(uint8 i_teamId) = 0;
	virtual bool RequestSpawnTicket(uint8 i_teamId) = 0;
	bool DoesTeamLimitSpawnTicketsForParticipant(uint8 i_participantId);
	bool RequestSpawnTicketForParticipant(uint8 i_participantId);
	bool DoesTeamLimitSpawnTicketsForLocalAvatar(void);
	bool RequestSpawnTicketForLocalAvatar(void);
	virtual uint16 ParticipantSpawnTicketCount(uint8 i_participantId) = 0;
	virtual uint16 TeamSpawnTicketCount(uint8 i_teamId) = 0;
	virtual bool IsParticipantTalking(uint8 i_id) { return false; }
	virtual bool IsParticipantHeadsetConnected(uint8 i_participantId) {return false;}
	virtual bool IsParticipantVoiceReDirected(uint8 i_participantId) {return false;}

	// Match
	virtual void SetRoundsInMatch(uint8 i_roundsInMatch) = 0;
	virtual uint8 GetRoundsInMatch(void) = 0;
	virtual uint8 GetRoundCount(void) = 0;
	virtual bool IsRoundOpen(void) = 0;

	// Get at player agents and actors.
	virtual CPlayerAgent* CreateLocalPlayerAgent(void) = 0;
	virtual void DestroyLocalPlayerAgent(void) = 0;
	CPlayerAgent* GetLocalPlayerAgent(void) {return(d_localAgent);}
	uint8 GetLocalPlayerAgentId(void);
	virtual CPlayerAgent* GetPlayerAgent(uint8 i_id) = 0;
	CActor* GetActor(uint8 i_id);
	virtual int GetPlayerAgentCount(void) = 0;
	bool DoAllAgentsHaveAvatars(void);

	// Populate world.
	void SetWorldLoader(const WorldActorLoader* i_worldLoader)
		{d_worldLoader = i_worldLoader;}
	virtual bool CanLoadWorldActors(int &o_start, int &o_end) = 0;
	virtual bool IsConsistent(void) {return(true);}

	// Game mode support.
	void ResetTeamingModel(void);
	void InstallTeamingModel(const char* i_name);
	void LoadTeamingModel(void);
	void ResetScoringModel(void);
	virtual void InstallScoringModel(const char* i_name);

	CTeamingModel &GetTeamingModel(void) {return(*d_teamingModel);}
	CScoringModel &GetScoringModel(void) {return(*d_scoringModel);}
	CMessageOwner &GetMessageOwner(void) {return(d_messageOwner);}

	// Make referee responsible for team roles so they stay consistent across
	// the network.
	virtual void SetAlphasRole(CTeamingModel::ERole i_eRole) = 0;
	virtual void SetBetasRole(CTeamingModel::ERole i_eRole) = 0;
	virtual void AssignAlphaToBlueTeam() = 0;
	virtual void AssignBetaToBlueTeam() = 0;
	virtual void AssignAlphaToRedTeam() = 0;
	virtual void AssignBetaToRedTeam() = 0;

	// Session specific info.
	virtual int GetSessionTeamWins(uint8 i_teamId) {return(0);}
	virtual void IncrementSessionAlphaWins() {};
	virtual void IncrementSessionBetaWins() {};

	// Logging
	static void EnableAgentsLog(bool i_logAgents) {s_logAgents = i_logAgents;}
	virtual void DumpAgentList(void) {}

	// Debug
	const char* GetPhaseName(EPhase i_phase);
	const char* GetPhaseName(void) {return(GetPhaseName(d_phase));}

  protected:
	Referee(NParticipantConfig::TConfig* i_config, uint8* i_directPhaseExit,
			int32* i_phaseEndTime, int32* i_phaseLifetime, uint8* i_teamRole,
			ActorHandle* i_actor, uint8 i_maxPlayers, uint8 i_maxParticipants);
	void AssignParticipant(uint8 i_id, CPlayerAgent* i_playerAgent);
	void AssignParticipant(uint8 i_id, CActor* i_actor);

	enum EPhaseChangeDirection
	{
		NO_CHANGE,
		CHANGE_ALLOWED,
		CHANGE_REQUIRED
	};
	EPhaseChangeDirection GetPhaseChangeDirection(EPhase &o_nextPhase);
	void SetPhase(EPhase i_phase, int32 i_startTime = -MAXINT);

	virtual void DirectPhaseExit(EPhase i_phase,
			EPhaseChangeDirection i_direction) = 0;
	virtual void SetPhaseEndTime(EPhase i_phase, int32 i_time) = 0;
	virtual void SetPhaseLifetime(EPhase i_phase, int32 i_time) = 0;
	virtual void UpdatePhase(void) = 0;
	virtual void ResetRoundCount(void) = 0;
	virtual void IncrementRoundCount(void) = 0;

	enum EPlayersStatus
	{
		ALL_INACTIVE,
		ALL_UNLOADED,
		ALL_LOADED,
		ALL_READY,
		OTHER
	};
	virtual EPlayersStatus DeterminePlayersStatus(void) = 0;

	virtual void CountPlayerSlots(uint8 &o_filled, uint8 &o_open,
			uint8 &o_closed) = 0;
	virtual void ClosePlayerSlots(uint8 i_numToClose) = 0;
	virtual void OpenPlayerSlots(uint8 i_numToOpen) = 0;

	template<class TYPE>
	uint8 FindOpenSlot(TYPE i_key, TYPE* i_keyArray, TYPE i_openKey,
			bool i_player);
	template<class TYPE>
	void CountPlayerSlots(TYPE* i_keyArray, TYPE i_openKey, TYPE i_closedKey,
			uint8 &o_filled, uint8 &o_open, uint8 &o_closed);
	template<class TYPE>
	void ClosePlayerSlots(uint8 i_numToClose, TYPE* i_keyArray, TYPE i_openKey,
			TYPE i_closedKey);
	template<class TYPE>
	void OpenPlayerSlots(uint8 i_numToOpen, TYPE* i_keyArray, TYPE i_openKey,
			TYPE i_closedKey);

	virtual void ApplyTeamingModel(void) = 0;
	virtual void ApplyScoringModel(void) {}

	CPlayerAgent* d_localAgent;

	bool d_localActive;
	bool d_enablePhase;
	EPhase d_phase;
	int32 d_currentPhaseStartTime;
	int32 d_currentPhaseEndTime;
	bool d_returnToReady;
	bool d_allPlayersPublished;
	bool d_populateFailed;

	uint8* d_directPhaseExit;
	int32* d_phaseEndTime;
	int32* d_phaseLifetime;

	ActorHandle* d_actor;
	const uint8 d_maxPlayers;
	const uint8 d_maxParticipants;
	uint8 d_maxSlots;

	const WorldActorLoader* d_worldLoader;

	CTeamingModel* d_teamingModel;
	CScoringModel* d_scoringModel;

	static bool s_logAgents;

  private:
	Referee();

	NParticipantConfig::TConfig* d_config;
	uint8* d_teamRole;
	CMessageOwner d_messageOwner;
	CTeamingModel d_defaultTeamingModel;
	CScoringModel d_defaultScoringModel;
	CModeScript d_modeScript;

	// Message handling
	REF_SEND_MESSAGE_HANDLER(CChangeOptions, Referee) d_changeOptionsHandler;
	REF_SEND_MESSAGE_HANDLER(CFixTeams, Referee) d_fixTeamsHandler;
};

// A global for accessing this bad-boy
DefineSingleton(Referee)
#define g_referee GetSingleton(Referee)

////////////////////////////// StandAloneReferee ///////////////////////////////

#define MAX_STANDALONE_PLAYERS       1
#define MAX_STANDALONE_PARTICIPANTS  56

class StandAloneReferee : public Referee
{
  public:
	StandAloneReferee();
	virtual ~StandAloneReferee();
	virtual const char* Validate(void);

	virtual bool IsParticipationClosed(bool i_player);
	virtual void RegisterParticipant(CPlayerAgent* i_playerAgent);
	virtual void RegisterParticipant(CActor* i_actor);
	virtual void RemoveParticipant(CPlayerAgent* i_playerAgent);
	virtual void RemoveParticipant(CActor* i_actor);
	virtual void RequestTeam(uint8 i_id, uint8 i_requestedTeam);
	virtual void SetActive(uint8 i_id, bool i_active);
	virtual bool IsParticipant(uint8 i_id);
	virtual uint8 GetNumStartPlayers(void);
	virtual void SetUnlimitedSpawnTickets(uint8 i_teamId);
	virtual void SetUnlimitedSpawnTicketsForAll(void);
	virtual void SetNumSpawnTickets(uint8 i_teamId, uint16 d_numSpawnTickets);
	virtual void SetNumSpawnTicketsForAll(uint16 d_numSpawnTickets);
	virtual bool DoesTeamLimitSpawnTickets(uint8 i_teamId);
	virtual bool RequestSpawnTicket(uint8 i_teamId);
	virtual void SetRoundsInMatch(uint8 i_roundsInMatch);
	virtual uint8 GetRoundsInMatch(void);
	virtual uint8 GetRoundCount(void);
	virtual bool IsRoundOpen(void);
	virtual CPlayerAgent* CreateLocalPlayerAgent(void);
	virtual void DestroyLocalPlayerAgent(void);
	virtual CPlayerAgent* GetPlayerAgent(uint8 i_id);
	virtual bool CanLoadWorldActors(int &o_start, int &o_end);
	virtual void InstallScoringModel(const char* i_name);
	virtual void SetAlphasRole(CTeamingModel::ERole i_eRole);
	virtual void SetBetasRole(CTeamingModel::ERole i_eRole);
	virtual void AssignAlphaToBlueTeam();
	virtual void AssignBetaToBlueTeam();
	virtual void AssignAlphaToRedTeam();
	virtual void AssignBetaToRedTeam();
	virtual int GetPlayerAgentCount(void);
	virtual uint16 ParticipantSpawnTicketCount(uint8 i_participantId);
	virtual uint16 TeamSpawnTicketCount(uint8 i_teamId);

  protected:
	virtual void DirectPhaseExit(EPhase i_phase,
			EPhaseChangeDirection i_direction);
	virtual void SetPhaseEndTime(EPhase i_phase, int32 i_time);
	virtual void SetPhaseLifetime(EPhase i_phase, int32 i_time);
	virtual void UpdatePhase(void);
	virtual void ResetRoundCount(void);
	virtual void IncrementRoundCount(void);
	virtual EPlayersStatus DeterminePlayersStatus(void);
	virtual void CountPlayerSlots(uint8 &o_filled, uint8 &o_open,
			uint8 &o_closed);
	virtual void ClosePlayerSlots(uint8 i_numToClose);
	virtual void OpenPlayerSlots(uint8 i_numToOpen);
	virtual void ApplyTeamingModel(void);

  private:
	void* d_participantKey[MAX_STANDALONE_PARTICIPANTS];
	NParticipantConfig::TConfig d_standAloneConfig[MAX_STANDALONE_PARTICIPANTS];
	ActorHandle d_standAloneActor[MAX_STANDALONE_PARTICIPANTS];
	uint8 d_standAloneDirectPhaseExit[NUM_PHASES];
	int32 d_standAlonePhaseEndTime[NUM_PHASES];
	int32 d_standAlonePhaseLifetime[NUM_PHASES];
	uint8 d_standAloneTeamRole[MAX_TEAMS];
	uint16 d_spawnTickets[MAX_TEAMS];
	uint8 d_roundsInMatch;
	uint8 d_roundCount;
};

/******************************************************************************/
/******************************************************************************/
template<class TYPE>
inline uint8 Referee::FindOpenSlot(TYPE i_key, TYPE* i_keyArray,
		TYPE i_openKey, bool i_player)
{
	int count = 0;
	uint8 openSlot = INVALID_PARTICIPANT;
	uint8 i;
	for (i = 0; i < d_maxPlayers; i++)
	{
		if (i_keyArray[i] == i_openKey)
		{
			if (i_player && (count == 0) && (openSlot == INVALID_PARTICIPANT))
				openSlot = i;
		}
		else if (i_keyArray[i] == i_key)
		{
			ASSERT(i_player);
			count++;
			openSlot = INVALID_PARTICIPANT;
		}
	}

	for (; i < d_maxParticipants; i++)
	{
		if (i_keyArray[i] == i_openKey)
		{
			if (!i_player && (count == 0) && (openSlot == INVALID_PARTICIPANT))
				openSlot = i;
		}
		else if (i_keyArray[i] == i_key)
		{
			ASSERT(!i_player);
			count++;
			openSlot = INVALID_PARTICIPANT;
		}
	}

	ASSERT(count <= 1);
	return(openSlot);
}

/******************************************************************************/
/******************************************************************************/
template<class TYPE>
inline void Referee::CountPlayerSlots(TYPE* i_keyArray, TYPE i_openKey,
		TYPE i_closedKey, uint8 &o_filled, uint8 &o_open, uint8 &o_closed)
{
	o_filled = 0;
	o_open = 0;
	o_closed = 0;
	int count = 0;

	for (uint8 i = 0; i < d_maxPlayers; i++)
	{
		if (i_keyArray[i] == i_openKey)
			o_open++;
		else if (i_keyArray[i] == i_closedKey)
			o_closed++;
		else
			o_filled++;
	}
}

/******************************************************************************/
/******************************************************************************/
template<class TYPE>
inline void Referee::ClosePlayerSlots(uint8 i_numToClose, TYPE* i_keyArray,
		TYPE i_openKey, TYPE i_closedKey)
{
	for (int i = (int)d_maxPlayers - 1; (i >= 0) && (i_numToClose > 0); i--)
	{
		if (i_keyArray[i] == i_openKey)
		{
			i_keyArray[i] = i_closedKey;
			i_numToClose--;
		}
	}
}

/******************************************************************************/
/******************************************************************************/
template<class TYPE>
inline void Referee::OpenPlayerSlots(uint8 i_numToOpen, TYPE* i_keyArray,
		TYPE i_openKey, TYPE i_closedKey)
{
	for (uint8 i = 0; (i < d_maxPlayers) && (i_numToOpen > 0); i++)
	{
		if (i_keyArray[i] == i_closedKey)
		{
			i_keyArray[i] = i_openKey;
			i_numToOpen--;
		}
	}
}

#endif
