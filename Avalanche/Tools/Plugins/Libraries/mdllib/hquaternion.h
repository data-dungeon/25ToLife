#ifndef __HQUATERNION_H
#define __HQUATERNION_H

#ifndef __HVECTOR_H
#include "hvector.h"
#endif

#define RADIANS			57.29578f

class HVector;

class HQuaternion
{
public:
	HQuaternion() { w = 0; };

	HQuaternion( const HVector &vec, double scalar );
	HQuaternion( double x, double y, double z, double s );
	HQuaternion( const HQuaternion &q );
	HQuaternion( double radangle, const HVector &axis );
	void					SetAngleAxis( double radangle, const HVector &axis );

	friend HQuaternion		IdentityQ() { return HQuaternion( 0.0f,0.0f,0.0f,1.0f ); }
	friend void				TransformVector( const HQuaternion &q, HVector &v ) { v *= q; }
	friend float			Dot( const HQuaternion &a, const HQuaternion &b ) { return (float)(dot(a.v,b.v) + a.w*b.w); };

	friend HQuaternion		RotateVQ( double angle, const HVector &axis );
	friend HQuaternion		XRotateQ( float angle ){ return HQuaternion( angle / RADIANS, HVector(1.0f,0.0f,0.0f)); }
	friend HQuaternion		YRotateQ( float angle ){ return HQuaternion( angle / RADIANS, HVector(0.0f,1.0f,0.0f)); }
	friend HQuaternion		ZRotateQ( float angle ){ return HQuaternion( angle / RADIANS, HVector(0.0f,0.0f,1.0f)); }
	friend HQuaternion		XRotateRadiansQ( float anglerad ) { return HQuaternion( anglerad, HVector(1.0f,0.0f,0.0f)); }
	friend HQuaternion		YRotateRadiansQ( float anglerad ) { return HQuaternion( anglerad, HVector(0.0f,1.0f,0.0f)); }
	friend HQuaternion		ZRotateRadiansQ( float anglerad ) { return HQuaternion( anglerad, HVector(0.0f,0.0f,1.0f)); }

	float					GetAngle() const  { return (float)acos( w ) * 2.0f; }
	HVector					GetAxis() const;

	float					Norm() const;
	HQuaternion				&Normalize();
	HQuaternion				Normalized() const;
	float					NormalizeLength( void );

	HQuaternion				Inverse() const;
	HQuaternion				Conjugate() const;
	HQuaternion				&Negate( void );

	int operator == ( const HQuaternion &a ) const;
	int operator != ( const HQuaternion &a ) const;  
	HQuaternion &operator *= ( const HQuaternion &q );
	HQuaternion &operator *= ( double scalar );
	HQuaternion &operator /= ( double scalar );
	HQuaternion &operator += ( const HQuaternion &q2 );
	HQuaternion operator - ( void ) const;
	double &operator[]( int i );
   
	HQuaternion operator * ( const HQuaternion &b ) const;
	HQuaternion operator * ( double scalar ) const;

	friend HQuaternion operator * ( double scalar, const HQuaternion &q );

	HQuaternion operator / ( double scalar ) const;
	HQuaternion operator + ( const HQuaternion &b ) const;
	HQuaternion operator - ( const HQuaternion &b ) const;
	HQuaternion &operator = ( const HQuaternion &other );

	HVector operator * ( const HVector &v ) const;

public:
	HVector v;
	double w;
};

#include "hquaternion.hpp"

#endif