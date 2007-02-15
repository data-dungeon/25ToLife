/*
**
**  File:   LineToObb.hpp
**  Date:   February 4, 2003
**  By:     Bryant Collard
**  Desc:   Tests intersections between lines and OBBs.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/collide/LineToObb.hpp $
**  $Revision: 8 $
**      $Date: 6/11/03 2:33p $
**    $Author: Tyler Colbert $
**
*/

#ifdef LINE_TO_OBB_HPP
#undef LINE_TO_OBB_HPP

// System includes //
#include <new.h>

// Gamelib includes //
#include "collide/Stack.h"
#include "Layers/Assert/Assert.h"

/***************************************************************************/
/***************************************************************************/
inline CLineToObbIntersection::CLineToObbIntersection() :
	CCollisionIntersection(LINE_TO_OBB),
	d_line(NULL),
	d_obb(NULL)
{
	d_modifiedLine = false;
	d_validFlags = 0;
}

/***************************************************************************/
/***************************************************************************/
inline CLineToObbIntersection::CLineToObbIntersection(
		CLineCollisionObject &i_line, OBB &i_obb) :
	CCollisionIntersection(LINE_TO_OBB),
	d_line(&i_line),
	d_obb(&i_obb)
{
	d_modifiedLine = false;
	d_validFlags = 0;
}

/***************************************************************************/
/***************************************************************************/
inline CLineToObbIntersection::CLineToObbIntersection(
		CLineCollisionObject &i_line, OBB &i_obb, CCollisionStack &i_stack) :
	CCollisionIntersection(LINE_TO_OBB, i_stack),
	d_line(&i_line),
	d_obb(&i_obb)
{
	d_modifiedLine = false;
	d_validFlags = 0;
}

/***************************************************************************/
/***************************************************************************/
inline CLineToObbIntersection::~CLineToObbIntersection()
{
	if (d_modifiedLine)
		d_line->RestoreFindState(this, d_previousModifier, d_previousPoint);
}

/***************************************************************************/
/***************************************************************************/
inline CLineToObbIntersection* CLineToObbIntersection::New(
		CLineCollisionObject &i_line, OBB &i_obb, CCollisionStack* i_stack)
{
	if (i_stack != NULL)
	{
		CLineToObbIntersection* intersection =
				(CLineToObbIntersection*)i_stack->Allocate(
				sizeof(CLineToObbIntersection));

		if (intersection)
			new(intersection) CLineToObbIntersection(i_line, i_obb, *i_stack);

		return(intersection);
	}

	return(new CLineToObbIntersection(i_line, i_obb));
}

/***************************************************************************/
/***************************************************************************/
inline CLineToObbIntersection& CLineToObbIntersection::operator=(
		CLineToObbIntersection &i_source)
{
	d_line = i_source.d_line;
	d_obb = i_source.d_obb;
	d_validFlags = i_source.d_validFlags;

	if ((d_validFlags & INTERSECTION_TEST) != 0)
	{
		d_intersects = i_source.d_intersects;
		d_enters = i_source.d_enters;
		d_exits = i_source.d_exits;
		d_entryFace = i_source.d_entryFace;
		d_entryFrac = i_source.d_entryFrac;
		d_exitFace = i_source.d_exitFace;
		d_exitFrac = i_source.d_exitFrac;
	}

	if ((d_validFlags & ENTRY_POINT) != 0)
		d_entryPoint = i_source.d_entryPoint;

	if ((d_validFlags & ENTRY_NORMAL) != 0)
		d_entryNormal = i_source.d_entryNormal;

	if ((d_validFlags & EXIT_POINT) != 0)
		d_exitPoint = i_source.d_exitPoint;

	if ((d_validFlags & EXIT_NORMAL) != 0)
		d_exitNormal = i_source.d_exitNormal;

	d_modifiedLine = false;

	return(*this);
}

/***************************************************************************/
/***************************************************************************/
inline void CLineToObbIntersection::ClearTests(void)
{
	if (d_modifiedLine)
		d_line->RestoreFindState(this, d_previousModifier, d_previousPoint);

	d_modifiedLine = false;
	d_validFlags = 0;
}

/***************************************************************************/
/***************************************************************************/
inline void CLineToObbIntersection::Set(CLineCollisionObject* i_line,
		OBB* i_obb)
{
	ASSERT((i_line != NULL) && (i_obb != NULL));
	ClearTests();
	d_line = i_line;
	d_obb = i_obb;
}

/***************************************************************************/
/***************************************************************************/
inline bool CLineToObbIntersection::IntersectionTest(void)
{
	if ((d_validFlags & INTERSECTION_TEST) == 0)
		PerformIntersectionTest();

	return(d_intersects);
}

/***************************************************************************/
/***************************************************************************/
inline bool CLineToObbIntersection::Enters(void)
{
	if ((d_validFlags & INTERSECTION_TEST) == 0)
		PerformIntersectionTest();
	return(d_enters);
}

/***************************************************************************/
/***************************************************************************/
inline uint CLineToObbIntersection::EntryFace(void)
{
	if ((d_validFlags & INTERSECTION_TEST) == 0)
		PerformIntersectionTest();
	return(d_entryFace);
}

/***************************************************************************/
/***************************************************************************/
inline float CLineToObbIntersection::EntryFrac(void)
{
	if ((d_validFlags & INTERSECTION_TEST) == 0)
		PerformIntersectionTest();
	return(d_entryFrac);
}

/***************************************************************************/
/***************************************************************************/
inline Vector3CRef CLineToObbIntersection::EntryPoint(void)
{
	if ((d_validFlags & ENTRY_POINT) == 0)
	{
		if ((d_validFlags & INTERSECTION_TEST) == 0)
			PerformIntersectionTest();

		if (d_intersects)
			d_line->ComputePoint(d_entryFrac, d_entryPoint);
		else
			d_entryPoint.Clear();

		d_validFlags |= ENTRY_POINT;
	}

	return(d_entryPoint);
}

/***************************************************************************/
/***************************************************************************/
inline Vector3CRef CLineToObbIntersection::EntryNormal(void)
{
	if ((d_validFlags & ENTRY_NORMAL) == 0)
	{
		if ((d_validFlags & INTERSECTION_TEST) == 0)
			PerformIntersectionTest();

		if (d_intersects)
			d_obb->GetFaceNormal(d_entryFace, d_entryNormal);
		else
			d_entryNormal.Set(0.0f, 1.0f, 0.0f);

		d_validFlags |= ENTRY_NORMAL;
	}

	return(d_entryNormal);
}

/***************************************************************************/
/***************************************************************************/
inline bool CLineToObbIntersection::Exits(void)
{
	if ((d_validFlags & INTERSECTION_TEST) == 0)
		PerformIntersectionTest();
	return(d_exits);
}

/***************************************************************************/
/***************************************************************************/
inline uint CLineToObbIntersection::ExitFace(void)
{
	if ((d_validFlags & INTERSECTION_TEST) == 0)
		PerformIntersectionTest();
	return(d_exitFace);
}

/***************************************************************************/
/***************************************************************************/
inline float CLineToObbIntersection::ExitFrac(void)
{
	if ((d_validFlags & INTERSECTION_TEST) == 0)
		PerformIntersectionTest();
	return(d_exitFrac);
}

/***************************************************************************/
/***************************************************************************/
inline Vector3CRef CLineToObbIntersection::ExitPoint(void)
{
	if ((d_validFlags & EXIT_POINT) == 0)
	{
		if ((d_validFlags & INTERSECTION_TEST) == 0)
			PerformIntersectionTest();

		if (d_intersects)
			d_line->ComputePoint(d_exitFrac, d_exitPoint);
		else
			d_exitPoint.Clear();

		d_validFlags |= EXIT_POINT;
	}

	return(d_exitPoint);
}

/***************************************************************************/
/***************************************************************************/
inline Vector3CRef CLineToObbIntersection::ExitNormal(void)
{
	if ((d_validFlags & EXIT_NORMAL) == 0)
	{
		if ((d_validFlags & INTERSECTION_TEST) == 0)
			PerformIntersectionTest();

		if (d_intersects)
			d_obb->GetFaceNormal(d_exitFace, d_exitNormal);
		else
			d_exitNormal.Set(0.0f, 1.0f, 0.0f);

		d_validFlags |= EXIT_NORMAL;
	}

	return(d_exitNormal);
}

/***************************************************************************/
/***************************************************************************/
inline CCollisionIntersection* CLineToObbIntersection::TestForIntersection(
		CLineCollisionObject* i_line, OBB* i_obb, CCollisionStack* i_stack)
{
	CLineToObbIntersection* intersection = CLineToObbIntersection::New(
			*i_line, *i_obb, i_stack);
	if (intersection)
	{
		if (intersection->IntersectionTest())
			return(intersection);
		CCollisionStackItem::Delete(intersection);
	}
	return(NULL);
}

#endif // LINE_TO_OBB_HPP
