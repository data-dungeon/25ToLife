/*
**
**  File:   SightEntity.h
**  Date:   July 26, 2004
**  By:     Bryant Collard
**  Desc:   A query entity that performs sight checks. It has a sense of
**          range and field of view. It also can perform partial visibility
**          checks.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/CollideHelper/SightEntity.h $
**  $Revision: #5 $
**  $DateTime: 2005/06/08 16:04:21 $
**    $Author: Bryant_Collard $
**
*/

#ifndef SIGHT_ENTITY_H
#define SIGHT_ENTITY_H

#include "platform/BaseType.h"
#include "collide/QueryEntity.h"
#include "collide/Node.h"
#include "collide/obb.h"
#include "collide/Sphere.h"
#include "Math/Vector.h"

class CActor;
class CCollisionEvent;

//////////////////////////////// CObscureEntity ////////////////////////////////

class CObscureEntity : public CQueryEntity
{
  public:
	CObscureEntity(uint &i_config, Vector3 &i_locationInWorld, CActor* &i_actor);

	virtual bool GetCollisionCull(CCollisionCull &o_cull);
	virtual CCollisionNode* BeginCollisionTest(CStageEntity* i_entity,
			ESelect i_select);

	bool Test(CCollisionEvent* &io_event);

  private:
	enum EObjectTestResult
	{
		NO_OBJECT,
		INVALID_OBJECT,
		OBJECT_OBSCURED,
		OBJECT_VISIBLE
	};
	void BuildObb(CActor* i_target);
	void TestNode(CCollisionNode* i_node, CCollisionList* i_list,
			int &io_numObjects, int &io_numVisible, Vector3 &io_centroid,
			CCollisionPair* &io_pair);
	EObjectTestResult TestObject(CCollisionObject* i_object,
			CCollisionList* i_list, bool i_noVisibleFound, Vector3 &o_location,
			CCollisionPair* &o_pair);

	// References to useful variable stored elsewhere.
	uint &d_config;
	Vector3 &d_locationInWorld;
	CActor* &d_actor;

	// The test obb.
	OBBInstance d_obb;

	// Target entity to ignore.
	CStageEntity* d_target;
};

////////////////////////////////// CSightNode //////////////////////////////////

class CSightNode : public CCollisionNode
{
  public:
	// Construct
	CSightNode();

	// Setup.
	void SetRange(float i_range);
	void SetFov(float i_fov);
	void SetUnitDirection(Vector3CRef i_directionInWorld);

	// Access
	CSphereCollisionObject &Sphere(void) {return(d_sphere);}
	float Range(void) {return(d_sphere.Radius());}
	float Fov(void) {return(d_fov);}
	Vector3 &Location(void) {return(d_sphere.Center());}
	Vector3 &Direction(void) {return(d_directionInWorld);}

	// Override CCollisionNode
	virtual CCollisionObject* BeginObjectTest(void) {return(&d_sphere);}
	virtual bool IgnoreIntersection(CCollisionIntersection* i_intersection);

  private:
	// See if a point is in the FOV.
	bool TestFov(Vector3CRef i_targetLocation);

	// Collision object
	CSphereCollisionObject d_sphere;

	// FOV
	float d_fov;
	float d_cosFovSquared;
	Vector3 d_directionInWorld;
};

///////////////////////////////// CSightEvent //////////////////////////////////

class CSightEvent : public CCollisionEvent
{
  public:
	// Construct
	CSightEvent();

	// Allocate
	static inline CSightEvent* New(CCollisionStack* i_stack);

	// Set
	void SetPercentVisible(float i_pctVisible) {d_pctVisible = i_pctVisible;}
	void SetCentroid(Vector3CRef i_centroid) {d_centroid = i_centroid;}

	// Access
	float GetPercentVisible(void) {return(d_pctVisible);}
	Vector3CRef GetCentroid(void) {return(d_centroid);}

  private:
	// Construct
	CSightEvent(CCollisionStack &i_stack);

	// Data
	float d_pctVisible;
	Vector3 d_centroid;
};

///////////////////////////////// CSightEntity /////////////////////////////////

class CSightEntity : public CQueryEntity
{
  public:
	// Configuration bit masks.

	// Don't perform any extra processing.
	static const uint TEST_ONLY;

	// Estimate the percentage of the target that is visible and the centroid of
	// the visible portion. If set, the returned events will be CSightEvents
	// instead of CCollisionEvents. It is the user's responsibility to know that
	// the returned events can be down cast.
	static const uint DESCRIBE_VISIBLE;

	// Perform a line intersection with any of the visible collision objects.
	static const uint INTERSECT_ANY;

	// Perform a line intersection with all of the visible collision objects.
	static const uint INTERSECT_ALL;

	// Construction.
	CSightEntity();

	// Get at obscure entity. The user should set the Positive/Negative
	// Include/Exclude fields and ignore list to control what obscures.
	CObscureEntity &Obscure(void) {return(d_obscure);}

	// Setup.
	void SetConfiguration(uint i_config);
	void SetRange(float i_range);
	void SetFov(float i_fov);
	void SetVolume(float i_range, float i_fov);
	void SetLocation(Vector3CRef i_locationInWorld);
	void SetUnitDirection(Vector3CRef i_directionInWorld);
	void SetDirection(Vector3CRef i_directionInWorld);
	void SetActor(CActor* i_actor);
	void SetActor(CActor* i_actor, Vector3CRef i_locationInActor,
			Vector3CRef i_directionInActor);

	// Update.
	void SyncWithActor(void);

	// Access.
	uint GetConfiguration(void) {return(d_config);}
	float Range(void) {return(d_sightNode.Sphere().Radius());}
	float Fov(void) {return(d_sightNode.Fov());}
	Vector3 &Location(void) {return(d_sightNode.Sphere().Center());}
	Vector3 &Direction(void) {return(d_sightNode.Direction());}
	CActor* Actor(void) {return(d_actor);}

	// Queries.
	CCollisionEvent* Test(CStageEntity* i_target);
	CCollisionList* Test(void);

	// Override CStageEntity
	virtual bool GetCollisionCull(CCollisionCull &o_cull);
	virtual CCollisionNode* BeginCollisionTest(CStageEntity* i_entity,
			ESelect i_select);
	virtual const char* GetClassName(void) {return("Sight");}

  private:
	// Entity used to test of obscuration.
	CObscureEntity d_obscure;

	// Configuration bits.
	uint d_config;

	// Entity state.
	CActor* d_actor;
	Vector3 d_locationInActor;
	Vector3 d_directionInActor;

	// Collision node.
	CSightNode d_sightNode;
};

#define SIGHT_ENTITY_HPP
#include "CollideHelper/SightEntity.hpp"

#endif // SIGHT_ENTITY_H
