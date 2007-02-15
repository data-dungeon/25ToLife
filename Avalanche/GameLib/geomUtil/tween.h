/***************************************************************************/
// Simple tweening stuff
/***************************************************************************/
#ifndef TWEEN_H
#define TWEEN_H

#include "linearAlgebra/cvector.h"

class Tween
{
public:
	// Linearly tween between start/end.
	// t goes from 0 to 1
	static inline float Linear(float start, float end, float t);
	static inline Vector3 Linear(Vector3 &start, Vector3 &end, float t);

	// Rotate the point in the plane defined by start/end around center
	// t goes from 0 to 1
	static inline Vector3 Circular(Vector3 &start, Vector3 &end, Vector3 &center, float t);

	// Rotate the point in the plane defined by start/end around origin
	// t goes from 0 to 1
	static inline Vector3 Circular(Vector3 &start, Vector3 &end, float t);

	// Rotate the point in the plane defined by start/end around center
	// t goes from 0 to 1
	// Start and End are unit
	static inline Vector3 CircularUnit(Vector3 &start, Vector3 &end, float t);
};

// Include the implementation here
#include "tween.hpp"

#endif
