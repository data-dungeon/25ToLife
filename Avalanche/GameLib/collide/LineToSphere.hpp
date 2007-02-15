/*
**
**  File:   LineToSphere.hpp
**  Date:   May 1, 2003
**  By:     Bryant Collard
**  Desc:   Tests intersections between lines and spheres.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/collide/LineToSphere.hpp $
**  $Revision: 1 $
**      $Date: 5/05/03 3:12p $
**    $Author: Bcollard $
**
*/

#ifdef LINE_TO_SPHERE_HPP
#undef LINE_TO_SPHERE_HPP

// System includes //
#include <new.h>

// Gamelib includes //
#include "collide/Stack.h"
#include "Layers/Assert/Assert.h"

/***************************************************************************/
/***************************************************************************/
inline CLineToSphereIntersection::CLineToSphereIntersection() :
	CCollisionIntersection(LINE_TO_SPHERE),
	d_line(NULL),
	d_sphere(NULL)
{
	d_modifiedLine = false;
	d_validFlags = 0;
}

/***************************************************************************/
/***************************************************************************/
inline CLineToSphereIntersection::CLineToSphereIntersection(
		CLineCollisionObject &i_line, CSphereCollisionObject &i_sphere) :
	CCollisionIntersection(LINE_TO_SPHERE),
	d_line(&i_line),
	d_sphere(&i_sphere)
{
	d_modifiedLine = false;
	d_validFlags = 0;
}

/***************************************************************************/
/***************************************************************************/
inline CLineToSphereIntersection::CLineToSphereIntersection(
		CLineCollisionObject &i_line, CSphereCollisionObject &i_sphere,
		CCollisionStack &i_stack) :
	CCollisionIntersection(LINE_TO_SPHERE, i_stack),
	d_line(&i_line),
	d_sphere(&i_sphere)
{
	d_modifiedLine = false;
	d_validFlags = 0;
}

/***************************************************************************/
/***************************************************************************/
inline CLineToSphereIntersection::~CLineToSphereIntersection()
{
	if (d_modifiedLine)
		d_line->RestoreFindState(this, d_previousModifier, d_previousPoint);
}

/***************************************************************************/
/***************************************************************************/
inline CLineToSphereIntersection* CLineToSphereIntersection::New(
		CLineCollisionObject &i_line, CSphereCollisionObject &i_sphere,
		CCollisionStack* i_stack)
{
	if (i_stack != NULL)
	{
		CLineToSphereIntersection* intersection =
				(CLineToSphereIntersection*)i_stack->Allocate(
				sizeof(CLineToSphereIntersection));

		if (intersection)
			new(intersection) CLineToSphereIntersection(i_line, i_sphere,
					*i_stack);

		return(intersection);
	}

	return(new CLineToSphereIntersection(i_line, i_sphere));
}

/***************************************************************************/
/***************************************************************************/
inline CLineToSphereIntersection& CLineToSphereIntersection::operator=(
		CLineToSphereIntersection &i_source)
{
	d_line = i_source.d_line;
	d_sphere = i_source.d_sphere;
	d_validFlags = i_source.d_validFlags;

	if ((d_validFlags & INTERSECTION_TEST) != 0)
	{
		d_intersects = i_source.d_intersects;
		d_enters = i_source.d_enters;
		d_exits = i_source.d_exits;
		d_entryFrac = i_source.d_entryFrac;
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
inline void CLineToSphereIntersection::ClearTests(void)
{
	if (d_modifiedLine)
		d_line->RestoreFindState(this, d_previousModifier, d_previousPoint);

	d_modifiedLine = false;
	d_validFlags = 0;
}

/***************************************************************************/
/***************************************************************************/
inline void CLineToSphereIntersection::Set(CLineCollisionObject* i_line,
		CSphereCollisionObject* i_sphere)
{
	ASSERT((i_line != NULL) && (i_sphere != NULL));
	ClearTests();
	d_line = i_line;
	d_sphere = i_sphere;
}

/***************************************************************************/
/***************************************************************************/
inline bool CLineToSphereIntersection::IntersectionTest(void)
{
	if ((d_validFlags & INTERSECTION_TEST) == 0)
		PerformIntersectionTest();

	return(d_intersects);
}

/***************************************************************************/
/***************************************************************************/
inline bool CLineToSphereIntersection::Enters(void)
{
	if ((d_validFlags & INTERSECTION_TEST) == 0)
		PerformIntersectionTest();
	return(d_enters);
}

/***************************************************************************/
/***************************************************************************/
inline float CLineToSphereIntersection::EntryFrac(void)
{
	if ((d_validFlags & INTERSECTION_TEST) == 0)
		PerformIntersectionTest();
	return(d_entryFrac);
}

/***************************************************************************/
/***************************************************************************/
inline const Vector3& CLineToSphereIntersection::EntryPoint(void)
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
inline const Vector3& CLineToSphereIntersection::EntryNormal(void)
{
	if ((d_validFlags & ENTRY_NORMAL) == 0)
	{
		d_validFlags |= ENTRY_NORMAL;

		if ((d_validFlags & ENTRY_POINT) == 0)
		{
			d_validFlags |= ENTRY_POINT;

			if ((d_validFlags & INTERSECTION_TEST) == 0)
				PerformIntersectionTest();

			if (d_intersects)
			{
				d_line->ComputePoint(d_entryFrac, d_entryPoint);
			}
			else
			{
				d_entryPoint.Clear();
				d_entryNormal.Set(0.0f, 1.0f, 0.0f);
				return(d_entryNormal);
			}
		}
		else if (!d_intersects)
		{
			d_entryNormal.Set(0.0f, 1.0f, 0.0f);
			return(d_entryNormal);
		}

		d_entryNormal = (d_entryPoint - d_sphere->Center()) / d_sphere->Radius();
	}

	return(d_entryNormal);
}

/***************************************************************************/
/***************************************************************************/
inline bool CLineToSphereIntersection::Exits(void)
{
	if ((d_validFlags & INTERSECTION_TEST) == 0)
		PerformIntersectionTest();
	return(d_exits);
}

/***************************************************************************/
/***************************************************************************/
inline float CLineToSphereIntersection::ExitFrac(void)
{
	if ((d_validFlags & INTERSECTION_TEST) == 0)
		PerformIntersectionTest();
	return(d_exitFrac);
}

/***************************************************************************/
/***************************************************************************/
inline const Vector3& CLineToSphereIntersection::ExitPoint(void)
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
inline const Vector3& CLineToSphereIntersection::ExitNormal(void)
{
	if ((d_validFlags & EXIT_NORMAL) == 0)
	{
		d_validFlags |= EXIT_NORMAL;

		if ((d_validFlags & EXIT_POINT) == 0)
		{
			d_validFlags |= EXIT_POINT;

			if ((d_validFlags & INTERSECTION_TEST) == 0)
				PerformIntersectionTest();

			if (d_intersects)
			{
				d_line->ComputePoint(d_exitFrac, d_exitPoint);
			}
			else
			{
				d_exitPoint.Clear();
				d_exitNormal.Set(0.0f, 1.0f, 0.0f);
				return(d_exitNormal);
			}
		}
		else if (!d_intersects)
		{
			d_exitNormal.Set(0.0f, 1.0f, 0.0f);
			return(d_exitNormal);
		}

		d_exitNormal = (d_exitPoint - d_sphere->Center()) / d_sphere->Radius();
	}

	return(d_exitNormal);
}

/***************************************************************************/
/***************************************************************************/
inline CCollisionIntersection* CLineToSphereIntersection::TestForIntersection(
		CLineCollisionObject* i_line, CSphereCollisionObject* i_sphere,
		CCollisionStack* i_stack)
{
	CLineToSphereIntersection* intersection = CLineToSphereIntersection::New(
			*i_line, *i_sphere, i_stack);
	if (intersection)
	{
		if (intersection->IntersectionTest())
			return(intersection);
		CCollisionStackItem::Delete(intersection);
	}
	return(NULL);
}

#endif // LINE_TO_SPHERE_HPP
