
#ifndef DIRECTX
#pragma message( "WARNING: DIRECTX should be defined when compiling this module!" )
#endif

#include "Display/DisplayPCH.h"

#include "DXBase.h"
#include "DXShaderLibrary.h"
#include "DXSharedDCache.h"

#include "PatchHelper.h"

#ifdef _XBOX
#include "Shaders/AnimAllXB.h"
#include "Shaders/AnimNLXB.h"
#include "Shaders/AnimShadowCastXB.h"
#include "Shaders/AnimAll2TXB.h"
#include "Shaders/AnimAll3TXB.h"
#include "Shaders/Anim2PassXB.h"
#include "Shaders/Anim3PassXB.h"
#else
#define D3DPIXELSHADERDEF DWORD
#include "Shaders/AnimAllDX.h"
#include "Shaders/AnimNLDX.h"
#include "Shaders/AnimShadowCastDX.h"
#include "Shaders/AnimAll2TDX.h"
#include "Shaders/AnimAll3TDX.h"
#include "Shaders/Anim2PassDX.h"
#include "Shaders/Anim3PassDX.h"
#endif

extern D3DXMATRIX						_MatrixStack_Identity;
StandardTCMaterial::RenderMode	StandardTCMaterial::m_RenderMode = MODE_DEFAULT;
DXRenderShaderBase *					StandardTCMaterial::m_CurrentShader = 0;
bool										StandardTCMaterial::m_BlendAlpha = false;
unsigned int							StandardTCMaterial::m_CurrentFVF = 0;
unsigned int							StandardTCMaterial::m_DisableMaps = 0;
unsigned int							StandardTCMaterial::m_TextureUnitsClearStart = 0;

extern void _TextureState__Bind( int unit, int value );

StandardTCMaterial::StandardTCMaterial( const DXMeshInterface & Interface, const ShaderRecord & _Record, ts_TextureContext * _TextureContext ) :
	p_Descriptor( Interface.getStreamDescriptor( _Record.d_StreamIndex ) )
{
	m_ShaderFlags = _Record.d_ShaderFlags;
	p_TextureContext = _TextureContext;
	pPShader = 0;

	if( p_Descriptor->m_StreamFlags & STFLAG_POSITION4 )
	{
		if( p_Descriptor->m_StreamFlags & STFLAG_NORMAL )
		{
			unsigned int texLayers = textureChannels( p_Descriptor->m_StreamFlags );

			unsigned int textureLayer = 0;
			unsigned int textureLayerCount = p_TextureContext->TextureCount;

			bool bSpecular = false;

			while( textureLayer < textureLayerCount )	
			{
				TextureDatabase *	pDB;
				Texture *			pTexture;

				ts_TextureLayer & TextureLayer = p_TextureContext->TextureLayer[textureLayer];

				pDB = TextureLayer.textureDatabase;
				ASSERT_PTR( pDB );

				pTexture = pDB->GetTexture( TextureLayer.s16TextureIndex );
				ASSERT_PTR( pTexture );

				if( TextureLayer.u8Flags & TEXTURE_IS_ENVIRONMENT_MAP )
				{
					bSpecular = true;
					texLayers--;
				}

				textureLayer++;
			}

			DXVShader * pVShader = 0;

			HRESULT result = D3D_OK;

			switch( texLayers )
			{
			case 3:
				DirectX_Device->CreatePixelShader( (D3DPIXELSHADERDEF *) Anim3Pass, & pPShader );
				ASSERT( result == D3D_OK );

				pVShader = DXShaderLib.createVertexShader( p_Descriptor->p_Descriptor, p_Descriptor->m_StreamComponents, descAnimAll3T );
				break;
			case 2:
				result = DirectX_Device->CreatePixelShader( (D3DPIXELSHADERDEF *) Anim2Pass, & pPShader );
				ASSERT( result == D3D_OK );

				pVShader = DXShaderLib.createVertexShader( p_Descriptor->p_Descriptor, p_Descriptor->m_StreamComponents, descAnimAll2T );
				break;
			default:
				pVShader = DXShaderLib.createVertexShader( p_Descriptor->p_Descriptor, p_Descriptor->m_StreamComponents, descAnimAll );
			}

			p_Shader = new DXAnimShader( *pVShader, true, bSpecular ? 32.0f : 0.0f );
		}
		else
		{
			DXVShader * pVShader = DXShaderLib.createVertexShader( p_Descriptor->p_Descriptor, p_Descriptor->m_StreamComponents, descAnimNL );
			p_Shader = new DXAnimShader( *pVShader, false );
		}

		DXVShader * pVShader = DXShaderLib.createVertexShader( p_Descriptor->p_Descriptor, p_Descriptor->m_StreamComponents, descAnimShadowCast );
		p_ShadowCastShader = new DXAnimShadowCastShader( *pVShader );
	}
	else
	{
		p_Shader = 0;
		p_ShadowCastShader = 0;
	}
}

StandardTCMaterial::~StandardTCMaterial( )
{
	if( pPShader )
	{
		HRESULT result = DirectX_Device->DeletePixelShader( pPShader );
		ASSERT( result == D3D_OK );

		pPShader = 0;
	}

	if( p_Shader )
	{
		delete p_Shader;
		p_Shader = 0;
	}

	if( p_ShadowCastShader )
	{
		delete p_ShadowCastShader;
		p_ShadowCastShader = 0;
	}
}

void StandardTCMaterial::applyPass( unsigned int pass )
{

	switch( m_RenderMode )
	{
	case MODE_SHADOW_CAST:
		if( !p_ShadowCastShader  &&  (pass == 0)  &&  (p_Descriptor->m_StreamFVF != m_CurrentFVF) )
		{
			HRESULT result = DirectX_Device->SetVertexShader( p_Descriptor->m_StreamFVF );
			ASSERT( result == D3D_OK );

			m_CurrentFVF = p_Descriptor->m_StreamFVF;
		}

		return;
	default:
		if( p_Shader )
			pPShader ? applyVSPShader( pass ) : applyVSMaterial( pass );
		else
         applyFFPMaterial( pass );
		return;
	}
}

void StandardTCMaterial::setupShadowCast( )
{
	HRESULT result = D3D_OK;

	int value = RenderState__Get( RS_BASECOLOR );

	result = DirectX_Device->SetRenderState( D3DRS_TEXTUREFACTOR, value );
	ASSERT( result == D3D_OK );

	result = DirectX_Device->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	ASSERT( result == D3D_OK );
	_RenderState__Change( RS_CULL, RS_CULL_NONE );

	result = DirectX_Device->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE );
	ASSERT( result == D3D_OK );

	result = DirectX_Device->SetRenderState( D3DRS_ZFUNC, D3DCMP_ALWAYS );
	ASSERT( result == D3D_OK );

	result = DirectX_Device->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
	ASSERT( result == D3D_OK );

	result = DirectX_Device->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TFACTOR );
	ASSERT( result == D3D_OK );

	result = DirectX_Device->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
	ASSERT( result == D3D_OK );

	result = DirectX_Device->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR );
	ASSERT( result == D3D_OK );

	result = DirectX_Device->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	assert( result == D3D_OK );

   result = DirectX_Device->SetRenderState( D3DRS_FOGENABLE, FALSE );
	assert( result == D3D_OK );
	_RenderState__Change( RS_FOG, FALSE );

	result = DirectX_Device->SetRenderState( D3DRS_LIGHTING, FALSE );
	assert( result == D3D_OK );
	_RenderState__Change( RS_LIGHTING, FALSE );

	result = DirectX_Device->SetRenderState( D3DRS_SPECULARENABLE, FALSE );
	ASSERT( result == D3D_OK );

	unsigned int textureUnitMax = TextureState__GetNumUnits( );

	for( unsigned int textureUnit = 0; textureUnit < textureUnitMax; textureUnit++ )
	{
		result = DirectX_Device->SetTexture( textureUnit, NULL );
		assert( result == D3D_OK );

		result = DirectX_Device->SetTextureStageState( textureUnit, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
		ASSERT( result == D3D_OK );

		result = DirectX_Device->SetTextureStageState( textureUnit, D3DTSS_TEXCOORDINDEX, textureUnit );
		ASSERT( result == D3D_OK );

		if( textureUnit )
		{
			result = DirectX_Device->SetTextureStageState( textureUnit, D3DTSS_COLOROP, D3DTOP_DISABLE );
			ASSERT( result == D3D_OK );

			result = DirectX_Device->SetTextureStageState( textureUnit, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
			ASSERT( result == D3D_OK );
		}

	}

	if( p_ShadowCastShader )
		p_ShadowCastShader->render( );
}

void StandardTCMaterial::meshStart( void )
{
	switch( m_RenderMode )
	{
	case MODE_SHADOW_CAST:
		setupShadowCast( );
		return;
	default:
		setupBaseMaterial( );
		return;
	}
}

void StandardTCMaterial::setupBaseMaterial( void )
{
	HRESULT result = D3D_OK;

	m_CurrentShader = 0;
	m_CurrentFVF = 0;

	m_DisableMaps = 0;
	
	if( !RenderMgr__DiffuseMapsOn( ) )
		m_DisableMaps |= TEXTURE_IS_DIFFUSE_MAP;			// remove diffuse maps
	if( !RenderMgr__EnvironmentMapsOn( ) )
		m_DisableMaps |= TEXTURE_IS_ENVIRONMENT_MAP;		// remove environment maps

	int iObjectAlpha = RenderState__Get( RS_OBJECT_CONSTANTALPHA );
	
	float fObjectAlpha = *(float *) &iObjectAlpha;
	
	if( fObjectAlpha < .995f )
	{
		result = DirectX_Device->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_RGBA( 0xff, 0xff, 0xff, (char) (fObjectAlpha * 255.0f) ) );
		ASSERT( result == D3D_OK );

		m_BlendAlpha = true;
	}
	else
	{
		m_BlendAlpha = false;
	}

	unsigned int textureUnitMax = TextureState__GetNumUnits( );

	for( unsigned int textureUnit = 0; textureUnit < textureUnitMax; textureUnit++ )
	{
		if( textureUnit )
		{
			result = DirectX_Device->SetTexture( textureUnit, NULL );
			ASSERT( result == D3D_OK );
	
			result = DirectX_Device->SetTextureStageState( textureUnit, D3DTSS_COLOROP, D3DTOP_DISABLE );
			ASSERT( result == D3D_OK );
			result = DirectX_Device->SetTextureStageState( textureUnit, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
			ASSERT( result == D3D_OK );
		}

		result = DirectX_Device->SetTextureStageState( textureUnit, D3DTSS_TEXCOORDINDEX, textureUnit );
		ASSERT( result == D3D_OK );

		if( (MatrixStack__IsIdentity( MS_TEXTURE + textureUnit ) == false)  &&  !p_Shader )
		{
			result = DirectX_Device->SetTextureStageState( textureUnit, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );
			ASSERT( result == D3D_OK );
		}
		else
		{
			result = DirectX_Device->SetTextureStageState( textureUnit, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
			ASSERT( result == D3D_OK );
		}
		 
//		result = DirectX_Device->SetTextureStageState( textureUnit, D3DTSS_TEXTURETRANSFORMFLAGS,  D3DTTFF_DISABLE );
//		ASSERT( result == D3D_OK );
	}

	m_TextureUnitsClearStart = 1;

	if( !p_Shader )
	{
		result = DirectX_Device->SetRenderState( D3DRS_SPECULARENABLE, FALSE );
		ASSERT( result == D3D_OK );

		setMaterial( );
		DXShaderDCache.sendFFLights( );
	}
}

void StandardTCMaterial::applyFFPMaterial( unsigned int pass )
{
	HRESULT				result = D3D_OK;
	u8						u8DisabledMaps = 0;


	RenderState__Request( RS_CULL, m_ShaderFlags & ShaderRecord::SHFLAG_TWO_SIDED ? RS_CULL_NONE : RS_CULL_BACK );

 	unsigned int textureUnit = 0;
	unsigned int textureUnitMax = TextureState__GetNumUnits( );

	unsigned int textureCoordinateSet = 0;
	unsigned int textureCoordinateSetMax = (p_Descriptor->m_StreamFlags & STFLAG_TEXTURE_CHANNELS_MASK) >> STFLAG_TEXTURE_CHANNELS_SHIFT;
	unsigned int textureCoordinateSetBit = (1 << STFLAG_DUMMY_TEXTURE_SHIFT);

	unsigned int textureLayer = 0;
	unsigned int textureLayerCount = p_TextureContext->TextureCount;

	ASSERT( textureLayerCount <= textureUnitMax );

	if( textureLayerCount == 0 )
	{
		RenderState__Request( RS_BLENDFUNC, RS_BLENDFUNC_DEFAULT );
		RenderState__Request( RS_ALPHAREF, RS_ALPHA_REF_DEFAULT_VALUE );
	}
	else
	{
		RenderState__Request( RS_BLENDFUNC, p_TextureContext->TextureLayer[0].u8Blend );
		RenderState__Request( RS_ALPHAREF, (p_TextureContext->TextureLayer[0].u8Flags & TEXTURE_ALPHA_REF) ? RS_ALPHA_REF_NO_HALO_VALUE : RS_ALPHA_REF_DEFAULT_VALUE );
	}

	while( textureLayer != p_TextureContext->TextureCount )	
	{
		TextureDatabase *	pDB;
		Texture *			pTexture;

		ts_TextureLayer & TextureLayer = p_TextureContext->TextureLayer[textureLayer];

		pDB = TextureLayer.textureDatabase;
		ASSERT_PTR( pDB );

		pTexture = pDB->GetTexture( TextureLayer.s16TextureIndex );
		ASSERT_PTR( pTexture );

		pTexture->Lock( );

		if( pass == 0 )
		{
			result = DirectX_Device->SetTextureStageState( textureUnit, D3DTSS_COLOROP, D3DTOP_MODULATE );
			ASSERT( result == D3D_OK );

			result = DirectX_Device->SetTextureStageState( textureUnit, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
			ASSERT( result == D3D_OK );

			result = DirectX_Device->SetTextureStageState( textureUnit, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			ASSERT( result == D3D_OK );

			result = DirectX_Device->SetTextureStageState( textureUnit, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
			ASSERT( result == D3D_OK );

			if( textureUnit == 0 )
			{
				result = DirectX_Device->SetTextureStageState( textureUnit, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
				ASSERT( result == D3D_OK );

				result = DirectX_Device->SetTextureStageState( textureUnit, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
				ASSERT( result == D3D_OK );
			}
			else
			{
				result = DirectX_Device->SetTextureStageState( textureUnit, D3DTSS_COLORARG2, D3DTA_CURRENT );
				ASSERT( result == D3D_OK );

				result = DirectX_Device->SetTextureStageState( textureUnit, D3DTSS_ALPHAARG2, D3DTA_CURRENT );
				ASSERT( result == D3D_OK );
			}
		}

		if( (TextureLayer.u8Flags & u8DisabledMaps) == 0 )
		{
			TextureState__Force( textureUnit, TS_BIND );
			TextureState__Force( textureUnit, TS_METHOD );
			TextureState__RequestAll( textureUnit, TS_TYPE_2D, pTexture, TextureLayer.u8Method, TextureLayer.u8WrapU, TextureLayer.u8WrapV, TextureLayer.u8Filter);

			if( TextureLayer.u8Flags & TEXTURE_IS_ENVIRONMENT_MAP )
			{
				D3DCOLOR	Blend = D3DCOLOR_RGBA( 255, 255, 255, (int) (255 * .2f) );

				DirectX_Device->SetRenderState( D3DRS_TEXTUREFACTOR, Blend );
				ASSERT( result == D3D_OK );

				result = DirectX_Device->SetTextureStageState( textureUnit, D3DTSS_COLOROP, D3DTOP_BLENDTEXTUREALPHA );
				ASSERT( result == D3D_OK );

				result = DirectX_Device->SetTextureStageState( textureUnit, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2 );
				ASSERT( result == D3D_OK );

				result = DirectX_Device->SetTextureStageState( textureUnit, D3DTSS_COLORARG1, D3DTA_TEXTURE );
				ASSERT( result == D3D_OK );

				result = DirectX_Device->SetTextureStageState( textureUnit, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
				ASSERT( result == D3D_OK );

				result = DirectX_Device->SetTextureStageState( textureUnit, D3DTSS_COLORARG2, D3DTA_CURRENT );
				ASSERT( result == D3D_OK );

				result = DirectX_Device->SetTextureStageState( textureUnit, D3DTSS_ALPHAARG2, D3DTA_CURRENT );
				ASSERT( result == D3D_OK );


				D3DXMATRIX mat;
				mat._11 = 0.5f; mat._12 = 0.0f;
				mat._21 = 0.0f; mat._22 =-0.5f;
				mat._31 = 0.0f; mat._32 = 0.0f;
				mat._41 = 0.5f; mat._42 = 0.5f;

				DirectX_Device->SetTransform( (D3DTRANSFORMSTATETYPE) (D3DTS_TEXTURE0 + textureUnit), &mat );
				DirectX_Device->SetTextureStageState( textureUnit, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );
				DirectX_Device->SetTextureStageState( textureUnit, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR );

			}
			else
			{
				result = DirectX_Device->SetTextureStageState( textureUnit, D3DTSS_TEXCOORDINDEX, textureCoordinateSet );
				ASSERT( result == D3D_OK );
			}

			textureCoordinateSet++;
		}
	
		pTexture->Unlock( );

		textureUnit++;
		textureLayer++;
	}

#ifdef _DEBUG
	while( p_Descriptor->m_StreamFlags & textureCoordinateSetBit )
	{
		textureCoordinateSet++;
		textureCoordinateSetBit <<= 1;
	}

//	ASSERT( textureCoordinateSet == textureCoordinateSetMax );
#endif

#ifndef DIRECTX_PC
	if( !(m_ShaderFlags & ShaderRecord::SHFLAG_NO_SHADOW) )
#endif
		ShadowMgr__ApplyShadows( textureUnit );

	if( m_BlendAlpha )
	{
		result = DirectX_Device->SetTextureStageState( textureUnit, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
		ASSERT( result == D3D_OK );
		result = DirectX_Device->SetTextureStageState( textureUnit, D3DTSS_COLORARG1, D3DTA_CURRENT );
		ASSERT( result == D3D_OK );

		result = DirectX_Device->SetTextureStageState( textureUnit, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
		ASSERT( result == D3D_OK );
		result = DirectX_Device->SetTextureStageState( textureUnit, D3DTSS_ALPHAARG1, D3DTA_TFACTOR );
		ASSERT( result == D3D_OK );
		result = DirectX_Device->SetTextureStageState( textureUnit, D3DTSS_ALPHAARG2, D3DTA_CURRENT );
		ASSERT( result == D3D_OK );

		textureUnit++;
	}

	ASSERT( textureUnit <= textureUnitMax );

	if( textureUnit < m_TextureUnitsClearStart )
	{
		unsigned int textureUnitClear = textureUnit;

		while( textureUnitClear < m_TextureUnitsClearStart )
		{
//			result = DirectX_Device->SetTexture( textureUnit, NULL );
//			ASSERT( result == D3D_OK );

			result = DirectX_Device->SetTextureStageState( textureUnitClear, D3DTSS_COLOROP, D3DTOP_DISABLE );
			ASSERT( result == D3D_OK );
			result = DirectX_Device->SetTextureStageState( textureUnitClear, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
			ASSERT( result == D3D_OK );

//			result = DirectX_Device->SetTextureStageState( textureUnitClear, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
//			ASSERT( result == D3D_OK );
//			result = DirectX_Device->SetTextureStageState( textureUnitClear, D3DTSS_TEXCOORDINDEX, textureUnit );
//			ASSERT( result == D3D_OK );

			textureUnitClear++;
		}
	}

	m_TextureUnitsClearStart = textureUnit;

	if( pass == 0 )
	{
		if( p_Descriptor->m_StreamFlags & STFLAG_COLOR_CHANNELS_MASK )
			result = DirectX_Device->SetRenderState( D3DRS_COLORVERTEX, TRUE );
		else
			result = DirectX_Device->SetRenderState( D3DRS_COLORVERTEX, FALSE );

		ASSERT( result == D3D_OK );

		DWORD dLighting = ( p_Descriptor->m_StreamFlags & STFLAG_NORMAL ) ? TRUE : FALSE;

		result = DirectX_Device->SetRenderState( D3DRS_LIGHTING, dLighting );
		ASSERT( result == D3D_OK );

		_RenderState__Change( RS_LIGHTING, dLighting );

		result = DirectX_Device->SetPixelShader( NULL );
		ASSERT( result == D3D_OK );

		if( p_Descriptor->m_StreamFVF != m_CurrentFVF )
		{
			result = DirectX_Device->SetVertexShader( p_Descriptor->m_StreamFVF );
			ASSERT( result == D3D_OK );

			m_CurrentFVF = p_Descriptor->m_StreamFVF;
		}
	}

}

void StandardTCMaterial::applyVSPShader( unsigned int pass )
{
	HRESULT result = D3D_OK;

	RenderState__Request( RS_CULL, m_ShaderFlags & ShaderRecord::SHFLAG_TWO_SIDED ? RS_CULL_NONE : RS_CULL_BACK );

 	unsigned int textureUnit = 0;
	unsigned int textureUnitMax = TextureState__GetNumUnits( );

	unsigned int textureCoordinateSet = 0;
	unsigned int textureCoordinateSetMax = (p_Descriptor->m_StreamFlags & STFLAG_TEXTURE_CHANNELS_MASK) >> STFLAG_TEXTURE_CHANNELS_SHIFT;
	unsigned int textureCoordinateSetBit = (1 << STFLAG_DUMMY_TEXTURE_SHIFT);

	unsigned int textureLayer = 0;
	unsigned int textureLayerCount = p_TextureContext->TextureCount;

	ASSERT( textureLayerCount <= textureUnitMax );

	while( textureLayer < textureLayerCount )	
	{
		TextureDatabase *	pDB;
		Texture *			pTexture;

		ts_TextureLayer & TextureLayer = p_TextureContext->TextureLayer[textureLayer];

		pDB = TextureLayer.textureDatabase;
		ASSERT_PTR( pDB );

		pTexture = pDB->GetTexture( TextureLayer.s16TextureIndex );
		ASSERT_PTR( pTexture );

		if( !(TextureLayer.u8Flags & TEXTURE_IS_ENVIRONMENT_MAP) )
		{
			pTexture->Lock( );
	
			_TextureState__Unit( textureUnit );
			_TextureState__Type( textureUnit, TS_TYPE_2D );
			_TextureState__Bind( textureUnit, (int) pTexture );
			_TextureState__Filter( textureUnit, TextureLayer.u8Filter );
	
			pTexture->Unlock( );
			textureUnit++;
		}

		textureCoordinateSet++;
		textureLayer++;
	}

	while( p_Descriptor->m_StreamFlags & textureCoordinateSetBit )
	{
		textureCoordinateSet++;
		textureCoordinateSetBit <<= 1;
	}

	ASSERT( textureCoordinateSet == textureCoordinateSetMax );

	ASSERT( textureUnit <= textureUnitMax );

	if( textureUnit < m_TextureUnitsClearStart )
	{
		unsigned int textureUnitClear = textureUnit;

		while( textureUnitClear < m_TextureUnitsClearStart )
		{
			result = DirectX_Device->SetTextureStageState( textureUnitClear, D3DTSS_COLOROP, D3DTOP_DISABLE );
			ASSERT( result == D3D_OK );
			result = DirectX_Device->SetTextureStageState( textureUnitClear, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
			ASSERT( result == D3D_OK );

			textureUnitClear++;
		}
	}

	m_TextureUnitsClearStart = textureUnit;

	ASSERT( pPShader );
	result = DirectX_Device->SetPixelShader( pPShader );
	ASSERT( result == D3D_OK );

	if( p_Shader != m_CurrentShader )
	{
		p_Shader->render( );

		m_CurrentShader = p_Shader;
	}

}

void StandardTCMaterial::applyVSMaterial( unsigned int pass )
{
	HRESULT				result = D3D_OK;
	u8						u8DisabledMaps = 0;


	RenderState__Request( RS_CULL, m_ShaderFlags & ShaderRecord::SHFLAG_TWO_SIDED ? RS_CULL_NONE : RS_CULL_BACK );

 	unsigned int textureUnit = 0;
	unsigned int textureUnitMax = TextureState__GetNumUnits( );

	unsigned int textureCoordinateSet = 0;
	unsigned int textureCoordinateSetMax = (p_Descriptor->m_StreamFlags & STFLAG_TEXTURE_CHANNELS_MASK) >> STFLAG_TEXTURE_CHANNELS_SHIFT;
	unsigned int textureCoordinateSetBit = (1 << STFLAG_DUMMY_TEXTURE_SHIFT);

	unsigned int textureLayer = 0;
	unsigned int textureLayerCount = p_TextureContext->TextureCount;

	ASSERT( textureLayerCount <= textureUnitMax );

	if( textureLayerCount == 0 )
	{
		RenderState__Request( RS_BLENDFUNC, RS_BLENDFUNC_DEFAULT );
		RenderState__Request( RS_ALPHAREF, RS_ALPHA_REF_DEFAULT_VALUE );
	}
	else
	{
		RenderState__Request( RS_BLENDFUNC, p_TextureContext->TextureLayer[0].u8Blend );
		RenderState__Request( RS_ALPHAREF, (p_TextureContext->TextureLayer[0].u8Flags & TEXTURE_ALPHA_REF) ? RS_ALPHA_REF_NO_HALO_VALUE : RS_ALPHA_REF_DEFAULT_VALUE );
	}

 	while( textureLayer < textureLayerCount )	
	{
		TextureDatabase *	pDB;
		Texture *			pTexture;

		ts_TextureLayer & TextureLayer = p_TextureContext->TextureLayer[textureLayer];

		pDB = TextureLayer.textureDatabase;
		ASSERT_PTR( pDB );

		pTexture = pDB->GetTexture( TextureLayer.s16TextureIndex );
		ASSERT_PTR( pTexture );

		pTexture->Lock( );

		if( pass == 0 )
		{
			result = DirectX_Device->SetTextureStageState( textureUnit, D3DTSS_COLOROP, D3DTOP_MODULATE );
			ASSERT( result == D3D_OK );

			result = DirectX_Device->SetTextureStageState( textureUnit, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
			ASSERT( result == D3D_OK );

			result = DirectX_Device->SetTextureStageState( textureUnit, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			ASSERT( result == D3D_OK );

			result = DirectX_Device->SetTextureStageState( textureUnit, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
			ASSERT( result == D3D_OK );

			if( textureUnit == 0 )
			{
				result = DirectX_Device->SetTextureStageState( textureUnit, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
				ASSERT( result == D3D_OK );

				result = DirectX_Device->SetTextureStageState( textureUnit, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
				ASSERT( result == D3D_OK );
			}
			else
			{
				result = DirectX_Device->SetTextureStageState( textureUnit, D3DTSS_COLORARG2, D3DTA_CURRENT );
				ASSERT( result == D3D_OK );

				result = DirectX_Device->SetTextureStageState( textureUnit, D3DTSS_ALPHAARG2, D3DTA_CURRENT );
				ASSERT( result == D3D_OK );
			}
		}

		if( (TextureLayer.u8Flags & u8DisabledMaps) == 0 )
		{
			u8 u8Method = (textureUnit == 0) ? TextureLayer.u8Method : TS_METHOD_DECAL;
			TextureState__Force( textureUnit, TS_METHOD );
			if( !(TextureLayer.u8Flags & TEXTURE_IS_ENVIRONMENT_MAP) )
				TextureState__RequestAll( textureUnit, TS_TYPE_2D, pTexture, u8Method, TextureLayer.u8WrapU, TextureLayer.u8WrapV, TextureLayer.u8Filter);

			textureCoordinateSet++;
		}

		pTexture->Unlock( );

		textureUnit++;
		textureLayer++;
	}

	while( p_Descriptor->m_StreamFlags & textureCoordinateSetBit )
	{
		textureCoordinateSet++;
		textureCoordinateSetBit <<= 1;
	}

//	ASSERT( textureCoordinateSet == textureCoordinateSetMax );

	if( m_BlendAlpha )
	{
		result = DirectX_Device->SetTextureStageState( textureUnit, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
		ASSERT( result == D3D_OK );
		result = DirectX_Device->SetTextureStageState( textureUnit, D3DTSS_COLORARG1, D3DTA_CURRENT );
		ASSERT( result == D3D_OK );

		result = DirectX_Device->SetTextureStageState( textureUnit, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
		ASSERT( result == D3D_OK );
		result = DirectX_Device->SetTextureStageState( textureUnit, D3DTSS_ALPHAARG1, D3DTA_TFACTOR );
		ASSERT( result == D3D_OK );
		result = DirectX_Device->SetTextureStageState( textureUnit, D3DTSS_ALPHAARG2, D3DTA_CURRENT );
		ASSERT( result == D3D_OK );

		textureUnit++;
	}

	ASSERT( textureUnit <= textureUnitMax );

	if( textureUnit < m_TextureUnitsClearStart )
	{
		unsigned int textureUnitClear = textureUnit;

		while( textureUnitClear < m_TextureUnitsClearStart )
		{
//			result = DirectX_Device->SetTexture( textureUnitClear, NULL );
//			ASSERT( result == D3D_OK );

			result = DirectX_Device->SetTextureStageState( textureUnitClear, D3DTSS_COLOROP, D3DTOP_DISABLE );
			ASSERT( result == D3D_OK );
			result = DirectX_Device->SetTextureStageState( textureUnitClear, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
			ASSERT( result == D3D_OK );

			textureUnitClear++;
		}
	}

	m_TextureUnitsClearStart = textureUnit;

	result = DirectX_Device->SetRenderState( D3DRS_SPECULARENABLE, TRUE );
	ASSERT( result == D3D_OK );

	result = DirectX_Device->SetPixelShader( NULL );
	ASSERT( result == D3D_OK );

	if( p_Shader != m_CurrentShader )
	{
		p_Shader->render( );

		m_CurrentShader = p_Shader;
	}
}

void StandardTCMaterial::setMaterial( void )
{
D3DMATERIAL8			Material; 

HRESULT					result = D3D_OK;

	ZeroMemory( &Material, sizeof( Material ) );
	
	Material.Diffuse.r = 1.0f; 
	Material.Diffuse.g = 1.0f; 
	Material.Diffuse.b = 1.0f; 
	Material.Diffuse.a = 1.0f; 
	
	Material.Ambient.r = 1.0f; 
	Material.Ambient.g = 1.0f; 
	Material.Ambient.b = 1.0f; 
	Material.Ambient.a = 1.0f; 
	
	Material.Specular.r = 0.0f; 
	Material.Specular.g = 0.0f; 
	Material.Specular.b = 0.0f; 
	Material.Specular.a = 1.0f; 
	Material.Power = 20.0f; 
	
	Material.Emissive.r = 0.0f; 
	Material.Emissive.g = 0.0f; 
	Material.Emissive.b = 0.0f; 
	Material.Emissive.a = 1.0f; 
	
	result = DirectX_Device->SetMaterial( & Material );
	ASSERT( result == D3D_OK );

	result = DirectX_Device->SetRenderState( D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL );
	ASSERT( result == D3D_OK );

	result = DirectX_Device->SetRenderState( D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL );
	ASSERT( result == D3D_OK );

	result = DirectX_Device->SetRenderState( D3DRS_SPECULARMATERIALSOURCE, D3DMCS_MATERIAL );
	ASSERT( result == D3D_OK );

	result = DirectX_Device->SetRenderState( D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_COLOR1 );
	ASSERT( result == D3D_OK );

	result = DirectX_Device->SetRenderState( D3DRS_LOCALVIEWER, TRUE );
	ASSERT( result == D3D_OK );

	result = DirectX_Device->SetRenderState( D3DRS_SPECULARENABLE, FALSE );
	ASSERT( result == D3D_OK );
}

