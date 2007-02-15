/***************************************************************************/
// A group of tire tracks are defined by having a the same texture
/***************************************************************************/
#ifndef TTREAD_H
#define TTREAD_H

#include "container/dllist.h"
#include "container/pool.h"

#if defined(GCN) && defined(CW)
#include "platform/GCN/GCNMWPort.h"
#endif

#include "Effects/ttrack.h"

class TireTrackSystem;
typedef char *TREADTEXTUREID;

class TireTreadType
{
public:
	// Do the do
	TireTreadType(TireTrackSystem &theSystem, TREADTEXTUREID theTexture, float baseAlpha);
	~TireTreadType(void);

	// Get at the texture of this group (should be const!)
	inline TREADTEXTUREID Texture(void)
		{ return(texture); }

	// Is this textureID equivalent to the one in this class
	inline bool IsEqual(TREADTEXTUREID testTexture)
		{ return(strcmpi(testTexture, texture) == 0); }

	// Get a new tire track (emitter calls this)
   TireTrack *New(void);
   void Delete(TireTrack *killMe);

	// Return the head of the the TireTrackList
	inline TireTrack *Head(void)
		{ return(list.Head()); }

	// Return the oldest DELETABLE track
	inline TireTrack *Oldest(void);

	// Render this group
	void Render(Graphics4x4 &camera, float maxDrawDistance);

	// Clear
	void Clear(void);

private:
	TireTrackSystem					&system;				// The system
	TREADTEXTUREID						texture;				// our texture ID
	t_Handle								hTexture;			// From the dblLoad
	ts_TextureContext					textureContext;   // Our texture context
	TextureArray*						textureArray;
	void BuildTextureContext(t_Handle hTexture, float baseAlpha);

	DoublyLinkedList<TireTrack>   list;					// list of tire tracks in our group

	// Culling
	inline bool CullTireTrack(Graphics4x4 &camera, TireTrack &track, float maxDrawDistance);
};

// implementations
#include "Effects/ttread.hpp"

#endif
