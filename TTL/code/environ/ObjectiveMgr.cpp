/*
**
**  File:   ObjectiveMgr.cpp
**  Date:   July 7, 2005
**  By:     Bryant Collard
**  Desc:   Contains a list of objectives(pickups) in a level.
**
**  Copyright (c) 2005, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/environ/ObjectiveMgr.cpp $
**  $Revision: #2 $
**  $DateTime: 2005/07/16 10:40:25 $
**    $Author: Bryant_Collard $
**
*/

// Pre-compiled header
#include "TTLPCH.h"

// My header
#include "environ/ObjectiveMgr.h"

// TTL headers
#include "InterfacesTTL/ObjectiveIF.h"

// Setup the singleton
DeclareSingleton(CObjectiveMgr);

/******************************************************************************/
/******************************************************************************/
CObjectiveMgr::CObjectiveMgr() : d_pool(0, 1), d_cast(d_pool)
{
}

/******************************************************************************/
/******************************************************************************/
CObjectiveMgr::~CObjectiveMgr()
{
}

/******************************************************************************/
/******************************************************************************/
void CObjectiveMgr::Clear(void)
{
	d_cast.Clear();
	d_pool.Clear();
}

/******************************************************************************/
/******************************************************************************/
bool CObjectiveMgr::Add(CActor* i_objective)
{
	// Validate objective.
	if ((i_objective == NULL) || d_cast.IsMember(i_objective) ||
			(CObjectiveIF::GetInterface(i_objective) == NULL))
		return(false);

	return(d_cast.AddActor(i_objective) != NULL);
}

/******************************************************************************/
/******************************************************************************/
bool CObjectiveMgr::Remove(CActor* i_objective)
{
	return(d_cast.RemoveActor(i_objective));
}

/******************************************************************************/
/******************************************************************************/
bool CObjectiveMgr::IsObjective(CActor* i_objective) const
{
	return(d_cast.IsMember(i_objective));
}

/******************************************************************************/
/******************************************************************************/
bool CObjectiveMgr::IsObjective(ActorHandle i_objective) const
{
	return(d_cast.IsMember(CActor::FromHandle(i_objective)));
}

/******************************************************************************/
/******************************************************************************/
uint CObjectiveMgr::Count(void)
{
	return(d_cast.Members());
}

/******************************************************************************/
/******************************************************************************/
CActor* CObjectiveMgr::GetNext(CActor* i_objective) const
{
	CCastMember* castMember = FindNextCastMember(i_objective);

	if (castMember != NULL)
		return(castMember->GetActor());

	return(NULL);
}

/******************************************************************************/
/******************************************************************************/
CActor* CObjectiveMgr::GetNext(CActor* i_objective,
		CObjectiveIF::EResidence i_residence) const
{
	CActor* objective;
	CObjectiveIF* objectiveIF;
	CCastMember* curr = FindNextCastMember(i_objective);
	while(ProcessCastMember(curr, objective, objectiveIF))
	{
		if (objectiveIF->IsResidence(i_residence))
			return(objective);
		curr = curr->GetNextActor();
	}
	return(NULL);
}

/******************************************************************************/
/******************************************************************************/
CActor* CObjectiveMgr::GetNext(CActor* i_objective,
		CObjectiveIF::EControl i_control) const
{
	CActor* objective;
	CObjectiveIF* objectiveIF;
	CCastMember* curr = FindNextCastMember(i_objective);
	while(ProcessCastMember(curr, objective, objectiveIF))
	{
		if (objectiveIF->IsControl(i_control))
			return(objective);
		curr = curr->GetNextActor();
	}
	return(NULL);
}

/******************************************************************************/
/******************************************************************************/
CActor* CObjectiveMgr::GetNext(CActor* i_objective, uint8 i_state) const
{
	CActor* objective;
	CObjectiveIF* objectiveIF;
	CCastMember* curr = FindNextCastMember(i_objective);
	while(ProcessCastMember(curr, objective, objectiveIF))
	{
		if (objectiveIF->IsState(i_state))
			return(objective);
		curr = curr->GetNextActor();
	}
	return(NULL);
}

/******************************************************************************/
/******************************************************************************/
CActor* CObjectiveMgr::GetFirst(CObjectiveIF::EResidence i_residence) const
{
	int32 bestTime = MAXINT;
	CActor* bestObjective = NULL;
	CActor* objective;
	CObjectiveIF* objectiveIF;
	CCastMember* curr = d_cast.GetNextMember(NULL);
	while(ProcessCastMember(curr, objective, objectiveIF))
	{
		if (objectiveIF->IsResidence(i_residence))
		{
			int32 time = objectiveIF->GetTimeOfResidenceMS();
			if (time < bestTime)
			{
				bestTime = time;
				bestObjective = objective;
			}
		}
		curr = curr->GetNextActor();
	}
	return(bestObjective);
}

/******************************************************************************/
/******************************************************************************/
CActor* CObjectiveMgr::GetLast(CObjectiveIF::EResidence i_residence) const
{
	int32 bestTime = -MAXINT;
	CActor* bestObjective = NULL;
	CActor* objective;
	CObjectiveIF* objectiveIF;
	CCastMember* curr = d_cast.GetNextMember(NULL);
	while(ProcessCastMember(curr, objective, objectiveIF))
	{
		if (objectiveIF->IsResidence(i_residence))
		{
			int32 time = objectiveIF->GetTimeOfResidenceMS();
			if (time > bestTime)
			{
				bestTime = time;
				bestObjective = objective;
			}
		}
		curr = curr->GetNextActor();
	}
	return(bestObjective);
}

/******************************************************************************/
/******************************************************************************/
CActor* CObjectiveMgr::GetEarlier(CActor* i_objective,
		CObjectiveIF::EResidence i_residence) const
{
	if (i_objective == NULL)
		return(GetLast(i_residence));

	CObjectiveIF* objectiveIF = CObjectiveIF::GetInterface(i_objective);
	if ((objectiveIF == NULL) || !objectiveIF->IsResidence(i_residence))
	{
		ASSERT(false);
		return(NULL);
	}

	// Find objectives transitioning at the same time (but only once).
	bool foundBase = false;

	int32 baseTime = objectiveIF->GetTimeOfResidenceMS();
	int32 bestTime = -MAXINT;
	CActor* bestObjective = NULL;
	CActor* objective;
	CCastMember* curr = d_cast.GetNextMember(NULL);
	while(ProcessCastMember(curr, objective, objectiveIF))
	{
		if (objective == i_objective)
			foundBase = true;
		else if (objectiveIF->IsResidence(i_residence))
		{
			int32 time = objectiveIF->GetTimeOfResidenceMS();
			if (foundBase)
			{
				if ((time <= baseTime) && (time > bestTime))
				{
					bestTime = time;
					bestObjective = objective;
				}
			}
			else
			{
				if ((time < baseTime) && (time > bestTime))
				{
					bestTime = time;
					bestObjective = objective;
				}
			}
		}
		curr = curr->GetNextActor();
	}
	return(bestObjective);
}

/******************************************************************************/
/******************************************************************************/
CActor* CObjectiveMgr::GetLater(CActor* i_objective,
		CObjectiveIF::EResidence i_residence) const
{
	if (i_objective == NULL)
		return(GetFirst(i_residence));

	CObjectiveIF* objectiveIF = CObjectiveIF::GetInterface(i_objective);
	if ((objectiveIF == NULL) || !objectiveIF->IsResidence(i_residence))
	{
		ASSERT(false);
		return(NULL);
	}

	// Find objectives dropped at the same time (but only once).
	bool foundBase = false;

	int32 baseTime = objectiveIF->GetTimeOfResidenceMS();
	int32 bestTime = MAXINT;
	CActor* bestObjective = NULL;
	CActor* objective;
	CCastMember* curr = d_cast.GetNextMember(NULL);
	while(ProcessCastMember(curr, objective, objectiveIF))
	{
		if (objective == i_objective)
			foundBase = true;
		else if (objectiveIF->IsResidence(i_residence))
		{
			int32 time = objectiveIF->GetTimeOfResidenceMS();
			if (foundBase)
			{
				if ((time >= baseTime) && (time < bestTime))
				{
					bestTime = time;
					bestObjective = objective;
				}
			}
			else
			{
				if ((time > baseTime) && (time < bestTime))
				{
					bestTime = time;
					bestObjective = objective;
				}
			}
		}
		curr = curr->GetNextActor();
	}
	return(bestObjective);
}

/******************************************************************************/
/******************************************************************************/
CActor* CObjectiveMgr::GetFirst(CObjectiveIF::EControl i_control) const
{
	int32 bestTime = MAXINT;
	CActor* bestObjective = NULL;
	CActor* objective;
	CObjectiveIF* objectiveIF;
	CCastMember* curr = d_cast.GetNextMember(NULL);
	while(ProcessCastMember(curr, objective, objectiveIF))
	{
		if (objectiveIF->IsControl(i_control))
		{
			int32 time = objectiveIF->GetTimeOfControlMS();
			if (time < bestTime)
			{
				bestTime = time;
				bestObjective = objective;
			}
		}
		curr = curr->GetNextActor();
	}
	return(bestObjective);
}

/******************************************************************************/
/******************************************************************************/
CActor* CObjectiveMgr::GetLast(CObjectiveIF::EControl i_control) const
{
	int32 bestTime = -MAXINT;
	CActor* bestObjective = NULL;
	CActor* objective;
	CObjectiveIF* objectiveIF;
	CCastMember* curr = d_cast.GetNextMember(NULL);
	while(ProcessCastMember(curr, objective, objectiveIF))
	{
		if (objectiveIF->IsControl(i_control))
		{
			int32 time = objectiveIF->GetTimeOfControlMS();
			if (time > bestTime)
			{
				bestTime = time;
				bestObjective = objective;
			}
		}
		curr = curr->GetNextActor();
	}
	return(bestObjective);
}

/******************************************************************************/
/******************************************************************************/
CActor* CObjectiveMgr::GetEarlier(CActor* i_objective,
		CObjectiveIF::EControl i_control) const
{
	if (i_objective == NULL)
		return(GetLast(i_control));

	CObjectiveIF* objectiveIF = CObjectiveIF::GetInterface(i_objective);
	if ((objectiveIF == NULL) || !objectiveIF->IsControl(i_control))
	{
		ASSERT(false);
		return(NULL);
	}

	// Find objectives transitioning at the same time (but only once).
	bool foundBase = false;

	int32 baseTime = objectiveIF->GetTimeOfControlMS();
	int32 bestTime = -MAXINT;
	CActor* bestObjective = NULL;
	CActor* objective;
	CCastMember* curr = d_cast.GetNextMember(NULL);
	while(ProcessCastMember(curr, objective, objectiveIF))
	{
		if (objective == i_objective)
			foundBase = true;
		else if (objectiveIF->IsControl(i_control))
		{
			int32 time = objectiveIF->GetTimeOfControlMS();
			if (foundBase)
			{
				if ((time <= baseTime) && (time > bestTime))
				{
					bestTime = time;
					bestObjective = objective;
				}
			}
			else
			{
				if ((time < baseTime) && (time > bestTime))
				{
					bestTime = time;
					bestObjective = objective;
				}
			}
		}
		curr = curr->GetNextActor();
	}
	return(bestObjective);
}

/******************************************************************************/
/******************************************************************************/
CActor* CObjectiveMgr::GetLater(CActor* i_objective,
		CObjectiveIF::EControl i_control) const
{
	if (i_objective == NULL)
		return(GetFirst(i_control));

	CObjectiveIF* objectiveIF = CObjectiveIF::GetInterface(i_objective);
	if ((objectiveIF == NULL) || !objectiveIF->IsControl(i_control))
	{
		ASSERT(false);
		return(NULL);
	}

	// Find objectives dropped at the same time (but only once).
	bool foundBase = false;

	int32 baseTime = objectiveIF->GetTimeOfControlMS();
	int32 bestTime = MAXINT;
	CActor* bestObjective = NULL;
	CActor* objective;
	CCastMember* curr = d_cast.GetNextMember(NULL);
	while(ProcessCastMember(curr, objective, objectiveIF))
	{
		if (objective == i_objective)
			foundBase = true;
		else if (objectiveIF->IsControl(i_control))
		{
			int32 time = objectiveIF->GetTimeOfControlMS();
			if (foundBase)
			{
				if ((time >= baseTime) && (time < bestTime))
				{
					bestTime = time;
					bestObjective = objective;
				}
			}
			else
			{
				if ((time > baseTime) && (time < bestTime))
				{
					bestTime = time;
					bestObjective = objective;
				}
			}
		}
		curr = curr->GetNextActor();
	}
	return(bestObjective);
}

/******************************************************************************/
/******************************************************************************/
CActor* CObjectiveMgr::GetFirst(uint8 i_state) const
{
	int32 bestTime = MAXINT;
	CActor* bestObjective = NULL;
	CActor* objective;
	CObjectiveIF* objectiveIF;
	CCastMember* curr = d_cast.GetNextMember(NULL);
	while(ProcessCastMember(curr, objective, objectiveIF))
	{
		if (objectiveIF->IsState(i_state))
		{
			int32 time = objectiveIF->GetTimeOfStateMS();
			if (time < bestTime)
			{
				bestTime = time;
				bestObjective = objective;
			}
		}
		curr = curr->GetNextActor();
	}
	return(bestObjective);
}

/******************************************************************************/
/******************************************************************************/
CActor* CObjectiveMgr::GetLast(uint8 i_state) const
{
	int32 bestTime = -MAXINT;
	CActor* bestObjective = NULL;
	CActor* objective;
	CObjectiveIF* objectiveIF;
	CCastMember* curr = d_cast.GetNextMember(NULL);
	while(ProcessCastMember(curr, objective, objectiveIF))
	{
		if (objectiveIF->IsState(i_state))
		{
			int32 time = objectiveIF->GetTimeOfStateMS();
			if (time > bestTime)
			{
				bestTime = time;
				bestObjective = objective;
			}
		}
		curr = curr->GetNextActor();
	}
	return(bestObjective);
}

/******************************************************************************/
/******************************************************************************/
CActor* CObjectiveMgr::GetEarlier(CActor* i_objective, uint8 i_state) const
{
	if (i_objective == NULL)
		return(GetLast(i_state));

	CObjectiveIF* objectiveIF = CObjectiveIF::GetInterface(i_objective);
	if ((objectiveIF == NULL) || !objectiveIF->IsState(i_state))
	{
		ASSERT(false);
		return(NULL);
	}

	// Find objectives transitioning at the same time (but only once).
	bool foundBase = false;

	int32 baseTime = objectiveIF->GetTimeOfStateMS();
	int32 bestTime = -MAXINT;
	CActor* bestObjective = NULL;
	CActor* objective;
	CCastMember* curr = d_cast.GetNextMember(NULL);
	while(ProcessCastMember(curr, objective, objectiveIF))
	{
		if (objective == i_objective)
			foundBase = true;
		else if (objectiveIF->IsState(i_state))
		{
			int32 time = objectiveIF->GetTimeOfStateMS();
			if (foundBase)
			{
				if ((time <= baseTime) && (time > bestTime))
				{
					bestTime = time;
					bestObjective = objective;
				}
			}
			else
			{
				if ((time < baseTime) && (time > bestTime))
				{
					bestTime = time;
					bestObjective = objective;
				}
			}
		}
		curr = curr->GetNextActor();
	}
	return(bestObjective);
}

/******************************************************************************/
/******************************************************************************/
CActor* CObjectiveMgr::GetLater(CActor* i_objective, uint8 i_state) const
{
	if (i_objective == NULL)
		return(GetFirst(i_state));

	CObjectiveIF* objectiveIF = CObjectiveIF::GetInterface(i_objective);
	if ((objectiveIF == NULL) || !objectiveIF->IsState(i_state))
	{
		ASSERT(false);
		return(NULL);
	}

	// Find objectives dropped at the same time (but only once).
	bool foundBase = false;

	int32 baseTime = objectiveIF->GetTimeOfStateMS();
	int32 bestTime = MAXINT;
	CActor* bestObjective = NULL;
	CActor* objective;
	CCastMember* curr = d_cast.GetNextMember(NULL);
	while(ProcessCastMember(curr, objective, objectiveIF))
	{
		if (objective == i_objective)
			foundBase = true;
		else if (objectiveIF->IsState(i_state))
		{
			int32 time = objectiveIF->GetTimeOfStateMS();
			if (foundBase)
			{
				if ((time >= baseTime) && (time < bestTime))
				{
					bestTime = time;
					bestObjective = objective;
				}
			}
			else
			{
				if ((time > baseTime) && (time < bestTime))
				{
					bestTime = time;
					bestObjective = objective;
				}
			}
		}
		curr = curr->GetNextActor();
	}
	return(bestObjective);
}

/******************************************************************************/
/******************************************************************************/
bool CObjectiveMgr::AreAll(CObjectiveIF::EResidence i_residence) const
{
	CActor* objective;
	CObjectiveIF* objectiveIF;
	CCastMember* curr = d_cast.GetNextMember(NULL);
	while(ProcessCastMember(curr, objective, objectiveIF))
	{
		if (!objectiveIF->IsResidence(i_residence))
			return(false);
		curr = curr->GetNextActor();
	}
	return(true);
}

/******************************************************************************/
/******************************************************************************/
bool CObjectiveMgr::AreNone(CObjectiveIF::EResidence i_residence) const
{
	CActor* objective;
	CObjectiveIF* objectiveIF;
	CCastMember* curr = d_cast.GetNextMember(NULL);
	while(ProcessCastMember(curr, objective, objectiveIF))
	{
		if (objectiveIF->IsResidence(i_residence))
			return(false);
		curr = curr->GetNextActor();
	}
	return(true);
}

/******************************************************************************/
/******************************************************************************/
bool CObjectiveMgr::AreAll(CObjectiveIF::EControl i_control) const
{
	CActor* objective;
	CObjectiveIF* objectiveIF;
	CCastMember* curr = d_cast.GetNextMember(NULL);
	while(ProcessCastMember(curr, objective, objectiveIF))
	{
		if (!objectiveIF->IsControl(i_control))
			return(false);
		curr = curr->GetNextActor();
	}
	return(true);
}

/******************************************************************************/
/******************************************************************************/
bool CObjectiveMgr::AreNone(CObjectiveIF::EControl i_control) const
{
	CActor* objective;
	CObjectiveIF* objectiveIF;
	CCastMember* curr = d_cast.GetNextMember(NULL);
	while(ProcessCastMember(curr, objective, objectiveIF))
	{
		if (objectiveIF->IsControl(i_control))
			return(false);
		curr = curr->GetNextActor();
	}
	return(true);
}

/******************************************************************************/
/******************************************************************************/
bool CObjectiveMgr::AreAll(uint8 i_state) const
{
	CActor* objective;
	CObjectiveIF* objectiveIF;
	CCastMember* curr = d_cast.GetNextMember(NULL);
	while(ProcessCastMember(curr, objective, objectiveIF))
	{
		if (!objectiveIF->IsState(i_state))
			return(false);
		curr = curr->GetNextActor();
	}
	return(true);
}

/******************************************************************************/
/******************************************************************************/
bool CObjectiveMgr::AreNone(uint8 i_state) const
{
	CActor* objective;
	CObjectiveIF* objectiveIF;
	CCastMember* curr = d_cast.GetNextMember(NULL);
	while(ProcessCastMember(curr, objective, objectiveIF))
	{
		if (objectiveIF->IsState(i_state))
			return(false);
		curr = curr->GetNextActor();
	}
	return(true);
}

/******************************************************************************/
/******************************************************************************/
CCastMember* CObjectiveMgr::FindCastMember(CActor* i_objective) const
{
	CCastMember* curr = i_objective->CastHead();
	while(curr != NULL)
	{
		if (curr->GetCast() == &d_cast)
			return(curr);
		curr = curr->GetNextCast();
	}
	return(NULL);
}

/******************************************************************************/
/******************************************************************************/
CCastMember* CObjectiveMgr::FindNextCastMember(CActor* i_objective) const
{
	if (i_objective == NULL)
		return(d_cast.GetNextMember(NULL));

	CCastMember* castMember = FindCastMember(i_objective);
	if (castMember != NULL)
		return(castMember->GetNextActor());

	return(NULL);
}

/******************************************************************************/
/******************************************************************************/
bool CObjectiveMgr::ProcessCastMember(CCastMember* i_castMember,
		CActor* &o_objective, CObjectiveIF* &o_objectiveIF) const
{
	if (i_castMember != NULL)
	{
		o_objective = i_castMember->GetActor();
		ASSERT_PTR(o_objective);
		if (o_objective != NULL)
		{
			o_objectiveIF = CObjectiveIF::GetInterface(o_objective);
			ASSERT_PTR(o_objectiveIF);
			return(o_objectiveIF != NULL);
		}
	}
	return(false);
}
