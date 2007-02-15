#pragma once

#include <math.h>
#include "Vector.h"

class Matrix44;

class Vector2 {
public:
   float x, y;
   
   Vector2() { x = 0; y = 0; }
   Vector2( float xinit, float yinit )
   {
      x = xinit;
      y = yinit;
   }
   Vector2( const Vector2 &other )
   {
      x = other.x;
      y = other.y;
   }
   Vector2( float scalar )
   {
      x = y = scalar;
   }

   float operator[]( int i ) const
   {
      return (&x)[i];
   }
   float &operator[]( int i )
   {
      return (&x)[i];
   }

   Vector2 &operator=( const Vector2 &other )
   {
      x = other.x;
      y = other.y;
      return *this;
   }
   Vector2 &operator=( float scalar )
   {
      x = y = scalar;
      return *this;
   }

   void operator += ( const Vector2 & );
   void operator -= ( const Vector2 & );
   void operator *= ( const Vector2 & );
   void operator *= ( const float );
   void operator /= ( const Vector2 & );
   void operator /= ( const float );
   Vector2 & operator *= ( const Matrix44 &m );

   float Length() const
   {
      return (float)sqrt( x*x + y*y );
   } 
   float LengthSq( void ) const
   {
      return (float)(x*x + y*y);
   }
   Vector2 &Negate();
   void Set( float px, float py );
   void Set( float );
   Vector2 Normalized() const;
   Vector2 Absolute() const;
   Vector2 Reciprocal() const;
   Vector2 Orthogonal() const;

   float Normalize()
   {
      float length;
      
      if (length = Length())
         (*this) /= length;
      return length;
   }
   float Dot( const Vector2 & );

   friend Vector2 operator - ( const Vector2 & );
   friend Vector2 operator + ( const Vector2 &, const Vector2 & );
   friend Vector2 operator - ( const Vector2 &, const Vector2 & );
   friend Vector2 operator * ( const Vector2 &, const float );
   friend Vector2 operator * ( const float, const Vector2 & );
   friend Vector2 operator / ( const Vector2 &, const float );

   friend Vector2 operator * ( const Vector2 &, const Vector2 & ); // MEMBERWISE MULTIPLY
   friend float operator | ( const Vector2 &, const Vector2 & );  // DOT PRODUCT
   friend float operator ^ ( const Vector2 &, const Vector2 & );  // DOT PRODUCT

   friend int operator == ( const Vector2 &, const Vector2 & );
   friend int operator != ( const Vector2 &, const Vector2 & );
   friend void Swap( Vector2 &, Vector2 & );

   friend float Dot( const Vector2 &a, const Vector2 &b ) { return a.x * b.x + a.y * b.y; }

   friend Vector2 ToVector2( const Vector &p )
   {
      return Vector2( p.x, p.y );
   }

   BOOL IsParallel(const Vector2 &other, const float tol) const;
};

inline int operator == ( const Vector2 &a, const Vector2 &b )
{
   return (a.x==b.x) && (a.y==b.y);
}

inline void Vector2::operator += ( const Vector2 &other )
{
   x += other.x;
   y += other.y;
}

inline void Vector2::operator -= ( const Vector2 &other )
{
   x -= other.x;
   y -= other.y;
}

inline void Vector2::operator *= ( const Vector2 &other )
{
   x *= other.x;
   y *= other.y;
}

inline void Vector2::operator *= ( const float scale )
{
   x *= scale;
   y *= scale;
}

inline void Vector2::operator /= ( const float scale )
{
   x /= scale;
   y /= scale;
}

inline void Vector2::operator /= ( const Vector2 &other )
{
   x /= other.x;
   y /= other.y;
}

inline Vector2 & Vector2::Negate()
{
   x = -x;
   y = -y;
   return *this;
}

inline void Vector2::Set( float xinit, float yinit )
{
   x = xinit;
   y = yinit;
}

inline void Vector2::Set( float init )
{
   x = init;
   y = init;
}

inline Vector2 operator / ( const Vector2 &a, const float s )
{
   return Vector2( a.x/s, a.y/s );
}

inline Vector2 operator - ( const Vector2 &unary )
{
   return Vector2( -unary.x, -unary.y );
}   

inline Vector2 operator + ( const Vector2 &a, const Vector2 &b )
{
   return Vector2( a.x+b.x, a.y+b.y );
}

inline Vector2 operator - ( const Vector2 &a, const Vector2 &b )
{
   return Vector2( a.x-b.x, a.y-b.y );
}

inline Vector2 operator * ( const Vector2 &a, const float s )
{
   return Vector2( a.x*s, a.y*s );
}

inline Vector2 operator * ( const float s, const Vector2 &a )
{
   return a*s;
}

inline Vector2 operator * ( const Vector2 &a, const Vector2 &b )
{
   return Vector2( a.x*b.x, a.y*b.y );
}

inline float operator | ( const Vector2 &a, const Vector2 &b )
{
   return a.x*b.x + a.y*b.y;
}

inline float operator ^ ( const Vector2 &a, const Vector2 &b )
{
   return a.x*b.y - a.y*b.x;
}

inline int operator != ( const Vector2 &a, const Vector2 &b )
{
   return !(a==b);
}   

inline float Vector2::Dot( const Vector2 &other )
{
   return x * other.x + y * other.y;
}

inline void Swap( Vector2 &a, Vector2 &b )
{
   Vector2 temp( a );
   a = b;
   b = temp;
}

inline Vector2 Vector2::Normalized() const
{
   float length;
   Vector2 n = *this;
   
   if (length = n.Length())
      n /= length;
   return n;
}

inline Vector2 Vector2::Absolute() const
{
   return Vector2( (float)fabs( x ), (float)fabs( y ));
}

inline Vector2 Vector2::Reciprocal() const
{
   return Vector2( 1.f/x, 1.f/y );
}

inline Vector2 Vector2::Orthogonal() const
{
   return Vector2( y, -x );
}

inline Vector::Vector( const Vector2 &v2 )
{
   x = v2.x; y = v2.y; z = 0.0f;
}

inline BOOL Vector2::IsParallel(const Vector2 &vOther, const float fTol) const
{
   float fDot = Normalized().Dot( vOther.Normalized() );
   float fAbsDot = fabs(fDot);

   if (fAbsDot >= (1.0f-(fabs(fTol))) && fAbsDot <= 1.0f)
      return TRUE;

   return FALSE;
}