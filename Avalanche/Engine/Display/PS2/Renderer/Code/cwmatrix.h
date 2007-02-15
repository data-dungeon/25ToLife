//
// NOTICE!  this is intentionally not the engines math library, although it looks similar
// This is intentionally simple.  Its designed to simulate the SN version of this stuff.
// Please only add methods that match those in the sn code unless you plan on adding an
// optimized version into the sn code.
//
// DO NOT, UNDER ANY CIRCUMSTANCE, USE x,y,z,w IN Vector4 OUTSIDE OF THIS FILE!  ON PAIN OF DEATH!
//
// Talk to Tyler if you need to know why.
//

// THIS IS ALL PRELIMINARY... I plan on changing it all around after I get a moment, hopefully later
// this month (apr 03)

#ifndef __THIS_HEADER_SUCKS_H__
#define __THIS_HEADER_SUCKS_H__

#include <math.h>
#include <string.h>

class Vector4
{
	// TJC - this stuff is intentionally private... dont change it please
	float& operator[] (int index) {
		switch(index){
		case 0: return x;
		case 1: return y;
		case 2: return z;
		case 3: return w;
		}
		return x;
	}
	// TJC - this stuff is intentionally private... dont change it please
	float operator[] (int index) const {
		switch(index){
		case 0: return x;
		case 1: return y;
		case 2: return z;
		case 3: return w;
		}
		return x;
	}
	friend class Matrix4x4;
public:
	union {
		u_long128 vec128;
		struct {
			float x,y,z,w;
		};
	};

	Vector4(){}
	Vector4(float _x, float _y, float _z, float _w) { set(_x,_y,_z,_w); }

	float get_x() const { return x; }
	float get_y() const { return y; }
	float get_z() const { return z; }
	float get_w() const { return w; }

	void set(float _x, float _y, float _z, float _w) { x = _x; y = _y; z = _z; w = _w; }
	void set_x(float _x) { x = _x; }
	void set_y(float _y) { y = _y; }
	void set_z(float _z) { z = _z; }
	void set_w(float _w) { w = _w; }

	float dot(Vector4 v) const
	{
		return x * v.x + y * v.y + z * v.z + w * v.w;
	}
	float mag_sq() const
	{
		return dot(*this);
	}
	float mag() const
	{
		return sqrtf(mag_sq());
	}
	Vector4 operator *(float scale) const
	{
		return Vector4(x*scale, y*scale, z*scale, w*scale);
	}
	Vector4 operator-() const
	{	
		Vector4 result;
		result.x = -x;
		result.y = -y;
		result.z = -z;
		result.w = -w;
		return result;
	}
	float length() const
	{
		return mag();
	}
	Vector4 normalized() const
	{
		Vector4 result;
		float _mag = mag();
		return *this * (1.0f / _mag);
	}
	void operator *= (float scale)
	{
		*this = *this * scale;
	}
	void operator=(Vector4 v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
		w = v.w;
	}
	Vector4 operator+ (Vector4 v)
	{
		Vector4 result;
		result.x = x + v.x;
		result.y = y + v.y;
		result.z = z + v.z;
		result.w = w + v.w;
		return result;
	}

	void operator+= (Vector4 v)
	{
		*this = *this + v;
	}
};

typedef Vector4 FloatColor;
typedef Vector4 vec_xyzw;

class Point : public Vector4
{
};

class Vector : public Vector4
{
};

class Matrix4x4
{
public:
	Vector4 col0, col1, col2, col3;

	operator Vector4* () { return &col0; }
	Vector4& operator[](int index)
	{
		switch(index)
		{
		case 0: return col0; 
		case 1: return col1; 
		case 2: return col2; 
		case 3: return col3; 
		}
		return col0;
	}
	const Vector4& operator[](int index) const { 
		switch(index)
		{
		case 0: return col0; 
		case 1: return col1; 
		case 2: return col2; 
		case 3: return col3; 
		}
		return col0;
	}
	const Vector4& operator[](unsigned int index) const { return operator[]((int)index); }
	Vector4& operator[](unsigned int index) { return operator[]((int)index); }
	void set_zero()
	{
		memset(this, 0, sizeof(*this));
	}
	void operator = (const Matrix4x4& m)
	{
		col0 = m.col0;
		col1 = m.col1;
		col2 = m.col2;
		col3 = m.col3;
	}
	void set_identity() {
		col0.set(1.0f, 0.0f, 0.0f, 0.0f);
		col1.set(0.0f, 1.0f, 0.0f, 0.0f);
		col2.set(0.0f, 0.0f, 1.0f, 0.0f);
		col3.set(0.0f, 0.0f, 0.0f, 1.0f);
	}
	Matrix4x4 operator* (const Matrix4x4& m) const
	{
		Matrix4x4 result;
		sceVu0MulMatrix(*(sceVu0FMATRIX*)&result, *(sceVu0FMATRIX*)&m, *(sceVu0FMATRIX*)this);
		return result;
	}
	Vector4 operator*(Vector4 v)
	{
		Vector4 result;
		result.x = v.x * col0.x + v.y * col1.x + v.z * col2.x + v.w * col3.x;
		result.y = v.x * col0.y + v.y * col1.y + v.z * col2.y + v.w * col3.y;
		result.z = v.x * col0.z + v.y * col1.z + v.z * col2.z + v.w * col3.z;
		result.w = v.x * col0.w + v.y * col1.w + v.z * col2.w + v.w * col3.w;
		return result;
	}

	void set(Vector4 v0, 
		Vector4 v1, 
		Vector4 v2, 
		Vector4 v3)
	{
		col0 = v0;
		col1 = v1;
		col2 = v2;
		col3 = v3;
	}

	Matrix4x4 transpose() const
	{
		Matrix4x4 m;
		for(int i = 0; i < 4; i++)
			for(int j = 0; j < 4; j++)
				m[i][j] = (*this)[j][i];
		return m;
	}
	void transpose_in_place() 
	{
		*this = transpose();
	}
	Matrix4x4 inverse() const
	{
		Matrix4x4 m;
		Invert(m, *this);
		return m;
	}

protected:
	/***************************************************************************/
	// Return the det of a 3x3
	/***************************************************************************/
	static float Determinant( 	float _11, float _12, float _21, float _22)
	{
		return( _11 * _22 - _21 * _12);
	}

	static float Determinant( 	float _11, float _12, float _13, 
								float _21, float _22, float _23, 
								float _31, float _32, float _33)
	{
		return(_11 * Determinant(_22, _23, _32, _33) -
				 _12 * Determinant(_21, _23, _31, _33) +
				 _13 * Determinant(_21, _22, _31, _32));
	}

	/***************************************************************************/
	// Return the det of a 4x4
	/***************************************************************************/
	static float Determinant( 	float _11, float _12, float _13, float _14, 
								float _21, float _22, float _23, float _24, 
								float _31, float _32, float _33, float _34, 
								float _41, float _42, float _43, float _44)
	{
		return(_11 * Determinant(_22, _23, _24, _32, _33, _34, _42, _43, _44) -
				 _12 * Determinant(_21, _23, _24, _31, _33, _34, _41, _43, _44) +
				 _13 * Determinant(_21, _22, _24, _31, _32, _34, _41, _42, _44) -
				 _14 * Determinant(_21, _22, _23, _31, _32, _33, _41, _42, _43));
	}

	static float Determinant( const Matrix4x4 &Src)
	{
		return( Determinant( Src[0].x, Src[0].y, Src[0].z, Src[0].w,
									Src[1].x, Src[1].y, Src[1].z, Src[1].w,
									Src[2].x, Src[2].y, Src[2].z, Src[2].w,
									Src[3].x, Src[3].y, Src[3].z, Src[3].w));
	}
								
		
	inline static void Adjoint( Matrix4x4 &Dest, const Matrix4x4 &Src)
	{

		Dest[0].x =  Determinant(Src[1].y, Src[1].z, Src[1].w, Src[2].y, Src[2].z, Src[2].w, Src[3].y, Src[3].z, Src[3].w);
		Dest[1].x = -Determinant(Src[1].x, Src[1].z, Src[1].w, Src[2].x, Src[2].z, Src[2].w, Src[3].x, Src[3].z, Src[3].w);
		Dest[2].x =  Determinant(Src[1].x, Src[1].y, Src[1].w, Src[2].x, Src[2].y, Src[2].w, Src[3].x, Src[3].y, Src[3].w);
		Dest[3].x = -Determinant(Src[1].x, Src[1].y, Src[1].z, Src[2].x, Src[2].y, Src[2].z, Src[3].x, Src[3].y, Src[3].z);

		Dest[0].y = -Determinant(Src[0].y, Src[0].z, Src[0].w, Src[2].y, Src[2].z, Src[2].w, Src[3].y, Src[3].z, Src[3].w);
		Dest[1].y =  Determinant(Src[0].x, Src[0].z, Src[0].w, Src[2].x, Src[2].z, Src[2].w, Src[3].x, Src[3].z, Src[3].w);
		Dest[2].y = -Determinant(Src[0].x, Src[0].y, Src[0].w, Src[2].x, Src[2].y, Src[2].w, Src[3].x, Src[3].y, Src[3].w);
		Dest[3].y =  Determinant(Src[0].x, Src[0].y, Src[0].z, Src[2].x, Src[2].y, Src[2].z, Src[3].x, Src[3].y, Src[3].z);

		Dest[0].z =  Determinant(Src[0].y, Src[0].z, Src[0].w, Src[1].y, Src[1].z, Src[1].w, Src[3].y, Src[3].z, Src[3].w);
		Dest[1].z = -Determinant(Src[0].x, Src[0].z, Src[0].w, Src[1].x, Src[1].z, Src[1].w, Src[3].x, Src[3].z, Src[3].w);
		Dest[2].z =  Determinant(Src[0].x, Src[0].y, Src[0].w, Src[1].x, Src[1].y, Src[1].w, Src[3].x, Src[3].y, Src[3].w);
		Dest[3].z = -Determinant(Src[0].x, Src[0].y, Src[0].z, Src[1].x, Src[1].y, Src[1].z, Src[3].x, Src[3].y, Src[3].z);

		Dest[0].w = -Determinant(Src[0].y, Src[0].z, Src[0].w, Src[1].y, Src[1].z, Src[1].w, Src[2].y, Src[2].z, Src[2].w);
		Dest[1].w =  Determinant(Src[0].x, Src[0].z, Src[0].w, Src[1].x, Src[1].z, Src[1].w, Src[2].x, Src[2].z, Src[2].w);
		Dest[2].w = -Determinant(Src[0].x, Src[0].y, Src[0].w, Src[1].x, Src[1].y, Src[1].w, Src[2].x, Src[2].y, Src[2].w);
		Dest[3].w =  Determinant(Src[0].x, Src[0].y, Src[0].z, Src[1].x, Src[1].y, Src[1].z, Src[2].x, Src[2].y, Src[2].z);

	}
	static bool Invert( Matrix4x4 &Dest, const Matrix4x4 &Src)
	{

		float Det = Determinant( Src);
		
		if( Det == 0.0f)
			return false;

		float InvDet = 1.0f / Det;
				
		Adjoint( Dest, Src);
		
		for( int i = 0; i < 4; i++)
			Dest[i] *= InvDet;
		
		return true;
	}

};

typedef Matrix4x4 Graphics4x4;

#endif // __THIS_HEADER_SUCKS_H__
