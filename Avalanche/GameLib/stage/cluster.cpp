/*
**
**  File:   Cluster.cpp
**  Date:   June 4, 2003
**  By:     Bryant Collard
**  Desc:   Clusters
**
**  Copyright (c) 2000, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/gamelibnew/stage/cluster.cpp $
**  $Revision: 58 $
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
CCluster::CCluster(CCastMemberPool &i_castMemberPool) : CCast(i_castMemberPool)
{
	prev = next = NULL;
}

/***************************************************************************/
// We have to do this here, as we have vitual functions being called in
// the DeleteMember
/***************************************************************************/
CCluster::~CCluster()
{
	while (d_actorHead)
	{
		CActor *actor = d_actorHead->GetActor();
		d_castMemberPool.DeleteMember(d_actorHead);
		if (actor->ShouldDelete())
			CActor::Destroy(actor);
	}
}

/***************************************************************************/
/***************************************************************************/
void CCluster::Advance(void)
{
/*
 *	Find the most restrictive update time.
 */
	float maxUpdateSec = 100.0f;
	CCastMember* curr = d_actorHead;
	while (curr)
	{
		if ((curr->GetActor())->GetMaxUpdateSec() < maxUpdateSec)
			maxUpdateSec = (curr->GetActor())->GetMaxUpdateSec();
		curr = curr->GetNextActor();
	}
/*
 *	Perform updates until the end of the frame is reached.
 */
	float updateStartTime, deltaSec;
	float updateEndTime = 0.0f;
	float frameEndTime = g_timer.GetFrameSec();
	do
	{
/*
 *		Set the start time of this update to the end time of the just completed
 *		update and find the update end (clamped to the end of the frame).
 */
		updateStartTime = updateEndTime;
		updateEndTime += maxUpdateSec;
		if (updateEndTime > frameEndTime)
			updateEndTime = frameEndTime;
#ifdef ENABLE_RECORD
		curr = d_actorHead;
		while(curr)
		{
			float recordDeltaSec = updateEndTime - updateStartTime;
			if ((curr->GetActor())->ManageRecording(updateStartTime,
					recordDeltaSec))
			{
				updateEndTime = updateStartTime + recordDeltaSec;
				frameEndTime = updateEndTime;
			}
			curr = curr->GetNextActor();
		}
#endif
/*
 *		Tell each actor to begin an update.
 */
		curr = d_actorHead;
		while (curr)
		{
			CProtagonist* actor = (CProtagonist*)(curr->GetActor());
			actor->BeginUpdate();
			actor->SetUpdating(true);
			curr = curr->GetNextActor();
		}
/*
 *		Treat the beginning of the update as the last known time no
 *		discontinuities are detected and the end of the update as the first
 *		known time an irreconcilable	state exists.
 */
		float noDiscontinuityTime = updateStartTime;
		float irreconcilableTime = updateEndTime;
/*
 *		Look for the end of the update period or a discontinuity.
 */
		while (true)
		{
/*
 *			Find the delta time for this attempt and have each actor attempt
 *			an update.
 */
			deltaSec = updateEndTime - updateStartTime;
			curr = d_actorHead;
			while (curr)
			{
				CProtagonist* actor = (CProtagonist*)(curr->GetActor());
				actor->AttemptUpdate(deltaSec);
				curr = curr->GetNextActor();
			}
/*
 *			Detect collisions.
 */
			CCollisionList* list = NULL;
			ECollisionResult result = NO_DISCONTINUITY;
			if( deltaSec > 0.0f )
				result = CollisionDetect(list);
/*
 *			If the cluster advanced to the end of the update period without finding
 *			a discontinuity or if a discontinuity was found, accept collisions and
 *			go on to the next update. Note that collisions may have been detected
 *			but they may not result in a discontinuity.
 */
			if (((result == NO_DISCONTINUITY) &&
					(updateEndTime >= irreconcilableTime)) ||
					(result == FOUND_DISCONTINUITY))
			{
				curr = d_actorHead;
				while (curr)
				{
					((CProtagonist*)(curr->GetActor()))->SetUpdating(false);
					curr = curr->GetNextActor();
				}
				if (list != NULL)
					CollisionAccept(list);
				break;
			}
/*
 *			To get here, an irreconcilable collision was detected and a
 *			discontinuity that will cause the entities to avoid that state is
 *			being looked for. Give up when the time between an update with no
 *			discontinuities and an irreconcilable collision is too small.
 */
			if ((irreconcilableTime - noDiscontinuityTime) <= CONVERGE_THRESHOLD)
			{
/*
 *				Tell each actor they are not updating.
 */
				curr = d_actorHead;
				while (curr)
				{
					((CProtagonist*)(curr->GetActor()))->SetUpdating(false);
					curr = curr->GetNextActor();
				}
/*
 *				Tell each actor to end the update.
 */
				curr = d_actorHead;
				while (curr)
				{
					((CProtagonist*)(curr->GetActor()))->EndUpdate(deltaSec);
					curr = curr->GetNextActor();
				}
/*
 *				Tell each of the actors that they could not be advanced to the end
 *				of the frame and the amount of time they will have "lost". This
 *				would be a good time for an actor to try to adjust itself so things
 *				can start working again next frame.
 */
				deltaSec = frameEndTime - updateEndTime;
				curr = d_actorHead;
				while (curr)
				{
					((CProtagonist*)(curr->GetActor()))->UpdateFailed(deltaSec);
					curr = curr->GetNextActor();
				}
/*
 *				Discontinue update attempts for this frame.
 */
				return;
			}
/*
 *			Set a new irreconcilable or no discontinutiy time and compute a new
 *			update end time between them. Loop to attempt to reach this new time.
 */
			if (result == IRRECONCILABLE)
				irreconcilableTime = updateEndTime;
			else
				noDiscontinuityTime = updateEndTime;
			updateEndTime = 0.5f * (noDiscontinuityTime + irreconcilableTime);
		}
/*
 *		Tell each actor to end the update.
 */
		curr = d_actorHead;
		while (curr)
		{
			((CProtagonist*)(curr->GetActor()))->EndUpdate(deltaSec);
			curr = curr->GetNextActor();
		}
/*
 *		Loop to try the next update.
 */
	} while (updateEndTime < frameEndTime);
}

/***************************************************************************/
/***************************************************************************/
CCluster::ECollisionResult CCluster::CollisionDetect(CCollisionList* &o_list)
{
/*
 *	Reset the stack and get a collision list.
 */
	g_collisionSystem.ResetStack();
	o_list = CCollisionList::New(&g_collisionSystem.GetStack());
	if (o_list == NULL)
		return(NO_DISCONTINUITY);
/*
 *	Setup some internal casts.
 *
 *	The members of the cluster that do not displace other members of the cluster
 *	as a result of a collision between them.
 */
	CCast impotentCast(d_castMemberPool);
/*
 *	The members of the cluster that are not impotent and that are not displaced
 *	as a result of collisions with other members of the cluster.
 */
	CCast juggernautCast(d_castMemberPool);
/*
 *	Casts used for normal members of the cluster that is members that are neither
 *	juggernauts or impotent.
 */
	CCast pendingCast(d_castMemberPool);
	CCast processedCast(d_castMemberPool);
	CCast displacedCast(d_castMemberPool);
	CCast stationaryCast(d_castMemberPool);
/*
 *	Put each of the members of the cluster in the appropriate internal cast.
 */
	CCastMember* curr = d_actorHead;
	while (curr)
	{
		CProtagonist* actor = (CProtagonist*)(curr->GetActor());
		uint hint = actor->GetInteractionHint();
		if ((hint & CProtagonist::HINT_IMPOTENT) != 0)
			impotentCast.AddActor(actor);
		else if ((hint & CProtagonist::HINT_JUGGERNAUT) != 0)
			juggernautCast.AddActor(actor);
		else
			pendingCast.AddActor(actor);
		curr = curr->GetNextActor();
	}
/*
 *	Declare and initialize a variable that records if a displacement has occurred
 *	and if a detected collision will result in a discontinuity.
 */
	uint effect = CProtagonist::EFFECT_NONE;
/*
 *	Collide the members of the juggernaut cast against the world and each other.
 *	Call CollisionDetect to determine if a detected collision will be
 *	irreconcilable for some reason or if the collision will result in a
 *	discontinuity.
 */
	curr = NULL;
	while ((curr = juggernautCast.GetNextMember(curr)) != NULL)
	{
		effect |= CollideJuggernaut(o_list, (CProtagonist*)(curr->GetActor()),
				&juggernautCast, curr);
		ASSERT((effect & CProtagonist::EFFECT_DISPLACED) == 0);
		if ((effect & CProtagonist::EFFECT_IRRECONCILABLE) != 0)
			return(IRRECONCILABLE);
	}
/*
 *	Collide the normal members of the cluster. These members may displace in
 *	response to a collision which may result in new intersections. Keep trying
 *	to collide until everyone is "clean" or until a fixed number of attempts
 *	are made.
 */
	int i;
	for (i = 0; i < MAX_DISPLACE_ATTEMPTS; i++)
	{
/*
 *		The pending cast is the list of members that need to be tested for
 *		collisions and that have not displaced during the current pass. Use
 *		each member of the pending cast as a query against each of the normal
 *		cluster members that have yet to be used as a query.
 */
		curr = pendingCast.GetNextMember(NULL);
		while (curr)
		{
/*
 *			Get the actor and check it for collisions with the world and
 *			juggernaut members.
 */
			CProtagonist* actor = (CProtagonist*)(curr->GetActor());
			effect &= ~CProtagonist::EFFECT_DISPLACED;
			effect |= CollideJuggernaut(o_list, actor, &juggernautCast, NULL);
			if ((effect & CProtagonist::EFFECT_IRRECONCILABLE) != 0)
				return(IRRECONCILABLE);
/*
 *			The displaced cast contains members that have been targets of a
 *			collision check and that have displaced as a result. Check each of
 *			them first.
 */
			effect |= CollideNormal(o_list, actor, &displacedCast, NULL);
			if ((effect & CProtagonist::EFFECT_IRRECONCILABLE) != 0)
				return(IRRECONCILABLE);
/*
 *			The stationary cast contains members that have been through the
 *			last pass without being moved. Check each of them and move them to
 *			the displaced cast if they move this time.
 */
			effect |= CollideNormal(o_list, actor, &stationaryCast, NULL,
					&displacedCast);
			if ((effect & CProtagonist::EFFECT_IRRECONCILABLE) != 0)
				return(IRRECONCILABLE);
/*
 *			Test the subsequent members of the pending cast and move them to the
 *			displaced cast if they move.
 */
			effect |= CollideNormal(o_list, actor, &pendingCast, curr,
					&displacedCast);
			if ((effect & CProtagonist::EFFECT_IRRECONCILABLE) != 0)
				return(IRRECONCILABLE);
/*
 *			If the current query member displaced in response to any collision
 *			during this pass, leave it on the pending cast to be checked again
 *			in the next pass. If not, move it to the processed cast. The processed
 *			cast contains members that have made it through the current pass
 *			without being displaced.
 */
			if ((effect & CProtagonist::EFFECT_DISPLACED) != 0)
				curr = curr->GetNextActor();
			else
			{
				CCastMember* test = curr->GetNextActor();
				curr->ChangeCast(&processedCast);
				curr = test;
			}
		}
/*
 *		At this point, the displaced cast contains members that have been targets
 *		of a collision check this frame and have moved as a result. Now use each
 *		one as a query.
 */
		while ((curr = displacedCast.GetNextMember(NULL)) != NULL)
		{
/*
 *			Get the actor and check it for collisions with the world and
 *			juggernaut members.
 */
			CProtagonist* actor = (CProtagonist*)(curr->GetActor());
			effect |= CollideJuggernaut(o_list, actor, &juggernautCast, NULL);
			if ((effect & CProtagonist::EFFECT_IRRECONCILABLE) != 0)
				return(IRRECONCILABLE);
/*
 *			Test subsequent members of the displaced cast.
 */
			effect |= CollideNormal(o_list, actor, &displacedCast, curr);
			if ((effect & CProtagonist::EFFECT_IRRECONCILABLE) != 0)
				return(IRRECONCILABLE);
/*
 *			Test the stationary cast and move them to the displaced cast if they
 *			displace.
 */
			effect |= CollideNormal(o_list, actor, &stationaryCast, NULL,
					&displacedCast);
			if ((effect & CProtagonist::EFFECT_IRRECONCILABLE) != 0)
				return(IRRECONCILABLE);
/*
 *			Since the current query is known to have moved this pass, move it to
 *			the pending cast so it can be checked again next pass.
 */
			curr->ChangeCast(&pendingCast);
		}
/*
 *		If this point is reached and the pending cast is empty, no collisions
 *		were detected and checks on the normal members can be stopped.
 */
		if (pendingCast.GetNextMember(NULL) == NULL)
			break;
/*
 *		At this point, the displaced cast is empty and the pending cast contains
 *		all the members that need to be checked again. The processed cast contains
 *		members that did not displace this pass. Move them to the stationary cast
 *		for the next pass.
 */
		while ((curr = processedCast.GetNextMember(NULL)) != NULL)
			curr->ChangeCast(&stationaryCast);
	}
/*
 *	If the normal cast members could not be displaced to a "legal" position,
 *	make one last check against juggernauts to attempt to get them in the most
 *	acceptable position (i.e., don't let them fall through the ground.
 */
	if (i >= MAX_DISPLACE_ATTEMPTS)
	{
		curr = NULL;
		while ((curr = pendingCast.GetNextMember(curr)) != NULL)
		{
			effect |= CollideJuggernaut(o_list, (CProtagonist*)(curr->GetActor()),
					&juggernautCast, NULL);
			if ((effect & CProtagonist::EFFECT_IRRECONCILABLE) != 0)
				return(IRRECONCILABLE);
		}
	}
/*
 *	All of the normal members have been tested and have been moved into their
 *	new positions. The only members of the cluster that still need to be tested
 *	are impotent members. Since the normal members behave as juggernauts to the
 *	impotent members, move the normal members to the juggernaut cast. They
 *	could be in the processed, pending, or stationary casts but the displaced
 *	cast will be empty at this point.
 */
	while ((curr = processedCast.GetNextMember(NULL)) != NULL)
		curr->ChangeCast(&juggernautCast);
	while ((curr = pendingCast.GetNextMember(NULL)) != NULL)
		curr->ChangeCast(&juggernautCast);
	while ((curr = stationaryCast.GetNextMember(NULL)) != NULL)
		curr->ChangeCast(&juggernautCast);
/*
 *	Test each impotent member. Since they cannot displace each other, they
 *	only need to be checked against the juggernaut members.
 */
	curr = NULL;
	while ((curr = impotentCast.GetNextMember(curr)) != NULL)
	{
		CProtagonist* actor = (CProtagonist*)(curr->GetActor());
		for (i = 0; i < MAX_DISPLACE_ATTEMPTS; i++)
		{
			effect &= ~CProtagonist::EFFECT_DISPLACED;
			effect |= CollideJuggernaut(o_list, actor, &juggernautCast, NULL);
			if ((effect & CProtagonist::EFFECT_IRRECONCILABLE) != 0)
				return(IRRECONCILABLE);
			if ((effect & CProtagonist::EFFECT_DISPLACED) == 0)
				break;
		}
	}
/*
 *	Collide the members of the impotent cast against the each other.
 */
	curr = NULL;
	while ((curr = impotentCast.GetNextMember(curr)) != NULL)
	{
		effect |= CollideImpotent(o_list, (CProtagonist*)(curr->GetActor()),
				&impotentCast, curr);
		if ((effect & CProtagonist::EFFECT_IRRECONCILABLE) != 0)
			return(IRRECONCILABLE);
	}
/*
 *	Return.
 */
	if (o_list->Empty())
	{
		CCollisionStackItem::Delete(o_list);
		o_list = NULL;
		return(NO_DISCONTINUITY);
	}
	return(((effect & CProtagonist::EFFECT_DISCONTINUITY) != 0) ?
			FOUND_DISCONTINUITY : NO_DISCONTINUITY);
}

/***************************************************************************/
/***************************************************************************/
uint CCluster::CollideJuggernaut(CCollisionList* &o_list,
		CProtagonist* i_actor, CCast* i_cast, CCastMember *i_member)
{
/*
 *	Setup
 */
	CCollisionEvent* event;
	CCollisionStackLock lock;
	CStageEntity* entity = (CStageEntity*)i_actor;
	uint effect = CProtagonist::EFFECT_NONE;
/*
 *	Detect juggernaut actors. Do them first because they are the least offensive
 *	if actors fail to displace to acceptable positions.
 */
	while ((i_member = i_cast->GetNextMember(i_member)) != NULL)
	{
		CProtagonist* testActor = (CProtagonist*)(i_member->GetActor());
		CStageEntity* testEntity = (CStageEntity*)testActor;
		event = CCollisionEvent::TestEntityPair(entity, testEntity,
				&g_collisionSystem.GetStack());
		if (event)
		{
			lock.Set(g_collisionSystem.GetStack());
			SET_LOCK_NAME(lock, "CollideJuggernaut");
			uint queryEffect = i_actor->CollisionDetect(testEntity, event, false);
			effect |= queryEffect;
			uint targetEffect = testActor->CollisionDetect(entity, event,
					(queryEffect & CProtagonist::EFFECT_DISPLACED) != 0);
			g_collisionSystem.ResetStack();
			lock.Release();
			ASSERT((targetEffect & CProtagonist::EFFECT_DISPLACED) == 0);
			effect |= targetEffect;
			if ((effect & CProtagonist::EFFECT_IRRECONCILABLE) != 0)
				return(effect);
			o_list->AddEvent(event, true);
		}
	}
/*
 *	Detect the static entity. This is the next the least offensive.
 */
	event = CCollisionEvent::TestEntityPair(entity,
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
			SET_LOCK_NAME(lock, "CollideJuggernaut");
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
 *	Detect terrain. Make sure an actor does not fall through terrain.
 */
	event = CCollisionEvent::TestEntityPair(entity,
			g_collisionSystem.TerrainEntity(), &g_collisionSystem.GetStack());
	if (event)
	{
		lock.Set(g_collisionSystem.GetStack());
		SET_LOCK_NAME(lock, "CollideJuggernaut");
		effect |= i_actor->CollisionDetect(g_collisionSystem.TerrainEntity(),
				event, false);
		g_collisionSystem.ResetStack();
		lock.Release();
		if ((effect & CProtagonist::EFFECT_IRRECONCILABLE) != 0)
			return(effect);
		o_list->AddEvent(event, true);
	}
/*
 *	Return
 */
	return(effect);
}

/***************************************************************************/
/***************************************************************************/
uint CCluster::CollideNormal(CCollisionList* &o_list,
		CProtagonist* i_actor, CCast* i_cast, CCastMember *i_member)
{
	CCollisionStackLock lock;
	CStageEntity* entity = (CStageEntity*)i_actor;
	uint result = CProtagonist::EFFECT_NONE;

	while ((i_member = i_cast->GetNextMember(i_member)) != NULL)
	{
		CProtagonist* testActor = (CProtagonist*)(i_member->GetActor());
		CStageEntity* testEntity = (CStageEntity*)testActor;
		CCollisionEvent* event = CCollisionEvent::TestEntityPair(entity,
				testEntity, &g_collisionSystem.GetStack());
		if (event)
		{
			uint effect, testEffect;
			lock.Set(g_collisionSystem.GetStack());
			SET_LOCK_NAME(lock, "CollideNormal");
			if (!i_actor->CoupledCollisionDetect(testActor, event, effect,
					testEffect) && !testActor->CoupledCollisionDetect(i_actor,
					event, testEffect, effect))
			{
				effect = i_actor->CollisionDetect(testEntity, event, false);
				testEffect = testActor->CollisionDetect(entity, event,
						(effect & CProtagonist::EFFECT_DISPLACED) != 0);
			}
			g_collisionSystem.ResetStack();
			lock.Release();
			result |= effect | (testEffect & ~CProtagonist::EFFECT_DISPLACED);
			if ((result & CProtagonist::EFFECT_IRRECONCILABLE) != 0)
				return(result);

			o_list->AddEvent(event, true);
		}
	}

	return(result);
}

/***************************************************************************/
/***************************************************************************/
uint CCluster::CollideNormal(CCollisionList* &o_list,
		CProtagonist* i_actor, CCast* i_cast, CCastMember *i_member,
		CCast* i_displacedCast)
{
	CCollisionStackLock lock;
	CStageEntity* entity = (CStageEntity*)i_actor;
	uint result = CProtagonist::EFFECT_NONE;

	i_member = i_cast->GetNextMember(i_member);
	while (i_member)
	{
		CCastMember* next = i_cast->GetNextMember(i_member);
		CProtagonist* testActor = (CProtagonist*)(i_member->GetActor());
		CStageEntity* testEntity = (CStageEntity*)testActor;
		CCollisionEvent* event = CCollisionEvent::TestEntityPair(entity,
				testEntity, &g_collisionSystem.GetStack());
		if (event)
		{
			uint effect, testEffect;
			lock.Set(g_collisionSystem.GetStack());
			SET_LOCK_NAME(lock, "CollideNormal");
			if (!i_actor->CoupledCollisionDetect(testActor, event, effect,
					testEffect) && !testActor->CoupledCollisionDetect(i_actor,
					event, testEffect, effect))
			{
				effect = i_actor->CollisionDetect(testEntity, event, false);
				testEffect = testActor->CollisionDetect(entity, event,
						(effect & CProtagonist::EFFECT_DISPLACED) != 0);
			}
			g_collisionSystem.ResetStack();
			lock.Release();
			if ((testEffect & CProtagonist::EFFECT_DISPLACED) != 0)
				i_member->ChangeCast(i_displacedCast);
			result |= effect | (testEffect & ~CProtagonist::EFFECT_DISPLACED);
			if ((result & CProtagonist::EFFECT_IRRECONCILABLE) != 0)
				return(result);

			o_list->AddEvent(event, true);
		}
		i_member = next;
	}

	return(result);
}

/***************************************************************************/
/***************************************************************************/
uint CCluster::CollideImpotent(CCollisionList* &o_list,
		CProtagonist* i_actor, CCast* i_cast, CCastMember *i_member)
{
	CCollisionStackLock lock;
	CStageEntity* entity = (CStageEntity*)i_actor;
	uint effect = CProtagonist::EFFECT_NONE;

	while ((i_member = i_cast->GetNextMember(i_member)) != NULL)
	{
		CProtagonist* testActor = (CProtagonist*)(i_member->GetActor());
		CStageEntity* testEntity = (CStageEntity*)testActor;
		CCollisionEvent* event = CCollisionEvent::TestEntityPair(entity,
				testEntity, &g_collisionSystem.GetStack());
		if (event)
		{
			lock.Set(g_collisionSystem.GetStack());
			SET_LOCK_NAME(lock, "CollideNormal");
			effect |= i_actor->CollisionDetect(testEntity, event, false) |
					testActor->CollisionDetect(entity, event, false);
			g_collisionSystem.ResetStack();
			lock.Release();
			ASSERT((effect & CProtagonist::EFFECT_DISPLACED) == 0);
			if ((effect & CProtagonist::EFFECT_IRRECONCILABLE) != 0)
				return(effect);
			o_list->AddEvent(event, true);
		}
	}

	return(effect);
}

/***************************************************************************/
/***************************************************************************/
void CCluster::CollisionAccept(CCollisionList* i_list)
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
	CCastMember* curr = d_actorHead;
	while (curr)
	{
		CStageEntity* entityA = curr->GetActor();
		CProtagonist* actorA = (CProtagonist*)entityA;

		CCollisionEvent* event;
		if ((event = BuildOrderedEventList(i_list, entityA,
				g_collisionSystem.TerrainEntity())) != NULL)
			actorA->CollisionAccept(g_collisionSystem.TerrainEntity(), event);

		if ((event = BuildOrderedEventList(i_list, entityA,
				g_collisionSystem.StaticEntity())) != NULL)
			actorA->CollisionAccept(g_collisionSystem.StaticEntity(), event);

		CCastMember* mate = curr->GetNextActor();
		while (mate)
		{
			CStageEntity* entityB = mate->GetActor();
			if ((event = BuildEventList(i_list, entityA, entityB)) != NULL)
			{
				CProtagonist* actorB = (CProtagonist*)entityB;
				if (!actorA->CoupledCollisionAccept(actorB, event) &&
						!actorB->CoupledCollisionAccept(actorA, event))
				{
					actorA->CollisionAccept(entityB, event);
					actorB->CollisionAccept(entityA, event);
				}
				g_messageDispatcher.SendMessageToActor("ActorsCollided", NULL,
						actorB->GetHandle(), actorA->GetHandle());
				g_messageDispatcher.SendMessageToActor("ActorsCollided", NULL,
						actorA->GetHandle(), actorB->GetHandle());
			}
			mate = mate->GetNextActor();
		}
		curr = curr->GetNextActor();
	}
	lock.Release();
}

/***************************************************************************/
/***************************************************************************/
CCollisionEvent* CCluster::BuildEventList(CCollisionList* i_list,
		const CStageEntity* i_entityA, const CStageEntity* i_entityB)
{
	CCollisionEvent head;
	CCollisionEvent* tail = head.SetTempListNext(NULL);
	CCollisionEvent* event = i_list->GetHeadEvent();
	while (event)
	{
		if (((i_entityA == event->GetQueryEntity()) &&
				(i_entityB == event->GetTargetEntity())) ||
				((i_entityA == event->GetTargetEntity()) &&
				(i_entityB == event->GetQueryEntity())))
		{
			tail->SetTempListNext(event->SetTempListNext(NULL));
			tail = event;
		}
		event = event->GetNext();
	}
	return(head.GetTempListNext());
}

/***************************************************************************/
/***************************************************************************/
CCollisionEvent* CCluster::BuildOrderedEventList(CCollisionList* i_list,
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
