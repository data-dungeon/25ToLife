#ifndef SPLINE_H
#define SPLINE_H

#include "Math/Vector.h"

// ===========================================================================
//	CLASS: Spline

class Spline
{
	public:
		Vector3 vEndPointA;
		Vector3 vControlPointA;
		Vector3 vControlPointB;
		Vector3 vEndPointB;

	public:
		Spline() {};
		Spline(const Vector3& i_vEndPointA, const Vector3& i_vControlPointA,
			 const Vector3& i_vControlPointB, const Vector3& i_vEndPointB);

		void GetPointAndTangent(float i_fT, Vector3* o_vPoint, Vector3* o_vTangent);

	public:	// *** STATIC FUNCTIONS ***

		static void GetPointAndTangent(float i_fT, const Vector3& i_vEndPointA,
			const Vector3& i_vControlPointA, const Vector3& i_vControlPointB,
			const Vector3& i_vEndPointB, Vector3* o_vPoint, Vector3* o_vTangent);

		static float EstimateLength(const Vector3& i_vEndPointA,
			const Vector3& i_vControlPointA, const Vector3& i_vControlPointB,
			const Vector3& i_vEndPointB, int i_iSubdivisions = 16);
};

// ===========================================================================
//	CLASS: SplineChain
//
//	A spline chain is a series of splines hooked together such that the last
//	endpoint of one spline is the first endpoint of the next.  A 2-spline chain
//	would be represented as: EP, CP, CP, EP, CP, CP, EP -- essentially,
//	elminating the duplication of shared endpoints in the data.

class SplineChain
{
	public:	// *** STATIC FUNCTIONS ***

		static void GetPointAndTangent(float i_fT, const Vector3* i_pvPointList,
			int i_iPointCount, Vector3* o_vPoint, Vector3* o_vTangent);
};

#endif
