#pragma once

#define HASH_MATRIX_3X3_H

#ifndef HASH_VECTOR3_H
#include "HashVector3.h"
#endif

class HashMatrix3x3
{
public:
	HashMatrix3x3(void);
	HashMatrix3x3(const double mat[3][3]);
	HashMatrix3x3(const HashVector3 v0,const HashVector3 v1,const HashVector3 v2);
	HashMatrix3x3(const double scalar);
	HashMatrix3x3(const HashMatrix3x3 &other);
	~HashMatrix3x3() {};

	HashMatrix3x3        &operator = ( const HashMatrix3x3 &other );
	HashMatrix3x3        &operator += ( const HashMatrix3x3 &other );
	HashMatrix3x3        &operator -= ( const HashMatrix3x3 &other );
	HashMatrix3x3        &operator *= ( const HashMatrix3x3 &other );
	HashMatrix3x3        &operator *= ( double scalar );

	HashMatrix3x3        operator + (const HashMatrix3x3 &other) const;
	HashMatrix3x3        operator - (const HashMatrix3x3 &other) const;
	HashMatrix3x3        operator * (const HashMatrix3x3 &other) const;
	HashMatrix3x3        operator * (double scalar) const;

	bool			         operator == (const double scalar) const;
	bool			         operator == (const HashMatrix3x3 &other) const;
	bool			         operator != (const HashMatrix3x3 &other) const;

	HashVector3          operator * ( const HashVector3 &other ) const;

	const HashVector3	   &operator[]( int row ) const { return v[row]; }
	HashVector3			   &operator[]( int row ) { return v[row]; }

	void			         Set(const double m[3][3]);

	void			         SetIdentity();
	HashMatrix3x3        GetIdentity();

	HashMatrix3x3        Transpose();
	void			         TransposeSelf();

	HashVector3		      GetXAxis() const;
	HashVector3		      GetYAxis() const;
	HashVector3		      GetZAxis() const;

	static HashMatrix3x3	GetRotateXMatrix(const double anglerad);
	static HashMatrix3x3	GetRotateYMatrix(const double anglerad);
	static HashMatrix3x3	GetRotateZMatrix(const double anglerad);

	static HashMatrix3x3	GetRotateYXMatrix(const double anglerad);
	static HashMatrix3x3	GetRotateZXMatrix(const double anglerad);
	static HashMatrix3x3	GetRotateYXZMatrix(const double anglerad);

	void			         Scale(const HashVector3 &other);
	static HashMatrix3x3 GetScaleMatrix(const HashVector3 &other);

	friend void		      TransformVector( const HashMatrix3x3 &matrix, HashVector3 &vect ) { vect = matrix * vect; }

	static HashMatrix3x3 GetRotateAngleAxis(double anglerad,HashVector3 axis);
	static HashMatrix3x3 GetRotate2VMatrix(HashVector3 from, HashVector3 to);
	static HashMatrix3x3 VectorsToStandard(const HashVector3 &becomezdir,const HashVector3 &becomeydir);

public:
	HashVector3 v[3];
};

#include "HashMatrix3x3.hpp"

