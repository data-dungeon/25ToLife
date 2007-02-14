/*
**
**  File:   DOReferee.h
**  Date:   January 13, 2004
**  By:     Bryant Collard
**  Desc:   Implementation of the referee distributed object.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
*/

#pragma once

#ifndef DO_REFEREE_H
#define DO_REFEREE_H

#include <netz.h>
#include "NetZTTL/DORefereeDDL.h"
#include "sim/Referee.h"
#include "main/ParticipantConfig.h"
#include "GameHelper/Handle.h"
#include "GameHelper/LogUtil.h"
#include "GameHelper/Callback.h"

class DOPlayerAgent;
class DOActor;
handledef(CActor) ActorHandle;

class DOReferee : public DOCLASS(DOReferee), public Referee
{
  public:
	DOReferee();

	virtual void InitDO(void);
	virtual void OperationEnd(DOOperation* i_operation);
	virtual const char* Validate(void);
	virtual void Update(void);

	// Participant configuration
	void SubmitParticipant(DuplicatedObject* i_do, uint8 i_id,
			NParticipantConfig::TConfig i_config);
	void UpdateParticipant(DuplicatedObject* i_do, uint8 i_id);
	void WithdrawParticipant(DuplicatedObject* i_do, uint8 i_id);
	void UpdateIds(void);
	void UpdateConfig(uint8 i_id, NParticipantConfig::TConfig i_config);
	virtual bool IsParticipationClosed(bool i_player);
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
	virtual int GetPlayerAgentCount(void);
	virtual uint16 ParticipantSpawnTicketCount(uint8 i_participantId);
	virtual uint16 TeamSpawnTicketCount(uint8 i_teamId);
	virtual bool IsParticipantTalking(uint8 i_id);
	virtual bool CanLoadWorldActors(int &o_start, int &o_end);
	virtual bool IsConsistent(void);
	virtual bool IsParticipantHeadsetConnected(uint8 i_participantId);
	virtual bool IsParticipantVoiceReDirected(uint8 i_participantId);
	virtual void SetAlphasRole(CTeamingModel::ERole i_eRole);
	virtual void SetBetasRole(CTeamingModel::ERole i_eRole);
	virtual void AssignAlphaToBlueTeam();
	virtual void AssignBetaToBlueTeam();
	virtual void AssignAlphaToRedTeam();
	virtual void AssignBetaToRedTeam();
	virtual int GetSessionTeamWins(uint8 i_teamId);
	virtual void IncrementSessionAlphaWins();
	virtual void IncrementSessionBetaWins();
	virtual void DumpAgentList(void);

	// Game mode support.
	void UpdateScoringModelBuffer(void);

	void SetWorldExtents(void);

  protected:
	// Phase
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
	virtual void ApplyScoringModel(void);

	virtual CMessageOwner* MessageOwner(void);

  private:
	bool ValidateParticipant(DuplicatedObject* i_do, uint8 i_id, bool &o_isAgent);
	void AddAgent(DOPlayerAgent* i_do, uint8 i_id);
	void RemoveAgent(uint8 i_id);
	void AddAvatar(DOActor* i_do, uint8 i_id);
	void RemoveAvatar(uint8 i_id);
	void AddActor(DOActor* i_do, uint8 i_id);
	void RemoveActor(uint8 i_id);
	void ReconcileParticipants(void);
	void IdentifyPlayers(void);
	void ClearPlayers(void);
	bool ManagePopulate(void);

	ActorHandle d_networkedActor[MAX_PARTICIPANTS];
	DOPlayerAgent* d_player[MAX_PLAYERS];
	DOActor* d_nonPlayer[MAX_PARTICIPANTS - MAX_PLAYERS];
	DOPlayerAgent* d_localDOAgent;

	// Message handling
	REF_SEND_MESSAGE_HANDLER(CChangeConfig, DOReferee) d_changeConfigHandler;
	REF_SEND_MESSAGE_HANDLER(COpenRound, DOReferee) d_openRoundHandler;
	REF_SEND_MESSAGE_HANDLER(CStartRound, DOReferee) d_startRoundHandler;
	REF_SEND_MESSAGE_HANDLER(CEndRound, DOReferee) d_endRoundHandler;
	REF_SEND_MESSAGE_HANDLER(CAssignActor, DOReferee) d_assignActorHandler;
	REF_BASIC_MESSAGE_HANDLER(CRequestSpawnTicket, MESSAGE_VALUE, DOReferee)
			d_requestSpawnTicketHandler;

	CCallback<CScoringModel> d_callback;

  // Debugging
  private:
	CLogFile d_logFile;
	REF_SEND_MESSAGE_HANDLER(CLog, DOReferee) d_logHandler;
};

#endif // DO_REFEREE
