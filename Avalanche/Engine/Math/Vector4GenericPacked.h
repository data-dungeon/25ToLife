#ifndef __VECTOR4_GENERIC_PACKED_H__
#define __VECTOR4_GENERIC_PACKED_H__

class Vector4PackedData
{
protected:
	friend class Vector4Template<Vector4PackedData>;
	friend class Vector4Template<Vector4AlignedData>;

public:
	float _x, _y, _z, _w;

	float x() const { return _x; }
	float y() const { return _y; }
	float z() const { return _z; }
	float w() const { return _w; }
	void x(float x) { _x = x; }
	void y(float y) { _y = y; }
	void z(float z) { _z = z; }
	void w(float w) { _w = w; }

	// TJC - new names
	float X() const { return _x; }
	float Y() const { return _y; }
	float Z() const { return _z; }
	float W() const { return _w; }
	void X(float x) { _x = x; }
	void Y(float y) { _y = y; }
	void Z(float z) { _z = z; }
	void W(float w) { _w = w; }
};

#endif __VECTOR4_GENERIC_PACKED_H__
