/***************************************************************************/
// Tire track system
/***************************************************************************/
#ifndef TTSYSTEM_H
#define TTSYSTEM_H

#include "container/pool.h"
#include "container/dynlist.h"

#include "Effects/ttread.h"
#include "Effects/ttemit.h"
class TireTreadType;
class TireTrackPad16;

class TireTrackSystem
{
public:
	// constructor
	TireTrackSystem(void);
	~TireTrackSystem(void);

	// Setup all kinds of stuff
	void Setup(float emitterIgnore, float heightTweak, float trackLength, uint textureRepeat);

	// Set the total number of trax
	void SetMaxTracks(uint maximumTracks);

	// Set the max draw distance
	void SetMaxDrawDistance(float theMaxDrawDistance)
		{ maxDrawDistance = theMaxDrawDistance; }

	// A tread type
	typedef int TREADTYPE;

	// Load in a tire track (returns index)
	TREADTYPE AddTreadType(TREADTEXTUREID texture, float baseAlpha = 0.5f);

	// find a given tread type
	TREADTYPE FindTreadType(TREADTEXTUREID texture);

	// Create a new emitter
	TireTrackEmitter *CreateEmitter(TREADTYPE tread);

	// For debugging, allow rendering to be toggled on and off
	void ToggleRender(void)
		{ render = !render; }
	bool RenderOn(void)
		{ return(render); }

	// Render all non-culled tracks
//	void Render(Graphics4x4 &camera);

	// Clear all tracks!
	void ClearAll(void);

private:
	Pool<TireTrackPad16>			pool;
	int								maxTracks;
	int								tracksLeft;	// Tracks left
	uint								time;

	DynList<TireTreadType> 		treads; 		// List of the tread types

	float								maxDrawDistance;

	// The display stuff
	bool								render;
//  	TireTrackDisplay				displayTrack;

	// These calls provide the interface for the TireTread to allocate new tracks
	TireTrack *New(TireTreadType &parent);
	void Delete(TireTrack *killMe);

	// Reclaim some tracks, the pool is full
	void ReclaimTracks(void);

	friend class TireTreadType;
};

// Our singleton
extern TireTrackSystem *g_trackSystemPtr;
inline TireTrackSystem &SafePtr(TireTrackSystem *ptr)
{
	ASSERT(ptr);
	return(*ptr);
}
#define g_trackSystem	SafePtr(g_trackSystemPtr)

#endif
