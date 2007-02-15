/***************************************************************************/
// The magic matrix class
/***************************************************************************/

/******************* includes ****************************/

#include "Math/MathPCH.h"





/***************************************************************************/
/***************************************************************************/
bool Matrix4x4::Deconstruct(
DirCos3x3 &rot,
Vector3 &trans,
float &scaleX,
float &scaleY,
float &scaleZ,
float &shearXY,
float &shearXZ,
float &shearYZ,
float &persX,
float &persY,
float &persZ,
float &persW) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_DECONSTRUCT);
#endif //MATH_STATS

	trans.Set(tx, ty, tz);

	Matrix3x3 ul;
	ul = (Graphics4x4 &)(*this);
	Matrix3x3 ulInv;
	if (!ul.Invert(ulInv))
		return (false);

	Vector3 v(e03, e13, e23);
	Vector3 p = v * ulInv.Transposed();
	persX = p.x();
	persY = p.y();
	persZ = p.z();
	persW = e33 - trans.Dot(p);

	return (ul.Deconstruct(rot, scaleX, scaleY, scaleZ, shearXY, shearXZ,
			shearYZ));
}

/***************************************************************************/
/***************************************************************************/
bool Matrix4x4::Invert(
Matrix4x4 &inverse) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_INVERT_M4X4);
#endif //MATH_STATS

	// Does the determinant exist?
	float det = Det();
	if (det == 0.0f)
		return(false);

	// Construct the adjoint
	Adj(inverse);

	// inv = 1/det * adjoint
	inverse /= det;

	// that was easy
	return(true);
}

/***************************************************************************/
/***************************************************************************/
bool Graphics4x4::Invert(
Matrix4x4 &inverse) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_INVERT_M4X4);
#endif //MATH_STATS

	Matrix3x3 ul, ulInv;
	GetOrientation(ul);

	// Does the determinant exist?
	float detInv = Matrix3x3::Det(ul);
	if (Math::DivByZero(1.0f, detInv))
		return(false);
	detInv = 1.0f / detInv;

	// Construct the adjoint
	ul.Adj(ulInv);

	// inv = 1/det * adjoint
	ulInv *= detInv;

	Vector3 ll;
	ll.Set(-detInv * Determinant::Calc(e10, e11, e12, e20, e21, e22, e30, e31, e32),
			 detInv * Determinant::Calc(e00, e01, e02, e20, e21, e22, e30, e31, e32),
			 -detInv * Determinant::Calc(e00, e01, e02, e10, e11, e12, e30, e31, e32));

	((Graphics4x4*)(&inverse))->Compose(ulInv, ll);

	// that was easy
	return(true);
}

/***************************************************************************/
// Invert, always works. (it's the transpose and translation transformation)
/***************************************************************************/
bool Cartesian4x4::Invert(
Matrix4x4 &inverse) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(CARTESIAN4X4_INVERT_M4X4);
#endif //MATH_STATS

	inverse.r00 = r00;
	inverse.r01 = r10;
	inverse.r02 = r20;
	inverse.u0 = 0.0f;

	inverse.r10 = r01;
	inverse.r11 = r11;
	inverse.r12 = r21;
	inverse.u1 = 0.0f;

	inverse.r20 = r02;
	inverse.r21 = r12;
	inverse.r22 = r22;
	inverse.u2 = 0.0f;

	inverse.tx = -tx * r00 - ty * r01 - tz * r02;
	inverse.ty = -tx * r10 - ty * r11 - tz * r12;
	inverse.tz = -tx * r20 - ty * r21 - tz * r22;
	inverse.w = 1.0f;

	return(true);
}

/***************************************************************************/
/***************************************************************************/
bool Graphics4x4::DecodeToYXZ(
Vector3 &v3RotVect) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_DECODE_TO_YXZ_V3);
#endif //MATH_STATS

	if(Math::Abs(r21 > 0.99f))
		return(false);
	v3RotVect.Set(Math::ArcSin(-r21),
					  Math::ArcTan2(r20, r22),
					  Math::ArcTan2(r01, r11));
	return(true);
}

/***************************************************************************/
/***************************************************************************/
bool Graphics4x4::DecodeToXZY(
Vector3 &v3RotVect) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_DECODE_TO_XZY_V3);
#endif //MATH_STATS

	if(Math::Abs(r10 > 0.99f))
		return(false);
	v3RotVect.Set(Math::ArcTan2(r12, r11),
					  Math::ArcTan2(r20, r00),
					  Math::ArcSin(-r10));
	return(true);
}

/***************************************************************************/
/***************************************************************************/
bool Graphics4x4::DecodeToZYX(
Vector3 &v3RotVect) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_DECODE_TO_ZYX_V3);
#endif //MATH_STATS

	if(Math::Abs(r02 > 0.99f))
		return(false);
	v3RotVect.Set(Math::ArcTan2(r12, r22),
					  Math::ArcSin(-r02),
					  Math::ArcTan2(r01, r00));
	return(true);
}

/***************************************************************************/
/***************************************************************************/
void Graphics4x4::SetIntermediate(
float rPercentage,
Graphics4x4 &g4To)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_SET_INTERMEDIATE_F_G4X4);
#endif //MATH_STATS

	ts_Quatvw pQuatFrom;
	ts_Quatvw pQuatTo;
	ts_Quatvw pQuatOut;

	pQuatFrom.V.Clear();
	pQuatFrom.W = 1.0f;
	qatFromMat(&g4To, &pQuatTo);
	qatSlerp(&pQuatFrom, &pQuatTo, rPercentage, &pQuatOut);
	qatToRotMat(&pQuatOut, (Cartesian4x4*)&g4To);
}

/***************************************************************************/
/***************************************************************************/
void Graphics4x4::BuildFromQuat(float rA, float rB, float rC, float rD)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_BUILD_FROM_QUAT_F);
#endif //MATH_STATS

	ts_Quatvw pQuat;
	pQuat.V.Set(rA,
					rB,
					rC);
	pQuat.W = rD;
	qatToRotMat(&pQuat, (Cartesian4x4*)this);
}

/***************************************************************************/
// Builds a matrix from a vector (see DirCos3x3::BuildFromUnitVector
// comments)
/***************************************************************************/
bool Graphics4x4::BuildFromVector(
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
		MATH_STATS_INC(GRAPHICS4X4_BUILD_FROM_VECTOR_V3);
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
// Builds a matrix from a unit vector (see DirCos3x3::BuildFromUnitVector
// comments)
/***************************************************************************/
bool Graphics4x4::BuildFromUnitVector(
Vector3CRef v,
int vectorRow,
int alignRow,
int alignAxis)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_BUILD_FROM_VECTOR_V3);
#endif //MATH_STATS
/*
 *	Build the matrix part.
 */
	DirCos3x3 m;
	if (!m.BuildFromUnitVector(v, vectorRow, alignRow, alignAxis))
	{
		Identity();
		return(false);
	}
	Set(m);
	return(true);
}

/***************************************************************************/
// plow through an array of vertices
/***************************************************************************/

#ifndef EXCLUDE_GENERIC_GRAPHICS4X4_TRANSFORM_ARRAY_VECTOR3	// compile control from mat4x4.h for platform-specific implementation

void Graphics4x4::TransformArray(
const Vector3 *vIn,
Vector3 *vOut,
int nCount) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_TRANSFORM_ARRAY_V3_V3_I);
#endif //MATH_STATS

#ifdef CHECK_GRAPHICS4X4	// defined in Math\Matrix.h
	ASSERT(u0 == 0.0f);
	ASSERT(u1 == 0.0f);
	ASSERT(u2 == 0.0f);
	ASSERT(w == 1.0f);
#endif //CHECK_GRAPHICS4X4

	for (int i = 0; i < nCount; i++)
		vOut[i].FastMultiply(vIn[i], *this);
}

#endif //EXCLUDE_GENERIC_GRAPHICS4X4_TRANSFORM_ARRAY_VECTOR3	// compile control from mat4x4.h for platform-specific implementation

/***************************************************************************/
/***************************************************************************/

#ifndef EXCLUDE_GENERIC_GRAPHICS4X4_TRANSFORM_ARRAY_VECTOR4	// compile control from mat4x4.h for platform-specific implementation

void Graphics4x4::TransformArray(
const Vector4 *vIn,
Vector4 *vOut,
int nCount) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_TRANSFORM_ARRAY_V4_V4_I);
#endif //MATH_STATS

#ifdef CHECK_GRAPHICS4X4	// defined in Math\Matrix.h
	ASSERT(u0 == 0.0f);
	ASSERT(u1 == 0.0f);
	ASSERT(u2 == 0.0f);
	ASSERT(w == 1.0f);
#endif //CHECK_GRAPHICS4X4

	for (int i = 0; i < nCount; i++)
		vOut[i].FastMultiply(vIn[i], *this);
}

#endif //EXCLUDE_GENERIC_GRAPHICS4X4_TRANSFORM_ARRAY_VECTOR4	// compile control from mat4x4.h for platform-specific implementation

/***************************************************************************/
/***************************************************************************/

#ifndef EXCLUDE_GENERIC_GRAPHICS4X4_BASIS_TRANSFORM_ARRAY_VECTOR3	// compile control from mat4x4.h for platform-specific implementation

void Graphics4x4::BasisTransformArray(
const Vector3 *vIn,
Vector3 *vOut,
int nCount) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_BASIS_TRANSFORM_ARRAY_V3_V3_I);
#endif //MATH_STATS

#ifdef CHECK_GRAPHICS4X4	// defined in Math\Matrix.h
	ASSERT(u0 == 0.0f);
	ASSERT(u1 == 0.0f);
	ASSERT(u2 == 0.0f);
	ASSERT(w == 1.0f);
#endif //CHECK_GRAPHICS4X4

	for (int i = 0; i < nCount; i++)
		vOut[i].FastBasisTransform(vIn[i], *this);
}

#endif //EXCLUDE_GENERIC_GRAPHICS4X4_BASIS_TRANSFORM_ARRAY_VECTOR3	// compile control from mat4x4.h for platform-specific implementation

/***************************************************************************/
/***************************************************************************/

#ifndef EXCLUDE_GENERIC_GRAPHICS4X4_BASIS_TRANSFORM_ARRAY_VECTOR4	// compile control from mat4x4.h for platform-specific implementation

void Graphics4x4::BasisTransformArray(
const Vector4 *vIn,
Vector4 *vOut,
int nCount) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_BASIS_TRANSFORM_ARRAY_V4_V4_I);
#endif //MATH_STATS

#ifdef CHECK_GRAPHICS4X4	// defined in Math\Matrix.h
	ASSERT(u0 == 0.0f);
	ASSERT(u1 == 0.0f);
	ASSERT(u2 == 0.0f);
	ASSERT(w == 1.0f);
#endif //CHECK_GRAPHICS4X4

	for (int i = 0; i < nCount; i++)
		vOut[i].FastBasisTransform(vIn[i], *this);
}

#endif //EXCLUDE_GENERIC_GRAPHICS4X4_BASIS_TRANSFORM_ARRAY_VECTOR4	// compile control from mat4x4.h for platform-specific implementation

/***************************************************************************/
/***************************************************************************/
bool Graphics4x4::Deconstruct(
DirCos3x3 &rot,
Vector3 &trans,
float &scaleX,
float &scaleY,
float &scaleZ,
float &shearXY,
float &shearXZ,
float &shearYZ) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_DECONSTRUCT);
#endif //MATH_STATS

	Matrix3x3 ul;
	Decompose(ul, trans);

	return (ul.Deconstruct(rot, scaleX, scaleY, scaleZ, shearXY, shearXZ,
			shearYZ));
}


