#ifndef __SOMTRISTRIP_H_INCLUDED__
#define __SOMTRISTRIP_H_INCLUDED__

#include "GameHelper/ctristrip.h"

class SOMTriStrip : CTriStrip
{
	// Pointers into the buffer (these are public!)
	// TN - these are no longer public, nor should they ever have been
	ts_TriStripHeader		*hdr;
	Vector4					*v;
	Vector4					*normal;
	UVCOORD					*uv;

public:
	// What would the buffer size be for a given number of vertices
	static uint BufferSize(uint vertices, uint creationFlags = CTRISTRIP_DEFAULTFLAGS);

	// create the buffer
	SOMTriStrip(void *dataBuffer, uint maxVertices, uint creationFlags = CTRISTRIP_DEFAULTFLAGS);
	inline ~SOMTriStrip(void) {}

	// Return the header
	inline ts_TriStripHeader *Header(void)
		{ return(hdr); }

	// The actual vertex count
	virtual u16 &Vertices(void)
		{ return(hdr->u16PointCount); }

	// changed these from the older forms so we don't break encapsulation
	// quite so badly
	virtual Vector4 &Vertex( int index )
		{ return( v[index] ); }
	virtual UVCOORD &UV( int index )
		{ return( uv[index] ); }
	virtual Vector4 &Normal( int index )
		{ return( normal[index] ); }

	// vector forms
	virtual void SetVertexCoords( int index, Vector4 &vtxin )
		{ v[index] = vtxin; }
	virtual void SetVertexNormal( int index, Vector4 &normin )
		{ normal[index] = normin; }
	virtual void SetVertexUV( int index, UVCOORD &uvin )
		{ uv[index] = uvin; }
	// xyzw/uv forms
	virtual void SetVertexCoords( int index, float x, float y, float z, float w )
		{v[index].x = x; v[index].y = y; v[index].z = z; v[index].w = w;}
	virtual void SetVertexNormal( int index, float x, float y, float z, float w )
		{normal[index].x = x; normal[index].y = y; normal[index].z = z; normal[index].w = w;}
	virtual void SetVertexUV( int index, float u, float v )
		{uv[index].u = u; uv[index].v = v;}

	virtual void SetTextureContext(ts_TextureContext *textureContext)
		{ hdr->pTextureContext = textureContext; }

	virtual void AddVertexCount( int add )
		{ hdr->u16PointCount += add; }

	virtual void LinkTriStrip( CTriStrip &otherstrip )

};

#endif