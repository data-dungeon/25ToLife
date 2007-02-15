
#define WORD_SIZE 4

class VtxBufferPlatformData
{
	// DONT FUTZ WITH THIS!
public:
	~VtxBufferPlatformData();
	bool Init(u32 vtxCount, u32 vtxFlags, u32 texFlags[8]);

	u32 mVtxCount;
	u32 mVtxFlags;
	u32 mTexFlags[8];
	u_long128 *mBuffer;			// buffer holding vertices
	u32 mVtxSize;			// size of a vertex in bytes. (stride)
	u32 mNumPasses;
	u32 mTexOffset[8];
	u32 mDiffuseOffset;
	u32 mSpecularOffset;
	bool mLocked;
};

inline u32 VtxBuffer::cvtRGBA( u32 color ) { return color; }

////////////////////////////////////////////////////////////////////////////

inline u32 VtxBuffer::GetVtxCount( void )
{
	return m_PlatformData->mVtxCount;
}

////////////////////////////////////////////////////////////////////////////

inline u32& VtxBuffer::Diffuse(u32 vtx)
{
	ASSERT(m_PlatformData);
	ASSERT(m_PlatformData->mLocked);
	ASSERT(m_PlatformData->mVtxFlags & VERT_HAS_DIFFUSE);
	ASSERT(vtx < m_PlatformData->mVtxCount);
	return *(u32*)&((char*)m_PlatformData->mBuffer)[m_PlatformData->mVtxSize*vtx + m_PlatformData->mDiffuseOffset];
}

////////////////////////////////////////////////////////////////////////////

inline u32& VtxBuffer::Specular(u32 vtx)
{
	ASSERT(m_PlatformData->mVtxFlags & VERT_HAS_SPECULAR);
	ASSERT(m_PlatformData->mLocked);
	ASSERT(vtx < m_PlatformData->mVtxCount);
	return *(u32*)&((char*)m_PlatformData->mBuffer)[m_PlatformData->mVtxSize*vtx + m_PlatformData->mSpecularOffset];
}

////////////////////////////////////////////////////////////////////////////

inline Vector3Packed& VtxBuffer::Normal(u32 vtx)
{
	ASSERT(m_PlatformData->mVtxFlags & VERT_HAS_NORMAL);
	ASSERT(m_PlatformData->mLocked);
	ASSERT(vtx < m_PlatformData->mVtxCount);

	// the normal (if there is one), always follows immediately after the vtx..
	return *(Vector3Packed*)&((char*)m_PlatformData->mBuffer)[m_PlatformData->mVtxSize*vtx + (WORD_SIZE*3)];
}

////////////////////////////////////////////////////////////////////////////

inline Vector3Packed& VtxBuffer::Vertex(u32 vtx)
{
	ASSERT(vtx < m_PlatformData->mVtxCount);
	ASSERT(m_PlatformData->mLocked);

	// the vertex always follow immeidately after the first unpack...
	return *(Vector3Packed*)&((char*)m_PlatformData->mBuffer)[m_PlatformData->mVtxSize*vtx];
}

////////////////////////////////////////////////////////////////////////////

inline float& VtxBuffer::TexCoordf(u32 layer, u32 vtx)
{
	ASSERT(m_PlatformData->mTexFlags[layer] & VERT_TEX_DIM1);
	ASSERT(m_PlatformData->mLocked);
	ASSERT(vtx < m_PlatformData->mVtxCount);
	return *(float*)&((char*)m_PlatformData->mBuffer)[m_PlatformData->mVtxSize*vtx + m_PlatformData->mTexOffset[layer]];
}

////////////////////////////////////////////////////////////////////////////

inline Vector2& VtxBuffer::TexCoordv2(u32 layer, u32 vtx)
{
	ASSERT(m_PlatformData->mTexFlags[layer] & VERT_TEX_DIM2);
	ASSERT(m_PlatformData->mLocked);
	ASSERT(vtx < m_PlatformData->mVtxCount);
	return *(Vector2*)&((char*)m_PlatformData->mBuffer)[m_PlatformData->mVtxSize*vtx + m_PlatformData->mTexOffset[layer]];
}

////////////////////////////////////////////////////////////////////////////

inline Vector3Packed& VtxBuffer::TexCoordv3(u32 layer, u32 vtx)
{
	ASSERT(m_PlatformData->mTexFlags[layer] & VERT_TEX_DIM3);
	ASSERT(m_PlatformData->mLocked);
	ASSERT(vtx < m_PlatformData->mVtxCount);
	return *(Vector3Packed*)&((char*)m_PlatformData->mBuffer)[m_PlatformData->mVtxSize*vtx + m_PlatformData->mTexOffset[layer]];
}

////////////////////////////////////////////////////////////////////////////

inline Vector4& VtxBuffer::TexCoordv4(u32 layer, u32 vtx)
{
	ASSERT(m_PlatformData->mTexFlags[layer] & VERT_TEX_DIM4);
	ASSERT(m_PlatformData->mLocked);
	ASSERT(vtx < m_PlatformData->mVtxCount);
	return *(Vector4*)&((char*)m_PlatformData->mBuffer)[m_PlatformData->mVtxSize*vtx + m_PlatformData->mTexOffset[layer]];
}


