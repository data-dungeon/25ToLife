#include <string.h>
#include "GameHelper/SOMTriStrip.h"

/***************************************************************************/
// What would the buffer size be for a given number of vertices
/***************************************************************************/
uint SOMTriStrip::BufferSize(
uint vertices,
uint creationFlags)
{
	uint size = sizeof(ts_TriStripHeader) + sizeof(Vector4) * vertices;
	if (creationFlags & CTRISTRIP_HASNORMALS)
		size += sizeof(Vector4) * vertices;
	if (creationFlags & CTRISTRIP_HASTEXTURE)
		size += sizeof(UVCOORD) * ((vertices + 1) & ~1);

	return(size);
}

/***************************************************************************/
// create the buffer
/***************************************************************************/
SOMTriStrip::SOMTriStrip(
void *dataBuffer,
uint maxVertices,
uint creationFlags)
{
	uint ptr = (uint)dataBuffer;

	// Setup the header pointer
	hdr = (ts_TriStripHeader *)ptr;
	ptr += sizeof(ts_TriStripHeader);

	// Setup the vertex list
	v = (Vector4 *)ptr;
	ptr += sizeof(Vector4) * maxVertices;

	// Do we have normals?
	if ((creationFlags & CTRISTRIP_HASNORMALS) == 0)
		normal = NULL;
	else
	{
		normal = (Vector4 *)ptr;
		ptr += sizeof(Vector4) * maxVertices;
	}

	// Do we have uv's?
	if ((creationFlags & CTRISTRIP_HASTEXTURE) == 0)
		uv = NULL;
	else
	{
		uv = (UVCOORD *)ptr;
		ptr += sizeof(UVCOORD) * ((maxVertices + 1) & ~1);
	}

	// Do they want to zerofill this badboy?
	if (creationFlags & CTRISTRIP_ZEROFILLALL)
		memset(dataBuffer, '\0', ptr - (uint)dataBuffer);
	else if (creationFlags & CTRISTRIP_ZEROFILLHDR)
		memset(dataBuffer, '\0', sizeof(ts_TriStripHeader));

	// Set the actual number of vertices
	hdr->u16SkipPointCount = maxVertices;
}