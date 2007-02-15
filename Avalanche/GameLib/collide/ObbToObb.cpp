/*
**
**  File:   ObbToObb.hpp
**  Date:   February 4, 2003
**  By:     Bryant Collard
**  Desc:   Tests intersections between OBBs.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/collide/ObbToObb.cpp $
**  $Revision: 8 $
**      $Date: 7/25/03 11:41a $
**    $Author: Bcollard $
**
*/

#include "collide/CollidePCH.h"

// My header (always first) //
#include "collide/ObbToObb.h"

#if 0

// Engine includes //
#include "Math/MathClass.h"
#include "Math/Matrix.h"
#include "Math/Vector.h"

// Gamelib includes //
#include "geomUtil/planefit.h"

#endif

// Face bits.
#define NEG_X_BIT 0x01
#define POS_X_BIT 0x02
#define NEG_Y_BIT 0x04
#define POS_Y_BIT 0x08
#define NEG_Z_BIT 0x10
#define POS_Z_BIT 0x20

// Valid data masks.
const uint CObbToObbIntersection::RADIUS_TEST             = 0x01;
const uint CObbToObbIntersection::SEPARATING_AXIS_TEST    = 0x02;
const uint CObbToObbIntersection::CLOSEST_SEPARATING_AXIS = 0x04;
const uint CObbToObbIntersection::TRANSFORM               = 0x08;
const uint CObbToObbIntersection::INTERSECTION_POINTS     = 0x10;
const uint CObbToObbIntersection::INTERSECTION_NORMAL_SUM = 0x20;
const uint CObbToObbIntersection::INTERSECTION_PLANE      = 0x40;

// Map edge ids to face masks.
const uint16 CObbToObbIntersection::EDGE_MAP[12] =
{
	NEG_Y_BIT | NEG_Z_BIT,
	POS_X_BIT | NEG_Y_BIT,
	NEG_Y_BIT | POS_Z_BIT,
	NEG_X_BIT | NEG_Y_BIT,
	POS_Y_BIT | NEG_Z_BIT,
	POS_X_BIT | POS_Y_BIT,
	POS_Y_BIT | POS_Z_BIT,
	NEG_X_BIT | POS_Y_BIT,
	NEG_X_BIT | NEG_Z_BIT,
	POS_X_BIT | NEG_Z_BIT,
	NEG_X_BIT | POS_Z_BIT,
	POS_X_BIT | POS_Z_BIT
};

/***************************************************************************/
// If the collision isn't trivally rejected, do the separating
// axis test.  This is reasonably fast if they objects don't
// collide.  If this test passes the boxes are touching.
/***************************************************************************/
bool CObbToObbIntersection::SeparatingAxisTest(void)
{
	ASSERT((d_obbA != NULL) && (d_obbB != NULL));

	if ((d_validFlags & SEPARATING_AXIS_TEST) != 0)
		return(d_noSeparatingPlane);
	d_validFlags |= SEPARATING_AXIS_TEST;
	d_noSeparatingPlane = false;

	// convenience variables
	Vector3CRef aextent = d_obbA->GetConstAABB().maxV;
	Vector3CRef bextent = d_obbB->GetConstAABB().maxV;

	// Please mister compiler-man optimize this crap out
	Vector3 ax, ay, az;
	ax = d_obbA->Matrix()->GetRow0();	ay = d_obbA->Matrix()->GetRow1();	az = d_obbA->Matrix()->GetRow2();
	Vector3 bx, by, bz;
	bx = d_obbB->Matrix()->GetRow0();	by = d_obbB->Matrix()->GetRow1();	bz = d_obbB->Matrix()->GetRow2();

	// compute difference of box centers, D = C1-C0
	Vector3 d = *(d_obbA->Center()) - *(d_obbB->Center());

	Matrix3x3 c;			// matrix C = A^T B, c_{ij} = Dot(A_i,B_j)
	Matrix3x3 absC;		// |c_{ij}|
	Vector3 ad;          // Dot(A_i,D)
	float r0, r1, r;		// interval radii and distance between centers

	// axis C0+t*A0
	c.e00 = ax.Dot(bx);
	c.e01 = ax.Dot(by);
	c.e02 = ax.Dot(bz);
	ad.x( ax.Dot(d));
	absC.e00 = Math::Abs(c.e00);
	absC.e01 = Math::Abs(c.e01);
	absC.e02 = Math::Abs(c.e02);
	r = Math::Abs(ad.x());
	r1 = bextent.x() * absC.e00 + bextent.y() * absC.e01 + bextent.z() * absC.e02;
	d_axisOverlap[A0] = aextent.x() + r1 - r;
	if (d_axisOverlap[A0] < 0.0f)
		return(false);

	// axis C0 + t * A1
	c.e10 = ay.Dot(bx);
	c.e11 = ay.Dot(by);
	c.e12 = ay.Dot(bz);
	ad.y( ay.Dot(d));
	absC.e10 = Math::Abs(c.e10);
	absC.e11 = Math::Abs(c.e11);
	absC.e12 = Math::Abs(c.e12);
	r = Math::Abs(ad.y());
	r1 = bextent.x() * absC.e10 + bextent.y() * absC.e11 + bextent.z() * absC.e12;
	d_axisOverlap[A1] = aextent.y() + r1 - r;
	if (d_axisOverlap[A1] < 0.0f)
		return(false);

	// axis C0 + t * A2
	c.e20 = az.Dot(bx);
	c.e21 = az.Dot(by);
	c.e22 = az.Dot(bz);
	ad.z( az.Dot(d));
	absC.e20 = Math::Abs(c.e20);
	absC.e21 = Math::Abs(c.e21);
	absC.e22 = Math::Abs(c.e22);
	r = Math::Abs(ad.z());
	r1 = bextent.x() * absC.e20 + bextent.y() * absC.e21 + bextent.z() * absC.e22;
	d_axisOverlap[A2] = aextent.z() + r1 - r;
	if (d_axisOverlap[A2] < 0.0f)
		return(false);

	// axis C0 + t * B0
	r = Math::Abs(bx.Dot(d));
	r0 = aextent.x() * absC.e00 + aextent.y() * absC.e10 + aextent.z() * absC.e20;
	d_axisOverlap[B0] = r0 + bextent.x() - r;
	if (d_axisOverlap[B0] < 0.0f)
		return(false);

	// axis C0 + t * B1
	r = Math::Abs(by.Dot(d));
	r0 = aextent.x() * absC.e01 + aextent.y() * absC.e11 + aextent.z() * absC.e21;
	d_axisOverlap[B1] = r0 + bextent.y() - r;
	if (d_axisOverlap[B1] < 0.0f)
		return(false);

	// axis C0 + t * B2
	r = Math::Abs(bz.Dot(d));
	r0 = aextent.x() * absC.e02 + aextent.y() * absC.e12 + aextent.z() * absC.e22;
	d_axisOverlap[B2] = r0 + bextent.z() - r;
	if (d_axisOverlap[B2] < 0.0f)
		return(false);

	// axis C0 + t * A0xB0
	r = Math::Abs(ad.z() * c.e10 - ad.y() * c.e20);
	r0 = aextent.y() * absC.e20 + aextent.z() * absC.e10;
	r1 = bextent.y() * absC.e02 + bextent.z() * absC.e01;
	d_axisOverlap[A0XB0] = r0 + r1 - r;
	if (d_axisOverlap[A0XB0] < 0.0f)
		return(false);

	// axis C0 + t * A0xB1
	r = Math::Abs(ad.z() * c.e11 - ad.y() * c.e21);
	r0 = aextent.y() * absC.e21 + aextent.z() * absC.e11;
	r1 = bextent.x() * absC.e02 + bextent.z() * absC.e00;
	d_axisOverlap[A0XB1] = r0 + r1 - r;
	if (d_axisOverlap[A0XB1] < 0.0f)
		return(false);

	// axis C0 + t * A0xB2
	r = Math::Abs(ad.z() * c.e12 - ad.y() * c.e22);
	r0 = aextent.y() * absC.e22 + aextent.z() * absC.e12;
	r1 = bextent.x() * absC.e01 + bextent.y() * absC.e00;
	d_axisOverlap[A0XB2] = r0 + r1 - r;
	if (d_axisOverlap[A0XB2] < 0.0f)
		return(false);

	// axis C0 + t * A1xB0
	r = Math::Abs(ad.x() * c.e20 - ad.z() * c.e00);
	r0 = aextent.x() * absC.e20 + aextent.z() * absC.e00;
	r1 = bextent.y() * absC.e12 + bextent.z() * absC.e11;
	d_axisOverlap[A1XB0] = r0 + r1 - r;
	if (d_axisOverlap[A1XB0] < 0.0f)
		return(false);

	// axis C0 + t * A1xB1
	r = Math::Abs(ad.x() * c.e21 - ad.z() * c.e01);
	r0 = aextent.x() * absC.e21 + aextent.z() * absC.e01;
	r1 = bextent.x() * absC.e12 + bextent.z() * absC.e10;
	d_axisOverlap[A1XB1] = r0 + r1 - r;
	if (d_axisOverlap[A1XB1] < 0.0f)
		return(false);

	// axis C0 + t * A1xB2
	r = Math::Abs(ad.x() * c.e22 - ad.z() * c.e02);
	r0 = aextent.x() * absC.e22 + aextent.z() * absC.e02;
	r1 = bextent.x() * absC.e11 + bextent.y() * absC.e10;
	d_axisOverlap[A1XB2] = r0 + r1 - r;
	if (d_axisOverlap[A1XB2] < 0.0f)
		return(false);

	// axis C0 + t * A2xB0
	r = Math::Abs(ad.y() * c.e00 - ad.x() * c.e10);
	r0 = aextent.x() * absC.e10 + aextent.y() * absC.e00;
	r1 = bextent.y() * absC.e22 + bextent.z() * absC.e21;
	d_axisOverlap[A2XB0] = r0 + r1 - r;
	if (d_axisOverlap[A2XB0] < 0.0f)
		return(false);

	// axis C0 + t * A2xB1
	r = Math::Abs(ad.y() * c.e01 - ad.x() * c.e11);
	r0 = aextent.x() * absC.e11 + aextent.y() * absC.e01;
	r1 = bextent.x() * absC.e22 + bextent.z() * absC.e20;
	d_axisOverlap[A2XB1] = r0 + r1 - r;
	if (d_axisOverlap[A2XB1] < 0.0f)
		return(false);

	// axis C0 + t * A2xB2
	r = Math::Abs(ad.y() * c.e02 - ad.x() * c.e12);
	r0 = aextent.x() * absC.e12 + aextent.y() * absC.e02;
	r1 = bextent.x() * absC.e21 + bextent.y() * absC.e20;
	d_axisOverlap[A2XB2] = r0 + r1 - r;
	if (d_axisOverlap[A2XB2] < 0.0f)
		return(false);

	d_noSeparatingPlane = true;

	d_aToB.Set(c);
	d_aToB.PokeTranslation(bx.Dot(d), by.Dot(d), bz.Dot(d));
	d_validFlags |= TRANSFORM;

	return(true);
}

/***************************************************************************/
/***************************************************************************/
bool CObbToObbIntersection::ClosestSeparatingAxis(Vector3 &o_axis)
{
	ASSERT((d_obbA != NULL) && (d_obbB != NULL));

	if (!RadiusTest() || !SeparatingAxisTest())
		return(false);

	if ((d_validFlags & CLOSEST_SEPARATING_AXIS) == 0)
	{
		int i;
		int closestAxis = A0;
		for (i = A1; i <= B2; i++)
		{
			if (d_axisOverlap[i] < d_axisOverlap[closestAxis])
				closestAxis = i;
		}

		float closestLengthSq = 1.0f;
		float closestOverlapSq = d_axisOverlap[closestAxis] *
				d_axisOverlap[closestAxis];

		CompareSeparatingAxes(A0XB0, d_aToB.e10, d_aToB.e20, closestAxis,
				closestLengthSq, closestOverlapSq);
		CompareSeparatingAxes(A0XB1, d_aToB.e11, d_aToB.e21, closestAxis,
				closestLengthSq, closestOverlapSq);
		CompareSeparatingAxes(A0XB2, d_aToB.e12, d_aToB.e22, closestAxis,
				closestLengthSq, closestOverlapSq);

		CompareSeparatingAxes(A1XB0, d_aToB.e00, d_aToB.e20, closestAxis,
				closestLengthSq, closestOverlapSq);
		CompareSeparatingAxes(A1XB1, d_aToB.e01, d_aToB.e21, closestAxis,
				closestLengthSq, closestOverlapSq);
		CompareSeparatingAxes(A1XB2, d_aToB.e02, d_aToB.e22, closestAxis,
				closestLengthSq, closestOverlapSq);

		CompareSeparatingAxes(A2XB0, d_aToB.e00, d_aToB.e10, closestAxis,
				closestLengthSq, closestOverlapSq);
		CompareSeparatingAxes(A2XB1, d_aToB.e01, d_aToB.e11, closestAxis,
				closestLengthSq, closestOverlapSq);
		CompareSeparatingAxes(A2XB2, d_aToB.e02, d_aToB.e12, closestAxis,
				closestLengthSq, closestOverlapSq);

		switch(closestAxis)
		{
		 case A0:
			d_closestSeparatingAxis = d_obbA->Matrix()->GetRow0();
			break;
		 case A1:
			d_closestSeparatingAxis = d_obbA->Matrix()->GetRow1();
			break;
		 case A2:
			d_closestSeparatingAxis = d_obbA->Matrix()->GetRow2();
			break;
		 case B0:
			d_closestSeparatingAxis = d_obbB->Matrix()->GetRow0();
			d_closestSeparatingAxis.Negate();
			break;
		 case B1:
			d_closestSeparatingAxis = d_obbB->Matrix()->GetRow1();
			d_closestSeparatingAxis.Negate();
			break;
		 case B2:
			d_closestSeparatingAxis = d_obbB->Matrix()->GetRow2();
			d_closestSeparatingAxis.Negate();
			break;
		 case A0XB0:
		 {
			Vector3 a0, b0;
			a0 = d_obbA->Matrix()->GetRow0();
			b0 = d_obbB->Matrix()->GetRow0();
			d_closestSeparatingAxis = a0.Cross(b0);
			break;
		 }
		 case A0XB1:
		 {
			Vector3 a0, b1;
			a0 = d_obbA->Matrix()->GetRow0();
			b1 = d_obbB->Matrix()->GetRow1();
			d_closestSeparatingAxis = a0.Cross(b1);
			break;
		 }
		 case A0XB2:
		 {
			Vector3 a0, b2;
			a0 = d_obbA->Matrix()->GetRow0();
			b2 = d_obbB->Matrix()->GetRow2();
			d_closestSeparatingAxis = a0.Cross(b2);
			break;
		 }
		 case A1XB0:
		 {
			Vector3 a1, b0;
			a1 = d_obbA->Matrix()->GetRow1();
			b0 = d_obbB->Matrix()->GetRow0();
			d_closestSeparatingAxis = a1.Cross(b0);
			break;
		 }
		 case A1XB1:
		 {
			Vector3 a1, b1;
			a1 = d_obbA->Matrix()->GetRow1();
			b1 = d_obbB->Matrix()->GetRow1();
			d_closestSeparatingAxis = a1.Cross(b1);
			break;
		 }
		 case A1XB2:
		 {
			Vector3 a1, b2;
			a1 = d_obbA->Matrix()->GetRow1();
			b2 = d_obbB->Matrix()->GetRow2();
			d_closestSeparatingAxis = a1.Cross(b2);
			break;
		 }
		 case A2XB0:
		 {
			Vector3 a2, b0;
			a2 = d_obbA->Matrix()->GetRow2();
			b0 = d_obbB->Matrix()->GetRow0();
			d_closestSeparatingAxis = a2.Cross(b0);
			break;
		 }
		 case A2XB1:
		 {
			Vector3 a2, b1;
			a2 = d_obbA->Matrix()->GetRow2();
			b1 = d_obbB->Matrix()->GetRow1();
			d_closestSeparatingAxis = a2.Cross(b1);
			break;
		 }
		 case A2XB2:
		 {
			Vector3 a2, b2;
			a2 = d_obbA->Matrix()->GetRow2();
			b2 = d_obbB->Matrix()->GetRow2();
			d_closestSeparatingAxis = a2.Cross(b2);
			break;
		 }
		}

		float closestLength = Math::Sqrt(closestLengthSq);
		if (!Math::DivByZero(d_axisOverlap[closestAxis], closestLength))
			d_closestSeparatingAxis *= d_axisOverlap[closestAxis] / closestLength;
		else
			d_closestSeparatingAxis.Clear();
		d_validFlags |= CLOSEST_SEPARATING_AXIS;
	}
	o_axis = d_closestSeparatingAxis;
	return(true);
}

/***************************************************************************/
/***************************************************************************/
uint16 CObbToObbIntersection::SelectFaces(bool i_selectAFaces,
		Vector3CRef i_direction, float i_scaledCosAngle)
{
	// Fill in a bit field that identifies, or selects, faces. Faces of an obb
	// are identified by the integers 0-5 for the -x, +x, -y, +y, -z and +z faces
	// respectively. Setting the corresponding bit in the mask selects that face
	// of OBB A. Adding 8 to the face id and setting the corresponding bits (that
	// is, bits 8-13) selects that face of OBB B.
	uint16 selectedFaceMask = 0;
	int selectShift;
	Vector3 directionInObb;
	if (i_selectAFaces)
	{
		directionInObb = i_direction / (*(d_obbA->Matrix()));
		selectShift = 0;
	}
	else
	{
		directionInObb = i_direction / (*(d_obbB->Matrix()));
		selectShift = 8;
	}
	if (-directionInObb.X() > i_scaledCosAngle)
		selectedFaceMask |= NEG_X_BIT;
	if (directionInObb.X() > i_scaledCosAngle)
		selectedFaceMask |= POS_X_BIT;
	if (-directionInObb.Y() > i_scaledCosAngle)
		selectedFaceMask |= NEG_Y_BIT;
	if (directionInObb.Y() > i_scaledCosAngle)
		selectedFaceMask |= POS_Y_BIT;
	if (-directionInObb.Z() > i_scaledCosAngle)
		selectedFaceMask |= NEG_Z_BIT;
	if (directionInObb.Z() > i_scaledCosAngle)
		selectedFaceMask |= POS_Z_BIT;
	return(selectedFaceMask << selectShift);
}

/***************************************************************************/
/***************************************************************************/
uint16 CObbToObbIntersection::SelectAllFaces(bool i_selectAFaces)
{
	// Fill in a bit field that identifies, or selects, all faces of an
	// obb. Faces of an obb are identified by the integers 0-5 for the -x, +x,
	// -y, +y, -z and +z faces respectively. Setting the corresponding bit in the
	// mask selects that face of OBB A. Adding 8 to the face id and setting the
	// corresponding bits (that is, bits 8-13) selects that face of OBB B.
	if (i_selectAFaces)
		return(0x003f);
	return(0x3f00);
}

/***************************************************************************/
/***************************************************************************/
void CObbToObbIntersection::FindEdgeObbIntersections(
		const Cartesian4x4 &i_edgeToFace, Vector3CRef i_edgeSize,
		const OBB &i_faceOBB, uint8 i_obbFlag)
{
/*
 *	Find the vectors from the center to the faces of the "edge OBB".
 */
	Vector3 xDim, yDim, zDim;
	xDim = i_edgeToFace.GetRow0();
	yDim = i_edgeToFace.GetRow1();
	zDim = i_edgeToFace.GetRow2();
	xDim *= i_edgeSize.x();
	yDim *= i_edgeSize.y();
	zDim *= i_edgeSize.z();
/*
 *	Find the vector from the center of the "face OBB" to the center of the
 *	"edge OBB".
 */
	Vector3 offset;
	offset = i_edgeToFace.GetRow3();
/*
 *	Test each edge of the "edge OBB" against the faces of the "face OBB". The
 *	edges are specified using the vector to the center of each edge and the
 *	vector from the edge center to the edge endpoint. An edge center is found
 *	by adding the vector from the center of the "face OBB" to the center of the
 *	"edge OBB", the vector from the center of the "edge OBB" to the center of a
 *	face of the "edge OBB", and the vector from the center of the face to the
 *	center of the edge.
 *
 *	Use the center of the positive y-face to the z-faces (edges aligned with x).
 */
	Vector3 faceCenter = offset + yDim;
	FindEdgeFaceIntersections(faceCenter + zDim, xDim, i_faceOBB, i_obbFlag, 6);
	FindEdgeFaceIntersections(faceCenter - zDim, xDim, i_faceOBB, i_obbFlag, 4);
/*
 *	Use the center of the negative y-face to the z-faces (edges aligned with x).
 */
	faceCenter = offset - yDim;
	FindEdgeFaceIntersections(faceCenter + zDim, xDim, i_faceOBB, i_obbFlag, 2);
	FindEdgeFaceIntersections(faceCenter - zDim, xDim, i_faceOBB, i_obbFlag, 0);
/*
 *	Use the center of the positive x-face to the z-faces (edges aligned with y).
 */
	faceCenter = offset + xDim;
	FindEdgeFaceIntersections(faceCenter + zDim, yDim, i_faceOBB, i_obbFlag, 11);
	FindEdgeFaceIntersections(faceCenter - zDim, yDim, i_faceOBB, i_obbFlag, 9);
/*
 *	Use the center of the positive x-face to the y-faces (edges aligned with z).
 */
	FindEdgeFaceIntersections(faceCenter + yDim, zDim, i_faceOBB, i_obbFlag, 5);
	FindEdgeFaceIntersections(faceCenter - yDim, zDim, i_faceOBB, i_obbFlag, 1);
/*
 *	Use the center of the negative x-face to the z-faces (edges aligned with y).
 */
	faceCenter = offset - xDim;
	FindEdgeFaceIntersections(faceCenter + zDim, yDim, i_faceOBB, i_obbFlag, 10);
	FindEdgeFaceIntersections(faceCenter - zDim, yDim, i_faceOBB, i_obbFlag, 8);
/*
 *	Use the center of the negative x-face to the y-faces (edges aligned with z).
 */
	FindEdgeFaceIntersections(faceCenter + yDim, zDim, i_faceOBB, i_obbFlag, 7);
	FindEdgeFaceIntersections(faceCenter - yDim, zDim, i_faceOBB, i_obbFlag, 3);
}

/***************************************************************************/
/***************************************************************************/
void CObbToObbIntersection::FindEdgeFaceIntersections(Vector3CRef i_center,
		Vector3CRef i_halfEdge, const OBB &i_faceObb, uint8 i_obbFlag,
		uint8 i_edgeID)
{
/*
 *	Find the start point and the full edge vector.
 */
	Vector3 start = i_center - i_halfEdge;
	Vector3 edge = 2.0f * i_halfEdge;
/*
 *	Find the intersections.
 */
	uint entryFace, exitFace;
	float entryFrac, exitFrac;
	if (i_faceObb.GetConstAABB().IntersectLine(start, edge, entryFace, entryFrac,
			exitFace, exitFrac))
	{
/*
 *		See if the edge intersects the OBB at all.
 */
		if ((entryFrac <= 1.0f) && (exitFrac > 0.0f))
		{
/*
 *			Save the intersection points if appropriate.
 */
			if (entryFrac >= 0.0f)
			{
				ASSERT(d_numPoints < MAX_EDGE_FACE_INTERSECTIONS);
				d_faceID[d_numPoints] = (uint8)entryFace | i_obbFlag;
				d_edgeID[d_numPoints] = i_edgeID;
				d_point[d_numPoints] = (start + entryFrac * edge) *
						(*(i_faceObb.Matrix())) + (*(i_faceObb.Center()));
				d_numPoints++;
			}

			if (exitFrac < 1.0f)
			{
				ASSERT(d_numPoints < MAX_EDGE_FACE_INTERSECTIONS);
				d_faceID[d_numPoints] = (uint8)exitFace | i_obbFlag;
				d_edgeID[d_numPoints] = i_edgeID;
				d_point[d_numPoints] = (start + exitFrac * edge) *
						(*(i_faceObb.Matrix())) + (*(i_faceObb.Center()));
				d_numPoints++;
			}
		}
	}
}

/***************************************************************************/
/***************************************************************************/
void CObbToObbIntersection::PerformEdgeIntersectionNormalSum(void)
{
	ASSERT((d_obbA != NULL) && (d_obbB != NULL));

/*
 *	There are some edge/face intersections.
 */
	if (d_numPoints > 0)
	{
		Vector3 sum[2];
		sum[0].Clear();
		sum[1].Clear();
		Vector3 sumB;
		sumB.Clear();
		for (int i = 0; i < d_numPoints; i++)
		{
			int obbIndex;
/*
 *			On OBB A face.
 */
			if ((d_faceID[i] & 0x8) == 0)
				obbIndex = 0;
/*
 *			On OBB B face.
 */
			else
				obbIndex = 1;
/*
 *			Negative face.
 */
			if ((d_faceID[i] & 0x1) == 0)
			{
				switch((d_faceID[i] >> 1) & 0x3)
				{
				 case 0:
					 sum[obbIndex].x( sum[obbIndex].x() - 1.0f);
					 break;
				 case 1:
					 sum[obbIndex].y( sum[obbIndex].y() - 1.0f);
					 break;
				 case 2:
					 sum[obbIndex].z(sum[obbIndex].z() - 1.0f);
					 break;
				}
			}
/*
 *			Positive face.
 */
			else
			{
				switch((d_faceID[i] >> 1) & 0x3)
				{
				 case 0:
					 sum[obbIndex].x(sum[obbIndex].x() + 1.0f);
					 break;
				 case 1:
					 sum[obbIndex].y(sum[obbIndex].y() + 1.0f);
					 break;
				 case 2:
					 sum[obbIndex].z(sum[obbIndex].z() + 1.0f);
					 break;
				}
			}
		}
/*
 *		Find the sum. The normal should roughly point from OBB B to OBB A so
 *		the negative of the intersections with the faces of OBB A are added.
 */
		d_normalSum = sum[1] * (*(d_obbB->Matrix())) -
				sum[0] * (*(d_obbA->Matrix()));
	}
/*
 *	There are no intersection points so one box is completely contained within
 *	the other or they do not intersect at all. Either way, us the vector from
 *	the center of OBB B to the center of OBB A as the normal sum.
 */
	else
		d_normalSum = *(d_obbA->Center()) - *(d_obbB->Center());

	d_validFlags |= INTERSECTION_NORMAL_SUM;
}

/***************************************************************************/
/***************************************************************************/
Vector3 CObbToObbIntersection::EdgeIntersectionNormalSum(
		uint16 i_selectedFaceMask)
{
	if ((d_validFlags & INTERSECTION_POINTS) == 0)
		PerformEdgeIntersections();
/*
 *	There are some edge/face intersections.
 */
	if (d_numPoints > 0)
	{
/*
 *		Add up normals from intersections with selected faces.
 */
		Vector3 sum[2];
		sum[0].Clear();
		sum[1].Clear();
		Vector3 sumB;
		sumB.Clear();
		for (int i = 0; i < d_numPoints; i++)
		{
			if (((1 << d_faceID[i]) & i_selectedFaceMask) != 0)
			{
				int obbIndex;
/*
 *				On OBB A face.
 */
				if ((d_faceID[i] & 0x8) == 0)
					obbIndex = 0;
/*
 *				On OBB B face.
 */
				else
					obbIndex = 1;
/*
 *				Negative face.
 */
				if ((d_faceID[i] & 0x1) == 0)
				{
					switch((d_faceID[i] >> 1) & 0x3)
					{
					 case 0:
						sum[obbIndex].X(sum[obbIndex].X() - 1.0f);
						break;
					 case 1:
						sum[obbIndex].Y(sum[obbIndex].Y() - 1.0f);
						break;
					 case 2:
						sum[obbIndex].Z(sum[obbIndex].Z() - 1.0f);
						break;
					}
				}
/*
 *				Positive face.
 */
				else
				{
					switch((d_faceID[i] >> 1) & 0x3)
					{
					 case 0:
						sum[obbIndex].X(sum[obbIndex].X() + 1.0f);
						break;
					 case 1:
						sum[obbIndex].Y(sum[obbIndex].Y() + 1.0f);
						break;
					 case 2:
						sum[obbIndex].Z(sum[obbIndex].Z() + 1.0f);
						break;
					}
				}
			}
		}
/*
 *		Find the sum. The normal should roughly point from OBB B to OBB A so
 *		the negative of the intersections with the faces of OBB A are added.
 */
		return(sum[1] * (*(d_obbB->Matrix())) - sum[0] * (*(d_obbA->Matrix())));
	}
/*
 *	There are no intersection points so one box is completely contained within
 *	the other or they do not intersect at all. Either way, use the vector from
 *	the center of OBB B to the center of OBB A as the normal sum.
 */
	return(*(d_obbA->Center()) - *(d_obbB->Center()));
}

/***************************************************************************/
/***************************************************************************/
void CObbToObbIntersection::EdgeIntersectionPlaneFit(Vector3 &o_loc,
		Vector3 &o_normal)
{
	ASSERT((d_obbA != NULL) && (d_obbB != NULL));

	if ((d_validFlags & INTERSECTION_PLANE) == 0)
	{
		if ((d_validFlags & INTERSECTION_POINTS) == 0)
			PerformEdgeIntersections();

		if ((d_validFlags & INTERSECTION_NORMAL_SUM) == 0)
			PerformEdgeIntersectionNormalSum();
/*
 *		If there are intersection points, fit a plane to them.
 */
		if (d_numPoints > 0)
		{
			d_planeResult = CPlaneFit::TransformFit(d_numPoints,
					d_point, d_normalSum, d_planeLocation, d_planeNormal);
		}
/*
 *		There are no intersection points.
 */
		else
		{
/*
 *			When there are no intersection points, the normal sum is the vector
 *			from OBB A to OBB B. Set the plane normal to the normalized normal
 *			sum. If the normal sum is zero length (the two OBBs are coincident),
 *			arbitrarily use the y-axis.
 */
			d_planeNormal = d_normalSum;
			if (!d_planeNormal.SafeNormalize())
				d_planeNormal.Set(0.0f, 1.0f, 0.0f);
/*
 *			Set the plane location to a point between the two OBBs weighted by
 *			their radii.
 */
			float radiusWeight = d_obbA->Radius() + d_obbB->Radius();
			if (!Math::DivByZero(d_obbA->Radius(), radiusWeight))
				radiusWeight = d_obbA->Radius() / radiusWeight;
			else
				radiusWeight = 0.5f;
			d_planeLocation = *(d_obbA->Center()) + radiusWeight * d_normalSum;

			d_planeResult = CPlaneFit::PLANE_FIT_NO_POINTS;
		}

		d_validFlags |= INTERSECTION_PLANE;
	}

	o_loc = d_planeLocation;
	o_normal = d_planeNormal;
}

/***************************************************************************/
/***************************************************************************/
void CObbToObbIntersection::DrawPoints(DrawUtility::COLOR i_color)
{
	if ((d_validFlags & INTERSECTION_POINTS) == 0)
		PerformEdgeIntersections();
	for (int i = 0; i < d_numPoints; i++)
	{
		DrawUtility::Sphere(d_point[i], i_color, 0.02f);
	}
}

#ifdef DEBUG_STACK
/**********************************************************************/
/**********************************************************************/
const char* CObbToObbIntersection::GetName(void)
{
	return("ObbToObb");
}
#endif
