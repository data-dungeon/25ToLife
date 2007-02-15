/*
**
**  File:   Intersection.hpp
**  Date:   January 31, 2003
**  By:     Bryant Collard
**  Desc:   A base class for intersection testing.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/becCollide/Intersection.hpp $
**  $Revision: 2 $
**      $Date: 3/11/03 7:53p $
**    $Author: Bcollard $
**
*/

#ifdef INTERSECTION_HPP
#undef INTERSECTION_HPP

/**********************************************************************
 **********************************************************************/
inline CCollisionIntersection::CCollisionIntersection(
		ECollisionIntersectionType i_type) : d_type(i_type)
{
}

/**********************************************************************
 **********************************************************************/
inline CCollisionIntersection::CCollisionIntersection(
		ECollisionIntersectionType i_type, CCollisionStack &i_stack) :
	CCollisionStackItem(i_stack),
	d_type(i_type)
{
}

#endif // INTERSECTION_HPP
