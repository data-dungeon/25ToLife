#pragma once

#include "FastFTOL.h"
#include "Vector.h"

class ColorVector {
public:
   float red;
   float green;
   float blue;

   ColorVector( void ) { }
   ColorVector( const float pred, const float pgreen, const float pblue ) { red = pred; green = pgreen; blue = pblue; }
   ColorVector( const float scalar ) { red = green = blue = scalar; }
   ColorVector( COLORREF c ) { red=GetRValue(c); green=GetGValue(c); blue=GetBValue(c); }
   ColorVector( const ColorVector &other ) { *this = other; }

   ColorVector & operator = ( const float scalar) { red = green = blue = scalar; return *this; }
   ColorVector & operator = ( const ColorVector &other) { red = other.red; green = other.green; blue = other.blue; return *this; }

   ColorVector & operator += ( const ColorVector &other ) { red += other.red; green += other.green; blue += other.blue; return *this; }
   ColorVector & operator -= ( const ColorVector &other ) { red -= other.red; green -= other.green; blue -= other.blue; return *this; }
   ColorVector & operator *= ( const ColorVector &other ) { red *= other.red; green *= other.green; blue *= other.blue; return *this; }
   ColorVector & operator *= ( float scale )              { red *= scale; green *= scale; blue *= scale; return *this; }
   ColorVector & operator /= ( const ColorVector &other ) { red /= other.red; green /= other.green; blue /= other.blue; return *this; }
   ColorVector & operator /= ( float scale )              { return (*this *= (1/scale)); }

   void Set( float pred, float pgreen, float pblue )  { red = pred; green = pgreen; blue = pblue; }
   void Set( float init ) { red = green = blue = init; }

   int operator == ( const ColorVector &a ) const { return (red == a.red) && (green == a.green) && (blue == a.blue); }
   int operator != ( const ColorVector &a ) const { return !(*this == a); }   

   ColorVector operator + ( const ColorVector &a ) const { return ColorVector(*this) += a; }
   ColorVector operator - ( const ColorVector &a ) const { return ColorVector(*this) -= a; }
   ColorVector operator * ( const ColorVector &a ) const { return ColorVector(*this) *= a; }
   ColorVector operator * ( float s )              const { return ColorVector(*this) *= s; }
   ColorVector operator / ( float s )              const { return ColorVector(*this) /= s; }
   ColorVector operator / ( const ColorVector &a ) const { return ColorVector(*this) /= a; }
   float operator[]( int i ) const { return (&red)[i]; }

   COLORREF GetCOLORREF() const { return RGB(fast_round(red), fast_round(green), fast_round(blue)); }
   float GetLuminance() const { return 0.3f*red + 0.59f*green + 0.11f*blue; }

   friend ColorVector operator * ( const float s, const ColorVector &a )  { return a * s; }
   
   void Min(float min) { if (red<min) red=min; if (green<min) green=min; if (blue<min) blue=min; }
   void Max(float max) { if (red>max) red=max; if (green>max) green=max; if (blue>max) blue=max; }
   void MinMax(float min, float max) { if (red>max) red=max; else if (red<min) red=min; 
                                       if (green>max) green=max; else if (green<min) green=min;
                                       if (blue>max) blue=max; else if (blue<min) blue=min; }
};
#define V_PI 3.1415926535898
#include <math.h>

inline ColorVector GetSpectrumColor(float x)
{
   float theta = (1.66f * (float)V_PI) * x;
   return ColorVector((2.0f+(float)cos(theta))/3.0f, (1.0f+(float)cos(theta-(2.0f*V_PI/3.0f)))/2.0f, (2.0f+(float)cos(theta-(4.0f*V_PI/3.0f)))/3.0f);
}

