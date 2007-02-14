/*
**
**  File:   RobberyScoringModel.h
**  Date:   December 27, 2004
**  By:     Bryce Thomsen
**  Desc:   A scoring model that keeps track of kills and robbery objective.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/sim/RobberyScoringModel.h $
**  $Revision: #14 $
**  $DateTime: 2005/07/15 15:42:01 $
**    $Author: Bryce $
**
*/

#ifndef ROBBERY_SCORING_MODEL_H
#define ROBBERY_SCORING_MODEL_H

#include "sim/ScoringModel.h"
#include "GameHelper/Message.h"

class CRobberyScoringModel : public CScoringModel
{
  public:
	CRobberyScoringModel(uint8 i_maxPlayers, uint8 i_maxParticipants);
	~CRobberyScoringModel();

	// Get the competitive status of a team
	virtual CScoringModel::ETeamStatus GetTeamStatus(uint8 i_teamId);

  protected:
	virtual int GetTotalScore(uint8 i_id);

  private:
	// Messages
	CMessageOwner d_messageOwner;
	REF_SEND_MESSAGE_HANDLER(CStartReadyUp, CRobberyScoringModel) d_startReadyUpHandler;
	REF_SEND_MESSAGE_HANDLER(CBeginRound, CRobberyScoringModel) d_beginRoundHandler;
	REF_SEND_MESSAGE_HANDLER(CEndRound, CRobberyScoringModel) d_endRoundHandler;
	REF_BASIC_MESSAGE_HANDLER(CKill, MESSAGE_VALUE, CRobberyScoringModel) d_killHandler;
	REF_BASIC_MESSAGE_HANDLER(CApprehend, MESSAGE_VALUE, CRobberyScoringModel) d_apprehendHandler;
	//REF_BASIC_MESSAGE_HANDLER(CObjectiveTaken, MESSAGE_ACTOR_HANDLE, CRobberyScoringModel) d_objectiveTakenHandler;
	//REF_BASIC_MESSAGE_HANDLER(CObjectiveReturned, MESSAGE_ACTOR_HANDLE, CRobberyScoringModel) d_objectiveReturnedHandler;
	REF_BASIC_MESSAGE_HANDLER(CObjectiveCaptured, MESSAGE_ACTOR_HANDLE, CRobberyScoringModel) d_objectiveCapturedHandler;
	REF_SEND_MESSAGE_HANDLER(CSessionMasterChanged, CRobberyScoringModel) d_sessionMasterChangedHandler;
};

#endif // ROBBERY_SCORING_MODEL_H
