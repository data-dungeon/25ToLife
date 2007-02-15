/******************* includes ****************************/

#include "Display/DisplayPCH.h"


#include "Game/Managers/TextureMgr/PlatformImage.h"


/////////////////////////////////////////////////////////////////////
// defines/enums
/////////////////////////////////////////////////////////////////////

#define BLOCK_LIST_GROWTH 10
#define INIT_BLOCK_LIST_LEN 40

#define VU_MEM (1024)
#define VU_BUFFER (VU_MEM/2)
#define STATE_HEADER_SIZE (3)

#define MAX_CLIP_VERTS 32
#define CLIP_STACK_SIZE 11

// FlagsA bits 0x00000003
#define CULL_MODE 0
#define ALWAYS_MODE 1
#define CLIP_MODE 2

// block flags
#define STRIP_CONTINUATION		  0x10000
#define LINEAR_VERTEX_ORDER	  0x20000
#define QUAD_BLOCK				  0x40000

/////////////////////////////////////////////////////////////////////
// internal structures
/////////////////////////////////////////////////////////////////////

struct Block
{
	u32 mStripIndex;
	u32 mNumVerts;
	u32 mNumQWords;
	u32 mFlags;
	u32 mVtxIndex1;
	u32 mVtxIndex2;
};

class StripBufferPlatformData
{
public:
	VtxBuffer* m_VtxBuffer;
	u32 mFlags;
	u32 mMaxVerts;
	u32 mMaxVertsClipped;
	u32 mNumQWords;
	u32 mQWordsPerVtx;
	u32 mNumBlocks;
	u32 mNumBlocksAlloced;
	u32 mNumPasses;
	u32 mOffset;
	u32 mOffsetClipped;
	Block *mBlocks;
	u16 mPassDataSizes[8];
	u16 mTotalPassDataSize;
	u32* mData;
	u32* mPassData[8]; // contiguous memory so it can be dma'ed as one block...
	u32 mNumStrips;
	Texture *mTextures[8];
	u8 mTextureFilters[8];
	u8 mTextureMethod[8];
	u8 mTextureBlendModes[8];
	u8 mWrapU[8];
	u8 mWrapV[8];

	bool mOverBrightEnable;

	u32 mVtxFlags; // flags from the vtx buffer...
	u32 mTexFlags[8];
	void* mVUCodeStart;

	void Overbrighting(bool overbright);
	void AddBlock(u16 index1, u16 index2, u32 vtxCount, u32 stripIndex, u32 flags);
	void AddQuadBlock(u16 index1, u32 flags);
	u32 FindPriorQuadBlock(u16 index1, u32 flags);
	void SetupState();
	void SendVerts();
	int SendBlocks(u32 &block, int &startVert, bool reallysend, int maxverts);
	void SendMicrocode(void);
	void SendState(const Matrix4x4& modelMatrix, 
						const Matrix4x4* texMatrices);

	void AppendVerts(Block* block, u32 startVertIdx, u32 vertCount, u32 adcCount);
	void FixupQuadBuffer(u32* vtxAddr, u32 vtxCount);
	void FixupBuffer(u32* vtxAddr, u32 vtxCount, u32 adcCount);
	inline void Fixup(u32* vtxAddr, u32 colorOffset, bool compressed, u128 mask);
};

#ifndef CONSUMER_BUILD
#define DEBUG_STRIPBUFFER
#endif // CONSUMER_BUILD
#ifdef DEBUG_STRIPBUFFER
 struct 
{
	bool disableTextures;
	bool disableAlpha;
	bool forceWireFrame;
} StripBuffer_Dbg;
#endif

/////////////////////////////////////////////////////////////////////
// externs for vu code
/////////////////////////////////////////////////////////////////////

extern unsigned int VUStripBuffer1x2CD_DmaTag __attribute__ ((section( ".vudata" )));
extern unsigned int VUStripBuffer2x2CD_DmaTag __attribute__ ((section( ".vudata" )));
extern unsigned int VUStripBuffer3x2CD_DmaTag __attribute__ ((section( ".vudata" )));
extern unsigned int VUStripBuffer4x2CD_DmaTag __attribute__ ((section( ".vudata" )));
extern unsigned int VUStripBuffer5x2CD_DmaTag __attribute__ ((section( ".vudata" )));
extern unsigned int VUStripBuffer6x2CD_DmaTag __attribute__ ((section( ".vudata" )));
extern unsigned int VUStripBuffer7x2CD_DmaTag __attribute__ ((section( ".vudata" )));
extern unsigned int VUStripBuffer8x2CD_DmaTag __attribute__ ((section( ".vudata" )));
extern unsigned int VUStripBuffer1x3CD_DmaTag __attribute__ ((section( ".vudata" )));

/////////////////////////////////////////////////////////////////////
// Strip Buffer Code
/////////////////////////////////////////////////////////////////////

bool StripBuffer::Init(u32 flags)
{
	m_Flags = flags;

	m_PlatformData = new StripBufferPlatformData();
	if(!m_PlatformData) return false;
	m_PlatformData->mFlags = m_Flags;
	m_PlatformData->mNumStrips = 0;
	m_PlatformData->mNumBlocks = 0;
	m_PlatformData->mNumBlocksAlloced = 0;
	m_PlatformData->mMaxVerts = 0;
	m_PlatformData->mNumPasses = 0;
	m_PlatformData->mData = NULL;
	m_PlatformData->mBlocks = NULL;
	m_PlatformData->mOverBrightEnable = false;
	for(int i = 0; i < 8; i++)
	{
		m_PlatformData->mTextures[i] = NULL;
		m_PlatformData->mTextureBlendModes[i] = RS_BLENDFUNC_DEFAULT;
		m_PlatformData->mTextureFilters[i] = TS_FILTER_BILINEAR;
		m_PlatformData->mTextureMethod[i] = TS_METHOD_MODULATE;
		m_PlatformData->mWrapU[i] = TS_WRAP_CLAMP;
		m_PlatformData->mWrapV[i] = TS_WRAP_CLAMP;
	}
	return true;
}

/////////////////////////////////////////////////////////////////////

StripBuffer::StripBuffer() :
	m_PlatformData(NULL),
	m_VtxBuffer(NULL),
	m_TextureArray(NULL),
	m_TextureSubSet(0)
{
}

/////////////////////////////////////////////////////////////////////

StripBuffer::~StripBuffer()
{
	u32 i;

	if(!m_PlatformData)
		return;

	if(m_PlatformData->mData)
		memFree(m_PlatformData->mData);

	if(m_PlatformData->mBlocks)
		memFree(m_PlatformData->mBlocks);

	for(i = 0; i < m_PlatformData->mNumPasses; i++)
	{
		if(m_PlatformData->mPassData[i])
			memFree(m_PlatformData->mPassData[i]);
	}

	delete m_PlatformData;
}

/////////////////////////////////////////////////////////////////////

void StripBuffer::SetNumTexturePasses( u32 Passes )
{
	ASSERT(Passes == 1);
}

/////////////////////////////////////////////////////////////////////

void StripBuffer::Overbrighting(bool enabled)
{
	ASSERT(m_PlatformData);
	m_PlatformData->Overbrighting(enabled);
}

/////////////////////////////////////////////////////////////////////

void StripBuffer::SetVtxBuffer(VtxBuffer* vtxBuffer)
{
	// did you call Init()?
	ASSERT(m_PlatformData != NULL);

	// cant call this twice!
	ASSERT(m_PlatformData->mMaxVerts == 0);

	// get em..
	m_VtxBuffer = vtxBuffer;
	m_PlatformData->m_VtxBuffer= m_VtxBuffer;

	// initialize
	m_PlatformData->SetupState();
}

/////////////////////////////////////////////////////////////////////

void StripBuffer::SetPassTexture(u32 passNumber, Texture* texture, u8 method, u8 wrapU, u8 wrapV, u8 filter, u8 blend)
{
	// did you call Init()?
	ASSERT(m_PlatformData != NULL);

	bool needSetupState = false;

	if(
		m_PlatformData->mTextures[passNumber] != texture ||
		m_PlatformData->mTextureFilters[passNumber] != filter ||
		m_PlatformData->mTextureMethod[passNumber] != method ||
		m_PlatformData->mWrapV[passNumber] != wrapV ||
		m_PlatformData->mWrapU[passNumber] != wrapU)
		needSetupState = true;


	m_PlatformData->mTextureBlendModes[passNumber] = blend;
	m_PlatformData->mTextureMethod[passNumber] = method;
	m_PlatformData->mWrapU[passNumber] = wrapU;
	m_PlatformData->mWrapV[passNumber] = wrapV;
	m_PlatformData->mTextures[passNumber] = texture;
	m_PlatformData->mTextureFilters[passNumber] = filter;

	if(m_VtxBuffer && needSetupState)
	{
		m_PlatformData->SetupState();
	}
}

/////////////////////////////////////////////////////////////////////

// remove all strips
void StripBuffer::Reset(void)
{
	// did you call Init()?
	ASSERT(m_PlatformData != NULL);

	// remove them all...
	m_PlatformData->mNumStrips = 0;
	m_PlatformData->mNumBlocks = 0;
}

// note, numIndicies must be <= the value passed to AddStrip for this strip
u32 StripBuffer::ReplaceStrip(u32 index, const u16* indices, u16 numIndices)
{
	RemoveStrip(index);
	return AddStrip(indices, numIndices);
}
    


// remove a strip given a strip list
void StripBuffer::RemoveStrip(u32 stripIndex)
{
	// Removal of strips that were identified as quads is not supported on the PS2.
	ASSERT(stripIndex != 0xFFFFFFFF);

	// did you call Init()?
	ASSERT(m_PlatformData != NULL);
	// hunt strips that belong to this index..
	u32 block;
	for(block = 0; block < m_PlatformData->mNumBlocks; block++)
	{
		if(m_PlatformData->mBlocks[block].mStripIndex == stripIndex)
		{
			// remove this one and move the others down...
			memmove(&m_PlatformData->mBlocks[block], &m_PlatformData->mBlocks[block+1], sizeof(Block)*(m_PlatformData->mNumBlocksAlloced-block));
			m_PlatformData->mNumBlocks--;
		}
	}
}

/////////////////////////////////////////////////////////////////////

void StripBuffer::Render(const CInstance* pInstance, const Graphics4x4* pmLocalToCamera, float fZCamDist)
{
	// did you call Init()?
	ASSERT(m_PlatformData != NULL);

#ifdef DEBUG
	int i;
	for(i = 0; i < m_PlatformData->mNumPasses; i++)
	{
		// did you set the pass texture for this pass?
		ASSERT(m_PlatformData->mTextureBinds[i] != -1);
	}
#endif

	Matrix4x4 tex[8];
	tex[0].Identity();
	tex[1].Identity();
	tex[2].Identity();
	tex[3].Identity();
	tex[4].Identity();
	tex[5].Identity();
	tex[6].Identity();	
	tex[7].Identity();
	Render(*pmLocalToCamera, &tex[0], 0);
}



/////////////////////////////////////////////////////////////////////

// draw the strips...
void StripBuffer::Render(const Matrix4x4& modelMatrix, 
								 const Matrix4x4* texMatrices,
								 const u32 vertexOffset)
{
	// we need to make sure that the verts have been flushed from the cache
	// ICK!!!
	FlushCache(0);

	// did you call Init()?
	ASSERT(m_PlatformData != NULL);
	// did you call SetVtxBuffer()?
	ASSERT(m_VtxBuffer != NULL);

	// we dont have a compatible microcode.
	if(m_PlatformData->mVUCodeStart == NULL)
		return;

//	if(m_TextureArray)
//	{
//		TextureMgr::Cache().Lock( m_TextureArray );
//	}
//	TextureMgr::GetWhiteTexture()->Lock();

	RenderState__Request(RS_BLENDFUNC, m_PlatformData->mTextureBlendModes[0]);

//#ifndef CONSUMER_BUILD
#ifdef DEBUG_STRIPBUFFER
	if(StripBuffer_Dbg.forceWireFrame || RenderMgr__WireframeOn())
#else
	if(RenderMgr__WireframeOn())
#endif
	{
		// set test1 to only include the alpha stuff...
		GSState__Set(SCE_GS_TEST_1, SCE_GS_SET_TEST_1(
							 1, 	// ate, alpha test enable, 1 = on
							 0x7, 	// atst, alpha test method, 111 = notequal
							 0x80, // aref, alpha reference value for alpha test, 0x80 = opaque
							 0x0, 	// afail, what to do when alpha test fails, 0 = KEEP (transparent wireframe) 10=ZB_ONLY (opaque wireframe)
							 0, 	// date, destination alpha test, 0 = off, 1 = on
							 1, 	// datm, destination alpha test mode, 1 bit
							 1, 	// zte, depth test, 1 = on (TRC Requirement)
							 0x3));	// ztst, depth test method, 0 = never, 1 = always, 2 = gequal, 3 = greater
		// draw the lines dark (no alpha blending on the lines)
		GSState__Set(SCE_GS_ALPHA_1, SCE_GS_SET_ALPHA_1(2, 2, 0, 0, 0));
		GSState__Set(SCE_GS_PRMODECONT, 0);
#ifdef DEBUG_STRIPBUFFER
		GSState__Set(SCE_GS_PRMODE, SCE_GS_SET_PRMODE(1, !StripBuffer_Dbg.disableTextures, 0, 0, 1, 0, 0, 0));
#else
		GSState__Set(SCE_GS_PRMODE, SCE_GS_SET_PRMODE(1, 1, 0, 0, 1, 0, 0, 0));
#endif
	}
//#endif //CONSUMER_BUILD

	GSState__Send();
	m_PlatformData->SendMicrocode();
	Matrix4x4 finalMatrix = modelMatrix * g4WorldToCamera;

	m_PlatformData->SendState(finalMatrix, texMatrices);

	m_PlatformData->SendVerts();

//#ifndef CONSUMER_BUILD
#ifdef DEBUG_STRIPBUFFER
	if(StripBuffer_Dbg.forceWireFrame || RenderMgr__WireframeOn())
#else
	if(RenderMgr__WireframeOn())
#endif
	{
		// restore gs settings
		GSState__Set(SCE_GS_TEST_1, GSState.TEST_1);
		GSState__Set(SCE_GS_ALPHA_1, GSState.ALPHA_1);
		GSState__Set(SCE_GS_PRMODECONT, 1);
	}
//#endif //CONSUMER_BUILD
	GSState__Send();

//	TextureMgr::GetWhiteTexture()->Unlock();
//	TextureMgr::Cache().Unlock();
//	if (m_TextureArray)
//	{
//		//m_TextureArray->Unlock();
//	}
}

/////////////////////////////////////////////////////////////////////

// add a strip and return an index into the strip list for this strip
u32 StripBuffer::AddStrip(const u16* indices, u16 numIndices)
{
	// did you call Init()?
	ASSERT(m_PlatformData != NULL);
	// did you call SetVtxBuffer()?
	ASSERT(m_VtxBuffer != NULL);

	// cant have a strip of < 3 verts..:0
	ASSERT(numIndices > 2);

	u32 flags = 0;

	if(numIndices == 4 && 
		indices[0] + 1 == indices[1] && 
		indices[1] + 1 == indices[2] &&
		indices[2] + 1 == indices[3])
	{
		m_PlatformData->AddQuadBlock(indices[0], LINEAR_VERTEX_ORDER);
		return 0xFFFFFFFF;
	}

	if(indices[0] + 1 == indices[1])
	{
		flags |= LINEAR_VERTEX_ORDER;
		while(numIndices)
		{
			u16 index = indices[0];
			u32 numVerts = 1;

			while(numVerts < numIndices)
			{
				if(++index != indices[numVerts])
					break;
				numVerts++;
			}
			m_PlatformData->AddBlock(indices[0], 0, numVerts, m_PlatformData->mNumStrips, flags);

			flags |= STRIP_CONTINUATION;
			indices += numVerts;
			numIndices -= numVerts;
		}
	}
	else
	{
		while(numIndices)
		{
			if(numIndices == 1)
			{
				// one vert left...
				// just add it...
				m_PlatformData->AddBlock(*indices, 0, 1, m_PlatformData->mNumStrips, flags | LINEAR_VERTEX_ORDER);
				break;
			}

			u16 i1, i2;
			i1 = indices[0];
			i2 = indices[1];

			if(i1 > i2)
			{
				// these need to be in the other order, so we send down i1 as a single block, then continue on...
				m_PlatformData->AddBlock(*indices, 0, 1, m_PlatformData->mNumStrips, flags | LINEAR_VERTEX_ORDER);
				flags |= STRIP_CONTINUATION;
				indices++;
				numIndices--;
				continue;
			}

			// check for subset run thingy...
			// as long as we alternate between 2 sequential lists, we can
			// optimize.. :)...
			u32 numVerts = 2;
			while(numVerts < numIndices)
			{
				if(++i1 != indices[numVerts])
					break;

				// swap lists
				u16 t = i1;
				i1 = i2;
				i2 = t;

				numVerts++;
			}
			m_PlatformData->AddBlock(indices[0], indices[1], numVerts, m_PlatformData->mNumStrips, flags);

			flags |= STRIP_CONTINUATION;
			indices += numVerts;
			numIndices -= numVerts;
		}
	}

	return m_PlatformData->mNumStrips++;
}


/////////////////////////////////////////////////////////////////////
// StripBufferPlatformData code
/////////////////////////////////////////////////////////////////////

void StripBufferPlatformData::SetupState()
{
	u32 i;

	if(mData)
	{
		// release old state first...
		memFree(mData);

		for(i = 0; i < mNumPasses; i++)
		{
			memFree(mPassData[i]);
		}
	}

	mPassDataSizes[0] = 7; // 5 gs regs & 2 gif tags
	mPassDataSizes[1] = 7;
	mPassDataSizes[2] = 7;
	mPassDataSizes[3] = 7;
	mPassDataSizes[4] = 7;
	mPassDataSizes[5] = 7;
	mPassDataSizes[6] = 7;
	mPassDataSizes[7] = 7;

	mNumPasses = m_VtxBuffer->m_PlatformData->mNumPasses;
	mQWordsPerVtx = m_VtxBuffer->m_PlatformData->mVtxSize / 16;
	mData = (u32*)memAlloc(128);
	mVtxFlags = m_VtxBuffer->m_PlatformData->mVtxFlags;
	mTexFlags[0] = m_VtxBuffer->m_PlatformData->mTexFlags[0];
	mTexFlags[1] = m_VtxBuffer->m_PlatformData->mTexFlags[1];
	mTexFlags[2] = m_VtxBuffer->m_PlatformData->mTexFlags[2];
	mTexFlags[3] = m_VtxBuffer->m_PlatformData->mTexFlags[3];
	mTexFlags[4] = m_VtxBuffer->m_PlatformData->mTexFlags[4];
	mTexFlags[5] = m_VtxBuffer->m_PlatformData->mTexFlags[5];
	mTexFlags[6] = m_VtxBuffer->m_PlatformData->mTexFlags[6];
	mTexFlags[7] = m_VtxBuffer->m_PlatformData->mTexFlags[7];

	// pass state data
	mTotalPassDataSize = 0;
	for(i = 0; i < mNumPasses; i++)
		mTotalPassDataSize += mPassDataSizes[i];

	mNumQWords = mTotalPassDataSize + STATE_HEADER_SIZE + 2*4 + mNumPasses*4;
	u32 overheadPerBuffer = mTotalPassDataSize + 1;
	u32 totalQWPerVtx = (mQWordsPerVtx + 3*mNumPasses); // in & out of vu
	u32 totalOverhead = mNumQWords + overheadPerBuffer*2;
	u32 ClipBufferVertSize = mNumPasses + 2;
	u32 ClipBufferSize = (ClipBufferVertSize * MAX_CLIP_VERTS)*2 + CLIP_STACK_SIZE;
	mMaxVerts = (VU_MEM - totalOverhead) / (totalQWPerVtx*2);
	mMaxVertsClipped = (VU_MEM - totalOverhead - ClipBufferSize) / (totalQWPerVtx*2);
	mOffset = (VU_MEM - mNumQWords) / 2;
	mOffsetClipped = overheadPerBuffer + (totalQWPerVtx * mMaxVertsClipped);

	// fill in data block...
	memset(mData, 0, 128);
	mData[0] = 0x8000; // NOP gif tag
	mData[1] = 0; // flagsA
	mData[2] = 0; // fogShift;
	mData[3] = 0; // fogScale;
	for(i = 0; i < 8; i++)
	{
		if(i < mNumPasses)
			mData[4 + i] = mPassDataSizes[i];
		else
			mData[4 + i] = 0;
	}

	// pass state data follows (in order)...
	for(i = 0; i < mNumPasses; i++)
	{
		mPassData[i] = (u32*)memAlloc(mPassDataSizes[i] * 128);
		memset(mPassData[i], 0, mPassDataSizes[i]*128);
		// copy data into new buffer
		u_char* addr = (u_char*)mPassData[i];
		u_long tex0_1 = 0;
		u_long tex1_1 = 0;
		u_long clamp_1 = 0;
		u_long miptbp1_1 = 0;
		u_long miptbp2_1 = 0;

		Texture* texture = mTextures[i];
		if(texture == NULL)
			texture = TextureMgr::GetWhiteTexture();

		if(texture != NULL)
		{
			RenderState__Request(RS_BLENDFUNC, mTextureBlendModes[i]);
			TextureState__Request(0, TS_METHOD, mTextureMethod[i]);
			TextureState__Request(0, TS_WRAPU, mWrapU[i]);
			TextureState__Request(0, TS_WRAPV, mWrapV[i]);
			TextureState__Request(0, TS_FILTER, mTextureFilters[i]);
			texture->GetImage()->GetPlatformImage()->GetRegisterValues(NULL, &tex1_1, &clamp_1, &miptbp1_1, &miptbp2_1);
		}

		GIFTag *tag = (GIFTag*)addr;
		addr += 16;
		MasterList::Append8(addr, tex0_1);
		MasterList::Append8(addr, SCE_GS_TEX0_1);
		MasterList::Append8(addr, tex1_1);
		MasterList::Append8(addr, SCE_GS_TEX1_1);
		MasterList::Append8(addr, clamp_1);
		MasterList::Append8(addr, SCE_GS_CLAMP_1);
		MasterList::Append8(addr, miptbp1_1);
		MasterList::Append8(addr, SCE_GS_MIPTBP1_1);
		MasterList::Append8(addr, miptbp2_1);
		MasterList::Append8(addr, SCE_GS_MIPTBP2_1);
		tag->setFLG(GIFTag::FLG_PACKED);
		tag->setReg(0, GIFTag::REG_TEX0_1);
		tag->setReg(1, GIFTag::REG_A_D);
		tag->setReg(2, GIFTag::REG_CLAMP_1);
		tag->setReg(3, GIFTag::REG_A_D);
		tag->setReg(4, GIFTag::REG_A_D);
		tag->setNLOOP(1);
		tag->setNREGS(5);
		tag->setPRE(0);
		tag->setEOP(0);
		tag = (GIFTag*)addr;
		tag->setFLG(GIFTag::FLG_PACKED);
		tag->setPRE(0);
		tag->setNREGS(3);
		tag->setNLOOP(0);
		tag->setReg(0, GIFTag::REG_ST);
		tag->setReg(1, GIFTag::REG_RGBAQ);
		tag->setReg(2, GIFTag::REG_XYZ2);
		tag->setEOP(i+1 == mNumPasses);
		addr+= 16;
		ASSERT((u32)addr == (u32)&mPassData[i][mPassDataSizes[i]*4]);
	}


	mVUCodeStart = NULL;
	// Really, this shouldnt be based on what the microcode has, but rather on drawing mode requests
	//VtxBuffer::VERT_HAS_NORMAL|
	switch(mVtxFlags & (VtxBuffer::VERT_HAS_DIFFUSE|VtxBuffer::VERT_HAS_SPECULAR))
	{
	case VtxBuffer::VERT_HAS_DIFFUSE:
		switch(mTexFlags[0])
		{
		case VtxBuffer::VERT_TEX_DIM2:
			switch(mTexFlags[1])
			{
			case 0:
				mVUCodeStart = &VUStripBuffer1x2CD_DmaTag;
				break;
			case VtxBuffer::VERT_TEX_DIM2:
				switch(mTexFlags[2])
				{
				case 0:
					mVUCodeStart = &VUStripBuffer2x2CD_DmaTag;
					break;
				case VtxBuffer::VERT_TEX_DIM2:
					switch(mTexFlags[3])
					{
					case 0:
						mVUCodeStart = &VUStripBuffer3x2CD_DmaTag;
						break;
					case VtxBuffer::VERT_TEX_DIM2:
						switch(mTexFlags[4])
						{
						case 0:
							mVUCodeStart = &VUStripBuffer4x2CD_DmaTag;
							break;
// 						case VtxBuffer::VERT_TEX_DIM2:
// 							switch(mTexFlags[5])
// 							{
// 							case 0:
// 								mVUCodeStart = &VUStripBuffer5x2CD_DmaTag;
// 								break;
// 							case VtxBuffer::VERT_TEX_DIM2:
// 								switch(mTexFlags[6])
// 								{
// 								case 0:
// 									mVUCodeStart = &VUStripBuffer6x2CD_DmaTag;
// 									break;
// 								case VtxBuffer::VERT_TEX_DIM2:
// 									switch(mTexFlags[7])
// 									{
// 									case 0:
// 										mVUCodeStart = &VUStripBuffer7x2CD_DmaTag;
// 										break;
// 									case VtxBuffer::VERT_TEX_DIM2:
// 										mVUCodeStart = &VUStripBuffer8x2CD_DmaTag;
// 										break;
// 									}
// 									break;
// 								}
// 								break;
// 							}
// 							break;
						}
						break;
					}
					break;
				}
				break;
			}
			break;
		case VtxBuffer::VERT_TEX_DIM3:
			switch(mTexFlags[1])
			{
			case 0:
				mVUCodeStart = &VUStripBuffer1x3CD_DmaTag;
				break;
			}
		}
	}

	// Unsupported vtx buffer format
	ASSERT(mVUCodeStart != NULL);
}

/////////////////////////////////////////////////////////////////////

u32 StripBufferPlatformData::FindPriorQuadBlock(u16 index1, u32 flags)
{
	if(!mBlocks)
		return 0xFFFFFFFF;

	int block = 0;
	Block* currBlock = mBlocks;
	while((block < mNumBlocks) && 
			((currBlock->mVtxIndex1 + currBlock->mNumVerts != index1) ||
			(currBlock->mFlags != (flags | QUAD_BLOCK))))
	{
		currBlock++;
		block++;
	}
	if(block == mNumBlocks)
		return 0xFFFFFFFF;
	return block;
}

/////////////////////////////////////////////////////////////////////

void StripBufferPlatformData::AddQuadBlock(u16 index1, u32 flags)
{
	u32 blockNum = FindPriorQuadBlock(index1, flags);
	if(blockNum != 0xFFFFFFFF)
	{
		Block* thisBlock = &mBlocks[blockNum];

		// quad blocks must completely fit for now.
      if(thisBlock->mNumVerts + 4 < mMaxVertsClipped)
		{
			thisBlock->mNumVerts += 4;
			return;
		}
	}

	AddBlock(index1, 0, 4, -1, flags | QUAD_BLOCK);
}

/////////////////////////////////////////////////////////////////////

void StripBufferPlatformData::AddBlock(u16 index1, u16 index2, u32 vtxCount, u32 stripIndex, u32 flags)
{
	u32 blockNum = mNumBlocks++;
	if(blockNum >= mNumBlocksAlloced)
	{
		if(!mBlocks){
			mBlocks = (Block*)memAlloc(sizeof(Block) * INIT_BLOCK_LIST_LEN);
			mNumBlocksAlloced = INIT_BLOCK_LIST_LEN;
			ASSERT(mBlocks);
		} else {
			mNumBlocksAlloced += BLOCK_LIST_GROWTH;
			mBlocks = (Block*)memResize(mBlocks, sizeof(Block) * mNumBlocksAlloced);
			ASSERT(mBlocks);
		}
	}
	Block* thisBlock = &mBlocks[blockNum];
	thisBlock->mStripIndex = stripIndex;
	thisBlock->mNumVerts = vtxCount;
	thisBlock->mNumQWords = vtxCount * mQWordsPerVtx;
	thisBlock->mFlags = flags;
	thisBlock->mVtxIndex1 = index1;
	thisBlock->mVtxIndex2 = index2;
}

/////////////////////////////////////////////////////////////////////

void StripBufferPlatformData::SendMicrocode(void)
{
	ASSERT(mVUCodeStart);
	MasterList::AppendDMATAG(0, TAGID_CALL, mVUCodeStart);
	MasterList::Append(createNOP());
	MasterList::Append(createFLUSH());
	MasterList::SetFlushed(true);	PS2Renderer::JustTrashedVUInstructionMem(0, 4096);
}

/////////////////////////////////////////////////////////////////////

void StripBufferPlatformData::SendState(const Matrix4x4& modelMatrix, 
									 const Matrix4x4* texMatrices)
{
	u32 base = mNumQWords;
	u32 offset = 0;

#if 0
	u32 rsClipping = RenderState__Get(RS_VU_CLIPPING);
#else
	u32 rsClipping = RS_CLIPPING_CULL;
#endif

	// update the state for the new texture address.
	u_long tex0_1 = 0;
	u_long tex1_1 = 0;
	u_long clamp_1 = 0;
	u_long miptbp1_1 = 0;
	u_long miptbp2_1 = 0;
	for(int i = 0; i < mNumPasses; i++)
	{
		u_char* addr = (u_char*)mPassData[i];
		if(mTextures[i] != NULL)
		{
 			mTextures[i]->GetImage()->GetPlatformImage()->GetRegisterValues(&tex0_1, &tex1_1, &clamp_1, &miptbp1_1, &miptbp2_1);
			addr += 16;
			MasterList::Append8(addr, tex0_1);

			addr += 40;

			MasterList::Append8(addr, miptbp1_1);
			addr += 8;
			MasterList::Append8(addr, miptbp2_1);
		}
	}

	Matrix4x4 m1, m2;
	u32 temp = mData[1] & ~0x3;
	if(rsClipping == RS_CLIPPING_CLIP)
	{
// 		offset = mOffset;
// 		mData[1] = temp | CULL_MODE;
// 		m1 = modelMatrix * *(const Matrix4x4*)&PS2Renderer::GetCameraToScreen();
// 		m2 = modelMatrix * *(const Matrix4x4*)&PS2Renderer::GetCameraToCull();
		offset = mOffsetClipped;
		mData[1] = temp | CLIP_MODE;
		m1 = modelMatrix * *(const Matrix4x4*)&PS2Renderer::GetCameraToClip();
		m2 = *(const Matrix4x4*)&PS2Renderer::GetClipToScreen();
	}
	else if(rsClipping == RS_CLIPPING_CULL)
	{
		offset = mOffset;
		mData[1] = temp | CULL_MODE;
		m1 = modelMatrix * *(const Matrix4x4*)&PS2Renderer::GetCameraToScreen();
		m2 = modelMatrix * *(const Matrix4x4*)&PS2Renderer::GetCameraToCull();
	}
	else if(rsClipping == RS_CLIPPING_NONE)
	{
		offset = mOffset;
		mData[1] = temp | ALWAYS_MODE;
		m1 = modelMatrix * *(const Matrix4x4*)&PS2Renderer::GetCameraToScreen();
		m2 = modelMatrix * *(const Matrix4x4*)&PS2Renderer::GetCameraToCull();
	}
	else
	{
		// unknown clipping mode requested
		ASSERT(false);
	}

	u32 bufferSize = offset;
	
	if(!MasterList::IsBaseOffsetSame(base, offset))
	{
		MasterList::AppendDMATAG(1);
		MasterList::Append(createBASE(base));
		MasterList::Append(createOFFSET(offset));
		MasterList::Append(createITOP(bufferSize));
		MasterList::Append(createNOP());
		MasterList::Append(createNOP());
		MasterList::Append(createNOP());
		MasterList::SetBase(base);
		MasterList::SetOffset(offset);
	}

	MasterList::AppendDMATAG(mNumQWords + 3);
	MasterList::Append(createFLUSH(0));
	MasterList::Append(createDIRECT(2));
	MasterList::Append(createGIFTAG(1, 1, GIFTag::REG_PRIM));
			
#define FST false

	u16 striporfan;
	if(mFlags & STRIP_BUFFER_IS_FAN)
		striporfan = PRIM_TRIANGLE_FAN;
	else
		striporfan = PRIM_TRIANGLE_STRIP;

	//                               prim                 iip    tme   fge    abe   aa1    fst   ctxt  fix
#ifdef DEBUG_STRIPBUFFER
//#ifndef CONSUMER_BUILD
	if(StripBuffer_Dbg.forceWireFrame || RenderMgr__WireframeOn())
	{
		MasterList::Append8(SCE_GS_SET_PRIM(striporfan, true, !StripBuffer_Dbg.disableTextures, false, false, true, FST, 0, false));
	}
	else
//#endif // CONSUMER_BUILD
	{
		MasterList::Append8(SCE_GS_SET_PRIM(striporfan, true, !StripBuffer_Dbg.disableTextures, false, !StripBuffer_Dbg.disableAlpha, false, FST, 0, false));
	}
#else // DEBUG_STRIPBUFFER
//#ifndef CONSUMER_BUILD
	if(RenderMgr__WireframeOn())
	{
		MasterList::Append8(SCE_GS_SET_PRIM(striporfan, true, true, false, false, true, FST, 0, false));
	}
	else
//#endif // CONSUMER_BUILD
	{
		MasterList::Append8(SCE_GS_SET_PRIM(striporfan, true, true, false, true, false, FST, 0, false));
	}
#endif // DEBUG_STRIPBUFFER

	MasterList::Append4(0);
	MasterList::Append4(0);

	MasterList::Append(createNOP());
	MasterList::Append(createNOP());
	MasterList::Append(createNOP());
	MasterList::Append(createUNPACK(0, V4_32, mNumQWords, false));
	
	COPY_TO_MASTER_LIST(mData, STATE_HEADER_SIZE);
	COPY_TO_MASTER_LIST(&m1, 4);
	COPY_TO_MASTER_LIST(&m2, 4);
	COPY_TO_MASTER_LIST(texMatrices, 4*mNumPasses);

	for(int i = 0; i < mNumPasses; i++)
	{
		COPY_TO_MASTER_LIST(mPassData[i], mPassDataSizes[i]);
	}

}

/////////////////////////////////////////////////////////////////////
int StripBufferPlatformData::SendBlocks(u32 &block, int &startVert, bool reallysend, int maxverts)
{
	// see how many verts we can fit in the one tag...
	int vertcount = 0;

	int currblock = block;
	Block* currBlk = &mBlocks[currblock];

	int currvert = startVert;
	int adcBits;


	// deal with start vert != 0...
	// in this case, we need to dup 2 prior verts
	if(currvert > 0)
	{
		ASSERT(currvert > 2);
		if(reallysend)
			AppendVerts(currBlk, currvert-2, 2, 2);
		adcBits = 0;
		vertcount += 2;
	}
	// TJC -  deal with this block being a continuation of the prior
	else if(currBlk->mFlags & STRIP_CONTINUATION)
	{
		if(reallysend)
		{
			Block* priorBlock = currBlk-1;
			if(priorBlock->mNumVerts >= 2)
			{
				if((priorBlock->mNumVerts & 1)==0 || (priorBlock->mFlags & LINEAR_VERTEX_ORDER))
					AppendVerts(priorBlock, priorBlock->mNumVerts-2, 2, 2);
				else
				{
					AppendVerts(priorBlock, priorBlock->mNumVerts-2, 1, 1);
					AppendVerts(priorBlock, priorBlock->mNumVerts-1, 1, 1);
				}
			}
			else
			{
				AppendVerts(priorBlock-1, (priorBlock-1)->mNumVerts-1, 1, 1);
				AppendVerts(priorBlock, 0, 1, 1);
			}
		}

		adcBits = 0;
		vertcount += 2;
	}
	else
	{
		adcBits = 2;
	}

	for( ; currblock < mNumBlocks ; )
	{
		// how much of the block fits...
		if(currBlk->mNumVerts + vertcount - currvert > maxverts)
		{
			// doesnt all fit

			// quad blocks must completely fit for now.
			if(currBlk->mFlags & QUAD_BLOCK)
				break;

			// how many verts will we add?
			u32 numVerts = maxverts - vertcount;

			// avoid odd starts...
			if(numVerts & 1)
				numVerts--;

			if(numVerts <= 2)
				break;

			// append the verts
			if(reallysend)
				AppendVerts(currBlk, currvert, numVerts, adcBits);

			adcBits -= numVerts;
			if(adcBits < 0)
				adcBits = 0;

			// adjust start for next time
			currvert += numVerts;

			// verify no odd starts
			ASSERT((currvert & 1) == 0);

			// doesnt all fit
			vertcount += numVerts;
		}
		else
		{
			// does all fit

			u32 numVerts = currBlk->mNumVerts - currvert;

			// append the verts
			if(reallysend)
			{
				if(currBlk->mFlags & QUAD_BLOCK)
				{
					ASSERT(adcBits == 2);
					ASSERT((numVerts % 4) == 0);
					ASSERT(currvert == 0);
				}

				AppendVerts(currBlk, currvert, numVerts, adcBits);
			}

			// all fits...
			vertcount += numVerts;
			currvert = 0;

			adcBits -= numVerts;
			if(adcBits < 0)
				adcBits = 0;

			currblock++;
			currBlk++;

			// determine # of adcs needed
			if(!(currBlk->mFlags & STRIP_CONTINUATION))
				adcBits = 2;
		}


		ASSERT(vertcount <= maxverts);

		// is there room for at least 2 more tris?
		if(vertcount+4 >= maxverts)
			break;
	}

	if(reallysend)
	{
		block = currblock;
		startVert = currvert;
	}
	return vertcount;
}

/////////////////////////////////////////////////////////////////////
void StripBufferPlatformData::SendVerts()
{
	int kicknum = 0;
	u32 block = 0;
	int vertcount;
	int maxverts;
	int startVert = 0;

#if 0
	u32 rsClipping = RenderState__Get(RS_VU_CLIPPING);
#else
	u32 rsClipping = RS_CLIPPING_CULL;
#endif

	if(rsClipping == RS_CLIPPING_CLIP)
		maxverts = mMaxVertsClipped;
	else
		maxverts = mMaxVerts;

	do 
	{
		vertcount = SendBlocks(block, startVert, false, maxverts);


		// ok, now we know how many verts we will use.. lets use them...
		MasterList::AppendDMATAG(vertcount * mQWordsPerVtx + 2);

		MasterList::Append(createNOP());
		MasterList::Append(createNOP());
		MasterList::Append(createFLUSH());
		MasterList::Append(createFLUSH());

		if(kicknum == 0)
			MasterList::Append(createNOP());
		else if(kicknum == 1)
			MasterList::Append(createMSCAL(0));
		else
			MasterList::Append(createMSCNT());

		kicknum++;

		// the unpack for the verts & control word
		MasterList::Append(createUNPACK(0, V4_32, vertcount * mQWordsPerVtx + 1, true));

		// control QW
		MasterList::Append4(vertcount);
		MasterList::Append4(0); // flagsC
		MasterList::Append4(0); // flagsD
		MasterList::Append4(0); // flagsE

		// ok, now we need to add the verts
		SendBlocks(block, startVert, true, maxverts);

	} while(block != mNumBlocks);

	MasterList::AppendDMATAG(0);

	if(kicknum == 1)
		MasterList::Append(createMSCAL(0));
	else
		MasterList::Append(createMSCNT());

	MasterList::Append(createFLUSH());
}

/////////////////////////////////////////////////////////////////////
static void SwapVtx(u128* v1, u128* v2, u32 qwc)
{
	while(qwc--)
	{
		Math::Swap(*v1, *v2);
		v1++;
		v2++;
	}
}

/////////////////////////////////////////////////////////////////////
void StripBufferPlatformData::AppendVerts(Block* block, u32 startVertIdx, u32 vertCount, u32 adcCount)
{
	u32 index1 = block->mVtxIndex1;

	u32 wordsPerVtx = mQWordsPerVtx * 4;

	u32* vtxSrc = (u32*)&m_VtxBuffer->m_PlatformData->mBuffer[mQWordsPerVtx * index1];
	u32* destAddr = (u32*)MasterList::CurrentDMAPointer();
	
	// first the naive approach...
	if(block->mFlags & LINEAR_VERTEX_ORDER || vertCount == 1)
	{
		vtxSrc += startVertIdx * wordsPerVtx;

		// ok, just copy them into place...
		DMA_TO_SPR(vtxSrc, destAddr, vertCount * mQWordsPerVtx);
		DMA_Wait(D9_CHCR);
	}
	else
	{
		// this cant be odd
		ASSERT((startVertIdx & 1) == 0);

		vtxSrc += (startVertIdx >> 1) * wordsPerVtx;

		// ok, a little more compilcated... in this case, we need to dma them up, 
		// then rearrange them a little...
		u32 bufferWidth = block->mVtxIndex2 - index1;
		u32 halfVertsInFlightRU = (vertCount+1) >> 1;
		u32 halfVertsInFlightRD = (vertCount) >> 1;
		DMA_TO_SPR_INTERLEAVED(vtxSrc, destAddr, mQWordsPerVtx, halfVertsInFlightRU, 2, bufferWidth);
		DMA_Wait(D9_CHCR);

		// ok, they are up there now.. lets rearrange them...
		// they are ordered like:
		// 135246
		u128* buff1 = (u128*)destAddr;
		u128* buff2 = buff1 + halfVertsInFlightRU * mQWordsPerVtx;
		buff1 += mQWordsPerVtx;

		// move each even down into place one at a time... ICK!
		for(int i = 0; i < halfVertsInFlightRD; i++)
		{
			u128* bufft1 = buff2;
			while(bufft1 != buff1) {
				u128* bufft2 = bufft1 - mQWordsPerVtx;
				SwapVtx(bufft1, bufft2, mQWordsPerVtx);
				bufft1 = bufft2;
			}
			buff1 += 2 * mQWordsPerVtx;
			buff2 += mQWordsPerVtx;
		}

	}

	// now fix them up...
	if(block->mFlags & QUAD_BLOCK)
		FixupQuadBuffer(destAddr, vertCount);
	else
		FixupBuffer(destAddr, vertCount, adcCount);

	// move pointer along
	destAddr += vertCount * wordsPerVtx;
	MasterList::CurrentDMAPointer() = (u_char*)destAddr;
}


/////////////////////////////////////////////////////////////////////

void StripBufferPlatformData::Overbrighting(bool overbright)
{
	mOverBrightEnable = overbright;
}

/////////////////////////////////////////////////////////////////////

inline void StripBufferPlatformData::Fixup(u32* vtxAddr, u32 colorOffset, bool compressed, u128 mask)
{
	// fix color
	if(colorOffset)
	{
		if(compressed)
		{
			u32 temp1;
			u128 colr, vert;
			temp1 = *(vtxAddr + colorOffset);
			vert = *((u128*)vtxAddr);
			
			// todo: shrink output vtx when its supported...

			// mask off the lower byte of each word of the first quadword
			asm("pand   %0, %1, %2" : "=r" (vert) : "r" (vert), "r" (mask));
			
			// divide the color by 2 and unpack it 
			asm("pextlb %0, $0, %1" : "=r" (colr) : "r" (temp1)); // clr = 0x

			if(!mOverBrightEnable)
			{
				asm("psrlh  %0, %1,  1" : "=r" (colr) : "r" (colr));
			}

			asm("pextlh %0, $0, %1" : "=r" (colr) : "r" (colr));

			// mask color
			asm("pnor   %0, $0, %1" : "=r" (mask) : "r" (mask));
			asm("pand   %0, %1, %2" : "=r" (colr) : "r" (colr), "r" (mask));
			
			// or them together
			asm("por    %0, %1, %2" : "=r" (vert) : "r" (vert), "r" (colr));

			*((u128*)vtxAddr) = vert;
		}
		else
		{
			ASSERT(false);
		}
	}

	// clear adc bit... (leaving us with 7888 colors)
	*vtxAddr &= ~1;
}

/////////////////////////////////////////////////////////////////////

void StripBufferPlatformData::FixupBuffer(u32* vtxAddr, u32 vtxCount, u32 adcCount)
{
	u128 mask;
	s64 minus1 = -1;
	asm("pextlb %0, $0, %1" : "=r" (mask) : "r" (minus1));		 // mask = 0x00FF00FF, 0x00FF00FF, 0x00FF00FF, 0x00FF00FF
	asm("pextlb %0, $0, %1" : "=r" (mask) : "r" (mask));			 // mask = 0x000000FF, 0x000000FF, 0x000000FF, 0x000000FF
	asm("pnor   %0, $0, %1" : "=r" (mask) : "r" (mask));			 // mask = 0xFFFFFF00, 0xFFFFFF00, 0xFFFFFF00, 0xFFFFFF00

	u32 colorOffset = 0;
	if(mVtxFlags & VtxBuffer::VERT_HAS_DIFFUSE)
		colorOffset = m_VtxBuffer->m_PlatformData->mDiffuseOffset >> 2;

	while(vtxCount--)
	{
		Fixup(vtxAddr, colorOffset, true, mask);

		if(adcCount > 0)
		{
			adcCount--;
			vtxAddr[0] |= 1;
		}

		u32 wordsPerVtx = mQWordsPerVtx * 4;
		vtxAddr += wordsPerVtx;
	}
}

/////////////////////////////////////////////////////////////////////

void StripBufferPlatformData::FixupQuadBuffer(u32* vtxAddr, u32 vtxCount)
{
	u128 mask;
	s64 minus1 = -1;
	asm("pextlb %0, $0, %1" : "=r" (mask) : "r" (minus1));		 // mask = 0x00FF00FF, 0x00FF00FF, 0x00FF00FF, 0x00FF00FF
	asm("pextlb %0, $0, %1" : "=r" (mask) : "r" (mask));			 // mask = 0x000000FF, 0x000000FF, 0x000000FF, 0x000000FF
	asm("pnor   %0, $0, %1" : "=r" (mask) : "r" (mask));			 // mask = 0xFFFFFF00, 0xFFFFFF00, 0xFFFFFF00, 0xFFFFFF00

	u32 colorOffset = 0;
	if(mVtxFlags & VtxBuffer::VERT_HAS_DIFFUSE)
		colorOffset = m_VtxBuffer->m_PlatformData->mDiffuseOffset >> 2;

	while(vtxCount--)
	{
		Fixup(vtxAddr, colorOffset, true, mask);

		// vtxCount starts with %4==0
		if(vtxCount & 0x2) // first 2 verts should get ADC, next two not, next two yes, next two not
		{
			vtxAddr[0] |= 1;
		}

		u32 wordsPerVtx = mQWordsPerVtx * 4;
		vtxAddr += wordsPerVtx;
	}
}

