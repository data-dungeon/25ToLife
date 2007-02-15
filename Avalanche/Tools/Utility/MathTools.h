#ifndef __MathTools_H
#define __MathTools_H

#ifndef __QUATERNION_H
#include "Quaternion.h"
#endif

class MathTools
{
public:	
	static float	Bezier(float t,float p0,float p1,float p2,float p3);
	static Vector	Bezier(float t,const Vector &p0,const Vector &p1,const Vector &p2,const Vector &p3);
   static Vector2	Bezier(float t,const Vector2 &p0,const Vector2 &p1,const Vector2 &p2,const Vector2 &p3);
   static Vector4	Bezier(float t,const Vector4 &p0,const Vector4 &p1,const Vector4 &p2,const Vector4 &p3);
	
	static void		SplitBezierCurve(float t,const Vector &c0,const Vector &c1,const Vector &c2,const Vector &c3,
									Vector &a0,Vector &a1,Vector &a2,Vector &a3,
									Vector &b0,Vector &b1,Vector &b2,Vector &b3);

   static void    ComputeTangents( const Vector *pV0, const Vector &pV1, const Vector &pV2, const Vector *pV3, Vector *pOutTangent1, Vector *pOutTangent2, float fV1Alpha=0.0f, float fV1Gamma=0.0f, float fV1Magnitude=1.0f, float fV2Alpha=0.0f, float fV2Gamma=0.0f, float fV2Magnitude=1.0f );
   static void    ComputeTangents( const Vector2 *pV0, const Vector2 &pV1, const Vector2 &pV2, const Vector2 *pV3, Vector2 *pOutTangent1, Vector2 *pOutTangent2, float fV1Alpha=0.0f, float fV1Magnitude=1.0f, float fV2Alpha=0.0f, float fV2Magnitude=1.0f );

	static bool    IsBezierSegmentStraight(float tolerance,const Vector &p0,const Vector &p1,const Vector &p2,const Vector &p3);
	
	static int		CalcBezierCurveSubdivisions(float tolerance,const Vector &p0,const Vector &p1,const Vector &p2,const Vector &p3,int maxsubd);
	
	static float	Derivative(float t,float p0,float p1,float p2,float p3);
	static Vector	Derivative(float t,Vector p0,Vector p1,Vector p2,Vector p3);
	
	static float	SecondDerivative(float t,float p0,float p1,float p2,float p3);
	static Vector	SecondDerivative(float t,Vector p0,Vector p1,Vector p2,Vector p3);
	
	static void		Clamp( const float digits,float &value);

	static Vector	CalcPeakedDefaultEndTangent(Vector p2,Vector p3,Vector p4,Vector *gammaplane);
	static Vector	CalcSmoothDefaultEndTangent(Vector p2,Vector p3,Vector p4,Vector *gammaplane);
	static void		CalcSmoothDefaultTangent(Vector p1,Vector p2,Vector p3,Vector &in,Vector &out,Vector *gammaplane);
	static void		CalcPeakedDefaultTangent(Vector p1,Vector p2,Vector p3,Vector &it,Vector &ot,Vector *gammaplane);
	static void		CalcBias(Vector &tangent,float alpha,float gamma,float magnitude,Vector gammaplane);
	
   static Vector2	CalcPeakedDefaultEndTangent(Vector2 p2,Vector2 p3,Vector2 p4);
   static Vector2	CalcSmoothDefaultEndTangent(Vector2 p2,Vector2 p3,Vector2 p4);
   static void		CalcSmoothDefaultTangent(Vector2 p1,Vector2 p2,Vector2 p3,Vector2 &in,Vector2 &out);
   static void		CalcPeakedDefaultTangent(Vector2 p1,Vector2 p2,Vector2 p3,Vector2 &it,Vector2 &ot);
   static void		CalcBias(Vector2 &tangent,float alpha,float magnitude);

	static Vector	CalcPlaneNormalFrom3Points(const Vector &p1,const Vector &p2,const Vector &p3);
   static Vector	CalcPlaneNormalFrom4Points(const Vector &p1,const Vector &p2,const Vector &p3,const Vector &p4);
	
	static void		PointToLineIntersection(const Vector &point,const Vector &lpt1,const Vector &lpt2,Vector &i,float &percent);
	static float	PointToLineIntersectionPercent(const Vector &point,const Vector &lpt1,const Vector &lpt2);
	static float	PointToLineIntersectionPercent2D(const Vector2 &point,const Vector2 &lpt1,const Vector2 &lpt2);
	
	static bool		Do2DLinesIntersect(const Vector2 &point1A,const Vector2 &point1B,const Vector2 &point2A,const Vector2 &point2B,Vector2 &intersection);
	static bool		Do2DLinesIntersectBetweenSegments(const Vector2 &point1A,const Vector2 &point1B,const Vector2 &point2A,const Vector2 &point2B,Vector2 &intersection);
	static bool		Does2DLineIntersectRect(const Vector2 &point1,const Vector2 &point2,const RECT &rect);
	static bool		Is2DPointIn2DRect(Vector2 &point,const RECT &rect);
	static bool		Is2DPointOn2DLine(const Vector2 &p,const Vector2 &p1,const Vector2 &p2,float tol,bool &isfirstclosest);
	static float	GetClosestPointOnBezierSplineSegment(const Vector &point,const Vector &p0,const Vector &p1,const Vector &p2,const Vector &p3,
														float sample,float param0,float param1,Vector &intersect);

protected:
   // recursive support function
	static int		DoCalcBezierCurveSubdivisions(float tolerance,const Vector &p0,const Vector &p1,const Vector &p2,const Vector &p3,int level,int maxsubd);
};

#include "MathTools.hpp"

#endif