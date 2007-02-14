/*
**
**  File:   ObjectiveMgr.h
**  Date:   July 7, 2005
**  By:     Bryant Collard
**  Desc:   Contains a list of objectives(pickups) in a level.
**
**  Copyright (c) 2005, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/environ/ObjectiveMgr.h $
**  $Revision: #2 $
**  $DateTime: 2005/07/16 10:40:25 $
**    $Author: Bryant_Collard $
**
*/

#pragma once

#ifndef OBJECTIVE_MGR_H
#define OBJECTIVE_MGR_H

#include "stage/Cast.h"
#include "stage/BasicCastMemberPool.h"
#include "GameHelper/Handle.h"
#include "EngineHelper/Singleton.h"
#include "InterfacesTTL/ObjectiveIF.h"

class CCastMember;
class CActor;
handledef(CActor) ActorHandle;

class CObjectiveMgr
{
  public:
	// Construction/destruction.
	CObjectiveMgr();
	~CObjectiveMgr();

	// Manage the list.
	void Clear(void);
	bool Add(CActor* i_objective);
	bool Remove(CActor* i_objective);
	bool IsObjective(CActor* i_objective) const;
	bool IsObjective(ActorHandle i_objective) const;
	uint Count(void);

	// Get at lists of objectives.
	CActor* GetNext(CActor* i_objective) const;
	CActor* GetNext(CActor* i_objective,
			CObjectiveIF::EResidence i_residence) const;
	CActor* GetNext(CActor* i_objective, CObjectiveIF::EControl i_control) const;
	CActor* GetNext(CActor* i_objective, uint8 i_state) const;

	// Get at time ordered lists of objectives.
	CActor* GetFirst(CObjectiveIF::EResidence i_residence) const;
	CActor* GetLast(CObjectiveIF::EResidence i_residence) const;
	CActor* GetEarlier(CActor* i_objective,
			CObjectiveIF::EResidence i_residence) const;
	CActor* GetLater(CActor* i_objective,
			CObjectiveIF::EResidence i_residence) const;

	CActor* GetFirst(CObjectiveIF::EControl i_control) const;
	CActor* GetLast(CObjectiveIF::EControl i_control) const;
	CActor* GetEarlier(CActor* i_objective,
			CObjectiveIF::EControl i_control) const;
	CActor* GetLater(CActor* i_objective,
			CObjectiveIF::EControl i_control) const;

	CActor* GetFirst(uint8 i_state) const;
	CActor* GetLast(uint8 i_state) const;
	CActor* GetEarlier(CActor* i_objective, uint8 i_state) const;
	CActor* GetLater(CActor* i_objective, uint8 i_state) const;

	// Queries.
	bool AreAll(CObjectiveIF::EResidence i_residence) const;
	bool AreNone(CObjectiveIF::EResidence i_residence) const;
	bool AreAll(CObjectiveIF::EControl i_control) const;
	bool AreNone(CObjectiveIF::EControl i_control) const;
	bool AreAll(uint8 i_state) const;
	bool AreNone(uint8 i_state) const;

  private:
	// Find the cast member in the cast.
	CCastMember* FindCastMember(CActor* i_objective) const;
	CCastMember* FindNextCastMember(CActor* i_objective) const;
	bool ProcessCastMember(CCastMember* i_castMember, CActor* &o_objective,
			CObjectiveIF* &o_interface) const;

	// The cast member pool. Must be before the cast so the pool is constructed
	// before (not so important) and destructed after (very important) it.
	CBasicCastMemberPool d_pool;

	// The cast.
	CCast d_cast;
};

// A global for accessing this bad-boy
DefineSingleton(CObjectiveMgr)
#define g_objectiveMgr GetSingleton(CObjectiveMgr)

#endif // OBJECTIVE_MGR_H
