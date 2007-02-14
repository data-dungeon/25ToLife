/*
**
**  File:   KillsScoringModel.h
**  Date:   October 14, 2004
**  By:     Bryant Collard
**  Desc:   A scoring model that keeps track of kills.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/sim/KillsScoringModel.h $
**  $Revision: #15 $
**  $DateTime: 2005/07/15 15:42:01 $
**    $Author: Bryce $
**
*/

#ifndef KILLS_SCORING_MODEL_H
#define KILLS_SCORING_MODEL_H

#include "sim/ScoringModel.h"
#include "GameHelper/Message.h"

class CKillsScoringModel : public CScoringModel
{
  public:
	CKillsScoringModel(uint8 i_maxPlayers, uint8 i_maxParticipants);
	~CKillsScoringModel();

  private:
	// Messages
	CMessageOwner d_messageOwner;
	REF_SEND_MESSAGE_HANDLER(CStartReadyUp, CKillsScoringModel) d_startReadyUpHandler;
	REF_SEND_MESSAGE_HANDLER(CBeginRound, CKillsScoringModel) d_beginRoundHandler;
	REF_SEND_MESSAGE_HANDLER(CEndRound, CKillsScoringModel) d_endRoundHandler;
	REF_BASIC_MESSAGE_HANDLER(CKill, MESSAGE_VALUE, CKillsScoringModel) d_killHandler;
	REF_BASIC_MESSAGE_HANDLER(CApprehend, MESSAGE_VALUE, CKillsScoringModel) d_apprehendHandler;
	REF_SEND_MESSAGE_HANDLER(CSessionMasterChanged, CKillsScoringModel) d_sessionMasterChangedHandler;
};

#endif // KILLS_SCORING_MODEL_H
