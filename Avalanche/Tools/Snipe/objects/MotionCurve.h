#pragma once

#include "MotionKey.h"

#define MAXFRAMEERROR 0.003f

class MotionCurveDriver;
class ModifiedMotionCurve;

class MotionCurve 
{
private:
   union {
         ULONG    m_nMotionCurveFlags;
         struct {
            ULONG m_nDirtyMaxAndMinValue          : 1,
                  m_nModifiedCurveDirty           : 1,
                  m_nLastTimeValid                : 1;
         };
   };

   static GLImageList m_glImageListKey;

   MotionCurveDriver *m_pMotionCurveDriver;
   MotionKey *m_pHeadKey;
   MotionKey *m_pTailKey;
   MotionKey *m_pLastKey;
   Time m_dlastTime;
   RGBAFloat  m_color;
   ModifiedMotionCurve *m_pModifiedMotionCurve;

public:
   MotionCurve()
   {
      m_pMotionCurveDriver = NULL;
      m_pHeadKey = NULL;
      m_pTailKey = NULL;
      m_pLastKey = NULL;
      m_dlastTime.SetTagged(true);
      m_nLastTimeValid = false;
      m_color.Set(1.0f,1.0f,1.0f,1.0f);
      m_pModifiedMotionCurve = NULL;
   }
   virtual ~MotionCurve();

   void SetMotionCurveDriver( MotionCurveDriver *pMotionCurveDriver ) { m_pMotionCurveDriver = pMotionCurveDriver; }
   MotionCurveDriver *GetMotionCurveDriver() { return m_pMotionCurveDriver; }
   void SetColor( RGBAFloat color ) { m_color = color; }
   const RGBAFloat &GetColor() { return m_color; }
   UINT CountNumKeys() { UINT nCount = 0; for (MotionKey *pKey = m_pHeadKey; pKey; pKey = pKey->GetNextKey(), nCount++); return nCount; }
   int FindKeyIndex(MotionKey *pFindKey);
   MotionKey *GetHeadKey(BOOL bModified = TRUE);
   MotionKey *GetTailKey(BOOL bModified = TRUE);
   MotionKey *GetLastKey(BOOL bModified = TRUE);
   void SetLastKey( MotionKey *pLastKey ) { m_pLastKey = pLastKey; }
   BOOL StoreKey( MotionKey *pNewKey, BOOL bStoreUndo=TRUE );
   
   void InsertKeyBefore( MotionKey *pAt, MotionKey *pKey );
   void InsertKeyAfter( MotionKey *pAt, MotionKey *pKey );
   void AddKey( MotionKey *pKey );
   MotionKey *FindClosestKey( const Time &time );
   MotionKey *CreateMotionKey( const Time &time );
   void RemoveAll();
   void RemoveKey( MotionKey *pKey );

   void PushBounds( BoundingBox &box, CGLBaseView *pView=NULL );
   Time GetLength(BOOL bModified = TRUE);

   Time GetStartTime(BOOL bModified = TRUE);
   Time GetStopTime(BOOL bModified = TRUE);

   Time GetLastTime() const { return m_dlastTime; }
   void SetLastTime( const Time &time ) { m_dlastTime = time; }

   bool GetLastTimeValid() const { return m_nLastTimeValid; }
   void SetLastTimeValid( BOOL bFlag ) { m_nLastTimeValid = bFlag; }

   void SetMinAndMaxValueDirty(BOOL bFlag) { m_nDirtyMaxAndMinValue = bFlag; }
   BOOL IsMinAndMaxValueDirty() { return m_nDirtyMaxAndMinValue; }

   void SetModifiedCurveDirty(BOOL bFlag) { m_nModifiedCurveDirty = bFlag; }
   BOOL IsModifiedCurveDirty() { return m_nModifiedCurveDirty; }

   void AddModifiedMotionCurve();
   void RemoveModifiedMotionCurve();
   ModifiedMotionCurve *GetModifiedMotionCurve();

// Overridables
   virt_base virtual MotionKey *AllocateMotionKey() { return NULL; }
   virt_base virtual void Draw( CGLBaseView *pView ) {}
   virt_base virtual float GetMinValue() { return 0.0f; }
   virt_base virtual float GetMaxValue() { return 1.0f; }
   virt_base virtual void ReduceKeys( float fTolerance, MotionCurve *pDstMotionCurve ) {}
};

typedef Vector2 *BezierCurve;

class FloatMotionCurve : public MotionCurve
{
public:
   static BOOL m_bDrawInBackground;

   float m_dlastValue;

   float m_fMaxValue;
   float m_fMinValue;

   FloatMotionCurve()
   {
      m_dlastValue = 0.0f;
      m_fMaxValue = 0.0f;
      m_fMinValue = 0.0f;
   }

private:
   float InterpolateValue( const Time &time, FloatMotionKey *pKey );
   void CalcMinAndMaxValue();

public:
   FloatMotionKey *GetHeadKey(BOOL bModified = TRUE) { return (FloatMotionKey *)MotionCurve::GetHeadKey(bModified); }
   FloatMotionKey *GetTailKey(BOOL bModified = TRUE) { return (FloatMotionKey *)MotionCurve::GetTailKey(bModified); }
   FloatMotionKey *GetLastKey(BOOL bModified = TRUE) { return (FloatMotionKey *)MotionCurve::GetLastKey(bModified); }

   FloatMotionKey *StoreValue( float fValue, const Time &time=GetTime(), BOOL bStoreUndo=TRUE );
   float GetValue( const Time &time, BOOL bModified = TRUE );

   void RebuildAsCubic( float fTolerance, FloatMotionCurve *pDstMotionCurve );
   void RebuildAsLinear( float fTolerance, FloatMotionCurve *pDstMotionCurve );

private:
   //  rebuild as cubic
   void FitCubicCurve( Vector2 *pPoints, int nNumPoints, double dErrorSquared, FloatMotionCurve *pDstMotionCurve );
   void FitCubic( Vector2 *pPoints, int nFirst, int nLast, const Vector2 &tHat1, const Vector2 &tHat2, double dErrorSquared, FloatMotionCurve *pDstMotionCurve );
   void AddBezierCurve( BezierCurve bezCurve, FloatMotionCurve *pMotionCurve );
   Vector2 ComputeLeftTangent( Vector2 *pPoints, int nEnd );
   Vector2 ComputeRightTangent( Vector2 *pPoints, int nEnd );
   Vector2 ComputeCenterTangent( Vector2 *pPoints, int nCenter );
   double *ChordLengthParameterize( Vector2 *pPoints, int nFirst, int nLast );
   BezierCurve GenerateBezier( Vector2 *pPoints, int nFirst, int nLast, double *uPrime, const Vector2 &tHat1, const Vector2 &tHat2 );
   double ComputeCubicMaxError( Vector2 *pPoints, int nFirst, int nLast, BezierCurve bezCurve, double *u, int *splitPoint );
   double *Reparameterize( Vector2 *pPoints, int nFirst, int nLast, double *u, BezierCurve bezCurve );
   Vector2 BezierII( int degree, Vector2 *V, double t );
   double NewtonRaphsonRootFind( BezierCurve Q, const Vector2 &P, double u );

   // rebuild as linear
   void FitLinearCurve(const CArray<Vector2> &points, CArray<Vector2> &linearPointsFill, double error);
   void FitLinear(const CArray<Vector2> &points, int first, int last, CArray<Vector2> &linearPointsFill, double error);
   double ComputeLinearMaxError(const CArray<Vector2> &points, int first, int last,int &splitPoint);

public:
// MotionCurve Overrides
   virtual MotionKey *AllocateMotionKey() { return FloatMotionKey::Create(); }
   virtual float GetMinValue();
   virtual float GetMaxValue();
//   virtual void ReduceKeys( float fTolerance, MotionCurve *pDstMotionCurve );

// SnipeObject overrides
   virtual void Draw( CGLBaseView *pView );
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

class StringMotionCurve : public MotionCurve
{
public:
   static BOOL m_bDrawInBackground;

   String m_slastValue;

   StringMotionCurve()
   {
   }

public:
   StringMotionKey *GetHeadKey(BOOL bModified = TRUE) { return (StringMotionKey *)MotionCurve::GetHeadKey(bModified); }
   StringMotionKey *GetTailKey(BOOL bModified = TRUE) { return (StringMotionKey *)MotionCurve::GetTailKey(bModified); }
   StringMotionKey *GetLastKey(BOOL bModified = TRUE) { return (StringMotionKey *)MotionCurve::GetLastKey(bModified); }

   StringMotionKey *StoreValue(const String &sValue, const Time &time=GetTime(), BOOL bStoreUndo=TRUE );
   String GetValue( const Time &time );

public:
   // MotionCurve Overrides
   virtual MotionKey *AllocateMotionKey() { return StringMotionKey::Create(); }

   // SnipeObject overrides
   virtual void Draw( CGLBaseView *pView );
};
