
// fast curve evaluator (3rd degree only)
inline HashVector2 Bezier2::EvalCurve(HashVector2 *points,double t)
{
	HashVector2 result;
	HashVector2::Tween(result,points[BezierEnums::POINT1],points[BezierEnums::CONTROL1],points[BezierEnums::CONTROL2],points[BezierEnums::POINT2],t);
	return result;
}

// curve evaluator which works with degrees from 3 to 5
// also provides new sets of control points for cutting the curve up into smaller parts
inline HashVector2 Bezier2::EvalCurve(HashVector2 *points,double t,int degree,HashVector2 *left,HashVector2 *right)
{
	// 24 Mults & 12 adds
	int i, j;

	HashVector2 vtemp[BezierEnums::W_DEGREE+1][BezierEnums::W_DEGREE+1];
	double s = (((double)1)-t);

	// Copy control points
	for (j =0; j <= degree; j++)
	{
		vtemp[0][j] = points[j];
	}

	// Triangle computation
	for (i = 1; i <= degree; i++)
	{	
		for (j =0 ; j <= degree - i; j++)
		{
			HashVector2::Tween(vtemp[i][j],vtemp[i-1][j],vtemp[i-1][j+1],t);
		}
	}
	if (left)
	{
		for (j = 0; j <= degree; j++) 
		{
			left[j]  = vtemp[j][0];
		}
	}
	if (right)
	{
		for (j = 0; j <= degree; j++)
		{
			right[j] = vtemp[degree-j][j];
		}
	}
	return (vtemp[degree][0]);
}

// NearestPointOnCurve 	Compute the parameter value of the point on a Bezier
//	curve segment closest to some arbtitrary, user-input point.
//	Return the point on the curve at that parameter value and the parameter value
inline HashVector2 Bezier2::NearestPointOnCurve(HashVector2 *points,HashVector2 point,double *percent,double *distanceSq)
{
	HashVector2 w[BezierEnums::W_DEGREE+1];	// Ctl pts for 5th-degree equation
	double tCanidates[BezierEnums::W_DEGREE];	// Possible roots
	int nSolutions;				// Number of roots found
	double t;					// % value of closest pt
	double dist;
	HashVector2 bestPoint;

	// Convert problem to 5th-degree Bezier form
	ConvertToBezierForm(points,point,w);

	// Find all possible roots of 5th-degree equation
	nSolutions = FindRoots(w, BezierEnums::W_DEGREE, tCanidates, 0);

	// Compare distances of P to all candidates, and to t=0, and t=1
	HashVector2 delta,p;
	double newDist;
	int	i;

	// Check distance to beginning of curve, where t = 0
	delta = points[BezierEnums::POINT1]-point;
	dist = delta.LengthSq();
	t = 0.0f;
	bestPoint = points[BezierEnums::POINT1];

	// Find distances for candidate points
	for (i = 0; i < nSolutions; i++) 
	{
		p = EvalCurve(points,tCanidates[i]);

		delta = p-point;
		newDist = delta.LengthSq();
		if (newDist < dist)
		{
			dist = newDist;
			t = tCanidates[i];
			bestPoint = p;
		}
	}

	// Finally, look at distance to end point, where t = 1.0 
	delta = points[BezierEnums::POINT2]-point;
	newDist = delta.LengthSq();
	if (newDist < dist)
	{
		dist = newDist;
		t = 1.0f;
		bestPoint = points[BezierEnums::POINT2];
	}
	// if we need to return the %
	if (percent)
	{
		*percent = t;
	}
	// if we need to return the distance (return the squared distance)
	if (distanceSq)
	{
		*distanceSq = dist;
	}
	return bestPoint;
}


// ConvertToBezierForm
// Given a point and a Bezier curve, generate a 5th-degree
// Bezier-format equation whose solution finds the point on the
// curve nearest the user-defined point.
inline void Bezier2::ConvertToBezierForm(HashVector2 *points,HashVector2 point,HashVector2 *w)
{
	int i, j, k, m, n, ub, lb;	
	int row, column;		// Table indices
	HashVector2 c[BezierEnums::DEGREE+1];	// V(i)'s - P
	HashVector2 d[BezierEnums::DEGREE];	// V(i+1) - V(i)
	double cdTable[3][4];	// Dot product of c, d
	static double z[3][4] =	// Precomputed "z" for cubics	
	{ 
		{1.0f, 0.6f, 0.3f, 0.1f},
		{0.4f, 0.6f, 0.6f, 0.4f},
		{0.1f, 0.3f, 0.6f, 1.0f},
	};

	//Determine the c's -- these are vectors created by subtracting
	// point P from each of the control points
	for (i = 0; i <= BezierEnums::DEGREE; i++)
	{
		c[i] = points[i]-point;
	}

	// Determine the d's -- these are vectors created by subtracting
	// each control point from the next
	for (i = 0; i <= BezierEnums::DEGREE - 1; i++) 
	{ 
		d[i] = ((double)3.0)*(points[i+1]-points[i]);
	}

	// Create the c,d table -- this is a table of dot products of the 
	// c's and d's
	for (row = 0; row <= BezierEnums::DEGREE - 1; row++)
	{
		for (column = 0; column <= BezierEnums::DEGREE; column++)
		{
			cdTable[row][column] = d[row].Dot(c[column]);
		}
	}

	// Now, apply the z's to the dot products, on the skew diagonal
	// Also, set up the x-values, making these "points"
	for (i = 0; i <= BezierEnums::W_DEGREE; i++)
	{
		w[i].y = 0.0;
		w[i].x = ((double)(i)) / BezierEnums::W_DEGREE;
	}

	n = BezierEnums::DEGREE;
	m = BezierEnums::DEGREE-1;
	for (k = 0; k <= n + m; k++)
	{
		lb = GenMath::MAX(0, k - m);
		ub = GenMath::MIN(k, n);
		for (i = lb; i <= ub; i++)
		{
			j = k - i;
			w[i+j].y += cdTable[j][i] * z[j][i];
		}
	}
}

// FindRoots
// Given a nth-degree equation in Bernstein-Bezier form, find
// all of the roots in the interval [0, 1].  Return the number
// of roots found.
// degree can be up to 5
inline int Bezier2::FindRoots(HashVector2 *w,int degree,double *t,int depth)
{  
	int i;
	HashVector2 left[BezierEnums::W_DEGREE+1];		// New left and right
	HashVector2 right[BezierEnums::W_DEGREE+1];	// control polygons
	int	leftCount;					// Solution count from
	int rightCount;					// children
	double leftt[BezierEnums::W_DEGREE+1];		// Solutions from kids
	double rightt[BezierEnums::W_DEGREE+1];

	switch (CrossingCount(w, degree))
	{
	case 0: 
		return 0;	// No solutions here
	case 1: 
		// Unique solution
		// Stop recursion when the tree is deep enough
		// if deep enough, return 1 solution at midpoint
		if (depth >= BezierEnums::MAXDEPTH) 
		{
			t[0] = (w[0].x + w[degree].x) / 2.0f;
			return 1;
		}
		if (ControlPolygonFlatEnough(w, degree))
		{
			t[0] = ComputeXIntercept(w, degree);
			return 1;
		}
		break;
	}
	// Otherwise, solve recursively after
	// subdividing control polygon
	EvalCurve(w, 0.5f,degree, left, right);
	leftCount  = FindRoots(left,  degree, leftt, depth+1);
	rightCount = FindRoots(right, degree, rightt, depth+1);

	// Gather solutions together
	for (i = 0; i < leftCount; i++)
	{
		t[i] = leftt[i];
	}
	for (i = 0; i < rightCount; i++) {
		t[i+leftCount] = rightt[i];
	}

	// Send back total number of solutions
	return (leftCount+rightCount);
}



// CrossingCount
// Count the number of times a Bezier control polygon
// crosses the 0-axis. This number is <= the number of roots.
inline int Bezier2::CrossingCount(HashVector2 *points,int degree)
{
	int i;	
	int nCrossings = 0;		// Number of zero-crossings
	int	sign, oldSign;		// Sign of coefficients

	sign = oldSign = GenMath::SGN(points[0].y);

	for (i = 1; i <= degree; i++)
	{
		sign = GenMath::SGN(points[i].y);
		if (sign != oldSign)
			nCrossings++;
		oldSign = sign;
	}
	return nCrossings;
}



// ControlPolygonFlatEnough
// Check if the control polygon of a Bezier curve is flat enough
// for recursive subdivision to bottom out.
inline int Bezier2::ControlPolygonFlatEnough(HashVector2 *points,int degree)
{
	int i;						// Index variable
	double distance[BezierEnums::W_DEGREE+1];	// Distances from pts to line
	double maxDistanceAbove;		// maximum of these	
	double maxDistanceBelow;
	double error;				// Precision of root
	double intercept1;
	double intercept2;
	double leftIntercept;
	double rightIntercept;
	double a,b,c;				// Coefficients of implicit eqn for line from V[0]-V[deg]
	double abSquared;

	// Find the  perpendicular distance
	// from each interior control point to 
	// line connecting V[0] and V[degree]

	// Derive the implicit equation for line connecting first
	// and last control points 
	a = points[0].y - points[degree].y;
	b = points[degree].x - points[0].x;
	c = points[0].x * points[degree].y - points[degree].x * points[0].y;

	abSquared = (a * a) + (b * b);

	for (i = 1; i < degree; i++)
	{
		// Compute distance from each of the points to that line
		distance[i] = a * points[i].x + b * points[i].y + c;
		if (distance[i] > 0.0)
		{
			distance[i] = (distance[i] * distance[i]) / abSquared;
		}
		if (distance[i] < 0.0)
		{
			distance[i] = -((distance[i] * distance[i]) / abSquared);
		}
	}

	// Find the largest distance
	maxDistanceAbove = 0.0;
	maxDistanceBelow = 0.0;
	for (i = 1; i < degree; i++)
	{
		if (distance[i] < 0.0)
		{
			maxDistanceBelow = GenMath::MIN(maxDistanceBelow, distance[i]);
		}
		if (distance[i] > 0.0)
		{
			maxDistanceAbove = GenMath::MAX(maxDistanceAbove, distance[i]);
		}
	}

	double det, dInv;
	double a1, b1, c1, a2, b2, c2;

	// Implicit equation for zero line
	a1 = 0.0;
	b1 = 1.0;
	c1 = 0.0;

	// Implicit equation for "above" line
	a2 = a;
	b2 = b;
	c2 = c + maxDistanceAbove;

	det = a1 * b2 - a2 * b1;
	dInv = 1.0f/det;

	intercept1 = (b1 * c2 - b2 * c1) * dInv;

	// Implicit equation for "below" line
	a2 = a;
	b2 = b;
	c2 = c + maxDistanceBelow;

	det = a1 * b2 - a2 * b1;
	dInv = 1.0f/det;

	intercept2 = (b1 * c2 - b2 * c1) * dInv;

	/* Compute intercepts of bounding box	*/
	leftIntercept = GenMath::MIN(intercept1, intercept2);
	rightIntercept = GenMath::MAX(intercept1, intercept2);

	error = 0.5f * (rightIntercept-leftIntercept);    
	if (error < BEZIER_EPSILON)
		return 1;
	else
		return 0;
}



// ComputeXIntercept
// Compute intersection of chord from first control point to last with 0-axis.
// NOTE: "double" and "Y" do not have to be computed, and there are many useless
// operations in the following (e.g. "0.0 - 0.0").
inline double Bezier2::ComputeXIntercept(HashVector2 *points,int degree)
{
	double deltax = points[degree].x - points[0].x;
	double deltay = points[degree].y - points[0].y;
	double basex = points[0].x;
	double basey = points[0].y;

	double slope = 1.0f/(-deltay);

	double S = ((deltax*basey - deltay*basex) * slope);
	return S;
}




// FitCurve fit a Bezier curve(s) to a set of digitized points 
inline void Bezier2::FitCurve(const TupArray<HashVector2> &points, TupArray<HashVector2> &curvePointsFill, double error)
{
	HashVector2 tHat1, tHat2;	// Unit tangent vectors at endpoints
	int totalPoints = points.GetSize();
	tHat1 = ComputeLeftTangent(points, 0);
	tHat2 = ComputeRightTangent(points, totalPoints-1);
	FitCubic(points, 0, totalPoints-1, tHat1, tHat2, curvePointsFill, error);
}



// FitCubic : Fit a Bezier curve to a (sub)set of digitized points
inline void Bezier2::FitCubic(const TupArray<HashVector2> &points, int first, int last,const HashVector2 &tHat1,const HashVector2 &tHat2, TupArray<HashVector2> &curvePointsFill, double error)
{
	HashVector2 bezCurve[4];	// Control points of fitted Bezier curve
	double *u;				// Parameter values for point
	double maxError;		// Maximum fitting error
	int splitPoint;			// Point to split point set at
	int totalPoints;		// Number of points in subset
	double iterationError;	// Error below which you try iterating
	int maxIterations = 4;	// Max times to try iterating
	HashVector2 tHatCenter;	// Unit tangent vector at splitPoint
	int	i;		

	iterationError = error * error;
	totalPoints = last - first + 1;

	// Use heuristic if region only has two points in it
	if (totalPoints == 2)
	{
		double dist = points[last].DistanceTo(points[first])/3.0f;
		curvePointsFill.Add(points[first]);
		curvePointsFill.Add(points[first] + (tHat1 * dist));
		curvePointsFill.Add(points[last] + (tHat2 * dist));
		curvePointsFill.Add(points[last]);
		return;
	}

	//  Parameterize points, and attempt to fit curve
	u = new double[totalPoints];

	ChordLengthParameterize(points, first, last, u);
	GenerateBezier(points, first, last, u, tHat1, tHat2, bezCurve);

	// Find max deviation of points to fitted curve
	maxError = ComputeMaxError(points, first, last, bezCurve, u, splitPoint);
#ifdef BUILD_THIRDS
	maxError = sqrt(maxError);
#endif
	if (maxError < error)
	{
		// add curve segment
		for (i=0;i<4;i++)
			curvePointsFill.Add(bezCurve[i]);
		delete u;
		return;
	}


#ifndef BUILD_THIRDS
	double *uPrime;			// Improved parameter values
	// If error not too large, try some reparameterization and iteration
	if (maxError < iterationError)
	{
		for (i = 0; i < maxIterations; i++) 
		{
			uPrime = new double[totalPoints];
			Reparameterize(points, first, last, u, bezCurve, uPrime);
			GenerateBezier(points, first, last, uPrime, tHat1, tHat2, bezCurve);
			maxError = ComputeMaxError(points, first, last, bezCurve, uPrime, splitPoint);
			if (maxError < error)
			{
				// add curve segment
				for (i=0;i<4;i++)
					curvePointsFill.Add(bezCurve[i]);
				delete u;
				delete uPrime;
				return;
			}
			delete u;
			u = uPrime;
		}
	}
#endif

	// Fitting failed -- split at max error point and fit recursively
	delete u;
	tHatCenter = ComputeCenterTangent(points, splitPoint);
	FitCubic(points, first, splitPoint, tHat1, tHatCenter, curvePointsFill, error);
	tHatCenter = -tHatCenter;
	FitCubic(points, splitPoint, last, tHatCenter, tHat2, curvePointsFill, error);
}


// GenerateBezier :
// Use least-squares method to find Bezier control points for region.
inline void Bezier2::GenerateBezier(const TupArray<HashVector2> &points, int first, int last, double *uPrime,const HashVector2 &tHat1,const HashVector2 &tHat2,HashVector2 *bezCurve)
{
#ifdef BUILD_THIRDS
	double	dist = points[last].DistanceTo(points[first]) / 3.0f;
	// set up curve and return
	bezCurve[0] = points[first];
	bezCurve[1] = points[first] + (tHat1 * dist);
	bezCurve[2] = points[last] + (tHat2 * dist);
	bezCurve[3] = points[last];
#else
	int i;
	HashVector2 A[BezierEnums::MAXPOINTS][2];	// Precomputed rhs for eqn
	int totalPoints;			// Number of pts in sub-curve
	double C[2][2];				// Matrix C
	double X[2];				// Matrix X
	double det_C0_C1,			// Determinants of matrices
		det_C0_X,
		det_X_C1;
	double alpha_l,				// Alpha values, left and right
		alpha_r;
	HashVector2 tmp;				// Utility variable

	totalPoints = last - first + 1;

	// Compute the A's
	for (i = 0; i < totalPoints; i++)
	{
		A[i][0] = tHat1*B1(uPrime[i]);
		A[i][1] = tHat2*B2(uPrime[i]);
	}

	// Create the C and X matrices
	C[0][0] = 0.0;
	C[0][1] = 0.0;
	C[1][0] = 0.0;
	C[1][1] = 0.0;
	X[0]    = 0.0;
	X[1]    = 0.0;

	for (i = 0; i < totalPoints; i++)
	{
		C[0][0] += A[i][0].Dot(A[i][0]);
		C[0][1] += A[i][0].Dot(A[i][1]);
		C[1][0] = C[0][1];
		C[1][1] += A[i][1].Dot(A[i][1]);

		tmp = points[first + i]-
			((points[first]*B0(uPrime[i]))+
			(points[first]*B1(uPrime[i]))+
			(points[last]*B2(uPrime[i]))+
			(points[last]*B3(uPrime[i])));

		X[0] += A[i][0].Dot(tmp);
		X[1] += A[i][1].Dot(tmp);
	}

	// Compute the determinants of C and X
	det_C0_C1 = C[0][0] * C[1][1] - C[1][0] * C[0][1];
	det_C0_X  = C[0][0] * X[1]    - C[0][1] * X[0];
	det_X_C1  = X[0]    * C[1][1] - X[1]    * C[0][1];

	// Finally, derive alpha values
	if (det_C0_C1 == 0.0)
	{
		det_C0_C1 = (C[0][0] * C[1][1]) * 10e-12f;
	}
	alpha_l = det_X_C1 / det_C0_C1;
	alpha_r = det_C0_X / det_C0_C1;


	// If alpha negative, use the Wu/Barsky heuristic (see text)
	// (if alpha is 0, you get coincident control points that lead to
	// divide by zero in any subsequent NewtonRaphsonRootFind() call.
	if (alpha_l < 1.0e-6f || alpha_r < 1.0e-6f)
	{
		double	dist = points[last].DistanceTo(points[first]) / 3.0f;
		// set up curve and return
		bezCurve[0] = points[first];
		bezCurve[1] = points[first] + (tHat1 * dist);
		bezCurve[2] = points[last] + (tHat2 * dist);
		bezCurve[3] = points[last];
		return;
	}

	// First and last control points of the Bezier curve are
	// positioned exactly at the first and last data points
	// Control points 1 and 2 are positioned an alpha distance out
	// on the tangent vectors, left and right, respectively
	bezCurve[0] = points[first];
	bezCurve[1] = points[first] + (tHat1 * alpha_l);
	bezCurve[2] = points[last] + (tHat2 * alpha_r);
	bezCurve[3] = points[last];
	return;
#endif
}


// Reparameterize:
// Given set of points and their parameterization, try to find
// a better parameterization.
inline void Bezier2::Reparameterize(const TupArray<HashVector2> &points, int first, int last, double *u,HashVector2 *bezCurve, double *uPrime)
{
	int	totalPoints = last-first+1;	
	int i;

	for (i = first; i <= last; i++) 
	{
		uPrime[i-first] = NewtonRaphsonRootFind(bezCurve, points[i], u[i-first]);
	}
}

// NewtonRaphsonRootFind : Use Newton-Raphson iteration to find better root.
inline double Bezier2::NewtonRaphsonRootFind(HashVector2 *bezCurve,const HashVector2 &point,double u)
{
	double numerator, denominator;
	HashVector2 Q1[3], Q2[2];			// Q' and Q''
	HashVector2 Q_u, Q1_u, Q2_u;		//u evaluated at Q, Q', & Q''
	double uPrime;					// Improved u
	int i;

	// Compute Q(u)
	Q_u = EvalCurve(bezCurve,u);

	// Generate control vertices for Q'
	for (i = 0; i <= 2; i++)
	{
		Q1[i] = (bezCurve[i+1]-bezCurve[i])*(double)3.0;
	}

	// Generate control vertices for Q'' */
	for (i = 0; i <= 1; i++)
	{
		Q2[i] = (Q1[i+1]-Q1[i])*(double)2.0;
	}

	// Compute Q'(u) and Q''(u)
	Q1_u = EvalCurve(Q1,u,2);
	Q2_u = EvalCurve(Q2,u,1);

	// Compute f(u)/f'(u)
	numerator = ((Q_u - point) * Q1_u).Sum();
	denominator = ((Q1_u * Q1_u)+((Q_u-point)*Q2_u)).Sum();

	// u = u - f(u)/f'(u)
	uPrime = u - (numerator/denominator);
	return uPrime;
}

// B0, B1, B2, B3 : Bezier multipliers
inline double Bezier2::B0(double u)
{
	double tmp = 1.0f - u;
	return (tmp * tmp * tmp);
}

inline double Bezier2::B1(double u)
{
	double tmp = 1.0f - u;
	return (3 * u * (tmp * tmp));
}

inline double Bezier2::B2(double u)
{
	double tmp = 1.0f - u;
	return (3 * u * u * tmp);
}

inline double Bezier2::B3(double u)
{
	return (u * u * u);
}

// ComputeLeftTangent, ComputeRightTangent, ComputeCenterTangent :
// Approximate unit tangents at endpoints and "center" of digitized curve
inline HashVector2 Bezier2::ComputeLeftTangent(const TupArray<HashVector2> &points, int index)
{
	HashVector2 tHat1;
	tHat1 = points[index+1]-points[index];
	tHat1.Normalize();
	return tHat1;
}

inline HashVector2 Bezier2::ComputeRightTangent(const TupArray<HashVector2> &points, int index)
{
	HashVector2	tHat2;
	tHat2 = points[index-1]-points[index];
	tHat2.Normalize();
	return tHat2;
}

inline HashVector2 Bezier2::ComputeCenterTangent(const TupArray<HashVector2> &points, int center)
{
	HashVector2 V1, V2, tHatCenter;

	V1 = points[center-1]-points[center];
	V2 = points[center]-points[center+1];
	tHatCenter = (V1+V2)/((double)2);
	tHatCenter.Normalize();
	return tHatCenter;
}


// ChordLengthParameterize :
// Assign parameter values to digitized points 
// using relative distances between points.
inline void Bezier2::ChordLengthParameterize(const TupArray<HashVector2> &points, int first, int last, double *u)
{
	int	i;	

#ifdef BUILD_THIRDS
	u[0] = 0.0;
	for (i = first+1; i <= last; i++)
	{
		u[i-first] = u[i-first-1] + ::fabs(points[i].x - points[i-1].x);
	}

	for (i = first+1; i <= last; i++)
	{
		u[i-first] = u[i-first] / u[last-first];
	}
#else
	u[0] = 0.0;
	for (i = first+1; i <= last; i++)
	{
		u[i-first] = u[i-first-1] + points[i].DistanceTo(points[i-1]);
	}

	for (i = first+1; i <= last; i++)
	{
		u[i-first] = u[i-first] / u[last-first];
	}
#endif
}

// ComputeMaxError :
// Find the maximum squared distance of digitized points
// to fitted curve.
inline double Bezier2::ComputeMaxError(const TupArray<HashVector2> &points, int first, int last, HashVector2 *bezCurve,double *u,int &splitPoint)
{
	int	i;
	double maxDist;		// Maximum error
	double dist;		// Current error
	HashVector2 point;	// Point on curve
	HashVector2 vector;	// Vector from point to curve

	splitPoint = (last - first + 1)/2;
	maxDist = 0.0;
	for (i = first + 1; i < last; i++)
	{
		point = EvalCurve(bezCurve,u[i-first]);
		vector = point-points[i];
		dist = vector.LengthSq();
		if (dist >= maxDist)
		{
			maxDist = dist;
			splitPoint = i;
		}
	}
	return maxDist;
}

inline void Bezier2::CurveToPoints(HashVector2 *points,TupArray<HashVector2> &pointsFill,double threshold,bool addEnd)
{
	// determine location of control points if this curve were to be converted to a straight line
	HashVector2 targetC1,targetC2;;
	HashVector2::Tween(targetC1,points[BezierEnums::POINT1],points[BezierEnums::POINT2],((double)1.0)/((double)3.0));
	HashVector2::Tween(targetC2,points[BezierEnums::POINT2],points[BezierEnums::POINT1],((double)1.0)/((double)3.0));

	double distanceC1(DistanceSq(points[BezierEnums::CONTROL1],targetC1));
	double distanceC2(DistanceSq(points[BezierEnums::CONTROL2],targetC2));
	if (distanceC1+distanceC2<=threshold)
	{
		pointsFill.Add(points[BezierEnums::POINT1]);
		if (addEnd)
		{
			pointsFill.Add(points[BezierEnums::POINT2]);
		}
	}
	else
	{
		HashVector2 *leftCurve = new HashVector2[4];
		HashVector2 *rightCurve = new HashVector2[4];
		EvalCurve(points,0.5,3,leftCurve,rightCurve);
		CurveToPoints(leftCurve,pointsFill,threshold,false);
		CurveToPoints(rightCurve,pointsFill,threshold,addEnd);
		delete leftCurve;
		delete rightCurve;
	}
}

inline void Bezier2::CurveToPointsPercents(HashVector2 *points,TupArray<HashVector2> &pointsFill,TupArray<double> &percentFill,double threshold,bool addEnd,double min,double max)
{
	// determine location of control points if this curve were to be converted to a straight line
	HashVector2 targetC1,targetC2;;
	HashVector2::Tween(targetC1,points[BezierEnums::POINT1],points[BezierEnums::POINT2],((double)1.0)/((double)3.0));
	HashVector2::Tween(targetC2,points[BezierEnums::POINT2],points[BezierEnums::POINT1],((double)1.0)/((double)3.0));

	double distanceC1(DistanceSq(points[BezierEnums::CONTROL1],targetC1));
	double distanceC2(DistanceSq(points[BezierEnums::CONTROL2],targetC2));
	if (distanceC1+distanceC2<=threshold)
	{
		pointsFill.Add(points[BezierEnums::POINT1]);
		percentFill.Add(min);
		if (addEnd)
		{
			pointsFill.Add(points[BezierEnums::POINT2]);
			percentFill.Add(max);
		}
	}
	else
	{

		HashVector2 *leftCurve = new HashVector2[4];
		HashVector2 *rightCurve = new HashVector2[4];
		EvalCurve(points,0.5,3,leftCurve,rightCurve);
		double midpercent = min+((max-min)*0.5f);
		CurveToPointsPercents(leftCurve,pointsFill,percentFill,threshold,false,min,midpercent);
		CurveToPointsPercents(rightCurve,pointsFill,percentFill,threshold,addEnd,midpercent,max);
		delete leftCurve;
		delete rightCurve;
	}
}

inline int Bezier2::GetCrossingPercents(HashVector2 *points,TupArray<double> &percentFill,double y)
{
	int i;
	HashVector2 newPoints[4];
	// adjust for y
	for (i=0;i<4;i++)
	{
		// y equation is the curve
		newPoints[i].y = points[i].y - y;
		// x equation is a line from 0-1 (used to determine % of line)
		newPoints[i].x = i/3.0f; 
	}
	double tCanidates[BezierEnums::DEGREE];	// Possible roots
	int nSolutions;				// Number of roots found

	// Find all possible roots of 3th-degree equation
	nSolutions = FindRoots(points, BezierEnums::DEGREE, tCanidates, 0);
	for (i=0;i<nSolutions;i++)
	{
		percentFill.Add(tCanidates[i]);
	}
	return nSolutions;
}

inline int Bezier2::GetCrossingPoints(HashVector2 *points,TupArray<HashVector2> &pointsFill,double y)
{
	int i;
	HashVector2 newPoints[4];
	// adjust for y
	for (i=0;i<4;i++)
	{
		// y equation is the curve
		newPoints[i].y = points[i].y - y;
		// x equation is a line from 0-1 (used to determine % of line)
		newPoints[i].x = i/3.0f; 
	}
	double tCanidates[BezierEnums::DEGREE];	// Possible roots
	int nSolutions;				// Number of roots found

	// Find all possible roots of 3th-degree equation
	nSolutions = FindRoots(newPoints, BezierEnums::DEGREE, tCanidates, 0);
	for (i=0;i<nSolutions;i++)
	{
		HashVector2 newVertex = EvalCurve(points,tCanidates[i]);
		pointsFill.Add(newVertex);
	}
	return nSolutions;
}

// CombineCurves
// convert 7 points (shared point in the middle) to a single curve
inline void Bezier2::CombineCurves(HashVector2 *curves,HashVector2 *curveFill)
{
	int i;
	const int curveSteps = 8;
	const int numCurvePoints = (curveSteps*2)+1;
	// generate an array of points representing the curve
	TupArray<HashVector2> points;
	for (i=0;i<=curveSteps;i++)
	{
		points.Add(EvalCurve(curves,((double)i)/curveSteps));
	}
	for (i=1;i<=curveSteps;i++)
	{
		points.Add(EvalCurve(&curves[3],((double)i)/curveSteps));
	}
	HashVector2 leftTangent,rightTangent;
	leftTangent = curves[1]-curves[0]; // first two points
	leftTangent.Normalize();
	rightTangent = curves[5]-curves[6]; // last two points
	rightTangent.Normalize();

	double u[numCurvePoints];
	ChordLengthParameterize(points, 0, numCurvePoints-1, u);
	GenerateBezier(points, 0, numCurvePoints-1, u,leftTangent,rightTangent,curveFill);
}

// AdjustCurve
// move control points on curve to keep value at t lined up with new delta
inline void Bezier2::AdjustCurve(HashVector2 *curve,const HashVector2 &delta,double t)
{
	double factor = 3.0f*t*(1.0f-t);
	if (factor>1.0e-6f)
	{
		HashVector2 newDelta(delta/factor);
		curve[BezierEnums::CONTROL1] += newDelta;
		curve[BezierEnums::CONTROL2] += newDelta;
	}
}

inline double Bezier2::DistanceSq(const HashVector2 &point1,const HashVector2 &point2)
{
	return (point1-point2).LengthSq();
}

