//////////////////////////////////////////////////////////////////////////////////////////
//
//	HashVector4 CLASS
//
//////////////////////////////////////////////////////////////////////////////////////////

#define HASH_VECTOR4_H

#ifndef HASH_VECTOR3_H
#include "HashVector3.h"
#endif

class Matrix4x4;

class HashVector4 
{
protected:
   union {
      struct { double n[4]; };
   };

public:
   HashVector4( void ) {}
   HashVector4( double x, double y, double z, double w );
   HashVector4( double fill );
   HashVector4( const HashVector4 &v );
   HashVector4( const HashVector3 &other3 );
   
   friend class Matrix4x4;

   double operator[]( int i ) const;
   double &operator[]( int i );
   void Set( double x, double y, double z, double w );
   void Set( double fill )
   {
      n[0] = n[1] = n[2] = n[3] = fill;
   }
   HashVector4 & operator = ( const HashVector4 &v );
   HashVector4 & operator /= ( double scalar );
   HashVector4 & operator -= ( const HashVector4 &other );
   HashVector4 & operator += ( const HashVector4 &other );
   HashVector4 & operator *= ( double scale );
   int operator == ( const HashVector4 &a ) const;
   int operator != ( const HashVector4 &a ) const { return !(*this == a); }   

   HashVector4 operator + ( const HashVector4 &a ) const { return HashVector4(*this) += a; }
   HashVector4 operator - ( const HashVector4 &a ) const { return HashVector4(*this) -= a; }

   double operator | ( const HashVector4 &b ) const;   // Dot Product
   double operator | ( const HashVector3 &b ) const;   // Dot Product
   HashVector4 operator * ( const HashVector4 &b ) const // Member by member multiply
   {
      HashVector4 result;               // GOOD CANDIDATE FOR SIMD, NOT IN SML
      result[0] = n[0] * b[0];
      result[1] = n[1] * b[1];
      result[2] = n[2] * b[2];
      result[3] = n[3] * b[3];
      return result;
   }
   HashVector4 operator * ( double scalar ) const 
   {
      return HashVector4(*this) *= scalar;
   }
   friend HashVector4 operator * ( double scalar, const HashVector4 &v )
   {
      return v * scalar;
   }
   friend void Swap( HashVector4 &a, HashVector4 &b )
   {
      HashVector4 temp( a );
      a = b;
      b = temp;
   }
};


inline HashVector4::HashVector4( double x, double y, double z, double w )
{
   Set(x,y,z,w);
}

inline HashVector4::HashVector4( double fill )
{
   Set(fill);
}

inline HashVector4::HashVector4( const HashVector4 &other )
{
   *this = other;
}

inline HashVector4::HashVector4( const HashVector3 &other3 )
{
   n[0] = other3[0];
   n[1] = other3[1];
   n[2] = other3[2];
   n[3] = 1.0f;
}

inline double HashVector4::operator[]( int i ) const
{
   return n[i];
}

inline double &HashVector4::operator[]( int i )
{
   return n[i];
}

inline void HashVector4::Set( double x, double y, double z, double w )
{
    n[0] = x;
    n[1] = y;
    n[2] = z;
    n[3] = w;
}

inline HashVector4 & HashVector4::operator = ( const HashVector4 &other )
{
   n[0] = other[0];
   n[1] = other[1];
   n[2] = other[2];
   n[3] = other[3];
   return *this;
}

inline HashVector4 & HashVector4::operator /= ( double scalar )
{
   n[0] /= scalar;
   n[1] /= scalar;
   n[2] /= scalar;
   n[3] /= scalar;
   return *this;
}

inline HashVector4 & HashVector4::operator -= ( const HashVector4 &other )
{
   n[0] -= other[0];
   n[1] -= other[1];
   n[2] -= other[2];
   n[3] -= other[3];
   return *this;
}

inline HashVector4 & HashVector4::operator += ( const HashVector4 &other )
{
   n[0] += other[0];
   n[1] += other[1];
   n[2] += other[2];
   n[3] += other[3];
   return *this;
}

inline HashVector4 & HashVector4::operator *= ( double scale )
{
   n[0]*=scale;
   n[1]*=scale;
   n[2]*=scale;
   n[3]*=scale;
   return *this;
}

inline int HashVector4::operator == ( const HashVector4 &a ) const
{
   return n[0]==a[0] && n[1]==a[1] && n[2]==a[2] && n[3]==a[3];
}

inline double HashVector4::operator | ( const HashVector4 &b ) const   // Dot Product
{
   return n[0]*b[0] + n[1]*b[1] + n[2]*b[2] + n[3]*b[3];
}

inline double HashVector4::operator | ( const HashVector3 &b ) const   // Dot Product
{
   return n[0]*b[0] + n[1]*b[1] + n[2]*b[2] + n[3];
}
