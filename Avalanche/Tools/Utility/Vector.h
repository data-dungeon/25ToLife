#pragma once

#include <math.h>
#include "AMacros.h"

class Matrix44;
class Vector2;
class Quaternion;

class Vector 
{
public:
   float x, y, z;
   
// Constructors
   Vector() {}//; { x = 0; y = 0; z = 0; }
   Vector( float xi, float yi, float zi );
   Vector( float f );
   Vector( const Vector &vOther );
   Vector( const Vector2 &v2 );
   
// Operators
   float operator[]( int i ) const;
   float &operator[]( int i );

   Vector & operator += ( const Vector &vOther );
   Vector & operator -= ( const Vector &vOther );
   Vector & operator /= ( const Vector &vOther );
   Vector & operator *= ( const Vector &vOther );
   Vector & operator ^= ( const Vector &a ) { return (*this = Cross(a)); }

   Vector & operator *= ( float fScale );
   Vector & operator /= ( float fScale );

   Vector & operator *= ( const Matrix44 &m );
   Vector & operator *= ( const Quaternion &q );

   int operator == ( const Vector &a ) const;
   int operator != ( const Vector &a ) const { return !(*this == a); }   

   Vector operator + ( const Vector &a )   const { return Vector(*this) += a; }
   Vector operator - ( const Vector &a )   const { return Vector(*this) -= a; }
   Vector operator - ( void )              const { return Vector(*this).Negate(); }
   Vector operator * ( const Vector &a )   const { return Vector(*this) *= a; }
   Vector operator * ( float s )           const { return Vector(*this) *= s; }
   Vector operator / ( float s )           const { return Vector(*this) /= s; }
   Vector operator / ( const Vector &a )   const { return Vector(*this) /= a; }

// operations
   void Set( float xi, float yi, float zi );
   void Set( float f );
   void Swap( Vector &a ) { Vector temp(*this); *this = a; a = temp; }
   Vector &Negate();

   float Length() const;
   float LengthSq() const;
   Vector Normalized() const { return Vector(*this).Normalize(); }
   float NormalizeLength();
   Vector &Normalize();
   float Dot( const Vector &a ) const;
   float Square() const { return Dot(*this); }
   Vector Absolute() const { return Vector( (float)fabs( x ), (float)fabs( y ), (float)fabs( z )); }
   Vector Reciprocal()  const { return Vector( 1.0f/x, 1.0f/y, 1.0f/z ); }
   void CrossSelf( const Vector &a ) { *this = Cross( a ); }
   Vector Cross( const Vector &v ) const;
   Vector Reflect( const Vector &n ) const;
   Vector Hundredized() const { return Vector(*this).Hundredize(); } 
   Vector &Hundredize();

   float operator | ( const Vector &a ) const { return Dot(a); } // same as Dot
   Vector operator ^ ( const Vector &a ) const { return Cross(a); } // same as Cross

   void Min(float fMin) { if (x<fMin) x=fMin; if (y<fMin) y=fMin; if (z<fMin) z=fMin; }
   void Max(float fMax) { if (x>fMax) x=fMax; if (y>fMax) y=fMax; if (z>fMax) z=fMax; }

   BOOL IsParallel( const Vector &vOther ) const;
   BOOL IsParallel( const Vector &vOther, float fTol ) const;
   BOOL IsSameDirection( const Vector &vOther ) const;
   BOOL IsParallelAndSameDirection( const Vector &vOther, float fTol ) const;
   BOOL IsParallelAndOppositeDirection( const Vector &vOther, float fTol ) const;
   float DistanceTo( const Vector &vOther ) const;
   BOOL IsDistanceWithinTolerance( const Vector &vOther, float fTol ) const;
   float GetAngle( const Vector &vOther ) const;

//Friend Functions
   friend Vector operator * ( const float s, const Vector &a ) { return a * s; }
   friend float Dot( const Vector &a, const Vector &b ) { return a.Dot(b); }
   friend Vector Cross( const Vector &a, const Vector &b ) { return a.Cross(b); }
   friend void Swap( Vector &a, Vector &b ) { a.Swap(b); }
   friend BOOL IsBackward( const Vector &a, const Vector &b, const Vector &c ) { return (b.x-a.x) * (c.y-a.y) - (b.y-a.y) * (c.x-a.x) < 0; }
   
};

// Constructors
inline Vector::Vector( float xi, float yi, float zi )
{
   Set( xi, yi, zi);
}

inline Vector::Vector( const Vector &vOther )
{
   *this = vOther;
}

inline Vector::Vector( float fill )
{
   Set(fill);
}

// Operators
inline float Vector::operator[]( int i ) const
{
   return (&x)[i];
}

inline float &Vector::operator[]( int i )
{
   return (&x)[i];
}

inline Vector &Vector::operator += ( const Vector &vOther )
{
   x += vOther.x;
   y += vOther.y;
   z += vOther.z;
   return *this; 
}

inline Vector &Vector::operator -= ( const Vector &vOther )
{
   x -= vOther.x;
   y -= vOther.y;
   z -= vOther.z;
   return *this; 
}

inline Vector &Vector::operator *= ( float fScale )
{
   x *= fScale;
   y *= fScale;
   z *= fScale;

   return *this;
}

inline Vector &Vector::operator *= ( const Vector &vOther )
{
   x *= vOther.x;
   y *= vOther.y;
   z *= vOther.z;

   return *this; 
}

inline Vector &Vector::operator /= ( float fScale )
{
   x /= fScale;
   y /= fScale;
   z /= fScale;

   return *this;
}

inline Vector &Vector::operator /= ( const Vector &vOther )
{
   x /= vOther.x;
   y /= vOther.y;
   z /= vOther.z;

   return *this;
}

inline int Vector::operator == ( const Vector &a ) const
{
   return (x == a.x) && (y == a.y) && (z == a.z);
}

// Operations
inline float Vector::NormalizeLength()
{
   float length = Length();

   if (length)
      operator /= (length);

   return length;
}

inline Vector & Vector::Negate()
{
   x = -x;
   y = -y;
   z = -z;
   return *this;
}

inline void Vector::Set( float xi, float yi, float zi )
{
   x = xi;
   y = yi;
   z = zi;
}

inline void Vector::Set( float fValue )
{
   x = y = z = fValue;
}

inline float Vector::Length() const
{
   return (float)sqrt(Square());
}

inline float Vector::LengthSq() const
{
   return (float)Square();
}

inline Vector & Vector::Normalize( void )
{
   float length = Length();

   if (length)
      *this /= length;

   return *this;
}

inline float Vector::Dot( const Vector &a ) const
{
   return x * a.x + y * a.y + z * a.z;
}

inline Vector Vector::Cross( const Vector &v ) const
{
   return Vector(y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x);
}

inline Vector Vector::Reflect( const Vector &n ) const
{
   return *this - (2.0f * n * n.Dot( *this ));
}

inline float Vector::DistanceTo(const Vector &vOther) const
{
   return Vector( vOther.x-x, vOther.y-y, vOther.z-z ).Length();
}

inline BOOL Vector::IsDistanceWithinTolerance( const Vector &vOther, const float fTol) const
{
   Vector vDistance( vOther.x-x, vOther.y-y, vOther.z-z);

   if (vDistance.LengthSq() < (fTol * fTol))
      return TRUE;

   return FALSE;
}

inline Vector &Vector::Hundredize()
{
   float length = Length();

   if (length)
      *this *= (100/length);

   return *this;
}

inline float Vector::GetAngle( const Vector &vOther ) const
{
   return acos( Normalized().Dot( vOther.Normalized() ));
}

inline BOOL Vector::IsParallel( const Vector &vOther ) const
{
   float fDot = Normalized().Dot( vOther.Normalized() );

   if (fabs(fDot) == 1.0)
      return TRUE;

   return FALSE;
}

inline BOOL Vector::IsParallel( const Vector &vOther, float fTol) const
{
   float fDot = Normalized().Dot( vOther.Normalized() );
   float fAbsDot = fabs( fDot );

   if ( fAbsDot >= (1.0f - fTol) && fAbsDot <= (1.0f + fTol) )
      return TRUE;

   return FALSE;
}

inline BOOL Vector::IsSameDirection( const Vector &vOther ) const
{
   float fDot = Normalized().Dot( vOther.Normalized() );

   if (fDot > 0.0f)
      return TRUE;

   return FALSE;
}

inline BOOL Vector::IsParallelAndSameDirection( const Vector &vOther, float fTol ) const
{
   if (IsParallel( vOther, fTol ))
      if (IsSameDirection( vOther ))
         return TRUE;

   return FALSE;
}

inline BOOL Vector::IsParallelAndOppositeDirection( const Vector &vOther, float fTol ) const
{
   if (IsParallel(vOther, fTol))
      if (!IsSameDirection( vOther ))
         return TRUE;

   return FALSE;
}

// Misc
inline float DistancePointToPlane( const Vector &vPoint, const Vector &vPointOnPlane, const Vector &vNormal )
{
   Vector vDelta = vPoint;
   vDelta -= vPointOnPlane;

   return Dot( vDelta, vNormal );
}

inline void PushMinMax( const Vector &p, Vector &minv, Vector &maxv )
{
   if ( p.x > maxv.x )
      maxv.x = p.x;

   if ( p.x < minv.x )
      minv.x = p.x;

   if ( p.y > maxv.y )
      maxv.y = p.y;

   if ( p.y < minv.y )
      minv.y = p.y;

   if ( p.z > maxv.z )
      maxv.z = p.z;

   if ( p.z < minv.z )
      minv.z = p.z;
}

#ifdef __AFXTEMPL_H__
template<> AFX_INLINE UINT AFXAPI HashKey<const Vector &>( const Vector &key )
{
	double x = (int)(key.x * 1000.0f + SIGN(key.x) * .5f)/1000.0*12.3;
	double y = (int)(key.y * 1000.0f + SIGN(key.y) * .5f)/1000.0*23.4;
	double z = (int)(key.z * 1000.0f + SIGN(key.z) * .5f)/1000.0*34.5;
	return (int) (pow(x*x + y*y + z*z, 0.5 ) * 9.87);  
}

inline Vector fabs(const Vector &other)
{
   return Vector(fabs(other.x),fabs(other.y),fabs(other.z));
}

#endif