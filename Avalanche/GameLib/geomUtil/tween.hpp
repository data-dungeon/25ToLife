/***************************************************************************/
// Simple tweening stuff
/***************************************************************************/

/***************************************************************************/
// Linearly tween between start/end.
// t goes from 0 to 1
/***************************************************************************/
inline float Tween::Linear(
float start,
float end,
float t)
{
	return(start + (end - start) * t);
}

/***************************************************************************/
// Linearly tween between start/end.
// t goes from 0 to 1
/***************************************************************************/
inline Vector3 Tween::Linear(
Vector3 &start,
Vector3 &end,
float t)
{
	return(start + (end - start) * t);
}

/***************************************************************************/
// Rotate the point in the plane defined by start/end around center
// t goes from 0 to 1
/***************************************************************************/
inline Vector3 Tween::Circular(
Vector3 &start,
Vector3 &end,
Vector3 &center,
float t)
{
	Vector3 cStart = start - center;
	Vector3 cEnd = end - center;
	return(center + Circular(cStart, cEnd, t));
}

/***************************************************************************/
// Rotate the point in the plane defined by start/end.
// t goes from 0 to 1
/***************************************************************************/
inline Vector3 Tween::Circular(
Vector3 &start,
Vector3 &end,
float t)
{
	// Get the length of the vectors
	float sLength = start.Length();
	float eLength = end.Length();

	// Degenerate case?
	if (sLength < ZEROTHRESHOLD || eLength < ZEROTHRESHOLD)
		return(Linear(start, end, t));

	// Make them unit vectors
	Vector3 uStart = start * (1.0f / sLength);
	Vector3 uEnd = end * (1.0f / eLength);

	// Do the tween
	Vector3 tween = CircularUnit(uStart, uEnd, t);

	// Tween it's length
	tween *= Linear(sLength, eLength, t);
	return(tween);
}

/***************************************************************************/
// Rotate the point in the plane defined by start/end.
// t goes from 0 to 1
// Start and End are unit
/***************************************************************************/
inline Vector3 Tween::CircularUnit(
Vector3 &start,
Vector3 &end,
float t)
{
	// Error condition
	if (start == end)
		return(start);

	// What is the normal for start/end
	Vector3 n = Vector3::Cross(start, end);
	float sine = n.Length();
	n.Normalize();

	// what is the total angle between start and end?
	float cosine = Vector3::Dot(start, end);
	float totalTheta = Math::ArcTan2(sine, cosine);
	float theta = t * totalTheta;

	// We also need n x start
	Vector3 nxStart = Vector3::Cross(n, start);

	// This looks simple
	return(start * Math::Cos(theta) + nxStart * Math::Sin(theta));
}

