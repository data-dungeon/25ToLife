///////////////////////////////////////////////////////////////////////////
//
// VtxBuffer.hpp
//
// Vertex Buffer Module.
//
// Nate Robins, March 2002.
//
////////////////////////////////////////////////////////////////////////////


// private .. do not futz with it
class VtxBufferPlatformData
{
public:
	VtxBufferPlatformData();
	~VtxBufferPlatformData();
	bool Init(u32 vtxCount, u32 vtxFlags, u32 texFlags[8]);

	u32		m_VtxFlags;
	u32		m_TexFlags[8];
	
	u32		m_Size;						// size of an individual vertex (in bytes)
	u32		m_NumVertices;				// number of vertices in the buffer
	u8			*m_Vertices;				// pointer to vertices
	u32		m_VtxBuffSize;				// total size of vertex buffer
	
	u32		m_NumPasses;
	
	u32		m_DiffuseOffset;        // offset in a vertex at which diffuse component begins (in bytes)
	u32		m_SpecularOffset;       // offset in a vertex at which specular component begins (in bytes)
	u32		m_NormalOffset;         // offset in a vertex at which normal component begins (in bytes)
	u32		m_TexCoordOffset[8];    // offset in a vertex at which texcoord component begins (in bytes)
	u32		m_TexCoordDimension[8]; // dimension of the texture coordinate
	
	u32		m_Stride;               // stride between components in a vertex (in bytes)

	bool		m_bRendering;				// true if vtxBuffer is used as render source
    
	void SizeAndStride(u32 vtxFlags, u32 texFlags[8]);
};

////////////////////////////////////////////////////////////////////////////
//  Public
////////////////////////////////////////////////////////////////////////////

inline u32 VtxBuffer::cvtRGBA( u32 color ) { return color; }

////////////////////////////////////////////////////////////////////////////

inline u32 VtxBuffer::GetVtxCount( void )
{
	return m_PlatformData->m_NumVertices;
}

////////////////////////////////////////////////////////////////////////////

inline u32& VtxBuffer::Diffuse(u32 vtx)
{
    ASSERT(m_PlatformData->m_Vertices != 0);
    ASSERT((vtx * m_PlatformData->m_Size + m_PlatformData->m_DiffuseOffset + sizeof(u32)) <= m_PlatformData->m_Size * m_PlatformData->m_NumVertices);

    return *((u32*)&(m_PlatformData->m_Vertices[vtx * m_PlatformData->m_Size + m_PlatformData->m_DiffuseOffset]));
}

////////////////////////////////////////////////////////////////////////////

inline u32& VtxBuffer::Specular(u32 vtx)
{
    ASSERT(m_PlatformData->m_Vertices != 0);
    ASSERT((vtx * m_PlatformData->m_Size + m_PlatformData->m_SpecularOffset + sizeof(u32)) <= m_PlatformData->m_Size * m_PlatformData->m_NumVertices);

    return *((u32*)&(m_PlatformData->m_Vertices[vtx * m_PlatformData->m_Size + m_PlatformData->m_SpecularOffset]));
}

////////////////////////////////////////////////////////////////////////////

inline Vector3& VtxBuffer::Normal(u32 vtx)
{
    ASSERT(m_PlatformData->m_Vertices != 0);
    ASSERT((vtx * m_PlatformData->m_Size + m_PlatformData->m_NormalOffset + sizeof(Vector3)) <= m_PlatformData->m_Size * m_PlatformData->m_NumVertices);

    return *((Vector3*)&(m_PlatformData->m_Vertices[vtx * m_PlatformData->m_Size + m_PlatformData->m_NormalOffset]));
}

////////////////////////////////////////////////////////////////////////////

inline Vector3& VtxBuffer::Vertex(u32 vtx)
{
    ASSERT(m_PlatformData->m_Vertices != 0);
    ASSERT((vtx * m_PlatformData->m_Size + 0 + sizeof(Vector3)) <= m_PlatformData->m_Size * m_PlatformData->m_NumVertices);

    return *((Vector3*)&(m_PlatformData->m_Vertices[vtx * m_PlatformData->m_Size + 0]));
}

////////////////////////////////////////////////////////////////////////////

inline float& VtxBuffer::TexCoordf(u32 layer, u32 vtx)
{
    ASSERT(m_PlatformData->m_Vertices != 0);
    ASSERT(layer < 8);
    ASSERT((vtx * m_PlatformData->m_Size + m_PlatformData->m_TexCoordOffset[layer] + sizeof(float)) <= m_PlatformData->m_Size * m_PlatformData->m_NumVertices);

    return *((float*)&(m_PlatformData->m_Vertices[vtx * m_PlatformData->m_Size + m_PlatformData->m_TexCoordOffset[layer]]));
}

////////////////////////////////////////////////////////////////////////////

inline Vector2& VtxBuffer::TexCoordv2(u32 layer, u32 vtx)
{
    ASSERT(m_PlatformData->m_Vertices != 0);
    ASSERT(layer < 8);
    ASSERT((vtx * m_PlatformData->m_Size + m_PlatformData->m_TexCoordOffset[layer] + sizeof(Vector2)) <= m_PlatformData->m_Size * m_PlatformData->m_NumVertices);

    return *((Vector2*)&(m_PlatformData->m_Vertices[vtx * m_PlatformData->m_Size + m_PlatformData->m_TexCoordOffset[layer]]));
}

////////////////////////////////////////////////////////////////////////////

inline Vector3& VtxBuffer::TexCoordv3(u32 layer, u32 vtx)
{
    ASSERT(m_PlatformData->m_Vertices != 0);
    ASSERT(layer < 8);
    ASSERT((vtx * m_PlatformData->m_Size + m_PlatformData->m_TexCoordOffset[layer] + sizeof(Vector3)) <= m_PlatformData->m_Size * m_PlatformData->m_NumVertices);

    return *((Vector3*)&(m_PlatformData->m_Vertices[vtx * m_PlatformData->m_Size + m_PlatformData->m_TexCoordOffset[layer]]));
}

////////////////////////////////////////////////////////////////////////////

inline Vector4& VtxBuffer::TexCoordv4(u32 layer, u32 vtx)
{
    ASSERT(m_PlatformData->m_Vertices != 0);
    ASSERT(layer < 8);
    ASSERT((vtx * m_PlatformData->m_Size + m_PlatformData->m_TexCoordOffset[layer] + sizeof(Vector4)) <= m_PlatformData->m_Size * m_PlatformData->m_NumVertices);

    return *((Vector4*)&(m_PlatformData->m_Vertices[vtx * m_PlatformData->m_Size + m_PlatformData->m_TexCoordOffset[layer]]));
}

////////////////////////////////////////////////////////////////////////////
