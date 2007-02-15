#pragma once

#define GEN_MATH_H


namespace GenMath
{
	const double PI = 3.141592653589793238462643383279502884197169399375105820974944592308;
	const double DEGTORAD = PI/180.0;
	const double RADTODEG = 180.0/PI;
	const double PITIMES2 = PI*PI;
	const double PIOVER2 = PI/2.0;
	const double E = 2.718282;
	const double SQRT2 = 1.4142135623730950488016887242096980785696718753769480732;
	const double SQRT3 = 1.73205080757;
	const double GOLDEN = 1.618034;
	const double KAPPA = 0.552284749828; // bezier circle control point ratio ((SQRT(2)-1)/3)*4
	const double ONETHIRD = 0.333333333333333333333333333333333333333333333333333333333333;

	/***********************/
	/* One argument template functions
	/***********************/

	/* absolute value of a */
	template<class T> inline T ABS(T a) 
	{
		return (((a)<0) ? -(a) : (a));

	}

	/* round a to nearest int */
	template<class T> inline T ROUND(T a) 
	{
		return ((a)>0) ? (T)((int)((double)(a)+0.5)) : -(T)((int)(0.5-(double)(a)));
	}

	/* take sign of a, either -1, 0, or 1 */
	template<class T> inline int ZSGN(T a) 
	{
		return (((a)<0) ? -1 : (a)>0 ? 1 : 0);
	}

	/* take binary sign of a, either -1, or 1 if >= 0 */
	template<class T> inline int SGN(T a) 
	{
		return (((a)<0) ? -1 : 1);
	}

	/* square a */
	template<class T> inline T SQR(T a) 
	{
		return a*a;
	}

	/***********************/
	/* two-argument macros */
	/***********************/

	/* find minimum of a and b */
	template<class T> inline T MIN(T a,T b) 
	{
		return a<b ? a:b;
	}

	template<class T> inline T MAX(T a,T b) 
	{
		return a>b ? a:b;
	}

	template<class T> inline void SWAP(T a,T b) 
	{
		T c(a);
		a=b;
		b=a;
	}

	/* linear interpolation from l (when a=0) to h (when a=1)*/
	/* (equal to (a*h)+((1-a)*l) */
	template<class T> inline T LERP(double a,T l,T h) 
	{
		return (T)(((l)+(((h)-(l))*(a))));
	}

	/* clamp the input to the specified range */
	template<class T> inline T CLAMP(T v,T l,T h) 
	{
		return (v<l ? l : v>h ? h : v);
	}
};
