///////////////////////////////////////////////////////////////////////////
//
// VtxBuffer.cpp
//
// Vertex Buffer Module.
//
// Nate Robins, March 2002.
//
// Copyright (c) 2002 Avalanche Software.
//
////////////////////////////////////////////////////////////////////////////

#ifndef DIRECTX
#pragma message("WARNING: DIRECTX should be defined when compiling this module!")
#endif

////////////////////////////////////////////////////////////////////////////
//  Includes
////////////////////////////////////////////////////////////////////////////

#include "Display/DisplayPCH.h"

#include "DXPublished.h"



////////////////////////////////////////////////////////////////////////////
//  Public
////////////////////////////////////////////////////////////////////////////

VtxBuffer::VtxBuffer( )
	: m_PlatformData( NULL )
{
}

////////////////////////////////////////////////////////////////////////////

VtxBuffer::~VtxBuffer( )
{
	if( m_PlatformData ) 
		delete m_PlatformData;
}

////////////////////////////////////////////////////////////////////////////

bool VtxBuffer::Init( u32 vtxCount, u32 vtxFlags, u32 texFlags[VTX_BUFFER_TEXTURE_LAYERS] )
{
	ASSERT( m_PlatformData == NULL );

	m_PlatformData = new VtxBufferPlatformData( );
	ASSERT( m_PlatformData );

	return m_PlatformData->Init( vtxCount, vtxFlags, texFlags );
}

////////////////////////////////////////////////////////////////////////////

void VtxBuffer::Lock( void )
{
	ASSERT( m_PlatformData );
	ASSERT( m_PlatformData->m_Vertices != 0 );
}

////////////////////////////////////////////////////////////////////////////

void VtxBuffer::UnLock( void )
{
	ASSERT( m_PlatformData );
	ASSERT( m_PlatformData->m_Vertices != 0 );
}


////////////////////////////////////////////////////////////////////////////
// PlatformData
////////////////////////////////////////////////////////////////////////////

// statics used by derived classes 
DynamicVB *				VtxBufferPlatformData::p_UserVB = 0;
unsigned int			VtxBufferPlatformData::numVBVertexBytes = 200 * 1024;

void VtxBufferPlatformData::createDeviceObjects( void )
{
	ASSERT( DirectX_Device );
	ASSERT( p_UserVB == 0 );

	p_UserVB = new DynamicVB( DirectX_Device, numVBVertexBytes, 0, true );
	DXPublished::addLostDeviceCallback( p_UserVB );

}

void VtxBufferPlatformData::deleteDeviceObjects( void )
{
	if( p_UserVB )
	{
		DXPublished::removeLostDeviceCallback( p_UserVB );
		delete p_UserVB;

		p_UserVB = 0;
	}
}

VtxBufferPlatformData::VtxBufferPlatformData( )
{
    m_Vertices				= 0;

    m_NormalOffset      = -1;
    m_DiffuseOffset     = -1;
    m_SpecularOffset    = -1;
    m_TexCoordOffset[0] = -1;
    m_TexCoordOffset[1] = -1;
    m_TexCoordOffset[2] = -1;
    m_TexCoordOffset[3] = -1;
    m_TexCoordOffset[4] = -1;
    m_TexCoordOffset[5] = -1;
    m_TexCoordOffset[6] = -1;
    m_TexCoordOffset[7] = -1;
}

VtxBufferPlatformData::~VtxBufferPlatformData( )
{
	if( m_Vertices )
		memFree( m_Vertices );
}

bool VtxBufferPlatformData::Init( u32 vtxCount, u32 vtxFlags, u32 texFlags[VTX_BUFFER_TEXTURE_LAYERS] )
{
	setSizeAndFormat( vtxFlags, texFlags );
	
	m_NumVertices = vtxCount;
	m_BufferSize = m_Size * m_NumVertices;
	
	m_Vertices = (unsigned char *) memAlloc( m_BufferSize );
	ASSERT( m_Vertices );
	
	return true;
}

unsigned int VtxBufferPlatformData::Load( void )
{
	ASSERT( m_Vertices );
	ASSERT( p_UserVB );

	unsigned int baseVertex;

	void * pVertexMem = p_UserVB->lock( m_Size, m_NumVertices, baseVertex );
	ASSERT( pVertexMem );

	memcpy( pVertexMem, m_Vertices, m_Size * m_NumVertices );

	p_UserVB->unlock( );

	HRESULT result = D3D_OK;

	result = DirectX_Device->SetStreamSource( 0, (*p_UserVB), m_Size );
	ASSERT( result == D3D_OK );

	result = DirectX_Device->SetVertexShader( m_Format );
	ASSERT( result == D3D_OK );

	return baseVertex;
}

void VtxBufferPlatformData::setSizeAndFormat( u32 vtxFlags, u32 texFlags[VTX_BUFFER_TEXTURE_LAYERS] )
{
    m_Format = D3DFVF_XYZ;
    m_Size   = 3 * sizeof( float );
    
    if( vtxFlags & VtxBuffer::VERT_HAS_NORMAL )
    {
        m_Format |= D3DFVF_NORMAL;
        m_NormalOffset = m_Size;
        m_Size += 3 * sizeof( float );
    }

    if( vtxFlags & VtxBuffer::VERT_HAS_DIFFUSE )
    {
        m_Format |= D3DFVF_DIFFUSE;
        m_DiffuseOffset = m_Size;
        m_Size += sizeof( DWORD );
    }
    if( vtxFlags & VtxBuffer::VERT_HAS_SPECULAR )
    {
        m_Format |= D3DFVF_SPECULAR;
        m_SpecularOffset = m_Size;
        m_Size += sizeof( DWORD );
    }
    
    m_NumPasses = 0;
    for( int i = 0; i < 8; i++ )
    {
        u32 t = texFlags[i];
        
        if( t & VtxBuffer::VERT_TEX_DIM1 )
        {
            m_Format |= D3DFVF_TEXCOORDSIZE1( i );
            m_TexCoordOffset[i] = m_Size;
            m_Size += 1 * sizeof( float );
            m_NumPasses++;
        }
        if( t & VtxBuffer::VERT_TEX_DIM2 )
        {
            m_Format |= D3DFVF_TEXCOORDSIZE2( i );
            m_TexCoordOffset[i] = m_Size;
            m_Size += 2 * sizeof( float );
            m_NumPasses++;
        }
        if( t & VtxBuffer::VERT_TEX_DIM3 )
        {
            m_Format |= D3DFVF_TEXCOORDSIZE3( i );
            m_TexCoordOffset[i] = m_Size;
            m_Size += 3 * sizeof( float );
            m_NumPasses++;
        }
        if( t & VtxBuffer::VERT_TEX_DIM4 )
        {
            m_Format |= D3DFVF_TEXCOORDSIZE4( i );
            m_TexCoordOffset[i] = m_Size;
            m_Size += 4 * sizeof( float );
            m_NumPasses++;
        }
    }
    
#ifdef _XBOX
    // Xbox only allows up to 4 texture coordinate sets per vertex.
    int numTCSets = Math::Min(4, (int) m_NumPasses );
#else
    int numTCSets = m_NumPasses;
#endif
    
    switch( numTCSets )
    {
    case 0:
        m_Format |= D3DFVF_TEX0;
        break;
    case 1:
        m_Format |= D3DFVF_TEX1;
        break;
    case 2:
        m_Format |= D3DFVF_TEX2;
        break;
    case 3:
        m_Format |= D3DFVF_TEX3;
        break;
    case 4:
        m_Format |= D3DFVF_TEX4;
        break;
#ifndef _XBOX
    case 5:
        m_Format |= D3DFVF_TEX5;
        break;
    case 6:
        m_Format |= D3DFVF_TEX6;
        break;
    case 7:
        m_Format |= D3DFVF_TEX7;
        break;
    case 8:
        m_Format |= D3DFVF_TEX8;
        break;
#endif
    default:
        ASSERT( 0 );
        break;
    }
}

////////////////////////////////////////////////////////////////////////////
