/***************************************************************************/
// A 2d vector class that is memory footprint compatible with an x,y float
// pair.
//
// this file should not be included explicitly-- please use Math\Vector.h
/***************************************************************************/
#ifndef M_VECT2_H
#define M_VECT2_H

class Vector2Data
{
	// TJC - ICK!  these have to be PUBLIC to assign to one of these
	// with a "= { 0.0f, 0.0f}" type construct in MSVC... 
public:
	float _x, _y;

public:
	inline float x() const { return _x; }
	inline float y() const { return _y; }
	inline void x(float x) { _x = x; }
	inline void y(float y) { _y = y; }

	// TJC - new names
	inline float X() const { return _x; }
	inline float Y() const { return _y; }
	inline void X(float x) { _x = x; }
	inline void Y(float y) { _y = y; }
};

// Vector2 class.
class Vector2 : public Vector2Data
{
public:

//xxx ned says: supplying constructors and assignment operators screws up 2 things: using the vector in a union,
//xxx and trying to initialize a structure that contains a vector. this is apparently a known issue in c++.
//xxx so no constructors or operator= in vectors!
	// TJC -  ya, but its REALLY bad to do this.  the compiler has a
	// cow in the code it generates.  So we really do need these...
	// TJC -  a better way to impliment the solution would be what I
	// did to the Vector3 and will probably eventually do here.  That
	// is make an 'unaligned' version of this class that has the data
	// in a format that supports unions
	// Our various constructors
	inline Vector2(void) {}
	inline Vector2(float x, float y);
	inline Vector2(Vector2Data& d) { _x = d.x(); _y = d.y(); }

	// get/set at the values
	inline void Clear(void);
	inline void Set(float x, float y);
	inline void Set(const float *xyArray);
	
	// Special functions
	inline float Dot(const Vector2 &v) const;
	inline Vector2 Diag(const Vector2 &v) const;
	inline float Length(void) const;
	inline float LengthSquared(void) const;
	inline void Normalize(void);
	inline Vector2 Normalized(void) const;
	inline Vector2 Projection(const Vector2 &v);
	inline void Negate(void);

	// Static versions
	inline static float Dot(const Vector2 &v1, const Vector2 &v2);
	inline static Vector2 Diag(const Vector2 &v1, const Vector2 &v2);
	inline static float Length(const Vector2 &v);
	inline static void Normalize(Vector2 &v);

	// operators!!
	inline int operator==(const Vector2 &v) const;
	inline int operator!=(const Vector2 &v) const;
//xxx ned says: supplying constructors and assignment operators screws up 2 things: using the vector in a union,
//xxx and trying to initialize a structure that contains a vector. this is apparently a known issue in c++.
//xxx so no constructors or operator= in vectors!
//xxx	inline Vector2 &operator=(const Vector2 &v);
//xxx	inline Vector2 &operator=(float f);
//xxx	inline Vector2 &operator=(int i);

	inline Vector2 &operator+=(const Vector2 &v);
	inline Vector2 &operator-=(const Vector2 &v);
	inline Vector2 &operator*=(float scale);
	inline Vector2 &operator/=(float scale);

	inline Vector2 operator+(const Vector2 &v) const;
	inline Vector2 operator-(const Vector2 &v) const;
	inline Vector2 operator*(float scale) const;
	inline Vector2 operator/(float scale) const;

	inline Vector2 operator-(void) const;

	// Casts
	inline operator float*(void);

	// Byte-swapping for Gamecube
#ifdef GCN
	inline void EndianSwap(void);
#endif
};

#endif
