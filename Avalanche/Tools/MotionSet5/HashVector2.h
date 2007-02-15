//////////////////////////////////////////////////////////////////////////////////////////
//
//	HashVector2 CLASS
//
//////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#define HASH_VECTOR2_H

class Quaternion;

class HashVector2
{
public:

	HashVector2();
	HashVector2(const double X, const double Y);
	HashVector2(const HashVector2 &other);
	HashVector2(const double other);
	~HashVector2();

	HashVector2				&operator = (const HashVector2 &other);
	HashVector2				&operator += (const HashVector2 &other);
	HashVector2				&operator -= (const HashVector2 &other);
	HashVector2				&operator /= (const HashVector2 &other);
	HashVector2				&operator *= (const HashVector2 &other);
	HashVector2				&operator *= (const Quaternion &q);
	HashVector2				&operator *= (const double scale);
	HashVector2				&operator /= (const double scale);

	HashVector2				operator + (const HashVector2 &other) const;
	HashVector2				operator - (const HashVector2 &other) const;
	HashVector2				operator - (void) const;
	HashVector2				operator * (const HashVector2 &other) const;
	HashVector2				operator * (const double d) const;
	HashVector2				operator / (const double d) const;
	HashVector2				operator / (const HashVector2 &other) const;
	double				   operator | (const HashVector2 &other) const;

	bool				   operator == (const double other) const;
	bool				   operator == (const HashVector2 &other) const;
	bool				   operator != (const HashVector2 &other) const;
	bool				   operator < (const HashVector2 &other) const;

	double				operator[]( const int index ) const;
	double				&operator[]( const int index );

	void				   Set(const double X, const double Y);
	double				Dot(const HashVector2 &other) const;
	HashVector2			Absolute() const;
	HashVector2			Reciprocal() const;
	HashVector2			&Normalize();
	HashVector2			Normalized() const;
	double				LengthSq() const;
	double				Length() const;
	HashVector2			&Negate();
   double				DistanceTo(const HashVector2 &p) const;
	double	         Sum(void) const;
	friend double		Dot(const HashVector2 &a, const HashVector2 &b);

	friend HashVector2 operator * (const double s, const HashVector2 &a);

	static void Tween(HashVector2 &pointFill,const HashVector2 &point1,const HashVector2 &point2,double percent);
	static void Tween(HashVector2 &pointFill,const HashVector2 &point1,const HashVector2 &control1,const HashVector2 &control2,const HashVector2 &point2,double percent);

	static void Slerp(HashVector2 &pointFill,const HashVector2 &point1,const HashVector2 &point2,double percent);
	static void Sberp(HashVector2 &pointFill,const HashVector2 &point1,const HashVector2 &control1,const HashVector2 &control2,const HashVector2 &point2,double percent);

	////////////////////////////////////////////////////////////////////////////////////////
	// CLASS VARIABLES
	////////////////////////////////////////////////////////////////////////////////////////

	double x,y;
};


#include "HashVector2.hpp"

