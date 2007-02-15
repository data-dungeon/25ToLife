inline bool RaySegment::IntersectWithPlane(const Vector &planeP,
										const Vector &planeN,
										Vector &i) const
{
	float t;
	return IntersectWithPlane(planeP, planeN, i, t);
}

#define Epsilon 1e-7f

inline Vector RaySegment::ClosestPoint(const Vector &point) const
{
	// Make segment into a ray
	Vector v = m_end - m_start;

	// Find the perpendicular point
	Vector perp = m_start + v * (Dot(point - m_start, v) / Dot(v, v));

	return perp;
}

inline Vector RaySegment::ClosestPointOnSegment(const Vector &point) const
{
	// Compute the direction vector
	Vector w = m_end - m_start;

	// Normalize, but we also need length, so do it ourselves
	float fLength = (float)w.Length();
	if (fLength == 0.0f)
		return m_start;
	float fOneOverLength = 1.0f / fLength;
	Vector v = w * fOneOverLength;

	// Find the length of the projection of (p - start) onto v
	float t = (float)Dot(point - m_start, v);

	// test against segment
	Vector perp;
	if (t < 0.0f)
		perp = m_start;
	else if (t > fLength)
		perp = m_end;
	else
		perp = m_start + v * t;
	return perp;
}

inline bool RaySegment::IntersectWithPlane(const Vector &planeP,const Vector &planeN,Vector &i, float &t) const
{
	// Make segment into a ray
	Vector w = m_end - m_start;

	// Is the line parallel to the plane?
	float vDotN = (float)Dot(w, planeN);
	if (vDotN == 1.0)
		return false;

	// What is the intersection?
	t = (float)Dot((planeP - m_start), planeN) / vDotN;

	// Turn back into a point
	i = m_start + w * t;

	return true;
}

inline bool RaySegment::IntersectWithPlaneOnSegment(const Vector &planeP,const Vector &planeN,Vector &i, float &t) const
{
	// Make segment into a ray
	Vector w = m_end - m_start;

	// Is the line parallel to the plane?
	float vDotN = (float)Dot(w, planeN);
	if (vDotN == 1.0)
		return false;

	// What is the intersection?
	t = (float)Dot((planeP - m_start), planeN) / vDotN;

	// Turn back into a point
	i = m_start + w * t;

	// On the segment?
	if (t < 0.0f || t > 1.0f)
		return false;

	return true;
}

inline bool RaySegment::IntersectWithPlaneFromFront(const Vector &planeP,const Vector &planeN,Vector &i, float &t) const
{
	// Make segment into a ray
	Vector w = m_end - m_start;

	// Does the segment point away from the plane?
	float vDotN = (float)Dot(w, planeN);
	if (vDotN > -Epsilon)
		return false;

	// What is the intersection?
	t = (float)Dot((planeP - m_start), planeN) / vDotN;

	// Turn back into a point
	i = m_start + w * t;

	return true;
}
inline bool RaySegment::IntersectWithPlaneFromFrontOnSegment(const Vector &planeP,const Vector &planeN,Vector &i, float &t) const
{
	// Make segment into a ray
	Vector w = m_end - m_start;

	// Does the segment point away from the plane?
	float vDotN = (float)Dot(w, planeN);
	if (vDotN > -Epsilon)
		return false;

	// What is the intersection?
	t = (float)Dot((planeP - m_start), planeN) / vDotN;

	// Turn back into a point
	i = m_start + w * t;

	// On the segment?
	if (t < 0.0f || t > 1.0f)
		return false;

	return true;
}

inline bool RaySegment::IntersectWithPlaneFromBack(const Vector &planeP,const Vector &planeN,Vector &i, float &t) const
{
	// Make segment into a ray
	Vector w = m_end - m_start;

	// Does the segment point into the plane?
	float vDotN = (float)Dot(w, planeN);
	if (vDotN < Epsilon)
		return false;

	// What is the intersection?
	t = (float)Dot((planeP - m_start), planeN) / vDotN;

	// Turn back into a point
	i = m_start + w * t;

	return true;
}

inline bool RaySegment::IntersectWithPlaneFromBackOnSegment(const Vector &planeP,const Vector &planeN,Vector &i, float &t) const
{
	// Make segment into a ray
	Vector w = m_end - m_start;

	// Does the segment point into the plane?
	float vDotN = (float)Dot(w, planeN);
	if (vDotN < Epsilon)
		return false;

	// What is the intersection?
	t = (float)Dot((planeP - m_start), planeN) / vDotN;

	// Turn back into a point
	i = m_start + w * t;

	// On the segment?
	if (t < 0.0f || t > 1.0f)
		return false;

	return true;
}

