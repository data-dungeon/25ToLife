/***************************************************************************/
// The magic matrix class
/***************************************************************************/

/******************* includes ****************************/

#include "Math/MathPCH.h"





/***************************************************************************/
// Invert (returns false if not invertable)
/***************************************************************************/
bool Matrix3x3::Invert(
Matrix3x3 &inverse) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_INVERT_M3X3);
#endif //MATH_STATS

	// Does the determinant exist?
	float det = Det();
	if (det == 0.0f)
		return(false);

	// Construct the adjoint
	Adj(inverse);

	// inv = 1/det * adjoint
	inverse /= det;

	// we are done!
	return(true);
}

/***************************************************************************/
// Invert, always works. (it's the transpose)
/***************************************************************************/
bool DirCos3x3::Invert(
Matrix3x3 &inverse) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(DIRCOS3X3_INVERT_D3X3);
#endif //MATH_STATS

	Transpose(inverse);
	return(true);
}

/***************************************************************************/
// Orthonormalize this puppy
/***************************************************************************/
void DirCos3x3::Orthonormalize(void)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(DIRCOS3X3_ORTHONORMALIZE);
#endif //MATH_STATS

	Vector3 x, y, z;

	x = GetColumn0();
	y = GetColumn1();

	x.Normalize();
	z = Vector3::Cross(x, y).Normalized();
//	y = Vector3::Cross(z, x).Normalized();
	y = Vector3::Cross(z, x);  // x & z already normalized & perpendicular

	SetColumn0(x);
	SetColumn1(y);
	SetColumn2(z);
}

/***************************************************************************/
// Graphics Gems approach (Vol. 1, pg. 464)
/***************************************************************************/
void DirCos3x3::Orthogonalize(void)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(DIRCOS3X3_ORTHOGANALIZE);
#endif //MATH_STATS

/*
 *	X = Transp(M) * M - I
 */
	Matrix3x3 x(
			e00 * e00 + e10 * e10 + e20 * e20 - 1.0f,
			e00 * e01 + e10 * e11 + e20 * e21,
			e00 * e02 + e10 * e12 + e20 * e22,

			e01 * e00 + e11 * e10 + e21 * e20,
			e01 * e01 + e11 * e11 + e21 * e21 - 1.0f,
			e01 * e02 + e11 * e12 + e21 * e22,

			e02 * e00 + e12 * e10 + e22 * e20,
			e02 * e01 + e12 * e11 + e22 * e21,
			e02 * e02 + e12 * e12 + e22 * e22 - 1.0f);
/*
 *	A = 3/8 * I - 5/16 * X
 */
	Matrix3x3 a(-0.3125f * x.e00 + 0.3750f, -0.3125f * x.e01,
					-0.3125f * x.e02, -0.3125f * x.e10,
					-0.3125f * x.e11 + 0.3750f, -0.3125f * x.e12,
					-0.3125f * x.e20, -0.3125f * x.e21,
					-0.3125f * x.e22 + 0.3750f);
/*
 *	B = A * X - 1/2 * I = -1/2 * I + 3/8 * X - 5/16 * X^2
 */
	Matrix3x3 b(
			a.e00 * x.e00 + a.e01 * x.e10 + a.e02 * x.e20 - 0.5f,
			a.e00 * x.e01 + a.e01 * x.e11 + a.e02 * x.e21,
			a.e00 * x.e02 + a.e01 * x.e12 + a.e02 * x.e22,

			a.e10 * x.e00 + a.e11 * x.e10 + a.e12 * x.e20,
			a.e10 * x.e01 + a.e11 * x.e11 + a.e12 * x.e21 - 0.5f,
			a.e10 * x.e02 + a.e11 * x.e12 + a.e12 * x.e22,

			a.e20 * x.e00 + a.e21 * x.e10 + a.e22 * x.e20,
			a.e20 * x.e01 + a.e21 * x.e11 + a.e22 * x.e21,
			a.e20 * x.e02 + a.e21 * x.e12 + a.e22 * x.e22 - 0.5f);
/*
 *	A = B * X + I = I - 1/2 * X + 3/8 * X^2 - 5/16 * X^3
 */
	a.e00 = b.e00 * x.e00 + b.e01 * x.e10 + b.e02 * x.e20 + 1.0f;
	a.e01 = b.e00 * x.e01 + b.e01 * x.e11 + b.e02 * x.e21;
	a.e02 = b.e00 * x.e02 + b.e01 * x.e12 + b.e02 * x.e22;

	a.e10 = b.e10 * x.e00 + b.e11 * x.e10 + b.e12 * x.e20;
	a.e11 = b.e10 * x.e01 + b.e11 * x.e11 + b.e12 * x.e21 + 1.0f;
	a.e12 = b.e10 * x.e02 + b.e11 * x.e12 + b.e12 * x.e22;

	a.e20 = b.e20 * x.e00 + b.e21 * x.e10 + b.e22 * x.e20;
	a.e21 = b.e20 * x.e01 + b.e21 * x.e11 + b.e22 * x.e21;
	a.e22 = b.e20 * x.e02 + b.e21 * x.e12 + b.e22 * x.e22 + 1.0f;
/*
 *	M = M * A
 */
	((Matrix3x3) (*this)) *= a;
}

/***************************************************************************/
// decode to yxz rotations
/***************************************************************************/
bool DirCos3x3::DecodeToYXZ(
Vector3 &v3RotVect) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(DIRCOS3X3_DECODE_TO_YXZ_V3);
#endif //MATH_STATS

	if(Math::Abs(e21 > 0.99f))
		return(false);
	v3RotVect.Set(Math::ArcSin(-e21),
					  Math::ArcTan2(e20, e22),
					  Math::ArcTan2(e01, e11));
	return(true);
}

/***************************************************************************/
/***************************************************************************/
bool DirCos3x3::DecodeToXZY(
Vector3 &v3RotVect) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(DIRCOS3X3_DECODE_TO_XZY_V3);
#endif //MATH_STATS

	if(Math::Abs(e10 > 0.99f))
		return(false);

	v3RotVect.Set(Math::ArcTan2(e12, e11),
					  Math::ArcTan2(e20, e00),
					  Math::ArcSin(-e10));
	return(true);
}

/***************************************************************************/
/***************************************************************************/
bool DirCos3x3::DecodeToZYX(
Vector3 &v3RotVect) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(DIRCOS3X3_DECODE_TO_ZYX_V3);
#endif //MATH_STATS

	if(Math::Abs(e02 > 0.99f))
		return(false);
	v3RotVect.Set(Math::ArcTan2(e12, e22),
					  Math::ArcSin(-e02),
					  Math::ArcTan2(e01, e00));
	return(true);
}

/***************************************************************************/
// Builds a matrix from a vector (see DirCos3x3::BuildFromUnitVector
// comments)
/***************************************************************************/
bool DirCos3x3::BuildFromVector(
Vector3CRef v,
int vectorRow,
int alignRow,
int alignAxis)
{
/*
 *	Normalize the vector. If it cannot be normalized, build an identity matrix
 *	and return false.
 */
	Vector3 axis = v;
	if (!axis.SafeNormalize())
	{
#ifdef MATH_STATS	// defined in Math\MathStats.h
		MATH_STATS_INC(DIRCOS3X3_BUILD_FROM_VECTOR_V3);
#endif //MATH_STATS
		Identity();
		return(false);
	}
/*
 *	Build the matrix.
 */
	return(BuildFromUnitVector(axis, vectorRow, alignRow, alignAxis));
}

/***************************************************************************/
// Builds a matrix from a unit vector. The process can be thought of as
//    setting one axis of a coordinate system then spinning the other two
//    axes such that one of them is as closely aligned with a principle axis
//    as possible. Returns false if invalid arguments are used (also ASSERTS).
// Arguments:
//    v         - The unit vector that will be a row (or axis) of the matrix.
//    vectorRow - The row filled by v. Valid values are 0 (x-axis),
//                1 (y-axis), and 2 (z-axis).
//    alignRow  - The row to be aligned. Valid values are 0 (x-axis),
//                1 (y-axis), and 2 (z-axis) except that it cannot equal
//                vectorRow.
//    alignAxis - The principle axis the alignRow is to be aligned with.
//                Valid values are 0 ([1 0 0]), 1 ([0 1 0]), 2 ([0 0 1]), or
//                any other value. If the value is not 0, 1, or 2, or if the
//                vector v is aligned with the axis, the most numerically
//                stable axis is used.
/***************************************************************************/
bool DirCos3x3::BuildFromUnitVector(
Vector3CRef v,
int vectorRow,
int alignRow,
int alignAxis)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(DIRCOS3X3_BUILD_FROM_VECTOR_V3);
#endif //MATH_STATS
/*
 *	Validate arguments.
 */
	if ((vectorRow < 0) || (vectorRow > 2) || (alignRow < 0) || (alignRow > 2) ||
			(vectorRow == alignRow))
	{
		ASSERT(false);
		Identity();
		return(false);
	}
/*
 *	Try to find a vector perpendicular to the specified vector and the alignment
 *	axis. This will fail if no alignment axis is specified or if the specified
 *	vector is aligned with the alignment axis.
 */
	Vector3 perpAxis;
	bool perpFound;
	switch(alignAxis)
	{
	 case 0:
		perpAxis.Set(0.0f, v.z(), -v.y());
		perpFound = perpAxis.SafeNormalize();
		break;
	 case 1:
		perpAxis.Set(-v.z(), 0.0f, v.x());
		perpFound = perpAxis.SafeNormalize();
		break;
	 case 2:
		perpAxis.Set(v.y(), -v.x(), 0.0f);
		perpFound = perpAxis.SafeNormalize();
		break;
	 default:
		perpFound = false;
		break;
	}
/*
 *	Need to find the most numerically stable principle axis.
 */
	if (!perpFound)
	{
/*
 *		Find the principle axis the specified vector is least aligned with.
 */
		Vector3 absV = Vector3::Abs(v);
		if (absV.x() < absV.y())
		{
			if (absV.x() < absV.z())
				alignAxis = 0;
			else
				alignAxis = 2;
		}
		else if (absV.y() < absV.z())
			alignAxis = 1;
		else
			alignAxis = 2;
/*
 *		Build the perpendicluar axis.
 */
		switch(alignAxis)
		{
		 case 0:
			perpAxis.Set(0.0f, v.z(), -v.y());
			break;
		 case 1:
			perpAxis.Set(-v.z(), 0.0f, v.x());
			break;
		 case 2:
			perpAxis.Set(v.y(), -v.x(), 0.0f);
			break;
		}
/*
 *		Normalize the axis. It will be safe.
 */
		perpAxis.Normalize();
	}
/*
 *	Set the specified row and the alignment row.
 */
	SetRow(vectorRow, v);
	SetRow(alignRow, perpAxis.Cross(v));
/*
 *	Determine the remaining row and set it to the appropriately oriented
 *	perpendicular axis.
 */
	switch(vectorRow)
	{
	 case 0:
		if (alignRow == 1)
			SetRow(2, perpAxis);
		else
			SetRow(1, perpAxis.Negated());
		break;
	 case 1:
		if (alignRow == 0)
			SetRow(2, perpAxis.Negated());
		else
			SetRow(0, perpAxis);
		break;
	 case 2:
		if (alignRow == 0)
			SetRow(1, perpAxis);
		else
			SetRow(0, perpAxis.Negated());
		break;
	}
/*
 *	Success.
 */
	return(true);
}

/***************************************************************************/
/***************************************************************************/
bool Matrix3x3::Deconstruct(
DirCos3x3 &rot,
float &scaleX,
float &scaleY,
float &scaleZ,
float &shearXY,
float &shearXZ,
float &shearYZ) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_DECONSTRUCT);
#endif //MATH_STATS

	float temp = Det();
	if ((temp < 1.0e-6f) && (temp > -1.0e-6f))
	{
		ASSERT(false);
		scaleX = scaleY = scaleZ = 1.0f;
		shearXY = shearXZ = shearYZ = 0.0f;	
		return (false);
	}

	Vector3 row0, row1, row2;

	row0 = GetRow0();
	row1 = GetRow1();
	row2 = GetRow2();

	scaleX = row0.Length();
	row0 /= scaleX;

	shearXY = row0.Dot(row1);
	row1 -= shearXY * row0;
	scaleY = row1.Length();
	temp = 1.0f / scaleY;
	row1 *= temp;
	shearXY *= temp;

	shearXZ = row0.Dot(row2);
	shearYZ = row1.Dot(row2);
	row2 -= shearXZ * row0 + shearYZ * row1;
	scaleZ = row2.Length();
	temp = 1.0f / scaleZ;
	row2 *= temp;
	shearXZ *= temp;
	shearYZ *= temp;

	rot.SetRow0(row0);
	rot.SetRow1(row1);
	rot.SetRow2(row2);

	if (rot.Det() < 0.0f)
	{
		 (Matrix3x3)rot *= -1.0f;
		 scaleX = -scaleX;
		 scaleY = -scaleY;
		 scaleZ = -scaleZ;
	}

	return (true);
}

/***************************************************************************/
/***************************************************************************/
bool DirCos3x3::Validate(float epsilon) const
{
	Vector3 xAxis, yAxis, zAxis;
	xAxis = GetRow0();
	yAxis = GetRow1();
	zAxis = GetRow2();
	return(Math::Equal(xAxis.Dot(xAxis), 1.0f, epsilon) &&
			Math::Equal(yAxis.Dot(yAxis), 1.0f, epsilon) &&
			Math::Equal(zAxis.Dot(zAxis), 1.0f, epsilon) &&
			Math::Zero(xAxis.Dot(yAxis), epsilon) &&
			Math::Zero(xAxis.Dot(zAxis), epsilon) &&
			Math::Zero(yAxis.Dot(zAxis), epsilon) &&
			Math::Equal(zAxis.Dot(xAxis.Cross(yAxis)), 1.0f, epsilon));
}

/***************************************************************************/
// Algorithm found in "Spacecraft Attitude Dynamics" by Peter C. Hughes,
// pp. 13-14, Wiley (edition unknown).
/***************************************************************************/
void DirCos3x3::GetAngleAxis(float &angle, Vector3 &axis)
{
	// Find the cosine of the angle.
	float cosAngle = Math::Clamp(0.5f * (e00 + e11 + e22 - 1.0f), -1.0f, 1.0f);

	// Find the sine of the angle. The angle will be restricted to be between
	// 0 and PI; therefore, the sine of the angle will be positive.
	float sinAngle = Math::Sqrt(1.0f - cosAngle * cosAngle);

	// If the sine is 0, the angle is either 0 or PI. (The effective numerator
	// below is 1.)
	if (Math::DivByZero(1.0f, sinAngle))
	{
		// If the angle is 0, just pick some axis.
		if (cosAngle > 0.0f)
		{
			axis.Set(0.0f, 1.0f, 0.0f);
			angle = 0.0f;
		}

		// The angle is PI. Either axis direction is valid so just pick the one
		// with the positive y-component.
		else
		{
			if (e01 >= 0.0)
				axis.X(Math::Sqrt(0.5f * (1.0f + e00)));
			else
				axis.X(-Math::Sqrt(0.5f * (1.0f + e00)));
			axis.Y(Math::Sqrt(0.5f * (1.0f + e11)));
			if (e12 >= 0.0f)
				axis.Z(Math::Sqrt(0.5f * (1.0f + e22)));
			else
				axis.Z(-Math::Sqrt(0.5f * (1.0f + e22)));
			angle = Math::Pi;
		}
	}

	// The angle is between 0 and PI.
	else
	{
		float scale = 0.5f / sinAngle;
		axis.X(scale * (e12 - e21));
		axis.Y(scale * (e20 - e02));
		axis.Z(scale * (e01 - e10));
		angle = Math::ArcCos(cosAngle);
	}

	// Error seems to collect.
	axis.Normalize();
}
