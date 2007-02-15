/*
**
**  File:   LineToAabb.hpp
**  Date:   March 11, 2003
**  By:     Bryant Collard
**  Desc:   Tests intersections between lines and AABBs.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/collide/LineToAabb.hpp $
**  $Revision: 5 $
**      $Date: 6/11/03 2:33p $
**    $Author: Tyler Colbert $
**
*/

#ifdef LINE_TO_AABB_HPP
#undef LINE_TO_AABB_HPP

// System includes //
#include <new.h>

// Gamelib includes //
#include "collide/Stack.h"

/***************************************************************************/
/***************************************************************************/
inline CLineToAabbIntersection::CLineToAabbIntersection(
		CLineCollisionObject &i_line, CAabbCollisionObject &i_aabb) :
	CCollisionIntersection(LINE_TO_AABB),
	d_line(i_line),
	d_aabb(i_aabb)
{
	d_modifiedLine = false;
	d_validFlags = 0;
}

/***************************************************************************/
/***************************************************************************/
inline CLineToAabbIntersection::CLineToAabbIntersection(
		CLineCollisionObject &i_line, CAabbCollisionObject &i_aabb,
		CCollisionStack &i_stack) :
	CCollisionIntersection(LINE_TO_AABB, i_stack),
	d_line(i_line),
	d_aabb(i_aabb)
{
	d_modifiedLine = false;
	d_validFlags = 0;
}

/***************************************************************************/
/***************************************************************************/
inline CLineToAabbIntersection::~CLineToAabbIntersection()
{
	if (d_modifiedLine)
		d_line.RestoreFindState(this, d_previousModifier, d_previousPoint);
}

/***************************************************************************/
/***************************************************************************/
inline CLineToAabbIntersection* CLineToAabbIntersection::New(
		CLineCollisionObject &i_line, CAabbCollisionObject &i_aabb,
		CCollisionStack* i_stack)
{
	if (i_stack != NULL)
	{
		CLineToAabbIntersection* intersection =
				(CLineToAabbIntersection*)i_stack->Allocate(
				sizeof(CLineToAabbIntersection));

		if (intersection)
			new(intersection) CLineToAabbIntersection(i_line, i_aabb, *i_stack);

		return(intersection);
	}

	return(new CLineToAabbIntersection(i_line, i_aabb));
}

/***************************************************************************/
/***************************************************************************/
inline void CLineToAabbIntersection::ClearTests(void)
{
	if (d_modifiedLine)
		d_line.RestoreFindState(this, d_previousModifier, d_previousPoint);

	d_modifiedLine = false;
	d_validFlags = 0;
}

/***************************************************************************/
/***************************************************************************/
inline bool CLineToAabbIntersection::IntersectionTest(void)
{
	if ((d_validFlags & INTERSECTION_TEST) == 0)
		PerformIntersectionTest();

	return(d_intersects);
}

/***************************************************************************/
/***************************************************************************/
inline bool CLineToAabbIntersection::Enters(void)
{
	if ((d_validFlags & INTERSECTION_TEST) == 0)
		PerformIntersectionTest();
	return(d_enters);
}

/***************************************************************************/
/***************************************************************************/
inline uint CLineToAabbIntersection::EntryFace(void)
{
	if ((d_validFlags & INTERSECTION_TEST) == 0)
		PerformIntersectionTest();
	return(d_entryFace);
}

/***************************************************************************/
/***************************************************************************/
inline float CLineToAabbIntersection::EntryFrac(void)
{
	if ((d_validFlags & INTERSECTION_TEST) == 0)
		PerformIntersectionTest();
	return(d_entryFrac);
}

/***************************************************************************/
/***************************************************************************/
inline Vector3CRef CLineToAabbIntersection::EntryPoint(void)
{
	if ((d_validFlags & ENTRY_POINT) == 0)
	{
		if ((d_validFlags & INTERSECTION_TEST) == 0)
			PerformIntersectionTest();

		if (d_intersects)
			d_line.ComputePoint(d_entryFrac, d_entryPoint);
		else
			d_entryPoint.Clear();

		d_validFlags |= ENTRY_POINT;
	}

	return(d_entryPoint);
}

/***************************************************************************/
/***************************************************************************/
inline bool CLineToAabbIntersection::Exits(void)
{
	if ((d_validFlags & INTERSECTION_TEST) == 0)
		PerformIntersectionTest();
	return(d_exits);
}

/***************************************************************************/
/***************************************************************************/
inline uint CLineToAabbIntersection::ExitFace(void)
{
	if ((d_validFlags & INTERSECTION_TEST) == 0)
		PerformIntersectionTest();
	return(d_exitFace);
}

/***************************************************************************/
/***************************************************************************/
inline float CLineToAabbIntersection::ExitFrac(void)
{
	if ((d_validFlags & INTERSECTION_TEST) == 0)
		PerformIntersectionTest();
	return(d_exitFrac);
}

/***************************************************************************/
/***************************************************************************/
inline Vector3CRef CLineToAabbIntersection::ExitPoint(void)
{
	if ((d_validFlags & EXIT_POINT) == 0)
	{
		if ((d_validFlags & INTERSECTION_TEST) == 0)
			PerformIntersectionTest();

		if (d_intersects)
			d_line.ComputePoint(d_exitFrac, d_exitPoint);
		else
			d_exitPoint.Clear();

		d_validFlags |= EXIT_POINT;
	}

	return(d_exitPoint);
}

/***************************************************************************/
/***************************************************************************/
inline CCollisionIntersection* CLineToAabbIntersection::TestForIntersection(
		CLineCollisionObject* i_line, CAabbCollisionObject* i_aabb,
		CCollisionStack* i_stack)
{
	CLineToAabbIntersection* intersection = CLineToAabbIntersection::New(
			*i_line, *i_aabb, i_stack);
	if (intersection)
	{
		if (intersection->IntersectionTest())
			return(intersection);
		CCollisionStackItem::Delete(intersection);
	}
	return(NULL);
}

#endif // LINE_TO_AABB_HPP
