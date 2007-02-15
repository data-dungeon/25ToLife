///////////////////////////////////////////////////////////////////////////////
// Our specific subclass for sound collision tests
///////////////////////////////////////////////////////////////////////////////
#ifndef SOUNDCOLLIDE_H
#define SOUNDCOLLIDE_H

#include "Audio/Math/occlude.h"

class CLineToTerrainIntersection;

// This is pretty arbitrary
#define MAX_SOUND_HITS 	512

class SoundCollisionTest : public SoundOcclusionTest
{
public:
	SoundCollisionTest();
	~SoundCollisionTest();

protected:
	// Begin an occlusion test, return false if there are no occlusions
	bool Begin(Vector3CRef source, Vector3CRef listener);

	// Get the next occlusion,  returns false if there are no more
	// Your subclass fills out the intersection point, the material and the
	// t along the line segment.
	bool GetNext(Vector3 &intersection, u8 &material);

	// Called to match every begin...so long as it returns true
	void End();

private:
	CLineToTerrainIntersection 	*iterator;
	int									index;
};

#endif
