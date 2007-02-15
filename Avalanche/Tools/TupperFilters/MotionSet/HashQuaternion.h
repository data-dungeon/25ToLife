// Dan  2/24/2004  \DanLP110\Include\Quaterni.h

#define HASH_QUATERNION_H

#ifndef HASH_VECTOR3_H
#include "HashVector3.h"
#endif

class RotateVector3;
class RotateSphere;
class RotateEuler;
//class Bone;

#define RADIANS			57.29578f

class HashQuaternion 
{
protected:
   HashVector3 v;
   double w;

public:
// CONSTRUCTORS
   HashQuaternion() {}
 //  HashQuaternion( const HashMatrix3x3 &matrix33 );
   HashQuaternion( const HashMatrix4x4 &matrix44 );
//   HashQuaternion( const Matrix34 & );
   HashQuaternion( const HashVector3 &vec, double scalar )
   {
      v = vec;
      w = scalar;
   }
   HashQuaternion( double x, double y, double z, double s )
   {                                                                  
      v.x = x;
      v.y = y;
      v.z = z;
      w = s;
   }
   HashQuaternion( const HashQuaternion &q )
   {
      v = q.v;
      w = q.w;
   }
   HashQuaternion( double scalar )
   {
      v = HashVector3( 0.0f );
      w = scalar;
   }
   HashQuaternion( double radangle, const HashVector3 &axis ) { SetAngleAxis( radangle, axis ); }
   HashQuaternion( const RotateVector3 &rv );  // SOB STYLE ROLL
   HashQuaternion( const RotateSphere &rs );
//   HashQuaternion( const RotateSphere &rs, Bone *bone );
   HashQuaternion( const RotateEuler  &re );
   HashQuaternion( const RotateEuler  &re, BOOL isyxz );
   
// STORAGE & RETRIEVAL
   const HashVector3 &GetV() const { return v; }
   double GetW() const { return w; }
   void SetV( const HashVector3 &vec ) { v = vec; } 
   void SetV( int index, double value ) { v[index] = value; } 
   void SetW( double scalar ) { w = scalar; }
   void Set( double x, double y, double z, double pw ) { v.Set(x,y,z); w=pw; }
   void Set( HashVector3 pv, double pw ) { v=pv; w=pw; }
   void Set( double fillvalue ) { v.Set(fillvalue,fillvalue,fillvalue); w=fillvalue; }
   void SetIdentity() { Set(0.0f,0.0f,0.0f,1.0f); }
   HashVector3 GetXAxis() const { return *this * HashVector3(1.0f,0.0f,0.0f); }
   HashVector3 GetYAxis() const { return *this * HashVector3(0.0f,1.0f,0.0f); }
   HashVector3 GetZAxis() const { return *this * HashVector3(0.0f,0.0f,1.0f); }
   double GetAngle() const  // this now always returns the smaller angle, ie 30 rather than 330, 175 rather than 185
   {
      double fw = (double)fabs(w);
      if (fw < 1)
         return (double)acos(fw)*2.0f;
      else
         return 0.0f;
   }
   HashVector3 GetAxis() const
   {
      if ( w == 1.0f )
         return v;
      if ( w > 0 ) 
         return v / (double)sin( acos(w));
      else
         return v / -(double)sin( acos(w));
   }
   void SetAngleAxis( double radangle, const HashVector3 &axis )
   {
      v = axis;
      v.Normalize();
      w = (double)cos( radangle / 2 );
      v *= (double)sin( radangle / 2 );
   }   

// UNARY OPERATIONS
   double Norm() const
   {
      return (double)sqrt(Dot(*this,*this));
   }
   HashQuaternion & Normalize()
   {
      *this /= Norm();
      return *this;
   }   
   HashQuaternion Normalized() const
   {
      return *this / Norm();
   }   
   double NormalizeLength( void )
   {
      double length = Norm();
      if (length)
         *this /= length;
      return length;
   }
   HashQuaternion Conjugate() const
   {
      return HashQuaternion( -v, w );
   }
   HashQuaternion Inverse() const
   {
      return Conjugate(); // / Norm();  ASSUME ALL OUR QUATS ARE NORMALIZED
   }
   HashQuaternion &Negate( void )
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
   int operator == ( const HashQuaternion &a ) const   { return (v == a.v) && (w == a.w); }
   int operator != ( const HashQuaternion &a ) const   { return !(*this == a); }   
   HashQuaternion &operator *= ( const HashQuaternion &q ) { *this = *this * q; return *this; }
   HashQuaternion &operator *= ( double scalar ) { v*=scalar; w*=scalar; return *this; }
   HashQuaternion &operator /= ( double scalar ) { v/=scalar; w/=scalar; return *this; }
   HashQuaternion &operator += ( const HashQuaternion &q2 ) { v+=q2.v; w+=q2.w; return *this; }
   HashQuaternion &operator -= ( const HashQuaternion &q2 ) { v-=q2.v; w-=q2.w; return *this; }
   HashQuaternion operator - ( void )          const   { return HashQuaternion(*this).Negate(); }
   double &operator[]( int i ) { return (&v.x)[i]; }
   double operator[]( int i ) const { return (&v.x)[i]; }
   
   HashQuaternion operator * ( const HashQuaternion &b ) const;
   HashQuaternion operator * ( double scalar ) const
   {
      return HashQuaternion( v * scalar, w * scalar );
   }
   friend HashQuaternion operator * ( double scalar, const HashQuaternion &q )
   {
      return HashQuaternion( q.v * scalar, q.w * scalar );
   }
   HashQuaternion operator / ( double scalar ) const
   {
      return HashQuaternion( v / scalar, w / scalar );
   }
   HashQuaternion operator + ( const HashQuaternion &b ) const
   {
      return HashQuaternion( v + b.v, w + b.w );
   }
   HashQuaternion operator - ( const HashQuaternion &b ) const
   {
      return HashQuaternion( v - b.v, w - b.w );
   }
   HashVector3 operator * ( const HashVector3 &v ) const
   {
      return (*this * HashQuaternion(v,0.0f) * Inverse()).GetV();
   }

// FRIENDS
   friend class HashMatrix3x3;
//   friend class Matrix34;
   friend class HashMatrix4x4;
   friend HashQuaternion IdentityQ();
   friend void           TransformHashVector3( const HashQuaternion &q, HashVector3 &v );
   friend double         Dot( const HashQuaternion &a, const HashQuaternion &b );
   friend HashQuaternion RotateVQ( double angle, const HashVector3 &axis );
   friend HashQuaternion RotateVRadiansQ( double anglerad, const HashVector3 &axis );
   friend HashQuaternion XRotateRadiansQ( double anglerad );
   friend HashQuaternion YRotateRadiansQ( double anglerad );
   friend HashQuaternion ZRotateRadiansQ( double anglerad );
   friend HashQuaternion XRotateQ( double angle );
   friend HashQuaternion YRotateQ( double angle );
   friend HashQuaternion ZRotateQ( double angle );
   friend HashQuaternion YXZRotateRadiansQ( double y, double x, double z );
   friend HashQuaternion YXRotateRadiansQ( double y, double x );
   friend HashQuaternion YXZRotateQ( double y, double x, double z );
   friend HashQuaternion YXRotateQ( double y, double x );
   friend HashQuaternion XYZRotateQ( double x, double y, double z );
   friend HashQuaternion XYRotateQ( double x, double y );
   friend HashQuaternion Rotate2VQ( const HashVector3 &from, const HashVector3 &to );
   friend HashQuaternion HashVector3sToStandardQ( const HashVector3 &becomezdir, const HashVector3 &becomeydir );
   friend HashQuaternion Blend( const HashQuaternion &quat1, double percent1, const HashQuaternion &quat2, double percent2 );

   friend class HashVector3;
};

// move friend function implementations outside of the class to compile under Vs .Net 2003 (Niels)
inline HashQuaternion IdentityQ() 
{
	return HashQuaternion( 0.0f,0.0f,0.0f,1.0f ); 
}

inline void TransformHashVector3( const HashQuaternion &q, HashVector3 &v ) 
{
	v *= q; 
}

inline double Dot( const HashQuaternion &a, const HashQuaternion &b )
{
	return Dot(a.v,b.v) + a.w*b.w; 
}

inline HashQuaternion RotateVQ( double angle, const HashVector3 &axis ) 
{
	return HashQuaternion( angle/RADIANS, axis ); 
}

inline HashQuaternion XRotateRadiansQ( double anglerad ) 
{
	return HashQuaternion( anglerad, HashVector3(1.0f,0.0f,0.0f)); 
}

inline HashQuaternion YRotateRadiansQ( double anglerad ) 
{
	return HashQuaternion( anglerad, HashVector3(0.0f,1.0f,0.0f)); 
}
inline HashQuaternion ZRotateRadiansQ( double anglerad ) 
{
	return HashQuaternion( anglerad, HashVector3(0.0f,0.0f,1.0f)); 
}

inline HashQuaternion XRotateQ( double angle )  
{
	return XRotateRadiansQ( angle / RADIANS ); 
}

inline HashQuaternion YRotateQ( double angle )  
{
	return YRotateRadiansQ( angle / RADIANS );
}

inline HashQuaternion ZRotateQ( double angle )  
{
	return ZRotateRadiansQ( angle / RADIANS ); 
}

inline HashQuaternion YXZRotateRadiansQ( double y, double x, double z ) 
{
	return YRotateRadiansQ(y) * XRotateRadiansQ(x) * ZRotateRadiansQ(z); 
}

inline HashQuaternion YXRotateRadiansQ( double y, double x )    
{
	return YRotateRadiansQ(y) * XRotateRadiansQ(x); 
}

inline HashQuaternion YXZRotateQ( double y, double x, double z )  
{
	return YXZRotateRadiansQ(y/RADIANS, x/RADIANS, z/RADIANS);
}

inline HashQuaternion YXRotateQ( double y, double x )      
{
	return YXRotateRadiansQ(y/RADIANS, x/RADIANS); 
}

inline HashQuaternion XYZRotateQ( double x, double y, double z )    
{
	return XRotateQ(x) * YRotateQ(y) * ZRotateQ(z);
}

inline HashQuaternion XYRotateQ( double x, double y )    
{ 
	return XRotateQ(x) * YRotateQ(y); 
}

inline HashQuaternion HashVector3sToStandardQ( const HashVector3 &becomezdir, const HashVector3 &becomeydir )
{
   HashQuaternion vectostandard = Rotate2VQ( becomezdir, HashVector3(0.0f, 0.0f, 1.0f));
   HashVector3   rollvec = vectostandard * becomeydir;
   double    roll = (double)atan2( -rollvec.x, rollvec.y );
   if ( roll )
      vectostandard = ZRotateRadiansQ( -roll ) * vectostandard;
   return vectostandard;
}

inline HashQuaternion HashQuaternion::operator * ( const HashQuaternion &b ) const
{
   return HashQuaternion( (v^b.v) + (w*b.v) + (v*b.w), w*b.w - (v|b.v) );
}

inline HashVector3 &HashVector3::operator *= ( const HashQuaternion &q )
{
/*
   HashQuaternion qtemp( *this, 0.0f );  // 32*, 24+
   qtemp = q * qtemp * q.Inverse();
   *this = qtemp.GetV();
*/
   HashQuaternion q2( (q.v^(*this)) + (q.w*(*this)), -q.v|(*this));  // FASTER 24*, 17+
   *this = (q2.v*q.w)-(q2.w*q.v)-(q2.v^q.v);
   return *this;
}

