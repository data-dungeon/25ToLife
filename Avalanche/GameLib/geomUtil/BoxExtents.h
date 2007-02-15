/*
**
**  File:   BoxExtents.h
**  Date:   July 16, 2004
**  By:     Bryant Collard
**  Desc:   A utility that finds the extreme corners of a box.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/collide/Node.h $
**  $Revision: #1 $
**      $Date: 2004/07/20 $
**    $Author: Bryant_Collard $
**
*/

#ifndef BOX_EXTENTS_H
#define BOX_EXTENTS_H

#include "Math/Vector.h"
#include "Math/Matrix.h"

class CBoxExtents
{
  public:
	// Construction.
	CBoxExtents();

	// Initialize member variables. This sets the reference to target
	// transformation to identity and resets the min and max values.
	void Initialize(void);

	// Reset the min and max values but leave the reference to target
	// transformation alone.
	void Reset(void);

	// Information about the box of interest is expressed in some reference
	// system. It may be desirable to find the resulting extents in some other
	// target system. These routines provide the ability to specify a
	// transformation to that target system.
	void SetRefToTarget(const Graphics4x4 &i_refToTarget);
	void SetRefToTarget(Vector3CRef i_refInTarget,
			const Matrix3x3 &i_refToTarget);
	void SetTargetToRef(const Graphics4x4 &i_targetToRef);
	void SetTargetToRef(Vector3CRef i_targetInRef,
			const Matrix3x3 &i_targetToRef);
	void SetTargetToRef(const Cartesian4x4 &i_targetToRef);
	void SetTargetToRef(Vector3CRef i_targetInRef,
			const DirCos3x3 &i_targetToRef);

	// Describe a box and find its extents in the target system.
	static void FindExtents(Vector3CRef i_halfSize, const Graphics4x4 &i_boxToRef,
			Vector3 &o_minExtent, Vector3 &o_maxExtent,
			const Graphics4x4 &i_refToTarget);
	static void FindExtents(Vector3CRef i_halfSize, Vector3CRef i_boxInRef,
			const Matrix3x3 &i_boxToRef, Vector3 &o_minExtent,
			Vector3 &o_maxExtent, const Graphics4x4 &i_refToTarget);

	// Same thing but in the reference system.
	static void FindExtents(Vector3CRef i_halfSize, const Graphics4x4 &i_boxToRef,
			Vector3 &o_minExtent, Vector3 &o_maxExtent);
	static void FindExtents(Vector3CRef i_halfSize, Vector3CRef i_boxInRef,
			const Matrix3x3 &i_boxToRef, Vector3 &o_minExtent,
			Vector3 &o_maxExtent);

	// Describe a box and find its extents in the target system, non-static.
	void FindExtentsInTarget(Vector3CRef i_halfSize,
			const Graphics4x4 &i_boxToRef, Vector3 &o_minExtent,
			Vector3 &o_maxExtent);
	void FindExtentsInTarget(Vector3CRef i_halfSize, Vector3CRef i_boxInRef,
			const Matrix3x3 &i_boxToRef, Vector3 &o_minExtent,
			Vector3 &o_maxExtent);

	// Same thing but in the reference system, non-static.
	void FindExtentsInRef(Vector3CRef i_halfSize, const Graphics4x4 &i_boxToRef,
			Vector3 &o_minExtent, Vector3 &o_maxExtent);
	void FindExtentsInRef(Vector3CRef i_halfSize, Vector3CRef i_boxInRef,
			const Matrix3x3 &i_boxToRef, Vector3 &o_minExtent,
			Vector3 &o_maxExtent);

	// Describe a box and find its extents in the target system and compare them
	// to tests since the last reset to find the collective extents. Note that
	// all tests must be performed in the same coordnate system.
	void TestExtentsInTarget(Vector3CRef i_halfSize,
			const Graphics4x4 &i_boxToRef, Vector3 &o_minExtent,
			Vector3 &o_maxExtent);
	void TestExtentsInTarget(Vector3CRef i_halfSize, Vector3CRef i_boxInRef,
			const Matrix3x3 &i_boxToRef, Vector3 &o_minExtent,
			Vector3 &o_maxExtent);
	void TestExtentsInTarget(Vector3CRef i_halfSize,
			const Graphics4x4 &i_boxToRef);
	void TestExtentsInTarget(Vector3CRef i_halfSize, Vector3CRef i_boxInRef,
			const Matrix3x3 &i_boxToRef);

	// Same thing but in the reference system.
	void TestExtentsInRef(Vector3CRef i_halfSize, const Graphics4x4 &i_boxToRef,
			Vector3 &o_minExtent, Vector3 &o_maxExtent);
	void TestExtentsInRef(Vector3CRef i_halfSize, Vector3CRef i_boxInRef,
			const Matrix3x3 &i_boxToRef, Vector3 &o_minExtent,
			Vector3 &o_maxExtent);
	void TestExtentsInRef(Vector3CRef i_halfSize, const Graphics4x4 &i_boxToRef);
	void TestExtentsInRef(Vector3CRef i_halfSize, Vector3CRef i_boxInRef,
			const Matrix3x3 &i_boxToRef);

	// Actually perform the extent test.
	void TestExtents(Vector3CRef i_min, Vector3CRef i_max);

	// Get the extremes of all the tests since the last reset. If no tests have
	// been perfomed, the min extent will be as large as possible and the max
	// extent will be as small (large negative) as possible. Note that the min
	// extents will be larger than the max extents. Also, if tests in both
	// coordinate systems have been performed, the results are invalid.
	Vector3CRef GetMinExtent(void) {return(d_minExtent);}
	Vector3CRef GetMaxExtent(void) {return(d_maxExtent);}

  private:
	Graphics4x4 d_refToTarget;
	Vector3 d_minExtent;
	Vector3 d_maxExtent;
};

#endif // BOX_EXTENTS_H
