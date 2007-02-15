/***************************************************************************/
// Tire track instance
/***************************************************************************/
#ifndef TTRACK_H
#define TTRACK_H

#include "Layers/Assert/Assert.h"
#include "platform/BaseType.h"
#include "Math/Matrix.h"

#include "GameHelper/ctristrip.h"
#include "Effects/ttconst.h"
//#include "effects/ttdisplay.h"

#include "Display/Common/VtxBuffer.h"
#include "Display/Common/StripBuffer.h"
#include "Game/Managers/TextureMgr/TextureMgr.h"

class TireTrack : public InstancedDynamicObject
{
public:
	// constructor
	TireTrack(void);
	~TireTrack(void);

		// Set the constants
	static void Setup(float trackLength, int textureRepeat);
	static float GetLength(void)
		{ return(totalLength); }

	// What is my age?
	inline void SetBirth(uint myBirth)
		{ birth = myBirth; }
	inline uint Birth(void)
		{ return(birth); }

	// Lock it
	inline void Lock(void)
		{ ++lock; }
	inline void Unlock(void)
		{ ASSERT(lock > 0); --lock; }
	inline bool Locked(void)
		{ return(lock > 0); }

	// draw stuff
	inline bool DrawnLastFrame(void)
		{ return(drawnLastFrame != 0); }
	inline void SetDrawnLastFrame(bool drawn)
		{ drawnLastFrame = drawn; }

	// Add vertex data
	inline void AddVertexData(Vector3 &a, Vector3 &b, float segmentLength);

	// reset a vertex position...
	inline void SetVertexData(
		uint vertBase,
		Vector3 &a,
		Vector3 &b,
		float segmentLength);


	// Set the texture context
	inline void SetTextureContext(TextureArray* ts, ts_TextureContext *textureContext)
	{ 
		sbuf.SetTextureArray(ts, 0);
		TextureDatabase* tdb = textureContext->TextureLayer[0].textureDatabase;
		ASSERT_PTR(tdb);
		Texture* tex = tdb->GetTexture(textureContext->TextureLayer[0].s16TextureIndex);
		sbuf.SetPassTexture(0,
			tex, 
			textureContext->TextureLayer[0].u8Method, 
			textureContext->TextureLayer[0].u8WrapU, 
			textureContext->TextureLayer[0].u8WrapV,
			TS_FILTER_BILINEAR,
			textureContext->TextureLayer[0].u8Blend);
		LOCKUP();
		baseColor = *(u32*)&textureContext->BaseColor;
	}

	// Render it
	virtual void Render(const CInstance* pInstance, const Graphics4x4* pmLocalToCamera, float fZCamDist);

	// Handy
	inline int VertPairs(void)	{ return pairs; }
	inline void ClearVerts(void) { pairs = 0; sbuf.Reset(); }

	// Get the middle vert
	inline Vector3Packed *GetMiddleVert(void) { return &vbuf.Vertex(pairs); }

private:
	// Our triangle strip data
	VtxBuffer vbuf;
	StripBuffer sbuf;
	int pairs;
	int stripIndex;
	u32 baseColor;

	// Our time of birth (for LRU)
	uint						birth;

	// Our counts
	unsigned char			lock;
	unsigned char			drawnLastFrame;

	// some constants we need
	static u16 indices[TTRACK_VERTS];
	static float totalLength;
	static float vSeg[TTRACK_VERTPAIRS];
	static float invSegmentLength;
	static float invSegments;

public:
	// Ugly, but for fast doubly linked list
	TireTrack	*next, *prev;
};

class TireTrackPad16 : public TireTrack
{
public:
	inline TireTrackPad16(void) {}
	~TireTrackPad16(void);

private:
	char	pad[(sizeof(TireTrack) + 15) & ~16];
};

// Implementations
#include "Effects/ttrack.hpp"

#endif
