/*
**
**  File:   LineToTriangleList.hpp
**  Date:   August 3, 2004
**  By:     Bryant Collard
**  Desc:   Tests intersections between lines and triangle lists.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/collide/LineToTriangleList.hpp $
**  $Revision: #2 $
**  $DateTime: 2005/03/29 17:16:52 $
**    $Author: Rob $
**
*/

#ifdef LINE_TO_TRIANGLE_LIST_HPP
#undef LINE_TO_TRIANGLE_LIST_HPP

// System includes //
#include <new.h>

// Gamelib includes //
#include "collide/Stack.h"
#include "collide/StackArray.h"

/******************************************************************************/
/******************************************************************************/
inline CLineToTriangleListIntersection::CLineToTriangleListIntersection(
		CLineCollisionObject &i_line,
		CTriangleListCollisionObject &i_triangleList) :
	CCollisionIntersection(LINE_TO_TRIANGLE_LIST),
	d_line(i_line),
	d_triangleList(i_triangleList)
{
	d_foundPoint.d_triangle = NULL;
	d_pointList = NULL;
}

/***************************************************************************/
/***************************************************************************/
inline CLineToTriangleListIntersection::CLineToTriangleListIntersection(
		CLineCollisionObject &i_line,
		CTriangleListCollisionObject &i_triangleList,
		CCollisionStack &i_stack) :
	CCollisionIntersection(LINE_TO_TRIANGLE_LIST, i_stack),
	d_line(i_line),
	d_triangleList(i_triangleList)
{
	d_foundPoint.d_triangle = NULL;
	d_pointList = NULL;
}

/******************************************************************************/
/******************************************************************************/
inline CLineToTriangleListIntersection::~CLineToTriangleListIntersection()
{
	if (d_foundPoint.d_triangle != NULL)
		d_line.RestoreFindState(this, d_previousModifier, d_previousPoint);
	CCollisionStackItem::Delete(d_pointList);
}

/******************************************************************************/
/******************************************************************************/
inline CLineToTriangleListIntersection* CLineToTriangleListIntersection::New(
		CLineCollisionObject &i_line,
		CTriangleListCollisionObject &i_triangleList, CCollisionStack* i_stack)
{
	if (i_stack != NULL)
	{
		CLineToTriangleListIntersection* intersection =
				(CLineToTriangleListIntersection*)i_stack->Allocate(
				sizeof(CLineToTriangleListIntersection));

		if (intersection)
		{
			new(intersection) CLineToTriangleListIntersection(i_line,
					i_triangleList, *i_stack);
		}

		return(intersection);
	}
	return(new CLineToTriangleListIntersection(i_line, i_triangleList));
}

/******************************************************************************/
/******************************************************************************/
inline CLineToTriangleListPoint*
		CLineToTriangleListIntersection::GetIntersection(int i_index)
{
	if ((d_pointList == NULL) || (i_index < 0) ||
			(i_index >= d_pointList->NumEntries()))
		return(NULL);

	return(d_pointList->EntryPtr(i_index));
}

/******************************************************************************/
/******************************************************************************/
inline CLineToTriangleListPoint*
		CLineToTriangleListIntersection::GetFoundPoint(void)
{
	if (d_foundPoint.d_triangle == NULL)
		return(NULL);

	return(&d_foundPoint);
}

/******************************************************************************/
/******************************************************************************/
inline CCollisionIntersection*
		CLineToTriangleListIntersection::TestForIntersection(
		CLineCollisionObject* i_line,
		CTriangleListCollisionObject* i_triangleList, CCollisionStack* i_stack)
{
	CLineToTriangleListIntersection* intersection =
			(CLineToTriangleListIntersection*)CLineToTriangleListIntersection::New(
			*i_line, *i_triangleList, i_stack);
	if (intersection)
	{
		bool hit = intersection->IntersectionTest();
		if (hit)
			return(intersection);
		CCollisionStackItem::Delete(intersection);
	}
	return(NULL);
}

#endif // LINE_TO_TRIANGLE_LIST_HPP
