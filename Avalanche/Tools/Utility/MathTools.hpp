inline float MathTools::Bezier(float t, float p0, float p1, float p2, float p3)
{
	float OneMinusT2 = (1-t) * (1-t);
	float T2 = t * t;
	float OneMinusT3 = OneMinusT2 * (1-t);
	float T3 =  T2 * t;
	return ((OneMinusT3 * p0) + (3.0f * t * OneMinusT2 * p1) + (3.0f * T2 * (1-t) * p2) + (T3 * p3));
}

inline Vector MathTools::Bezier(float t,const Vector &p0,const Vector &p1,const Vector &p2,const Vector &p3)
{
	Vector result;
	result.x = Bezier(t,p0.x,p1.x,p2.x,p3.x);
	result.y = Bezier(t,p0.y,p1.y,p2.y,p3.y);
	result.z = Bezier(t,p0.z,p1.z,p2.z,p3.z);
	return result;
}

inline Vector2 MathTools::Bezier(float t,const Vector2 &p0,const Vector2 &p1,const Vector2 &p2,const Vector2 &p3)
{
   Vector2 result;
   result.x = Bezier(t,p0.x,p1.x,p2.x,p3.x);
   result.y = Bezier(t,p0.y,p1.y,p2.y,p3.y);
   return result;
}

inline Vector4 MathTools::Bezier(float t,const Vector4 &p0,const Vector4 &p1,const Vector4 &p2,const Vector4 &p3)
{
   Vector4 result;
   result.x = Bezier(t,p0.x,p1.x,p2.x,p3.x);
   result.y = Bezier(t,p0.y,p1.y,p2.y,p3.y);
   result.z = Bezier(t,p0.z,p1.z,p2.z,p3.z);
   result.w = Bezier(t,p0.w,p1.w,p2.w,p3.w);
   return result;
}

inline float MathTools::Derivative(float t, float p0, float p1, float p2, float p3)
{
	float OneMinusT2 = (1-t) * (1-t);
	float T2 = t * t;
	return ((p1 - p0) * 3.0f * OneMinusT2) + ((p2 - p1) * 6.0f * t * (1-t)) + ((p3 - p2) * 3.0f * T2);
}

inline Vector MathTools::Derivative(float t,Vector p0,Vector p1,Vector p2,Vector p3)
{
	Vector result;
	result.x = Derivative(t,p0.x,p1.x,p2.x,p3.x);
	result.y = Derivative(t,p0.y,p1.y,p2.y,p3.y);
	result.z = Derivative(t,p0.z,p1.z,p2.z,p3.z);
	return result;
}

inline float MathTools::SecondDerivative(float t, float p0, float p1, float p2, float p3)
{
	return 6.0f * (p0 - (2.0f * p1) + p2) * (1-t) + 6.0f * (p1 - (2.0f * p2) + p3) * t;
}

inline Vector MathTools::SecondDerivative(float t, Vector p0,Vector p1,Vector p2,Vector p3)
{
	Vector result;
	result.x = SecondDerivative(t,p0.x,p1.x,p2.x,p3.x);
	result.y = SecondDerivative(t,p0.y,p1.y,p2.y,p3.y);
	result.z = SecondDerivative(t,p0.z,p1.z,p2.z,p3.z);
	return result;
}

// Requires control tangents to be positions not vectors.
// Generates positions not vectors.
inline void MathTools::SplitBezierCurve(float t,const Vector &c0,const Vector &c1,const Vector &c2,const Vector &c3,
								   Vector &L0,Vector &L1,Vector &L2,Vector &L3,
								   Vector &R0,Vector &R1,Vector &R2,Vector &R3)
{
	L0 = c0;
	R3 = c3;
	L1 = c0 + t * (c1-c0);
	R2 = c2 + t * (c3-c2);
	Vector T = c1 + t * (c2-c1);
	L2 = L1 + t * (T-L1);
	R1 = T + t * (R2-T);
	L3 = L2 + t * (R1-L2);
	R0 = L3;
}

inline void MathTools::ComputeTangents( const Vector *pV0, const Vector &pV1, const Vector &pV2, const Vector *pV3, Vector *pOutTangent1, Vector *pOutTangent2, float fV1Alpha/*=0.0f*/, float fV1Gamma/*=0.0f*/, float fV1Magnitude/*=1.0f*/, float fV2Alpha/*=0.0f*/, float fV2Gamma/*=0.0f*/, float fV2Magnitude/*=1.0f*/ )
{
   Vector vPrevTangent;
   float fTangentLength;

   if (pV0)
   {
      vPrevTangent = pV2 - *pV0; 
      float fPrevLength = (*pV0 - pV1).Length();
      float fNextLength = (pV2 - pV1).Length();
      float fRatio = fNextLength / (fPrevLength + fNextLength);
      fTangentLength = vPrevTangent.Length() * fRatio;
   }
   else
   {
      vPrevTangent = pV2 - pV1;
      fTangentLength = vPrevTangent.Length() * 0.5f;
   }

   vPrevTangent.Normalize();

   Vector vGammaPlane;
   vGammaPlane = Cross( vPrevTangent, (pV2 - pV1).Normalized() );
   Vector vAlphaPlane = Cross( vPrevTangent, vGammaPlane );
   Quaternion quat = RotateVQ( fV1Alpha * DEGTORAD, vAlphaPlane ) * RotateVQ( -fV1Gamma * DEGTORAD, vGammaPlane );
   vPrevTangent *= quat;
   vPrevTangent *= fTangentLength;
   vPrevTangent *= fV1Magnitude;
   *pOutTangent1 = vPrevTangent;

   Vector vNextTangent;
   if (pV3)
   {
      vNextTangent = pV1 - *pV3;
      float fPrevLength = (*pV3 - pV2).Length();
      float fNextLength = (pV1 - pV2).Length();
      float fRatio = fNextLength / (fPrevLength + fNextLength);
      fTangentLength = vNextTangent.Length() * fRatio;
   }
   else
   {
      vNextTangent = pV1 - pV2;
      fTangentLength = vNextTangent.Length() * 0.5f;
   }

   vNextTangent.Normalize();

   vGammaPlane = Cross( vNextTangent ,(pV1 - pV2).Normalized() );
   vAlphaPlane = Cross( vNextTangent, vGammaPlane );
   quat = RotateVQ( fV2Alpha * DEGTORAD, vAlphaPlane ) * RotateVQ( -fV2Gamma * DEGTORAD, vGammaPlane );
   vNextTangent *= quat;
   vNextTangent *= fTangentLength;
   vNextTangent *= fV2Magnitude;
   *pOutTangent2 = vNextTangent;
}

inline void MathTools::ComputeTangents( const Vector2 *pV0, const Vector2 &pV1, const Vector2 &pV2, const Vector2 *pV3, Vector2 *pOutTangent1, Vector2 *pOutTangent2, float fV1Alpha/*=0.0f*/, float fV1Magnitude/*=1.0f*/, float fV2Alpha/*=0.0f*/, float fV2Magnitude/*=1.0f*/ )
{
   Vector2 vPrevTangent;
   float fTangentLength;

   if (pV0)
   {
      vPrevTangent = pV2 - *pV0; 
      float fPrevLength = (*pV0 - pV1).Length();
      float fNextLength = (pV2 - pV1).Length();
      float fRatio = fNextLength / (fPrevLength + fNextLength);
      fTangentLength = vPrevTangent.Length() * fRatio;
   }
   else
   {
      vPrevTangent = pV2 - pV1;
      fTangentLength = vPrevTangent.Length() * 0.5f;
   }

   vPrevTangent.Normalize();
   vPrevTangent *= fTangentLength;

   Vector2 vNextTangent;
   if (pV3)
   {
      vNextTangent = pV1 - *pV3;
      float fPrevLength = (*pV3 - pV2).Length();
      float fNextLength = (pV1 - pV2).Length();
      float fRatio = fNextLength / (fPrevLength + fNextLength);
      fTangentLength = vNextTangent.Length() * fRatio;
   }
   else
   {
      vNextTangent = pV1 - pV2;
      fTangentLength = vNextTangent.Length() * 0.5f;
   }

   vNextTangent.Normalize();
   vNextTangent *= fTangentLength;

   *pOutTangent1 = vPrevTangent;
   *pOutTangent2 = vNextTangent;
}

// Requires control tangents to be positions not vectors.
inline bool MathTools::IsBezierSegmentStraight(float tolerance,const Vector &p0,const Vector &p1,const Vector &p2,const Vector &p3)
{
	if (p0 == p1)
		return true;
	
	Vector a = p1 - p0;
	Vector b = p3 - p0;

	a.Normalize();
	b.Normalize();

	float angle = a.Dot(b);

	if (angle > 1.0-tolerance)
	{
		Vector c = p3 - p2;
		c.Normalize();
		angle = b.Dot(c);
		if (angle > 1.0f-tolerance)
			return 1;
	}
	return false;
}

// calculates the number of subdivisions based on straightness tests and max subdivision
inline int MathTools::CalcBezierCurveSubdivisions(float tolerance,const Vector &p0,const Vector &p1,const Vector &p2,const Vector &p3,int maxsubd)
{
	if (p0 == p3)
		return 0;
	return DoCalcBezierCurveSubdivisions(tolerance,p0,p1,p2,p3,0,maxsubd);
}

// recurse for the above
inline int MathTools::DoCalcBezierCurveSubdivisions(float tolerance,const Vector &p0,const Vector &p1,const Vector &p2,const Vector &p3,int level,int maxsubd)
{
	if (!IsBezierSegmentStraight(tolerance,p0,p1,p2,p3) && (level < maxsubd))
	{
		Vector a0,a1,a2,a3;
		Vector b0,b1,b2,b3;

		SplitBezierCurve(0.5,p0,p1,p2,p3,a0,a1,a2,a3,b0,b1,b2,b3);

		return DoCalcBezierCurveSubdivisions(tolerance,a0,a1,a2,a3,level+1,maxsubd);

//		Only need to test one side because of symmetry.
//		int level = DoCalcBezierCurveSubdivisions(tolerance,b0,b1,b2,b3,level+1);
	}	
	return level;
}

inline void MathTools::Clamp(const float digits,float &value)
{
	float n = pow(10.0f,digits);
	float min = -n, max = n;
	if (value < min)
		value = min;
	if (value > max)
		value = max;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//	HASH Spline Math: formula for solving for peaked default tangent.
//////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void MathTools::CalcPeakedDefaultTangent(Vector p1, Vector p2, Vector p3, Vector &it, Vector &ot, Vector *gammaplane)
{
	it = (p2-p1)* 0.5f;
	ot = (p3-p2)* 0.5f;
	if ( gammaplane)
		*gammaplane = Cross(it, ot);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//	HASH Spline Math: formula for solving for smooth default tangent.
//////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void MathTools::CalcSmoothDefaultTangent(Vector p1, Vector p2, Vector p3, Vector &it, Vector &ot, Vector *gammaplane)
{
	Vector direction;
	float itmagnitude, otmagnitude;

	direction = p3 - p1;
	otmagnitude = (p3-p2).Length();
	itmagnitude = (p2-p1).Length();
	
	float sum = itmagnitude + otmagnitude;

	if (sum == 0)
	{
		it = 0;
		ot = 0;//all points are coincident			
	}
	else
	{
		it = direction*(itmagnitude/sum);
		ot = direction*(otmagnitude/sum);
	}

	if (gammaplane)
		*gammaplane = Cross( direction,  p1-p2 );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//  HASH Spline Math: formula for multiplying bias to default tangent.
//////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void MathTools::CalcBias(Vector &tangent, float alpha, float gamma, float magnitude, Vector gammaplane)
{
	if (tangent.LengthSq() != 0)
	{
//		Vector alphaplane = Cross( gammaplane, tangent );
//		Quaternion quat = Rotate2VQ( alpha, alphaplane ) * Rotate2VQ( -gamma, gammaplane );
//		tangent *= quat;
		tangent *= magnitude;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//  HASH Spline Math: formula for solving for smooth default end tangent.
//////////////////////////////////////////////////////////////////////////////////////////////////////////

inline Vector MathTools::CalcSmoothDefaultEndTangent(Vector p2, Vector p3, Vector p4, Vector *gammaplane)
{
	Vector delta = (p2 - p3).Normalized();
	Vector L1 = (p3 - p2).Length();
	Vector T1 = (p2 - p4).Normalized() * L1;
	Vector along = Dot( T1, delta ) * delta;
	Vector away = T1 - along;
	Vector T3 = along - away;
	Vector maxv = ((p2 + p4)*0.5f - p3).Normalized();
	float mincos = Dot( maxv, delta );
	float T3len;
	if (T3len = T3.Length()) {
		Vector temp = T3;
		float newcos = Dot(T3/T3len, delta);
		if (newcos < mincos)
			T3 = maxv * T3len;
		if ( gammaplane )
			*gammaplane = Cross(temp, delta);
	}
	return T3;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//  HASH Spline Math: formula for solving for peaked default end tangent.
//////////////////////////////////////////////////////////////////////////////////////////////////////////

inline Vector MathTools::CalcPeakedDefaultEndTangent(Vector p2, Vector p3, Vector p4, Vector *gammaplane)
{
	Vector T3 = (p2-p3) * 0.5f;
	if (gammaplane)
		*gammaplane = Cross( T3,  p2 );
	return T3;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//	HASH Spline Math: formula for solving for peaked default tangent.
//////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void MathTools::CalcPeakedDefaultTangent(Vector2 p1, Vector2 p2, Vector2 p3, Vector2 &it, Vector2 &ot)
{
   it = (p2-p1)* 0.5f;
   ot = (p3-p2)* 0.5f;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//	HASH Spline Math: formula for solving for smooth default tangent.
//////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void MathTools::CalcSmoothDefaultTangent(Vector2 p1, Vector2 p2, Vector2 p3, Vector2 &it, Vector2 &ot)
{
   Vector2 direction;
   float itmagnitude, otmagnitude;

   direction = p3 - p1;
   otmagnitude = (p3-p2).Length();
   itmagnitude = (p2-p1).Length();

   float sum = itmagnitude + otmagnitude;

   if (sum == 0)
   {
      it = 0;
      ot = 0;//all points are coincident			
   }
   else
   {
      it = direction*(itmagnitude/sum);
      ot = direction*(otmagnitude/sum);
   }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//  HASH Spline Math: formula for multiplying bias to default tangent.
//////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void MathTools::CalcBias(Vector2 &tangent, float alpha, float magnitude)
{
   if (tangent.LengthSq() != 0)
   {
      //		Vector alphaplane = Cross( gammaplane, tangent );
      //		Quaternion quat = Rotate2VQ( alpha, alphaplane ) * Rotate2VQ( -gamma, gammaplane );
      //		tangent *= quat;
      tangent *= magnitude;
   }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//  HASH Spline Math: formula for solving for smooth default end tangent.
//////////////////////////////////////////////////////////////////////////////////////////////////////////

inline Vector2 MathTools::CalcSmoothDefaultEndTangent(Vector2 p2, Vector2 p3, Vector2 p4)
{
   Vector2 delta = (p2 - p3).Normalized();
   Vector2 L1 = (p3 - p2).Length();
   Vector2 T1 = (p2 - p4).Normalized() * L1;
   Vector2 along = Dot( T1, delta ) * delta;
   Vector2 away = T1 - along;
   Vector2 T3 = along - away;
   Vector2 maxv = ((p2 + p4)*0.5f - p3).Normalized();
   float mincos = Dot( maxv, delta );
   float T3len;
   if (T3len = T3.Length()) {
      Vector2 temp = T3;
      float newcos = Dot(T3/T3len, delta);
      if (newcos < mincos)
         T3 = maxv * T3len;
   }
   return T3;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//  HASH Spline Math: formula for solving for peaked default end tangent.
//////////////////////////////////////////////////////////////////////////////////////////////////////////

inline Vector2 MathTools::CalcPeakedDefaultEndTangent(Vector2 p2, Vector2 p3, Vector2 p4)
{
   Vector2 T3 = (p2-p3) * 0.5f;
   return T3;
}

inline Vector MathTools::CalcPlaneNormalFrom3Points(const Vector &p1,const Vector &p2,const Vector &p3)
{
   return (((p2-p1).Normalized() ^ (p3-p1).Normalized()) + ((p3-p2).Normalized() ^ (p1-p2).Normalized()) + ((p1-p3).Normalized() ^ (p2-p3).Normalized())).Normalized();
}

inline Vector MathTools::CalcPlaneNormalFrom4Points(const Vector &p1,const Vector &p2,const Vector &p3,const Vector &p4)
{
   return (((p2-p1).Normalized() ^ (p4-p1).Normalized()) + ((p3-p2).Normalized() ^ (p1-p2).Normalized()) + ((p4-p3).Normalized() ^ (p2-p3).Normalized()) + ((p1-p4).Normalized() ^ (p3-p4).Normalized())).Normalized();
}

inline void MathTools::PointToLineIntersection(const Vector &point,const Vector &lpt1,const Vector &lpt2,Vector &i,float &percent)
{
	if (lpt1==lpt2) // if the distance between points is 0 then give one of the points
	{
		i = lpt1;
		return;
	}
	float u = ((point.x-lpt1.x)*(lpt2.x-lpt1.x)+(point.y-lpt1.y)*(lpt2.y-lpt1.y)+(point.z-lpt1.z)*(lpt2.z-lpt1.z));
	float lengthSq = (lpt2-lpt1).LengthSq();
	u /= lengthSq;
	i = lpt1+(lpt2-lpt1)*u;
   percent = u;
	return;
}

inline float MathTools::PointToLineIntersectionPercent(const Vector &point,const Vector &lpt1,const Vector &lpt2)
{
	if (lpt1==lpt2) // if the distance between points is 0 then give one of the points
		return 0.0f;
	float u = ((point.x-lpt1.x)*(lpt2.x-lpt1.x)+(point.y-lpt1.y)*(lpt2.y-lpt1.y)+(point.z-lpt1.z)*(lpt2.z-lpt1.z));
	float lengthSq = (lpt2-lpt1).LengthSq();
	u /= lengthSq;
	return u;
}

inline float MathTools::PointToLineIntersectionPercent2D(const Vector2 &point,const Vector2 &lpt1,const Vector2 &lpt2)
{
	if (lpt1==lpt2) // if the distance between points is 0 then give one of the points
		return 0.0f;
	float u = (point.x-lpt1.x)*(lpt2.x-lpt1.x)+(point.y-lpt1.y)*(lpt2.y-lpt1.y);
	float lengthSq = (lpt2-lpt1).LengthSq();
	u /= lengthSq;
	return u;
}

inline bool MathTools::Do2DLinesIntersect(const Vector2 &lpt1A,const Vector2 &lpt2A,const Vector2 &lpt1B,const Vector2 &lpt2B,Vector2 &intersection)
{
	float x1 = lpt1A.x;
	float x2 = lpt2A.x;
	float x3 = lpt1B.x;
	float x4 = lpt2B.x;

	float y1 = lpt1A.y;
	float y2 = lpt2A.y;
	float y3 = lpt1B.y;
	float y4 = lpt2B.y;

	float denominator = ((y4-y3)*(x2-x1))-((x4-x3)*(y2-y1));
	float ua =(((x4-x3)*(y1-y3))-((y4-y3)*(x1-x3)))/denominator;
	float ub =(((x2-x1)*(y1-y3))-((y2-y1)*(x1-x3)))/denominator;

	intersection.x = x1 + ua*(x2 - x1);
	intersection.y = y1 + ub*(y2 - y1);
	
	if (denominator == 0.0f)// is parallel
		return false;
	return true;
}

inline bool MathTools::Do2DLinesIntersectBetweenSegments(const Vector2 &lpt1A,const Vector2 &lpt2A,
														 const Vector2 &lpt1B,const Vector2 &lpt2B,
														 Vector2 &intersection)
{
	float x1 = lpt1A.x;
	float x2 = lpt2A.x;
	float x3 = lpt1B.x;
	float x4 = lpt2B.x;

	float y1 = lpt1A.y;
	float y2 = lpt2A.y;
	float y3 = lpt1B.y;
	float y4 = lpt2B.y;

	float denominator = ((y4-y3)*(x2-x1))-((x4-x3)*(y2-y1));

	if (denominator == 0.0)// is parallel
		return false;
	
	float ua =(((x4-x3)*(y1-y3))-((y4-y3)*(x1-x3)))/denominator;
	float ub =(((x2-x1)*(y1-y3))-((y2-y1)*(x1-x3)))/denominator;
	
	intersection.x = x1 + ua*(x2 - x1);
	intersection.y = y1 + ub*(y2 - y1);

	if (((ua >= 0.0f)&&(ua <= 1.0f))&&((ub >= 0.0f)&&(ub <= 1.0f)))
		return true;

	return false;
}

inline bool MathTools::Does2DLineIntersectRect(const Vector2 &point1,const Vector2 &point2,const RECT &rect)
{
	Vector2 i;
	if (Do2DLinesIntersectBetweenSegments(point1,point2,Vector2(rect.left,rect.top),Vector2(rect.left,rect.bottom),i))
		return true;
	if (Do2DLinesIntersectBetweenSegments(point1,point2,Vector2(rect.left,rect.bottom),Vector2(rect.right,rect.bottom),i))
		return true;
	if (Do2DLinesIntersectBetweenSegments(point1,point2,Vector2(rect.right,rect.top),Vector2(rect.right,rect.bottom),i))
		return true;
	if (Do2DLinesIntersectBetweenSegments(point1,point2,Vector2(rect.left,rect.top),Vector2(rect.right,rect.top),i))
		return true;
	return false;
}

inline bool MathTools::Is2DPointIn2DRect(Vector2 &point,const RECT &rect)
{
	if (((point.x>rect.left)&&(point.x<rect.right))&&((point.y>rect.top)&&(point.y<rect.bottom)))
		return true;
	return false;
}

inline bool MathTools::Is2DPointOn2DLine(const Vector2 &p,
										 const Vector2 &p1,const Vector2 &p2,
										 float tol,bool &isfirstclosest)
{
	Vector2 vectA = p2-p1;
	Vector2 vectB = p-p1;

	Vector2 vectC = p1-p2;
	Vector2 vectD = p-p2;
	
	Vector2 dir = p2-p1;

	RECT rect;

	if (dir.x<0)
	{
		rect.left = (LONG)p2.x;
		rect.right = (LONG)p1.x;
	}

	else
	{
		rect.left = (LONG)p1.x;
		rect.right = (LONG)p2.x;
	}

	if (dir.y<0)
	{
		rect.top = (LONG)p2.y;
		rect.bottom = (LONG)p1.y;
	}
	else
	{
		rect.top = (LONG)p1.y;
		rect.bottom = (LONG)p2.y;
	}

	isfirstclosest = false;
	if ((vectB.Length()) < (vectD.Length()))
		isfirstclosest = true;

	if (((p.x>rect.left)&&(p.x<rect.right))&&((p.y>rect.top)&&(p.y<rect.bottom)))
	{
		if (vectA.IsParallel(vectB,tol))
			if (vectC.IsParallel(vectD,tol))
				return true;
	}

	return false;
}

inline float MathTools::GetClosestPointOnBezierSplineSegment(const Vector &point,
															  const Vector &p0,const Vector &p1,const Vector &p2,const Vector &p3,
															  float sample,float param0,float param1,Vector &intersect)
{
	if (sample==0)
	{
		if (((p0-point).LengthSq())<((p3-point).LengthSq()))
		{
			intersect = p0;
			return param0;
		}
		else
		{
			intersect = p3;
			return param1;
		}
	}
	Vector a0,a1,a2,a3;
	Vector b0,b1,b2,b3;
	MathTools::SplitBezierCurve(0.5f,p0,p1,p2,p3,a0,a1,a2,a3,b0,b1,b2,b3);
	if (((a0-point).LengthSq() + (a3-point).LengthSq())<((b0-point).LengthSq() + (b3-point).LengthSq()))
	{
		return GetClosestPointOnBezierSplineSegment(point,a0,a1,a2,a3,sample-1,param0,(param1-param0)*0.5f+param0,intersect);
	}
	return GetClosestPointOnBezierSplineSegment(point,b0,b1,b2,b3,sample-1,(param1-param0)*0.5f+param0,param1,intersect);
}
