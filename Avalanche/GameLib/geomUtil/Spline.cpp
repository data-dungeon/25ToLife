/******************* includes ****************************/

#include "geomUtil/GeomUtilPCH.h"





// ===========================================================================
//	IMPLEMENTATION: Spline
// ---------------------------------------------------------------------------
//	CONSTRUCTOR

Spline::Spline(const Vector3& i_vEndPointA, const Vector3& i_vControlPointA,
	const Vector3& i_vControlPointB, const Vector3& i_vEndPointB)
{
	vEndPointA		= i_vEndPointA;
	vControlPointA	= i_vControlPointA;
	vControlPointB	= i_vControlPointB;
	vEndPointB		= i_vEndPointB;
}

// ---------------------------------------------------------------------------
//	FUNCTION: GetPointAndTangent

void Spline::GetPointAndTangent(float i_fT, Vector3* o_vPoint, Vector3* o_vTangent)
{
	GetPointAndTangent(i_fT, vEndPointA, vControlPointA,
		vControlPointB, vEndPointB, o_vPoint, o_vTangent);
}

// ---------------------------------------------------------------------------
//	STATIC FUNCTION: GetPointAndTangent

void Spline::GetPointAndTangent(float t, const Vector3& a, const Vector3& b, const Vector3& c, const Vector3& d, Vector3* point, Vector3* tangent)
{
	if (t < 0.0f)
		t = 0.0f;

	else if (t > 1.0f)
		t = 1.0f;

	Vector3 e, f, g, h, i, j;

	e = (b - a) * t + a;
	f = (c - b) * t + b;
	g = (d - c) * t + c;

	h = (f - e) * t + e;
	i = (g - f) * t + f;

	j = i - h;

	if (tangent)
		*tangent = j.Normalized();

	if (point)
		*point = j * t + h;
}

// ---------------------------------------------------------------------------
//	STATIC FUNCTION: EstimateLength

float Spline::EstimateLength(const Vector3& i_vEndPointA, const Vector3& i_vControlPointA,
	const Vector3& i_vControlPointB, const Vector3& i_vEndPointB, int i_iSubdivisions)
{
	if (i_iSubdivisions < 0)
		i_iSubdivisions = 0;

	int iSegments = i_iSubdivisions + 1;

	Vector3 vA = i_vEndPointA;
	float fLength = 0.0f;

	for (int i = 1; i <= i_iSubdivisions; i++)
	{
		float fT = (1.0f / iSegments) * i;

		Vector3 vB;

		GetPointAndTangent(fT, i_vEndPointA, i_vControlPointA,
			i_vControlPointB, i_vEndPointB, &vB, 0);

		Vector3 vAB = vB - vA;
		fLength += vAB.Length();
	}

	Vector3 vAB = i_vEndPointB - vA;
	fLength += vAB.Length();

	return fLength;
}


// ===========================================================================
//	IMPLEMENTATION: SplineChain
// ---------------------------------------------------------------------------
//	STATIC FUNCTION: GetPointAndTangent

void SplineChain::GetPointAndTangent(float i_fT, const Vector3* i_pvPointList,
	int i_iPointCount, Vector3* o_vPoint, Vector3* o_vTangent)
{
	int iSplineCount = (i_iPointCount - 1) / 3;

	float fLocalT = i_fT * iSplineCount;
	int iSplineIdx = (int)fLocalT;
	int iPointIdx = iSplineIdx * 3;

	fLocalT = Math::Frac(fLocalT);

	Vector3 vEndPointA		= i_pvPointList[iPointIdx + 0];
	Vector3 vControlPointA	= i_pvPointList[iPointIdx + 1];
	Vector3 vControlPointB	= i_pvPointList[iPointIdx + 2];
	Vector3 vEndPointB		= i_pvPointList[iPointIdx + 3];

	Spline::GetPointAndTangent(fLocalT, vEndPointA, vControlPointA,
		vControlPointB, vEndPointB, o_vPoint, o_vTangent);
}
