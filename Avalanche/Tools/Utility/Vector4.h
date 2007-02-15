// RC  4/24/2002  \HRandy95\Include\Vector4.h
#pragma once

#define __VECTOR4_H

#ifndef __VECTOR_H
#include "Vector.h"
#endif

#ifdef _SIMD
#include "smlmath.h"
#endif

class Matrix44;

class Vector4 
{
public:
   union {
#ifdef _SIMD
      struct { SMLVec4f m_smlvec4f; };
#endif
      struct { float x, y, z, w; };
   };

   Vector4( void ) {}
   Vector4( float x, float y, float z, float w );
   Vector4( float fill );
   Vector4( const Vector4 &v );
   Vector4( const Vector &other3 );
   
   friend class Matrix44;

   float operator[]( int i ) const;
   float &operator[]( int i );
   void Set( float x, float y, float z, float w );
   void Set( float fill )
   {
      x = y = z = w = fill;
   }
   Vector4 & operator = ( const Vector4 &v );
   Vector4 & operator /= ( float scalar );
   Vector4 & operator -= ( const Vector4 &other );
   Vector4 & operator += ( const Vector4 &other );
   Vector4 & operator *= ( float scale );
   int operator == ( const Vector4 &a ) const;
   int operator != ( const Vector4 &a ) const { return !(*this == a); }   

   Vector4 operator + ( const Vector4 &a ) const { return Vector4(*this) += a; }
   Vector4 operator - ( const Vector4 &a ) const { return Vector4(*this) -= a; }

   float operator | ( const Vector4 &b ) const;   // Dot Product
   float operator | ( const Vector &b ) const;   // Dot Product
   Vector4 operator * ( const Vector4 &b ) const // Member by member multiply
   {
      Vector4 result;               // GOOD CANDIDATE FOR SIMD, NOT IN SML
      result.x = x * b.x;
      result.y = y * b.y;
      result.z = z * b.z;
      result.w = w * b.w;
      return result;
   }
   Vector4 operator / ( const Vector4 &b ) const // Member by member divide
   {
      Vector4 result;               // GOOD CANDIDATE FOR SIMD, NOT IN SML
      result.x = x / b.x;
      result.y = y / b.y;
      result.z = z * b.z;
      result.w = w / b.w;
      return result;
   }
   Vector4 operator * ( float scalar ) const 
   {
      return Vector4(*this) *= scalar;
   }
   friend Vector4 operator * ( float scalar, const Vector4 &v )
   {
      return v * scalar;
   }
   Vector4 & operator *= ( const Matrix44 &m );
   friend void Swap( Vector4 &a, Vector4 &b )
   {
      Vector4 temp( a );
      a = b;
      b = temp;
   }
   float Length() { return (float)sqrt((x * x) + (y * y) + (z * z) + (w * w)); }
   Vector4 Normalize()
   {
      float length = Length();

      if (length)
         *this /= length;

      return *this;
   }
};

#ifdef _SIMD

inline Vector4::Vector4( float x, float y, float z, float w ) : m_smlvec4f( x, y, z, w )
{
}

inline Vector4::Vector4( float f ) : m_smlvec4f( f, f, f, f )
{
}

inline Vector4::Vector4( const Vector4 &v ) : m_smlvec4f( v.m_smlvec4f )
{
}

inline Vector4::Vector4( const Vector &other3 ) : m_smlvec4f( other3[0],other3[1],other3[2],1.0f )
{
}

inline float Vector4::operator[]( int i ) const
{
   return m_smlvec4f.data()[i];
}

inline float &Vector4::operator[]( int i )
{
   return m_smlvec4f.data()[i];
}

inline void Vector4::Set( float x, float y, float z, float w )
{
   m_smlvec4f.Set( x, y, z, w );
}

inline Vector4 & Vector4::operator = ( const Vector4 &v )
{
   m_smlvec4f = v.m_smlvec4f; return *this;
}

inline Vector4 & Vector4::operator /= ( float scalar )
{
   m_smlvec4f /= scalar; return *this;
}

inline Vector4 & Vector4::operator -= ( const Vector4 &other )
{
   m_smlvec4f -= other.m_smlvec4f; return *this;
}

inline Vector4 & Vector4::operator += ( const Vector4 &other )
{
   m_smlvec4f += other.m_smlvec4f;
   return *this;
}

inline Vector4 & Vector4::operator *= ( float scale )
{
   m_smlvec4f *= scale;
   return *this;
}

inline int Vector4::operator == ( const Vector4 &a ) const
{
   return m_smlvec4f == a.m_smlvec4f;
}

inline float Vector4::operator | ( const Vector4 &b ) const   // Dot Product
{
   return m_smlvec4f.Dot( b.m_smlvec4f );
}

#else // NOT SIMD

inline Vector4::Vector4( float x, float y, float z, float w )
{
   Set(x,y,z,w);
}

inline Vector4::Vector4( float fill )
{
   Set(fill);
}

inline Vector4::Vector4( const Vector4 &other )
{
   *this = other;
}

inline Vector4::Vector4( const Vector &other )
{
   x = other.x;
   y = other.y;
   z = other.z;
   w = 1.0f;
}

inline float Vector4::operator[]( int i ) const
{
   return (&x)[i];
}

inline float &Vector4::operator[]( int i )
{
   return (&x)[i];
}

inline void Vector4::Set( float xi, float yi, float zi, float wi )
{
   x = xi;
   y = yi;
   z = zi;
   w = wi;
}

inline Vector4 & Vector4::operator = ( const Vector4 &other )
{
   x = other.x;
   y = other.y;
   z = other.z;
   w = other.w;
   return *this;
}

inline Vector4 & Vector4::operator /= ( float scalar )
{
   x /= scalar;
   y /= scalar;
   z /= scalar;
   w /= scalar;
   return *this;
}

inline Vector4 & Vector4::operator -= ( const Vector4 &other )
{
   x -= other.x;
   y -= other.y;
   z -= other.z;
   w -= other.w;
   return *this;
}

inline Vector4 & Vector4::operator += ( const Vector4 &other )
{
   x += other.x;
   y += other.y;
   z += other.z;
   w += other.w;
   return *this;
}

inline Vector4 & Vector4::operator *= ( float scale )
{
   x *= scale;
   y *= scale;
   z *= scale;
   w *= scale;
   return *this;
}

inline int Vector4::operator == ( const Vector4 &a ) const
{
   return x==a.x && y==a.y && z==a.z && w==a.w;
}

inline float Vector4::operator | ( const Vector4 &b ) const   // Dot Product
{
   return x*b.x + y*b.y + z*b.z + w*b.w;
}

inline float Vector4::operator | ( const Vector &b ) const   // Dot Product
{
   return x*b.x + y*b.y + z*b.z + w;
}

#endif // NOT SIMD
