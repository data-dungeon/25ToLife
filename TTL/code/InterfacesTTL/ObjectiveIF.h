/*
**
**  File:   ObjectiveIF.h
**  Date:   July 14, 2005
**  By:     Bryant Collard
**  Desc:   Interfaces to get at objective attributes.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/InterfacesTTL/ObjectiveIF.h $
**  $Revision: #1 $
**  $DateTime: 2005/07/16 10:40:25 $
**    $Author: Bryant_Collard $
**
*/

#pragma once

#ifndef OBJECTIVE_IF_H
#define OBJECTIVE_IF_H

#include "Interfaces/Interface.h"
#include "GameHelper/Handle.h"

class CActor;
handledef(CActor) ActorHandle;

#define RESIDENCE_MASK 0x0f
#define CONTROL_MASK   0xf0

class CObjectiveIF : public CInterface
{
  public:
	enum EResidence
	{
		HOME = 0x00,
		TRANSIT = 0x01,
		DROPPED = 0x02
	};
	enum EControl
	{
		AVAILABLE = 0x00,
		CLAIMED = 0x10,
		OWNED = 0x20
	};

	static uint GetID(void) {return(CInterface::GetValidID(s_objectiveID));}
	static CObjectiveIF* GetInterface(CActor* i_actor);
	static CObjectiveIF* GetInterface(ActorHandle i_handle);

	// Should override
	virtual EResidence GetResidence(void) {return(HOME);}
	virtual EControl GetControl(void) {return(AVAILABLE);}
	virtual uint32 GetTimeOfResidenceMS(void) {return(0);}
	virtual uint32 GetTimeOfControlMS(void) {return(0);}
	virtual uint32 GetTimeOfStateMS(void) {return(0);}
	virtual ActorHandle GetMostRecentHolder(void) {return(INVALID_OBJECT_HANDLE);}

	// May override
	virtual uint8 GetState(void)
		{return((uint8)GetResidence() | (uint8)GetControl());}
	virtual bool IsResidence(EResidence i_residence)
		{return(GetResidence() == i_residence);}
	virtual bool IsControl(EControl i_control)
		{return(GetControl() == i_control);}
	virtual bool IsState(uint8 i_state) {return(GetState() == i_state);}

  private:
	static uint s_objectiveID;
};

#endif // OBJECTIVE_IF_H
