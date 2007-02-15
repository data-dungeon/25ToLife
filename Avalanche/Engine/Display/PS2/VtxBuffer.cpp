// on the ps2, this buffer looks like:
//
// VtxX		VtxY		VtxZ
// [NormX	NormY		NormZ]
// [UV1..]
// [UV2..]
//	[UV3..]
// [UV4..]
// [UV5..]
// [UV6..]
// [UV7..]
// [DColorR]	[DColorG]	[DColorB]	[DColorA]
// [SColorR]	[SColorG]	[SColorB]	[SColorA]
// PAD to 1 QW boundry...
// 
// a few samples are in order:
//
// VERT_HAS_DIFFUSE|VERT_HAS_NORMAL|VERT_TEX0_DIM2|VERT_TEX1_DIM2
// VtxX		VtxY		VtxZ		NormX
//	NormY		NormZ		U1			V1
//	U2			V2			DColorR	DColorG
// DColorB	DColorA	PAD		PAD
//
// or
//
// VERT_TEX0_DIM3|VERT_TEX1_DIM2
//	VtxX		VtxY		VtxZ		U1
// V1			W1			U2			V2

/******************* includes ****************************/

#include "Display/DisplayPCH.h"



////////////////////////
// access functions
////////////////////////

// illegal...
void IllegalAccess(u32 vtx)
{
	// this kind of access to this vertex buffer is illegal.
	ASSERT(false);
}


////////////////////////
// init
////////////////////////

VtxBuffer::VtxBuffer()
	: m_PlatformData(NULL)
{
}

VtxBuffer::~VtxBuffer()
{
	if(m_PlatformData) delete m_PlatformData;
}

bool VtxBuffer::Init(u32 vtxCount, u32 vtxFlags, u32 texFlags[VTX_BUFFER_TEXTURE_LAYERS])
{
	m_PlatformData = new VtxBufferPlatformData();
	ASSERT(m_PlatformData);
	return m_PlatformData->Init(vtxCount, vtxFlags, texFlags);
}

void VtxBuffer::Lock(void)
{
	ASSERT(m_PlatformData);
	ASSERT(!m_PlatformData->mLocked);
	m_PlatformData->mLocked = true;
}

void VtxBuffer::UnLock(void)
{
	ASSERT(m_PlatformData);
	ASSERT(m_PlatformData->mLocked);
	m_PlatformData->mLocked = false;

	// flush the cache so that we can start to dma it immediately
	FlushCache(0);
}


////////////////////////
// platformData functions
////////////////////////

VtxBufferPlatformData::~VtxBufferPlatformData()
{
	memFree(mBuffer);
}

bool VtxBufferPlatformData::Init(u32 vtxCount, u32 vtxFlags, u32 texFlags[VTX_BUFFER_TEXTURE_LAYERS])
{
	mLocked = false;
	mVtxCount = vtxCount;
	mVtxFlags = vtxFlags;
	mTexFlags[0] = texFlags[0];
	mTexFlags[1] = texFlags[1];
	mTexFlags[2] = texFlags[2];
	mTexFlags[3] = texFlags[3];
	mTexFlags[4] = texFlags[4];
	mTexFlags[5] = texFlags[5];
	mTexFlags[6] = texFlags[6];
	mTexFlags[7] = texFlags[7];
	
	// build offsets & determine how long we will be...
	u32 offset = 0;
	u32 alignment = 0;

	mNumPasses = 0;
#define DATA_SIZE(size) {								\
		offset += (size)*4;								\
		alignment += (size);								\
	}

#define TEX_COORDS(channel) {																																			\
	if(mTexFlags[channel] & (VtxBuffer::VERT_TEX_DIM1|VtxBuffer::VERT_TEX_DIM2|VtxBuffer::VERT_TEX_DIM3|VtxBuffer::VERT_TEX_DIM4)){	\
		mNumPasses++;																																						\
		mTexOffset[channel] = offset;																																	\
		if(mTexFlags[channel] & VtxBuffer::VERT_TEX_DIM1)																										\
			DATA_SIZE(1);																																					\
		else if(mTexFlags[channel] & VtxBuffer::VERT_TEX_DIM2)																								\
			DATA_SIZE(2);																																					\
		else if(mTexFlags[channel] & VtxBuffer::VERT_TEX_DIM3)																								\
			DATA_SIZE(3);																																					\
		else if(mTexFlags[channel] & VtxBuffer::VERT_TEX_DIM4)																								\
			DATA_SIZE(4);																																					\
		else																																									\
			ASSERT(false); /*unsupported yet*/																														\
	}}
		
	DATA_SIZE(3); // for xyz
		
	if(mVtxFlags & VtxBuffer::VERT_HAS_NORMAL)
		DATA_SIZE(3); // nx, ny, nz

	for(int channel = 0; channel < 8; channel++)
	{
		if(mTexFlags[channel] & (VtxBuffer::VERT_TEX_DIM1|VtxBuffer::VERT_TEX_DIM2|VtxBuffer::VERT_TEX_DIM3|VtxBuffer::VERT_TEX_DIM4))
		{
			mNumPasses++;
			mTexOffset[channel] = offset;
			switch(mTexFlags[channel])
			{
			case VtxBuffer::VERT_TEX_DIM1:
				DATA_SIZE(1);
				break;
			case VtxBuffer::VERT_TEX_DIM2:
				DATA_SIZE(2);
				break;
			case VtxBuffer::VERT_TEX_DIM3:
				DATA_SIZE(3);
				break;
			case VtxBuffer::VERT_TEX_DIM4:
				DATA_SIZE(4);
				break;
			default:
				// unsupported vtx format...
				ASSERT(false);
			}
		}
	}

	if(mVtxFlags & (VtxBuffer::VERT_HAS_DIFFUSE|VtxBuffer::VERT_HAS_SPECULAR))
	{
		// might some day need this...:)...
		// unpack
		//offset += 4;

		if(mVtxFlags & VtxBuffer::VERT_HAS_DIFFUSE)
		{
			mDiffuseOffset = offset;
			DATA_SIZE(1);
		}
		if(mVtxFlags & VtxBuffer::VERT_HAS_SPECULAR)
		{
			mSpecularOffset = offset;
			DATA_SIZE(1);
		}
	}

	// how much padding
	u32 paddingOffset = offset;
	u32 paddingSize = 16 - (offset % 16);
	if(paddingSize == 16) paddingSize = 0;
	
	mVtxSize = offset + paddingSize;

	// get a buffer..
	MEM_SET_ALLOC_NAME("VtxBuffer");
	mBuffer = (u_long128*)memAlloc(mVtxSize * vtxCount);
	ASSERT(mBuffer);

	return true;
}

