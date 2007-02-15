// DAN  4/5/99  \Dan71\AMViewer\Channel.h

#define __CHANNEL_H

#include "Afxtempl.h"

#ifndef __HASHTIME_H
#include "HashTime.h"
#endif

#ifndef __MATRIX33_H
#include "Matrix33.h"
#endif

#ifndef __ROTATE_H
#include "Rotate.h"
#endif

class Keyframe {
public:
   enum INTERPOLATIONMETHOD { HOLD, LINEAR, SPLINE };
   Time  m_time;
   float m_value;
   Vector2 m_it, m_ot;
   union {
      BYTE m_flags;
      struct {
         BYTE  m_defaulttangents : 1,
               m_interpmethod    : 2;
      };
   };
};

class ChannelItem
{
public:
   CArray<Keyframe, Keyframe &>m_keyframes;
   int   m_cachedindex;
   float m_cachedvalue;
   Time  m_cachedtime;

   ChannelItem()
   {
       m_cachedindex = 0;
       m_cachedtime.SetTagged(TRUE);
   }
   float Interpolate(const Time &time);
   void CalcTangents(int index);

private:
   float InterpolateValue( const Time &time, int index);
};

class Channel
{
public:
   enum { XTRANS, YTRANS, ZTRANS, XSCALE, YSCALE, ZSCALE, XQUAT, YQUAT, ZQUAT, WQUAT, NUMCHANNELITEMS };

   CString m_name;
   Matrix33 m_bonetoparent;
   ChannelItem m_channelitem[10];

   TSQ GetTSQ(const Time &time);
   void ComputeParentToBone(const Vector &bonedirection, const Vector &rolldirection,
    const Vector &parentbonedirection, const Vector &parentrolldirection);
}; 

#define MAXFRAMEERROR      0.003f

class CubicCoefficients1D {
  public:
   float a, b, c, d;             // CUBIC COEFFICIENTS f(t) = a*t^3 + b*t^2 + c*t + d
   float fda, fdb, fdc, fdd;     // BUMP FD COEFFICIENTS
   
   CubicCoefficients1D( float p1, float p4, float r1, float r4 ) {
//   a     2 -2  1  1   p1
//   b =  -3  3 -2 -1 * p4
//   c     0  0  1  0   r1
//   d     1  0  0  0   r4
      a =  2*p1 + -2*p4 +    r1 +  r4;
      b = -3*p1 +  3*p4 + -2*r1 + -r4;
      c =                    r1      ;
      d =    p1                      ;
   }
   float Evaluate( float t ) { return t*(t*(a*t + b) + c) + d; }
   float Derivative( float t ) { return t*(3*a*t + 2*b) + c; }
   float InitFD( int steps ) {
//   fda     0          0          0     1       a
//   fdb  =  delta**3   delta**2   delta 0   *   b
//   fdc     6*delta**3 2*delta**2 0     0       c
//   fdd     6*delta**3 0          0     0       d
      float fd12 = 1.0f/steps;
      float fd11 = fd12 * fd12;
      float fd10 = fd12 * fd11;
      float fd20 = 6.0f * fd10;
      float fd21 = 2.0f * fd11;
      fda = d;
      fdb = a*fd10 + b*fd11 + c*fd12;
      fdc = a*fd20 + b*fd21;
      fdd = a*fd20;
      return fda;
   }   
   float BumpFD() { fda+=fdb; fdb+=fdc; fdc+=fdd; return fda; }
   float GetFDValue() { return fda; }
};

