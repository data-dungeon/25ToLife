/***************************************************************************/
// Tire track emitter
/***************************************************************************/
#ifndef TTEMIT_H
#define TTEMIT_H

#include "platform/BaseType.h"
#include "Math/Vector.h"

#include "Effects/ttrack.h"
#include "Effects/ttread.h"
#include "collide/collsys.h"

class TireTrackEmitter
{
public:
	// Blow this guy away
	~TireTrackEmitter(void) {}

	// Set stuff
	static void Initialize(float totalLength, float emitterIgnore = 0.1f, float heightTweakValue = 0.1f);

	// Set the width of the tiretrack
	void SetWidth(float tireWidth)
		{ halfWidth = tireWidth * 0.5f; }

	// Add a data point
	void AddPoint(Vector3 &pos, Vector3 &dir, bool sliding);
	
	void Reset( void );

private:
	// The constructor is private (TireTrackSystem creates us)
	TireTrackEmitter(TireTreadType &treadType);
	friend class TireTrackSystem;

	// Our tread type
	TireTreadType	&tread;

	// Our tracking data
	TireTrack	*track;				// the current track
	Vector3		lastPos;				// The last pos
	Vector3		lastDir;		  		// Last direction
	float			length; 				// Length along this track
	float			halfWidth;			// half the width of the tire track

	static float segmentLength;	// Length of a segment
	static float ignoreSquared;	// length of delta's to ignore
	static float heightTweak;		// adjustment in z over terrain

	inline void InitSegmentPosDir(Vector3 &pos, Vector3 &dir);
	inline void InitSegment(Vector3 &a, Vector3 &b);
	inline void AddSegment(Vector3 &pos, Vector3 &dir);
	inline void ModifyLastSegment(Vector3 &pos, Vector3 &dir, float length);

	void ComputeVertPair(Vector3 &pos, Vector3 &dir, Vector3 &a, Vector3 &b);
	inline float GetHeight(float x, float z);

	inline void InterpolateLastPoint(float lastLength, float length, Vector3 &lastPos, Vector3 &lastDir, Vector3 &pos, Vector3 &dir, Vector3 &ip, Vector3 &id);
};

/*
extern float segmentLength;
extern float ignoreSquared;
extern float heightTweak;
*/

// Inlines
#include "Effects/ttemit.hpp"

#endif
