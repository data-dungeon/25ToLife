//
// Author: Tyler Colbert
// Purpose: This is a subset of the Water class.  See Water.h for more
// details.  This is the collision subset...
//

#include "Effects/EffectsPCH.h"

#include "Effects/Water/Water.h"

///////////////////////////////////////////////////////////////////////////////////////
// Quick Water collisions
///////////////////////////////////////////////////////////////////////////////////////
// inaccurate collision testing...
// these dont take the motion of the water into account, but
// may be accurate enough for some tests

bool Water::QuickTestCollision(const Segment& s, Vector3& hitPoint)
{
	// TODO: this can be optimized if needs be by doing the calculation in planespace
	// and reducing the plane normal to 0,1,0 then simplifying...

	// move the segment into our space
	Segment seg;
	seg.start.FastMultiply(s.start, mWorldToLocal);
	seg.end.FastMultiply(s.end, mWorldToLocal);

	// figure the planes center 
	Vector3 center(0.5f, 0.0f, 0.5f);

	// the planes normal
	Vector3 normal(0.0f, 1.0f, 0.0f);

	// do the intersection
	Vector3 intersection;
	float t;
	if(!seg.TrueIntersectWithPlane(center, normal, intersection, t))
		return false;

	// see if it was in our bounds...
	if(intersection.x() < 0.0f || intersection.x() > 1.0f || intersection.z() < 0.0f || intersection.z() > 1.0f)
		return false;
	
	hitPoint.FastMultiply(intersection, mInstance->Matrix());

	return true;
}

bool Water::QuickTestCollision(const Ray& r, Vector3& hitPoint)
{
	// TODO: this can be optimized if needs be by doing the calculation in planespace
	// and reducing the plane normal to 0,1,0 then simplifying...

	// move the segment into our space
	Ray ray;
	ray.o.FastMultiply(r.o, mWorldToLocal);
	ray.dir.FastBasisTransform(r.dir, mWorldToLocal);

	// figure the planes center 
	Vector3 center(0.5f, 0.0f, 0.5f);

	// the planes normal
	Vector3 normal(0.0f, 1.0f, 0.0f);

	// do the intersection
	Vector3 intersection;
	float t;
	if(!ray.TrueIntersectWithPlane(center, normal, intersection, t))
		return false;

	// see if it was in our bounds...
	if(intersection.x() < 0.0f || intersection.x() > 1.0f || intersection.z() < 0.0f || intersection.z() > 1.0f)
		return false;
	
	hitPoint.FastMultiply(intersection, mInstance->Matrix());

	return true;
}

bool Water::QuickPointInWater(const Vector3& testPoint)
{
	Vector3 point;
	point.FastMultiply(testPoint, mWorldToLocal);
	return (point.y() <= 0.0f);
}


///////////////////////////////////////////////////////////////////////////////////////
// Accurate Water collisions
///////////////////////////////////////////////////////////////////////////////////////

bool Water::TestCollision(const Segment& s, Vector3& hitPoint)
{
	// first move the segment into water space
	Segment seg;
	seg.start.FastMultiply(s.start, mWorldToLocal);
	seg.end.FastMultiply(s.end, mWorldToLocal);

	Vector3 dir = seg.end - seg.start;

	return TestCollisionLocal(seg, dir, hitPoint);
}

static void TestTriangle(const Segment& seg, const Vector3& v1, const Vector3& v2, const Vector3& v3, Vector3& hitPoint, float& mint)
{
	Vector3 n = TriangleTest::Normal(v1, v2, v3);
	Vector3 i;
	float t;
	
	if(seg.TrueIntersectWithPlane(v1, n, i, t) &&
		t < mint &&
		TriangleTest::PointInsideXZ(i, v1, v2, v3))
	{
		mint = t;
		hitPoint = i;
	}
}

bool Water::TestCollisionLocal(const Segment& seg, Vector3& dir, Vector3& hitPoint)
{
	// clip the segment to water space x&z bounds..
	Segment clipped;
	if(!mSegClipper.Clip(seg.start, seg.end, clipped.start, clipped.end))
		return false;
	
	// now hunt down intersections...
	
	// determine step rate at maximum subdivision level
	float cellwidth = (float)(1 << (mMaxSubDivDepth+1));
	float steprate = 1.0f / cellwidth;

	// find start cell...
	int cellx, cellz;
	cellx = (int)(seg.start.x() * cellwidth);
	cellz = (int)(seg.start.z() * cellwidth);
	
	// find end cell...
	int cellendx, cellendz;
	cellendx = (int)(seg.end.x() * cellwidth);
	cellendz = (int)(seg.end.z() * cellwidth);

	float mint = 1.0f;
	
	// clamp to grid... just incase
	if(cellx < 0)
		cellx = 0;
	else if(cellx >= cellwidth)
		cellx = (int)(cellwidth-1.0f);
	if(cellz < 0)
		cellz = 0;
	else if(cellz >= cellwidth)
		cellz = (int)(cellwidth-1.0f);

	if(cellendx < 0)
		cellendx = 0;
	else if(cellendx >= cellwidth)
		cellendx = (int)(cellwidth-1.0f);
	if(cellendz < 0)
		cellendz = 0;
	else if(cellendz >= cellwidth)
		cellendz = (int)(cellwidth-1.0f);

	if(cellx > cellendx) Math::Swap(cellx, cellendx);
	if(cellz > cellendz) Math::Swap(cellz, cellendz);

	int x,z;
	float xf, zf;
	float halfstep = steprate * 0.5f;

	// TODO: dont walk every cell in the square, just walk the ones the line crosses through...
	zf = cellz * steprate;
	for(z = cellz; z <= cellendz; z++)
	{
		Vector3 v[5];
		xf = cellx * steprate;
		v[2].Set(xf,
					GetHeightLocal(xf, zf),
					zf);
		v[3].Set(xf,
					GetHeightLocal(xf, zf + steprate),
					zf + steprate);
		for(x = cellx; x <= cellendx; x++)
		{
			v[0].Set(xf + halfstep,
						GetHeightLocal(xf + halfstep, zf + halfstep),
						zf + halfstep);
			v[1] = v[2];
			v[2].Set(xf + steprate,
						GetHeightLocal(xf + steprate, zf),
						zf);
			v[4] = v[3];
			v[3].Set(xf + steprate,
						GetHeightLocal(xf + steprate, zf + steprate),
						zf + steprate);

			TestTriangle(seg, v[0], v[1], v[2], hitPoint, mint);
			TestTriangle(seg, v[0], v[1], v[4], hitPoint, mint);
			TestTriangle(seg, v[0], v[3], v[4], hitPoint, mint);
			TestTriangle(seg, v[0], v[3], v[2], hitPoint, mint);
			
			xf += steprate;
		}
		zf += steprate;
	}

	// move things back to where they belong.
	if(mint < 1.0f)
	{
		hitPoint.FastMultiply(hitPoint, mInstance->Matrix());
		return true;
	}

	return false;
}

bool Water::TestCollision(const Ray& r, Vector3& hitPoint)
{
	// move the ray into water space...
	Ray ray;
	ray.o.FastMultiply(r.o, mWorldToLocal);
	ray.dir.FastBasisTransform(r.dir, mWorldToLocal);

	float t1, t2;

	// create a segment spanning the water's x/z dimensions to represent the ray.
	if(Math::Zero(ray.dir.z()))
	{
		// cant use z... so use x...
		if(Math::Zero(ray.dir.x()))
		{
			// cant use x either... so either this ray is a point...
			ASSERT(!Math::Zero(ray.dir.y()));

			// or its vertical... which is somewhat easyer...
			float ly = GetHeightLocal(ray.o.x(), ray.o.z());
			float y = ly - ray.o.y();
			if(ray.dir.y() > 0.0f && y < 0.0f)
				return false;
			if(ray.dir.y() < 0.0f && y > 0.0f)
				return false;
			hitPoint.Set(ray.o.x(),
							 ly,
							 ray.o.z());
			return true;
		}

		// use X
		float invdirx = 1.0f / ray.dir.x();
		t1 = -ray.o.x() * invdirx;
		t2 = (1.0f - ray.o.x()) * invdirx;
	}
	else
	{
		// use Z
		float invdirz = 1.0f / ray.dir.z();
		t1 = -ray.o.z() * invdirz;
		t2 = (1.0f - ray.o.z()) * invdirz;
	}

	if(t1 < 0.0f) t1 = 0.0f;
	if(t2 < 0.0f) t2 = 0.0f;

	Segment seg;
	seg.start = ray.o + ray.dir * t1;
	seg.end = ray.o + ray.dir * t2;

	return TestCollisionLocal(seg, ray.dir, hitPoint);
}

bool Water::PointInWater(const Vector3& testPoint)
{
	Vector3 point;
	point.FastMultiply(testPoint, mWorldToLocal);
	return (point.y() <= GetHeightLocal(point.x(), point.z()));
}



