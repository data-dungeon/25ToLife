/*
**
**  File:   TagScoringModel.h
**  Date:   December 27, 2004
**  By:     Bryce Thomsen
**  Desc:   A scoring model that keeps track of kills and tags.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/sim/TagScoringModel.h $
**  $Revision: #17 $
**  $DateTime: 2005/07/15 15:42:01 $
**    $Author: Bryce $
**
*/

#ifndef TAG_SCORING_MODEL_H
#define TAG_SCORING_MODEL_H

#include "sim/ScoringModel.h"
#include "GameHelper/Message.h"

class CTagScoringModel : public CScoringModel
{
  public:
	CTagScoringModel(uint8 i_maxPlayers, uint8 i_maxParticipants);
	~CTagScoringModel();

  protected:
	virtual int GetTotalScore(uint8 i_id);

  private:
	// Messages
	CMessageOwner d_messageOwner;
	REF_SEND_MESSAGE_HANDLER(CStartReadyUp, CTagScoringModel) d_startReadyUpHandler;
	REF_SEND_MESSAGE_HANDLER(CBeginRound, CTagScoringModel) d_beginRoundHandler;
	REF_SEND_MESSAGE_HANDLER(CEndRound, CTagScoringModel) d_endRoundHandler;
	REF_BASIC_MESSAGE_HANDLER(CKill, MESSAGE_VALUE, CTagScoringModel) d_killHandler;
	REF_BASIC_MESSAGE_HANDLER(CTag, MESSAGE_VALUE, CTagScoringModel) d_tagsHandler;
	REF_SEND_MESSAGE_HANDLER(CTeamBonus, CTagScoringModel) d_teamBonusHandler;
	REF_SEND_MESSAGE_HANDLER(CSessionMasterChanged, CTagScoringModel) d_sessionMasterChangedHandler;
};

#endif // TAG_SCORING_MODEL_H
