/*
**
**  File:   cast.cpp
**  Date:   March 21, 2000
**  By:     Bryant Collard
**  Desc:   Classes for managing a set of actors in a scene.
**
**  Copyright (c) 2000, Avalanche Software Inc.
**  All Rights Reserved.
**
*/

/******************* includes ****************************/

#include "stage/StagePCH.h"





/***************************************************************************/
/**************** CCastMember ****************/
/***************************************************************************/
CCastMember::CCastMember (CActor* a_actor, CCast* a_cast)
{
	ASSERT((a_actor != NULL) && (a_cast != NULL));

	// Add cast to actor.
	d_actor = a_actor;
	d_prevCast = 0;
	d_nextCast = d_actor->d_castHead;
	if (d_nextCast)
		d_nextCast->d_prevCast = this;
	d_actor->d_castHead = this;

	// Add actor to cast.
	d_cast = a_cast;
	d_prevActor = 0;
	d_nextActor = d_cast->d_actorHead;
	if (d_nextActor)
		d_nextActor->d_prevActor = this;
	d_cast->d_actorHead = this;
}

/***************************************************************************/
/***************************************************************************/
CCastMember::~CCastMember ()
{
	// Clean up the actor's list of casts.
	if (d_prevCast)
		d_prevCast->d_nextCast = d_nextCast;
	else
		d_actor->d_castHead = d_nextCast;
	if (d_nextCast)
		d_nextCast->d_prevCast = d_prevCast;

	// Clean up the cast's list of actors.
	if (d_prevActor)
		d_prevActor->d_nextActor = d_nextActor;
	else
		d_cast->d_actorHead = d_nextActor;

	if (d_nextActor)
		d_nextActor->d_prevActor = d_prevActor;
}

/***************************************************************************/
/***************************************************************************/
void CCastMember::ChangeCast (CCast* a_cast, CCastMember* a_prevActor)
{
	ASSERT(a_cast != NULL);

	// Clean up the actor's list of casts.
	if (d_prevCast)
		d_prevCast->d_nextCast = d_nextCast;
	else
		d_actor->d_castHead = d_nextCast;
	if (d_nextCast)
		d_nextCast->d_prevCast = d_prevCast;

	// Clean up the cast's list of actors.
	if (d_prevActor)
		d_prevActor->d_nextActor = d_nextActor;
	else
		d_cast->d_actorHead = d_nextActor;
	if (d_nextActor)
		d_nextActor->d_prevActor = d_prevActor;

	// Add cast to actor.
	d_prevCast = 0;
	d_nextCast = d_actor->d_castHead;
	if (d_nextCast)
		d_nextCast->d_prevCast = this;
	d_actor->d_castHead = this;

	// Add actor to cast.
	d_cast = a_cast;
	d_prevActor = a_prevActor;
	if (!d_prevActor)
	{
		d_nextActor = d_cast->d_actorHead;
		d_cast->d_actorHead = this;
	}
	else
	{
		ASSERT(d_prevActor->d_cast == d_cast);
		d_nextActor = d_prevActor->d_nextActor;
		d_prevActor->d_nextActor = this;
	}
	if (d_nextActor)
		d_nextActor->d_prevActor = this;
}

/***************************************************************************/
/***************************************************************************/
void CCastMember::ChangeOrder (CCastMember* a_prevActor)
{
	// Take it out ot its old place
	if (d_prevActor)
		d_prevActor->d_nextActor = d_nextActor;
	else
		d_cast->d_actorHead = d_nextActor;
	if (d_nextActor)
		d_nextActor->d_prevActor = d_prevActor;

	// Insert it into its new place
	d_prevActor = a_prevActor;
	if (!d_prevActor)
	{
		d_nextActor = d_cast->d_actorHead;
		d_cast->d_actorHead = this;
	}
	else
	{
		d_nextActor = d_prevActor->d_nextActor;
		d_prevActor->d_nextActor = this;
	}
	if (d_nextActor)
		d_nextActor->d_prevActor = this;
}

