// quaternion class
#ifndef QUAT_H
#define QUAT_H

#include "platform/BaseType.h"
#include "Math/Matrix.h"
#include "Math/Quatern.h"

// work in progress (not much yet) - mdp

// general quat class
class Quat
{
public:
	// the elements
	float		e[4];					// u0, u1, u2, w

	inline Quat(void) {}
	inline Quat(const Quat &q);
	inline Quat(float u0, float u1, float u2, float w);
	inline Quat(const ts_Quatvw &q);
	inline Quat(const Matrix3x3 &m);
	inline Quat(const Matrix4x4 &m);
	inline Quat(float roll, float pitch, float yaw);	//build Quaternion from Euler Angles
	inline Quat(Vector3CRef a, Vector3CRef b, const float epsilon = Math::Epsilon);	//build Quaternion to rotate from a to b
	inline ~Quat() {}
	inline void Set(float u0, float u1, float u2, float w);
	inline void Set(Vector3 axis, float angle);
	inline void Set(const ts_Quatvw &q);
	inline void Set(float roll, float pitch, float yaw);	//set Quaternion from Euler Angles
	inline void Set(Vector3CRef a, Vector3CRef b, const float epsilon = Math::Epsilon);	//set Quaternion to rotate from a to b
	inline void Identity(void);
	inline void Normalize( void );
	inline DirCos3x3 ConvertToD3x3(void) const;
	inline Cartesian4x4 ConvertToC4x4(void) const;
	inline Quat Slerp(Quat &qTo, float rPercent) const;
	inline Quat Lerp(Quat &qTo, float rPercent) const;
	inline Quat Slerp(Quat &qFrom, Quat &qTo, float rPercent) const;
	inline Quat Lerp(Quat &qFrom, Quat &qTo, float rPercent) const;
	inline Quat operator*(const Quat &q) const;
	inline Quat &operator=(const Quat &q);
	inline Quat &operator=(const ts_Quatvw &q);
	inline Quat &operator=(const Matrix3x3 &m);
	inline Quat &operator=(const Matrix4x4 &m);
	inline operator ts_Quatvw*(void) const;
	inline operator ts_Quatvw(void) const;
	inline operator Vector3*(void) const;
	inline operator Vector3(void) const;
};

// The implementation
#include "linearAlgebra/cquat.hpp"

#endif
