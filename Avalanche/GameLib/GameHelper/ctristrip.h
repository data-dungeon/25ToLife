/***************************************************************************/
// A triangle strip wrapper class
/***************************************************************************/
#ifndef CTRISTRIP_H
#define CTRISTRIP_H

#include "Math/Matrix.h"
#include "Game/Database/Som.h"

// The creation flags for the TriStrip
#define CTRISTRIP_HASNORMALS		0x01
#define CTRISTRIP_TWOSIDED			0x02
#define CTRISTRIP_ZEROFILLHDR		0x04
#define CTRISTRIP_ZEROFILLALL		0x08
#define CTRISTRIP_DEFAULTFLAGS	(CTRISTRIP_ZEROFILLHDR)

// This should coorelate
#define CTRISTRIP_MAXTEXTURES		MAX_SOM_TEXTURES_PER_PATCH

// Simple UV structure
typedef struct
{
	float		u, v;
} UVCOORD;

class CTriStrip
{
public:
	// What would the buffer size be for a multi-texture strip	
	static uint BufferSize(uint vertices, uint textureLayers = 0, uint creationFlags = CTRISTRIP_DEFAULTFLAGS);

	// create the buffer
	CTriStrip(void *dataBuffer, uint vertices, uint textureLayers = 0, uint creationFlags = CTRISTRIP_DEFAULTFLAGS);
	virtual ~CTriStrip(void) {}

	// Return the header
	inline ts_TriStripHeader *Header(void)
		{ return(hdr); }

	// The actual vertex count
	inline u16 &Vertices(void)
		{ return(hdr->u16PointCount); }

	// Return the vertices
	inline Vector4	*Vertex(void)
		{ return(v); }

	// Return the uv's
	inline UVCOORD *UV(uint layer = 0)
		{ ASSERT(layer < CTRISTRIP_MAXTEXTURES); return(uv[layer]); }

	// Return the normals
	inline Vector3	*Normal(void)
		{ return(normal); }

	// Pointers into the buffer (these are public!)
	ts_TriStripHeader		*hdr;
	Vector4					*v;
	Vector3					*normal;
	UVCOORD					*uv[CTRISTRIP_MAXTEXTURES];
};

// A wrapper that does a malloc/free around the tri-strip buffer data
class TriStrip
{
public:
	// create the buffer
	TriStrip(uint vertices, uint texutreLayers = 0, uint creationFlags = CTRISTRIP_DEFAULTFLAGS);
	virtual ~TriStrip(void);

	// Get at the tristrip
	inline CTriStrip *Get(void)
		{ return(triStrip); }

private:
	void				*buffer;
	CTriStrip		*triStrip;
};

// A tristrip that is has a prev/next
class TriStripListElement : public TriStrip
{
public:
	TriStripListElement(uint vertices, uint textureLayers = 0, uint creationFlags = CTRISTRIP_DEFAULTFLAGS) :
		TriStrip(vertices, textureLayers, creationFlags) {}
	~TriStripListElement(void) {}

	TriStripListElement		*prev, *next;
};

#endif
