/*
**
**  File:   cast.h
**  Date:   March 21, 2000
**  By:     Bryant Collard
**  Desc:   Classes for managing a set of actors in a scene.
**
**  Copyright (c) 2000, Avalanche Software Inc.
**  All Rights Reserved.
**
*/

#ifndef CCAST_H
#define CCAST_H

#include "platform/BaseType.h"
#include "stage/castmember.h"

class CCastMemberPool;

class CCast
{
public:
	// constructor/destructor
	CCast(CCastMemberPool &i_castMemberPool);
	virtual ~CCast ();

	// Handle adding/removing/finding actors
	CCastMember* AddActor (CActor* a_actor);
	bool RemoveActor (CActor* a_actor);
	bool TransferActor (CActor* a_actor, CCast* a_newCast);
	void Clear (void);
	bool IsMember(CActor* i_actor) const;

	// The control/update/animation
	virtual void BeginFrame(void);
	virtual void Advance(void);
	virtual void EndFrame(void);
	virtual void RunAnimation(void);
	virtual void Propagate(float i_time);

	// Kill actors that want to die
	void GarbageCollect(void);

	// enumeration
	inline CCastMember* GetNextMember(CCastMember* i_curr) const;

	// Delete the this actor from all casts
	static void DeleteActor(CActor &actor);

	// Slow (takes a traversal)
	uint Members(void);

	// Debugging
	void ToggleHideActors(void);

	// Find actor by ID
	CActor* FindActorByID( u32 id );

	// Find actor by InstanceName
	CActor* FindActorByInstanceName( const char* name );

protected:
	// The main list
	CCastMember* d_actorHead;

	// A pool of cast members
	CCastMemberPool &d_castMemberPool;

private:
	CCast();

	friend class CCastMember;
};

// Inlines
#include "stage/cast.hpp"

#endif
