#ifndef __DMATRISTRIP_H_INCLUDED__
#define __DMATRISTRIP_H_INCLUDED__

#include "GameHelper/ctristrip.h"

class DMATriStrip : CTriStrip
{
	static uint BufferSize(uint vertices, uint creationFlags = CTRISTRIP_DEFAULTFLAGS);

	DMATriStrip(void *dataBuffer, uint maxVertices, uint creationFlags = CTRISTRIP_DEFAULTFLAGS);
	inline ~DMATriStrip(void) {}

	// The actual vertex count
	virtual int Vertices(void) = 0;

	// changed these from the older forms so we don't break encapsulation
	// quite so badly
	virtual Vector4 &Vertex( int index );
	virtual UVCOORD &UV( int index );
	virtual Vector4 &Normal( int index );

	// vector forms
	virtual void SetVertexCoords( int index, Vector4 &vtxin );
	virtual void SetVertexNormal( int index, Vector4 &normin );
	virtual void SetVertexUV( int index, UVCOORD &uvin );
	// xyzw/uv forms
	virtual void SetVertexCoords( int index, float x, float y, float z, float w );
	virtual void SetVertexNormal( int index, float x, float y, float z, float w );
	virtual void SetVertexUV( int index, float u, float v );

	virtual void SetTextureContext(ts_TextureContext *textureContext);

	virtual void AddVertexCount( int add );

	virtual void LinkTriStrip( CTriStrip &otherstrip );
};

#endif