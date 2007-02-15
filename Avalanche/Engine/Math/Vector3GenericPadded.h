#ifndef __VECTOR3_GENERIC_PADDED_H__
#define __VECTOR3_GENERIC_PADDED_H__

#include "Math/MathClass.h"

class Vector3PaddedData
{
protected:
	friend class Vector3Template<Vector3PackedData>;
	friend class Vector3Template<Vector3PaddedData>;

//	u128 vec;
	float _x, _y, _z;
	u32 _pad;

public:
	float x() const { return _x; }
	float y() const { return _y; }
	float z() const { return _z; }
	float w() const { return Math::IntAsFloat(_pad); }
	u32 pad() const { return _pad; }
	void x(float x) { _x = x; }
	void y(float y) { _y = y; }
	void z(float z) { _z = z; }
	void w(float w) { _pad = Math::FloatAsInt(w); }
	void pad(u32 pad) { _pad = pad; }

	// TJC -  new names
	float X() const { return _x; }
	float Y() const { return _y; }
	float Z() const { return _z; }
	float W() const { return Math::IntAsFloat(_pad); }
	u32 Pad() const { return _pad; }
	void X(float x) { _x = x; }
	void Y(float y) { _y = y; }
	void Z(float z) { _z = z; }
	void W(float w) { _pad = Math::FloatAsInt(w); }
	void Pad(u32 pad) { _pad = pad; }
};
#endif __VECTOR3_GENERIC_PADDED_H__
