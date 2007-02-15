/***************************************************************************/
// A super-nice Vector class that is fast too.
/***************************************************************************/
#include "Math/MathStats.h"

/***************************************************************************/
// Multiply a scalar times a vector (c++ won't automatically swap)
/***************************************************************************/
inline Vector2 operator*(
float scalar,
const Vector2 &v)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(F_TIMES_V2);
#endif //MATH_STATS

	return(v * scalar);
}

/***************************************************************************/
// Multiply a scalar times a vector (c++ won't automatically swap)
/***************************************************************************/
inline Vector3 operator*(
float scalar,
Vector3CRef v)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(F_TIMES_V3);
#endif //MATH_STATS

	return(v * scalar);
}

/***************************************************************************/
// Multiply a scalar times a vector (c++ won't automatically swap)
/***************************************************************************/
inline Vector4 operator*(
float scalar,
const Vector4 &v)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(F_TIMES_V4);
#endif //MATH_STATS

	return(v * scalar);
}

/***************************************************************************/
// Simple thing for comparing points in space
/***************************************************************************/
inline float Distance(
Vector3CRef a,
Vector3CRef b)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(DISTANCE_V3_V3);
#endif //MATH_STATS

	return((a - b).Length());
}

/***************************************************************************/
// Simple thing for comparing points in space
/***************************************************************************/
inline float DistanceSquared(
Vector3CRef a,
Vector3CRef b)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(DISTANCE_SQUARED_V3_V3);
#endif //MATH_STATS

	return((a - b).LengthSquared());
}

/***************************************************************************/
// Simple thing for comparing points in space
/***************************************************************************/
inline bool PointsCoincide(
Vector3CRef a,
Vector3CRef b,
float thresholdSquared)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(POINTS_COINCIDE_V3_V3_F);
#endif //MATH_STATS

	return((a - b).LengthSquared() < thresholdSquared);
}
