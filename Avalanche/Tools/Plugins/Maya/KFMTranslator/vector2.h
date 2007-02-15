//////////////////////////////////////////////////////////////////////////////////////////
//
//	Vector2 CLASS
//
//////////////////////////////////////////////////////////////////////////////////////////

#ifndef __Vector2_H
#define __Vector2_H

#include <math.h>

class Quaternion;

class Vector2
{
public:

	Vector2();
	Vector2(const double X, const double Y);
	Vector2(const Vector2 &other);
	Vector2(const double other);
	~Vector2();

	Vector2				&operator = (const Vector2 &other);
	Vector2				&operator += (const Vector2 &other);
	Vector2				&operator -= (const Vector2 &other);
	Vector2				&operator /= (const Vector2 &other);
	Vector2				&operator *= (const Vector2 &other);
	Vector2				&operator *= (const Quaternion &q);
	Vector2				&operator *= (const double scale);
	Vector2				&operator /= (const double scale);

	Vector2				operator + (const Vector2 &other) const;
	Vector2				operator - (const Vector2 &other) const;
	Vector2				operator - (void) const;
	Vector2				operator * (const Vector2 &other) const;
	Vector2				operator * (const double d) const;
	Vector2				operator / (const double d) const;
	Vector2				operator / (const Vector2 &other) const;
	double				operator | (const Vector2 &other) const;
	
	bool				operator == (const double other) const;
	bool				operator == (const Vector2 &other) const;
	bool				operator != (const Vector2 &other) const;
	bool				operator < (const Vector2 &other) const;

	double				operator[]( const int index ) const;
	double				&operator[]( const int index );

	void				Set(const double X, const double Y);

	double				Dot(const Vector2 &other) const;
	Vector2				Absolute() const;
	Vector2				Reciprocal() const;

	Vector2				&Normalize();
	Vector2				Normalized() const;

	double				Lengthsq() const;
	double				Length() const;

	Vector2				&Negate();

	double				DistanceTo(const Vector2 &p) const;

	friend double		Dot(const Vector2 &a, const Vector2 &b);
	friend Vector2		operator * (const double s, const Vector2 &a);

	////////////////////////////////////////////////////////////////////////////////////////
	// CLASS VARIABLES
	////////////////////////////////////////////////////////////////////////////////////////
	
	double x,y;
};


#include "Vector2.hpp"

#endif
