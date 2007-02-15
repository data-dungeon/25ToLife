/*
**
**  File:   LineEntity.cpp
**  Date:   March 11, 2003
**  By:     Bryant Collard
**  Desc:   Stage entities that serve up line collision objects.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/CollideHelper/LineEntity.cpp $
**  $Revision: 6 $
**      $Date: 7/24/03 4:41p $
**    $Author: Bcollard $
**
*/

#include "CollideHelper/CollideHelperPCH.h"

// My header (always first) //
#include "CollideHelper/LineEntity.h"

/**********************************************************************/
/**********************************************************************/
void CLineEntity::BeginQuery(void)
{
/*
 *	Make sure the line has a valid basis vector.
 */
	if (Math::Zero(d_line.Basis().LengthSquared()))
	{
		d_line.Basis().Set(0.0f, 0.0f, 1.0f);
		d_line.SetBoundary(CLineCollisionObject::BOUNDARY_MIN_MAX, 0.0f, 0.0f);
	}
/*
 *	Make sure the line object is a segment.
 */
	if (!d_line.TestMinBoundary())
	{
		d_line.SetMinBoundary(-2.0f * REALLYLARGEY);
		d_line.BitwiseSetBoundaryCondition(CLineCollisionObject::BOUNDARY_MIN);
	}
	if (!d_line.TestMaxBoundary())
	{
		d_line.SetMaxBoundary(2.0f * REALLYLARGEY);
		d_line.BitwiseSetBoundaryCondition(CLineCollisionObject::BOUNDARY_MAX);
	}
/*
 *	Prep the line.
 */
	d_line.InitializeFindMode();
}

/**********************************************************************/
/**********************************************************************/
void CLineEntity::EndQuery(void)
{
	d_line.TerminateFindMode();
}

/**********************************************************************/
/**********************************************************************/
bool CLineEntity::GetCollisionCull(CCollisionCull &o_cull)
{
	o_cull.SelectStart() = d_line.ComputePoint(d_line.MinBoundary());
	o_cull.SelectChord() = (d_line.MaxBoundary() - d_line.MinBoundary()) *
			d_line.ConstBasis();
	return(true);
}

/***************************************************************************/
/***************************************************************************/
const char* CLineEntity::GetClassName(void)
{
	return("Line");
}
