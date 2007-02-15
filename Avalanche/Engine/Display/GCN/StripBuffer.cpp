///////////////////////////////////////////////////////////////////////////
//
// StripBuffer.cpp
//
// Strip Buffer Module.
//
// Todd Blackburn, March 2002.
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
#include <Dolphin/gd.h>

////////////////////////////////////////////////////////////////////////////
//  Defines
////////////////////////////////////////////////////////////////////////////

#define MAX_PASSES			8
#define DLIST_HEADER_SIZE	3
#define DLIST_PAD				( 3 * 32)

////////////////////////////////////////////////////////////////////////////
//  Classes
////////////////////////////////////////////////////////////////////////////

#ifndef CDROM
extern u16 u16CurrToken;
#endif

class StripBufferGCN
{
public:
	StripBufferGCN( DynamicArray<u16> &IndicesPool, const u16 *pIndices, u32 nIndices)
	{
		m_IndicesIndex = IndicesPool.Count( );

		for( u32 index = 0; index < nIndices; index++)
		{
			IndicesPool.Add( pIndices[index]);
		}

		m_nIndices = nIndices;
	}

	u32					m_IndicesIndex;
	u32					m_nIndices;
};

////////////////////////////////////////////////////////////////////////////

class StripBufferPlatformData
{
public:
	StripBufferPlatformData( ) : m_StripList( 30, 30), m_IndicesPool( 100, 100)
	{
		m_pDisplayList = NULL;
		for( int iPass = 0; iPass < MAX_PASSES; iPass++)
		{
			m_apShader[iPass] = NULL;
			m_aBind[iPass] = NO_BIND;
		}
	}

	~StripBufferPlatformData( )
	{
		m_StripList.Reset( );
		m_IndicesPool.Reset( );
		RemoveDisplayList( );
	}

	void RemoveDisplayList( void)
	{
		if( m_pDisplayList != NULL)
		{
			g_pFrameData->RegisterMemFree( m_pDisplayList);
			m_pDisplayList = NULL;
		}
	}

	void BuildDisplayList( VtxBufferPlatformData &vb, u32 flags);

	DynamicArray<StripBufferGCN>			m_StripList;
	DynamicArray<u16>							m_IndicesPool;

	ShaderInst									*m_apShader[MAX_PASSES];
	s16											m_aBind[MAX_PASSES];
	u8												m_aWrapU[MAX_PASSES];
	u8												m_aWrapV[MAX_PASSES];
	u8												m_aFilter[MAX_PASSES];
	u8												m_aBlend[MAX_PASSES];

	void											*m_pDisplayList;

	u32											m_DisplayListSize;

};

////////////////////////////////////////////////////////////////////////////
//  Public
////////////////////////////////////////////////////////////////////////////

StripBuffer::StripBuffer( )
{
	m_PlatformData = NULL;
	m_VtxBuffer = NULL;
	m_TextureSet = NULL;
	m_TextureSubSet = 0;

	m_TexturePasses = -1;

}

////////////////////////////////////////////////////////////////////////////

StripBuffer::~StripBuffer( )
{
	if( m_PlatformData)
	{
		delete m_PlatformData;
	}
}

////////////////////////////////////////////////////////////////////////////

bool StripBuffer::Init(u32 flags)
{
	m_Flags = flags;

	m_PlatformData = new StripBufferPlatformData;

	return( m_PlatformData != NULL);
}

////////////////////////////////////////////////////////////////////////////

u32 StripBuffer::AddStrip( const u16* indices, u16 numIndices)
{
	
	StripBufferGCN		SB( m_PlatformData->m_IndicesPool, indices, numIndices);

	m_PlatformData->m_StripList.Add( SB);
	
	m_PlatformData->RemoveDisplayList( );

	return( m_PlatformData->m_StripList.Count( ) - 1);
}

////////////////////////////////////////////////////////////////////////////

void StripBuffer::SetVtxBuffer( VtxBuffer *pVtxBuffer)
{
	m_VtxBuffer = pVtxBuffer;
}

////////////////////////////////////////////////////////////////////////////

void StripBuffer::SetPassTexture( u32 passNumber, s16 bind, u8 method, u8 wrapU, u8 wrapV, u8 filter, u8 blend)
{

	ts_TextureContext		tc;

	ASSERT( passNumber < MAX_PASSES);

	tc.TextureCount = ( bind != NO_BIND) ? 1 : 0;
	tc.TextureLayer[0].s16TextureBind = bind;
	tc.TextureLayer[0].u8Method = method;
	tc.TextureLayer[0].u8WrapU = wrapU;
	tc.TextureLayer[0].u8WrapV = wrapV;
	tc.TextureLayer[0].u8Filter = filter;
	tc.TextureLayer[0].u8Blend = blend;
	tc.TextureLayer[0].u8Flags = 0;

	m_PlatformData->m_apShader[passNumber] = g_ShaderMgr.GetShader( &tc, false);
	m_PlatformData->m_aBind[passNumber] = bind;
	m_PlatformData->m_aWrapU[passNumber] = wrapU;
	m_PlatformData->m_aWrapV[passNumber] = wrapV;
	m_PlatformData->m_aFilter[passNumber] = filter;
	m_PlatformData->m_aBlend[passNumber] = blend;
}

////////////////////////////////////////////////////////////////////////////

void StripBuffer::Reset( void)
{
	m_PlatformData->m_StripList.Reset( );
	m_PlatformData->m_IndicesPool.Reset( );
	m_PlatformData->RemoveDisplayList( );
}

////////////////////////////////////////////////////////////////////////////

// note, numIndicies must be <= the value passed to AddStrip for this strip
u32 StripBuffer::ReplaceStrip(u32 index, const u16* indices, u16 numIndices)
{
	StripBufferGCN SB = m_PlatformData->m_StripList[index];

	if( numIndices <= SB.m_nIndices)
	{
		SB.m_nIndices = numIndices;

		for( u16 i = 0; i < numIndices; i++)
		{
			u16	index = indices[i];
			m_PlatformData->m_IndicesPool[ SB.m_IndicesIndex + i] = index;
		}
	}
	else
	{
		StripBufferGCN		newSB( m_PlatformData->m_IndicesPool, indices, numIndices);

		m_PlatformData->m_StripList[index] = newSB;
	}

	m_PlatformData->RemoveDisplayList( );
	
	return( index);
}

////////////////////////////////////////////////////////////////////////////

void StripBuffer::RemoveStrip( u32 stripIndex)
{
	if( stripIndex >= m_PlatformData->m_StripList.Count( ))
	{
		return;
	}

	m_PlatformData->m_StripList.Delete( stripIndex);

	m_PlatformData->RemoveDisplayList( );
}

////////////////////////////////////////////////////////////////////////////

void StripBuffer::Render( const CInstance *pInstance, const Graphics4x4 *pmLocalToCamera, float fZCamDist)
{

	Graphics4x4 texMtx[8];

	for( int i = 0; i < 8; i++)
	{
		texMtx[i].Identity( );
	}
	Render( *pmLocalToCamera, texMtx);
}

////////////////////////////////////////////////////////////////////////////

void StripBuffer::Render( const Matrix4x4& modelMatrix, const Matrix4x4* texMatrices, const u32 vertexOffset)
{

	int	TexturePassCount = 0;		// PAB - added for setting texture layer rendering

	VtxBufferPlatformData		*vb = m_VtxBuffer->m_PlatformData;

	ASSERT( vertexOffset == 0);
	
	if( m_PlatformData->m_StripList.Count( ) == 0)
	{
		return;
	}

	if( m_VtxBuffer == NULL)
	{
		return;
	}

	if( m_PlatformData->m_pDisplayList == NULL)
	{
		m_PlatformData->BuildDisplayList( *vb, m_Flags);
	}

#if 0
#ifndef CDROM
	GXSetDrawSync( u16CurrToken);
	OSReport( "Rendering %08X as token %d\n", this, u16CurrToken++);
	if( u16CurrToken >= 0x8000)
	{
		u16CurrToken = 0;
	}
#endif
#endif
	Matrix4x4 finalMatrix = modelMatrix * g4WorldToCamera;

	RenderState__Request( RS_CULL, RS_CULL_NONE);
	
	TextureState__RequestUnit( 0);
	MatrixStack__Mode( MS_TEXTURE);
	MatrixStack__Push( );
	
	MatrixStack__Mode( MS_MODELVIEW);
	MatrixStack__Push( );
	MatrixStack__Load( (Graphics4x4 *)&finalMatrix);

	u32	channelFlags = 0;

	if(	RenderState__Get( RS_LIGHTING) == RS_TRUE &&
			vb->m_VtxFlags & VtxBuffer::VERT_HAS_NORMAL)
	{
		channelFlags |= ShaderMgr::GCN_CHANNEL_HAS_LIGHTS;
	}

	if( vb->m_VtxFlags & VtxBuffer::VERT_HAS_DIFFUSE)
	{
		channelFlags |= ShaderMgr::GCN_CHANNEL_HAS_VTXCOLOR;
	}

	GXColor		gxColor;
	
	gxColor.r = 255;
	gxColor.g = 255;
	gxColor.b = 255;
	gxColor.a = 255;

	g_ShaderMgr.SetupLightingChannel( channelFlags, gxColor);

	GXSetArray( GX_VA_POS, vb->m_Vertices + ( vertexOffset * vb->m_Stride), vb->m_Stride);
	GXSetArray( GX_VA_NRM, vb->m_Vertices + ( vertexOffset * vb->m_Stride) + vb->m_NormalOffset, vb->m_Stride);
	GXSetArray( GX_VA_CLR0, vb->m_Vertices + ( vertexOffset * vb->m_Stride) + vb->m_DiffuseOffset, vb->m_Stride);
	
	// setup the number of texture layers to render (PAB)
	if (m_TexturePasses == -1)
		TexturePassCount = vb->m_NumPasses;
	else
		TexturePassCount = m_TexturePasses;

	for( u32 iPass = 0; iPass < TexturePassCount; iPass++)
	{
		if(	vb->m_TexFlags[iPass] != 0 &&
				m_PlatformData->m_aBind[iPass] != NO_BIND)
		{
			GXSetArray(	GX_VA_TEX0, vb->m_Vertices + ( vertexOffset * vb->m_Stride) + vb->m_TexCoordOffset[iPass], vb->m_Stride);

			TextureState__RequestUnit( 0);
			MatrixStack__Mode( MS_TEXTURE);
			MatrixStack__Load( (Graphics4x4 *)( texMatrices + iPass));

			RenderState__Request( RS_BLENDFUNC, m_PlatformData->m_aBlend[iPass]);

			TextureBind__SetWrap(	m_PlatformData->m_aBind[iPass],
											m_PlatformData->m_aWrapU[iPass],
											m_PlatformData->m_aWrapV[iPass]);
			TextureBind__SetFilter(	m_PlatformData->m_aBind[iPass],
										  	m_PlatformData->m_aFilter[iPass]);

			TextureBind__Load(	m_PlatformData->m_aBind[iPass],
										0,
										GX_TEXCOORD0);
			g_ShaderMgr.UseShader( m_PlatformData->m_apShader[iPass]);

			GXCallDisplayList( m_PlatformData->m_pDisplayList, m_PlatformData->m_DisplayListSize);
			vb->m_bRendering = true;
		}
	}
	
	TextureState__RequestUnit( 0);
	MatrixStack__Mode( MS_TEXTURE);
	MatrixStack__Pop( );
	
	MatrixStack__Mode( MS_MODELVIEW);
	MatrixStack__Pop( );
}

void StripBuffer::SetNumTexturePasses( u32 Passes)
{
	m_TexturePasses = Passes;
}

static void StripBufferOverflow( void)
{
	LOCKUP( );
}

void StripBufferPlatformData::BuildDisplayList( VtxBufferPlatformData &vb, u32 flags)
{

	u32							nBytes = 0,
									nElements,
									iPass;

	StripBufferGCN				*pStrip;

	pStrip = m_StripList.GetPtr( );

	for( int i = 0; i < m_StripList.Count( ); i++, pStrip++)
	{
		//add enough bytes for display list header
		nBytes += DLIST_HEADER_SIZE;

		//all vertices will have position index
		nElements =	1;

		if( vb.m_VtxFlags & VtxBuffer::VERT_HAS_NORMAL)
		{
			nElements++;
		}

		if( vb.m_VtxFlags & VtxBuffer::VERT_HAS_DIFFUSE)
		{
			nElements++;
		}

		//all vertices will have UV index
		nElements++;

		//add enough bytes for all elements
		nBytes += sizeof(u16) * nElements * pStrip->m_nIndices;
	}

	CGXVtxDescList	gxVtxDescList;
	
	//add enough bytes for padding and flushing
	nBytes += DLIST_PAD;
	
	nBytes = OSRoundUp32B( nBytes);
	
	//get rid of old list if allocated
	ASSERT( m_pDisplayList == NULL);

	MEM_SET_ALLOC_NAME( "StripBuff DL");
	m_pDisplayList = memAlloc( nBytes, 32);
	ASSERT( m_pDisplayList);

	GDLObj	gdlObj;

	g_ShaderMgr.LockDLMutex( );

	GDInitGDLObj( &gdlObj, m_pDisplayList, nBytes);
	GDSetCurrent( &gdlObj);
	GDSetOverflowCallback( StripBufferOverflow);

	gxVtxDescList.Add( GX_VA_POS, GX_INDEX16);
	if( vb.m_VtxFlags & VtxBuffer::VERT_HAS_NORMAL)
	{
		gxVtxDescList.Add( GX_VA_NRM, GX_INDEX16);
	}
	if( vb.m_VtxFlags & VtxBuffer::VERT_HAS_DIFFUSE)
	{
		gxVtxDescList.Add( GX_VA_CLR0, GX_INDEX16);
	}
	gxVtxDescList.Add( GX_VA_TEX0, GX_INDEX16);

	GDSetVtxDescv( gxVtxDescList);

	pStrip = m_StripList.GetPtr( );

	for( int i = 0; i < m_StripList.Count( ); i++, pStrip++)
	{
		GDBegin( ( flags & STRIP_BUFFER_IS_FAN) ? GX_TRIANGLEFAN : GX_TRIANGLESTRIP, GX_VTXFMT0, pStrip->m_nIndices);
		for( u32 iPoint = 0; iPoint < pStrip->m_nIndices; iPoint++)
		{

			ASSERT(( pStrip->m_IndicesIndex + iPoint) < m_IndicesPool.Count( ));

			u16 index = m_IndicesPool[ pStrip->m_IndicesIndex + iPoint];

			ASSERT( index < vb.m_NumVertices);

			GDPosition1x16( index);
			
			if( vb.m_VtxFlags & VtxBuffer::VERT_HAS_NORMAL)
			{
				GDNormal1x16( index);
			}
			if( vb.m_VtxFlags & VtxBuffer::VERT_HAS_DIFFUSE)
			{
				GDColor1x16( index);
			}
			
			GDTexCoord1x16( index);
		}
	}
	GDPadCurr32( );
	GDFlushCurrToMem( );

	m_DisplayListSize = GDGetCurrOffset( );
	
	g_ShaderMgr.UnlockDLMutex( );
}
