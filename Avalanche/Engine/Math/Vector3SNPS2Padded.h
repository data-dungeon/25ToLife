#ifndef __VECTOR3_SN_PS2_PADDED_H__
#define __VECTOR3_SN_PS2_PADDED_H__

class Vector4X;
class Vector4Y;
class Vector4Z;
class Vector4W;

class Vector3PaddedData
{
protected:
	friend Vector3Packed;
	friend Vector3Padded;

	u128 _vec;

public:
	Vector4X X() const;
	Vector4Y Y() const;
	Vector4Z Z() const;
	Vector4W W() const;
	u32 Pad() const;
	void Pad(u32);

	void X(Vector4X x);
	void X(Vector4Y x);
	void X(Vector4Z x);
	void X(Vector4W x);
	void X(float x);

	void Y(Vector4X y);
	void Y(Vector4Y y);
	void Y(Vector4Z y);
	void Y(Vector4W y);
	void Y(float y);

	void Z(Vector4X z);
	void Z(Vector4Y z);
	void Z(Vector4Z z);
	void Z(Vector4W z);
	void Z(float z);

	void W(Vector4X w);
	void W(Vector4Y w);
	void W(Vector4Z w);
	void W(Vector4W w);
	void W(float w);

	// TJC - old names

	Vector4X x() const;
	Vector4Y y() const;
	Vector4Z z() const;
	Vector4W w() const;
	u32 pad() const;
	void pad(u32);

	void x(Vector4X x);
	void x(Vector4Y x);
	void x(Vector4Z x);
	void x(Vector4W x);
	void x(float x);

	void y(Vector4X y);
	void y(Vector4Y y);
	void y(Vector4Z y);
	void y(Vector4W y);
	void y(float y);

	void z(Vector4X z);
	void z(Vector4Y z);
	void z(Vector4Z z);
	void z(Vector4W z);
	void z(float z);

	void w(Vector4X w);
	void w(Vector4Y w);
	void w(Vector4Z w);
	void w(Vector4W w);
	void w(float w);


	u128 vec() const { return _vec; }
	void vec(u128 vec) { _vec = vec; }

	// init x, overwritting yzw
	// used internally for some stuff..
	void InitX(Vector4X x);
} __attribute__((aligned(VU_ALIGN)));



#endif __VECTOR3_SN_PS2_PADDED_H__
