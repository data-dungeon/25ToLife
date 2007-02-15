//////////////////////////////////////////////////////////////////////////////////////////
//
//	Vector CLASS
//
//////////////////////////////////////////////////////////////////////////////////////////

#ifndef __Vector_H
#define __Vector_H


#define DEGTORAD  (3.14 / 180.0)
#define RADTODEG  (180.0 / 3.14)

class Vector4;
class Quaternion;
class Matrix33;
class Matrix44;

class Vector
{
public:

	Vector();
	Vector(const double X, const double Y, const double Z);
	Vector(const Vector &other);
	Vector(const double scalar);
	~Vector();

	Vector				&operator = (const Vector &other);
	Vector				&operator += (const Vector &other);
	Vector				&operator -= (const Vector &other);
	Vector				&operator /= (const Vector &other);
	Vector				&operator *= (const Vector &other);
	Vector				&operator ^= (const Vector &other);

	Vector				&operator *= (const Quaternion &q);
	
	Vector				&operator *= (const double scalar);
	Vector				&operator /= (const double scalar);

	Vector				&operator *= (const Matrix33 &other);
	Vector				&operator *= (const Matrix44 &other);

	Vector				operator + (const Vector &other) const;
	Vector				operator - (const Vector &other) const;
	Vector				operator - (void) const;
	Vector				operator * (const Vector &other) const;
	Vector				operator * (const double scalar) const;
	Vector				operator / (const double scalar) const;
	Vector				operator / (const Vector &other) const;
	double				operator | (const Vector &other) const;
	Vector				operator ^ (const Vector &other) const;
	
	bool				operator == (const double other) const;
	bool				operator == (const Vector &other) const;
	bool				operator != (const Vector &other) const;
	bool				operator < (const Vector &other) const;

	double				operator[]( const int index ) const;
	double				&operator[]( const int index );

	void				Set(const double X, const double Y, const double Z);
	
	Vector				Cross(const Vector &other) const;
	double				Dot(const Vector &other) const;
	double				Square() const;
	Vector				Absolute() const;
	Vector				Reciprocal() const;
	void				CrossSelf(const Vector &other);

	Vector				&Normalize();
	Vector				Normalized() const;
	double				NormalizeLength();

	double				LengthSq() const;
	double				Length() const;

	Vector				&Negate();

	bool				IsParallel(const Vector &other) const;
	bool				IsParallel(const Vector &other, const double tol) const;
	bool				IsSameDirection(const Vector &other, const double tol) const;
	bool				IsParallelAndSameDirection(const Vector &other,const double tol);

	double				DistanceTo(const Vector &other) const;
	bool				IsWithinTolerance(const Vector &other,const double tol);

	double				AngleBetweenVectors(const Vector &other);

	void				GetRotation(Vector &rotation);//Warning: Needs to be reevaluated.

	friend Vector		operator * (const double scalar, const Vector &other);
	friend Vector		Cross(const Vector &a, const Vector &b);
	friend double		Dot(const Vector &a, const Vector &b);
		
	////////////////////////////////////////////////////////////////////////////////////////
	// CLASS VARIABLES
	////////////////////////////////////////////////////////////////////////////////////////

	struct { double x, y, z; };
};

#include "Vector.hpp"

#endif
