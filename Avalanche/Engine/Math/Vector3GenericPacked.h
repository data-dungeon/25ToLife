#ifndef __VECTOR3_GENERIC_PACKED_H__
#define __VECTOR3_GENERIC_PACKED_H__

class Vector3PackedData
{
public:
	friend class Vector3Template<Vector3PackedData>;
	friend class Vector3Template<Vector3PaddedData>;

	// TJC -  ICK!!! this shouldnt have to be public, but there are
	// certain instances in the code now that require it.. namely,
	// watch client.. ICK

	float _x, _y, _z;

public:
	float x() const { return _x; }
	float y() const { return _y; }
	float z() const { return _z; }
	void x(float x) { _x = x; }
	void y(float y) { _y = y; }
	void z(float z) { _z = z; }

	// TJC -  new names
	float X() const { return _x; }
	float Y() const { return _y; }
	float Z() const { return _z; }
	void X(float x) { _x = x; }
	void Y(float y) { _y = y; }
	void Z(float z) { _z = z; }
};

#endif __VECTOR3_GENERIC_PACKED_H__
