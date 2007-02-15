#ifndef __STRIP_BUFFER_H__
#define __STRIP_BUFFER_H__

#include "Display/Common/VtxBuffer.h"

class CInstance;
class Graphics4x4;
class Matrix4x4;
class Texture;
class TextureArray;

// Hello,  My name is Ed...
// A horse is a horse of course of course
// and no one can talk to a horse of course
// unless of course that horse (of course) is the
// famous Mr. Ed.

// Ok, so here is the intended usage of this thing...
// Derive a new class from it for you purpose
// Then override the virtual Render() method.
// You can do whatever setup/changes/whatever you 
// want in there, then, call the Render method
// that takes a model matrix and a set of texture
// matrices.  It will handle the drawing for you.

#define STRIP_BUFFER_IS_FAN	  0x00000001

class StripBuffer
{
public:
	StripBuffer();
	virtual ~StripBuffer();

	// from flags defined above
	bool Init(u32 flags = 0x00000000);
	bool IsInited() { return m_PlatformData != NULL; }

	// add a strip and return an index into the strip list for this strip
	u32 AddStrip(const u16* indices, u16 numIndices);
	void SetPassTexture(u32 passNumber, Texture* texture, u8 method, u8 wrapU, u8 wrapV, u8 filter, u8 blend);

	// note, numIndicies must be <= the value passed to AddStrip for this strip
	u32 ReplaceStrip(u32 index, const u16* indices, u16 numIndices);
    
   // set the vertex buffer used by all strips in this buffer.
   void SetVtxBuffer(VtxBuffer* vtxBuffer);
	
	// remove a strip given a strip list
	void RemoveStrip(u32 stripIndex);

	// remove all strips
	void Reset(void);

	// render function that currently gets called...
	virtual void Render(const CInstance* pInstance, const Graphics4x4* pmLocalToCamera, float fZCamDist);

	// draw the strips...
	void Render(const Matrix4x4& modelMatrix, 
					const Matrix4x4* texMatrices,
					const u32 vertexOffset = 0);

	void SetNumTexturePasses ( u32 Passes );

#ifdef PS2
	// turn off the divide by 2 for PS2
	void Overbrighting(bool enabled);
#endif

	// used for PS2's MakeTextureCurrent/StartNextDynamicTransfer
	// if this isnt called, dynamic textures wont get uploaded before
	// drawing this model, so this stripbuffer better not reference any
	// all implimentations must default the member vars to 
	//  m_TextureSet(NULL), mTextureSubSet(0)
	void SetTextureArray(TextureArray* ts, u32 subsetIndex)
		{	m_TextureArray = ts; m_TextureSubSet = subsetIndex;	}

	// here are the things you can muck with if you derive from this thing.
	// lots eh?
private:
   VtxBuffer*      m_VtxBuffer;

	TextureArray*     m_TextureArray;
	u32				 m_TextureSubSet;

	u32				 m_Flags;

	int				 m_TexturePasses;		// PAB - added to allow rendering of 'x' layers for ANIHF water FX.

	// everything from here down is to support the implimentation specific versions of this code
	// you can expect anything you touch down here to probably fail miserably.. :)..
private:
	class StripBufferPlatformData* m_PlatformData;
	
};
#endif // __STRIP_BUFFER_H__
