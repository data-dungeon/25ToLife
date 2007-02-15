#ifndef _RaySeg_H
#define _RaySeg_H

class RaySegment
{
public:
	RaySegment() {};
	~RaySegment() {};
	
	RaySegment(const Vector &start,const Vector &end)
	{
		m_start = start;
		m_end = end;
	}

	Vector ClosestPoint(const Vector &point) const;
	Vector ClosestPointOnSegment(const Vector &point) const;
	bool IntersectWithPlane(const Vector &planeP,const Vector &planeN,Vector &i) const;
	bool IntersectWithPlane(const Vector &planeP,const Vector &planeN,Vector &i,float &t) const;
	bool IntersectWithPlaneOnSegment(const Vector &planeP,const Vector &planeN,Vector &i,float &t) const;
	bool IntersectWithPlaneFromFront(const Vector &planeP,const Vector &planeN,Vector &i,float &t) const;
	bool IntersectWithPlaneFromBack(const Vector &planeP,const Vector &planeN,Vector &i,float &t) const;
	bool IntersectWithPlaneFromFrontOnSegment(const Vector &planeP,const Vector &planeN,Vector &i,float &t) const;
	bool IntersectWithPlaneFromBackOnSegment(const Vector &planeP,const Vector &planeN,Vector &i,float &t) const;
	
protected:
	Vector m_start,m_end;
};

#include "rayseg.hpp"

#endif