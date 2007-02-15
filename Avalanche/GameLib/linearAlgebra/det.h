/***************************************************************************/
// A static class for computing various determinants
// Since these are shared between the two matrix classes, they use
// parameters for all matrix entries
/***************************************************************************/
#ifndef DET_H
#define DET_H

class Determinant
{
public:
	// Return the determinant of a 2x2
	inline static float Det(float _11, float _12,
									float _21, float _22);

	// Return the det of a 3x3
	inline static float Det(float _11, float _12, float _13,
								 	float _21, float _22, float _23,
								 	float _31, float _32, float _33);

	// Return the det of a 4x4
	inline static float Det(float _11, float _12, float _13, float _14,
								 	float _21, float _22, float _23, float _24,
								 	float _31, float _32, float _33, float _34,
								 	float _41, float _42, float _43, float _44);
};

// Include the implementation file
#include "linearAlgebra/det.hpp"

#endif
