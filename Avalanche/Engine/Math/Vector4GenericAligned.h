#ifndef __VECTOR4_GENERIC_ALIGNED_H__
#define __VECTOR4_GENERIC_ALIGNED_H__

class Vector4AlignedData
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

	void X(const Vector4AlignedData& x) { _x = x.X(); }
	void Y(const Vector4AlignedData& y) { _y = y.Y(); }
	void Z(const Vector4AlignedData& z) { _z = z.Z(); }
	void W(const Vector4AlignedData& w) { _w = w.W(); }

};

#endif __VECTOR4_GENERIC_ALIGNED_H__
