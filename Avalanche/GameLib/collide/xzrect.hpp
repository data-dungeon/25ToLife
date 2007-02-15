/*************************************************************************/
// A really simple XZRect class
/*************************************************************************/

/*************************************************************************/
// The set
/*************************************************************************/
inline void XZRect::Set(
float newMinX,
float newMinZ,
float newMaxX,
float newMaxZ)
{
	minx = newMinX;
	minz = newMinZ;
	maxx = newMaxX;
	maxz = newMaxZ;
}

/*************************************************************************/
// Set from chord
/*************************************************************************/
inline void XZRect::Set(
Vector3CRef i_start,
Vector3CRef i_chord)
{
	if (i_chord.x() >= 0.0f)
	{
		minx = i_start.x();
		maxx = i_start.x() + i_chord.x();
	}
	else
	{
		minx = i_start.x() + i_chord.x();
		maxx = i_start.x();
	}
	if (i_chord.z() >= 0.0f)
	{
		minz = i_start.z();
		maxz = i_start.z() + i_chord.z();
	}
	else
	{
		minz = i_start.z() + i_chord.z();
		maxz = i_start.z();
	}
}

/*************************************************************************/
/*************************************************************************/
inline XZRect::XZRect(float minx, float minz, float maxx, float maxz)
{
	Set(minx, minz, maxx, maxz);
}

/*************************************************************************/
/*************************************************************************/
inline XZRect::XZRect(Vector3CRef i_start, Vector3CRef i_chord)
{
	Set(i_start, i_chord);
}

/*************************************************************************/
// Make sure minx < maxx, minz < maxz
/*************************************************************************/
inline void XZRect::Normalize(void)
{
	if (minx > maxx)
		Swap(minx, maxx);
	if (minz > maxz)
		Swap(minz, maxz);
}

/*************************************************************************/
// Inflate deflate the rect
/*************************************************************************/
inline void XZRect::Inflate(
float x,
float z)
{
	minx -= x;
	maxx += x;
	minz -= z;
	maxz += z;
}

/*************************************************************************/
// Does this rectangle contain r? (only do this on normal rects)
/*************************************************************************/
inline bool XZRect::Contains(
const XZRect &r) const
{
	return(r.minx >= minx && r.maxx <= maxx &&
				r.minz >= minz && r.maxz <= maxz);
}

/*************************************************************************/
// Is this point in the rectangle?
/*************************************************************************/
inline bool XZRect::Inside(
float x,
float z) const
{
 	return(x >= minx && x <= maxx && z >= minz && z <= maxz);
}

/*************************************************************************/
// Does the rect intersect us?
/*************************************************************************/
inline bool XZRect::Intersects(const XZRect &r) const
{
	return(maxx >= r.minx && minx <= r.maxx && minz <= r.maxz && maxz >= r.minz);
}

/*************************************************************************/
// Does the rect intersect us?
/*************************************************************************/
inline bool XZRect::IntersectsX(const XZRect &r) const
{
	return(maxx >= r.minx && minx <= r.maxx);
}

/*************************************************************************/
// Does the rect intersect us?
/*************************************************************************/
inline bool XZRect::IntersectsZ(const XZRect &r) const
{
	return(maxz >= r.minz && minz <= r.maxz);
}

/*************************************************************************/
// Does this line cross the box?
/*************************************************************************/
inline bool XZRect::Crosses(
Vector3CRef o,
Vector3CRef b,
float &enter,
float &exit) const
{
	float invDir;

	// Is the line parallel to the x-faces.
	if (Math::DivByZero(1.0f, b.x()))
	{

		// Parallel line not between x-faces.
		if ((o.x() < minx) || (o.x() > maxx))
			return(false);

		// Parallel line between x-faces. It enters and leaves region at infinity.
		enter = -MAXFLOAT;
		exit = MINFLOAT;
	}

	// The line is not parallel to the x-faces.
	else
	{
		invDir = 1.0f / b.x();

		// Line points in positive x.
		if (invDir >= 0.0f)
		{
			enter = (minx - o.x()) * invDir;
			exit = (maxx - o.x()) * invDir;
		}

		// Line points in negative x.
		else
		{
			enter = (maxx - o.x()) * invDir;
			exit = (minx - o.x()) * invDir;
		}
	}

	// If the line is parallel to the z-faces, the x-face entry and exit points
	// are correct if the line is between the z-faces. If not between the
	// z-faces, return false.
	if (Math::DivByZero(1.0f, b.z()))
		return((o.z() >= minz) && (o.z() <= maxz));

	// Find the z-face entry and exit points.
	float enterZ, exitZ;
	invDir = 1.0f / b.z();

	// Line points in positive z.
	if (invDir >= 0.0f)
	{
		enterZ = (minz - o.z()) * invDir;
		exitZ = (maxz - o.z()) * invDir;
	}

	// Line points in negative z.
	else
	{
		enterZ = (maxz - o.z()) * invDir;
		exitZ = (minz - o.z()) * invDir;
	}

	// Fail if the x- and z-regions do not overlap.
	if ((enterZ > exit) || (exitZ < enter))
		return(false);

	// Use the latest entry point and earliest exit point.
	if (enterZ > enter)
		enter = enterZ;
	if (exitZ < exit)
		exit = exitZ;

	return(true);
}

/*************************************************************************/
// Does this line cross the box?
/*************************************************************************/
inline bool XZRect::Touches(
const SemiSegment &s) const
{
	 // Put the semi-segment into rect space
	const float cx = s.m_center.X() - CenterX();
	const float cz = s.m_center.Z() - CenterZ();

	// Compute the abs vector of the half vector
	const float vx = Math::Abs(s.m_half.X());
	const float vz = Math::Abs(s.m_half.Z());

	const float halfX = Width() * 0.5f;
	const float halfZ = Height() * 0.5f;

	// Do the separating axis test on the primary axes
	if (Math::Abs(cx) > (vx + halfX))
		return false;
	if (Math::Abs(cz) > (vz + halfZ))
		return false;

	// Do the cross products
	if (Math::Abs(cx * s.m_half.z() - cz * s.m_half.x()) > (halfX * vz + halfZ * vx))
		return false;

	// We are overlapping
	return true;
}

/*************************************************************************/
// Clipping
/*************************************************************************/
inline void XZRect::Clip(
const XZRect &clipBy)
{
   if (maxx < clipBy.minx || minx > clipBy.maxx || minz > clipBy.maxz || maxz < clipBy.minz)
      Clear();
   else
	{
		minx = Math::Max(minx, clipBy.minx);
		maxx = Math::Min(maxx, clipBy.maxx);
		minz = Math::Max(minz, clipBy.minz);
		maxz = Math::Min(maxz, clipBy.maxz);
	}
}

/*************************************************************************/
// Operators - mainly to clean up the header file...
/*************************************************************************/
inline int XZRect::operator==(
const XZRect &r)
{
   return(r.minx == minx && r.maxx == maxx &&
          r.minz == minz && r.maxz == maxz);
}

/*************************************************************************/
/*************************************************************************/
inline int XZRect::operator!=(
const XZRect &r)
{
   return(r.minx != minx || r.maxx != maxx ||
          r.minz != minz || r.maxz != maxz);
}

/*************************************************************************/
/*************************************************************************/
inline XZRect &XZRect::operator=(
const XZRect &r)
{
	Set(r.minx, r.minz, r.maxx, r.maxz);
	return(*this);
}

/*************************************************************************/
// Find the union
/*************************************************************************/
inline XZRect &XZRect::operator+=(
const XZRect &r)
{
   minx = Math::Min(minx, r.minx);
   minz = Math::Min(minz, r.minz);
   maxx = Math::Max(maxx, r.maxx);
   maxz = Math::Max(maxz, r.maxz);

   return(*this);
}

/*************************************************************************/
// Find the union
/*************************************************************************/
inline XZRect XZRect::operator+(
const XZRect &r)
{
   XZRect temp = *this;

   temp += r;
   return(temp);
}

/*************************************************************************/
// Find the intersection
/*************************************************************************/
inline XZRect &XZRect::operator-=(
const XZRect &r)
{
	// Do the rects even intersect?
	if (maxx < r.minx || minx > r.maxx || minz > r.maxz || maxz < r.minz)
		Clear();
	else
	{
		// Do the other work
		minx = Math::Max(minx, r.minx);
		minz = Math::Max(minz, r.minz);
		maxx = Math::Min(maxx, r.maxx);
		maxz = Math::Min(maxz, r.maxz);
	}

	return(*this);
}

/*************************************************************************/
// Find the intersection
/*************************************************************************/
inline XZRect XZRect::operator-(
const XZRect &r)
{
   XZRect temp = *this;

   temp -= r;
   return(temp);
}

/*************************************************************************/
// Sort a and b into min/max
/*************************************************************************/
inline void XZRect::Sort(
float a,
float b,
float &smaller,
float &bigger)
{
	if (a < b)
	{
		smaller = a;
		bigger = b;
	}
	else
	{
		smaller = b;
		bigger = a;
	}
}


