///////////////////////////////////////////////////////////////////////////
//
//  TextureState.c
//
//  Texture State Management Module.
//
//  Ned Martin, May, 2000.
//  Nate Robins, September 2000.
//
//  Copyright (c) 2000 Avalanche Software.
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
//  Includes
////////////////////////////////////////////////////////////////////////////

#include "Display/DisplayPCH.h"





////////////////////////////////////////////////////////////////////////////
//  Globals
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
//  Functions
////////////////////////////////////////////////////////////////////////////

void _TextureState__Unit( int value )
{
	ASSERT( value == 0 );
	_TextureState_ActiveUnit = value;
}

////////////////////////////////////////////////////////////////////////////

void _TextureState__Type( int unit, int value )
{
    ASSERT( value == TS_TYPE_2D || value == TS_TYPE_NONE );

    _TextureState__Change( unit, TS_TYPE, value );
}

////////////////////////////////////////////////////////////////////////////

void _TextureState__Bind( int unit, int value )
{
    int type;

    ASSERT( unit == _TextureState_ActiveUnit );

	 /* send texture state registers down */


	if (value != 0)	// 0 is an invalid bind
	{
		Texture* tex = (Texture*)value;
		tex->Select();
	}
	else
	{
		Texture* tex = Texture::GetSelectedTexture();
		if(tex)
			tex->Unselect();
	}

	/* keep track of the current bind */

   _TextureState__Change( unit, TS_BIND, value );
}

////////////////////////////////////////////////////////////////////////////

void _TextureState__Method( int unit, int value )
{
    _TextureState__Change( unit, TS_METHOD, value );
}

////////////////////////////////////////////////////////////////////////////

void _TextureState__WrapU( int unit, int value )
{
	ASSERT( unit == _TextureState_ActiveUnit );
	ASSERT( value == TS_WRAP_CLAMP ||
           value == TS_WRAP_REPEAT );

	/* on ps2, this is called by _FixTextureContext at texture load time */

	_TextureState__Change( unit, TS_WRAPU, value );
}

////////////////////////////////////////////////////////////////////////////

void _TextureState__WrapV( int unit, int value )
{
	ASSERT( unit == _TextureState_ActiveUnit );
	ASSERT( value == TS_WRAP_CLAMP ||
           value == TS_WRAP_REPEAT );

	/* on ps2, this is called by _FixTextureContext at texture load time */

	_TextureState__Change( unit, TS_WRAPV, value );
}

////////////////////////////////////////////////////////////////////////////

void _TextureState__Filter( int unit, int value )
{
	ASSERT( unit == _TextureState_ActiveUnit );
   ASSERT( value == TS_FILTER_POINTSAMPLE					|| 
			  value == TS_FILTER_BILINEAR						||
			  value == TS_FILTER_NEAREST_MIPMAP_NEAREST	||
			  value == TS_FILTER_NEAREST_MIPMAP_LINEAR	||
			  value == TS_FILTER_LINEAR_MIPMAP_NEAREST	||
			  value == TS_FILTER_LINEAR_MIPMAP_LINEAR		 );

	/* on ps2, this is called by _FixTextureContext at texture load time */

	_TextureState__Change( unit, TS_FILTER, value );
}


////////////////////////////////////////////////////////////////////////////

void _TextureState__Effect( int unit, int value )
{
	ASSERT( unit == _TextureState_ActiveUnit );
	ASSERT( value == TS_EFFECT_NONE || 
		     value == TS_EFFECT_ENVMAP );
	
	switch ( value )
	{
//	case TS_EFFECT_NONE:
//		D3DSTSS( unit, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
//		D3DSTSS( unit, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU );
//		break;
//		
//	case TS_EFFECT_ENVMAP:
//		{
//			D3DXMATRIX m;
//			m._11 = 0.5f; m._12 =  0.0f;
//			m._21 = 0.0f; m._22 = -0.5f;
//			m._31 = 0.0f; m._32 =  0.0f;
//			m._41 = 0.5f; m._42 =  0.5f;
//			IDirect3DDevice8_SetTransform( DirectX_Device, (D3DTRANSFORMSTATETYPE)(D3DTS_TEXTURE0 + unit), &m );
//		}
//		D3DSTSS( unit, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );
//		D3DSTSS( unit, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACENORMAL );
//		break;
//		
//	default:
//		ASSERT( 0 );
//		break;
	}
	
	_TextureState__Change( unit, TS_EFFECT, value );
}

////////////////////////////////////////////////////////////////////////////

