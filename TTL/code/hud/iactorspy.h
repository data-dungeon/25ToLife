/*
**
**  File:   iactorspy.h
**  Date:   August 2, 2005
**  By:     Bryant Collard
**  Desc:   Instrument to display actor's names (and hosts if applicable).
**
**  Copyright (c) 2005, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/hud/iactorspy.h $
**  $Revision: #1 $
**  $DateTime: 2005/08/02 15:34:31 $
**    $Author: Bryant_Collard $
**
*/

#pragma once

#ifndef I_ACTOR_SPY_H
#define I_ACTOR_SPY_H

#include "hud/ipanel/itext.h"
#include "GameHelper/Message.h"

class CActorSpyInstrument : public TextInstrument
{
  public:
	CActorSpyInstrument();
	~CActorSpyInstrument() {}

	void Initialize(void);
	virtual void Update(float i_deltaSec);

  private:
	bool d_active;
	CMessageOwner d_messageOwner;
	void ToggleActorSpy(void* i_data, ActorHandle i_sender, bool i_posted);
	CCallbackMessageHandler<CActorSpyInstrument> d_toggleActorSpyHandler;
};

#endif // I_ACTOR_SPY_H
