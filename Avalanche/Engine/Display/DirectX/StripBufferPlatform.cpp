#include "Display/DisplayPCH.h"

#include "Display/DirectX/StripBufferPlatform.h"

////////////////////////////////////////////////////////////////////////////
//  Classes
////////////////////////////////////////////////////////////////////////////

DXStripData::DXStripData( const u16 * _Indices, unsigned int _NumIndices )
{
	ASSERT( _Indices );
	ASSERT( _NumIndices );

	p_Indices = new u16[_NumIndices];
	
	m_MaxIdx = 0;
	m_MinIdx = UINT_MAX;

	for( unsigned int i = 0; i < _NumIndices; i++ )
	{
		u16 idx = _Indices[i];

		p_Indices[i] = idx;

		if( idx > m_MaxIdx )
			m_MaxIdx = idx;
		if( idx < m_MinIdx )
			m_MinIdx = idx;
	}

	m_NumIndices = _NumIndices;
}

DXStripData::~DXStripData( )
{
	if( p_Indices )
		delete [] p_Indices;
}

////////////////////////////////////////////////////////////////////////////

unsigned int	StripBufferPlatformData::m_StripBufferStart	= 4;
unsigned int	StripBufferPlatformData::m_StripBufferExpand = 4;

unsigned int	StripBufferPlatformData::m_SharedIBIndices = 1024;

DynamicIB *		StripBufferPlatformData::p_IB = 0;

StripBufferPlatformData::StripBufferPlatformData( D3DPRIMITIVETYPE _Type ) :
	m_PrimitiveType( _Type ),
	m_RenderTextures( 0 ),
	a_Strips( m_StripBufferStart, m_StripBufferExpand )
{
	memset( m_Passes, 0, 8 * sizeof( StripBufferPass ) );
}

StripBufferPlatformData::~StripBufferPlatformData( )
{
	for( int i = 0; i < a_Strips.Count( ); i++ )
	{
		if( a_Strips[i] )
			delete a_Strips[i];
	}
}

void StripBufferPlatformData::createDeviceObjects( void )
{
	ASSERT( DirectX_Device );
	ASSERT( p_IB == 0 );

	p_IB = new DynamicIB( DirectX_Device, m_SharedIBIndices, D3DFMT_INDEX16, true );
	DXPublished::addLostDeviceCallback( p_IB );
}

void StripBufferPlatformData::deleteDeviceObjects( void )
{
	if( p_IB )
	{
		DXPublished::removeLostDeviceCallback( p_IB );
		delete p_IB;

		p_IB = 0;
	}
}

bool StripBufferPlatformData::VerifyTexturePasses( unsigned int numPasses )
{
	for( unsigned int i = 0; i < numPasses; i++ )
		if( m_Passes[i].m_Texture == 0 )
			return false;

	m_RenderTextures = numPasses;
	return true;
}

////////////////////////////////////////////////////////////////////////////

void StripBufferPlatformData::Render( const Matrix4x4 & modelMatrix, const Matrix4x4 * texMatrices, VtxBuffer & VB )
{
	ASSERT( m_RenderTextures <= VB.m_PlatformData->m_NumPasses );

	if( a_Strips.Count( ) == 0 )
		return;

	Matrix4x4 finalMatrix = modelMatrix * g4WorldToCamera;

	RenderState__Request( RS_CULL, RS_CULL_NONE );
	
	MatrixStack__Mode( MS_MODELVIEW );
	MatrixStack__Push( );
	MatrixStack__Identity( );
	MatrixStack__Load( (Graphics4x4*) & finalMatrix );

	unsigned int baseVertex = VB.m_PlatformData->Load( );

	HRESULT result = DirectX_Device->SetIndices( *p_IB, baseVertex );
	ASSERT( result == D3D_OK );

	bool				flagCW = true;

	int				lastAddedIndex = -1;

	u16 *				pIndexMem = 0;

	unsigned int	indicesAvailable = 0;
	unsigned int	addedIndices = 0;

	unsigned int	minIndex = UINT_MAX;
	unsigned int	maxIndex = 0;

	unsigned int	baseIndex = 0;

	unsigned int	currentStrip = 0;
	
	unsigned int	stripCount = a_Strips.Count( );

	do
	{
		bool				bNeedsMerge = (addedIndices != 0);
		unsigned int	extraIndices = (bNeedsMerge == true) ? ( (flagCW == true) ? 2 : 3 ) : 0;

		DXStripData * pStrip = a_Strips[currentStrip];
		ASSERT( pStrip );

		if( (indicesAvailable - addedIndices) < (pStrip->m_NumIndices + extraIndices) )
		{
			if( addedIndices )
			{
				p_IB->unlock( indicesAvailable - addedIndices );
				p_IB->setFlush( );

				renderTexturePasses( baseIndex, addedIndices, minIndex, (maxIndex - minIndex) + 1, texMatrices );

			}
			else if( p_IB->lockableIndices( ) < (pStrip->m_NumIndices + extraIndices) )
				p_IB->setFlush( );

			bNeedsMerge = false;

			flagCW = true;
			lastAddedIndex = -1;
			addedIndices = 0;

			minIndex = UINT_MAX;
			maxIndex = 0;


			indicesAvailable = 0;	
			pIndexMem = (u16 *) p_IB->lock( indicesAvailable, baseIndex );

			ASSERT( indicesAvailable >= (pStrip->m_NumIndices + extraIndices) );
		}
	
		if( bNeedsMerge )
		{
			if( flagCW == false )
			{
				*pIndexMem++ = lastAddedIndex;
					
				addedIndices++;
				flagCW = true;
			}

			*pIndexMem++ = lastAddedIndex;
			*pIndexMem++ = pStrip->p_Indices[0];
				
			addedIndices += 2;
		}

		memcpy( pIndexMem, pStrip->p_Indices, pStrip->m_NumIndices * sizeof( u16 ) );
		pIndexMem += pStrip->m_NumIndices;
		
		if( pStrip->m_MaxIdx > maxIndex ) maxIndex = pStrip->m_MaxIdx;
		if( pStrip->m_MinIdx < minIndex ) minIndex = pStrip->m_MinIdx;

		if( pStrip->m_NumIndices & 1 )
			flagCW = !flagCW;

		addedIndices += pStrip->m_NumIndices;
		lastAddedIndex = pStrip->p_Indices[pStrip->m_NumIndices - 1];

	} while( ++currentStrip < stripCount );

	p_IB->unlock( indicesAvailable - addedIndices );

	if( addedIndices )
		renderTexturePasses( baseIndex, addedIndices, minIndex, (maxIndex - minIndex) + 1, texMatrices );

	/* done with render */
	RenderState__End( );
	MatrixStack__Pop( );

}

void StripBufferPlatformData::renderTexturePasses( unsigned int baseIndex, unsigned int numIndices, unsigned int minIndex, unsigned int numVertices, const Matrix4x4 * texMatrices )
{
	bool					bGlossHack = false;

	HRESULT				result = D3D_OK;

	unsigned int		textureLayer = 0;
	unsigned int		textureUnits = TextureState__GetNumUnits( );
	unsigned int		textureUnitsMax = TextureState__GetNumUnits( );

	result = DirectX_Device->SetPixelShader( NULL );
	ASSERT( result == D3D_OK );

	result = DirectX_Device->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	ASSERT( result == D3D_OK );

	result = DirectX_Device->SetRenderState( D3DRS_LIGHTING, false );
	ASSERT( result == D3D_OK );

	do
	{
		unsigned int textureUnit = 0;
		unsigned int textureCoordinates = 0;

		do
		{
			u8 method;

			StripBufferPass * pTexturePass = m_Passes + textureLayer;

			// setup texture 
			RenderState__Request( RS_BLENDFUNC, pTexturePass->m_Blend );
			TextureState__Force( textureUnit, TS_BIND );
			TextureState__Force( textureUnit, TS_METHOD );

			if( pTexturePass->m_Method == TS_METHOD_GLOSS )
			{
				D3DXMATRIX mat;

				D3DXMatrixIdentity(&mat);
				mat._11 = 0.5f; mat._12 = 0.0f;
				mat._21 = 0.0f; mat._22 =-0.5f;
				mat._31 = 0.0f; mat._32 = 0.0f;
				mat._41 = 0.5f; mat._42 = 0.5f;

				DirectX_Device->SetTransform( (D3DTRANSFORMSTATETYPE) (D3DTS_TEXTURE0 + textureUnit), &mat );
				DirectX_Device->SetTextureStageState( textureUnit, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );
				DirectX_Device->SetTextureStageState( textureUnit, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR );

				method = TS_METHOD_MODULATE;
			}
			else
			{
				if( texMatrices )
				{
					result = DirectX_Device->SetTransform( D3DTRANSFORMSTATETYPE (D3DTS_TEXTURE0 + textureUnit), (D3DMATRIX *) & texMatrices[textureLayer] );
					ASSERT( result == D3D_OK );

					result = DirectX_Device->SetTextureStageState( textureUnit, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );
					ASSERT( result == D3D_OK );
				}
				else
				{
					result = DirectX_Device->SetTextureStageState( textureUnit, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
					ASSERT( result == D3D_OK );
				}

				result = DirectX_Device->SetTextureStageState( textureUnit, D3DTSS_TEXCOORDINDEX, textureCoordinates++ );
				ASSERT( result == D3D_OK );

				if( bGlossHack == true )
				{
					method = TS_METHOD_GLOSS;
					bGlossHack = false;
				}
				else
					method = pTexturePass->m_Method;
			}

			TextureState__RequestAll( textureUnit, TS_TYPE_2D, pTexturePass->m_Texture, method, pTexturePass->m_WrapU, pTexturePass->m_WrapV, pTexturePass->m_Filter );
			
			if( pTexturePass->m_Method == TS_METHOD_GLOSS )
				bGlossHack = true;

			textureUnit++;
			textureLayer++;

		} while( (textureUnit < textureUnits)  &&  (textureLayer < m_RenderTextures) );

		while( textureUnit < textureUnitsMax )
		{
			result = DirectX_Device->SetTextureStageState( textureUnit, D3DTSS_COLOROP, D3DTOP_DISABLE );
			ASSERT( result == D3D_OK );
			result = DirectX_Device->SetTextureStageState( textureUnit, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
			ASSERT( result == D3D_OK );

			textureUnit++;
		}

		result = DirectX_Device->DrawIndexedPrimitive( m_PrimitiveType, minIndex, numVertices, baseIndex, numIndices - 2 );
		ASSERT( result == D3D_OK );

	} while( textureLayer < m_RenderTextures );

}

