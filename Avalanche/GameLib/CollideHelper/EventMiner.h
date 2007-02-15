/*
**
**  File:   EventMiner.h
**  Date:   June 11, 2003
**  By:     Bryant Collard
**  Desc:   Extracts information from events.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/CollideHelper/EventMiner.h $
**  $Revision: 8 $
**      $Date: 8/01/03 5:25p $
**    $Author: Bcollard $
**
*/

#ifndef EVENT_MINER_H
#define EVENT_MINER_H

class CCollisionEvent;

class CEventMiner
{
  public:
	// Try to represent a collision event as a plane and a point on that plane.
	static bool PlaneFit(CCollisionEvent* i_event, Vector3 &o_point,
			Vector3 &o_normal);

	// Try to represent a collision event as a plane and a point on that plane
	// but constrain the plane to be perpendicular to some other plane, that is,
	// make the result plane parallel to i_constraintNormal.
	static bool PlaneFit(CCollisionEvent* i_event,
			Vector3CRef i_constraintNormal, Vector3 &o_point, Vector3 &o_normal);

	// Try to represent a collision event as a plane and a point on that plane
	// but use selected information from the event to do so. Identify one of the
	// entities as the "selection entity" and only use surfaces it intersects
	// that point in a general direction in computing results. The direction is
	// set with i_direction and the selection threshold is set with
	// i_scaledCosAngle. To get expected results, i_scaledCosAngle should be the
	// cosine of the allowed angle between the surface normal and i_direction
	// scaled by the magnitude of i_direction. If the selection entity is NULL,
	// the query entity will be used.
	static bool SelectedPlaneFit(CCollisionEvent* i_event,
			CStageEntity* i_selectionEntity, Vector3CRef i_direction,
			float i_scaledCosAngle, Vector3 &o_point, Vector3 &o_normal);

	// Try to represent a collision event as a plane and a point on that plane
	// but constrain the plane to be perpendicular to some other plane and use
	// selected information from the event to do so.
	static bool SelectedPlaneFit(CCollisionEvent* i_event,
			CStageEntity* i_selectionEntity, Vector3CRef i_direction,
			float i_scaledCosAngle, Vector3CRef i_constraintNormal,
			Vector3 &o_point, Vector3 &o_normal);

	// Build a list of points to which a plane will be fit.
	static Vector3 PlaneFitPrep(CCollisionEvent* i_event, int i_maxPoints,
			Vector3* o_pointList, int &o_numPoints);

	// Build a selected list of points to which a plane will be fit.
	static Vector3 SelectedPlaneFitPrep(CCollisionEvent* i_event,
			CStageEntity* i_selectionEntity, Vector3CRef i_direction,
			float i_scaledCosAngle, int i_maxPoints,
			Vector3* o_pointList, int &o_numPoints);

	// Find the penetration depths of the collision entities relative to the
	// given plane.
	static void Depths(CCollisionEvent* i_event, Vector3CRef i_point,
			Vector3CRef i_normal, float &o_queryDepth, float &o_targetDepth);

	// Find the surface types of the collision entities at the given point
	// in the given direction.
	static void SurfaceType(CCollisionEvent* i_event, Vector3CRef i_point,
			Vector3CRef i_normal, u8 &o_queryType, u8 &o_targetType);

	static float OBBSimpleDepth( OBB *obb, Vector3CRef point, Vector3CRef normal );
#if 0
	static bool OBBToOBBFirstPointOfCollision( CCollisionEvent *i_event, OBB *first, OBB *second, Vector3CRef movementDirection, Vector3 &o_point, Vector3 &o_normal );
#endif
};

#endif // EVENT_MINER_H
