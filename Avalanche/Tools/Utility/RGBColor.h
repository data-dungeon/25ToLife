#pragma once

#include "BaseStream.h"

#ifndef UTILITYEXPORT
#define UTILITYEXPORT
#endif

inline BYTE ToByte(BYTE a) { return a; }
inline BYTE ToByte(float a) { return fast_round(MIN(255.0f, a*255.0f)); }
//inline BYTE ToByte(half a) { return fast_round(MIN(255.0f, a*255.0f)); }

inline float ToFloat(float a) { return a; }
//inline float ToFloat(half a) { return (float)a; }
inline float ToFloat(BYTE a) { return (float)a/255; }

template <class TYPE>
class RGBColor {
private:
   TYPE m_blue;
   TYPE m_green;
   TYPE m_red;
public:

   RGBColor() { }
   RGBColor( TYPE red, TYPE green, TYPE blue ) { m_red = red; m_green = green; m_blue = blue; }
   RGBColor( TYPE scalar ) { m_red = m_green = m_blue = scalar; }
   RGBColor( const RGBColor &other ) { *this = other; }

   RGBColor & operator = ( TYPE scalar) { m_red = m_green = m_blue = scalar; return *this; }
   RGBColor & operator = ( const RGBColor<TYPE> &other) { m_red = other.GetRed(); m_green = other.GetGreen(); m_blue = other.GetBlue(); return *this; }

   RGBColor & operator += ( const RGBColor<TYPE> &other ) { m_red += other.GetRed(); m_green += other.GetGreen(); m_blue += other.GetBlue(); return *this; }
   RGBColor & operator -= ( const RGBColor<TYPE> &other ) { m_red -= other.GetRed(); m_green -= other.GetGreen(); m_blue -= other.GetBlue(); return *this; }
   RGBColor & operator *= ( const RGBColor<TYPE> &other ) { m_red *= other.GetRed(); m_green *= other.GetGreen(); m_blue *= other.GetBlue(); return *this; }
   
   RGBColor & operator *= ( float scale ) { m_red = (float)m_red * scale; m_green = (float)m_green * scale; m_blue = (float)m_blue * scale; return *this; }
   RGBColor & operator /= ( const RGBColor<TYPE> &other ) { m_red /= other.GetRed(); m_green /= other.GetGreen(); m_blue /= other.GetBlue(); return *this; }
   RGBColor & operator /= ( float scale )              { return (*this *= (1/scale)); }

   void Set( TYPE red, TYPE green, TYPE blue )  { m_red = red; m_green = green; m_blue = blue; }
   void Set( TYPE init ) { m_red = m_green = m_blue = init; }

   int operator == ( const RGBColor<TYPE> &a ) const { return (m_red == a.GetRed()) && (m_green == a.GetGreen()) && (m_blue == a.GetBlue()); }
   int operator != ( const RGBColor<TYPE> &a ) const { return !(*this == a); }   

   RGBColor operator + ( const RGBColor<TYPE> &a ) const { return RGBColor<TYPE>(*this) += a; }
   RGBColor operator - ( const RGBColor<TYPE> &a ) const { return RGBColor<TYPE>(*this) -= a; }
   RGBColor operator * ( const RGBColor<TYPE> &a ) const { return RGBColor<TYPE>(*this) *= a; }
   RGBColor operator * ( float s )                    const { return RGBColor<TYPE>(*this) *= s; }
   RGBColor operator / ( float s )                    const { return RGBColor<TYPE>(*this) /= s; }
   RGBColor operator / ( const RGBColor<TYPE> &a ) const { return RGBColor<TYPE>(*this) /= a; }
   TYPE operator[]( int i ) const { ASSERT(i < 3); return (&m_blue)[i]; }

   TYPE GetRed() const  { return m_red; }
   TYPE GetGreen() const { return m_green; }
   TYPE GetBlue() const { return m_blue; }

   void SetRed(TYPE red)     { m_red = red; }   
   void SetGreen(TYPE green) { m_green = green; }   
   void SetBlue(TYPE blue)   { m_blue = blue; }   

   COLORREF GetCOLORREF() const { return RGB(ToByte(m_red), ToByte(m_green), ToByte(m_blue)); }
   float GetLuminance() const { return 0.3f*ToFloat(m_red) + 0.59f*ToFloat(m_green) + 0.11f*ToFloat(m_blue); }

   friend RGBColor<TYPE> operator * ( const float s, const RGBColor<TYPE> &a )  { return a * s; }
   
   void Min(TYPE min) { if (m_red<min) m_red=min; if (m_green<min) m_green=min; if (m_blue<min) m_blue=min; }
   void Max(TYPE max) { if (m_red>max) m_red=max; if (m_green>max) m_green=max; if (m_blue>max) m_blue=max; }
   void MinMax(TYPE min, TYPE max) { if (m_red>max)   m_red=max;   else if (m_red<min) m_red=min; 
                                     if (m_green>max) m_green=max; else if (m_green<min) m_green=min;
                                     if (m_blue>max)  m_blue=max;  else if (m_blue<min) m_blue=min; }
};
#define V_PI 3.1415926535898
#include <math.h>

inline RGBColor<float> GetSpectrumColor(float x)
{
   float theta = (1.66f * (float)V_PI) * x;
   return RGBColor<float>((2.0f+(float)cos(theta))/3.0f, (1.0f+(float)cos(theta-(2.0f*V_PI/3.0f)))/2.0f, (2.0f+(float)cos(theta-(4.0f*V_PI/3.0f)))/3.0f);
}

template <class TYPE>
inline BaseStream &operator << (BaseStream &bs, const RGBColor<TYPE> &color)
{
   return bs << color.GetRed() << sp << color.GetGreen() << sp << color.GetBlue();
}

template <class TYPE>
inline BaseStream &operator >> (BaseStream &bs, RGBColor<TYPE> &color)
{
   TYPE value;
   bs >> value; color.SetRed( value );
   bs >> value; color.SetGreen( value );
   bs >> value; color.SetBlue( value );
   return bs;
}

//*******************************************************************************************************
//* RGBAColor

template <class TYPE>
class RGBAColor {
private:
   TYPE m_blue;
   TYPE m_green;
   TYPE m_red;
   TYPE m_alpha;

public:
   RGBAColor() { }
   RGBAColor( TYPE red, TYPE green, TYPE blue, TYPE alpha ) { m_red = red; m_green = green; m_blue = blue; m_alpha = alpha; }
   RGBAColor( TYPE scalar ) { m_red = m_green = m_blue = m_alpha = scalar; }
   RGBAColor( const RGBAColor<TYPE> &other ) { *this = other; }
   RGBAColor( const RGBColor<TYPE> &other, TYPE alpha ) { m_red = other.GetRed(); m_green = other.GetGreen(); m_blue = other.GetBlue(); m_alpha = alpha; }

   RGBAColor & operator = ( TYPE scalar) { m_red = m_green = m_blue = m_alpha = scalar; return *this; }
   RGBAColor & operator = ( const RGBAColor<TYPE> &other) { m_red = other.GetRed(); m_green = other.GetGreen(); m_blue = other.GetBlue(); m_alpha = other.GetAlpha(); return *this; }

   RGBAColor & operator += ( const RGBAColor<TYPE> &other ) { m_red += other.GetRed(); m_green += other.GetGreen(); m_blue += other.GetBlue(); m_alpha += other.GetAlpha(); return *this; }
   RGBAColor & operator -= ( const RGBAColor<TYPE> &other ) { m_red -= other.GetRed(); m_green -= other.GetGreen(); m_blue -= other.GetBlue(); m_alpha -= other.GetAlpha(); return *this; }
   RGBAColor & operator *= ( const RGBAColor<TYPE> &other ) { m_red *= other.GetRed(); m_green *= other.GetGreen(); m_blue *= other.GetBlue(); m_alpha *= other.GetAlpha(); return *this; }
   
   RGBAColor & operator *= ( float scale ) { m_red = (float)m_red * scale; m_green = (float)m_green * scale; m_blue = (float)m_blue * scale; m_alpha = (float)m_alpha * scale; return *this; }
   RGBAColor & operator /= ( const RGBAColor<TYPE> &other ) { m_red /= other.GetRed(); m_green /= other.GetGreen(); m_blue /= other.GetBlue(); m_alpha /= other.GetAlpha(); return *this; }
   RGBAColor & operator /= ( float scale ) { return (*this *= (1/scale)); }

   void Set( TYPE red, TYPE green, TYPE blue, TYPE alpha ) { m_red = red; m_green = green; m_blue = blue; m_alpha = alpha; }
   void Set( TYPE init ) { m_red = m_green = m_blue = m_alpha = init; }

   int operator == ( const RGBAColor<TYPE> &a ) const { return (m_red == a.GetRed()) && (m_green == a.GetGreen()) && (m_blue == a.GetBlue())&& (m_alpha == a.GetAlpha()); }
   int operator != ( const RGBAColor<TYPE> &a ) const { return !(*this == a); }   

   RGBAColor operator + ( const RGBAColor<TYPE> &a ) const { return RGBAColor<TYPE>(*this) += a; }
   RGBAColor operator - ( const RGBAColor<TYPE> &a ) const { return RGBAColor<TYPE>(*this) -= a; }
   RGBAColor operator * ( const RGBAColor<TYPE> &a ) const { return RGBAColor<TYPE>(*this) *= a; }
   RGBAColor operator * ( float s ) const { return RGBAColor<TYPE>(*this) *= s; }
   RGBAColor operator / ( float s ) const { return RGBAColor<TYPE>(*this) /= s; }
   RGBAColor operator / ( const RGBAColor<TYPE> &a ) const { return RGBAColor<TYPE>(*this) /= a; }

   TYPE operator[]( int i ) const { ASSERT(i < 4); return (&m_blue)[i]; }

   TYPE GetRed() const   { return m_red; }
   TYPE GetGreen() const { return m_green; }
   TYPE GetBlue() const  { return m_blue; }
   TYPE GetAlpha() const { return m_alpha; }

   void SetRed(TYPE red)     { m_red = red; }   
   void SetGreen(TYPE green) { m_green = green; }   
   void SetBlue(TYPE blue)   { m_blue = blue; }   
   void SetAlpha(TYPE alpha) { m_alpha = alpha; }   

   COLORREF GetCOLORREF() const { return RGB(ToByte(m_red), ToByte(m_green), ToByte(m_blue)); }
   float GetLuminance() const { return 0.3f*ToFloat(m_red) + 0.59f*ToFloat(m_green) + 0.11f*ToFloat(m_blue); }

   friend RGBAColor<TYPE> operator * ( const float s, const RGBAColor<TYPE> &a )  { return a * s; }
   
   void Min(TYPE min) { if (m_red<min) m_red=min; if (m_green<min) m_green=min; if (m_blue<min) m_blue=min; if (m_alpha<min) m_alpha=min; }
   void Max(TYPE max) { if (m_red>max) m_red=max; if (m_green>max) m_green=max; if (m_blue>max) m_blue=max; if (m_alpha>max) m_alpha=max; }
   void MinMax(TYPE min, TYPE max) { if (m_red>max)   m_red=max;   else if (m_red<min)   m_red=min; 
                                     if (m_green>max) m_green=max; else if (m_green<min) m_green=min;
                                     if (m_blue>max)  m_blue=max;  else if (m_blue<min)  m_blue=min;
                                     if (m_alpha>max) m_alpha=max; else if (m_alpha<min) m_alpha=min; }
};

template <class TYPE>
inline BaseStream &operator << (BaseStream &bs, const RGBAColor<TYPE> &color)
{
   return bs << color.GetRed() << sp << color.GetGreen() << sp << color.GetBlue() << sp << color.GetAlpha();
}

template <class TYPE>
inline BaseStream &operator >> (BaseStream &bs, RGBAColor<TYPE> &color)
{
   TYPE value;
   bs >> value; color.SetRed( value );
   bs >> value; color.SetGreen( value );
   bs >> value; color.SetBlue( value );
   bs >> value; color.SetAlpha( value );
   return bs;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LAColor Luminance and Alpha
template <class TYPE >
class LAColor 
{
private:
   TYPE m_lum;
   TYPE m_alpha;

public:
   LAColor() {}
   LAColor( TYPE lum, TYPE alpha ) { m_lum = lum; m_alpha = alpha; }
   LAColor( TYPE scalar ) { m_lum = m_alpha = scalar; }
   LAColor( const LAColor<TYPE> &other ) { *this = other; }
   LAColor & operator = ( TYPE scalar) { m_lum = m_alpha = scalar; return *this; }
   LAColor & operator = ( const LAColor<TYPE> &other) { m_lum = other.GetLum(); m_alpha = other.GetAlpha(); return *this; }

   LAColor & operator += ( const LAColor<TYPE> &other ) { m_lum += other.GetLum(); m_alpha += other.GetAlpha(); return *this; }
   LAColor & operator -= ( const LAColor<TYPE> &other ) { m_lum -= other.GetLum(); m_alpha -= other.GetAlpha(); return *this; }
   LAColor & operator *= ( const LAColor<TYPE> &other ) { m_lum *= other.GetLum(); m_alpha *= other.GetAlpha(); return *this; }
   
   LAColor & operator *= ( float scale ) { m_lum = (float)m_lum * scale; m_alpha = (float)m_alpha * scale; return *this; }
   LAColor & operator /= ( const LAColor<TYPE> &other ) { m_lum /= other.GetLum(); m_alpha /= other.GetAlpha(); return *this; }
   LAColor & operator /= ( float scale )              { return (*this *= (1/scale)); }

   void Set( TYPE lum, TYPE alpha )  { m_lum = lum; m_alpha = alpha; }

   int operator == ( const LAColor<TYPE> &a ) const { return (m_lum == a.GetLum()) && (m_alpha == a.GetAlpha()); }
   int operator != ( const LAColor<TYPE> &a ) const { return !(*this == a); }   

   LAColor operator + ( const LAColor<TYPE> &a ) const { return LAColor<TYPE>(*this) += a; }
   LAColor operator - ( const LAColor<TYPE> &a ) const { return LAColor<TYPE>(*this) -= a; }
   LAColor operator * ( const LAColor<TYPE> &a ) const { return LAColor<TYPE>(*this) *= a; }
   LAColor operator * ( float s )                const { return LAColor<TYPE>(*this) *= s; }
   LAColor operator / ( float s )                const { return LAColor<TYPE>(*this) /= s; }
   LAColor operator / ( const LAColor<TYPE> &a ) const { return LAColor<TYPE>(*this) /= a; }

   TYPE GetLum() const  { return m_lum; }
   TYPE GetAlpha() const { return m_alpha; }

   void SetLum(TYPE lum)     { m_lum = lum; }   
   void SetAlpha(TYPE alpha) { m_alpha = alpha; }   

   COLORREF GetCOLORREF() const { return RGB(ToByte(m_lum), ToByte(m_lum), ToByte(m_lum)); }
   float GetLuminance() const { return ToFloat(m_lum); }

   friend LAColor<TYPE> operator * ( const float s, const LAColor<TYPE> &a )  { return a * s; }
   
   void Min(TYPE min) { if (m_lum<min) m_lum=min; if (m_alpha<min) m_alpha=min; }
   void Max(TYPE max) { if (m_lum>max) m_lum=max; if (m_alpha>max) m_alpha=max; }
   void MinMax(TYPE min, TYPE max) 
   { 
      if (m_lum>max)   
         m_lum=max;   
      else 
         if (m_lum<min)
            m_lum=min; 
      
      if (m_alpha>max) 
         m_alpha=alpha;
   }
};

template <class TYPE>
inline BaseStream &operator << (BaseStream &bs, const LAColor<TYPE> &color)
{
   return bs << color.GetLum() << sp << color.GetAlpha();
}

template <class TYPE>
inline BaseStream &operator >> (BaseStream &bs, LAColor<TYPE> &color)
{
   TYPE value;
   bs >> value; color.SetLum( value );
   bs >> value; color.SetAlpha( value );
   return bs;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// typedefs
typedef LAColor<BYTE> LAByte;
typedef LAColor<float> LAFloat;
//typedef LAColor<half> LAHalf;

typedef RGBColor<BYTE> RGBByte;
typedef RGBColor<float> RGBFloat;
//typedef RGBColor<half> RGBHalf;

typedef RGBAColor<BYTE> RGBAByte;
typedef RGBAColor<float> RGBAFloat;
//typedef RGBAColor<half> RGBAHalf;
