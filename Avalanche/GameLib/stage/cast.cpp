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

/*** pre-compiled headers through .h file (visual studio), can't be inside  ***/
/*** #if statement. see .h file for more info.for codewarrior, pre-compiled ***/
/*** header is handled through xcl compile/link interpreter, and doesn't    ***/
/*** need any explicit include                                              ***/

#include "stage/StagePCH.h"

#include "EngineHelper/timer.h"

/***************************************************************************/
/**************** CCast ****************/
/***************************************************************************/
CCast::CCast(CCastMemberPool &i_castMemberPool) :
	d_castMemberPool(i_castMemberPool)
{
	d_actorHead = 0;
}

/***************************************************************************/
/***************************************************************************/
CCast::~CCast ()
{
	while (d_actorHead)
	{
		CActor *actor = d_actorHead->GetActor ();
		d_castMemberPool.DeleteMember(d_actorHead);
		if (actor->ShouldDelete ())
			CActor::Destroy(actor);
	}
}

/***************************************************************************/
/***************************************************************************/
CCastMember *CCast::AddActor (CActor* a_actor)
{
#ifdef _DEBUG
	CCastMember* curr = d_actorHead;
	while ( curr )
	{
		if ( curr->GetActor() == a_actor )
			ASSERT(false);
		curr = curr->GetNextActor();
	}
#endif
	// Create it!
	CCastMember *castMember = d_castMemberPool.NewMember(a_actor, this);
	ASSERT(castMember != NULL);
	return(castMember);
}

/***************************************************************************/
/***************************************************************************/
bool CCast::RemoveActor (CActor* a_actor)
{
	CCastMember* curr = d_actorHead;
	while (curr)
	{
		if (a_actor == curr->GetActor ())
		{
			d_castMemberPool.DeleteMember(curr);
			if (a_actor->ShouldDelete ())
				CActor::Destroy(a_actor);
			return(true);
		}
		curr = curr->GetNextActor ();
	}
	return(false);
}

/***************************************************************************/
/***************************************************************************/
bool CCast::TransferActor (CActor* a_actor, CCast* a_newCast)
{
	CCastMember* curr = d_actorHead;
	while (curr)
	{
		if (a_actor == curr->GetActor ())
		{
			// Are they deleting the actor?
			if (a_newCast == NULL)
			{
				d_castMemberPool.DeleteMember(curr);
				if (a_actor->ShouldDelete ())
					CActor::Destroy(a_actor);
			}

			// Do the two casts not share the same cast member pool?
			else if (&(a_newCast->d_castMemberPool) != &d_castMemberPool)
			{
				d_castMemberPool.DeleteMember(curr);
				a_newCast->AddActor(a_actor);
			}

			// Transfer them
			else
			{
				curr->ChangeCast(a_newCast);
			}
			return(true);
		}
		curr = curr->GetNextActor ();
	}
	return(false);
}

/***************************************************************************/
/***************************************************************************/
void CCast::Clear (void)
{
	while (d_actorHead)
	{
		CActor* actor = d_actorHead->GetActor ();
		d_castMemberPool.DeleteMember(d_actorHead);
		if (actor->ShouldDelete())
			CActor::Destroy(actor);
	}
}

/***************************************************************************/
// Do the begin frame
/***************************************************************************/
void CCast::BeginFrame(void)
{
	CCastMember* curr = d_actorHead;
	while(curr)
	{
		CActor *actor = curr->GetActor();
		if ( actor->IsActive() )
		{
			actor->BeginFrame();
		}
		curr = curr->GetNextActor();
	}
}

/***************************************************************************/
// Run the update phase. This is the generic version, used for the
// non-protagonist cast that doesn't need to worry about interaction
/***************************************************************************/
void CCast::Advance(void)
{
	CActor* actor;

	CCastMember* curr = d_actorHead;
	while(curr)
	{
		actor = curr->GetActor();
		if ( actor->IsActive() )
		{
			float frameEndTime = g_timer.GetFrameSec();
			float maxDeltaSec = actor->GetMaxUpdateSec();
			float updateEndTime = 0.0f;
			do
			{
				float deltaSec = frameEndTime - updateEndTime;
				if (deltaSec > maxDeltaSec)
					deltaSec = maxDeltaSec;
#ifdef ENABLE_RECORD
				actor->ManageRecording(updateEndTime, deltaSec);
#endif
				actor->Update(deltaSec);
				updateEndTime += maxDeltaSec;
			} while (updateEndTime < frameEndTime);
		}
		curr = curr->GetNextActor();
	}
}

/***************************************************************************/
// Do the end frame
/***************************************************************************/
void CCast::EndFrame(void)
{
	CCastMember* curr = d_actorHead;
	while(curr)
	{
		CActor *actor = curr->GetActor();
		if ( actor->IsActive() )
		{
			actor->EndFrame();
		}
		curr = curr->GetNextActor();
	}
}

/***************************************************************************/
// Do the animation phase
/***************************************************************************/
void CCast::RunAnimation(void)
{
	CCastMember* curr = d_actorHead;
	while(curr)
	{
		CActor *actor = curr->GetActor();
		actor->RunAnimation(false);
		actor->UpdateGeomBoundingVolumes();
		curr = curr->GetNextActor();
	}
}

/***************************************************************************/
// Propagate state to the session
/***************************************************************************/
void CCast::Propagate(float i_time)
{
	CCastMember* curr = d_actorHead;
	while(curr)
	{
		CActor *actor = curr->GetActor();
		actor->PropagateNetActor(i_time);
		actor->ClearDiscontinuity();
		curr = curr->GetNextActor();
	}
}

/***************************************************************************/
// Kill actors that want to die
/***************************************************************************/
void CCast::GarbageCollect(void)
{
	CCastMember *curr = d_actorHead;
	while (curr)
	{
		// Get the actor
		CActor *killMe = curr->GetActor();

		// Advance to the next
		curr = curr->GetNextActor();

		// Do they want to die?
		if (killMe->MarkedForDeath())
			CActor::Destroy(killMe);
	}
}

/***************************************************************************/
// Delete the this actor from all casts
/***************************************************************************/
void CCast::DeleteActor(
CActor &actor)
{
	while (actor.CastHead())
	{
		(actor.CastHead())->d_cast->d_castMemberPool.DeleteMember(
				actor.CastHead());
	}
}

/***************************************************************************/
// Slow (takes a traversal)
/***************************************************************************/
uint CCast::Members(void)
{
	uint count = 0;
	CCastMember *curr = d_actorHead;
	while (curr)
	{
		++count;
		curr = curr->GetNextActor();
	}

	return(count);
}

/***************************************************************************/
// Debugging
/***************************************************************************/
void CCast::ToggleHideActors(void)
{
	CCastMember* curr = d_actorHead;
	while(curr)
	{
		CActor* actor = curr->GetActor();
		actor->Hide( !actor->Instance()->Hidden() );
		curr = curr->GetNextActor();
	}
}

/***************************************************************************/
// Find actor by ID
/***************************************************************************/
CActor* CCast::FindActorByID( u32 id )
{
	CCastMember* curr = d_actorHead;
	while ( curr )
	{
		if ( curr->GetActor()->getID() == id )
		{
			return curr->GetActor();
		}
		curr = curr->GetNextActor();
	}
	return NULL;
}

/***************************************************************************/
// Find actor by instance name
/***************************************************************************/
CActor* CCast::FindActorByInstanceName( const char* name )
{
	if ( name && name[0] && stricmp( name, "none" ) != 0 )
	{
		CCastMember* castMember;
		for ( castMember = d_actorHead ; castMember ; castMember = castMember->d_nextActor )
		{
			CActor* actor = castMember->GetActor();
			const char* actorInstanceName = actor->InstanceName();

			if ( actor && actor->InstanceName() && stricmp( actor->InstanceName(), name ) == 0 )
				return actor;
		}
	}
	return NULL;
}
