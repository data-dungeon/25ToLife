#pragma once

#include "MotionCurveDriver.h"
#include "../Utility/ATime.h"
#include "../Utility/Vector2.h"

template< typename TYPE >
class BasicMotionCurveDriver : public MotionCurveDriver
{
public:
   template < typename TYPE >
   class Keyframe
   {
   public:
      Keyframe()
      {
         m_nFlags = 0;
         m_pIT = NULL;
         m_pOT = NULL;
      }

      Keyframe(Time time, TYPE value, int nNumChannelItems)
      {
         m_time = time;
         m_value = value;
         m_nNumChannelItems = nNumChannelItems;
         m_pIT = SNEW Vector2[nNumChannelItems];
         m_pOT = SNEW Vector2[nNumChannelItems];
         m_nFlags = 0;
      }
      ~Keyframe()
      {
         if (m_pIT)
            delete [] m_pIT;
         if (m_pOT)
            delete [] m_pOT;
      }
      
      Time  m_time;
      TYPE  m_value;
      int m_nNumChannelItems;
      Vector2 *m_pIT, *m_pOT;

      union 
      {
         unsigned char m_nFlags;
         struct 
         {
            unsigned char  m_bUserDefinedTangents   : 1;
            unsigned char  m_nInterpMethod          : 2;
         };
      };

      Keyframe &operator = (const Keyframe &other)
      {
         m_time = other.m_time;
         m_value = other.m_value;
         m_nNumChannelItems = other.m_nNumChannelItems;
         m_pIT = SNEW Vector2[m_nNumChannelItems];
         m_pOT = SNEW Vector2[m_nNumChannelItems];
         for (int nChannelItemIndex = 0; nChannelItemIndex < other.m_nNumChannelItems; nChannelItemIndex++)
         {
            m_pIT[nChannelItemIndex] = other.m_pIT[nChannelItemIndex];
            m_pOT[nChannelItemIndex] = other.m_pOT[nChannelItemIndex];
         }
         m_nFlags = other.m_nFlags;
         return *this;
      }
   };

  union {
      ULONG m_nBasicDriverFlags;
      struct {
         ULONG m_bDirtyTangents     : 1;
      };
   };

   CArray< Keyframe<TYPE> > m_keyframes;
   mutable int m_dindex;
   mutable TYPE m_dvalue;
   mutable Time m_dtime;

   BasicMotionCurveDriver()
   {
      m_dindex = 0;
      m_dtime.SetTagged(true);
   }

   BasicMotionCurveDriver &operator = (const BasicMotionCurveDriver &other)
   {
      m_keyframes = other.m_keyframes;
      m_dindex = other.m_dindex;
      m_dvalue = other.m_dvalue;
      m_dtime = other.m_dtime;
      return *this;
   }

   void RemoveAll();

   int StoreValue( const TYPE &value, const Time &time=GetTime(), BOOL bStoreUndo = TRUE);

   void SetKey(int nIndex, const TYPE &value, const Time &time);
   void GetKey(int nIndex,TYPE &value, Time &time);

   float InterpolateChannelItem(const Time &time,int nChannelItemIndex);

   BOOL IsTangentsDirty(){ return m_bDirtyTangents; }
   void SetTangentsDirtyFlag( BOOL bFlag ) { m_bDirtyTangents = bFlag; }

   // Motion Curve Driver Overrides
   virtual Time GetStartTime();
   virtual Time GetEndTime();
   virtual int GetNumKeys() { return m_keyframes.GetSize(); }
   virtual void SetNumKeys( int nSize );
   virtual void SetKeyInTangent(int nIndex, Vector2 inTan, int nChannelItemIndex=0);
   virtual void SetKeyOutTangent(int nIndex, Vector2 outTan, int nChannelItemIndex=0);
   virtual Vector2 GetKeyInTangent(int nIndex, int nChannelItemIndex=0); 
   virtual Vector2 GetKeyOutTangent(int nIndex, int nChannelItemIndex=0);
   virtual void SetKeyInterpolationMethod(int nIndex, InterpolationMethod nInterpolationMethod);
   virtual InterpolationMethod GetKeyInterpolationMethod(int nIndex);

private:
   float InterpolateChannelItemValue( const Time &time, int index, int nChannelItemIndex );
   void CalcTangents();
   void CalcTangents(int index);
   int FindClosestKey(Time time);
};

class CubicCoefficients1D 
{
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

#include "BasicMotionCurveDriver.hpp"