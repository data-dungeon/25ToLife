/*
**
**  File:   protagonist.h
**  Date:   March 28, 2000
**  By:     Bryant Collard
**  Desc:   A protagonist is an actor that interacts with other
**          protagonists in some direct, physical way.
**
**  Copyright (c) 2000, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/stage/protagonist.h $
**  $Revision: #106 $
**  $DateTime: 2005/06/08 16:04:21 $
**    $Author: Bryant_Collard $
**
*/

#ifndef CPROTAGONIST_H
#define CPROTAGONIST_H

#include "stage/cactor.h"

class ClusterElement;
class Weapon;
class CActorComponentList;
class CProtagonistNode;

class CProtagonist : public CActor
{
public:
	CProtagonist();
	CProtagonist(const CEntityType &i_type);
	virtual ~CProtagonist(void);

	// Handle
	static CProtagonist* FromHandle(ActorHandle i_handle);

	void SetCollisionNode(CProtagonistNode* i_collisionNode,
			ECollisionConfiguration i_collisionConfiguration,
			bool i_ownCollisionNode);

	virtual bool Initialize(void);

	virtual CCollisionNode* BeginCollisionTest(CStageEntity* i_entity,
			ESelect i_select);
	virtual bool GetCollisionCull(CCollisionCull &o_cull);
	virtual void RunAnimation(bool i_forceUpdate);

	virtual void Update(float i_deltaSec);
	virtual void BeginUpdate(void) {}
	virtual void AttemptUpdate(float i_deltaSec) {}
	virtual void EndUpdate(float i_deltaSec) {}
	virtual void UpdateFailed(float i_deltaSec) {}

	void SetUpdating(bool i_updating) {d_updating = i_updating;}
	bool IsUpdating(void) const {return(d_updating);}

	// Constants used in a bitfield to provide hints to aid updating and
	// colliding protagonists. See their definition for descriptions.
	static const uint HINT_IMPOTENT;
	static const uint HINT_JUGGERNAUT;
	static const uint HINT_TEST_PROCESSED;
	static const uint HINT_FIXED;
	static const uint HINT_STATIONARY;
	static const uint HINT_INACTIVE;

	// Constants used in a bitfield to describe the effect of a collision.
	static const uint EFFECT_NONE;
	static const uint EFFECT_DISPLACED;
	static const uint EFFECT_DISCONTINUITY;
	static const uint EFFECT_IRRECONCILABLE;
	static const uint EFFECT_DISPLACEMENT;
	static const uint EFFECT_VELOCITY_ONLY;

	// Get at the hint bitfield.
	void SetInteractionHint(uint Hint) { d_interactionHint = Hint; }
	uint GetInteractionHint(void) {return(d_interactionHint);}
	virtual uint GetTailoredInteractionHint(CProtagonist* i_actor)
		{return(d_interactionHint);}

	// Give an actor the opportunity to respond to a collision detection for
	// both participants.
	virtual bool CoupledCollisionDetect(CStageEntity* i_entity,
			CCollisionEvent* i_event, uint &o_myEffect, uint &o_otherEffect)
		{return(false);}

	// Respond to a collision detection. This is where you displace as the
	// result of a collision. The query entity is always called first. If the
	// query entity displaces, intersections in the event could be invalid.
	// The i_mateDisplaced alerts to that possibility.
	virtual uint CollisionDetect(CStageEntity* i_entity,
			CCollisionEvent* i_event, bool i_mateDisplaced)
		{return(EFFECT_NONE);}

	// Give an actor the opportunity to accept a collision for both participants.
	virtual bool CoupledCollisionAccept(CStageEntity* i_entity,
			CCollisionEvent* i_event)
		{return(false);}

	// Accept a collision.
	virtual void CollisionAccept(CStageEntity* i_entity,
			CCollisionEvent* i_event) {}

	static const CEntityType &ClassType(void) {return(s_protagonistClass);}
	virtual const char* GetClassName(void);

	virtual uint16 Color(CCollisionPair* i_pair);
	virtual uint8 Material(CCollisionPair* i_pair);
	virtual void Draw(CCollisionEvent* i_event);

	virtual CInterface* GetInterface(uint i_id);

	// move from protagonist to non-protagonist cast, and vice versa. this involves a number of sub-system updates,
	// so formalize it here. mostly used for environmental actors
	void MoveToProtagonistCast(void);
	void MoveToNonProtagonistCast(void);

	// Make me depend on another protagonist
	enum EDependOnResult
	{
		DEPEND_ON_PROPOSED,
		DEPEND_ON_NO_CHANGE,
		DEPEND_ON_NO_CLUSTER_ELEMENT,
		DEPEND_ON_NO_MASTER_CLUSTER_ELEMENT,
	};
	EDependOnResult DependOn(CProtagonist* i_master);
	CProtagonist *GetMaster();
	virtual void DependOnSucceeded(void) {}
	virtual void DependOnFailed(void) {}
	virtual void DependOnChanged(CProtagonist* i_newMaster) {}

	// stuff for the clustering system
	inline virtual void UpdateCollisionHull(float timeStep);
	inline const XZRect &CollisionHull(void) const
		{ return(d_collisionHull); }
	inline void SetClusterElement(ClusterElement *i_clusterElement)
		{ d_clusterElement = i_clusterElement; }
	inline ClusterElement* GetClusterElement(void)
		{ return(d_clusterElement); }
	virtual void MasterDeleteNotify(CProtagonist* i_mastersMaster) {}
	virtual void MasterStateChangeNotify(void) {}
	void ModifyMasterState(void);
	void SyncDependents(void);
	bool IsIndependent(void);
	bool IsNotInDependency(void);

	virtual void SyncState(void);
	virtual void SyncActor(void);

protected:

	uint d_interactionHint;
	bool d_overrideNumTicks;
	float d_overrideTickSec;

	CInteractionCategory d_chainExclude;

	// Collision stuff
	CProtagonistNode		*d_collisionNode;
	bool						d_ownCollisionNode;

	// our current collision hull
	XZRect					d_collisionHull;

	// our current cluster
	ClusterElement		*d_clusterElement;

private:
	bool d_updating;
	static const CEntityType s_protagonistClass;
	
public:
	const u32			GetLocalSaveGameMemoryNeeded				( void ) const;
	virtual const u32 GetSaveGameMemoryNeeded						( void );
	virtual const u32 SerializeSaveGame								( char *pBuf, bool bDirection );
};

#endif // CPROTAGONIST_H
