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

#ifndef GCN
#pragma message("WARNING: GCN should be defined when compiling this module!")
#endif

////////////////////////////////////////////////////////////////////////////
//  Includes
////////////////////////////////////////////////////////////////////////////

/******************* includes ****************************/

#include "Display/DisplayPCH.h"

#include "Display/GCN/FrameData.h"

////////////////////////////////////////////////////////////////////////////
//  Public
////////////////////////////////////////////////////////////////////////////

VtxBuffer::VtxBuffer()
	: m_PlatformData(NULL)
{
}

////////////////////////////////////////////////////////////////////////////

VtxBuffer::~VtxBuffer()
{
	if(m_PlatformData) 
		delete m_PlatformData;
}

////////////////////////////////////////////////////////////////////////////

bool VtxBuffer::Init(	u32 vtxCount,
								u32 vtxFlags,
								u32 texFlags[VTX_BUFFER_TEXTURE_LAYERS])
{
	ASSERT( m_PlatformData == NULL);

	m_PlatformData = new VtxBufferPlatformData( );
	ASSERT( m_PlatformData);

	return( m_PlatformData->Init( vtxCount, vtxFlags, texFlags));
}

////////////////////////////////////////////////////////////////////////////

void VtxBuffer::Lock( void)
{
	if( m_PlatformData->m_Vertices != NULL && !m_PlatformData->m_bRendering)
	{
		return;
	}

	u8		*pNewVertices = (u8 *)memAlloc( m_PlatformData->m_VtxBuffSize);
		
	ASSERT( pNewVertices != NULL);
		
	if( m_PlatformData->m_Vertices != NULL)
	{
		memcpy( pNewVertices, m_PlatformData->m_Vertices, m_PlatformData->m_VtxBuffSize);
		g_pFrameData->RegisterMemFree( m_PlatformData->m_Vertices);
	}
	m_PlatformData->m_Vertices = pNewVertices;
	m_PlatformData->m_bRendering = false;
}

////////////////////////////////////////////////////////////////////////////

void VtxBuffer::UnLock( void)
{
	ASSERT( m_PlatformData->m_Vertices != NULL);
	DCFlushRange( m_PlatformData->m_Vertices, m_PlatformData->m_VtxBuffSize);
}


////////////////////////////////////////////////////////////////////////////
// Private
////////////////////////////////////////////////////////////////////////////

VtxBufferPlatformData::VtxBufferPlatformData( )
{
	m_Vertices = NULL;

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

	m_bRendering = false;
}

VtxBufferPlatformData::~VtxBufferPlatformData( )
{
	if( m_Vertices != NULL)
	{
		g_pFrameData->RegisterMemFree( m_Vertices);
	}
}

bool VtxBufferPlatformData::Init(u32 vtxCount, u32 vtxFlags, u32 texFlags[VTX_BUFFER_TEXTURE_LAYERS])
{
	ASSERT(m_Vertices == 0);
    
	SizeAndStride( vtxFlags, texFlags);

	m_NumVertices = vtxCount;

	m_VtxBuffSize = m_Size * m_NumVertices;

	return( true);
}

void VtxBufferPlatformData::SizeAndStride(u32 vtxFlags, u32 texFlags[VTX_BUFFER_TEXTURE_LAYERS])
{
	m_VtxFlags = vtxFlags;
	m_TexFlags[0] = texFlags[0];
	m_TexFlags[1] = texFlags[1];
	m_TexFlags[2] = texFlags[2];
	m_TexFlags[3] = texFlags[3];
	m_TexFlags[4] = texFlags[4];
	m_TexFlags[5] = texFlags[5];
	m_TexFlags[6] = texFlags[6];
	m_TexFlags[7] = texFlags[7];

	m_Size = 3 * sizeof(float);

	if( vtxFlags & VtxBuffer::VERT_HAS_NORMAL)
	{
		m_NormalOffset = m_Size;
		m_Size += 3 * sizeof(float);
	}

	if( vtxFlags & VtxBuffer::VERT_HAS_DIFFUSE)
	{
		m_DiffuseOffset = m_Size;
		m_Size += sizeof(u32);
	}

	if( vtxFlags & VtxBuffer::VERT_HAS_SPECULAR)
	{
		m_SpecularOffset = m_Size;
		m_Size += sizeof(u32);
	}

	m_NumPasses = 0;
	for( int i = 0; i < 8; i++)
	{
		u32 t = m_TexFlags[i];

		if (t & VtxBuffer::VERT_TEX_DIM1)
		{
			m_TexCoordOffset[i] = m_Size;
			m_TexCoordDimension[i] = 1;
			m_Size += 1 * sizeof(float);
			m_NumPasses++;
		}
		if (t & VtxBuffer::VERT_TEX_DIM2)
		{
			m_TexCoordOffset[i] = m_Size;
			m_TexCoordDimension[i] = 2;
			m_Size += 2 * sizeof(float);
			m_NumPasses++;
		}
		if (t & VtxBuffer::VERT_TEX_DIM3)
		{
			m_TexCoordOffset[i] = m_Size;
			m_TexCoordDimension[i] = 3;
			m_Size += 3 * sizeof(float);
			m_NumPasses++;
		}
		if (t & VtxBuffer::VERT_TEX_DIM4)
		{
			m_TexCoordOffset[i] = m_Size;
			m_TexCoordDimension[i] = 4;
			m_Size += 4 * sizeof(float);
			m_NumPasses++;
		}
	}

	// Currently the stride is just the size of the vertex.
	m_Stride = m_Size;
}

////////////////////////////////////////////////////////////////////////////
