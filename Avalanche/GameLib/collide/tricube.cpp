/***************************************************************************/
/***************************************************************************/
#include "collide/CollidePCH.h"
#include "collide/tricube.h"

// The epsilon
const float TriangleCubeTest::epsilon = 10e-5f;

/***************************************************************************/
// Does it intersect?
/***************************************************************************/
bool TriangleCubeTest::Intersects(
const Vector3 &cubeCenter,
float cubeHalfSize,
const Vector3 &v1,
const Vector3 &v2,
const Vector3 &v3,
float expandCube)
{
	// Build up the triangle in normalized cube space
	Triangle t;

	t.v1 = v1 - cubeCenter;
	t.v2 = v2 - cubeCenter;
	t.v3 = v3 - cubeCenter;

	// We have to scale it correctly
	float invH = 1.0f / (cubeHalfSize + expandCube);

	// Scale it into normalized cube space
	t.v1 *= invH;
	t.v2 *= invH;
	t.v3 *= invH;

	// Do it!!!
	return (Intersection(t) == INSIDE);
}

/***************************************************************************/
// Give the sign 3
/***************************************************************************/
inline long TriangleCubeTest::Sign3(const Vector3 &v)
{
 	return ((v.X() < epsilon) ? 4 : 0 | (v.X() > -epsilon) ? 32 : 0 |
   			(v.Y() < epsilon) ? 2 : 0 | (v.Y() > -epsilon) ? 16 : 0 |
   			(v.Z() < epsilon) ? 1 : 0 | (v.Z() > -epsilon) ? 8 : 0);
}

/***************************************************************************/
// Lerp
/***************************************************************************/
inline float TriangleCubeTest::LERP(float a, float b, float c)
{
	return (b + a * (c - b));
}

/***************************************************************************/
// Which of the six face-plane(s) is point P outside of?
/***************************************************************************/
inline long TriangleCubeTest::FacePlane(const Vector3 &p)
{
	long outcode = 0;
  	if (p.X() >  0.5f) outcode |= 0x01;
  	if (p.X() < -0.5f) outcode |= 0x02;
  	if (p.Y() >  0.5f) outcode |= 0x04;
  	if (p.Y() < -0.5f) outcode |= 0x08;
  	if (p.Z() >  0.5f) outcode |= 0x10;
  	if (p.Z() < -0.5f) outcode |= 0x20;
  	return outcode;
}

/***************************************************************************/
// Which of the twelve edge plane(s) is point P outside of?
/***************************************************************************/
inline long TriangleCubeTest::Bevel2d(const Vector3 &p)
{
	long outcode = 0;
	if ( p.X() + p.Y() > 1.0f) outcode |= 0x001;
	if ( p.X() - p.Y() > 1.0f) outcode |= 0x002;
	if (-p.X() + p.Y() > 1.0f) outcode |= 0x004;
	if (-p.X() - p.Y() > 1.0f) outcode |= 0x008;
	if ( p.X() + p.Z() > 1.0f) outcode |= 0x010;
	if ( p.X() - p.Z() > 1.0f) outcode |= 0x020;
	if (-p.X() + p.Z() > 1.0f) outcode |= 0x040;
	if (-p.X() - p.Z() > 1.0f) outcode |= 0x080;
	if ( p.Y() + p.Z() > 1.0f) outcode |= 0x100;
	if ( p.Y() - p.Z() > 1.0f) outcode |= 0x200;
	if (-p.Y() + p.Z() > 1.0f) outcode |= 0x400;
	if (-p.Y() - p.Z() > 1.0f) outcode |= 0x800;
	return outcode;
}

/***************************************************************************/
// Which of the eight corner plane(s) is point P outside of?
/***************************************************************************/
inline long TriangleCubeTest::Bevel3d(const Vector3 &p)
{
	long outcode = 0;
	if (( p.X() + p.Y() + p.Z()) > 1.5f) outcode |= 0x01;
	if (( p.X() + p.Y() - p.Z()) > 1.5f) outcode |= 0x02;
	if (( p.X() - p.Y() + p.Z()) > 1.5f) outcode |= 0x04;
	if (( p.X() - p.Y() - p.Z()) > 1.5f) outcode |= 0x08;
	if ((-p.X() + p.Y() + p.Z()) > 1.5f) outcode |= 0x10;
	if ((-p.X() + p.Y() - p.Z()) > 1.5f) outcode |= 0x20;
	if ((-p.X() - p.Y() + p.Z()) > 1.5f) outcode |= 0x40;
	if ((-p.X() - p.Y() - p.Z()) > 1.5f) outcode |= 0x80;
	return outcode;
}

/***************************************************************************/
// Test the point "alpha" of the way from P1 to P2
// See if it is on a face of the cube
// Consider only faces in "mask"
/***************************************************************************/
inline long TriangleCubeTest::CheckPoint(const Vector3 &p1, const Vector3 &p2, float alpha, long mask)
{
	Vector3 planePoint;

	planePoint.X(LERP(alpha, p1.X(), p2.X()));
	planePoint.Y(LERP(alpha, p1.Y(), p2.Y()));
	planePoint.Z(LERP(alpha, p1.Z(), p2.Z()));

	return (FacePlane(planePoint) & mask);
}

/***************************************************************************/
// Compute intersection of P1 --> P2 line segment with face planes
// Then test intersection point to see if it is on cube face
// Consider only face planes in "outcode_diff"
// Note: Zero bits in "outcodeDiff" means face line is outside of
/***************************************************************************/
long TriangleCubeTest::CheckLine(const Vector3 &p1, const Vector3 &p2, long outcodeDiff)
{

	if ((0x01 & outcodeDiff) != 0)
	   if (CheckPoint(p1,p2,( 0.5f-p1.X())/(p2.X()-p1.X()),0x3e) == INSIDE) return INSIDE;
	if ((0x02 & outcodeDiff) != 0)
	   if (CheckPoint(p1,p2,(-0.5f-p1.X())/(p2.X()-p1.X()),0x3d) == INSIDE) return INSIDE;
	if ((0x04 & outcodeDiff) != 0)
	   if (CheckPoint(p1,p2,( 0.5f-p1.Y())/(p2.Y()-p1.Y()),0x3b) == INSIDE) return INSIDE;
	if ((0x08 & outcodeDiff) != 0)
	   if (CheckPoint(p1,p2,(-0.5f-p1.Y())/(p2.Y()-p1.Y()),0x37) == INSIDE) return INSIDE;
	if ((0x10 & outcodeDiff) != 0)
	   if (CheckPoint(p1,p2,( 0.5f-p1.Z())/(p2.Z()-p1.Z()),0x2f) == INSIDE) return INSIDE;
	if ((0x20 & outcodeDiff) != 0)
	   if (CheckPoint(p1,p2,(-0.5f-p1.Z())/(p2.Z()-p1.Z()),0x1f) == INSIDE) return INSIDE;

	return OUTSIDE;
}

/***************************************************************************/
// Test if 3D point is inside 3D triangle
/***************************************************************************/
long TriangleCubeTest::PointTriangleIntersection(const Point &p, const Triangle &t)
{
	// First, a quick bounding-box test:
	// If P is outside triangle bbox, there cannot be an intersection.
	if (p.x > Math::Max(t.v1.X(), t.v2.X(), t.v3.X()))
		return OUTSIDE;
	if (p.y > Math::Max(t.v1.Y(), t.v2.Y(), t.v3.Y()))
		return OUTSIDE;
	if (p.z > Math::Max(t.v1.Z(), t.v2.Z(), t.v3.Z()))
		return OUTSIDE;
	if (p.x < Math::Min(t.v1.X(), t.v2.X(), t.v3.X()))
		return OUTSIDE;
	if (p.y < Math::Min(t.v1.Y(), t.v2.Y(), t.v3.Y()))
		return OUTSIDE;
	if (p.z < Math::Min(t.v1.Z(), t.v2.Z(), t.v3.Z()))
		return OUTSIDE;

	// For each triangle side, make a vector out of it by subtracting vertexes;
	// make another vector from one vertex to point P.
	// The crossproduct of these two vectors is orthogonal to both and the
	// signs of its X,Y,Z components indicate whether P was to the inside or
	// to the outside of this triangle side.
	Vector3 pv(p.x, p.y, p.z);

	Vector3 vect12 = t.v1 - t.v2;
	Vector3 vect1h = t.v1 - pv;
	Vector3 cross12_1p = Vector3::Cross(vect12, vect1h);
	long sign12 = Sign3(cross12_1p);      // Extract X,Y,Z signs as 0..7 or 0...63 integer

	Vector3 vect23 = t.v2 - t.v3;
	Vector3 vect2h = t.v2 - pv;
	Vector3 cross23_2p = Vector3::Cross(vect23, vect2h);
	long sign23 = Sign3(cross23_2p);

	Vector3 vect31 = t.v3 - t.v1;
	Vector3 vect3h = t.v3 - pv;
	Vector3 cross31_3p = Vector3::Cross(vect31, vect3h);
	long sign31 = Sign3(cross31_3p);

	// If all three crossproduct vectors agree in their component signs,
	// then the point must be inside all three.
	// P cannot be OUTSIDE all three sides simultaneously.
	return (((sign12 & sign23 & sign31) == 0) ? OUTSIDE : INSIDE);
}

/***************************************************************************/
// This is the main algorithm procedure.
// Triangle t is compared with a unit cube,
// centered on the origin.
// It returns INSIDE (0) or OUTSIDE(1) if t
// intersects or does not intersect the cube.
/***************************************************************************/
long TriangleCubeTest::Intersection(const Triangle &t)
{
	// First compare all three vertexes with all six face-planes
	// If any vertex is inside the cube, return immediately!
	long test1,test2,test3;
   if ((test1 = FacePlane(t.v1)) == INSIDE)
		return INSIDE;
   if ((test2 = FacePlane(t.v2)) == INSIDE)
		return INSIDE;
   if ((test3 = FacePlane(t.v3)) == INSIDE)
		return INSIDE;

	// If all three vertexes were outside of one or more face-planes,
	// return immediately with a trivial rejection!
   if ((test1 & test2 & test3) != 0)
		return OUTSIDE;

	// Now do the same trivial rejection test for the 12 edge planes
   test1 |= Bevel2d(t.v1) << 8;
   test2 |= Bevel2d(t.v2) << 8;
   test3 |= Bevel2d(t.v3) << 8;
   if ((test1 & test2 & test3) != 0)
		return OUTSIDE;

	// Now do the same trivial rejection test for the 8 corner planes
   test1 |= Bevel3d(t.v1) << 24;
   test2 |= Bevel3d(t.v2) << 24;
   test3 |= Bevel3d(t.v3) << 24;
   if ((test1 & test2 & test3) != 0)
		return OUTSIDE;

	// If vertex 1 and 2, as a pair, cannot be trivially rejected
	// by the above tests, then see if the t.v1-->t.v2 triangle edge
	// intersects the cube.  Do the same for t.v1-->t.v3 and t.v2-->t.v3.
	// Pass to the intersection algorithm the "OR" of the outcode
	// bits, so that only those cube faces which are spanned by
	// each triangle edge need be tested.
   if ((test1 & test2) == 0)
      if (CheckLine(t.v1,t.v2,test1|test2) == INSIDE)
			return INSIDE;

   if ((test1 & test3) == 0)
      if (CheckLine(t.v1,t.v3,test1|test3) == INSIDE)
			return INSIDE;

   if ((test2 & test3) == 0)
      if (CheckLine(t.v2,t.v3,test2|test3) == INSIDE)
			return INSIDE;

	// By now, we know that the triangle is not off to any side,
	// and that its sides do not penetrate the cube.  We must now
	// test for the cube intersecting the interior of the triangle.
	// We do this by looking for intersections between the cube
	// diagonals and the triangle...first finding the intersection
	// of the four diagonals with the plane of the triangle, and
	// then if that intersection is inside the cube, pursuing
	// whether the intersection point is inside the triangle itself.

	// To find plane of the triangle, first perform crossproduct on
	// two triangle side vectors to compute the normal vector.
   Vector3 vect12 = t.v1 - t.v2;
   Vector3 vect13 = t.v1 - t.v3;
	Vector3 norm = Vector3::Cross(vect12, vect13);

	// The normal vector "norm" X,Y,Z components are the coefficients
	// of the triangles AX + BY + CZ + D = 0 plane equation.  If we
	// solve the plane equation for X=Y=Z (a diagonal), we get
	// -D/(A+B+C) as a metric of the distance from cube center to the
	// diagonal/plane intersection.  If this is between -0.5 and 0.5,
	// the intersection is inside the cube.  If so, we continue by
	// doing a point/triangle intersection.
	// Do this for all four diagonals.
	float d = norm.X() * t.v1.X() + norm.Y() * t.v1.Y() + norm.Z() * t.v1.Z();

	Point hitpp, hitpn, hitnp, hitnn;

   hitpp.x = hitpp.y = hitpp.z = d / (norm.X() + norm.Y() + norm.Z());
   if (Math::Abs(hitpp.x) <= 0.5f)
      if (PointTriangleIntersection(hitpp,t) == INSIDE)
			return INSIDE;

   hitpn.z = -(hitpn.x = hitpn.y = d / (norm.X() + norm.Y() - norm.Z()));
   if (Math::Abs(hitpn.x) <= 0.5f)
      if (PointTriangleIntersection(hitpn,t) == INSIDE)
			return INSIDE;

   hitnp.y = -(hitnp.x = hitnp.z = d / (norm.X() - norm.Y() + norm.Z()));
   if (Math::Abs(hitnp.x) <= 0.5f)
      if (PointTriangleIntersection(hitnp,t) == INSIDE)
			return INSIDE;

   hitnn.y = hitnn.z = -(hitnn.x = d / (norm.X() - norm.Y() - norm.Z()));
   if (Math::Abs(hitnn.x) <= 0.5f)
      if (PointTriangleIntersection(hitnn,t) == INSIDE)
			return INSIDE;

	// No edge touched the cube; no cube diagonal touched the triangle.
	// We're done...there was no intersection.
   return OUTSIDE;
}
