/*
**
**  File:   Scatter.cpp
**  Date:   June 4, 2003
**  By:     Bryant Collard
**  Desc:   Scattered protagonists.
**
**  Copyright (c) 2000, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/gamelibnew/stage/scatter.cpp $
**  $Revision: 41 $
**      $Date: 8/18/03 7:24p $
**    $Author: Travis Nixon $
**
*/

// Pre-compiled header //
#include "stage/StagePCH.h"

// GameLib includes //
#include "EngineHelper/timer.h"

#define MAX_DISPLACE_ATTEMPTS   10
#define CONVERGE_THRESHOLD      1.0e-2f

/***************************************************************************/
/***************************************************************************/
void CScatter::Advance(void)
{
/*
 *	Process each actor.
 */
	CCastMember* curr = d_actorHead;
	while (curr)
	{
		CProtagonist* actor = (CProtagonist*)curr->GetActor();

/*
 *		Perform updates until the end of the frame is reached.
 */
		float updateStartTime, deltaSec;
		float updateEndTime = 0.0f;
		float frameEndTime = g_timer.GetFrameSec();
		float maxUpdateSec = actor->GetMaxUpdateSec();
		do
		{
/*
 *			Set the start time of this update to the end time of the just completed
 *			update and find the update end (clamped to the end of the frame).
 */
			updateStartTime = updateEndTime;
			updateEndTime += maxUpdateSec;
			if (updateEndTime > frameEndTime)
				updateEndTime = frameEndTime;
#ifdef ENABLE_RECORD
			float recordDeltaSec = updateEndTime - updateStartTime;
			if (actor->ManageRecording(updateStartTime, recordDeltaSec))
			{
				updateEndTime = updateStartTime + recordDeltaSec;
				frameEndTime = updateEndTime;
			}
#endif
/*
 *			Tell the actor to begin an update.
 */
			actor->BeginUpdate();
			actor->SetUpdating(true);
/*
 *			Treat the beginning of the update as the last known time no
 *			discontinuities are detected and the end of the update as the first
 *			known time an irreconcilable	state exists.
 */
			float noDiscontinuityTime = updateStartTime;
			float irreconcilableTime = updateEndTime;
/*
 *			Look for the end of the update period or a discontinuity.
 */
			while (true)
			{
/*
 *				Have the actor attempt an update.
 */
				deltaSec = updateEndTime - updateStartTime;
				actor->AttemptUpdate(deltaSec);
/*
 *				Detect collisions.
 */
				CCollisionList* list = NULL;
				uint effect = CProtagonist::EFFECT_NONE;
				if( deltaSec > 0.0f )
					effect = CollisionDetect(actor, list);
/*
 *				If no irreconcialable collision was found and the actor advanced to
 *				the end of the update period without finding a discontinuity or if
 *				a discontinuity was found, accept collisions and go on to the next
 *				update. Note that collisions may have been detected but they may not
 *				result in a discontinuity.
 */
				if (((effect & CProtagonist::EFFECT_IRRECONCILABLE) == 0) &&
						((updateEndTime >= irreconcilableTime) ||
						((effect & CProtagonist::EFFECT_DISCONTINUITY) != 0)))
				{
					actor->SetUpdating(false);
					if (list != NULL)
						CollisionAccept(actor, list);
					break;
				}
/*
 *				To get here, an irreconcilable collision has been detected and a
 *				discontinuity that will cause the actor to avoid that state is
 *				being looked for. Give up when the time between an update with no
 *				discontinuities and an irreconcilable collision is too small.
 */
				if ((irreconcilableTime - noDiscontinuityTime) <=
						CONVERGE_THRESHOLD)
				{
/*
 *					Tell the actor to end the update and that it could not be
 *					advanced to the end of the frame (including the amount of time it
 *					will have "lost"). This would be a good time for the actor to try
 *					to adjust itself so things can start working again next frame.
 */
					actor->SetUpdating(false);
					actor->EndUpdate(deltaSec);
					actor->UpdateFailed(frameEndTime - updateEndTime);
/*
 *					Discontinue update attempts for this frame.
 */
					return;
				}
/*
 *				Set a new irreconcilable or no discontinutiy time and compute a
 *				new update end time between them. Loop to attempt to reach this
 *				new time.
 */
				if ((effect & CProtagonist::EFFECT_IRRECONCILABLE) != 0)
					irreconcilableTime = updateEndTime;
				else
					noDiscontinuityTime = updateEndTime;
				updateEndTime = 0.5f * (noDiscontinuityTime + irreconcilableTime);
			}
/*
 *			Tell the actor to end the update.
 */
			actor->EndUpdate(deltaSec);
/*
 *			Loop to try the next update.
 */
		} while (updateEndTime < frameEndTime);
/*
 *		Move on to the next actor.
 */
		curr = curr->GetNextActor();
	}
}

/***************************************************************************/
/***************************************************************************/
uint CScatter::CollisionDetect(CProtagonist* i_actor, CCollisionList* &o_list)
{
/*
 *	Reset the stack and get a collision list.
 */
	g_collisionSystem.ResetStack();
	o_list = CCollisionList::New(&g_collisionSystem.GetStack());
	if (o_list == NULL)
		return(CProtagonist::EFFECT_NONE);
/*
 *	Setup
 */
	CCollisionStackLock lock;
	CStageEntity* entity = (CStageEntity*)i_actor;
	uint effect = CProtagonist::EFFECT_NONE;
/*
 *	Detect collisions until no displacement results or until a set number of
 *	attempts are made.
 */
	for (int i = 0; i < MAX_DISPLACE_ATTEMPTS; i++)
	{
/*
 *		Clear the displaced bit.
 */
		effect &= ~CProtagonist::EFFECT_DISPLACED;
/*
 *		Test the static entity.
 */
		CCollisionEvent* event = CCollisionEvent::TestEntityPair(entity,
				g_collisionSystem.StaticEntity(), &g_collisionSystem.GetStack());
		if (event && event->GetHeadPair())
		{
			CCollisionEvent* staticEvent;
			CCollisionPair** headPair = &(event->GetHeadPair());
			do
			{
				staticEvent = event;
				if ((*headPair)->GetNext() != NULL)
				{
					staticEvent =
							CCollisionEvent::New(&g_collisionSystem.GetStack());
					if (staticEvent != NULL)
					{
						staticEvent->GetHeadPair() = (*headPair)->MoveToEvent(
								staticEvent, *headPair, NULL);
						staticEvent->SetEntities(event->GetQueryEntity(),
								event->GetTargetEntity());
					}
					else
						staticEvent = event;
				}
				lock.Set(g_collisionSystem.GetStack());
				SET_LOCK_NAME(lock, "CollideScatter");
				effect |= i_actor->CollisionDetect(g_collisionSystem.StaticEntity(),
						staticEvent, false);
				g_collisionSystem.ResetStack();
				lock.Release();
				if ((effect & CProtagonist::EFFECT_IRRECONCILABLE) != 0)
					return(effect);
				o_list->AddEvent(staticEvent, true);
			} while(staticEvent != event);
		}
/*
 *		Test terrain.
 */
		event = CCollisionEvent::TestEntityPair(entity,
				g_collisionSystem.TerrainEntity(), &g_collisionSystem.GetStack());
		if (event)
		{
			lock.Set(g_collisionSystem.GetStack());
			SET_LOCK_NAME(lock, "CollideScatter");
			effect |= i_actor->CollisionDetect(g_collisionSystem.TerrainEntity(),
					event, false);
			g_collisionSystem.ResetStack();
			lock.Release();
			if ((effect & CProtagonist::EFFECT_IRRECONCILABLE) != 0)
				return(effect);
			o_list->AddEvent(event, true);
		}
/*
 *		Stop detecting if no displacement occured.
 */
		if ((effect & CProtagonist::EFFECT_DISPLACED) == 0)
			break;
	}
/*
 *	Return.
 */
	return(effect);
}

/***************************************************************************/
/***************************************************************************/
void CScatter::CollisionAccept(CProtagonist* i_actor, CCollisionList* i_list)
{
/*
 *	The collision list contains all of the collision events for the cluster.
 *	For each possible pairing, compile the list of associated events and
 *	notify the participants. The list of events will be ordered from the last
 *	detected to the first.
 */
	CCollisionStackLock lock;
	lock.Set(g_collisionSystem.GetStack());
	SET_LOCK_NAME(lock, "CollideAccept");

	CStageEntity* entity = (CStageEntity*)i_actor;
	CCollisionEvent* event;
	if ((event = BuildEventList(i_list, entity,
			g_collisionSystem.TerrainEntity())) != NULL)
		i_actor->CollisionAccept(g_collisionSystem.TerrainEntity(), event);

	if ((event = BuildEventList(i_list, entity,
			g_collisionSystem.StaticEntity())) != NULL)
		i_actor->CollisionAccept(g_collisionSystem.StaticEntity(), event);

	lock.Release();
}

/***************************************************************************/
/***************************************************************************/
CCollisionEvent* CScatter::BuildEventList(CCollisionList* i_list,
		const CStageEntity* i_query, const CStageEntity* i_target)
{
	CCollisionEvent head;
	CCollisionEvent* tail = head.SetTempListNext(NULL);
	CCollisionEvent* event = i_list->GetHeadEvent();
	while (event)
	{
		if ((i_query == event->GetQueryEntity()) &&
				(i_target == event->GetTargetEntity()))
		{
			tail->SetTempListNext(event->SetTempListNext(NULL));
			tail = event;
		}
		event = event->GetNext();
	}
	return(head.GetTempListNext());
}
