// Dan  2/24/2004  \DanLP110\Include\Quaterni.h

#pragma once

#include "Vector.h"

class RotateVector;
class RotateSphere;
class RotateEuler;
class Bone;

#define MATRIXEXPORT

class MATRIXEXPORT Quaternion 
{
protected:
   Vector v;
   float w;

public:
// CONSTRUCTORS
   Quaternion() {}
   Quaternion( const Vector &vec, float scalar )
   {
      v = vec;
      w = scalar;
   }
   Quaternion( float x, float y, float z, float s )
   {                                                                  
      v.x = x;
      v.y = y;
      v.z = z;
      w = s;
   }
   Quaternion( const Quaternion &q )
   {
      v = q.v;
      w = q.w;
   }
   Quaternion( float scalar )
   {
      v = Vector( 0.0f );
      w = scalar;
   }
   Quaternion( float radangle, const Vector &axis ) { SetAngleAxis( radangle, axis ); }
   Quaternion( const RotateVector &rv );  // SOB STYLE ROLL
   Quaternion( const RotateSphere &rs );
   Quaternion( const RotateSphere &rs, Bone *bone );
   Quaternion( const RotateEuler  &re );
   Quaternion( const RotateEuler  &re, BOOL isyxz );
   
// STORAGE & RETRIEVAL
   const Vector &GetV() const { return v; }
   float GetW() const { return w; }
   void SetV( const Vector &vec ) { v = vec; } 
   void SetV( int index, float value ) { v[index] = value; } 
   void SetW( float scalar ) { w = scalar; }
   void Set( float x, float y, float z, float pw ) { v.Set(x,y,z); w=pw; }
   void Set( Vector pv, float pw ) { v=pv; w=pw; }
   void Set( float fillvalue ) { v.Set(fillvalue); w=fillvalue; }
   void SetIdentity() { Set(0.0f,0.0f,0.0f,1.0f); }
   Vector GetXAxis() const { return *this * Vector(1.0f,0.0f,0.0f); }
   Vector GetYAxis() const { return *this * Vector(0.0f,1.0f,0.0f); }
   Vector GetZAxis() const { return *this * Vector(0.0f,0.0f,1.0f); }
   float GetAngle() const  // this now always returns the smaller angle, ie 30 rather than 330, 175 rather than 185
   {
      float fw = (float)fabs(w);
      if (fw < 1)
         return (float)acos(fw)*2.0f;
      else
         return 0.0f;
   }
   Vector GetAxis() const
   {
      if ( w == 1.0f )
         return v;
      if ( w > 0 ) 
         return v / (float)sin( acos(w));
      else
         return v / -(float)sin( acos(w));
   }
   void SetAngleAxis( float radangle, const Vector &axis )
   {
      v = axis;
      v.Normalize();
      w = (float)cos( radangle / 2 );
      v *= (float)sin( radangle / 2 );
   }   

// UNARY OPERATIONS
   float Norm() const
   {
      return (float)sqrt(Dot(*this,*this));
   }
   Quaternion & Normalize()
   {
      *this /= Norm();
      return *this;
   }   
   Quaternion Normalized() const
   {
      return *this / Norm();
   }   
   float NormalizeLength( void )
   {
      float length = Norm();
      if (length)
         *this /= length;
      return length;
   }
   Quaternion Conjugate() const
   {
      return Quaternion( -v, w );
   }
   Quaternion Inverse() const
   {
      return Conjugate(); // / Norm();  ASSUME ALL OUR QUATS ARE NORMALIZED
   }
   Quaternion &Negate( void )
   {
      v.Negate();
      w = -w;
      return *this;
   }
   BOOL IsIdentity()
   {
      return *this == IdentityQ();
   }

// OPERATORS
   int operator == ( const Quaternion &a ) const   { return (v == a.v) && (w == a.w); }
   int operator != ( const Quaternion &a ) const   { return !(*this == a); }   
   Quaternion &operator *= ( const Quaternion &q ) { *this = *this * q; return *this; }
   Quaternion &operator *= ( float scalar ) { v*=scalar; w*=scalar; return *this; }
   Quaternion &operator /= ( float scalar ) { v/=scalar; w/=scalar; return *this; }
   Quaternion &operator += ( const Quaternion &q2 ) { v+=q2.v; w+=q2.w; return *this; }
   Quaternion &operator -= ( const Quaternion &q2 ) { v-=q2.v; w-=q2.w; return *this; }
   Quaternion operator - ( void )          const   { return Quaternion(*this).Negate(); }
   float &operator[]( int i ) { return (&v.x)[i]; }
   float operator[]( int i ) const { return (&v.x)[i]; }
   
   Quaternion operator * ( const Quaternion &b ) const;
   Quaternion operator * ( float scalar ) const
   {
      return Quaternion( v * scalar, w * scalar );
   }
   friend Quaternion operator * ( float scalar, const Quaternion &q )
   {
      return Quaternion( q.v * scalar, q.w * scalar );
   }
   Quaternion operator / ( float scalar ) const
   {
      return Quaternion( v / scalar, w / scalar );
   }
   Quaternion operator + ( const Quaternion &b ) const
   {
      return Quaternion( v + b.v, w + b.w );
   }
   Quaternion operator - ( const Quaternion &b ) const
   {
      return Quaternion( v - b.v, w - b.w );
   }
   Vector operator * ( const Vector &v ) const
   {
      return (*this * Quaternion(v,0.0f) * Inverse()).GetV();
   }

// FRIENDS
   friend class Matrix44;
   friend Quaternion IdentityQ();
   friend void       TransformVector( const Quaternion &q, Vector &v );
   friend float      Dot( const Quaternion &a, const Quaternion &b );
   friend Quaternion RotateVQ( float angle, const Vector &axis );
   friend MATRIXEXPORT Quaternion RotateVRadiansQ( float anglerad, const Vector &axis );
   friend Quaternion XRotateRadiansQ( float anglerad );
   friend Quaternion YRotateRadiansQ( float anglerad );
   friend Quaternion ZRotateRadiansQ( float anglerad );
   friend Quaternion XRotateQ( float angle );
   friend Quaternion YRotateQ( float angle );
   friend Quaternion ZRotateQ( float angle );
   friend Quaternion YXZRotateRadiansQ( float y, float x, float z );
   friend Quaternion YXRotateRadiansQ( float y, float x );
   friend Quaternion YXZRotateQ( float y, float x, float z );
   friend Quaternion YXRotateQ( float y, float x );
   friend Quaternion XYZRotateQ( float x, float y, float z );
   friend Quaternion XYRotateQ( float x, float y );
   friend MATRIXEXPORT Quaternion Rotate2VQ( const Vector &from, const Vector &to );
   friend Quaternion VectorsToStandardQ( const Vector &becomezdir, const Vector &becomeydir );
   friend MATRIXEXPORT Quaternion Blend( const Quaternion &quat1, float percent1, const Quaternion &quat2, float percent2 );

   friend class Vector;
};

// move friend function implementations outside of the class to compile under Vs .Net 2003 (Niels)
inline Quaternion IdentityQ() 
{
	return Quaternion( 0.0f,0.0f,0.0f,1.0f ); 
}
inline void TransformVector( const Quaternion &q, Vector &v ) 
{
	v *= q; 
}
inline float Dot( const Quaternion &a, const Quaternion &b )
{
	return Dot(a.v,b.v) + a.w*b.w; 
}
inline Quaternion RotateVQ( float angle, const Vector &axis ) 
{
	return Quaternion( angle/RADIANS, axis ); 
}
inline Quaternion XRotateRadiansQ( float anglerad ) 
{
	return Quaternion( anglerad, Vector(1.0f,0.0f,0.0f)); 
}
inline Quaternion YRotateRadiansQ( float anglerad ) 
{
	return Quaternion( anglerad, Vector(0.0f,1.0f,0.0f)); 
}
inline Quaternion ZRotateRadiansQ( float anglerad ) 
{
	return Quaternion( anglerad, Vector(0.0f,0.0f,1.0f)); 
}
inline Quaternion XRotateQ( float angle )  
{
	return XRotateRadiansQ( angle / RADIANS ); 
}
inline Quaternion YRotateQ( float angle )  
{
	return YRotateRadiansQ( angle / RADIANS );
}
inline Quaternion ZRotateQ( float angle )  
{
	return ZRotateRadiansQ( angle / RADIANS ); 
}
inline Quaternion YXZRotateRadiansQ( float y, float x, float z ) 
{
	return YRotateRadiansQ(y) * XRotateRadiansQ(x) * ZRotateRadiansQ(z); 
}
inline Quaternion YXRotateRadiansQ( float y, float x )    
{
	return YRotateRadiansQ(y) * XRotateRadiansQ(x); 
}
inline Quaternion YXZRotateQ( float y, float x, float z )  
{
	return YXZRotateRadiansQ(y/RADIANS, x/RADIANS, z/RADIANS);
}
inline Quaternion YXRotateQ( float y, float x )      
{
	return YXRotateRadiansQ(y/RADIANS, x/RADIANS); 
}
inline Quaternion XYZRotateQ( float x, float y, float z )    
{
	return XRotateQ(x) * YRotateQ(y) * ZRotateQ(z);
}
inline Quaternion XYRotateQ( float x, float y )    
{ 
	return XRotateQ(x) * YRotateQ(y); 
}

inline Quaternion Rotate2VQ( const Vector &from, const Vector &to )
{
   // build quaternion to rotate to vector
   Vector A(from);
   Vector B(to);
   A.Normalize();
   B.Normalize();
   Vector V = A ^ B;
   double angle = (acos(Dot(A,B)))/2.0;
   V.Normalize();
   Quaternion Q(V * (float)sin(angle), (float)cos(angle));
   return Q;
}

inline Quaternion VectorsToStandardQ( const Vector &becomezdir, const Vector &becomeydir )
{
   Quaternion vectostandard = Rotate2VQ( becomezdir, Vector(0.0f, 0.0f, 1.0f));
   Vector   rollvec = vectostandard * becomeydir;
   float    roll = (float)atan2( -rollvec.x, rollvec.y );
   if ( roll )
      vectostandard = ZRotateRadiansQ( -roll ) * vectostandard;
   return vectostandard;
}

inline Quaternion Quaternion::operator * ( const Quaternion &b ) const
{
   return Quaternion( (v^b.v) + (w*b.v) + (v*b.w), w*b.w - (v|b.v) );
}

inline Vector &Vector::operator *= ( const Quaternion &q )
{
/*
   Quaternion qtemp( *this, 0.0f );  // 32*, 24+
   qtemp = q * qtemp * q.Inverse();
   *this = qtemp.GetV();
*/
   Quaternion q2( (q.v^(*this)) + (q.w*(*this)), -q.v|(*this));  // FASTER 24*, 17+
   *this = (q2.v*q.w)-(q2.w*q.v)-(q2.v^q.v);
   return *this;
}
