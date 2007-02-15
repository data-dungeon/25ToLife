///////////////////////////////////////////////////////////////////////////
//
// VtxBuffer.hpp
//
// Vertex Buffer Module.
//
// Nate Robins, March 2002.
//
////////////////////////////////////////////////////////////////////////////

#include "Display/DirectX/DynamicVB.h"

// this thing is private, dont futz
class VtxBufferPlatformData
{
public:
	VtxBufferPlatformData( );
	~VtxBufferPlatformData( );

	static void				createDeviceObjects( void );
	static void				deleteDeviceObjects( void );

	bool						Init( u32 vtxCount, u32 vtxFlags, u32 texFlags[8] );
	unsigned int			Load( void );

	unsigned char *         m_Vertices;

	u32                     m_BufferSize;
	u32                     m_Format;
	u32                     m_Size;
	u32                     m_NumVertices;
	u32							m_NumPasses;
	u32							m_DiffuseOffset;
	u32							m_SpecularOffset;
	u32							m_NormalOffset;
	u32							m_TexCoordOffset[8];
	
private:
	static DynamicVB *			p_UserVB;
	static unsigned int			numVBVertexBytes;

	void						setSizeAndFormat( u32 vtxFlags, u32 texFlags[8] );

	void						initDXDevice( IDirect3DDevice8 * pNewDXDevice );
	void						releaseDXDevice( void );
};

////////////////////////////////////////////////////////////////////////////
//  Public
////////////////////////////////////////////////////////////////////////////

inline u32 VtxBuffer::cvtRGBA( u32 color ) { 
	return	color & 0xff00ff00 |
				(((color >> 16) & 0xff )) |
				(((color      ) & 0xff ) << 16)				
				; }

////////////////////////////////////////////////////////////////////////////

inline u32& VtxBuffer::Diffuse(u32 vtx)
{
    ASSERT(m_PlatformData->m_Vertices != 0);
    ASSERT((vtx * m_PlatformData->m_Size + m_PlatformData->m_DiffuseOffset + sizeof(u32)) <= m_PlatformData->m_BufferSize);

    return *((u32*)&(m_PlatformData->m_Vertices[vtx * m_PlatformData->m_Size + m_PlatformData->m_DiffuseOffset]));
}

////////////////////////////////////////////////////////////////////////////

inline u32& VtxBuffer::Specular(u32 vtx)
{
    ASSERT(m_PlatformData->m_Vertices != 0);
    ASSERT((vtx * m_PlatformData->m_Size + m_PlatformData->m_SpecularOffset + sizeof(u32)) <= m_PlatformData->m_BufferSize);

    return *((u32*)&(m_PlatformData->m_Vertices[vtx * m_PlatformData->m_Size + m_PlatformData->m_SpecularOffset]));
}

////////////////////////////////////////////////////////////////////////////

inline Vector3& VtxBuffer::Normal(u32 vtx)
{
    ASSERT(m_PlatformData->m_Vertices != 0);
    ASSERT((vtx * m_PlatformData->m_Size + m_PlatformData->m_NormalOffset + sizeof(Vector3)) <= m_PlatformData->m_BufferSize);

    return *((Vector3*)&(m_PlatformData->m_Vertices[vtx * m_PlatformData->m_Size + m_PlatformData->m_NormalOffset]));
}

////////////////////////////////////////////////////////////////////////////

inline Vector3& VtxBuffer::Vertex(u32 vtx)
{
    ASSERT(m_PlatformData->m_Vertices != 0);
    ASSERT((vtx * m_PlatformData->m_Size + 0 + sizeof(Vector3)) <= m_PlatformData->m_BufferSize);

    return *((Vector3*)&(m_PlatformData->m_Vertices[vtx * m_PlatformData->m_Size + 0]));
}

////////////////////////////////////////////////////////////////////////////

inline float& VtxBuffer::TexCoordf(u32 layer, u32 vtx)
{
    ASSERT(m_PlatformData->m_Vertices != 0);
    ASSERT(layer < 8);
    ASSERT((vtx * m_PlatformData->m_Size + m_PlatformData->m_TexCoordOffset[layer] + sizeof(float)) <= m_PlatformData->m_BufferSize);

    return *((float*)&(m_PlatformData->m_Vertices[vtx * m_PlatformData->m_Size + m_PlatformData->m_TexCoordOffset[layer]]));
}

////////////////////////////////////////////////////////////////////////////

inline u32 VtxBuffer::GetVtxCount( void )
{
	return m_PlatformData->m_NumVertices;
}

////////////////////////////////////////////////////////////////////////////

inline Vector2& VtxBuffer::TexCoordv2(u32 layer, u32 vtx)
{
    ASSERT(m_PlatformData->m_Vertices != 0);
    ASSERT(layer < 8);
    ASSERT((vtx * m_PlatformData->m_Size + m_PlatformData->m_TexCoordOffset[layer] + sizeof(Vector2)) <= m_PlatformData->m_BufferSize);

    return *((Vector2*)&(m_PlatformData->m_Vertices[vtx * m_PlatformData->m_Size + m_PlatformData->m_TexCoordOffset[layer]]));
}

////////////////////////////////////////////////////////////////////////////

inline Vector3& VtxBuffer::TexCoordv3(u32 layer, u32 vtx)
{
    ASSERT(m_PlatformData->m_Vertices != 0);
    ASSERT(layer < 8);
    ASSERT((vtx * m_PlatformData->m_Size + m_PlatformData->m_TexCoordOffset[layer] + sizeof(Vector3)) <= m_PlatformData->m_BufferSize);

    return *((Vector3*)&(m_PlatformData->m_Vertices[vtx * m_PlatformData->m_Size + m_PlatformData->m_TexCoordOffset[layer]]));
}

////////////////////////////////////////////////////////////////////////////

inline Vector4& VtxBuffer::TexCoordv4(u32 layer, u32 vtx)
{
    ASSERT(m_PlatformData->m_Vertices != 0);
    ASSERT(layer < 8);
    ASSERT((vtx * m_PlatformData->m_Size + m_PlatformData->m_TexCoordOffset[layer] + sizeof(Vector4)) <= m_PlatformData->m_BufferSize);

    return *((Vector4*)&(m_PlatformData->m_Vertices[vtx * m_PlatformData->m_Size + m_PlatformData->m_TexCoordOffset[layer]]));
}

////////////////////////////////////////////////////////////////////////////
