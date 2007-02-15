/*************************************6**************************************/
// Fast axis aligned bounding box stuff
/***************************************************************************/

/***************************************************************************/
/***************************************************************************/
inline Cube::Cube(
const Vector3 &center,
float halfSize) :
m_center(center),
m_halfSize(halfSize)
{
}

/***************************************************************************/
// Make this AABB into a cube using the largest dimension
/***************************************************************************/
inline void Cube::Fit(
const AABB &aabb)
{
	// Equal centers
	m_center = aabb.Center();

	// What is the largest dimension?
	Vector3 d = aabb.maxV - m_center;
	m_halfSize = d.x();
	if (d.y() > m_halfSize)
		m_halfSize = d.y();
	if (d.z() > m_halfSize)
		m_halfSize = d.z();
}

/***************************************************************************/
/***************************************************************************/
inline Cube::Cube(
const AABB &aabb)
{
	Fit(aabb);
}

/***************************************************************************/
// Clear this to an empty AABB
/***************************************************************************/
inline void Cube::Clear(void)
{
	m_center.Clear();
	m_halfSize = 0.0f;
}

/***************************************************************************/
/***************************************************************************/
inline void Cube::Set(
const Vector3 &center,
float halfSize)
{
	m_center = center;
	m_halfSize = halfSize;
}

/***************************************************************************/
// Expand by a point
/***************************************************************************/
inline void Cube::ExpandByPoint(
const Vector3 &p)
{
	Vector3 d = (p - m_center).Abs();

	if (d.x() > m_halfSize)
		m_halfSize = d.x();
	if (d.y() > m_halfSize)
		m_halfSize = d.y();
	if (d.z() > m_halfSize)
		m_halfSize = d.z();
}

/***************************************************************************/
// Get at the center
/***************************************************************************/
inline Vector3 Cube::Center() const
{
	return m_center;
}

/***************************************************************************/
// Get at the max
/***************************************************************************/
inline Vector3 Cube::Max() const
{
	return Vector3(m_center.X() + m_halfSize,
						m_center.Y() + m_halfSize,
						m_center.Z() + m_halfSize);
}

/***************************************************************************/
// Get at the min
/***************************************************************************/
inline Vector3 Cube::Min() const
{
	return Vector3(m_center.X() - m_halfSize,
						m_center.Y() - m_halfSize,
						m_center.Z() - m_halfSize);
}

/***************************************************************************/
// Get at the sizes
/***************************************************************************/
inline float Cube::HalfSize() const
{
	return m_halfSize;
}

/***************************************************************************/
/***************************************************************************/
inline float Cube::Size() const
{
	return m_halfSize * 2.0f;
}

/***************************************************************************/
// The volume
/***************************************************************************/
inline float Cube::Volume() const
{
	return Math::Cube(m_halfSize * 2.0f);
}

/***************************************************************************/
/***************************************************************************/
inline void Cube::Inflate(
float inflate)
{
	m_halfSize += inflate;
}

/***************************************************************************/
// Point inside?
/***************************************************************************/
inline bool Cube::Inside(
const Vector3 &p) const
{
	Vector3 d = (p - m_center).Abs();

	return(d.x() <= m_halfSize && d.y() <= m_halfSize && d.z() <= m_halfSize);
}

/***************************************************************************/
// Point inside?
/***************************************************************************/
inline bool Cube::Inside(
const Vector3 &p,
float epsilon) const
{
	Vector3 d = (p - m_center).Abs();

	const float halfSize = m_halfSize + epsilon;
	return(d.x() <= halfSize && d.y() <= halfSize && d.z() <= halfSize);
}


/*************************************************************************/
/*************************************************************************/
inline bool Cube::InsideFaceXY(const Vector3 &p) const
{
	return (Math::Abs(p.x() - m_center.x()) <= m_halfSize &&
				Math::Abs(p.y() - m_center.y()) <= m_halfSize);
}

/*************************************************************************/
/*************************************************************************/
inline bool Cube::InsideFaceXZ(const Vector3 &p) const
{
	return (Math::Abs(p.x() - m_center.x()) <= m_halfSize &&
				Math::Abs(p.z() - m_center.z()) <= m_halfSize);
}

/*************************************************************************/
/*************************************************************************/
inline bool Cube::InsideFaceYZ(const Vector3 &p) const
{
	return (Math::Abs(p.y() - m_center.y()) <= m_halfSize &&
				Math::Abs(p.z() - m_center.z()) <= m_halfSize);
}

/*************************************************************************/
// Is this AABB touching this cube in any way
/*************************************************************************/
inline bool Cube::Touches(
const AABB &aabb) const
{
	return (aabb.maxV.x() >= m_center.x() - m_halfSize &&
				aabb.minV.x() <= m_center.x() + m_halfSize &&
				aabb.maxV.y() >= m_center.y() - m_halfSize &&
				aabb.minV.y() <= m_center.y() + m_halfSize &&
				aabb.maxV.z() >= m_center.z() - m_halfSize &&
				aabb.minV.z() <= m_center.z() + m_halfSize);
}

/*************************************************************************/
// Is this AABB contianed by the cube?
/*************************************************************************/
inline bool Cube::Contains(
const AABB &aabb) const
{
	const Vector3 adjMin = (aabb.minV - m_center).Abs();
	const Vector3 adjMax = (aabb.maxV - m_center).Abs();

	return (Inside(adjMin) && Inside(adjMax));
}

/*************************************************************************/
// Is this AABB contianed by the cube?
/*************************************************************************/
inline bool Cube::Contains(
const Vector3 *pList,
int count) const
{
	for (int p = 0; p < count; p++)
	{
		if (!Inside(pList[p]))
			return false;
	}

	return true;
}

/*************************************************************************/
// Is this line touching this cube?
/*************************************************************************/
inline bool Cube::Touches(
const SemiSegment &s) const
{
	// Put the semi-segment into cube space
	const Vector3 c = s.m_center - m_center;

	// Compute the abs vector of the half vector
	const Vector3 v = s.m_half.Abs();

#if 1
	// Do the separating axis test on the primary axes
	if (Math::Abs(c.x()) > v.x() + m_halfSize)
		return false;
	if (Math::Abs(c.y()) > v.y() + m_halfSize)
		return false;
	if (Math::Abs(c.z()) > v.z() + m_halfSize)
		return false;

	// Do the cross products
	if (Math::Abs(c.y() * s.m_half.z() - c.z() * s.m_half.y()) > m_halfSize * (v.z() + v.y()))
		return false;
	if (Math::Abs(c.x() * s.m_half.z() - c.z() * s.m_half.x()) > m_halfSize * (v.z() + v.x()))
		return false;
	if (Math::Abs(c.x() * s.m_half.y() - c.y() * s.m_half.x()) > m_halfSize * (v.y() + v.x()))
		return false;
#else
	Vector3 cAbs = c.Abs();
	// Do the separating axis test on the primary axes
	if (cAbs.x() > v.x() + m_halfSize)
		return false;
	if (cAbs.y() > v.y() + m_halfSize)
		return false;
	if (cAbs.z() > v.z() + m_halfSize)
		return false;

	Vector3 cross = Vector3::Cross(c, s.m_half).Abs();
	if (cross.x() > m_halfSize * (v.z() + v.y()))
		return false;
	if (cross.y() > m_halfSize * (v.z() + v.x()))
		return false;
	if (cross.z() > m_halfSize * (v.y() + v.x()))
		return false;
#endif

	// We are overlapping
	return true;
}

/************************************************************************/
/* Do this test, inflating the cube by fEpsilon (for error)             */
/************************************************************************/
inline bool Cube::Touches(
const SemiSegment &s,
float fEpsilon) const
{
	// Put the semi-segment into cube space
	const Vector3 c = s.m_center - m_center;

	// Compute the abs vector of the half vector
	const Vector3 v = s.m_half.Abs();

#if 1
	// Do the separating axis test on the primary axes
	if (Math::Abs(c.x()) > v.x() + ( m_halfSize + fEpsilon ))
		return false;
	if (Math::Abs(c.y()) > v.y() + ( m_halfSize + fEpsilon ))
		return false;
	if (Math::Abs(c.z()) > v.z() + ( m_halfSize + fEpsilon ))
		return false;

	// Do the cross products
	if (Math::Abs(c.y() * s.m_half.z() - c.z() * s.m_half.y()) > ( m_halfSize + fEpsilon ) * (v.z() + v.y()))
		return false;
	if (Math::Abs(c.x() * s.m_half.z() - c.z() * s.m_half.x()) > ( m_halfSize + fEpsilon ) * (v.z() + v.x()))
		return false;
	if (Math::Abs(c.x() * s.m_half.y() - c.y() * s.m_half.x()) > ( m_halfSize + fEpsilon ) * (v.y() + v.x()))
		return false;
#else
	Vector3 cAbs = c.Abs();
	// Do the separating axis test on the primary axes
	if (cAbs.x() > v.x() + ( m_halfSize + fEpsilon ))
		return false;
	if (cAbs.y() > v.y() + ( m_halfSize + fEpsilon ))
		return false;
	if (cAbs.z() > v.z() + ( m_halfSize + fEpsilon ))
		return false;

	Vector3 cross = Vector3::Cross(c, s.m_half).Abs();
	if (cross.x() > ( m_halfSize + fEpsilon ) * (v.z() + v.y()))
		return false;
	if (cross.y() > ( m_halfSize + fEpsilon ) * (v.z() + v.x()))
		return false;
	if (cross.z() > ( m_halfSize + fEpsilon ) * (v.y() + v.x()))
		return false;
#endif

	// We are overlapping
	return true;
}

/*************************************************************************/
// Is this line contained by the cube?
/*************************************************************************/
inline bool Cube::Contains(
const SemiSegment &s) const
{
	return (Inside(s.Start()) && Inside(s.End()));
}

/*************************************************************************/
// Does this triangle fit inside of this cube
/*************************************************************************/
inline bool Cube::Contains(
const Vector3 &a,
const Vector3 &b,
const Vector3 &c) const
{
	return (Inside(a) && Inside(b) && Inside(c));
}

/*************************************************************************/
/*************************************************************************/
inline bool Cube::Contains(
const Vector3 &a,
const Vector3 &b,
const Vector3 &c,
float epsilon) const
{
	return (Inside(a, epsilon), Inside(b, epsilon), Inside(c, epsilon));
}

/*************************************************************************/
// Convert to an AABB
/*************************************************************************/
inline AABB Cube::ToAABB() const
{
	AABB aabb;

	aabb.minV.x(m_center.x() - m_halfSize);
	aabb.minV.y(m_center.y() - m_halfSize);
	aabb.minV.z(m_center.z() - m_halfSize);

	aabb.maxV.x(m_center.x() + m_halfSize);
	aabb.maxV.y(m_center.y() + m_halfSize);
	aabb.maxV.z(m_center.z() + m_halfSize);

	return aabb;
}

/*************************************************************************/
/*************************************************************************/
inline Cube &Cube::operator=(
const Cube &c)
{
	m_center = c.m_center;
	m_halfSize = c.m_halfSize;

	return(*this);
}

/*************************************************************************/
/*************************************************************************/
inline Octant::Octant(const Vector3 &center, float halfSize)
	: Cube(center, halfSize)
{
}

/*************************************************************************/
/*************************************************************************/
inline Octant::Octant()
{
}

/*************************************************************************/
/*************************************************************************/
inline Octant::Octant(const AABB &aabb)
	: Cube(aabb)
{
}

/*************************************************************************/
// Make the bounding box for this specified child
/*************************************************************************/
inline Octant Octant::StandardChild(
OCTANT octant) const
{
	// Figure out the quarter size of m_bounds...which is half size of child
	float quarterSize = m_halfSize * 0.5f;

	// Compute the center
	Vector3 center = m_center + (s_sign[octant] * quarterSize);

	// Setup the child
	Octant child(center, quarterSize);
	return child;
}

/*************************************************************************/
/*************************************************************************/
inline Octant Octant::LooseChild(
OCTANT octant) const
{
	// Figure out the quarter size of m_bounds...which is half size of child
	float quarterSize = m_halfSize * 0.5f;

	// Compute the center
	Vector3 center = m_center + (s_sign[octant] * quarterSize * 0.5f);

	// Setup the child
	Octant child(center, quarterSize);
	return child;
}

/*************************************************************************/
// Return the bounds of the parent, given the octant ID
/*************************************************************************/
inline Octant Octant::StandardParent(
OCTANT myOctant) const
{
	Vector3 center = m_center - (s_sign[myOctant] * m_halfSize);
	Octant parent(center, m_halfSize * 2.0f);
	return parent;
}

/*************************************************************************/
// Return the bounds of the parent, given the octant ID
/*************************************************************************/
inline Octant Octant::LooseParent(
OCTANT myOctant) const
{
	// Don't use this, or write it
	Vector3 center = m_center - (s_sign[myOctant] * m_halfSize * 0.5f);
	Octant parent(center, m_halfSize * 2.0f);
	return parent;
}

/*************************************************************************/
// Make the bounding box for this specified child
/*************************************************************************/
inline Octant Octant::Child(
OCTANT octant) const
{
#ifndef LOOSEOCTTREE
	// Figure out the quarter size of m_bounds...which is half size of child
	float quarterSize = m_halfSize * 0.5f;

	// Compute the center
	Vector3 center = m_center + (s_sign[octant] * quarterSize);

	// Setup the child
	Octant child(center, quarterSize);
	return child;
#else
	// Figure out the quarter size of m_bounds...which is half size of child
	float quarterSize = m_halfSize * 0.5f;

	// Compute the center
	Vector3 center = m_center + (s_sign[octant] * quarterSize * 0.5f);

	// Setup the child
	Octant child(center, quarterSize);
	return child;
#endif
}

/*************************************************************************/
/*************************************************************************/
inline Octant Octant::Parent(
OCTANT myOctant) const
{
#ifdef LOOSEOCTTREE
	return LooseParent(myOctant);
#else
	return StandardParent(myOctant);
#endif
}

/*************************************************************************/
// What is the best octant for this point?
/*************************************************************************/
inline Octant::OCTANT Octant::BestChild(
const Vector3 &p) const
{
	int octant = 0;

	Vector3 delta = p - m_center;
	if (delta.x() < 0.0f)
		octant |= 1;
	if (delta.y() < 0.0f)
		octant |= 2;
	if (delta.z() < 0.0f)
		octant |= 4;

	return (OCTANT)octant;
}

/*************************************************************************/
// What is the size of an octant n levels below myself
/*************************************************************************/
inline float Octant::SizeAtDepth(
int depth)
{
	float childSize = m_halfSize / (float)(1 << depth);
	return childSize * 2.0f;
}

/*************************************************************************/
// What is the center of the octant that contains the point
/*************************************************************************/
inline Vector3 Octant::CenterAtDepth(
int depth,
Vector3CRef forPoint)
{
	// What is the size of each node at this level
	float size = SizeAtDepth(depth);

	// Point the point in our "coordinate system"
	Vector3 p = (forPoint - Min()) / size;

	float halfSize = size * 0.5f;
	p.X(Math::Floor(p.X()) * size + halfSize);
	p.Y(Math::Floor(p.Y()) * size + halfSize);
	p.Z(Math::Floor(p.Z()) * size + halfSize);

	// Put it back into "world" coordinates
	return p + Min();
}

/*************************************************************************/
// I hate you C++
/*************************************************************************/
inline Octant &Octant::operator=(
const Octant &o)
{
	m_center = o.m_center;
	m_halfSize = o.m_halfSize;

	return *this;
}


