/////////////////////////////////////////////////////////////////////////////
// A triangle strip wrapper class
/////////////////////////////////////////////////////////////////////////////

/******************* includes ****************************/

#include "GameHelper/GameHelperPCH.h"





/////////////////////////////////////////////////////////////////////////////
// What would the buffer size be for a given number of vertices
/////////////////////////////////////////////////////////////////////////////
uint CTriStrip::BufferSize(
uint vertices,
uint textureLayers,
uint creationFlags)
{
	// Can't have too many textures
	ASSERT(textureLayers <= CTRISTRIP_MAXTEXTURES);

	uint size = sizeof(ts_TriStripHeader) + sizeof(Vector4) * vertices;
	if (creationFlags & CTRISTRIP_HASNORMALS)
		size += sizeof(Vector4) * vertices;

	// What about UV's?
	if (textureLayers > 0)
	{
		uint layerSize = sizeof(UVCOORD) * ((vertices + 1) & ~1);
		size += layerSize * textureLayers;
	}

	return(size);
}

/////////////////////////////////////////////////////////////////////////////
// create the buffer
/////////////////////////////////////////////////////////////////////////////
CTriStrip::CTriStrip(
void *dataBuffer,
uint vertices,
uint textureLayers,
uint creationFlags)
{
	// Can't have too many textures
	ASSERT(textureLayers <= CTRISTRIP_MAXTEXTURES);

	uint ptr = (uint)dataBuffer;

	// Setup the header pointer
	hdr = (ts_TriStripHeader *)ptr;
	ptr += sizeof(ts_TriStripHeader);

	// Setup the vertex list
	v = (Vector4 *)ptr;
	ptr += sizeof(Vector4) * vertices;

	// Do we have normals?
	if ((creationFlags & CTRISTRIP_HASNORMALS) == 0)
		normal = NULL;
	else
	{
		normal = (Vector3 *)ptr;
		ptr += sizeof(Vector4) * vertices;
	}

	// Do we have uv's?
	for (uint i = 0; i < CTRISTRIP_MAXTEXTURES; i++)
	{
		if (i >= textureLayers)
			uv[i] = NULL;
		else
		{
			uv[i] = (UVCOORD *)ptr;
			ptr += sizeof(UVCOORD) * ((vertices + 1) & ~1);
		}
	}

	// Do they want to zerofill this badboy?
	if (creationFlags & CTRISTRIP_ZEROFILLALL)
		memset(dataBuffer, '\0', ptr - (uint)dataBuffer);
	else if (creationFlags & CTRISTRIP_ZEROFILLHDR)
		memset(dataBuffer, '\0', sizeof(ts_TriStripHeader));

	// Set the actual number of vertices
	hdr->u16SkipPointCount = vertices;

	// Set the flags field
	if ((creationFlags & CTRISTRIP_HASNORMALS) != 0)
		hdr->u8Flags |= TRI_STRIP_HAS_NORMALS;
	if ((creationFlags & CTRISTRIP_TWOSIDED) != 0)
		hdr->u8Flags |= TRI_STRIP_TWO_SIDED;
}

/////////////////////////////////////////////////////////////////////////////
// create the buffer
/////////////////////////////////////////////////////////////////////////////
TriStrip::TriStrip(
uint vertices,
uint textureLayers,
uint creationFlags) 
{
	// Allocate the data buffer
	uint size = CTriStrip::BufferSize(vertices, textureLayers, creationFlags);
	buffer = memAlloc(size);
	ASSERT(buffer);

	// Allocate the tri strip
	triStrip = new CTriStrip(buffer, vertices, textureLayers, creationFlags);
	ASSERT(triStrip);
}

/////////////////////////////////////////////////////////////////////////////
// Destroy the buffer
/////////////////////////////////////////////////////////////////////////////
TriStrip::~TriStrip(void)
{
	if (triStrip)
		delete triStrip;

	if (buffer)
		memFree(buffer);
}

