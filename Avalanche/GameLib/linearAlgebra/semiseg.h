//*****************************************************************************
// SemiSegment
//
// SemiSegment is a segment represented by a center point and a half segment
// such that:
// start = center - half;
// end = center - half;
//*****************************************************************************
#ifndef SEMISEGMENT_H
#define SEMISEGMENT_H

class SemiSegment
{
public:
	inline SemiSegment();
	inline SemiSegment(Vector3CRef start, Vector3CRef end);

	// Set the endpoints
	inline void SetEndPoints(Vector3CRef start, Vector3CRef end);

	// Get at the real start and end
	inline Vector3 Start() const;
	inline Vector3 End() const;

	// Here's the data
	Vector3Packed	m_center;
	Vector3Packed	m_half;
};

#include "linearAlgebra/semiseg.hpp"

#endif
