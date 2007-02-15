//////////////////////////////////////////////////////////////////////////////////////////
//
//	HashVector3 CLASS
//
//////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#define HASH_VECTOR3_H

#define DEGTORAD  (3.141592654f / 180.0f)
#define RADTODEG  (180.0f / 3.141592654f)

//class Vector4;
class HashQuaternion;
class HashMatrix3x3;
class Matrix44;

class HashVector3
{
public:

	HashVector3();
	HashVector3(const double X, const double Y, const double Z);
	HashVector3(const HashVector3 &other);
	HashVector3(const double scalar);
	~HashVector3();

	HashVector3				&operator = (const HashVector3 &other);
	HashVector3				&operator += (const HashVector3 &other);
	HashVector3				&operator -= (const HashVector3 &other);
	HashVector3				&operator /= (const HashVector3 &other);
	HashVector3				&operator *= (const HashVector3 &other);
	HashVector3				&operator ^= (const HashVector3 &other);

	HashVector3				&operator *= (const HashQuaternion &q);

	HashVector3				&operator *= (const double scalar);
	HashVector3				&operator /= (const double scalar);

	HashVector3				&operator *= (const HashMatrix3x3 &other);
	HashVector3				&operator *= (const Matrix44 &other);

	HashVector3				operator + (const HashVector3 &other) const;
	HashVector3				operator - (const HashVector3 &other) const;
	HashVector3				operator - (void) const;
	HashVector3				operator * (const HashVector3 &other) const;
	HashVector3				operator * (const double scalar) const;
	HashVector3				operator / (const double scalar) const;
	HashVector3				operator / (const HashVector3 &other) const;
	double				   operator | (const HashVector3 &other) const;
	HashVector3				operator ^ (const HashVector3 &other) const;

	bool				operator == (const double other) const;
	bool				operator == (const HashVector3 &other) const;
	bool				operator != (const HashVector3 &other) const;
	bool				operator < (const HashVector3 &other) const;

	double				operator[]( const int index ) const;
	double				&operator[]( const int index );

	void				   Set(const double X, const double Y, const double Z);

	HashVector3			Cross(const HashVector3 &other) const;
	double				Dot(const HashVector3 &other) const;
	double				Square() const;
	HashVector3			Absolute() const;
	HashVector3			Reciprocal() const;
	void				   CrossSelf(const HashVector3 &other);
	HashVector3			&Normalize();
	HashVector3			Normalized() const;
	double				NormalizeLength();
	double				LengthSq() const;
	double				Length() const;
	HashVector3			&Negate();
   double				DistanceTo(const HashVector3 &other) const;

	friend HashVector3 operator * (const double scalar, const HashVector3 &other);
	friend HashVector3 Cross(const HashVector3 &a, const HashVector3 &b);
	friend double      Dot(const HashVector3 &a, const HashVector3 &b);

	////////////////////////////////////////////////////////////////////////////////////////
	// CLASS VARIABLES
	////////////////////////////////////////////////////////////////////////////////////////

	struct { double x, y, z; };
};

#include "HashVector3.hpp"

