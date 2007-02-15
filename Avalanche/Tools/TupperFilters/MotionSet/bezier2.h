#pragma once

#define BEZIER2_H

#ifndef HASH_VECTOR2_H
#include "HashVector2.h"
#endif

class BezierEnums
{
public:
	enum PointTypes
	{
		POINT1,
		CONTROL1,
		CONTROL2,
		POINT2
	};
	enum Values
	{
		MAXDEPTH = 64,
		W_DEGREE = 5,
		DEGREE = 3,
		MAXPOINTS = 1000,
	};
};

const double BEZIER_EPSILON = 1.08420217249e-19; // (ldexp(1.0,-MAXDEPTH-1)) Flatness control value

// define if code needs to build curves that have tangent x values placed at thirds
#define BUILD_THIRDS

class Bezier2
{
public:
	static HashVector2 EvalCurve(HashVector2 *points,double t);
	static HashVector2 EvalCurve(HashVector2 *points,double t,int degree,HashVector2 *left=NULL,HashVector2 *right=NULL);
	static HashVector2 NearestPointOnCurve(HashVector2 *points,HashVector2 point,double *percent=NULL,double *distanceSq=NULL);
	static void FitCurve(const TupArray<HashVector2> &points, TupArray<HashVector2> &curvePointsFill, double error);
	static void CurveToPoints(HashVector2 *points,TupArray<HashVector2> &pointsFill,double threshold,bool addEnd);
	static void CurveToPointsPercents(HashVector2 *points,TupArray<HashVector2> &pointsFill,TupArray<double> &percentFill,double threshold,bool addEnd=true,double min=0.0,double max=1.0);
	static int GetCrossingPercents(HashVector2 *points,TupArray<double> &percentFill,double y);
	static int GetCrossingPoints(HashVector2 *points,TupArray<HashVector2> &pointsFill,double y);
	static void CombineCurves(HashVector2 *curves,HashVector2 *curveFill); // convert 7 points on two curves to a single curve
	static void AdjustCurve(HashVector2 *curve,const HashVector2 &delta,double t); // adjust curve at t with delta
	static double DistanceSq(const HashVector2 &point1,const HashVector2 &point2);

private:
	// nearest point on curve stuff
	static void ConvertToBezierForm(HashVector2 *points,HashVector2 point,HashVector2 *w);
	static int FindRoots(HashVector2 *w,int degree,double *t,int depth);
	static int CrossingCount(HashVector2 *points,int degree);
	static int ControlPolygonFlatEnough(HashVector2 *points,int degree);
	static double ComputeXIntercept(HashVector2 *points,int degree);
	// fit curve stuff
	static void FitCubic(const TupArray<HashVector2> &points, int first, int last,const HashVector2 &tHat1,const HashVector2 &tHat2, TupArray<HashVector2> &curvePointsFill, double error);
	static void GenerateBezier(const TupArray<HashVector2> &points, int first, int last, double *uPrime,const HashVector2 &tHat1,const HashVector2 &tHat2,HashVector2 *bezCurve);
	static void Reparameterize(const TupArray<HashVector2> &points, int first, int last, double *u,HashVector2 *bezCurve, double *uPrime);
	static double NewtonRaphsonRootFind(HashVector2 *bezCurve,const HashVector2 &point,double u);
	static double B0(double u);
	static double B1(double u);
	static double B2(double u);
	static double B3(double u);
	static HashVector2 ComputeLeftTangent(const TupArray<HashVector2> &points, int index);
	static HashVector2 ComputeRightTangent(const TupArray<HashVector2> &points, int index);
	static HashVector2 ComputeCenterTangent(const TupArray<HashVector2> &points, int center);
	static void ChordLengthParameterize(const TupArray<HashVector2> &points, int first, int last, double *u);
	static double ComputeMaxError(const TupArray<HashVector2> &points, int first, int last, HashVector2 *bezCurve,double *u,int &splitPoint);

};

#include "bezier2.hpp"


