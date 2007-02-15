#pragma once

#define HASH_MATRIX_4X4_H

#ifndef HASH_VECTOR4_H
#include "HashVector4.h"
#endif

class HashMatrix4x4
{
public:
	HashMatrix4x4(void);
   HashMatrix4x4(const float mat[4][4]);
	HashMatrix4x4(const double mat[4][4]);
   HashMatrix4x4( double f00, double f01, double f02, double f03,
                  double f10, double f11, double f12, double f13,
                  double f20, double f21, double f22, double f23,
                  double f30, double f31, double f32, double f33 );
   HashMatrix4x4(float *pMatrix44);
	HashMatrix4x4(const HashVector4 v0,const HashVector4 v1,const HashVector4 v2, const HashVector4 v3);
	HashMatrix4x4(const double scalar);
	HashMatrix4x4(const HashMatrix4x4 &other);
   HashMatrix4x4(const HashQuaternion &quat);
	~HashMatrix4x4() {};

	HashMatrix4x4        &operator = ( const HashMatrix4x4 &other );
	HashMatrix4x4        &operator += ( const HashMatrix4x4 &other );
	HashMatrix4x4        &operator -= ( const HashMatrix4x4 &other );
	HashMatrix4x4        &operator *= ( const HashMatrix4x4 &other );
	HashMatrix4x4        &operator *= ( double scalar );

	HashMatrix4x4        operator + (const HashMatrix4x4 &other) const;
	HashMatrix4x4        operator - (const HashMatrix4x4 &other) const;
	HashMatrix4x4        operator * (const HashMatrix4x4 &other) const;
	HashMatrix4x4        operator * (double scalar) const;

	bool			         operator == (const double scalar) const;
	bool			         operator == (const HashMatrix4x4 &other) const;
	bool			         operator != (const HashMatrix4x4 &other) const;

	HashVector3          operator * ( const HashVector3 &other ) const;
   HashVector4			   operator * ( const HashVector4 &other ) const;

	const HashVector4	   &operator[]( int row ) const { return v[row]; }
	HashVector4			   &operator[]( int row ) { return v[row]; }

	void			         Set(const double m[4][4]);

	void			         SetIdentity();
	HashMatrix4x4        GetIdentityMatrix();

	HashMatrix4x4        Transpose();
   HashMatrix4x4		   Inverse();

	HashVector3		      GetXAxis() const;
	HashVector3		      GetYAxis() const;
	HashVector3		      GetZAxis() const;

   HashVector3			   GetTranslation() const;

   friend HashMatrix4x4 GetXRotateMatrix( double anglerad );
   friend HashMatrix4x4 GetYRotateMatrix( double anglerad );
   friend HashMatrix4x4 GetZRotateMatrix( double anglerad );
   friend HashMatrix4x4 GetYXRotateMatrix( double yanglerad, double xanglerad );
   friend HashMatrix4x4 GetYXZRotateMatrix( double yanglerad, double xanglerad, double zanglerad );

   friend HashMatrix4x4 GetTranslationMatrix(const HashVector3 &other);

	friend HashMatrix4x4 GetScaleMatrix(const HashVector3 &other);

	friend void		      TransformVector( const Matrix44 &matrix, HashVector3 &v ) { v *= matrix; }

   void			         Decompose(HashVector3 &translation,HashVector3 &scale,HashVector3 &shear,HashVector3 &rotation);

private:

   static double	Det2x2(double _11, double _12,double _21, double _22);
   static double	Det3x3(double _11, double _12, double _13,double _21, double _22, double _23,double _31, double _32, double _33);
   double			Det4x4();
   void			   Adj4x4(HashMatrix4x4 &out);

public:
	HashVector4 v[4];
};

#include "HashMatrix4x4.hpp"

