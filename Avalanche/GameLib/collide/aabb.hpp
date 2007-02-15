/*************************************6**************************************/
// Fast axis aligned bounding box stuff
/***************************************************************************/

/***************************************************************************/
// Access the data
/***************************************************************************/
inline void AABB::Set(
Vector3CRef newMinV,
Vector3CRef newMaxV)
{
	minV = newMinV;
	maxV = newMaxV;
}

/***************************************************************************/
// Dunno if these corners are normalized
/***************************************************************************/
inline void AABB::SetNormalized(
Vector3CRef a,
Vector3CRef b)
{
	minV = Vector3::Min(a,b);
	maxV = Vector3::Max(a,b);
}

/***************************************************************************/
/***************************************************************************/
inline void AABB::MakeCube(
Vector3CRef center,
float size)
{
	size *= 0.5f;

	minV = center - size;
	maxV = center + size;
}

/***************************************************************************/
// Set a center and halfsize
/***************************************************************************/
inline void AABB::CenterHalfSize(
Vector3CRef center, 
Vector3CRef halfSize)
{
	minV = center - halfSize;
	maxV = center + halfSize;
}

/***************************************************************************/
// From a sphere
/***************************************************************************/
inline void AABB::FromSphere(
const Vector3 &center,
float radius)
{
	minV = center - radius;
	maxV = center + radius;
}

/***************************************************************************/
// Clear this to an empty AABB
/***************************************************************************/
inline void AABB::Clear(void)
{
	minV.Clear();
	maxV.Clear();
}

/***************************************************************************/
// Normalize the box (make sure min is min and max is max)
/***************************************************************************/
inline void AABB::Normalize(void)
{
	Vector3 min = Vector3::Min(minV,maxV);
	Vector3 max = Vector3::Max(minV,maxV);
	minV = min;
	maxV = max;
}

/***************************************************************************/
// Expand by a point
/***************************************************************************/
inline void AABB::ExpandByPoint(
const Vector3 &p)
{
	minV = Vector3::Min(p, minV);
	maxV = Vector3::Max(p, maxV);
}

/***************************************************************************/
// The volume
/***************************************************************************/
inline float AABB::Volume() const
{
	Vector3 size = Size();
	
	Vector3 abs = size.Abs();
	return abs.x() * abs.y() * abs.z();
}

/***************************************************************************/
// Make this AABB into a cube using the largest dimension
/***************************************************************************/
inline void AABB::FitCube()
{
	// figure out the widest dimension x, y, z
	Vector3 size = Size();
	float width;

	// x bigger than y?
	if (size.x() > size.y())
	{
		if (size.x() > size.z())
			width = size.x();
		else
			width = size.z();
	}
	else // y is bigger than x!
	{
		if (size.y() > size.z())
			width = size.y();
		else
			width = size.z();
	}

	// Now do the half width
	float halfWidth = width * 0.5f;

	// Keep same center, now use halfwidth all around
	Vector3 center = Center();
	minV = center - halfWidth;
	maxV = center + halfWidth;
}

/***************************************************************************/
/***************************************************************************/
inline void AABB::Inflate(
const Vector3 &inflate)
{
	minV -= inflate;
	maxV += inflate;
}

/***************************************************************************/
// Get the XZ Rect
/***************************************************************************/
inline XZRect AABB::ToRect(void)
{
	XZRect rect(minV.x(), minV.z(), maxV.x(), maxV.z());
	return rect;
}

/***************************************************************************/
// Point in box?
/***************************************************************************/
inline bool AABB::PointIn(
Vector3CRef p) const
{
	return(p.x() >= minV.x() && p.x() <= maxV.x() &&
				p.y() >= minV.y() && p.y() <= maxV.y() &&
				p.z() >= minV.z() && p.z() <= maxV.z());
}

/******************************************************************************/
// This routine takes a projection of a line segment onto one of the axes of the
// box and determines if the line segment penetrates either of the faces of the
// box. The axis is specified by i_axis which can have values of 0, 2, and 4 for
// the x, y, and z axes respectively.  The value of i_axis is used for the id of
// negative faces, i.e. 0, 2, and 4 for the -x, -y, and -z axes.  The id for a
// positive face is generated by (uint)i_axis | 1 which produces 1, 3, and 5 for
// the +x, +y, and +z faces.
/******************************************************************************/
inline bool AABB::FindPlanePenetration(float i_negOrigin, float i_basis,
		float i_face, EPlaneAxis i_axis, uint &io_entryFace, float &io_entryPoint,
		uint &io_exitFace, float &io_exitPoint)
{
/*
 *	The basis vector is not parallel to the face planes.
 */
	if (!Math::DivByZero(Math::Abs(i_negOrigin) + i_face, i_basis))
	{
/*
 *		Find the points where the line enters and leaves the region between
 *		the faces.
 */
		float negPoint = (i_negOrigin - i_face) / i_basis;
		float posPoint = (i_negOrigin + i_face) / i_basis;
/*
 *		Enters at the negative face side and leave at the positive.
 */
		if (negPoint <= posPoint)
		{
/*
 *			If the portion of the line that passes through this region does not
 *			overlap any of the previously tested regions, the line does not
 *			intersect the box.
 */
			if ((negPoint > io_exitPoint) || (posPoint < io_entryPoint))
				return(false);
/*
 *			Check for a later entry point into the overlapping portion.
 */
			if (negPoint > io_entryPoint)
			{
				io_entryFace = (uint)i_axis;
				io_entryPoint = negPoint;
			}
/*
 *			Check for an earlier exit point from the overlapping portion.
 */
			if (posPoint < io_exitPoint)
			{
				io_exitFace = (uint)i_axis | 1;
				io_exitPoint = posPoint;
			}
		}
/*
 *		Enters at the positive face side and leave at the negative.
 */
		else
		{
/*
 *			If the portion of the line that passes through this region does not
 *			overlap any of the previously tested regions, the line does not
 *			intersect the box.
 */
			if ((posPoint > io_exitPoint) || (negPoint < io_entryPoint))
				return(false);
/*
 *			Check for a later entry point into the overlapping portion.
 */
			if (posPoint > io_entryPoint)
			{
				io_entryFace = (uint)i_axis | 1;
				io_entryPoint = posPoint;
			}
/*
 *			Check for an earlier exit point from the overlapping portion.
 */
			if (negPoint < io_exitPoint)
			{
				io_exitFace = (uint)i_axis;
				io_exitPoint = negPoint;
			}
		}
	}
/*
 *	The basis vector is parallel to the face planes. If the origin of the line
 *	is not between the faces, the line never enters the region between them and
 *	does not intersect the AABB. If the origin is between the faces, the line
 *	enters and leaves the region at the same points it enters and leaves world
 *	space so no extra analysis needs to take place.
 */
	else if (Math::Abs(i_negOrigin) > i_face)
		return(false);

	return(true);
}

/***************************************************************************/
// Find the points, if any, where a line intersects the AABB.
/***************************************************************************/
inline bool AABB::IntersectLine(Vector3CRef i_origin,
		Vector3CRef i_basis, uint &o_entryFace, float &o_entryPoint,
		uint &o_exitFace, float &o_exitPoint) const
{
/*
 *	The line is expressed as a one-dimensional coordinate system with an
 *	origin and basis vector and the entry and exit points are expressed in
 *	terms of the basis vector.
 *
 *	The world space contains 4 regions. The first is the entire space and the
 *	other 3 are the regions between each of the parallel faces of the AABB. If
 *	the line passes through all 4 regions simultainiously, the line intersects
 *	the AABB.
 *
 *	The line enter the first region, world space, at negative infinity and leaves
 *	at positive infinity.
 */
	o_entryPoint = -MAXFLOAT;
	o_exitPoint = MAXFLOAT;
/*
 *	Check the x-faces.
 */
	if (!FindPlanePenetration(-i_origin.x(), i_basis.x(), maxV.x(), PLANE_AXIS_X,
			o_entryFace, o_entryPoint, o_exitFace, o_exitPoint))
		return(false);
/*
 *	Check the y-faces.
 */
	if (!FindPlanePenetration(-i_origin.y(), i_basis.y(), maxV.y(), PLANE_AXIS_Y,
			o_entryFace, o_entryPoint, o_exitFace, o_exitPoint))
		return(false);
/*
 *	Check the z-faces.
 */
	if (!FindPlanePenetration(-i_origin.z(), i_basis.z(), maxV.z(), PLANE_AXIS_Z,
			o_entryFace, o_entryPoint, o_exitFace, o_exitPoint))
		return(false);
/*
 *	Success.
 */
	return(true);
}

/***************************************************************************/
// Segment crosses box
/***************************************************************************/
inline bool AABB::SegmentCrosses(
Vector3 &start,
Vector3 &end)
{
	// Make the sgement into a semi segment
	Vector3 midPoint = (start + end) * 0.5f;
	Vector3 w = end - midPoint;

	// Compute the abs vector of the half vector
	const Vector3 v = w.Abs();

	// Put the semi-segment into AABB space
	const Vector3 aabbCenter = Center();
	const Vector3 c = midPoint - aabbCenter;

	// Figure out the halfsize
	const Vector3 h = maxV - aabbCenter;

	// Do the separating axis test on the primary axes
	Vector3 absc = c.Abs();
	Vector3 sum = v + h;
	if (absc.x() > sum.x())
		return false;
	if (absc.y() > sum.y())
		return false;
	if (absc.z() > sum.z())
		return false;

	// Do the cross products
#if 0
	Vector3 cwcross = c.Cross(w);
	Vector3 hvcross = h.Cross(v);
//	if (Math::Abs(c.y * w.z - c.z * w.y) > h.y * v.z + h.z * v.y)
//		return false;
//	if (Math::Abs(c.x * w.z - c.z * w.x) > h.x * v.z + h.z * v.x)
//		return false;
//	if (Math::Abs(c.x * w.y - c.y * w.x) > h.x * v.y + h.y * v.x)
//		return false;
	if(cwcross.x() > hvcross.x() ||
		cwcross.y() > hvcross.y() ||
		cwcross.z() > hvcross.z())
		return false;
#else
	Vector3 abscwcross = c.Cross(w).Abs();
	if (abscwcross.X() > (h.Y() * v.Z() + h.Z() * v.Y()))
		return false;
	if (abscwcross.Y() > (h.X() * v.Z() + h.Z() * v.X()))
		return false;
	if (abscwcross.Z() > (h.X() * v.Y() + h.Y() * v.X()))
		return false;
#endif

	// We are overlapping
	return true;
}

/***************************************************************************/
// Does b intersect us in any way?
/***************************************************************************/
inline bool AABB::Intersects(
const AABB &test) const
{
	return (test.maxV.x() >= minV.x() && test.minV.x() <= maxV.x() &&
				test.maxV.y() >= minV.y() && test.minV.y() <= maxV.y() &&
				test.maxV.z() >= minV.z() && test.minV.z() <= maxV.z());
}

/*************************************************************************/
// Is b contained by us?
/*************************************************************************/
inline bool AABB::Contains(
const AABB &test) const
{
	return (test.minV.x() >= minV.x() && test.maxV.x() <= maxV.x() &&
				test.minV.y() >= minV.y() && test.maxV.y() <= maxV.y() &&
				test.minV.z() >= minV.z() && test.maxV.z() <= maxV.z());
}

/*************************************************************************/
// Does this triangle touch us?
/*************************************************************************/

#define CROSS_BUFFER  1.0e-4f   // Catch parallel edges.

inline bool AABB::TriangleTouches(
const Vector3 &i_normal,
const Vector3 &i_v0,
const Vector3 &i_v1,
const Vector3 &i_v2) const
{
	// Check faces.
	if (((i_v0.x() > maxV.x()) && (i_v1.x() > maxV.x()) &&
			(i_v2.x() > maxV.x())) || ((i_v0.x() < minV.x()) &&
			(i_v1.x() < minV.x()) && (i_v2.x() < minV.x())))
		return(false);

	if (((i_v0.y() > maxV.y()) && (i_v1.y() > maxV.y()) &&
			(i_v2.y() > maxV.y())) || ((i_v0.y() < minV.y()) &&
			(i_v1.y() < minV.y()) && (i_v2.y() < minV.y())))
		return(false);

	if (((i_v0.z() > maxV.z()) && (i_v1.z() > maxV.z()) &&
			(i_v2.z() > maxV.z())) || ((i_v0.z() < minV.z()) &&
			(i_v1.z() < minV.z()) && (i_v2.z() < minV.z())))
		return(false);

 	//	Transform into a system whose origin is at the center of the aabb.
	Vector3 center = 0.5f * (minV + maxV);
	Vector3 extent = 0.5f * (maxV - minV);
	Vector3 v0 = i_v0 - center;
	Vector3 v1 = i_v1 - center;
	Vector3 v2 = i_v2 - center;

	// Check triangle face.
	if (Math::Abs(i_normal.Dot(v0)) > extent.Dot(Vector3::Abs(i_normal)))
		return(false);

 	// Now check the cross products of the edges.
	Vector3 crossTerm0 = v1.Cross(v0);
	Vector3 crossTerm1 = v2.Cross(v1);
	Vector3 crossTerm2 = v0.Cross(v2);

	// Edge 0.
	Vector3 edge = Vector3::Abs(v1 - v0);
	Vector3 crossSum = crossTerm1 + crossTerm2;

	//	X-axis cross edge 0.
	float aabbDist = extent.Y() * edge.Z() + extent.Z() * edge.Y() + CROSS_BUFFER;
	if (((crossTerm0.X() > aabbDist) && (crossSum.X() < -aabbDist)) ||
			((crossTerm0.X() < -aabbDist) && (crossSum.X() > aabbDist)))
		return(false);

	// Y-axis cross edge 0.
	aabbDist = extent.X() * edge.Z() + extent.Z() * edge.X() + CROSS_BUFFER;
	if (((crossTerm0.Y() > aabbDist) && (crossSum.Y() < -aabbDist)) ||
			((crossTerm0.Y() < -aabbDist) && (crossSum.Y() > aabbDist)))
		return(false);

	// Z-axis cross edge 0.
	aabbDist = extent.X() * edge.Y() + extent.Y() * edge.X() + CROSS_BUFFER;
	if (((crossTerm0.Z() > aabbDist) && (crossSum.Z() < -aabbDist)) ||
			((crossTerm0.Z() < -aabbDist) && (crossSum.Z() > aabbDist)))
		return(false);

	// Edge 1.
	edge = Vector3::Abs(v2 - v1);
	crossSum = crossTerm0 + crossTerm2;

	// X-axis cross edge 1.
	aabbDist = extent.Y() * edge.Z() + extent.Z() * edge.Y() + CROSS_BUFFER;
	if (((crossTerm1.X() > aabbDist) && (crossSum.X() < -aabbDist)) ||
			((crossTerm1.X() < -aabbDist) && (crossSum.X() > aabbDist)))
		return(false);

	// Y-axis cross edge 1.
	aabbDist = extent.X() * edge.Z() + extent.Z() * edge.X() + CROSS_BUFFER;
	if (((crossTerm1.Y() > aabbDist) && (crossSum.Y() < -aabbDist)) ||
			((crossTerm1.Y() < -aabbDist) && (crossSum.Y() > aabbDist)))
		return(false);

	// Z-axis cross edge 1.
	aabbDist = extent.X() * edge.Y() + extent.Y() * edge.X() + CROSS_BUFFER;
	if (((crossTerm1.Z() > aabbDist) && (crossSum.Z() < -aabbDist)) ||
			((crossTerm1.Z() < -aabbDist) && (crossSum.Z() > aabbDist)))
		return(false);

	// Edge 2.
	edge = Vector3::Abs(v0 - v2);
	crossSum = crossTerm0 + crossTerm1;

	// X-axis cross edge 2.
	aabbDist = extent.Y() * edge.Z() + extent.Z() * edge.Y() + CROSS_BUFFER;
	if (((crossTerm2.X() > aabbDist) && (crossSum.X() < -aabbDist)) ||
			((crossTerm2.X() < -aabbDist) && (crossSum.X() > aabbDist)))
		return(false);

	// Y-axis cross edge 2.
	aabbDist = extent.X() * edge.Z() + extent.Z() * edge.X() + CROSS_BUFFER;
	if (((crossTerm2.Y() > aabbDist) && (crossSum.Y() < -aabbDist)) ||
			((crossTerm2.Y() < -aabbDist) && (crossSum.Y() > aabbDist)))
		return(false);

	// Z-axis cross edge 2.
	aabbDist = extent.X() * edge.Y() + extent.Y() * edge.X() + CROSS_BUFFER;
	if (((crossTerm2.Z() > aabbDist) && (crossSum.Z() < -aabbDist)) ||
			((crossTerm2.Z() < -aabbDist) && (crossSum.Z() > aabbDist)))
		return(false);

	return(true);
}

/*************************************************************************/
/*************************************************************************/
inline AABB &AABB::operator=(
const AABB &aabb)
{
	minV = aabb.minV;
	maxV = aabb.maxV;

	return(*this);
}

/*************************************************************************/
// Find the union
/*************************************************************************/
inline AABB &AABB::operator+=(
const AABB &aabb)
{
	minV = Vector3::Min(minV,aabb.minV);
	maxV = Vector3::Max(maxV,aabb.maxV);

   return(*this);
}

/*************************************************************************/
// Find the union
/*************************************************************************/
inline AABB AABB::operator+(
const AABB &aabb)
{
   AABB temp = *this;

   temp += aabb;
   return(temp);
}

/*************************************************************************/
// Find the intersection
/*************************************************************************/
inline AABB &AABB::operator-=(
const AABB &aabb)
{
	// Do the rects even intersect?
	if (maxV.x() < aabb.minV.x() || minV.x() > aabb.maxV.x() ||
			maxV.y() < aabb.minV.y() || minV.y() > aabb.maxV.y() ||
			maxV.z() < aabb.minV.z() || minV.z() > aabb.maxV.z())
		Clear();
	else
	{
		// Do the other work
		minV = Vector3::Max(minV,aabb.minV);
		maxV = Vector3::Min(maxV,aabb.maxV);
	}

	return(*this);
}

/*************************************************************************/
// Find the intersection
/*************************************************************************/
inline AABB AABB::operator-(
const AABB &aabb)
{
   AABB temp = *this;

   temp -= aabb;
   return(temp);
}

/***************************************************************************/
/***************************************************************************/
inline Vector3 AABB::GetFaceNormal(
uint face)
{
	switch(face)
	{
	 case 0:
		return Vector3(-1.0f, 0.0f, 0.0f);
	 case 1:
		return Vector3(1.0f, 0.0f, 0.0f);
	 case 2:
		return Vector3(0.0f, -1.0f, 0.0f);
	 case 3:
		return Vector3(0.0f, 1.0f, 0.0f);
	 case 4:
		return Vector3(0.0f, 0.0f, -1.0f);
	 case 5:
		return Vector3(0.0f, 0.0f, 1.0f);
	 default:
		ASSERT(false);
		return Vector3(0.0f, 1.0f, 0.0f);
	}
}

/***************************************************************************/
// Get a normal
/***************************************************************************/
inline void AABB::GetFaceNormal(
uint face,
Vector3 &normal)
{
	normal = GetFaceNormal(face);
}

/***************************************************************************/
// Make the c struct into the class
/***************************************************************************/
inline AABB &ToAABB(
ts_BoundingBox &bb)
{
	return(*((AABB *)&bb));
}

/***************************************************************************/
// Transform the box
/***************************************************************************/
inline AABB operator*(
AABB &b,
DirCos3x3 &m)
{
	AABB temp;

	temp.Set(b.minV * m, b.maxV * m);
	temp.Normalize();
	return(temp);
}

/***************************************************************************/
/***************************************************************************/
inline AABB operator*(
AABB &b,
Cartesian4x4 &m)
{
	AABB temp;

	temp.Set(b.minV * m, b.maxV * m);
	temp.Normalize();
	return(temp);
}

/***************************************************************************/
// Transform by the transpose
/***************************************************************************/
inline AABB operator/(
AABB &b,
DirCos3x3 &m)
{
	AABB temp;

	temp.Set(b.minV / m, b.maxV / m);
	temp.Normalize();
	return(temp);
}
