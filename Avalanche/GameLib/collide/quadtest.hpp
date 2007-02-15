/***************************************************************************/
// Implementation file for the quad stuff
/***************************************************************************/

/***************************************************************************/
// Return the normal for the quad
/***************************************************************************/
inline Vector3 QuadTest::Normal(
Vector3CRef v0,
Vector3CRef v1,
Vector3CRef v2,
Vector3CRef v3)
{
	// Shorthand
	Vector3 normal = Vector3::Cross(v2 - v1, v0 - v1);
	normal.SafeNormalize();

	return normal;
}

/***************************************************************************/
// Given the quad v0, v1, v2, v3 and the co-planar point p, is p
// inside the quad?  If p is on an edge, it counts
/***************************************************************************/
inline bool QuadTest::PointInside(
Vector3CRef p,
Vector3CRef v0,
Vector3CRef v1,
Vector3CRef v2,
Vector3CRef v3)
{
	Vector3 normal = Normal(v0, v1, v2, v3);
	return(PointInside(p, v0, v1, v2, v3, normal));
}

/***************************************************************************/
// Given the quad v0, v1, v2, v3, and the co-planar point p, is p
// inside the quad?  If p is on an edge, it counts.
// If you don't specify a normal, one will have to be computed
/***************************************************************************/
inline bool QuadTest::PointInside(
Vector3CRef p,
Vector3CRef v0,
Vector3CRef v1,
Vector3CRef v2,
Vector3CRef v3,
Vector3CRef normal)
{
   // Which plane is best to project into?
   if (normal.X() > normal.Y() && normal.X() > normal.Z())
      return(PointInsideYZ(p, v0, v1, v2, v3));
   else if (normal.Y() > normal.X() && normal.Y() > normal.Z())
      return(PointInsideXZ(p, v0, v1, v2, v3));
   else
      return(PointInsideXY(p, v0, v1, v2, v3));
}

/***************************************************************************/
// Return true if the point p is in the quad defined by v1, v[2], v[3]
// Right now I am assuming p is co-planar with the quad.
// This test works by projecting the quad into a plane defined by
// two coordinate axes
/***************************************************************************/
inline bool QuadTest::PointInsideXY(
Vector3CRef p,
Vector3CRef v0,
Vector3CRef v1,
Vector3CRef v2,
Vector3CRef v3)
{
	// Do the tests
	SIGN s[4];
   s[0] = Sign(SideOfLine(p.X(), p.Y(), v0.X(), v0.Y(), v1.X(), v1.Y()));
   s[1] = Sign(SideOfLine(p.X(), p.Y(), v1.X(), v1.Y(), v2.X(), v2.Y()));
   s[2] = Sign(SideOfLine(p.X(), p.Y(), v2.X(), v2.Y(), v3.X(), v3.Y()));
   s[3] = Sign(SideOfLine(p.X(), p.Y(), v3.X(), v3.Y(), v0.X(), v0.Y()));

   // If they are all the same sign or 0's, it's okay
   return(SignTest(s));
}

/***************************************************************************/
// Return true if the point p is in the quad defined by v1, v[2], v[3]
// Right now I am assuming p is co-planar with the quad.
// This test works by projecting the quad into a plane defined by
// two coordinate axes
/***************************************************************************/
inline bool QuadTest::PointInsideYZ(
Vector3CRef p,
Vector3CRef v0,
Vector3CRef v1,
Vector3CRef v2,
Vector3CRef v3)
{
	// Do the tests
	SIGN s[4];
   s[0] = Sign(SideOfLine(p.Y(), p.Z(), v0.Y(), v0.Z(), v1.Y(), v1.Z()));
   s[1] = Sign(SideOfLine(p.Y(), p.Z(), v1.Y(), v1.Z(), v2.Y(), v2.Z()));
   s[2] = Sign(SideOfLine(p.Y(), p.Z(), v2.Y(), v2.Z(), v3.Y(), v3.Z()));
   s[3] = Sign(SideOfLine(p.Y(), p.Z(), v3.Y(), v3.Z(), v0.Y(), v0.Z()));

   // If they are all the same sign or 0's, it's okay
   return(SignTest(s));
}

/***************************************************************************/
// Return true if the point p is in the quad defined by v1, v[2], v[3]
// Right now I am assuming p is co-planar with the quad.
// This test works by projecting the quad into a plane defined by
// two coordinate axes
/***************************************************************************/
inline bool QuadTest::PointInsideXZ(
Vector3CRef p,
Vector3CRef v0,
Vector3CRef v1,
Vector3CRef v2,
Vector3CRef v3)
{
	// Do the tests
	SIGN s[4];
   s[0] = Sign(SideOfLine(p.X(), p.Z(), v0.X(), v0.Z(), v1.X(), v1.Z()));
   s[1] = Sign(SideOfLine(p.X(), p.Z(), v1.X(), v1.Z(), v2.X(), v2.Z()));
   s[2] = Sign(SideOfLine(p.X(), p.Z(), v2.X(), v2.Z(), v3.X(), v3.Z()));
   s[3] = Sign(SideOfLine(p.X(), p.Z(), v3.X(), v3.Z(), v0.X(), v0.Z()));

   // If they are all the same sign or 0's, it's okay
   return(SignTest(s));
}

/***************************************************************************/
// Do the test
/***************************************************************************/
inline bool QuadTest::SignTest(
SIGN s[4])
{
	register uint neg = 0;
	register uint pos = 0;

	// test each sign
 	if (s[0] == SIGN_NEG) ++neg; else if (s[0] == SIGN_POS) ++pos;
 	if (s[1] == SIGN_NEG) ++neg; else if (s[1] == SIGN_POS) ++pos;
 	if (s[2] == SIGN_NEG) ++neg; else if (s[2] == SIGN_POS) ++pos;
 	if (s[3] == SIGN_NEG) ++neg; else if (s[3] == SIGN_POS) ++pos;

	// If we have a neg and pos we are out
	return(neg == 0 || pos == 0);
}
