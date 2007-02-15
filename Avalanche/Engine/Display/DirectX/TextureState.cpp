///////////////////////////////////////////////////////////////////////////
//
//  TextureState.c
//
//  Texture State Management Module.
//
//  Nate Robins, September 2000.
//
//  Copyright (c) 2000 Avalanche Software.
//
////////////////////////////////////////////////////////////////////////////

#ifndef DIRECTX
#pragma message( "WARNING: DIRECTX should be defined when compiling this module!" )
#endif


////////////////////////////////////////////////////////////////////////////
//  Includes
////////////////////////////////////////////////////////////////////////////

#include "Display/DisplayPCH.h"


////////////////////////////////////////////////////////////////////////////
//  Globals
////////////////////////////////////////////////////////////////////////////

int _TextureState_Wrap[] =
{
	D3DTADDRESS_CLAMP,
	D3DTADDRESS_WRAP,
};


////////////////////////////////////////////////////////////////////////////
//  Functions
////////////////////////////////////////////////////////////////////////////

void _TextureState__Unit( int value )
{
	_TextureState_ActiveUnit = value;
}

////////////////////////////////////////////////////////////////////////////

void _TextureState__Type( int unit, int value )
{
	ASSERT( value == TS_TYPE_2D || value == TS_TYPE_NONE );
	
	if ( value == TS_TYPE_NONE )
	{
		D3DST( unit, NULL );
	}
	
	_TextureState__Change( unit, TS_TYPE, value );
}

////////////////////////////////////////////////////////////////////////////

void _TextureState__Bind( int unit, int value )
{
	ASSERT( unit == _TextureState_ActiveUnit );
	
	int type = TextureState__Get( unit, TS_TYPE );
	
	if ( type != TS_TYPE_NONE )
	{
			Texture * pTex =	Texture::GetSelectedTexture( );

			if( pTex )
				pTex->Unselect( );

			if( value )
			{
				pTex = (Texture *) value;
				pTex->Select( );
			}
	}
	else
	{
		Texture * pTex =	Texture::GetSelectedTexture( );

		if( pTex )
			pTex->Unselect( );
	}
	
	_TextureState__Change( unit, TS_BIND, value );
}

////////////////////////////////////////////////////////////////////////////

void _TextureState__Method( int unit, int value )
{
	ASSERT( unit == _TextureState_ActiveUnit );
	ASSERT( value == TS_METHOD_PASSTHRU		||
		value == TS_METHOD_REPLACE				||
		value == TS_METHOD_INTERPOLATE		||
		value == TS_METHOD_CONSTANTALPHA		||
		value == TS_METHOD_MODULATE			|| 
		value == TS_METHOD_MODULATESELECT	|| 
		value == TS_METHOD_DOTPRODUCT			|| 
		value == TS_METHOD_DISABLE				||
		value == TS_METHOD_DECAL				||
		value == TS_METHOD_GLOSS				||
		value == TS_METHOD_DOT3);
	
	switch ( value )
	{
	case TS_METHOD_DISABLE:
		D3DSTSS( unit, D3DTSS_COLOROP, D3DTOP_DISABLE );
		D3DSTSS( unit, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
		break;
	case TS_METHOD_REPLACE:
		//we want C=Ct
		//we want A=At
		D3DSTSS( unit, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		D3DSTSS( unit, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		D3DSTSS( unit, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		D3DSTSS( unit, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
		D3DSTSS( unit, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
		D3DSTSS( unit, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
		break;

	case TS_METHOD_GLOSS:
		D3DSTSS( unit, D3DTSS_COLOROP,	D3DTOP_BLENDCURRENTALPHA );
		D3DSTSS( unit, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		D3DSTSS( unit, D3DTSS_COLORARG2, D3DTA_CURRENT );

		D3DSTSS( unit, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		D3DSTSS( unit, D3DTSS_ALPHAOP,	D3DTOP_SELECTARG1 );
		break;

	case TS_METHOD_DOT3:
		D3DSTSS( unit, D3DTSS_COLOROP,	D3DTOP_DOTPRODUCT3 );
		D3DSTSS( unit, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		D3DSTSS( unit, D3DTSS_COLORARG2, D3DTA_TFACTOR );

		D3DSTSS( unit, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		D3DSTSS( unit, D3DTSS_ALPHAOP,	D3DTOP_SELECTARG1 );
		break;

	case TS_METHOD_DECAL:
		//we want C=CtAt+Cf(1-At)
		//we want A=Af
		D3DSTSS( unit, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		D3DSTSS( unit, D3DTSS_COLORARG2, D3DTA_CURRENT );
		D3DSTSS( unit, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		D3DSTSS( unit, D3DTSS_ALPHAARG2, D3DTA_CURRENT );
		D3DSTSS( unit, D3DTSS_COLOROP, D3DTOP_BLENDTEXTUREALPHA );
		D3DSTSS( unit, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2 );
		break;
		
	case TS_METHOD_MODULATE:
		//we want C=CtCf
		//we want A=AtAf
		D3DSTSS( unit, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		D3DSTSS( unit, D3DTSS_COLORARG2, D3DTA_CURRENT );
		D3DSTSS( unit, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		D3DSTSS( unit, D3DTSS_ALPHAARG2, D3DTA_CURRENT );
		D3DSTSS( unit, D3DTSS_COLOROP, D3DTOP_MODULATE );
		D3DSTSS( unit, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
		break;

	case TS_METHOD_MODULATESELECT:
		//we want C=CtCf
		//we want A=AtAf
		D3DSTSS( unit, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		D3DSTSS( unit, D3DTSS_COLORARG2, D3DTA_CURRENT );
		D3DSTSS( unit, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		D3DSTSS( unit, D3DTSS_ALPHAARG2, D3DTA_CURRENT );
		D3DSTSS( unit, D3DTSS_COLOROP, D3DTOP_MODULATE );
		D3DSTSS( unit, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
		break;
		
	case TS_METHOD_INTERPOLATE:
		//we want C=CtAc+Cf(1-Ac)
		D3DSTSS( unit, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		D3DSTSS( unit, D3DTSS_COLORARG2, D3DTA_CURRENT );
		D3DSTSS( unit, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		D3DSTSS( unit, D3DTSS_ALPHAARG2, D3DTA_CURRENT );
		D3DSTSS( unit, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
		D3DSTSS( unit, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2 );
		break;
		
	case TS_METHOD_CONSTANTALPHA:
		//we want C=CtCf
		//we want A=AcAt
		D3DSTSS( unit, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		D3DSTSS( unit, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		D3DSTSS( unit, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		D3DSTSS( unit, D3DTSS_ALPHAARG2, D3DTA_TFACTOR );
		D3DSTSS( unit, D3DTSS_COLOROP, D3DTOP_MODULATE );
		D3DSTSS( unit, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
		break;
		
	case TS_METHOD_PASSTHRU:
		//we want C=Cf
		//we want A=Af
		D3DSTSS( unit, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		D3DSTSS( unit, D3DTSS_COLORARG2, D3DTA_CURRENT );
		D3DSTSS( unit, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		D3DSTSS( unit, D3DTSS_ALPHAARG2, D3DTA_CURRENT );
		D3DSTSS( unit, D3DTSS_COLOROP, D3DTOP_SELECTARG2 );
		D3DSTSS( unit, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2 );
		break;
		
	case TS_METHOD_DOTPRODUCT:
		//we want Crgba = Ar * Br + Ag * Bg + Ab * Bb
		//where A == normal (from normal map texture)
		//where B == light dir (from diffuse vertex color)
		D3DSTSS( unit, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		D3DSTSS( unit, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		D3DSTSS( unit, D3DTSS_COLOROP,   D3DTOP_DOTPRODUCT3 );
		break;
		
	default:
		ASSERT( 0 );
		break;
	}
	
	_TextureState__Change( unit, TS_METHOD, value );
}

////////////////////////////////////////////////////////////////////////////

void _TextureState__WrapU( int unit, int value )
{
	ASSERT( unit == _TextureState_ActiveUnit );
	ASSERT( value == TS_WRAP_CLAMP || 
		value == TS_WRAP_REPEAT );
	
	switch ( value )
	{
	case TS_WRAP_CLAMP:
		D3DSTSS( unit, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
		break;
		
	case TS_WRAP_REPEAT:
		D3DSTSS( unit, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP );
		break;
		
	default:
		ASSERT( 0 );
		break;
	}
	
	_TextureState__Change( unit, TS_WRAPU, value );
}

////////////////////////////////////////////////////////////////////////////

void _TextureState__WrapV( int unit, int value )
{
	ASSERT( unit == _TextureState_ActiveUnit );
	ASSERT( value == TS_WRAP_CLAMP || 
		value == TS_WRAP_REPEAT );
	
	switch ( value )
	{
	case TS_WRAP_CLAMP:
		D3DSTSS( unit, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );
		break;
		
	case TS_WRAP_REPEAT:
		D3DSTSS( unit, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP );
		break;
		
	default:
		ASSERT( 0 );
		break;
	}
	
	_TextureState__Change( unit, TS_WRAPV, value );
}

////////////////////////////////////////////////////////////////////////////

void _TextureState__Filter( int unit, int value )
{
	ASSERT( unit == _TextureState_ActiveUnit );
	ASSERT(	value == TS_FILTER_POINTSAMPLE				||
		value == TS_FILTER_BILINEAR					||
		value == TS_FILTER_NEAREST_MIPMAP_NEAREST	||
		value == TS_FILTER_NEAREST_MIPMAP_LINEAR	||
		value == TS_FILTER_LINEAR_MIPMAP_NEAREST	||
		value == TS_FILTER_LINEAR_MIPMAP_LINEAR	 );

#ifdef DIRECTX_PC
	// override texture filter settings if aniso is selected
	if(_TextureState_AnisotropyLevel)
	{
		D3DSTSS( unit, D3DTSS_MAXANISOTROPY, _TextureState_AnisotropyLevel );
		D3DSTSS( unit, D3DTSS_MINFILTER, D3DTEXF_ANISOTROPIC );
		D3DSTSS( unit, D3DTSS_MAGFILTER, D3DTEXF_ANISOTROPIC );
		D3DSTSS( unit, D3DTSS_MIPFILTER, D3DTEXF_ANISOTROPIC );
		return;
	}
#endif

	switch ( value )
	{
	case TS_FILTER_POINTSAMPLE:
		D3DSTSS( unit, D3DTSS_MINFILTER, D3DTEXF_POINT );
		D3DSTSS( unit, D3DTSS_MAGFILTER, D3DTEXF_POINT );
		D3DSTSS( unit, D3DTSS_MIPFILTER, D3DTEXF_NONE );
		break;
		
	case TS_FILTER_BILINEAR:
		D3DSTSS( unit, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
		D3DSTSS( unit, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
		D3DSTSS( unit, D3DTSS_MIPFILTER, D3DTEXF_NONE );
		break;
		
	case TS_FILTER_NEAREST_MIPMAP_NEAREST:
		D3DSTSS( unit, D3DTSS_MINFILTER, D3DTEXF_POINT );
		D3DSTSS( unit, D3DTSS_MAGFILTER, D3DTEXF_POINT );
		D3DSTSS( unit, D3DTSS_MIPFILTER, D3DTEXF_POINT );
		break;
		
	case TS_FILTER_NEAREST_MIPMAP_LINEAR:
		D3DSTSS( unit, D3DTSS_MINFILTER, D3DTEXF_POINT );
		D3DSTSS( unit, D3DTSS_MAGFILTER, D3DTEXF_POINT );
		D3DSTSS( unit, D3DTSS_MIPFILTER, D3DTEXF_LINEAR );
		break;
		
	case TS_FILTER_LINEAR_MIPMAP_NEAREST:
		D3DSTSS( unit, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
		D3DSTSS( unit, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
		D3DSTSS( unit, D3DTSS_MIPFILTER, D3DTEXF_POINT );
		break;
		
	case TS_FILTER_LINEAR_MIPMAP_LINEAR:
		D3DSTSS( unit, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
		D3DSTSS( unit, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
		D3DSTSS( unit, D3DTSS_MIPFILTER, D3DTEXF_LINEAR );
		break;
		
	default:
		ASSERT( 0 );
		break;
	}
	
	_TextureState__Change( unit, TS_FILTER, value );
}

////////////////////////////////////////////////////////////////////////////

void _TextureState__Effect( int unit, int value )
{
	ASSERT( unit == _TextureState_ActiveUnit );
	ASSERT( value == TS_EFFECT_NONE || 
		     value == TS_EFFECT_ENVMAP ||
		     value == TS_EFFECT_SHADOWMAP );
	
	switch ( value )
	{
	case TS_EFFECT_NONE:
//		{
//			D3DXMATRIX m;
//			D3DXMatrixIdentity( &m );
//			IDirect3DDevice8_SetTransform( DirectX_Device, (D3DTRANSFORMSTATETYPE)(D3DTS_TEXTURE0 + unit), &m );
//		}
//		D3DSTSS( unit, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
//		D3DSTSS( unit, D3DTSS_TEXCOORDINDEX, unit );
		break;
		
	case TS_EFFECT_ENVMAP:
		{
			D3DXMATRIX m;
			m._11 = 0.5f; m._12 =  0.0f;
			m._21 = 0.0f; m._22 = -0.5f;
			m._31 = 0.0f; m._32 =  0.0f;
			m._41 = 0.5f; m._42 =  0.5f;
			IDirect3DDevice8_SetTransform( DirectX_Device, (D3DTRANSFORMSTATETYPE)(D3DTS_TEXTURE0 + unit), &m );
		}
		D3DSTSS( unit, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );
		D3DSTSS( unit, D3DTSS_TEXCOORDINDEX, unit | D3DTSS_TCI_CAMERASPACENORMAL );
		break;
		
	case TS_EFFECT_SHADOWMAP:
		D3DSTSS( unit, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT3 | D3DTTFF_PROJECTED );
		D3DSTSS( unit, D3DTSS_TEXCOORDINDEX, unit | D3DTSS_TCI_CAMERASPACEPOSITION  );
		break;
			
	default:
		ASSERT( 0 );
		break;
	}
	
	_TextureState__Change( unit, TS_EFFECT, value );
}
