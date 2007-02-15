////////////////////////////////////////////////////////////////////////////
//
// TupVertex
//
// classes to handle vertices
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: TupVertex.h $
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 8/29/03    Time: 1:38p
 * Updated in $/Avalanche/tools/Tupper3d
 * tesselation code for 2d
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 7/29/03    Time: 3:25p
 * Updated in $/Avalanche/tools/Tupper3d
 * start of poly mesh and converters
 */

#ifndef TUPPER3D_TUP_VERTEX_H
#define TUPPER3D_TUP_VERTEX_H

#include <math.h>

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

template<class TYPE>
class TupVertex3
{
public:
	TYPE x,y,z;

	TupVertex3(void);
	TupVertex3(TYPE nx,TYPE ny,TYPE nz);
	TupVertex3( const TupVertex3 &s );  // copy constructor
	TupVertex3& operator=( const TupVertex3 &s );  // assignment operator
	void Clear(void);
	void Set(TYPE nx,TYPE ny,TYPE nz);
	void Abs(void);
	TYPE Length(void);
	TYPE LengthSq(void);
	static TYPE Distance(const TupVertex3 &p1,const TupVertex3 &p2);
	static TYPE DistanceSq(const TupVertex3 &p1,const TupVertex3 &p2);
	void Normalize(void);
	void Normalize(TYPE &value);
	static TYPE DotProduct(const TupVertex3 &p1,const TupVertex3 &p2);
	static TupVertex3 CrossProduct(const TupVertex3 &p1,const TupVertex3 &p2);
	static TupVertex3 ComputeFaceNormal(const TupVertex3 &p1,const TupVertex3 &p2,const TupVertex3 &p3);
	TYPE DistanceToLine(const TupVertex3 &point1,const TupVertex3 &point2);
	TupVertex3 PointToLineIntersection(const TupVertex3 &point1,const TupVertex3 &point2);
	TYPE PointToLineIntersectionPercent(const TupVertex3 &point1,const TupVertex3 &point2);
	static bool SphereLineIntersection(TupVertex3 &point1, TupVertex3 &point2, TupVertex3 &sphereCenter, TYPE radius, TupVertex3 &inpoint, TupVertex3 &outpoint);

	TupVertex3& operator+=(const TupVertex3 &s ); // += operator
	TupVertex3& operator-=(const TupVertex3 &s ); // -= operator
	TupVertex3& operator*=(const TupVertex3 &s ); // *= operator
	TupVertex3& operator/=(const TupVertex3 &s ); // /= operator
	TupVertex3& operator+=(const TYPE s ); // += operator
	TupVertex3& operator-=(const TYPE s ); // -= operator
	TupVertex3& operator*=(const TYPE s ); // *= operator
	TupVertex3& operator/=(const TYPE s ); // /= operator

	friend TupVertex3<TYPE> operator+(const TupVertex3<TYPE> &p1,const TupVertex3<TYPE> &p2)
	{
		return TupVertex3<TYPE>(p1.x+p2.x,p1.y+p2.y,p1.z+p2.z);
	}

	friend TupVertex3<TYPE> operator-(const TupVertex3<TYPE> &p1,const TupVertex3<TYPE> &p2)
	{
		return TupVertex3<TYPE>(p1.x-p2.x,p1.y-p2.y,p1.z-p2.z);
	}

	friend TupVertex3<TYPE> operator*(const TupVertex3<TYPE> &p1,const TupVertex3<TYPE> &p2)
	{
		return TupVertex3<TYPE>(p1.x*p2.x,p1.y*p2.y,p1.z*p2.z);
	}

	friend TupVertex3<TYPE> operator/(const TupVertex3<TYPE> &p1,const TupVertex3<TYPE> &p2)
	{
		return TupVertex3<TYPE>(p1.x/p2.x,p1.y/p2.y,p1.z/p2.z);
	}

	friend TupVertex3<TYPE> operator+(const TupVertex3<TYPE> &p1,const TYPE p)
	{
		return TupVertex3<TYPE>(p1.x+p,p1.y+p,p1.z+p);
	}

	friend TupVertex3<TYPE> operator-(const TupVertex3<TYPE> &p1,const TYPE p)
	{
		return TupVertex3<TYPE>(p1.x-p,p1.y-p,p1.z-p);
	}

	friend TupVertex3<TYPE> operator*(const TupVertex3<TYPE> &p1,const TYPE p)
	{
		return TupVertex3<TYPE>(p1.x*p,p1.y*p,p1.z*p);
	}

	friend TupVertex3<TYPE> operator/(const TupVertex3<TYPE> &p1,const TYPE p)
	{
		return TupVertex3<TYPE>(p1.x/p,p1.y/p,p1.z/p);
	}

	friend TupVertex3<TYPE> operator+(const TYPE p,const TupVertex3<TYPE> &p2)
	{
		return TupVertex3<TYPE>(p+p2.x,p+p2.y,p+p2.z);
	}

	friend TupVertex3<TYPE> operator-(const TYPE p,const TupVertex3<TYPE> &p2)
	{
		return TupVertex3<TYPE>(p-p2.x,p-p2.y,p-p2.z);
	}

	friend TupVertex3<TYPE> operator*(const TYPE p,const TupVertex3<TYPE> &p2)
	{
		return TupVertex3<TYPE>(p*p2.x,p*p2.y,p*p2.z);
	}

	friend TupVertex3<TYPE> operator/(const TYPE p,const TupVertex3<TYPE> &p2)
	{
		return TupVertex3<TYPE>(p/p2.x,p/p2.y,p/p2.z);
	}

	friend bool operator==(const TupVertex3<TYPE> &p1,const TupVertex3<TYPE> &p2)
	{
		return (p1.x==p2.x && p1.y==p2.y && p1.z==p2.z);
	}

	friend bool operator!=(const TupVertex3<TYPE> &p1,const TupVertex3<TYPE> &p2)
	{
		return (p1.x!=p2.x || p1.y!=p2.y || p1.z!=p2.z);
	}
};

typedef TupVertex3<float> TupVertex3f;
typedef TupVertex3<double> TupVertex3d;

extern double TupVertex3Sqrt(double value);

class TupVertex3Utils
{
public:
	static TupVertex3f Convert(TupVertex3d source);
	static TupVertex3d Convert(TupVertex3f source);
	static void Convert(TupArray<TupVertex3f> &dest,const TupArray<TupVertex3d> &source);
	static void Convert(TupArray<TupVertex3d> &dest,const TupArray<TupVertex3f> &source);
};

#include "TupVertex.hpp"

#endif