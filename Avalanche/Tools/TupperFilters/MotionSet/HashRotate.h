// RC  6/11/2002  \Randy95\Include\Rotate.h

#define __ROTATE_H

#ifndef HASH_VECTOR3_H
#include "HashVector3.h"
#endif
#ifndef HASH_QUATERNION_H
#include "HashQuaterni.h"
#endif
#ifndef HASH_MATRIX_4X4_H
#include "HashMatrix4x4.h"
#endif

class Rotate {
public:   
   double    		swivel, tilt, roll;
   //                x      z     y

   Rotate() {}
   Rotate( const double scalar ) { swivel = tilt = roll = scalar; }
   Rotate( const double pswivel, const double ptilt, const double proll )
    { swivel = pswivel; tilt = ptilt; roll = proll; }
   int operator == ( const Rotate &a ) const { return swivel==a.swivel && tilt == a.tilt && roll==a.roll; }
   int operator != ( const Rotate &a ) const { return !(*this == a); }   
};

class RotateSphere;
class RotateEuler;

class RotateVector3
{
public:
   HashVector3    		 end;
   double                roll;
   
   RotateVector3() {}
   RotateVector3( const HashVector3 &pend, double proll ) { end=pend; roll=proll; }
   ~RotateVector3() {}
   RotateVector3( const RotateSphere &sphere );
   RotateVector3( const RotateEuler &euler );
//   RotateVector3( const Matrix34 &matrix );
   RotateVector3( const HashQuaternion &quat );

   friend int operator != ( const RotateVector3 &a, const RotateVector3 &b )
    { return a.end!=b.end || a.roll!=b.roll; }
};

class RotateSphere
{
public:
   double                latitude,
                        longitude,
                        roll;
   RotateSphere() {}
   ~RotateSphere() {}
   RotateSphere( const HashVector3 &vec );
   RotateSphere( const RotateVector3 &rotateend );
   RotateSphere( const HashQuaternion &quat );
//   RotateSphere( const HashQuaternion &quat, Bone *bone );
};

class RotateEuler
{ 
public:   
   double    		x, y, z;

   RotateEuler() {}
   ~RotateEuler() {}
   RotateEuler( double px, double py, double pz ) { x=px; y=py; z=pz; }
//   RotateEuler( const Matrix34 &t );
   RotateEuler( const RotateVector3 &rotateend );
   RotateEuler( const HashQuaternion &quat );
//   void ToEuler( const Matrix34 &t );
   RotateEuler( const HashMatrix4x4 &matrix );
   RotateEuler operator * ( double scale ) { RotateEuler other; other.x = x * scale; other.y  = y * scale; other.z = z * scale; return other; }
   double operator[]( int i ) const
   {
      return (&x)[i];
   }
   double &operator[]( int i )
   {
      return (&x)[i];
   }
};

class TSR
{
public:
   HashVector3  		translate;
   HashVector3    	scale;
   RotateVector3		rotate;
   
   friend int operator != ( const TSR &a, const TSR &b )
    { return a.rotate!=b.rotate || a.translate!=b.translate || a.scale!=b.scale; }
   void SetDefault()
    { translate.Set(0.0f,0.0f,0.0f); scale.Set(1.0f,1.0f,1.0f); rotate.end.Set(0.0f,0.0f,1.0f); rotate.roll=0.0f; }
};

class TSQ
{
public:
   HashVector3  		translate;
   HashVector3    	scale;
   HashQuaternion  	quat;
   double            roll;
   BOOL              m_isrollcomputed;
   
   TSQ(){}
//   TSQ(const Matrix34 &actionmatrix);
   TSQ(const HashVector3 &ptranslate, const HashVector3 &pscale, const HashQuaternion &pquat)
   {
      translate = ptranslate;
      scale = pscale;
      quat = pquat;
   }
   
   int operator != ( const TSQ &b ) const
    { return quat!=b.quat || translate!=b.translate || scale!=b.scale; }
   void SetDefault()
    { translate.Set(0.0f,0.0f,0.0f); scale.Set(1.0f,1.0f,1.0f); quat.SetIdentity(); }
   TSQ &operator += ( const TSQ &other )
   {
      translate += other.translate;
      scale += other.scale;
      quat += other.quat;
      return *this;
   }
   TSQ &operator *= ( double scalar )
   {
      translate *= scalar;
      scale *= scalar;
      quat *= scalar;
      return *this;
   }
   void PostBlend()
   {
      quat.Normalize();
   }
//   void FillMatrices( Matrix34 &matrix, Matrix34 &inverse );
};

// ANY ROTATION TYPE TO AND FROM HashQuaternion

inline RotateEuler::RotateEuler( const HashQuaternion &quat )
{
   HashVector3 end( quat.GetZAxis());
   y = (double)(atan2( end.x, end.z ) * RADIANS);
   x = (double)(atan2( -end.y, sqrt(end.x*end.x + end.z*end.z)) * RADIANS);
   HashVector3 rolldir( quat.GetYAxis());
   rolldir *= YXRotateQ( y, x ).Inverse();
   z = (double)(atan2( -rolldir.x, rolldir.y ) * RADIANS);
}

inline RotateEuler::RotateEuler( const HashMatrix4x4 &matrix )
{
   HashVector3 end = matrix.GetZAxis();
   y = (double)atan2( end.x, end.z );
   x = (double)atan2( -end.y, sqrt(end.x*end.x + end.z*end.z));
   HashVector3 rolldir = matrix.GetYAxis();
   rolldir *= GetYXRotateMatrix( y, x ).Transpose();
   z = (double)atan2( -rolldir.x, rolldir.y ) * RADIANS;
   x *= RADIANS;
   y *= RADIANS;
}

inline RotateVector3::RotateVector3( const HashQuaternion &quat )  // SOB STYLE ROLL
{
   end = quat.GetZAxis();
   double y = (double)atan2( end.x, end.z );
   double x = (double)atan2( -end.y, (double)sqrt(end.x*end.x + end.z*end.z));
   HashVector3 rolldir = quat.GetYAxis();
   rolldir *= YXRotateRadiansQ( y, x ).Inverse();
   roll = (double)atan2( -rolldir.x, rolldir.y ) * RADIANS;
}

inline HashQuaternion::HashQuaternion( const RotateVector3 &rv )  // SOB STYLE ROLL
{
   double y = (double)atan2( rv.end.x, rv.end.z );
   double x = (double)atan2( -rv.end.y, (double)sqrt(rv.end.x*rv.end.x + rv.end.z*rv.end.z));
   if ( rv.roll )
      *this = YXZRotateRadiansQ( y, x, rv.roll / RADIANS );
   else
      *this = YXRotateRadiansQ( y, x );
}

inline RotateSphere::RotateSphere( const HashQuaternion &quat )
{
   *this = RotateSphere(RotateVector3(quat));
}

inline HashQuaternion::HashQuaternion( const RotateSphere &rs )
{
   *this = HashQuaternion(RotateVector3(rs));
}

inline HashQuaternion::HashQuaternion( const RotateEuler &re )
{
   if ( re.z )
      *this = YXZRotateQ( re.y, re.x, re.z );
   else
      *this = YXRotateQ( re.y, re.x );
}

inline HashQuaternion::HashQuaternion( const RotateEuler &re, BOOL isyxz )
{
   if (isyxz) {
      if ( re.z )
         *this = YXZRotateQ( re.y, re.x, re.z );
      else
         *this = YXRotateQ( re.y, re.x );
   }
   else {
      if ( re.z )
         *this = XYZRotateQ( re.x, re.y, re.z );
      else
         *this = XYRotateQ( re.x, re.y );
   }
}

// CONVERSIONS BETWEEN ROTATE TYPES

inline RotateSphere::RotateSphere( const HashVector3 &vec )
{
   longitude = (double)atan2( -vec.x, vec.y )*RADIANS;
   latitude = (double)atan2( (double)sqrt(vec.x*vec.x + vec.y*vec.y), vec.z )*RADIANS;
}

inline RotateSphere::RotateSphere( const RotateVector3 &rotate )
{
   longitude = (double)atan2( -rotate.end.x, rotate.end.y )*RADIANS;
   latitude = (double)atan2( (double)sqrt(rotate.end.x*rotate.end.x + rotate.end.y*rotate.end.y),
    rotate.end.z )*RADIANS;
   roll = rotate.roll; 
}

inline RotateVector3::RotateVector3( const RotateSphere &sphere )
{
    double r = (double)sin( sphere.latitude/RADIANS );
    end.z = (double)cos( sphere.latitude/RADIANS );
    end.y = r * (double)cos( sphere.longitude/RADIANS );
    end.x = -r * (double)sin( sphere.longitude/RADIANS );
    roll = sphere.roll;
}

inline RotateVector3::RotateVector3( const RotateEuler &euler )
{
   double cy = (double)cos( euler.y / RADIANS ),
         sy = (double)sin( euler.y / RADIANS ),
         cx = (double)cos( euler.x / RADIANS ),
         sx = (double)sin( euler.x / RADIANS );
   end.Set( sy*cx, -sx, cy*cx );  // SPEED UP: THESE ARE THE Z COLUMN OUT OF YXROTATE, WHICH IS THE SAME AS BUILDING THAT MATRIX, THEN TRANSFORMING (0,0,1) BY IT
   roll = euler.z;
 }

inline RotateEuler::RotateEuler( const RotateVector3 &rotatev )
{
   y = (double)atan2( rotatev.end.x, rotatev.end.z ) * RADIANS;
   x = (double)atan2( -rotatev.end.y, (double)sqrt(rotatev.end.x*rotatev.end.x +
    rotatev.end.z*rotatev.end.z)) * RADIANS;
   z = rotatev.roll;
}

// THE 4 FUNCTIONS BELOW WORK ONLY 0 TO 90 DEGREES

inline double HashVector3sToLinear( const HashVector3 &v, const HashVector3 &v1 )
{
   double cosa = Dot(v.Normalized(), v1.Normalized());
   if ( cosa == 0.0f )
      return 0.0f;
   double sina = (double)sqrt( 1.0f - cosa*cosa );
   double slope = sina / cosa;
   double percent = slope / (slope + 1.0f);
   return percent *= 90.0f;
}

inline double HashVector3sToAngle( const HashVector3 &v, const HashVector3 &v1 )
{
   return (double)acos(Dot(v.Normalized(), v1.Normalized())) * RADIANS;
}

inline double LinearToAngle( double linear )
{
   return (double)atan( linear / (90-linear)) * RADIANS;
}

inline double LinearToAngle360( double linear )
{
   if ( linear < 0 )
      return -LinearToAngle360( -linear );
   if ( linear <= 90 )
      return LinearToAngle( linear );
   double remainder = (double)fmod( linear, 90 );
   double excess = linear - remainder;
   return excess + LinearToAngle( remainder );
}

