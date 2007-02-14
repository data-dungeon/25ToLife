/*
**
**  File:   DOCombatant.h
**  Date:   January 13, 2004
**  By:     Bryant Collard
**  Desc:   Implementation of the combatant distributed object.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/NetZTTL/DOCombatant.h $
**  $Revision: #24 $
**  $DateTime: 2005/06/27 19:44:19 $
**    $Author: Bryant_Collard $
**
*/

#pragma once

#ifndef DO_COMBATANT_H
#define DO_COMBATANT_H

#include <netz.h>
#include "NetZTTL/DOCombatantDDL.h"
#include "Components/Component.h"
#include "GameHelper/Message.h"

class DSCombatantMode;
class DSCombatantState;

///////////////////////////////// DOCombatant /////////////////////////////////

class DOCombatant : public DOCLASS(DOCombatant)
{
  public:
	virtual ~DOCombatant();
	virtual void InitDO();
	void StateChanged(void);

  protected:
	virtual CActor* CreateProxy(const char* i_class);
	virtual GameInitGeometry GetInitGeometryFunc(void);
	virtual void InitMaster(void);
	virtual void InitProxy(void);
	virtual void BecomeMaster(void);
	virtual void BecomeProxy(void);
	virtual void PropagateMaster(float i_gameTime);
	virtual void Terminate(void);

  private:
	void Push(void);

	REF_SEND_MESSAGE_HANDLER(CToggleDebugHandler, DOCombatant)
			d_toggleDebugHandler;
	REF_SEND_MESSAGE_HANDLER(CClearDebugHandler, DOCombatant)
			d_clearDebugHandler;
	bool d_debug;
};

//////////////////////////// DOCombatantComponent /////////////////////////////

class DOCombatantComponent : public CActorComponent
{
  public:
	DOCombatantComponent(CCompActor &i_actor, DuplicatedObject &i_do,
			DSCombatantMode &i_mode, DSCombatantState &i_state);
	virtual void Initialize(void);

	virtual const char *ComponentName(void) {return("DOCombatant");}

	virtual void BeginFrame(void);
	virtual void EndUpdate(float i_deltaSec);
	void Pull(void);
	void StateChanged(void);

  private:
	DuplicatedObject &d_do;
	DSCombatantMode &d_mode;
	DSCombatantState &d_state;
	bool d_drClampAtGround;
	float d_time;

	REF_BASIC_MESSAGE_HANDLER(MunitionImpactAck, MESSAGE_VALUE, DOCombatantComponent) d_munitionImpactAckHandler;
};

#endif // DO_COMBATANT
