///////////////////////////////////////////////////////////////////////////
//
// StripBuffer.cpp
//
// Strip Buffer Module.
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

/******************* includes ****************************/

#include "Display/DisplayPCH.h"
#include "Display/DirectX/StripBufferPlatform.h"

////////////////////////////////////////////////////////////////////////////
//  Public
////////////////////////////////////////////////////////////////////////////

StripBuffer::StripBuffer( ) 
{
	m_PlatformData = 0;
	m_VtxBuffer = 0;
	m_TextureArray = NULL;
	m_TextureSubSet = 0;
	m_TexturePasses = -1;
}

////////////////////////////////////////////////////////////////////////////

StripBuffer::~StripBuffer( )
{
	if( m_PlatformData )
		delete m_PlatformData;
}

////////////////////////////////////////////////////////////////////////////

bool StripBuffer::Init( u32 flags )
{
	m_Flags = flags;

	MEM_SET_ONE_SHOT_NEW_NAME("StripBuff::Init");
	m_PlatformData = new StripBufferPlatformData( (flags & STRIP_BUFFER_IS_FAN) ? D3DPT_TRIANGLEFAN : D3DPT_TRIANGLESTRIP );

	m_VtxBuffer = 0;

	return true;
}

////////////////////////////////////////////////////////////////////////////

u32 StripBuffer::AddStrip( const u16 * pIndices, u16 numIndices )
{
	ASSERT( m_PlatformData );

	m_PlatformData->a_Strips.Add( new DXStripData( pIndices, numIndices ) );

	return m_PlatformData->a_Strips.Count( ) - 1;
}

////////////////////////////////////////////////////////////////////////////

void StripBuffer::SetVtxBuffer( VtxBuffer* vtxBuffer )
{
	m_VtxBuffer = vtxBuffer;
}

////////////////////////////////////////////////////////////////////////////

void StripBuffer::SetPassTexture( u32 passNumber, Texture * texture, u8 method, u8 wrapU, u8 wrapV, u8 filter, u8 blend )
{
	ASSERT( passNumber < 8 );
	
	StripBufferPass & Pass = m_PlatformData->m_Passes[passNumber];
	
	Pass.m_Texture = texture;

	Pass.m_Method = method;
	Pass.m_WrapU = wrapU;
	Pass.m_WrapV = wrapV;
	Pass.m_Filter = filter;
	Pass.m_Blend = blend;
}

////////////////////////////////////////////////////////////////////////////

void StripBuffer::Reset( void )
{
	ASSERT( m_PlatformData );

	for(int i = 0; i < m_PlatformData->a_Strips.Count(); i++)
		delete m_PlatformData->a_Strips[i];

	m_PlatformData->a_Strips.Clear( );
}

////////////////////////////////////////////////////////////////////////////

void StripBuffer::RemoveStrip( u32 stripIndex )
{
	ASSERT( m_PlatformData );

	m_PlatformData->a_Strips.Delete( stripIndex );
}

////////////////////////////////////////////////////////////////////////////

// note, numIndicies must be <= the value passed to AddStrip for this strip
u32 StripBuffer::ReplaceStrip( u32 index, const u16 * pIndices, u16 numIndices )
{
	ASSERT( m_PlatformData );
	ASSERT( (int) index < m_PlatformData->a_Strips.Count( ) );

	DXStripData * pRemove = m_PlatformData->a_Strips[index];

	m_PlatformData->a_Strips[index] = new DXStripData( pIndices, numIndices );

	if( pRemove )
		delete pRemove;

	return index;
}

////////////////////////////////////////////////////////////////////////////

void StripBuffer::Render( const CInstance * pInstance, const Graphics4x4 * pmLocalToCamera, float fZCamDist )
{
	m_PlatformData->Render( *pmLocalToCamera, NULL, *m_VtxBuffer );
}

////////////////////////////////////////////////////////////////////////////

void StripBuffer::Render( const Matrix4x4& mLocalToCamera, const Matrix4x4* texMatrices, const u32 vertexOffset )
{
	ASSERT( m_PlatformData );

	unsigned int m_RenderTextures = (m_TexturePasses == -1) ? m_VtxBuffer->m_PlatformData->m_NumPasses : m_TexturePasses;

	bool bPassesValid = m_PlatformData->VerifyTexturePasses( m_RenderTextures );
	ASSERT( bPassesValid == true );

	for( int i = 1; i < TextureState__GetNumUnits( ); i++ )
	{
		HRESULT result = DirectX_Device->SetTexture( i, NULL );
		ASSERT( result == D3D_OK );

		result = DirectX_Device->SetTextureStageState( i, D3DTSS_COLOROP, D3DTOP_DISABLE );
		ASSERT( result == D3D_OK );
		result = DirectX_Device->SetTextureStageState( i, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
		ASSERT( result == D3D_OK );
	}

	m_PlatformData->Render( mLocalToCamera, texMatrices, *m_VtxBuffer );

	for( int i = 1; i < TextureState__GetNumUnits( ); i++ )
	{
		HRESULT result = D3D_OK;

		result = DirectX_Device->SetTextureStageState( i, D3DTSS_COLOROP, D3DTOP_DISABLE );
		ASSERT( result == D3D_OK );
		result = DirectX_Device->SetTextureStageState( i, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
		ASSERT( result == D3D_OK );

		result = DirectX_Device->SetTextureStageState( i, D3DTSS_TEXCOORDINDEX, i );
		ASSERT( result == D3D_OK );
		result = DirectX_Device->SetTextureStageState( i, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
		ASSERT( result == D3D_OK );
	}

}

////////////////////////////////////////////////////////////////////////////

void StripBuffer::SetNumTexturePasses ( u32 Passes )
{
	m_TexturePasses = Passes;
}

////////////////////////////////////////////////////////////////////////////
