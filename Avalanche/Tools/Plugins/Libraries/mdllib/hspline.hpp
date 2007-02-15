//////////////////////////////////////////////////////////////////////////////////////////
//
// INLINE HSPLINE METHODS
//
//////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////////////
//	HASH Spline Math: formula for solving for peaked default tangent.
//////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void HSpline::calcPeakedDefaultTangent(HVector p1, HVector p2, HVector p3, HVector &it, HVector &ot, HVector *gammaplane){
	ot = (p2-p1)* 0.5;
	it = (p3-p2)* 0.5;
	if ( gammaplane)
		*gammaplane = cross(it, ot);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//	HASH Spline Math: formula for solving for smooth default tangent.
//////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void HSpline::calcSmoothDefaultTangent(HVector p1, HVector p2, HVector p3, HVector &it, HVector &ot, HVector *gammaplane) {
	HVector direction;
	double itmagnitude, otmagnitude;

	direction = p3 - p1;
	itmagnitude = (p2-p3).length();
	p1 -= p2;
	otmagnitude = p1.length();
	
	double sum = itmagnitude + otmagnitude;

	if (sum == 0) {
		it = 0;
		ot = 0;//all points are coincident			
	}
	else {
		it = direction*(itmagnitude/sum);
		ot = direction*(otmagnitude/sum);
	}

	if (gammaplane)
		*gammaplane = cross( direction,  p1 );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//  HASH Spline Math: formula for multiplying bias to default tangent.
//////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void HSpline::calcHBias(HVector &tangent, HBias bias, HVector gammaplane){
	if (tangent.lengthsq() != 0) {
		HVector alphaplane = cross( gammaplane, tangent );
		HQuaternion quat = RotateVQ( bias.alpha, alphaplane ) * RotateVQ( -bias.gamma, gammaplane );
		tangent *= quat;
		tangent *= bias.magnitude;
   }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//  HASH Spline Math: formula for solving for smooth default end tangent.
//////////////////////////////////////////////////////////////////////////////////////////////////////////

inline HVector HSpline::calcSmoothDefaultEndTangent(HVector p2, HVector p3, HVector p4, HVector *gammaplane) {

	HVector delta = (p2 - p3).normalized();
	HVector L1 = (p3 - p2).length();
	HVector T1 = (p2 - p4).normalized() * L1;
	HVector along = dot( T1, delta ) * delta;
	HVector away = T1 - along;
	HVector T3 = along - away;
	HVector maxv = ((p2 + p4)*0.5 - p3).normalized();
	float mincos = (float)dot( maxv, delta );

	float T3len;
	if (T3len = (float)T3.length()) {
		HVector temp = T3;
		float newcos = (float)dot(T3/T3len, delta);
		if (newcos < mincos)
			T3 = maxv * T3len;
		if ( gammaplane )
			*gammaplane = cross(temp, delta);
	}
	return T3;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//  HASH Spline Math: formula for solving for peaked default end tangent.
//////////////////////////////////////////////////////////////////////////////////////////////////////////

inline HVector HSpline::calcPeakedDefaultEndTangent(HVector p2, HVector p3, HVector p4, HVector *gammaplane) {
	HVector T3 = (p2-p3) * 0.5;
	if (gammaplane)
		*gammaplane = cross( T3,  p2 );
	return T3;
}

inline double HSpline::bezier(double t, double p0, double p1, double p2, double p3) {
	double OneMinusT2 = (1-t) * (1-t);
	double T2 = t * t;
	double OneMinusT3 = OneMinusT2 * (1-t);
	double T3 =  T2 * t;
	return ((OneMinusT3 * p0) + (3 * t * OneMinusT2 * p1) + (3 * T2 * (1-t) * p2) + (T3 * p3));
}

inline double HSpline::derivative(double t, double p0, double p1, double p2, double p3) {
	double OneMinusT2 = (1-t) * (1-t);
	double T2 = t * t;
	return ((p1 - p0) * 3 * OneMinusT2) + ((p2 - p1) * 6 * t * (1-t)) + ((p3 - p2) * 3 * T2);
}
