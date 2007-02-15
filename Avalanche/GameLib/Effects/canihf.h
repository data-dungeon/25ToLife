/////////////////////////////////////////////////////////////////////////////
// Curved animated height field, this height field works off a spline
/////////////////////////////////////////////////////////////////////////////
#ifndef CANIHF_H
#define CANIHF_H

#include "Effects/anihf.h"
#include "path/SplinePath.h"

class CurvedAnimatedHeightField : public AnimatedHeightField
{
public:
	CurvedAnimatedHeightField(void);
	~CurvedAnimatedHeightField(void);

	// Build and place around a spline
	bool SetSplines(SplinePath &pathA, SplinePath &pathB);

	// Test for collisions
	bool Test(Segment &seg, AnimatedHeightField::Collision &collision);

private:
	// Our edges
	SplinePath		*spline[2];
};

#endif
