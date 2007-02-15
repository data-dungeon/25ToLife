/***************************************************************************/
// Implementation file for the chord stuff
/***************************************************************************/

/***************************************************************************/
// Do these two chords intersect?
/***************************************************************************/
inline bool ChordIntersect::IntersectXY(
Vector3 &a,
Vector3 &b,
Vector3 &i,
Vector3 &j)
{
	int test1 = Sign(SideOfLineXY(a, i, j)) + 1;
	int test2 = Sign(SideOfLineXY(b, i, j)) + 1;

	if (testCrossGrid[test1][test2])
	{
		int test3 = Sign(SideOfLineXY(i, a, b)) + 1;
		int test4 = Sign(SideOfLineXY(j, a, b)) + 1;

		return(testCrossGrid[test3][test4]);
	}

	return(false);
}

/***************************************************************************/
// Do these two chords intersect?
/***************************************************************************/
inline bool ChordIntersect::IntersectXZ(
Vector3 &a,
Vector3 &b,
Vector3 &i,
Vector3 &j)
{
	int test1 = Sign(SideOfLineXZ(a, i, j)) + 1;
	int test2 = Sign(SideOfLineXZ(b, i, j)) + 1;

	if (testCrossGrid[test1][test2])
	{
		int test3 = Sign(SideOfLineXZ(i, a, b)) + 1;
		int test4 = Sign(SideOfLineXZ(j, a, b)) + 1;

		return(testCrossGrid[test3][test4]);
	}

	return(false);
}

/***************************************************************************/
// Do these two chords intersect?
/***************************************************************************/
inline bool ChordIntersect::IntersectYZ(
Vector3 &a,
Vector3 &b,
Vector3 &i,
Vector3 &j)
{
	int test1 = Sign(SideOfLineYZ(a, i, j)) + 1;
	int test2 = Sign(SideOfLineYZ(b, i, j)) + 1;

	if (testCrossGrid[test1][test2])
	{
		int test3 = Sign(SideOfLineYZ(i, a, b)) + 1;
		int test4 = Sign(SideOfLineYZ(j, a, b)) + 1;

		return(testCrossGrid[test3][test4]);
	}

	return(false);
}

/***************************************************************************/
// Solve for the intersection of the chords formed by these sets of points
// (v1-v2) (v3-v4) and return in i.
//
// You must be sure that the chords intersect.
/***************************************************************************/
inline void ChordIntersect::IntersectionXZ(
Vector3 &v1,
Vector3 &v2,
Vector3 &v3,
Vector3 &v4,
Vector3 &i)
{
	float r;

	// calculate ratio in segment to determine intersection location
	r = ((v1.z() - v3.z()) * (v4.x() - v3.x()) - (v1.x() - v3.x()) * (v4.z() - v3.z())) /
		 ((v2.x() - v1.x()) * (v4.z() - v3.z()) - (v2.z() - v1.z()) * (v4.x() - v3.x()));

	i.Set(v1.x() + (v2.x() - v1.x()) * r,
			0.0f,
			v1.z() + (v2.z() - v1.z()) * r);
}

/***************************************************************************/
// Return 1 it t > 0.0, -1 if t < 0.0 and 0 if t == 0.0
/***************************************************************************/
inline int ChordIntersect::Sign(float t)
{
#define ZERO_THRESHOLD	0.0001f

   if (t < -ZERO_THRESHOLD)
      return(-1);
   else if (t > ZERO_THRESHOLD)
      return(1);
   else
      return(0);
}

/***************************************************************************/
// Determine which side of the chord p is on in the X Y plane
/***************************************************************************/
inline float ChordIntersect::SideOfLineXY(
Vector3 &p,
Vector3 &v1,
Vector3 &v2)
{
   Vector3 a, b;

   a.x(v2.x() - v1.x());
   a.y(v2.y() - v1.y());

   b.x( p.x() - v2.x());
   b.y( p.y() - v2.y());

   return(a.x() * b.y() - a.y() * b.x());
}

/***************************************************************************/
// Determine which side of the chord p is on in the X Z plane
/***************************************************************************/
inline float ChordIntersect::SideOfLineXZ(
Vector3 &p,
Vector3 &v1,
Vector3 &v2)
{
   Vector3 a, b;

   a.x( v2.x() - v1.x());
   a.z( v2.z() - v1.z());

   b.x( p.x() - v2.x());
   b.z( p.z() - v2.z());

   return(a.x() * b.z() - a.z() * b.x());
}

/***************************************************************************/
// Determine which side of the chord p is on in the Y Z plane
/***************************************************************************/
inline float ChordIntersect::SideOfLineYZ(
Vector3 &p,
Vector3 &v1,
Vector3 &v2)
{
   Vector3 a, b;

   a.y( v2.y() - v1.y());
   a.z( v2.z() - v1.z());

   b.y( p.y() - v2.y());
   b.z( p.z() - v2.z());

   return(a.y() * b.z() - a.z() * b.y());
}












