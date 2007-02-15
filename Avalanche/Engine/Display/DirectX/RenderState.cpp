///////////////////////////////////////////////////////////////////////////
//
//  RenderState.c
//
//  RenderState Rendering Module.
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
#include "Display/DirectX/DXSharedDCache.h"

const float		dxLightAmbientScale = 1.0f;
const float		dxLightDirectionalScale = 1.0f;


////////////////////////////////////////////////////////////////////////////
//  Defines
////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////
//  Types
////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////
//  Globals
////////////////////////////////////////////////////////////////////////////

int _RenderState_Primitives[] =
{
	D3DPT_POINTLIST,
	D3DPT_LINELIST,
	D3DPT_LINESTRIP,
	0,  //LINELOOP,
	D3DPT_TRIANGLELIST,
	D3DPT_TRIANGLESTRIP,
	D3DPT_TRIANGLEFAN,
	0,  //QUADS,
	0,  //QUADSTRIP,
	0,  //POLYGON,
	0,  //DRAWARRAY,
	0,  //DISPLAYLIST,
};

////////////////////////////////////////////////////////////////////////////

int _RenderState_ZFunc[] =
{
	D3DCMP_NEVER,
	D3DCMP_ALWAYS,
	D3DCMP_EQUAL,
	D3DCMP_NOTEQUAL,
	D3DCMP_LESS,
	D3DCMP_LESSEQUAL,
	D3DCMP_GREATER,
	D3DCMP_GREATEREQUAL,
};

////////////////////////////////////////////////////////////////////////////

int _RenderState_PolygonMode[] =
{
	D3DFILL_POINT,
	D3DFILL_WIREFRAME,
	D3DFILL_SOLID,
};

////////////////////////////////////////////////////////////////////////////

int _RenderState_FrontFace[] =
{
	// These values are intentionally swapped, since DirectX has no notion 
	// of front face. It only cares about which face is culled. So, by 
	// switching the sense of these two, selecting front face of CW will 
	// actually cull CCW, which is what is wanted. 
	D3DCULL_CCW,
	D3DCULL_CW,
};

////////////////////////////////////////////////////////////////////////////

int _RenderState_Cull[] =
{
	// These values are intentionally swapped, since DirectX uses a left-handed
	// coordinate system.
	D3DCULL_CCW,
	D3DCULL_CW,
	D3DCULL_NONE,
};

////////////////////////////////////////////////////////////////////////////

int _RenderState_FogMode[] =
{
	D3DFOG_LINEAR,
	D3DFOG_EXP,
	D3DFOG_EXP2,
	D3DFOG_NONE,
};

////////////////////////////////////////////////////////////////////////////

int _RenderState_AlphaFunc[] =
{
	D3DCMP_NEVER,
	D3DCMP_ALWAYS,
	D3DCMP_EQUAL,
	D3DCMP_NOTEQUAL,
	D3DCMP_LESS,
	D3DCMP_LESSEQUAL,
	D3DCMP_GREATER,
	D3DCMP_GREATEREQUAL,
};


////////////////////////////////////////////////////////////////////////////
//  Functions
////////////////////////////////////////////////////////////////////////////

void _RenderState__Begin( int primitive )
{
}

////////////////////////////////////////////////////////////////////////////

void _RenderState__End( void )
{
}


////////////////////////////////////////////////////////////////////////////
void RenderState__RegisterHardwareAmbientLight( ts_fRGB Color )
{
	DXShaderDCache.m_SkyColor.r = Color.R * dxLightAmbientScale;
	DXShaderDCache.m_SkyColor.g = Color.G * dxLightAmbientScale;
	DXShaderDCache.m_SkyColor.b = Color.B * dxLightAmbientScale;

	DXShaderDCache.m_GroundColor.r = DXShaderDCache.m_SkyColor.r;
	DXShaderDCache.m_GroundColor.g = DXShaderDCache.m_SkyColor.g;
	DXShaderDCache.m_GroundColor.b = DXShaderDCache.m_SkyColor.b;

	unsigned char r = (unsigned char) ( DXShaderDCache.m_SkyColor.r * 255.0f );
	unsigned char g = (unsigned char) ( DXShaderDCache.m_SkyColor.g * 255.0f );
	unsigned char b = (unsigned char) ( DXShaderDCache.m_SkyColor.b * 255.0f );
	
	D3DCOLOR color  = D3DCOLOR_XRGB( r, g, b );
	
	HRESULT result = DirectX_Device->SetRenderState( D3DRS_AMBIENT, color );
	ASSERT( result == D3D_OK );
}

////////////////////////////////////////////////////////////////////////////

void _RenderState__Light( ts_Light* pLight, D3DLIGHT8* light )
{
	ASSERT( 0 );
}

////////////////////////////////////////////////////////////////////////////

void RenderState__RegisterHardwareLight( ts_Light* pLight )
{
	ASSERT( 0 );
}

////////////////////////////////////////////////////////////////////////////

void RenderState__RegisterLightSetHardwareLight( u32 index, CLightRecord *pLight, float attenuationScale )
{
bool					colorMultiplierDirty;
float					colorMultiplier;

	ASSERT( index < DXShaderDCache.m_Lights );

	D3DLIGHT8 * pDXLight = DXShaderDCache.p_Lights + index;

	ZeroMemory( pDXLight, sizeof( D3DLIGHT8 ) );

	pDXLight->Ambient.r = 0.0f;
	pDXLight->Ambient.g = 0.0f;
	pDXLight->Ambient.b = 0.0f;
	pDXLight->Ambient.a = 0.0f;

	pDXLight->Specular.r = 1.0f;
	pDXLight->Specular.g = 1.0f;
	pDXLight->Specular.b = 1.0f;
	pDXLight->Specular.a = 0.0f;

	Vector4 Color = pLight->getColor( ) * pLight->getIntensity( );

	switch( pLight->getType( ) )
	{
	case CLightRecord::LIGHT_OMNI:
		if( attenuationScale == 0.0f )
			return;

		pDXLight->Type = D3DLIGHT_POINT;
		pDXLight->Diffuse.r = Color.x( );
		pDXLight->Diffuse.g = Color.y( );
		pDXLight->Diffuse.b = Color.z( );

		pDXLight->Position.x = pLight->getPosition( ).x( );
		pDXLight->Position.y = pLight->getPosition( ).y( );
		pDXLight->Position.z = pLight->getPosition( ).z( );

		pDXLight->Direction.x = 0;
		pDXLight->Direction.y = 0;
		pDXLight->Direction.z = 0;

		pDXLight->Range = 1000.0f;
		pDXLight->Attenuation2 = .33f * ( pLight->getQuadraticAttenuation( ) * attenuationScale );
		break;
	case CLightRecord::LIGHT_FREE_DIRECT:
		colorMultiplierDirty = g_lightSet.getLightColorMult( colorMultiplier );
		colorMultiplier *= dxLightDirectionalScale;
		
		pDXLight->Type = D3DLIGHT_DIRECTIONAL;

		pDXLight->Diffuse.r = Color.x( ) * colorMultiplier;
		pDXLight->Diffuse.g = Color.y( ) * colorMultiplier;
		pDXLight->Diffuse.b = Color.z( ) * colorMultiplier;

		pDXLight->Direction.x = pLight->getDirection( ).x( );
		pDXLight->Direction.y = pLight->getDirection( ).y( );
		pDXLight->Direction.z = pLight->getDirection( ).z( );
		break;
	}

}

////////////////////////////////////////////////////////////////////////////

void RenderState__DisableHardwareLightRange( u32 from )
{
	if( from >= DXShaderDCache.m_Lights )
		return;

	for( unsigned int i = from; i < DXShaderDCache.m_Lights; i++ )
		DXShaderDCache.p_Lights[i].Type = (D3DLIGHTTYPE) 0;

}


////////////////////////////////////////////////////////////////////////////

void RenderState__RemoveHardwareLight( ts_Light* pLight )
{
	ASSERT( 0 );

}

////////////////////////////////////////////////////////////////////////////

bool RenderState__HardwareLightsEmpty( void )
{
	return( _RenderState_NumLights == 0 );
}

////////////////////////////////////////////////////////////////////////////

void _RenderState__SendHardwareLights( void )
{
}

////////////////////////////////////////////////////////////////////////////

void RenderState__PrepareHardwareLights( void )
{
}

// States //////////////////////////////////////////////////////////////////

void _RenderState__Primitive( int value )
{
	// It is important to call the change function because this is where 
	// the state structure actually gets updated. 
	_RenderState__Change( RS_PRIMITIVE, value );
}

////////////////////////////////////////////////////////////////////////////

void _RenderState__BlendFunc( int value )
{
	switch ( value )
	{
	case RS_BLENDFUNC_NONE:
		D3DSRS( D3DRS_ALPHABLENDENABLE, FALSE );
		D3DSRS( D3DRS_BLENDOP, D3DBLENDOP_ADD );
		break;
		
	case RS_BLENDFUNC_FIXEDALPHA:
		// TJC - make this the same as default.  that lets me specify
		// RS_BLENDFUNC_FIXEDALPHA together with TS_METHOD_CONSTANTALPHA,
		// and just set both RS_FIXEDALPHA and RS_TEXTURE_CONSTANTALPHA
		// and get the same look cross platform.
	case RS_BLENDFUNC_DEFAULT:
		D3DSRS( D3DRS_ALPHABLENDENABLE, TRUE );
		D3DSRS( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
		D3DSRS( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		D3DSRS( D3DRS_BLENDOP, D3DBLENDOP_ADD );
		break;
		 
	case	RS_BLENDFUNC_GLOSS:
		D3DSRS( D3DRS_ALPHABLENDENABLE, TRUE );
		D3DSRS( D3DRS_SRCBLEND,  D3DBLEND_ZERO );
		D3DSRS( D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR );
		D3DSRS( D3DRS_BLENDOP, D3DBLENDOP_ADD );
		break;

	case	RS_BLENDFUNC_DOT3:
		break;

	case RS_BLENDFUNC_DIFFUSE:
		D3DSRS( D3DRS_ALPHABLENDENABLE, TRUE );
		D3DSRS( D3DRS_SRCBLEND,  D3DBLEND_DESTCOLOR );
		D3DSRS( D3DRS_DESTBLEND, D3DBLEND_ZERO );
		D3DSRS( D3DRS_BLENDOP, D3DBLENDOP_ADD );
		break;
		
	case RS_BLENDFUNC_SATURATE:
		D3DSRS( D3DRS_ALPHABLENDENABLE, TRUE );
		D3DSRS( D3DRS_SRCBLEND,  D3DBLEND_ONE );
		D3DSRS( D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR );
		D3DSRS( D3DRS_BLENDOP, D3DBLENDOP_ADD );
		break;
		
	case RS_BLENDFUNC_ADD:
		D3DSRS( D3DRS_ALPHABLENDENABLE, TRUE );
		D3DSRS( D3DRS_SRCBLEND,  D3DBLEND_ONE );
		D3DSRS( D3DRS_DESTBLEND, D3DBLEND_ONE );
		D3DSRS( D3DRS_BLENDOP, D3DBLENDOP_ADD );
		break;
		
	case RS_BLENDFUNC_DOUBLE:
		D3DSRS( D3DRS_ALPHABLENDENABLE, TRUE );
		D3DSRS( D3DRS_SRCBLEND,  D3DBLEND_DESTCOLOR );
		D3DSRS( D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR );
		D3DSRS( D3DRS_BLENDOP, D3DBLENDOP_ADD );
		break;
		
	case RS_BLENDFUNC_NEGATIVE:
		// cool... there's a way to sort of do what the PS2 does here...
		D3DSRS( D3DRS_ALPHABLENDENABLE, TRUE );
		D3DSRS( D3DRS_SRCBLEND,  D3DBLEND_INVDESTCOLOR );
		D3DSRS( D3DRS_DESTBLEND, D3DBLEND_ZERO );
		D3DSRS( D3DRS_BLENDOP, D3DBLENDOP_ADD );
		break;
		
	case RS_BLENDFUNC_SUBTRACTIVE:	// used for shadow rendering on ps2
		D3DSRS( D3DRS_ALPHABLENDENABLE, TRUE );
		D3DSRS( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		D3DSRS( D3DRS_DESTBLEND, D3DBLEND_ONE );
		D3DSRS( D3DRS_BLENDOP, D3DBLENDOP_REVSUBTRACT );
		break;

	case RS_BLENDFUNC_ADDITIVE:
		// frame texel = src_texel * src_alpha + frame_texel * 1-- for additive particle effects
		D3DSRS( D3DRS_ALPHABLENDENABLE, TRUE );
		D3DSRS( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
		D3DSRS( D3DRS_DESTBLEND, D3DBLEND_ONE );
		D3DSRS( D3DRS_BLENDOP, D3DBLENDOP_ADD );
		break;
		
	case RS_BLENDFUNC_CONSTANTALPHA:
#ifdef _XBOX
		D3DSRS( D3DRS_ALPHABLENDENABLE, TRUE );
		D3DSRS( D3DRS_SRCBLEND,  D3DBLEND_CONSTANTALPHA );
		D3DSRS( D3DRS_DESTBLEND, D3DBLEND_INVCONSTANTALPHA );
		D3DSRS( D3DRS_BLENDOP, D3DBLENDOP_ADD );
#endif
		break;
	default:
		ASSERT( 0 );
		break;
	}
	
	_RenderState__Change( RS_BLENDFUNC, value );
}

////////////////////////////////////////////////////////////////////////////

void _RenderState__ZFunc( int value )
{
	ASSERT( value >= 0 && value < sizeof ( _RenderState_ZFunc ) / sizeof ( int ) );
	
	D3DSRS( D3DRS_ZFUNC, _RenderState_ZFunc[ value ] );
	
	_RenderState__Change( RS_ZFUNC, value );
}

////////////////////////////////////////////////////////////////////////////

void _RenderState__ZTest( int value )
{
	ASSERT( value == RS_TRUE || value == RS_FALSE );
	
	D3DSRS( D3DRS_ZENABLE, value );
	
	_RenderState__Change( RS_ZTEST, value );
}

////////////////////////////////////////////////////////////////////////////

void _RenderState__PolygonMode( int value )
{
	ASSERT( value >= 0 && value < sizeof ( _RenderState_PolygonMode ) / sizeof ( int ) );
	
	D3DSRS( D3DRS_FILLMODE, _RenderState_PolygonMode[ value ] );
	
	_RenderState__Change( RS_POLYGONMODE, value );
}

////////////////////////////////////////////////////////////////////////////

void _RenderState__FrontFace( int value )
{
	ASSERT( value >= 0 && value < sizeof ( _RenderState_FrontFace ) / sizeof ( int ) );
	
	// N8 - !!! - what do we do for this - there is no notion of different windings in DirectX.

	RenderState__Force( RS_CULL );
	
	_RenderState__Change( RS_FRONTFACE, value );
}

////////////////////////////////////////////////////////////////////////////

void _RenderState__Cull( int value )
{
	ASSERT( value >= 0 && value < sizeof ( _RenderState_Cull ) / sizeof ( int ) );
	
	if( value == RS_CULL_NONE )
		D3DSRS( D3DRS_CULLMODE, D3DCULL_NONE );
	else
	{
		unsigned int rs = RenderState__Get( RS_FRONTFACE );
		unsigned int value = _RenderState_FrontFace[ rs ];
		D3DSRS( D3DRS_CULLMODE, value );
	}

	
	_RenderState__Change( RS_CULL, value );
}

////////////////////////////////////////////////////////////////////////////

void _RenderState__BaseColor( int value )
{
	//glColor4ubv( ( GLubyte*)&value );
	// DXTODO: can a current color be set in DirectX?
	// N8 sez: Not really, but the DrawArray and DrawList subsystems handle 
	// it pretty well by querying the value set here and shoving that into 
	// the current vertex.
	// Ah, but the value as a color is ABGR, we need to send DirectX an RGBA,
	// so swap this sucka around.
	unsigned int  c = value;
	unsigned char a = ( c >> 24 ) & 0xFF;
	unsigned char b = ( c >> 16 ) & 0xFF;
	unsigned char g = ( c >>  8 ) & 0xFF;
	unsigned char r = ( c >>  0 ) & 0xFF;
	value = D3DCOLOR_RGBA( r, g, b, a );
	
	_RenderState__Change( RS_BASECOLOR, value );
}

////////////////////////////////////////////////////////////////////////////

void _RenderState__FarClipping( int value )
{
	float fFarClip;
	
	ASSERT( value == RS_FALSE || value == RS_TRUE );
	
	/* only modify projection matrix if we are currently rendering a scene */

	CScene *pScene = SceneMgr::GetCurrentScene();
	if (pScene)
	{
		if ( value )
		{
			fFarClip = pScene->d_sceneView.GetFarClip();
		}
		else
		{
			// Disable by setting the far clip WAY out there.
			// N8: 10k should be enough, and it is much easier on the Z buffer than
			// MAXFLOAT32 (which is what it used to be), especially on Xbox.
			fFarClip = 10000.0f;
		}

		MatrixStack__Mode( MS_PROJECTION );
		MatrixStack__Identity();
		MatrixStack__Perspective(
			pScene->d_sceneView.GetFieldOfViewY(),
			pScene->d_sceneView.GetAspectRatio(),
			pScene->d_sceneView.GetNearClip(),
			fFarClip);

		/* only update value if we changed projection matrix */

		_RenderState__Change( RS_FAR_CLIPPING, value );
	}
}

////////////////////////////////////////////////////////////////////////////

void _RenderState__Fog( int value )
{
	ASSERT( value == RS_TRUE || value == RS_FALSE );

	D3DSRS( D3DRS_FOGENABLE, value );

	_RenderState__Change( RS_FOG, value );
}

////////////////////////////////////////////////////////////////////////////

void _RenderState__FogMode( int value )
{
	ASSERT( value >= 0 && value < sizeof ( _RenderState_FogMode ) / sizeof ( int ) );

#ifdef _XBOX
	D3DSRS( D3DRS_FOGTABLEMODE, _RenderState_FogMode[ value ] );
#else
	D3DSRS( D3DRS_FOGVERTEXMODE, _RenderState_FogMode[ value ] );
#endif

	_RenderState__Change( RS_FOGMODE, value );
}

////////////////////////////////////////////////////////////////////////////

void _RenderState__FogStart( int value )
{
	// DirectX uses a left-handed coordinate system, so the fog start/end 
	// must be negated. 
#if defined _XBOX || defined DIRECTX_PC
	float start = RenderState__itof( value ) * -1.0f;
#else
	float start = RenderState__itof( value );
#endif
	
	D3DSRS( D3DRS_FOGSTART, *(DWORD *) &start );
	
	_RenderState__Change( RS_FOGSTART, value );
}

////////////////////////////////////////////////////////////////////////////

void _RenderState__FogEnd( int value )
{
	// DirectX uses a left-handed coordinate system, so the fog start/end 
	// must be negated. 
#ifdef _XBOX
	float end = RenderState__itof( value ) * -1.0f;
#else
	float end = RenderState__itof( value );
#endif

	D3DSRS( D3DRS_FOGEND, *(DWORD *) &end );
	
	_RenderState__Change( RS_FOGEND, value );
}

////////////////////////////////////////////////////////////////////////////

void _RenderState__FogColor( int value )
{
	unsigned char color[ 4 ];
	
	RenderState__Unpackubv( color, value );
	
	D3DSRS( D3DRS_FOGCOLOR, D3DCOLOR_RGBA( color[0], color[1], color[2], 255 )  );
	
	_RenderState__Change( RS_FOGCOLOR, value );
}

////////////////////////////////////////////////////////////////////////////

void _RenderState__FogDensity( int value )
{
	D3DSRS( D3DRS_FOGDENSITY, value );
	
	_RenderState__Change( RS_FOGDENSITY, value );
}

////////////////////////////////////////////////////////////////////////////

void _RenderState__AlphaTest( int value )
{
	ASSERT( value == RS_TRUE || value == RS_FALSE );
	
	D3DSRS( D3DRS_ALPHATESTENABLE, value );
	
	_RenderState__Change( RS_ALPHATEST, value );
}

////////////////////////////////////////////////////////////////////////////

void _RenderState__AlphaFunc( int value )
{
	ASSERT( value >= 0 && value < sizeof ( _RenderState_AlphaFunc ) / sizeof ( int ) );
	
	D3DSRS( D3DRS_ALPHAFUNC, _RenderState_AlphaFunc[ value ] );
	
	_RenderState__Change( RS_ALPHAFUNC, value );
}

////////////////////////////////////////////////////////////////////////////

void _RenderState__AlphaRef( int value )
{
	ASSERT( value >= 0 && value < 256 );
	
	D3DSRS( D3DRS_ALPHAREF, value );
	
	_RenderState__Change( RS_ALPHAREF, value );
}

////////////////////////////////////////////////////////////////////////////

void _RenderState__Lighting( int value )
{
	ASSERT( value == RS_TRUE || value == RS_FALSE );
	
	// If the render manager says no lights, disable them regardless.
	if ( ! RenderMgr__LightingOn() )
	{
		value = RS_FALSE;
	}
	
	D3DSRS( D3DRS_LIGHTING, value );
	
	_RenderState__Change( RS_LIGHTING, value );
}

////////////////////////////////////////////////////////////////////////////

void _RenderState__ZWrite( int value )
{
	ASSERT( value == RS_TRUE || value == RS_FALSE );
	
	D3DSRS( D3DRS_ZWRITEENABLE, value );
	
	_RenderState__Change( RS_ZWRITE, value );
}

////////////////////////////////////////////////////////////////////////////

void _RenderState__ZBias( int value )
{
	// DirectX's implementation of ZBIAS has 16 levels (0-16) where 16 is 
	// topmost, 0 is bottommost. So, when the ZBias is > 0 just set the 
	// bias at 16. 
	
	if ( RenderState__itof( value ) > 0 )
	{
		D3DSRS( D3DRS_ZBIAS, 16 );
	}
	else
	{
		D3DSRS( D3DRS_ZBIAS, 0 );
	}
	
	_RenderState__Change( RS_ZBIAS, value );
}

////////////////////////////////////////////////////////////////////////////

void _RenderState__ZDist( int value )
{
	// DirectX doesn't require a zdistance for Z biasing, so this is a NOP.
	
	_RenderState__Change( RS_ZDIST, value );
}

////////////////////////////////////////////////////////////////////////////

void _RenderState__TextureBrightness( int value )
{
	/* DXTODO - !!! - how do we do this in DirectX??
	float brightness = RenderState__itof( value );

	ASSERT( ( brightness == 1.0f ) || ( brightness == 2.0f ) || ( brightness == 4.0f ) );
  
	// only ever want texture unit 0 to be brightened??? we never want to 
	// brighten a light map or a diffusion map, since it will be modulated 
	// into the incoming texture fragment. for decals, when we're lighting 
	// we're screwed since the decal won't be modulated-- it's opaque 
	// pixels need to replace the incoming texture fragment 
	#ifdef WIN32
	{
	extern void _TextureState__Unit( int value );

	_TextureState__Unit( 1 ); //glActiveTextureARB( GL_TEXTURE1_ARB );
	glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT );
	//djr: should second texture unit get brightness???				glTexEnvf(GL_TEXTURE_ENV, GL_RGB_SCALE_EXT, fRenderStateTextureBrightness);
	glTexEnvf( GL_TEXTURE_ENV, GL_RGB_SCALE_EXT, 1.0f );
	_TextureState__Unit( 0 ); //glActiveTextureARB( GL_TEXTURE0_ARB );
	glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);
	glTexEnvf( GL_TEXTURE_ENV, GL_RGB_SCALE_EXT, brightness );
	}
	#endif
	*/
	
	_RenderState__Change( RS_TEXTURE_BRIGHTNESS, value );
}

////////////////////////////////////////////////////////////////////////////

void _RenderState__TextureConstantAlpha( int value )
{
	_RenderState__Change( RS_TEXTURE_CONSTANTALPHA, value );
}
	
////////////////////////////////////////////////////////////////////////////
	
void _RenderState__ObjectConstantAlpha( int value )
{
	_RenderState__Change( RS_OBJECT_CONSTANTALPHA, value );
}

////////////////////////////////////////////////////////////////////////////

void _RenderState__EnvMapping( int value )
{
	ASSERT( value == RS_TRUE || value == RS_FALSE );
	
	/* DXTODO - !!! - how do we do this in DirectX?? */
	
	_RenderState__Change( RS_ENV_MAPPING, value );
}

////////////////////////////////////////////////////////////////////////////

void _RenderState__OverrideColor( int value )
{
	ASSERT( value == RS_TRUE || value == RS_FALSE );
	
	/* DXTODO - !!! - how do we do this in DirectX?? */
	
	_RenderState__Change( RS_OVERRIDE_COLOR, value );
}

////////////////////////////////////////////////////////////////////////////

void _RenderState__Skinning( int value )
{
	ASSERT( value == RS_TRUE || value == RS_FALSE );


	_RenderState__Change( RS_SKINNING, value );
}

////////////////////////////////////////////////////////////////////////////
void _RenderState__SetTFactor( float X, float Y, float Z, float W )
{
    D3DCOLOR r = (D3DCOLOR)( ( X + 1.0f ) * 127.5f );
    D3DCOLOR g = (D3DCOLOR)( ( Y + 1.0f ) * 127.5f );
    D3DCOLOR b = (D3DCOLOR)( ( Z + 1.0f ) * 127.5f );
    D3DCOLOR a = (D3DCOLOR)( 255.0f * W );
    D3DCOLOR tfactor = D3DCOLOR_ARGB( a, r, g, b );

	D3DSRS( D3DRS_TEXTUREFACTOR, tfactor );
}