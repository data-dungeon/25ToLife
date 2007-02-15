#include "collide/CollidePCH.h"

#include "collide/Pair.h"
#include "collide/Object.h"
#include "collide/LineToTerrain.h"
#include "collide/LineToObb.h"
#include "collide/LineToAabb.h"
#include "collide/LineToSphere.h"
#include "collide/ObbToTerrain.h"
#include "collide/ObbToObb.h"
#include "collide/AabbToTerrain.h"
#include "collide/AabbToObb.h"
#include "collide/AabbToAabb.h"
#include "collide/SphereToTerrain.h"
#include "collide/SphereToObb.h"
#include "collide/LineToTriangleList.h"
#include "collide/CapsuleToAabb.h"
#include "collide/LineToCapsule.h"
#include "collide/SphereToCapsule.h"
#include "collide/CapsuleToCapsule.h"
#include "collide/CapsuleToObb.h"
#include "collide/CapsuleToTerrain.h"

/**********************************************************************
*
*	Class:       CCollisionPair
*	Function:    TestObjectPair() [static]
*	Access:      public
*	Description: Identifies the type of intersection for a pair of
*              objects and tests for an intersection.
*	Arguments:
*    i_queryObject -  A collision object to be tested.
*    i_targetObject - The other collision object to be tested.
*    i_stack   -      A pointer the the stack to use. Use normal dynamic
*                     memory if NULL.
*	Return:      A pointer to a CCollisionIntersection if the two
*              intersect, NULL otherwise. The object pointed to is
*              derived from CCollisionIntersection and is the correct
*              one for this pair of objects. Also, it was allocated
*              so if it does not end up being used, it should be
*              deleted.
*
**********************************************************************/
// TN -- moved this into a cpp file, because I'm working in it, and every change I make
// causes 95% of the entire project to be recompiled.
// Besides which, no compiler in its right mind is going to obey that "inline" command,
// and even if it did, you'd be blowing out your instruction cache so much that the inline
// didn't buy you squat.
CCollisionIntersection* CCollisionPair::TestObjectPair(
	CCollisionObject* i_queryObject, CCollisionObject* i_targetObject,
	CCollisionStack* i_stack)
{
	/*
	*	Set the tester to the object with the higher object type and the testee
	*	to the object with the lower object type. Then, the tester need only check
	*	for intersections with objects of its type and lower. This has two
	*	advantages. First, the testing of a particular pairing of object types need
	*	only be dealt with in one place. Second, as new object types are added,
	*	one new section of code needs to be added that tests all the pairings
	*	with the new type.
	*
	*	If two objects of the same type are being tested, make the query object
	*	the tester and the target object the testee. That way the objects in an
	*	intersection can be identified as query and target.
	*/
	CCollisionObject* tester;
	CCollisionObject* testee;
	if (i_queryObject->GetObjectType() >= i_targetObject->GetObjectType())
	{
		tester = i_queryObject;
		testee = i_targetObject;
	}
	else
	{
		tester = i_targetObject;
		testee = i_queryObject;
	}
	/*
	*	Select on the type of tester.
	*/
	switch(tester->GetObjectType())
	{
		/*
		*	 Attempting to test terrain against itself is an error.
		*/
	case CCollisionObject::TERRAIN:
		break;

	case CCollisionObject::LINE:
		switch(testee->GetObjectType())
		{
		case CCollisionObject::TERRAIN:
			return(CLineToTerrainIntersection::TestForIntersection(
				(CLineCollisionObject*)tester, (CTerrainCollisionObject*)testee,
				i_stack));
			/*
			*		 Lines have no thickness so they cannot intersect with each other.
			*/
		case CCollisionObject::LINE:
			break;

		default:
			break;
		}
		break;

	case CCollisionObject::OBB_OBJECT:
		switch(testee->GetObjectType())
		{
		case CCollisionObject::TERRAIN:
			return(CObbToTerrainIntersection::TestForIntersection((OBB*)tester,
				(CTerrainCollisionObject*)testee, i_stack));
		case CCollisionObject::LINE:
			return(CLineToObbIntersection::TestForIntersection(
				(CLineCollisionObject*)testee, (OBB*)tester, i_stack));
		case CCollisionObject::OBB_OBJECT:
			return(CObbToObbIntersection::TestForIntersection((OBB*)tester,
				(OBB*)testee, i_stack));
		default:
			break;
		}
		break;

	case CCollisionObject::AABB_OBJECT:
		switch(testee->GetObjectType())
		{
		case CCollisionObject::TERRAIN:
			return(CAabbToTerrainIntersection::TestForIntersection(
				(CAabbCollisionObject*)tester, (CTerrainCollisionObject*)testee,
				i_stack));
		case CCollisionObject::LINE:
			return(CLineToAabbIntersection::TestForIntersection(
				(CLineCollisionObject*)testee, (CAabbCollisionObject*)tester,
				i_stack));
		case CCollisionObject::OBB_OBJECT:
			return(CAabbToObbIntersection::TestForIntersection(
				(CAabbCollisionObject*)tester, (OBB*)testee, i_stack));
		case CCollisionObject::AABB_OBJECT:
			return(CAabbToAabbIntersection::TestForIntersection(
				(CAabbCollisionObject*)tester, (CAabbCollisionObject*)testee,
				i_stack));
		}
		break;

	case CCollisionObject::SPHERE:
		switch(testee->GetObjectType())
		{
		case CCollisionObject::TERRAIN:
			return(CSphereToTerrainIntersection::TestForIntersection(
				(CSphereCollisionObject*)tester,
				(CTerrainCollisionObject*)testee, i_stack));
		case CCollisionObject::LINE:
			return(CLineToSphereIntersection::TestForIntersection(
				(CLineCollisionObject*)testee, (CSphereCollisionObject*)tester,
				i_stack));
		case CCollisionObject::OBB_OBJECT:
			return(CSphereToObbIntersection::TestForIntersection(
				(CSphereCollisionObject*)tester, (OBB*)testee, i_stack));
		case CCollisionObject::AABB_OBJECT:
			ASSERTS(false, "Sphere To AABB not implemented yet.\n");
			return(NULL);
		case CCollisionObject::SPHERE:
			ASSERTS(false, "Sphere To Sphere not implemented yet.\n");
			return(NULL);
		}
		break;

	case CCollisionObject::TRIANGLE_LIST:
		switch(testee->GetObjectType())
		{
		case CCollisionObject::TERRAIN:
			ASSERTS(false, "Triangle List To Terrain not implemented yet.\n");
			return(NULL);
		case CCollisionObject::LINE:
			return(CLineToTriangleListIntersection::TestForIntersection(
				(CLineCollisionObject*)testee,
				(CTriangleListCollisionObject*)tester, i_stack));
		case CCollisionObject::OBB_OBJECT:
			ASSERTS(false, "Triangle List To OBB not implemented yet.\n");
			return(NULL);
		case CCollisionObject::AABB_OBJECT:
			ASSERTS(false, "Triangle List To AABB not implemented yet.\n");
			return(NULL);
		case CCollisionObject::SPHERE:
			ASSERTS(false, "Triangle List To Sphere not implemented yet.\n");
			return(NULL);
		case CCollisionObject::TRIANGLE_LIST:
			ASSERTS(false, "Triangle List To Triangle List not implemented yet.\n");
			return(NULL);
		}
		break;
	case CCollisionObject::CAPSULE:
		switch(testee->GetObjectType())
		{
		case CCollisionObject::TERRAIN:
			return( CCapsuleToTerrainIntersection::TestForIntersection(
				(Capsule *)tester,
				(CTerrainCollisionObject*)testee,
				i_stack ) );
		case CCollisionObject::LINE:
			return( CLineToCapsuleIntersection::TestForIntersection(
				(CLineCollisionObject*)testee,
				(Capsule *)tester,
				i_stack ) );
		case CCollisionObject::OBB_OBJECT:
			return( CCapsuleToObbIntersection::TestForIntersection(
				(Capsule *)tester,
				(OBB *)testee, 
				i_stack ) );
		case CCollisionObject::AABB_OBJECT:
			return( CCapsuleToAabbIntersection::TestForIntersection(
				(Capsule *)tester,
				(CAabbCollisionObject *)testee, 
				i_stack ) );
		case CCollisionObject::SPHERE:
			return( CSphereToCapsuleIntersection::TestForIntersection(
				(CSphereCollisionObject*)testee,
				(Capsule *)tester,
				i_stack ) );
		case CCollisionObject::TRIANGLE_LIST:
			ASSERTS(false, "Capsule to Triangle list not implemented yet\n" );
			return(NULL);
		case CCollisionObject::CAPSULE:
			return( CCapsuleToCapsuleIntersection::TestForIntersection(
				(Capsule *)tester,
				(Capsule *)testee,
				i_stack ) );
		}
		break;
	}

	ASSERT(false);
	return(NULL);
}

