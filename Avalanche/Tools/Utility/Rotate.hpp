// ANY ROTATION TYPE TO AND FROM QUATERNION

#include "Matrix44.h"

inline RotateEuler::RotateEuler( const Quaternion &quat )
{
   Vector end( quat.GetZAxis());
   y = (float)(atan2( end.x, end.z ) * RADIANS);
   x = (float)(atan2( -end.y, sqrt(end.x*end.x + end.z*end.z)) * RADIANS);
   Vector rolldir( quat.GetYAxis());
   rolldir *= YXRotateQ( y, x ).Inverse();
   z = (float)(atan2( -rolldir.x, rolldir.y ) * RADIANS);
}

inline RotateEuler::RotateEuler( const Matrix44 &matrix )
{
   Vector end = matrix.GetZAxis();
   y = (float)atan2( end.x, end.z );
   x = (float)atan2( -end.y, (float)sqrt(end.x*end.x + end.z*end.z));
   Vector rolldir = matrix.GetYAxis();
   rolldir *= YXRotateRadians( y, x ).Transpose();
   z = (float)atan2( -rolldir.x, rolldir.y ) * RADIANS;
   x *= RADIANS;
   y *= RADIANS;
}

inline RotateVector::RotateVector( const Quaternion &quat )  // SOB STYLE ROLL
{
   end = quat.GetZAxis();
   float y = (float)atan2( end.x, end.z );
   float x = (float)atan2( -end.y, (float)sqrt(end.x*end.x + end.z*end.z));
   Vector rolldir = quat.GetYAxis();
   rolldir *= YXRotateRadiansQ( y, x ).Inverse();
   roll = (float)atan2( -rolldir.x, rolldir.y ) * RADIANS;
}

inline Quaternion::Quaternion( const RotateVector &rv )  // SOB STYLE ROLL
{
   float y = (float)atan2( rv.end.x, rv.end.z );
   float x = (float)atan2( -rv.end.y, (float)sqrt(rv.end.x*rv.end.x + rv.end.z*rv.end.z));
   if ( rv.roll )
      *this = YXZRotateRadiansQ( y, x, rv.roll / RADIANS );
   else
      *this = YXRotateRadiansQ( y, x );
}

inline RotateSphere::RotateSphere( const Quaternion &quat )
{
   *this = RotateSphere(RotateVector(quat));
}

inline Quaternion::Quaternion( const RotateSphere &rs )
{
   *this = Quaternion(RotateVector(rs));
}

inline Quaternion::Quaternion( const RotateEuler &re )
{
   if ( re.z )
      *this = YXZRotateQ( re.y, re.x, re.z );
   else
      *this = YXRotateQ( re.y, re.x );
}

inline Quaternion::Quaternion( const RotateEuler &re, BOOL isyxz )
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

inline RotateSphere::RotateSphere( const Vector &vec )
{
   longitude = (float)atan2( -vec.x, vec.y )*RADIANS;
   latitude = (float)atan2( (float)sqrt(vec.x*vec.x + vec.y*vec.y), vec.z )*RADIANS;
}

inline RotateSphere::RotateSphere( const RotateVector &rotate )
{
   longitude = (float)atan2( -rotate.end.x, rotate.end.y )*RADIANS;
   latitude = (float)atan2( (float)sqrt(rotate.end.x*rotate.end.x + rotate.end.y*rotate.end.y),
    rotate.end.z )*RADIANS;
   roll = rotate.roll; 
}

inline RotateVector::RotateVector( const RotateSphere &sphere )
{
    float r = (float)sin( sphere.latitude/RADIANS );
    end.z = (float)cos( sphere.latitude/RADIANS );
    end.y = r * (float)cos( sphere.longitude/RADIANS );
    end.x = -r * (float)sin( sphere.longitude/RADIANS );
    roll = sphere.roll;
}

inline RotateVector::RotateVector( const RotateEuler &euler )
{
   float cy = (float)cos( euler.y / RADIANS ),
         sy = (float)sin( euler.y / RADIANS ),
         cx = (float)cos( euler.x / RADIANS ),
         sx = (float)sin( euler.x / RADIANS );
   end.Set( sy*cx, -sx, cy*cx );  // SPEED UP: THESE ARE THE Z COLUMN OUT OF YXROTATE, WHICH IS THE SAME AS BUILDING THAT MATRIX, THEN TRANSFORMING (0,0,1) BY IT
   roll = euler.z;
 }

inline RotateEuler::RotateEuler( const RotateVector &rotatev )
{
   y = (float)atan2( rotatev.end.x, rotatev.end.z ) * RADIANS;
   x = (float)atan2( -rotatev.end.y, (float)sqrt(rotatev.end.x*rotatev.end.x +
    rotatev.end.z*rotatev.end.z)) * RADIANS;
   z = rotatev.roll;
}

// THE 4 FUNCTIONS BELOW WORK ONLY 0 TO 90 DEGREES

inline float VectorsToLinear( const Vector &v, const Vector &v1 )
{
   float cosa = Dot(v.Normalized(), v1.Normalized());
   if ( cosa == 0.0f )
      return 0.0f;
   float sina = (float)sqrt( 1.0f - cosa*cosa );
   float slope = sina / cosa;
   float percent = slope / (slope + 1.0f);
   return percent *= 90.0f;
}

inline float VectorsToAngle( const Vector &v, const Vector &v1 )
{
   return (float)acos(Dot(v.Normalized(), v1.Normalized())) * RADIANS;
}

inline float LinearToAngle( float linear )
{
   return (float)atan( linear / (90-linear)) * RADIANS;
}

inline float LinearToAngle360( float linear )
{
   if ( linear < 0 )
      return -LinearToAngle360( -linear );
   if ( linear <= 90 )
      return LinearToAngle( linear );
   float remainder = (float)fmod( linear, 90 );
   float excess = linear - remainder;
   return excess + LinearToAngle( remainder );
}

