/////////////////////////////////////////////////////////////////////////////
// Inlines for ScreenSpaceEffect
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Transform using a clip factor (making the cliping region larger/smaller
// than the screen)
/////////////////////////////////////////////////////////////////////////////
inline bool ScreenSpaceEffect::Transform(
const Matrix4x4 &transform,
Vector3CRef p,
Vector2 &s,
const Vector2 &clipFactor)
{
	// Do the math
	Vector4 t = p.Transform3(transform);

	// Behind us?
	if (t.w() < 0.0f)
		return(false);

	// Other clipping
	Vector2 clip = clipFactor * t.w();
	if (t.x() < -clip.x() || t.x() > clip.x() || t.y() < -clip.y() || t.y() > clip.y())
		return(false);

	// Figure out where it lies
	float oow = 1.0f / t.w();
	s.Set(((t.x() * oow) * 0.5f + 0.5f),
			((t.y() * oow) * 0.5f + 0.5f));

	return(true);
}

/////////////////////////////////////////////////////////////////////////////
// Do the transform, absolute clipping
/////////////////////////////////////////////////////////////////////////////
inline bool ScreenSpaceEffect::Transform(
const Matrix4x4 &transform,
Vector3CRef p,
Vector2 &s,
float &w)
{
	// Do the math
	Vector4 t = p.Transform3(transform);

	// Outside of fustrum?
	if (t.z() < 0.0f || t.z() > t.w() || t.x() < -t.w() || t.x() > t.w() || t.y() < -t.w() || t.y() > t.w())
		return(false);

	// Figure out where it lies on the screen
	float oow = 1.0f / t.w();
	s.Set(((t.x() * oow) * 0.5f + 0.5f),
			((t.y() * oow) * 0.5f + 0.5f));
	w = t.w();

	return(true);
}

