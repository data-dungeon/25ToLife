/*
**
**  File:   SightEntity.cpp
**  Date:   July 26, 2004
**  By:     Bryant Collard
**  Desc:   A query entity that performs sight checks. It has a sense of
**          range and field of view. It also can perform partial visibility
**          checks.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/CollideHelper/SightEntity.cpp $
**  $Revision: #13 $
**  $DateTime: 2005/06/08 16:04:21 $
**    $Author: Bryant_Collard $
**
*/

// Pre-compiled header
#include "CollideHelper/CollideHelperPCH.h"

// My header (always first)
#include "CollideHelper/SightEntity.h"

// Engine includes
#include "Game/Misc/Bounds.h"

// Gamelib includes
#include "collide/Event.h"
#include "collide/List.h"
#include "collide/SphereToTerrain.h"
#include "collide/SphereToObb.h"
#include "collide/SphereToCapsule.h"
#include "collide/TriangleList.h"
#include "CollideHelper/LineEntity.h"
#include "stage/cactor.h"
#include "Interfaces/CollisionIF.h"
#include "geomUtil/BoxExtents.h"

//#define DRAW_DEBUG

// Configuration bit masks.
const uint CSightEntity::TEST_ONLY        = 0x0;
const uint CSightEntity::DESCRIBE_VISIBLE = 0x1;
const uint CSightEntity::INTERSECT_ANY    = 0x2;
const uint CSightEntity::INTERSECT_ALL    = 0x4;

//////////////////////////////// CObscureEntity ////////////////////////////////

/******************************************************************************/
/******************************************************************************/
bool CObscureEntity::GetCollisionCull(CCollisionCull &o_cull)
{
	d_obb.ComputeWorldExtents(o_cull.SelectAABB());
	return(true);
}

/******************************************************************************/
/******************************************************************************/
CCollisionNode* CObscureEntity::BeginCollisionTest(CStageEntity* i_entity,
		ESelect i_select)
{
	// Make sure to ignore the actor and target.
	if ((i_entity == d_actor) || (i_entity == d_target))
		return(NULL);

	return(CQueryEntity::BeginCollisionTest(i_entity, i_select));
}

/******************************************************************************/
/******************************************************************************/
bool CObscureEntity::Test(CCollisionEvent* &io_event)
{
	// Save the target entity.
	d_target = io_event->GetTargetEntity();

	// For now, non-actors can never be seen. (Need to figure out how to deal
	// with configuration if such a test is desired.)
	if (!d_target->IsA(CActor::ClassType()))
		return(false);

	// Setup the obb.
	BuildObb((CActor*)d_target);

	// Find potentially obscuring objects.
	CCollisionList* list = g_collisionSystem.Test(this);

	// Setup for object tests.
	int numObjects = 0;
	int numVisible = 0;
	Vector3 centroid;
	CCollisionPair* pair = NULL;

	// The target has a simple configuration.
	if (d_target->ComplexBit(USE_COMPLEX) == 0)
	{
		switch(TestObject(d_target->GetCollisionObject(), list, true, centroid,
				pair))
		{
		 case NO_OBJECT:
		 case INVALID_OBJECT:
			break;

		 case OBJECT_OBSCURED:
			numObjects = 1;
			break;

		 case OBJECT_VISIBLE:
			numObjects = 1;
			numVisible = 1;
			break;
		}
	}

	// The target has a complex configuration.
	else
	{
		centroid.Clear();
		TestNode(d_target->BeginCollisionTest(io_event->GetQueryEntity(),
				USE_COMPLEX), list, numObjects, numVisible, centroid, pair);
		d_target->EndCollisionTest(NULL);
	}

	// The target is obscured.
	if (numVisible == 0)
		return(false);

	// Create a CSightEvent if needed to report results.
	if ((d_config & CSightEntity::DESCRIBE_VISIBLE) != 0)
	{
		// Allocate a sight event. Appear as if the target is obscured if
		// allocation fails.
		CSightEvent* sightEvent = CSightEvent::New(&g_collisionSystem.GetStack());
		if (sightEvent == NULL)
		{
			while(pair != NULL)
			{
				CCollisionPair* curr = pair;
				pair = pair->GetNext();
				CCollisionStackItem::Delete(curr);
			}
			return(false);
		}

		// Copy the entities from the source event to the sight event.
		sightEvent->SetEntities(io_event->GetQueryEntity(),
				io_event->GetTargetEntity());

		// If intersections were not performed, move all the source event's pairs
		// to the sight event.
		if (pair == NULL)
		{
			sightEvent->GetHeadPair() = io_event->GetHeadPair()->MoveAllToEvent(
					sightEvent, io_event->GetHeadPair(), sightEvent->GetHeadPair());
		}

		// Remove the source event from its list (deleteing the event along
		// with any remaining pairs) and add the sight event to the list.
		list = io_event->GetList();
		if ((list != NULL) && list->RemoveEvent(io_event))
			list->AddEvent(sightEvent);
		else
			CCollisionStackItem::Delete(io_event);

		// Now return the sight event.
		io_event = sightEvent;

		// Fill in the description of the visible portion.
		float invNumObjects = 1.0f / (float)numObjects;
		float invNumVisible = 1.0f / (float)numVisible;							//added this...brian
		sightEvent->SetPercentVisible(invNumObjects * (float)numVisible);
		sightEvent->SetCentroid(invNumVisible * centroid);						//modified this from
		//sightEvent->SetCentroid(invNumObjects * centroid);					//original...brian
	}

	// If intersections were performed, replace the pairs currently in the event
	// with the pairs describing the intersections.
	if (pair != NULL)
	{
		CCollisionPair* &head = io_event->GetHeadPair();
		while (head != NULL)
		{
			CCollisionPair* curr = head;
			head = head->GetNext();
			CCollisionStackItem::Delete(curr);
		}
		io_event->GetHeadPair() = pair->MoveAllToEvent(io_event, pair,
					io_event->GetHeadPair());
	}

	// The target is visible.
	return(true);
}

/******************************************************************************/
/******************************************************************************/
void CObscureEntity::BuildObb(CActor* i_target)
{
	// Find the orientation of the target's bounding box, its center, and size.
	DirCos3x3 targetToWorld;
	Vector3 targetLocation, targetHalfSize;
	COrientedCollisionIF* collisionIF = (COrientedCollisionIF*)
			i_target->GetInterface(COrientedCollisionIF::GetID());
	if (collisionIF != NULL)
	{
		targetToWorld = collisionIF->GetCollisionToWorld();
		targetLocation = collisionIF->GetCollisionInWorld();
		targetHalfSize.Set(0.5f * collisionIF->GetXSize(),
				0.5f * collisionIF->GetYSize(), 0.5f * collisionIF->GetZSize());
	}
	else
	{
		ts_BoundingBox &box = *i_target->Instance()->ModelBoundingBox();
		targetToWorld = i_target->GetBodyToWorld();
		targetHalfSize = 0.5f * (box.Max - box.Min);
		targetLocation = i_target->GetBodyInWorld() +
				targetHalfSize * targetToWorld;
	}

	// Orient the obb so its z-axis lies on the line from the sight location to
	// the center of the target.
	d_obb.Orientation().BuildFromAxis(targetLocation - d_locationInWorld, 2, 1);

	// Find the extents of the target bounding box in obb coordinates relative
	// to the sight location.
	Vector3 minExtent, maxExtent;
	CBoxExtents boxExtents;
	boxExtents.SetTargetToRef(d_locationInWorld, d_obb.Orientation());
	boxExtents.FindExtentsInTarget(targetHalfSize, targetLocation, targetToWorld,
			minExtent, maxExtent);

	// Imagine a line from the sight location to the center of the target. The x-
	// and y-coordinates of maxExtent are the perpendicular distances from that
	// line to the corners of the target box farthest from the line. The
	// z-coordinate is the distance from the sight location to the corner of the
	// target box farthest from the sight location. The goal is to build an obb
	// that contains the target box and the sight location. The x- and
	// y-coordinates of maxExtent are the needed half sizes of the desired box
	// but the z-coordinate is the full size. Make it the half size and set the
	// obb.
	maxExtent.Z(0.5f * maxExtent.Z());
	d_obb.GetAABB().Set(maxExtent.Negated(), maxExtent);
	d_obb.UpdateRadius();

	// Position the obb so the sight location is at the center of the -z face and
	// the positive z face lies just beyond the target bounding box.
	d_obb.Position() = d_locationInWorld +
			maxExtent.Z() * d_obb.Orientation().GetRow2();

#ifdef DRAW_DEBUG
	d_obb.Draw(DrawUtility::WHITE);
#endif
}

/******************************************************************************/
/******************************************************************************/
void CObscureEntity::TestNode(CCollisionNode* i_node, CCollisionList* i_list,
		int &io_numObjects, int &io_numVisible, Vector3 &io_centroid,
		CCollisionPair* &io_pair)
{
	if ((i_node != NULL) && ((io_numVisible == 0) || ((d_config &
			(CSightEntity::INTERSECT_ALL | CSightEntity::DESCRIBE_VISIBLE)) != 0)))
	{
		Vector3 location;
		CCollisionPair* pair;
		switch(TestObject(i_node->BeginObjectTest(), i_list, (io_numVisible == 0),
				location, pair))
		{
		 case NO_OBJECT:
			break;

		 case INVALID_OBJECT:
			i_node->EndObjectTest(CCollisionNode::INTERSECTION_IGNORED);
			break;

		 case OBJECT_OBSCURED:
			if (!i_node->IgnoreIntersection(NULL))
				io_numObjects++;
			i_node->EndObjectTest(CCollisionNode::INTERSECTION_IGNORED);
			break;

		 case OBJECT_VISIBLE:
		 {
			CCollisionIntersection* intersection = NULL;
			if (pair != NULL)
				intersection = pair->GetIntersection();
			if (i_node->IgnoreIntersection(intersection))
			{
				CCollisionStackItem::Delete(pair);
				i_node->EndObjectTest(CCollisionNode::INTERSECTION_IGNORED);
			}
			else
			{
				io_numObjects++;
				io_numVisible++;
				io_centroid += location;
				if (pair != NULL)
				{
					io_pair = pair->AddToEvent(NULL, io_pair, true);
					i_node->EndObjectTest(CCollisionNode::INTERSECTION_ACCEPTED,
							pair);
				}
				else
					i_node->EndObjectTest(CCollisionNode::INTERSECTION_IGNORED);
			}
			break;
		 }
		}

		CCollisionNode* child;
		while ((child = i_node->GetChild()) != NULL)
		{
			TestNode(child, i_list, io_numObjects, io_numVisible, io_centroid,
					io_pair);
		}
	}
}

/******************************************************************************/
/******************************************************************************/
CObscureEntity::EObjectTestResult CObscureEntity::TestObject(
		CCollisionObject* i_object, CCollisionList* i_list, bool i_noVisibleFound,
		Vector3 &o_location, CCollisionPair* &o_pair)
{
	// See if there is an object.
	if (i_object == NULL)
		return(NO_OBJECT);

	// Find the center of the object.
	switch(i_object->GetObjectType())
	{
	 case CCollisionObject::OBB_OBJECT:
		o_location = *((OBB*)i_object)->Center();
		break;
	 case CCollisionObject::AABB_OBJECT:
		o_location = ((AABB*)i_object)->Center();
		break;
	 case CCollisionObject::SPHERE:
		o_location = ((CSphereCollisionObject*)i_object)->ConstCenter();
		break;
	 case CCollisionObject::CAPSULE:
		o_location = ((Capsule*)i_object)->Center();
		break;
	 default:
		return(INVALID_OBJECT);
	}

	// See if a test is necessary.
	o_pair = NULL;
	bool intersect = ((d_config & (CSightEntity::INTERSECT_ALL |
			(i_noVisibleFound ? CSightEntity::INTERSECT_ANY : 0x0))) != 0);
	if ((i_list != NULL) || intersect)
	{
		// Setup a line query entity.
		CLineEntity line;
		line.Line().SetFindMode(CLineCollisionObject::FIND_ANY);
		line.Line().SetDetectionAbility(CLineCollisionObject::DETECT_FRONT_SIDE);
		line.Line().SetOriginEnd(d_locationInWorld, o_location);
		line.BeginQuery();

		// See if the object is obscured.
		if (i_list != NULL)
		{
			// Walk through each event in the list of possible obscuring entities.
			CCollisionEvent* event = i_list->GetHeadEvent();
			while (event != NULL)
			{
				// Walk through each object pair. Note that in all cases, the
				// query object is the obscure entity's obb.
				CCollisionPair* pair = event->GetHeadPair();
				while (pair != NULL)
				{
					// If the terrain is the potential obscurer, just test the
					// detected triangle list.
					CCollisionObject* object;
					if (pair->GetIntersectionType() ==
							CCollisionIntersection::OBB_TO_TERRAIN)
						object = &((CObbToTerrainIntersection*)pair->GetIntersection()
								)->GetTriangleList();

					// For any other case, test the target object.
					else
						object = pair->GetTargetObject();

					// Perform the intersection check. If an intersection is
					// detected, an obscuration was found.
					CCollisionIntersection* intersection =
							CCollisionPair::TestObjectPair(&line.Line(), object,
							&g_collisionSystem.GetStack());
					if (intersection != NULL)
					{
#ifdef DRAW_DEBUG
						line.Line().Draw(DrawUtility::RED);
						object->Draw(DrawUtility::YELLOW);
#endif
						CCollisionStackItem::Delete(intersection);
						line.EndQuery();
						return(OBJECT_OBSCURED);
					}

					// Next pair.
					pair = pair->GetNext();
				}

				// Next event.
				event = event->GetNext();
			}
		}

		// If desired, test for an intersection with the test object.  The only
		// way this should be able to fail is if the sight eyepoint is inside the
		// test object.
		if (intersect)
		{
			CCollisionIntersection* intersection =
					CCollisionPair::TestObjectPair(&line.Line(), i_object,
					&g_collisionSystem.GetStack());
			if (intersection != NULL)
			{
#ifdef DRAW_DEBUG
				line.Line().Draw(DrawUtility::GREEN);
				i_object->Draw(DrawUtility::BLUE);
#endif
				o_pair = CCollisionPair::New(&line.Line(), i_object,
						intersection, true, &g_collisionSystem.GetStack());
				if (o_pair == NULL)
					CCollisionStackItem::Delete(intersection);
			}
		}

		// End the query.
		line.EndQuery();
	}

	return(OBJECT_VISIBLE);
}

////////////////////////////////// CSightNode //////////////////////////////////

/******************************************************************************/
/******************************************************************************/
bool CSightNode::IgnoreIntersection(CCollisionIntersection* i_intersection)
{
	switch(i_intersection->GetIntersectionType())
	{
	 case CCollisionIntersection::SPHERE_TO_TERRAIN:
	 {
		CTriangleListCollisionObject &list =
				((CSphereToTerrainIntersection*)i_intersection)->GetTriangleList();
		for (int i = 0; i < list.NumTriangles(); i++)
		{
			if (TestFov(list.UnsafeGetCentroid(i)))
				return(false);
		}
		break;
	 }
	 case CCollisionIntersection::SPHERE_TO_OBB:
		return(!TestFov(
				*((CSphereToObbIntersection*)i_intersection)->Obb().Center()));
	 case CCollisionIntersection::SPHERE_TO_CAPSULE:
		return(!TestFov(((CSphereToCapsuleIntersection*)
				i_intersection)->GetSphere().Center()));
	 default:
		ASSERTS(false, "CSightNode cannot process this intersection type.");
		break;
	}
	return(true);
}

/******************************************************************************/
// See if a point is in the FOV.
/******************************************************************************/
bool CSightNode::TestFov(Vector3CRef i_targetLocation)
{
	// Find the vector to the target.
	Vector3 targetVector = i_targetLocation - d_sphere.ConstCenter();

	// Check the field of view.
	float scaledCosAngle = d_directionInWorld.Dot(targetVector);
	if (d_fov >= Math::HalfPi)
	{
		if ((scaledCosAngle < 0.0f) && (scaledCosAngle * scaledCosAngle >
				targetVector.LengthSquared() * d_cosFovSquared))
			return(false);
	}
	else if ((scaledCosAngle <= 0.0f) || (scaledCosAngle * scaledCosAngle <
			targetVector.LengthSquared() * d_cosFovSquared))
		return(false);

	// In FOV.
	return(true);
}

///////////////////////////////// CSightEntity /////////////////////////////////

/******************************************************************************/
/******************************************************************************/
void CSightEntity::SyncWithActor(void)
{
	if (d_actor != NULL)
	{
		d_sightNode.Sphere().SetCenter(d_actor->GetBodyInWorld() +
				d_locationInActor * d_actor->GetBodyToWorld());
		d_sightNode.SetUnitDirection(d_directionInActor *
				d_actor->GetBodyToWorld());
	}
}

/******************************************************************************/
/******************************************************************************/
CCollisionEvent* CSightEntity::Test(CStageEntity* i_target)
{
	CCollisionEvent* event = NULL;

	if (i_target != NULL)
	{
		g_collisionSystem.ResetStack();
		event = CCollisionEvent::TestEntityPair(this, i_target,
				&g_collisionSystem.GetStack());
		if (event && !d_obscure.Test(event))  //brian...added "event &&"
		{
			CCollisionStackItem::Delete(event);
			event = NULL;
		}
	}

	return(event);
}

/******************************************************************************/
/******************************************************************************/
CCollisionList* CSightEntity::Test(void)
{
	g_collisionSystem.ResetStack();
	CCollisionList* list = g_collisionSystem.Test(this);

	if (list != NULL)
	{
		CCollisionEvent* event = list->GetHeadEvent();
		while (event != NULL)
		{
			CCollisionEvent* next = event->GetNext();
			if (!d_obscure.Test(event))
				list->RemoveEvent(event);
			event = next;
		}

		if (list->Empty())
		{
			CCollisionStackItem::Delete(list);
			list = NULL;
		}
	}

	return(list);
}

/******************************************************************************/
/******************************************************************************/
bool CSightEntity::GetCollisionCull(CCollisionCull &o_cull)
{
	d_sightNode.Sphere().ComputeWorldExtents(o_cull.SelectAABB());
	return(true);
}

/******************************************************************************/
/******************************************************************************/
CCollisionNode* CSightEntity::BeginCollisionTest(CStageEntity* i_entity,
		ESelect i_select)
{
	if ((i_entity == d_actor) || IsIgnoredEntity(i_entity))
		return(NULL);

	return(&d_sightNode);
}
