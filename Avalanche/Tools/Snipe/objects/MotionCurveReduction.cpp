#include "stdafx.h"
#include "MotionCurve.h"

// CUBIC KEY REDUCTION

void FloatMotionCurve::RebuildAsCubic( float fTolerance, FloatMotionCurve *pDstMotionCurve )
{
   float fErrorTolerance = max( 0.01f, fTolerance );
   fErrorTolerance *= fErrorTolerance;
   fErrorTolerance /= 40.0;

   pDstMotionCurve->RemoveAll();

   float fps = GetFPS();
   Time tTotalTime = GetLength( FALSE );
   int nFrames = tTotalTime.GetSeconds() * fps + 1, nFrame=0;
   if (nFrames > 1)
   {
      Vector2 *pPoints = SNEW Vector2[ nFrames ];
      for (Time tTime=0; tTime<=tTotalTime; tTime.StepFrame(fps), nFrame++)
         pPoints[ nFrame ].Set( tTime.GetSeconds(), GetValue( tTime, FALSE ) );

      FitCubicCurve( pPoints, nFrames, fErrorTolerance, (FloatMotionCurve*)pDstMotionCurve );

      delete [] pPoints;
   }
   else
   {
      FloatMotionKey *pOrigKey = GetHeadKey( FALSE );
      if (pOrigKey)
      {
         FloatMotionKey *pNewKey = (FloatMotionKey *)pDstMotionCurve->CreateMotionKey( pOrigKey->GetTime() );
         pNewKey->SetValue( pOrigKey->GetValue() );
      }
   }
}

void FloatMotionCurve::FitCubicCurve( Vector2 *pPoints, int nNumPoints, double dErrorSquared, FloatMotionCurve *pDstMotionCurve )
{
   Vector2 tHat1 = ComputeLeftTangent(pPoints, 0); /*  Unit tangent vectors at endpoints */
   Vector2 tHat2 = ComputeRightTangent(pPoints, nNumPoints - 1);
   FitCubic( pPoints, 0, nNumPoints - 1, tHat1, tHat2, dErrorSquared, pDstMotionCurve );
}

/*
*  FitCubic :
*  	Fit a Bezier curve to a (sub)set of digitized points
*/
void FloatMotionCurve::FitCubic( Vector2 *pPoints, int nFirst, int nLast, const Vector2 &tHat1, const Vector2 &tHat2, double dErrorSquared, FloatMotionCurve *pDstMotionCurve )
{
   BezierCurve	bezCurve; /*Control points of fitted Bezier curve*/
   double	*u;		/*  Parameter values for point  */
   double	*uPrime;	/*  Improved parameter values */
   double	maxError;	/*  Maximum fitting error	 */
   int		splitPoint;	/*  Point to split point set at	 */
   int		nPts;		/*  Number of points in subset  */
   double	iterationError; /*Error below which you try iterating  */
   int		maxIterations = 4; /*  Max times to try iterating  */
   Vector2	tHatCenter;   	/* Unit tangent vector at splitPoint */
   int		i;		

   iterationError = dErrorSquared * dErrorSquared;
   nPts = nLast - nFirst + 1;

   /*  Use heuristic if region only has two points in it */
   if (nPts == 2)
   {
      double dist = (pPoints[nLast] - pPoints[nFirst]).Length() / 3.0;

      bezCurve = (Vector2 *)malloc(4 * sizeof(Vector2));
      bezCurve[0] = pPoints[nFirst];
      bezCurve[3] = pPoints[nLast];
      bezCurve[1] = bezCurve[0] + (tHat1 * dist);
      bezCurve[2] = bezCurve[3] + (tHat2 * dist);
      AddBezierCurve(bezCurve,pDstMotionCurve);

      return;
   }

   /*  Parameterize points, and attempt to fit curve */
   u = ChordLengthParameterize(pPoints, nFirst, nLast);
   bezCurve = GenerateBezier(pPoints, nFirst, nLast, u, tHat1, tHat2);

   /*  Find max deviation of points to fitted curve */
   maxError = ComputeCubicMaxError(pPoints, nFirst, nLast, bezCurve, u, &splitPoint);
   if (maxError < dErrorSquared)
   {
      AddBezierCurve(bezCurve,pDstMotionCurve);
      free((void *)u);
      free((void *)bezCurve);
      return;
   }

   /*  If error not too large, try some reparameterization  */
   /*  and iteration */
   if (maxError < iterationError)
   {
      for (i = 0; i < maxIterations; i++)
      {
         uPrime = Reparameterize(pPoints, nFirst, nLast, u, bezCurve);
         bezCurve = GenerateBezier(pPoints, nFirst, nLast, uPrime, tHat1, tHat2);
         maxError = ComputeCubicMaxError(pPoints, nFirst, nLast, bezCurve, uPrime, &splitPoint);
         if (maxError < dErrorSquared)
         {
            AddBezierCurve(bezCurve,pDstMotionCurve);
            free((void *)u);
            free((void *)bezCurve);
            return;
         }
         free((void *)u);
         u = uPrime;
      }
   }

   /* Fitting failed -- split at max error point and fit recursively */
   free((void *)u);
   free((void *)bezCurve);
   tHatCenter = ComputeCenterTangent(pPoints, splitPoint);
   FitCubic(pPoints, nFirst, splitPoint, tHat1, tHatCenter, dErrorSquared,pDstMotionCurve);
   tHatCenter.Negate();
   FitCubic(pPoints, splitPoint, nLast, tHatCenter, tHat2, dErrorSquared,pDstMotionCurve);
}

void FloatMotionCurve::AddBezierCurve( BezierCurve bezCurve, FloatMotionCurve *pDstMotionCurve )
{
   FloatMotionKey *pStartKey = GetLastKey( FALSE );
   if (!pStartKey)
   {
      pStartKey = (FloatMotionKey *)pDstMotionCurve->CreateMotionKey( bezCurve[0].x );
      pStartKey->SetValue( bezCurve[0].y );
   }

   FloatMotionKey *pEndKey = (FloatMotionKey *)pDstMotionCurve->CreateMotionKey( bezCurve[3].x );
   pEndKey->SetValue( bezCurve[3].y );

   pStartKey->SetOutTangent( (bezCurve[1]-bezCurve[0])*3 );
   pEndKey->SetInTangent( (bezCurve[3]-bezCurve[2])*3 );
}

Vector2 FloatMotionCurve::ComputeLeftTangent( Vector2 *pPoints, int nEnd )
{
   return (pPoints[nEnd+1] - pPoints[nEnd]).Normalized();
}

Vector2 FloatMotionCurve::ComputeRightTangent( Vector2 *pPoints, int nEnd )
{
   return (pPoints[nEnd-1] - pPoints[nEnd]).Normalized();
}

Vector2 FloatMotionCurve::ComputeCenterTangent( Vector2 *pPoints, int nCenter )
{
   Vector2	V1, V2, tHatCenter;

   V1 = pPoints[nCenter-1] - pPoints[nCenter];
   V2 = pPoints[nCenter] - pPoints[nCenter+1];
   tHatCenter = (V1 + V2)/2.0;
   return tHatCenter.Normalized();
}

/*
*  ChordLengthParameterize :
*	Assign parameter values to digitized points 
*	using relative distances between points.
*/
double *FloatMotionCurve::ChordLengthParameterize( Vector2 *pPoints, int nFirst, int nLast )
{
   int		i;
   double	*u;			/*  Parameterization		*/

   u = (double *)malloc((unsigned)(nLast-nFirst+1) * sizeof(double));

   u[0] = 0.0;
   for (i = nFirst+1; i <= nLast; i++)
      u[i-nFirst] = u[i-nFirst-1] + (pPoints[i] - pPoints[i-1]).Length();

   for (i = nFirst + 1; i <= nLast; i++)
      u[i-nFirst] = u[i-nFirst] / u[nLast-nFirst];

   return u;
}

/*
*  B0, B1, B2, B3 :
*	Bezier multipliers
*/
static double B0( double u )
{
   double tmp = 1.0 - u;
   return (tmp * tmp * tmp);
}


static double B1( double u )
{
   double tmp = 1.0 - u;
   return (3 * u * (tmp * tmp));
}

static double B2( double u )
{
   double tmp = 1.0 - u;
   return (3 * u * u * tmp);
}

static double B3( double u )
{
   return (u * u * u);
}

/*
*  GenerateBezier :
*  Use least-squares method to find Bezier control points for region.
*/
#define MAXPOINTS	10000		/* The most points you can have */
BezierCurve FloatMotionCurve::GenerateBezier( Vector2 *pPoints, int nFirst, int nLast, double *uPrime, const Vector2 &tHat1, const Vector2 &tHat2 )
//    Point2	*d;			/*  Array of digitized points	*/
//    int		first, last;		/*  Indices defining region	*/
//    double	*uPrime;		/*  Parameter values for region */
//    Vector2	tHat1, tHat2;	/*  Unit tangents at endpoints	*/
{
   int 	i;
   Vector2 	A[MAXPOINTS][2];	/* Precomputed rhs for eqn	*/
   int 	nPts;			/* Number of pts in sub-curve */
   double 	C[2][2];			/* Matrix C		*/
   double 	X[2];			/* Matrix X			*/
   double 	det_C0_C1,		/* Determinants of matrices	*/
      det_C0_X,
      det_X_C1;
   double 	alpha_l,		/* Alpha values, left and right	*/
      alpha_r;
   Vector2 	tmp;			/* Utility variable		*/
   BezierCurve	bezCurve;	/* RETURN bezier curve ctl pts	*/

   bezCurve = (Vector2 *)malloc(4 * sizeof(Vector2));
   nPts = nLast - nFirst + 1;

   /* Compute the A's	*/
   for (i = 0; i < nPts; i++) {
      Vector2		v1, v2;
      v1 = tHat1 * B1(uPrime[i]);
      v2 = tHat2 * B2(uPrime[i]);
      A[i][0] = v1;
      A[i][1] = v2;
   }

   /* Create the C and X matrices	*/
   C[0][0] = 0.0;
   C[0][1] = 0.0;
   C[1][0] = 0.0;
   C[1][1] = 0.0;
   X[0]    = 0.0;
   X[1]    = 0.0;

   for (i = 0; i < nPts; i++) {
      C[0][0] += A[i][0].Dot(A[i][0]);
      C[0][1] += A[i][0].Dot(A[i][1]);
      /*					C[1][0] += V2Dot(&A[i][0], &A[i][1]);*/	
      C[1][0] = C[0][1];
      C[1][1] += A[i][1].Dot(A[i][1]);

      tmp = pPoints[nLast] * B3(uPrime[i]);
      tmp += pPoints[nLast] * B2(uPrime[i]);
      tmp += pPoints[nFirst] * B1(uPrime[i]);
      tmp += pPoints[nFirst] * B0(uPrime[i]);
      tmp = pPoints[nFirst+i] - tmp;
      //      tmp = V2SubII(d[first + i],
      //             V2AddII( V2ScaleIII(d[first], B0(uPrime[i])),
      //              V2AddII( V2ScaleIII(d[first], B1(uPrime[i])),
      //               V2AddII( V2ScaleIII(d[last], B2(uPrime[i])),
      //                V2ScaleIII(d[last], B3(uPrime[i]))
      //               )
      //              )
      //             )
      //            );

      X[0] += A[i][0].Dot(tmp);
      X[1] += A[i][1].Dot(tmp);
   }

   /* Compute the determinants of C and X	*/
   det_C0_C1 = C[0][0] * C[1][1] - C[1][0] * C[0][1];
   det_C0_X  = C[0][0] * X[1]    - C[0][1] * X[0];
   det_X_C1  = X[0]    * C[1][1] - X[1]    * C[0][1];

   /* Finally, derive alpha values	*/
   if (det_C0_C1 == 0.0)
      det_C0_C1 = (C[0][0] * C[1][1]) * 10e-12;
   alpha_l = det_X_C1 / det_C0_C1;
   alpha_r = det_C0_X / det_C0_C1;

   /*  If alpha negative, use the Wu/Barsky heuristic (see text) */
   if (alpha_l < 0.0 || alpha_r < 0.0)
   {
      double dist = (pPoints[nLast] - pPoints[nFirst]).Length() / 3.0;

      bezCurve[0] = pPoints[nFirst];
      bezCurve[3] = pPoints[nLast];
      bezCurve[1] = bezCurve[0] + (tHat1 * dist);
      bezCurve[2] = bezCurve[3] + (tHat2 * dist);
      return (bezCurve);
   }

   /*  First and last control points of the Bezier curve are */
   /*  positioned exactly at the first and last data points */
   /*  Control points 1 and 2 are positioned an alpha distance out */
   /*  on the tangent vectors, left and right, respectively */
   bezCurve[0] = pPoints[nFirst];
   bezCurve[3] = pPoints[nLast];
   bezCurve[1] = bezCurve[0] + (tHat1 * alpha_l);
   bezCurve[2] = bezCurve[3] + (tHat2 * alpha_r);

   return (bezCurve);
}

/*
*  ComputeMaxError :
*	Find the maximum squared distance of digitized points
*	to fitted curve.
*/
double FloatMotionCurve::ComputeCubicMaxError( Vector2 *pPoints, int nFirst, int nLast, BezierCurve bezCurve, double *u, int *splitPoint)
//    Point2	*d;			/*  Array of digitized points	*/
//    int		first, last;		/*  Indices defining region	*/
//    BezierCurve	bezCurve;		/*  Fitted Bezier curve		*/
//    double	*u;			/*  Parameterization of points	*/
//    int		*splitPoint;		/*  Point of maximum error	*/
{
   int		i;
   double	maxDist;		/*  Maximum error		*/
   double	dist;		/*  Current error		*/
   Vector2	P;			/*  Point on curve		*/
   Vector2	v;			/*  Vector from point to curve	*/

   *splitPoint = (nLast - nFirst + 1)/2;
   maxDist = 0.0;
   for (i = nFirst + 1; i < nLast; i++)
   {
      P = BezierII(3, bezCurve, u[i-nFirst]);
      v = P - pPoints[i];
      dist = v.LengthSq();
      if (dist >= maxDist) {
         maxDist = dist;
         *splitPoint = i;
      }
   }
   return (maxDist);
}

/*
*  Reparameterize:
*	Given set of points and their parameterization, try to find
*   a better parameterization.
*
*/
double *FloatMotionCurve::Reparameterize( Vector2 *pPoints, int nFirst, int nLast, double *u, BezierCurve bezCurve )
//    Point2	*d;			/*  Array of digitized points	*/
//    int		first, last;		/*  Indices defining region	*/
//    double	*u;			/*  Current parameter values	*/
//    BezierCurve	bezCurve;	/*  Current fitted curve	*/
{
   int 	nPts = nLast-nFirst+1;	
   int 	i;
   double	*uPrime;		/*  New parameter values	*/

   uPrime = (double *)malloc(nPts * sizeof(double));
   for (i = nFirst; i <= nLast; i++)
      uPrime[i-nFirst] = NewtonRaphsonRootFind(bezCurve, pPoints[i], u[i-nFirst]);
   return (uPrime);
}

/*
*  Bezier :
*  	Evaluate a Bezier curve at a particular parameter value
* 
*/
Vector2 FloatMotionCurve::BezierII( int degree, Vector2 *V, double t )
//    int		degree;		/* The degree of the bezier curve	*/
//    Point2 	*V;		/* Array of control points		*/
//    double 	t;		/* Parametric value to find point for	*/
{
   int 	i, j;		
   Vector2 	Q;	        /* Point on curve at parameter t	*/
   Vector2 	*Vtemp;		/* Local copy of control points		*/

   /* Copy array	*/
   Vtemp = (Vector2 *)malloc((unsigned)((degree+1) * sizeof (Vector2)));
   for (i = 0; i <= degree; i++) {
      Vtemp[i] = V[i];
   }

   /* Triangle computation	*/
   for (i = 1; i <= degree; i++) {	
      for (j = 0; j <= degree-i; j++) {
         Vtemp[j].x = (1.0 - t) * Vtemp[j].x + t * Vtemp[j+1].x;
         Vtemp[j].y = (1.0 - t) * Vtemp[j].y + t * Vtemp[j+1].y;
      }
   }

   Q = Vtemp[0];
   free((void *)Vtemp);
   return Q;
}

/*
*  NewtonRaphsonRootFind :
*	Use Newton-Raphson iteration to find better root.
*/
double FloatMotionCurve::NewtonRaphsonRootFind( BezierCurve Q, const Vector2 &P, double u )
//    BezierCurve	Q;			/*  Current fitted curve	*/
//    Point2 		P;		/*  Digitized point		*/
//    double 		u;		/*  Parameter value for "P"	*/
{
   double 		numerator, denominator;
   Vector2    Q1[3], Q2[2];	/*  Q' and Q''			*/
   Vector2    Q_u, Q1_u, Q2_u; /*u evaluated at Q, Q', & Q''	*/
   double 		uPrime;		/*  Improved u			*/
   int 		i;

   /* Compute Q(u)	*/
   Q_u = BezierII(3, Q, u);

   /* Generate control vertices for Q'	*/
   for (i = 0; i <= 2; i++) {
      Q1[i].x = (Q[i+1].x - Q[i].x) * 3.0;
      Q1[i].y = (Q[i+1].y - Q[i].y) * 3.0;
   }

   /* Generate control vertices for Q'' */
   for (i = 0; i <= 1; i++) {
      Q2[i].x = (Q1[i+1].x - Q1[i].x) * 2.0;
      Q2[i].y = (Q1[i+1].y - Q1[i].y) * 2.0;
   }

   /* Compute Q'(u) and Q''(u)	*/
   Q1_u = BezierII(2, Q1, u);
   Q2_u = BezierII(1, Q2, u);

   /* Compute f(u)/f'(u) */
   numerator = (Q_u.x - P.x) * (Q1_u.x) + (Q_u.y - P.y) * (Q1_u.y);
   denominator = (Q1_u.x) * (Q1_u.x) + (Q1_u.y) * (Q1_u.y) +
      (Q_u.x - P.x) * (Q2_u.x) + (Q_u.y - P.y) * (Q2_u.y);

   /* u = u - f(u)/f'(u) */
   uPrime = u - (numerator/denominator);
   return (uPrime);
}


// LINEAR KEY REDUCTION

void FloatMotionCurve::RebuildAsLinear( float fTolerance, FloatMotionCurve *pDstMotionCurve )
{
   fTolerance *= 0.2f;

   if (fTolerance <= 0.0f)
      fTolerance = 0.0f;

   pDstMotionCurve->RemoveAll();

   Time endTime = GetStopTime(FALSE);

   // sample at 300 fps when we have a valid error tolerance
   // sample at 30 fps if the error tolerance is zero

#if 0
   Time sampleInterval(1/300.0);
#else
   Time sampleInterval(1/30.0);
#endif

   if (fTolerance==0)
      sampleInterval = (1/30.0);

   int numSamples = (int)(endTime/sampleInterval)+1;
   // we need at least 2 samples
   if (numSamples<2)
      numSamples=2;
   endTime = sampleInterval*numSamples;
   CArray<Vector2> sampleArray;
   sampleArray.SetSize(numSamples);
   double maxValue = -1.0e20;
   double minValue = 1.0e20;
   double scaleFactor = 1.0;

   for (int sampleIndex=0;sampleIndex<numSamples;sampleIndex++)
   {
      Time sampleTime = sampleInterval * sampleIndex;
      double value = GetValue(sampleTime,FALSE);
      sampleArray[sampleIndex].x = sampleTime.GetSeconds();
      sampleArray[sampleIndex].y = (float)value;
      if (value>maxValue)
         maxValue = value;
      if (value<minValue)
         minValue = value;
   }

   double maxExtent = max(fabs(maxValue),fabs(minValue));
   // smallest it can be is 1
   if (maxExtent<1.0) 
      maxExtent = 1.0;

   double errorFactor = maxExtent * fTolerance;

   CArray<Vector2> newLinearPoints;
   FitLinearCurve(sampleArray,newLinearPoints,errorFactor);
   int numKeyFrames = newLinearPoints.GetSize();

   int keyFrameIndex;
   for (keyFrameIndex=0;keyFrameIndex<numKeyFrames;keyFrameIndex++)
   {
      FloatMotionKey *pMotionKey = pDstMotionCurve->StoreValue(newLinearPoints[keyFrameIndex].y,newLinearPoints[keyFrameIndex].x);
      pMotionKey->SetInterpolationMethod(MotionKey::LINEAR);
   }
}

// FitCurve fit a Bezier curve(s) to a set of digitized points 
void FloatMotionCurve::FitLinearCurve(const CArray<Vector2> &points, CArray<Vector2> &linearPointsFill, double error)
{
   linearPointsFill.Add(points[0]); // add leftmost point
   int totalPoints = points.GetSize();
   FitLinear(points, 0, totalPoints-1, linearPointsFill, error);
}

// FitCubic : Fit a Bezier curve to a (sub)set of digitized points
void FloatMotionCurve::FitLinear(const CArray<Vector2> &points, int first, int last, CArray<Vector2> &linearPointsFill, double error)
{
   double maxError;		// Maximum fitting error
   int splitPoint;			// Point to split point set at
   int totalPoints;		// Number of points in subset
   Vector2 tHatCenter;	// Unit tangent vector at splitPoint

   totalPoints = last - first + 1;

   // Use heuristic if region only has two points in it
   if (totalPoints == 2)
   {
      linearPointsFill.Add(points[last]);
      return;
   }

   // Find max deviation of points to fitted curve
   maxError = ComputeLinearMaxError(points, first, last, splitPoint);
   if (maxError <= error)
   {
      linearPointsFill.Add(points[last]);
      return;
   }

   // Fitting failed -- split at max error point and fit recursively
   FitLinear(points, first, splitPoint, linearPointsFill, error);
   FitLinear(points, splitPoint, last, linearPointsFill, error);
}

// ComputeMaxError :
// Find the maximum squared distance of digitized points
// to fitted curve.
double FloatMotionCurve::ComputeLinearMaxError(const CArray<Vector2> &points, int first, int last,int &splitPoint)
{
   int	i;
   double dist;
   double maxDist;		// Maximum error
   Vector2 point;	// Point on curve
   Vector2 vector;	// Vector from point to curve

   splitPoint = (last - first + 1)/2;
   maxDist = 0.0;
   for (i = first + 1; i < last; i++)
   {
      double percent = (points[i].x-points[first].x)/(points[last].x-points[first].x);
      point = points[first] + ((points[last]-points[first]) * (float)percent);
      vector = point-points[i];
      dist = vector.y*vector.y;
      if (dist > maxDist)
      {
         maxDist = dist;
         splitPoint = i;
      }
   }
   return ::sqrt(maxDist);
}

