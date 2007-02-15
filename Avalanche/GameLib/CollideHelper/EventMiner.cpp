/*
**
**  File:   EventMiner.cpp
**  Date:   June 11, 2003
**  By:     Bryant Collard
**  Desc:   Extracts information from events.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/NetDev/NetDev.cpp $
**  $Revision: #12 $
**  $DateTime: 2003/11/19 14:00:18 $
**    $Author: Bryant_Collard $
**
*/

#include "CollideHelper/CollideHelperPCH.h"

// My header (always first) //
#include "CollideHelper/EventMiner.h"

#include "stage/cactor.h"
#include "collide/StaticEntity.h"
#include "collide/TerrainEntity.h"
#include "Game/Database/Som.h"     // For INVALID_MATERIALINDEX
#include "collide/StackArray.h"
#include "Interfaces/SurfaceTypeIF.h"
#include "collide/CapsuleToObb.h"

#define MAX_POINTS            20

/**********************************************************************/
/**********************************************************************/
bool CEventMiner::PlaneFit(CCollisionEvent* i_event, Vector3 &o_point,
		Vector3 &o_normal)
{
	if (i_event == NULL)
		return(false);

	Vector3 pointList[MAX_POINTS];
	int numPoints;
	Vector3 normalSum = PlaneFitPrep(i_event, MAX_POINTS, pointList, numPoints);
	
	// if the prep only gave back one point, use that point and the normalsum
	if( numPoints == 1 )
	{
		o_point = pointList[0];
		o_normal = normalSum;
		o_normal.SafeNormalize();
		return true;
	}

	CPlaneFit::EPlaneFitResult result = CPlaneFit::TransformFit(numPoints,
			pointList, normalSum, o_point, o_normal);
	
	return(result == CPlaneFit::PLANE_FIT_SCATTER);
}

/**********************************************************************/
/**********************************************************************/
bool CEventMiner::PlaneFit(CCollisionEvent* i_event,
		Vector3CRef i_constraintNormal, Vector3 &o_point, Vector3 &o_normal)
{
	if (i_event == NULL)
		return(false);

	Vector3 pointList[MAX_POINTS];
	int numPoints;
	Vector3 normalSum = PlaneFitPrep(i_event, MAX_POINTS, pointList, numPoints);
	CPlaneFit::EPlaneFitResult result = CPlaneFit::TransformFit(numPoints,
			pointList, normalSum, i_constraintNormal, o_point, o_normal);
	return(result == CPlaneFit::PLANE_FIT_SCATTER);
}

/**********************************************************************/
/**********************************************************************/
bool CEventMiner::SelectedPlaneFit(CCollisionEvent* i_event,
		CStageEntity* i_selectionEntity, Vector3CRef i_direction,
		float i_scaledCosAngle, Vector3 &o_point, Vector3 &o_normal)
{
	if (i_event == NULL)
		return(false);

	Vector3 pointList[MAX_POINTS];
	int numPoints;
	Vector3 normalSum = SelectedPlaneFitPrep(i_event, i_selectionEntity,
			i_direction, i_scaledCosAngle, MAX_POINTS, pointList, numPoints);
	CPlaneFit::EPlaneFitResult result = CPlaneFit::TransformFit(numPoints,
			pointList, normalSum, o_point, o_normal);
	return(result == CPlaneFit::PLANE_FIT_SCATTER);
}

/**********************************************************************/
/**********************************************************************/
bool CEventMiner::SelectedPlaneFit(CCollisionEvent* i_event,
		CStageEntity* i_selectionEntity, Vector3CRef i_direction,
		float i_scaledCosAngle, Vector3CRef i_constraintNormal,
		Vector3 &o_point, Vector3 &o_normal)
{
	if (i_event == NULL)
		return(false);

	Vector3 pointList[MAX_POINTS];
	int numPoints;
	Vector3 normalSum = SelectedPlaneFitPrep(i_event, i_selectionEntity,
			i_direction, i_scaledCosAngle, MAX_POINTS, pointList, numPoints);
	CPlaneFit::EPlaneFitResult result = CPlaneFit::TransformFit(numPoints,
			pointList, normalSum, i_constraintNormal, o_point, o_normal);
	return(result == CPlaneFit::PLANE_FIT_SCATTER);
}

/**********************************************************************/
/**********************************************************************/
Vector3 CEventMiner::PlaneFitPrep(CCollisionEvent* i_event, int i_maxPoints,
		Vector3* o_pointList, int &o_numPoints)
{
	o_numPoints = 0;
	Vector3 normalSum(0.0f, 0.0f, 0.0f);
	CCollisionPair* pair = i_event->GetHeadPair();
	while ((pair != NULL) && (o_numPoints < i_maxPoints))
	{
		if (pair->GetIntersectionType() == CCollisionIntersection::OBB_TO_TERRAIN)
		{
			CObbToTerrainIntersection* intersection =
					(CObbToTerrainIntersection*)pair->GetIntersection();
			normalSum += intersection->TriangleNormalSum();

			CObbToTerrainIntersection::SObbEdgeTerrainState state;
			if (intersection->InitObbEdgeIntersectionTest(state))
			{
				TerrainTriangle* triangle;
				while (intersection->GetNextObbEdgeIntersection(state, triangle,
						o_pointList[o_numPoints]))
				{
					if (++o_numPoints >= i_maxPoints)
						return(normalSum);
				}
			}

			CCollisionStackArray<Vector3>* triangleIntersection =
					intersection->GetTriangleIntersections();
			if (triangleIntersection != NULL)
			{
				int intersectionPoints = triangleIntersection->NumEntries();
				if ((o_numPoints + intersectionPoints) > i_maxPoints)
					intersectionPoints = i_maxPoints - o_numPoints;
				for (int i = 0; i < intersectionPoints; i++)
					o_pointList[o_numPoints++] = triangleIntersection->EntryRef(i);
			}
		}
		else if (pair->GetIntersectionType() == CCollisionIntersection::OBB_TO_OBB)
		{
			CObbToObbIntersection* intersection =
					(CObbToObbIntersection*)pair->GetIntersection();
			Vector3 obbNormalSum = intersection->EdgeIntersectionNormalSum();
			Vector3 centers = (*intersection->ObbA().Center()) -
					(*intersection->ObbB().Center());
			if (obbNormalSum.LengthSquared() < 1.0e-4f)
				obbNormalSum = centers;
			if (obbNormalSum.Dot(centers) < 0.0f)
				obbNormalSum.Negate();
			normalSum += obbNormalSum;
			int intersectionPoints = intersection->EdgeIntersections();
			if ((o_numPoints + intersectionPoints) > i_maxPoints)
				intersectionPoints = i_maxPoints - o_numPoints;
			for (int i = 0; i < intersectionPoints; i++)
				o_pointList[o_numPoints++] = *(intersection->EdgeIntersection(i));
		}
		else if( pair->GetIntersectionType() == CCollisionIntersection::CAPSULE_TO_OBB )
		{
			CCapsuleToObbIntersection* c2o =
				(CCapsuleToObbIntersection *)pair->GetIntersection();
			Vector3 capsuleNormalSum;
			float depth;
			c2o->GetResolutionNormal( capsuleNormalSum, depth );
			capsuleNormalSum.Negate();
			o_pointList[o_numPoints++] = c2o->GetClosestPointOnOBB();
			normalSum += capsuleNormalSum;
		}
		else
			ASSERTS(false, "Only support obbs so far.\n");
		pair = pair->GetNext();
	}
	return(normalSum);
}

/**********************************************************************/
/**********************************************************************/
Vector3 CEventMiner::SelectedPlaneFitPrep(CCollisionEvent* i_event,
		CStageEntity* i_selectionEntity, Vector3CRef i_direction,
		float i_scaledCosAngle, int i_maxPoints, Vector3* o_pointList,
		int &o_numPoints)
{
	ASSERT((i_selectionEntity == NULL) ||
			(i_selectionEntity == i_event->GetQueryEntity()) ||
			(i_selectionEntity == i_event->GetTargetEntity()));
	if (i_selectionEntity != i_event->GetTargetEntity())
		i_selectionEntity = i_event->GetQueryEntity();

	o_numPoints = 0;
	Vector3 normalSum(0.0f, 0.0f, 0.0f);
	CCollisionPair* pair = i_event->GetHeadPair();
	while ((pair != NULL) && (o_numPoints < i_maxPoints))
	{
		if (pair->GetIntersectionType() == CCollisionIntersection::OBB_TO_TERRAIN)
		{
			ASSERT(i_selectionEntity == i_event->GetQueryEntity());

			CObbToTerrainIntersection* intersection =
					(CObbToTerrainIntersection*)pair->GetIntersection();
			normalSum += intersection->TriangleNormalSum(i_direction,
					i_scaledCosAngle);

			CObbToTerrainIntersection::SObbEdgeTerrainState state;
			if (intersection->InitObbEdgeIntersectionTest(state))
			{
				TerrainTriangle* triangle;
				Vector3 point;
				while (intersection->GetNextObbEdgeIntersection(state, triangle,
						point))
				{
					if (i_direction.Dot(triangle->FaceNormal()) >= i_scaledCosAngle)
					{
						o_pointList[o_numPoints] = point;
						if (++o_numPoints >= i_maxPoints)
							return(normalSum);
					}
				}
			}

			CCollisionStackArray<Vector3>* triangleIntersection =
					intersection->GetTriangleIntersections(i_direction,
					i_scaledCosAngle);
			if (triangleIntersection != NULL)
			{
				int intersectionPoints = triangleIntersection->NumEntries();
				if ((o_numPoints + intersectionPoints) > i_maxPoints)
					intersectionPoints = i_maxPoints - o_numPoints;
				for (int i = 0; i < intersectionPoints; i++)
					o_pointList[o_numPoints++] = triangleIntersection->EntryRef(i);
				CCollisionStackItem::Delete(triangleIntersection);
			}
		}
		else if (pair->GetIntersectionType() == CCollisionIntersection::OBB_TO_OBB)
		{
			CObbToObbIntersection* intersection =
					(CObbToObbIntersection*)pair->GetIntersection();
			uint16 selectedFaceMask = intersection->SelectFaces(
					(i_selectionEntity == i_event->GetTargetEntity()),
					i_direction, i_scaledCosAngle);
			Vector3 obbNormalSum = intersection->EdgeIntersectionNormalSum(
					selectedFaceMask);
			Vector3 centers = (*intersection->ObbA().Center()) -
					(*intersection->ObbB().Center());
			if (obbNormalSum.LengthSquared() < 1.0e-4f)
				obbNormalSum = centers;
			if (obbNormalSum.Dot(centers) < 0.0f)
				obbNormalSum.Negate();
			normalSum += obbNormalSum;
			int intersectionPoints = intersection->EdgeIntersections();
			for (int i = 0; i < intersectionPoints; i++)
			{
				const Vector3* point = intersection->EdgeIntersection(i,
						selectedFaceMask);
				if (point != NULL)
				{
					o_pointList[o_numPoints] = *point;
					if (++o_numPoints >= i_maxPoints)
						return(normalSum);
				}
			}
		}
		else if( pair->GetIntersectionType() == CCollisionIntersection::CAPSULE_TO_OBB )
		{
			CCapsuleToObbIntersection* c2o =
				(CCapsuleToObbIntersection *)pair->GetIntersection();
			Vector3 capsuleNormalSum;
			float depth;
			c2o->GetResolutionNormal( capsuleNormalSum, depth );
			o_pointList[o_numPoints++] = c2o->GetClosestPointOnOBB();
			normalSum += capsuleNormalSum;
		}
		else
			ASSERTS(false, "Only support obbs so far.\n");
		pair = pair->GetNext();
	}
	return(normalSum);
}

/**********************************************************************/
/**********************************************************************/
void CEventMiner::Depths(CCollisionEvent* i_event, Vector3CRef i_point,
		Vector3CRef i_normal, float &o_queryDepth, float &o_targetDepth)
{
	float frontDistance, backDistance;
	o_queryDepth = 0.0f;
	o_targetDepth = 0.0f;

	if (i_event == NULL)
		return;

	CCollisionPair* pair = i_event->GetHeadPair();
	while (pair != NULL)
	{
		if (pair->GetIntersectionType() == CCollisionIntersection::OBB_TO_TERRAIN)
		{
			CObbToTerrainIntersection* intersection =
					(CObbToTerrainIntersection*)pair->GetIntersection();
			intersection->Obb().ComputeWorldPlaneDistance(i_point, i_normal,
					frontDistance, backDistance);
			if (backDistance < o_queryDepth)
				o_queryDepth = backDistance;

			CCollisionStackArray<Vector3>* triangleVertex =
					intersection->GetTriangleVertices();
			if (triangleVertex != NULL)
			{
				int vertexPoints = triangleVertex->NumEntries();
				for (int i = 0; i < vertexPoints; i++)
				{
					float vertexDepth =
							i_normal.Dot(triangleVertex->EntryRef(i) - i_point);
					if (vertexDepth > o_targetDepth)
						o_targetDepth = vertexDepth;
				}
			}

			CCollisionStackArray<Vector3>* triangleIntersection =
					intersection->GetTriangleIntersections();
			if (triangleIntersection != NULL)
			{
				int intersectionPoints = triangleIntersection->NumEntries();
				for (int i = 0; i < intersectionPoints; i++)
				{
					float intersectionDepth =
							i_normal.Dot(triangleIntersection->EntryRef(i) - i_point);
					if (intersectionDepth > o_targetDepth)
						o_targetDepth = intersectionDepth;
				}
			}
		}
		else if (pair->GetIntersectionType() == CCollisionIntersection::OBB_TO_OBB)
		{
			CObbToObbIntersection* intersection =
					(CObbToObbIntersection*)(pair->GetIntersection());

			Vector3 frontPoint = intersection->ObbB().GetWorldFrontCorner(i_normal);
			bool frontIsCorner = intersection->ObbA().PointInBox(frontPoint);

			Vector3 backPoint = intersection->ObbA().GetWorldBackCorner(i_normal);
			bool backIsCorner = intersection->ObbB().PointInBox(backPoint);

			if (!frontIsCorner || !backIsCorner)
			{
				frontDistance = -MAXFLOAT;
				backDistance = MAXFLOAT;
				int intersectionPoints = intersection->EdgeIntersections();
				for (int i = 0; i < intersectionPoints; i++)
				{
					float intersectionDepth =
							i_normal.Dot(*(intersection->EdgeIntersection(i)) -
							i_point);
					if (intersectionDepth > frontDistance)
						frontDistance = intersectionDepth;
					if (intersectionDepth < backDistance)
						backDistance = intersectionDepth;
				}

				if (frontIsCorner)
					frontDistance = i_normal.Dot(frontPoint - i_point);
				else if (backIsCorner)
					backDistance = i_normal.Dot(backPoint - i_point);
			}
			else
			{
				frontDistance = i_normal.Dot(frontPoint - i_point);
				backDistance = i_normal.Dot(backPoint - i_point);
			}

			if (backDistance < o_queryDepth)
				o_queryDepth = backDistance;
			if (frontDistance > o_targetDepth)
				o_targetDepth = frontDistance;
		}
		else if (pair->GetIntersectionType() == CCollisionIntersection::CAPSULE_TO_OBB)
		{
			CCapsuleToObbIntersection* c2o =
				(CCapsuleToObbIntersection *)pair->GetIntersection();

			Vector3 frontPoint = c2o->GetOBB().GetWorldFrontCorner( i_normal );
			o_targetDepth = (frontPoint - i_point).Dot( i_normal );

			float originDepth = (i_point - c2o->GetCapsule().Origin()).Dot( i_normal );
			float endDepth = (i_point - c2o->GetCapsule().End()).Dot( i_normal );

			if( originDepth < endDepth )
				o_queryDepth = originDepth;
			else
				o_queryDepth = endDepth;

			o_queryDepth -= c2o->GetCapsule().Radius();
		}
		pair = pair->GetNext();
	}
	o_queryDepth = -o_queryDepth;
}

/**********************************************************************/
/**********************************************************************/
void CEventMiner::SurfaceType(CCollisionEvent* i_event, Vector3CRef i_point,
		Vector3CRef i_normal, u8 &o_queryType, u8 &o_targetType)
{
	o_queryType = INVALID_MATERIALINDEX;
	o_targetType = INVALID_MATERIALINDEX;
	if (i_event == NULL)
		return;

	CStageEntity* query = i_event->GetQueryEntity();
	CSurfaceTypeIF* surfaceTypeIF = NULL;
	if (query->IsA(CActor::ClassType()))
		surfaceTypeIF = (CSurfaceTypeIF*)((CActor*)query)->GetInterface(
				CSurfaceTypeIF::GetID());
	if (surfaceTypeIF != NULL)
		o_queryType = surfaceTypeIF->GetSurfaceType(i_point, i_normal);
	else
		o_queryType = query->Material(i_event->GetHeadPair());

	CStageEntity* target = i_event->GetTargetEntity();
	if (target->IsA(CStaticEntity::ClassType()))
	{
		CCollisionPair* pair = i_event->GetHeadPair();
		if (pair != NULL)
		{
			OBBStaticInstance* obb = (OBBStaticInstance*)(pair->GetTargetObject());
			float minDistSq = (i_point - (*(obb->Center()))).LengthSquared();
			o_targetType = obb->Material();
			while ((pair = pair->GetNext()) != NULL)
			{
				obb = (OBBStaticInstance*)(pair->GetTargetObject());
				float distSq = (i_point - (*(obb->Center()))).LengthSquared();
				if (distSq < minDistSq)
				{
					minDistSq = distSq;
					o_targetType = obb->Material();
				}
			}
		}
	}
	else if (target->IsA(CTerrainEntity::ClassType()))
	{
		CTriangleListCollisionObject* triangles = NULL;
		CCollisionIntersection* intersection =
				i_event->GetHeadPair()->GetIntersection();
		switch(intersection->GetIntersectionType())
		{
		 case CCollisionIntersection::OBB_TO_TERRAIN:
			triangles =
					&((CObbToTerrainIntersection*)intersection)->GetTriangleList();
			break;
		}
		Vector3 centroid;
		if ((triangles != NULL) && triangles->GetCentroid(0, centroid))
		{
			float minDistSq = (i_point - centroid).LengthSquared();
			int index = 0;
			int numTriangles = triangles->NumTriangles();
			for (int i = 1; i < numTriangles; i++)
			{
				float distSq =
						(i_point - triangles->UnsafeGetCentroid(i)).LengthSquared();
				if (distSq < minDistSq)
				{
					minDistSq = distSq;
					index = i;
				}
			}
			o_targetType = triangles->UnsafeGetTriangle(index)->Material();
		}
	}
	else if (target->IsA(CActor::ClassType()))
	{
		surfaceTypeIF = (CSurfaceTypeIF*)((CActor*)target)->GetInterface(
				CSurfaceTypeIF::GetID());
		if (surfaceTypeIF != NULL)
			o_targetType = surfaceTypeIF->GetSurfaceType(i_point, i_normal);
		else
			o_targetType = target->Material(i_event->GetHeadPair());
	}
	else
		o_targetType = target->Material(i_event->GetHeadPair());
}

float CEventMiner::OBBSimpleDepth( OBB *obb, Vector3CRef point, Vector3CRef normal )
{
	// which corner is leading towards the collision normal?
	Vector3 lead = obb->GetWorldBackCorner( normal );
	return (point - lead).Dot( normal );
}

#if 0
bool CEventMiner::OBBToOBBFirstPointOfCollision( CCollisionEvent* i_event, OBB *first, OBB *second, 
		Vector3CRef movement, Vector3 &o_point, Vector3 &o_normal )
{
	Vector3 maxExtent = movement;

	if( !maxExtent.SafeNormalize() )
		return false;

	// figure out where our obbs hit first, by testing all of the following:
	// Each obb leading corner against the other obb (4x2)
	// Each obb leading edge against the other obb's leading edges (3x3)

	// static data space so we don't run into stack issues with a potentially large stack
	#define COLLISION_INDEX_COUNT 17
	static float collisionTime[COLLISION_INDEX_COUNT];
	static Vector3 collisionPoint[COLLISION_INDEX_COUNT];
	static Vector3 collisionNormal[COLLISION_INDEX_COUNT];

	OBB *obb[2];
	int face[2][3];
	int corner[2][4];
	Vector3 cornerV[2][8];
	int collisionIndex = 0, i, c;

	for( i = 0; i < COLLISION_INDEX_COUNT; i++ )
	{
		collisionTime[i] = MAXFLOAT;
	}

	maxExtent *= first->Radius() * 2.0f;

	corner[0][0] = first->GetWorldFrontCornerIndex( movement );
	corner[1][0] = second->GetWorldFrontCornerIndex( -movement );

	obb[0] = first;
	obb[1] = second;
	for( i = 0; i < 2; i++ )
	{
		obb[i]->Transform( &cornerV[i][0] );

		OBB::GetLeadingCorners( corner[i][0], corner[i][1], corner[i][2], corner[i][3] );
		OBB::GetLeadingFaces( corner[i][0], face[i][0], face[i][1], face[i][2] );
	}

	//DrawUtility::QueuePoint( cornerV[0][corner[0][0]], DrawUtility::MAGENTA, 0.1f, 10.0f );
	//DrawUtility::QueuePoint( cornerV[1][corner[1][0]], DrawUtility::MAGENTA, 0.1f, 10.0f );
	// corners into obbs
	for( c = 0; c < 4; c++ )
	{
		Vector3 v;
		v = cornerV[0][ corner[0][c] ];
		CLineCollisionObject line( v - maxExtent, maxExtent * 2.0f );
		line.SetFindMode( LINE_FIND_CLOSEST );
		line.InitializeFindMode();
		CLineToObbIntersection lineobb( line, *second );
		if( lineobb.IntersectionTest() && lineobb.Enters() )
		{
			collisionPoint[collisionIndex] = lineobb.EntryPoint();
			collisionNormal[collisionIndex] = lineobb.EntryNormal();
			collisionTime[collisionIndex] = (collisionPoint[collisionIndex] - line.Origin()).Dot( movement );
			DrawUtility::QueuePoint( collisionPoint[collisionIndex], DrawUtility::RED, 0.1f, 10.0f );
			DrawUtility::QueueNormal( collisionPoint[collisionIndex], collisionNormal[collisionIndex], DrawUtility::GREEN, 0.25f, 10.0f );
			DrawUtility::QueueLine( line.Origin(), line.End(), DrawUtility::YELLOW, 10.0f );
			DrawUtility::QueuePoint( v, DrawUtility::YELLOW, 0.1f, 10.0f );
		}
		else
		{
			DrawUtility::QueueLine( line.Origin(), line.End(), DrawUtility::CYAN, 10.0f );
			DrawUtility::QueuePoint( v, DrawUtility::CYAN, 0.1f, 10.0f );
		}
		collisionIndex++;
	}
	for( c = 0; c < 4; c++ )
	{
		Vector3 v = cornerV[1][ corner[1][c] ];
		CLineCollisionObject line( v + maxExtent, maxExtent * -2.0f );
		line.SetFindMode( LINE_FIND_CLOSEST );
		line.InitializeFindMode();
		CLineToObbIntersection lineobb( line, *first );
		if( lineobb.IntersectionTest() && lineobb.Enters() )
		{
			collisionPoint[collisionIndex] = lineobb.EntryPoint();
			collisionNormal[collisionIndex] = -lineobb.EntryNormal();
			collisionTime[collisionIndex] = (collisionPoint[collisionIndex] - line.End()).Dot( movement );
			DrawUtility::QueuePoint( collisionPoint[collisionIndex], DrawUtility::MAGENTA, 0.1f, 10.0f );
			DrawUtility::QueueNormal( collisionPoint[collisionIndex], collisionNormal[collisionIndex], DrawUtility::BLUE, 0.25f, 10.0f );
		}
		collisionIndex++;
	}


	#ifdef TERRAINTRIANGLE_LEAN
		ASSERT( 0 && "This code won't work with lean triangles.  Tell Travis to fix it" );
	#endif
	
	#if 0
	// edges into edges
	for( e1 = 0; e1 < 3; e1++ )
	{
		Vector3 faceNormal = (cornerV[0][e1] - cornerV[0][0]).Cross( movement );
		TerrainTriangle tri[2];
		tri[0].SetVertices( cornerV[0][0] - maxExtent, cornerV[0][e1] - maxExtent, cornerV[0][0] + maxExtent );
		tri[0].SetFaceNormal( faceNormal );
		tri[1].SetVertices( cornerV[0][e1] - maxExtent, cornerV[0][e1] + maxExtent, cornerV[0][0] + maxExtent );
		tri[1].SetFaceNormal( faceNormal );
		for( e2 = 0; e2 < 3; e2++ )
		{
			// e1 is the moving edge, e2 is the stationary one.  The actual test involves
			// sweeping e1 along the movement direction, then querying e2 into that object with the two 
			// triangles generated by that sweep.
			// We need to query e2 from the closest leading corner to the other point, so that we get the 
			// correct first intersection point if the two edges lie in the same plane.
			CLineCollisionObject line( cornerV[1][0], cornerV[1][e2] - cornerV[1][0] );
			line.SetFindMode( LINE_FIND_CLOSEST );
			line.InitializeFindMode();

			CTriangleListCollisionObject trilist;
			trilist.Open( &(g_collisionSystem.GetStack()) );
			trilist.Add( &tri[0] );
			trilist.Add( &tri[1] );
			trilist.Close();

			CLineToTriangleListIntersection linetotri( line, trilist );
			if( linetotri.IntersectionTest() )
			{
			}
			collisionIndex++;
		}
	}
	#endif
	collisionIndex += 9;

	ASSERT( collisionIndex == COLLISION_INDEX_COUNT );

	// which intersection hit first?

	int closest = -1;
	float closesttime = MAXFLOAT;
	for( i = 0; i < COLLISION_INDEX_COUNT; i++ )
	{
		if( collisionTime[i] < closesttime )
		{
			closest = i;
			closesttime = collisionTime[i];
		}
	}
	if( closest == -1 )
		return false;

	o_point = collisionPoint[closest];
	o_normal = collisionNormal[closest];

	//DrawUtility::QueuePoint( o_point, DrawUtility::WHITE, 0.1f, 10.0f );
	//DrawUtility::QueueNormal( o_point, o_normal, DrawUtility::WHITE, 0.75f, 10.0f );

	return true;
}
#endif