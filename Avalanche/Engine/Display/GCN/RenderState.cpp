///////////////////////////////////////////////////////////////////////////
//
//  RenderState.c
//
//  RenderState Rendering Module.
//
//  Todd Blackburn, May 2001.
//
//  Copyright (c) 2000 Avalanche Software.
//
////////////////////////////////////////////////////////////////////////////

#ifndef GCN
#pragma message( "WARNING: GCN should be defined when compiling this module!" )
#endif


////////////////////////////////////////////////////////////////////////////
//  Includes
////////////////////////////////////////////////////////////////////////////

/******************* includes ****************************/

#include "Display/DisplayPCH.h"





////////////////////////////////////////////////////////////////////////////
//  Defines
////////////////////////////////////////////////////////////////////////////

#define GX_POLYMODE_FILL	0
#define GX_POLYMODE_LINE	1
#define GX_POLYMODE_POINT	2

#define GX_FRONTFACE_CW		0
#define GX_FRONTFACE_CCW	1

#define LARGE_NUMBER 1048576
////////////////////////////////////////////////////////////////////////////
//  Types
////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////
//  Globals
////////////////////////////////////////////////////////////////////////////

int _RenderState_Primitives[] =
{
	GX_POINTS,
	GX_LINES,
	GX_LINESTRIP,
	-1,			//RS_PRIMITIVE_LINELOOP
	GX_TRIANGLES,
	GX_TRIANGLESTRIP,
	GX_TRIANGLEFAN,
	GX_QUADS,
	-1,			//RS_PRIMITIVE_QUADSTRIP
	-1,			//RS_PRIMITIVE_POLYGON
	0, 			//RS_PRIMITIVE_DRAWARRAY
	0,				//RS_PRIMITIVE_DISPLAYLIST
};

////////////////////////////////////////////////////////////////////////////

int _RenderState_ZFunc[] =
{
	GX_NEVER,
	GX_ALWAYS,
	GX_EQUAL,
	GX_NEQUAL,
	GX_LESS,
	GX_LEQUAL,
	GX_GREATER,
	GX_GEQUAL,
};

////////////////////////////////////////////////////////////////////////////

int _RenderState_PolygonMode[] =
{
	GX_POLYMODE_POINT,
	GX_POLYMODE_LINE,
	GX_POLYMODE_FILL,
};

////////////////////////////////////////////////////////////////////////////

int _RenderState_FrontFace[] =
{
	GX_FRONTFACE_CW,
	GX_FRONTFACE_CCW,
};

////////////////////////////////////////////////////////////////////////////

int _RenderState_Cull[] =
{
	GX_CULL_FRONT,
	GX_CULL_BACK,
	GX_CULL_NONE, //NONE,
};

////////////////////////////////////////////////////////////////////////////

int _RenderState_FogMode[] =
{
	GX_FOG_LIN,
	GX_FOG_EXP,
	GX_FOG_EXP2,
	GX_FOG_NONE,
};

////////////////////////////////////////////////////////////////////////////

int _RenderState_AlphaFunc[] =
{
	GX_NEVER,
	GX_ALWAYS,
	GX_EQUAL,
	GX_NEQUAL,
	GX_LESS,
	GX_LEQUAL,
	GX_GREATER,
	GX_GEQUAL,
};

////////////////////////////////////////////////////////////////////////////
//  Functions
////////////////////////////////////////////////////////////////////////////
static void _RenderState__ZMode( void);
static void _RenderState__Culling( void);
static void _RenderState__Fogging( void);
static void _RenderState__AlphaCompare( void);

////////////////////////////////////////////////////////////////////////////

void _RenderState__Begin( int primitive )
{
	ASSERT( 0);		//must use drawarrays

}

////////////////////////////////////////////////////////////////////////////

void _RenderState__End( void )
{
}

// Lighting ////////////////////////////////////////////////////////////////

void RenderState__RegisterHardwareAmbientLight(ts_fRGB Color)
{

	GXColor		gxcolor;

	_RenderState_GlobalAmbient[ 0 ] = Color.R;
	_RenderState_GlobalAmbient[ 1 ] = Color.G;
	_RenderState_GlobalAmbient[ 2 ] = Color.B;
	_RenderState_GlobalAmbient[ 3 ] = 1.0f;

	Color.R *= 1.20f;
	Color.G *= 1.20f;
	Color.B *= 1.20f;
	
	gxcolor.r = (u8)( Math::Min( Color.R, 1.0f) * 255.0f);
	gxcolor.g = (u8)( Math::Min( Color.G, 1.0f) * 255.0f);
	gxcolor.b = (u8)( Math::Min( Color.B, 1.0f) * 255.0f);
	gxcolor.a = 0xff;

	GXSetChanAmbColor( GX_COLOR0A0, gxcolor);
}

////////////////////////////////////////////////////////////////////////////

void RenderState__RegisterHardwareLight( ts_Light* pLight )
{

	static float	ambient[ 4 ] = { 0.0f, 0.0f, 0.0f, 1.0f };

	float				diffuse[ 4 ];
	
	int				index;
	
	if ( _RenderState_NumLights < RS_MAXLIGHTS )
	{
		/* is light on? */
		
		if ((pLight->u32Flags & LIGHT_ON) == 0)
			return;

		/* recognized type? */

		if ((pLight->u32Flags & (LIGHT_DIRECTIONAL | LIGHT_POINT)) == 0)
			return;
		
		/* record light */
		_RenderState_Lights[ _RenderState_NumLights ] = pLight;
		
		/* set up light */

		/* GameCube wants light direction light relative */
//		if( pLight->u32Flags & LIGHT_INFINITE)
//		{
//			pLight->Pos.x = -pLight->Pos.x;
//			pLight->Pos.y = -pLight->Pos.y;
//			pLight->Pos.z = -pLight->Pos.z;
//		}
		/* initial transform into camera space so that CameraPos is valid */
		(*(Vector3 *)&pLight->CameraPos).FastBasisTransform( pLight->Pos, g4WorldToCamera);
//		mat44MultVct3fNoTranslate(	pmWorldToCamera,
//											&pLight->Pos,
//											(ts_Vct3f *)&pLight->CameraPos);
		_RenderState_NumLights++;
	}
}

////////////////////////////////////////////////////////////////////////////

void RenderState__RegisterLightSetHardwareLight( u32 index, CLightRecord *pLightRecord, float attenuationScale )
{

	GXLightObj	gxlo;

	bool			bLightColorMultDirty;

	float			fLightColorMult;

	Vector4		v4Color;

	GXColor		gxcolor;

	Vector3		vec3;

	switch( pLightRecord->getType( ) )
	{
		// --- point light
		case CLightRecord::LIGHT_OMNI:
		
			v4Color = ( pLightRecord->getColor( ) * pLightRecord->getIntensity( ));
			v4Color *= 1.10f;
			gxcolor.r = Math::Min( v4Color.x( ), 1.0f) * 255.0f;
			gxcolor.g = Math::Min( v4Color.y( ), 1.0f) * 255.0f;
			gxcolor.b = Math::Min( v4Color.z( ), 1.0f) * 255.0f;
			gxcolor.a = 0xff;

			GXInitLightColor( &gxlo, gxcolor);

			vec3 = pLightRecord->getPosition( ) * g4WorldToCamera;

			GXInitLightPos(	&gxlo,
									vec3.x( ),
									vec3.y( ),
									vec3.z( ));
			GXInitLightAttn(	&gxlo,
									3.0f,
									0.0f,
									0.0f,
									pLightRecord->getConstantAttenuation( ) * attenuationScale,
									pLightRecord->getLinearAttenuation( ) * attenuationScale,
									pLightRecord->getQuadraticAttenuation( ) * attenuationScale );
			break;

		// --- directional light
		case CLightRecord::LIGHT_FREE_DIRECT:
			bLightColorMultDirty = g_lightSet.getLightColorMult( fLightColorMult );
			v4Color = ( pLightRecord->getColor( ) * pLightRecord->getIntensity( ) * fLightColorMult);
			v4Color *= 1.50f;
			gxcolor.r = Math::Min( v4Color.x( ), 1.0f) * 255.0f;
			gxcolor.g = Math::Min( v4Color.y( ), 1.0f) * 255.0f;
			gxcolor.b = Math::Min( v4Color.z( ), 1.0f) * 255.0f;
			gxcolor.a = 0xff;

			GXInitLightColor( &gxlo, gxcolor);

			vec3.FastBasisTransform( pLightRecord->getDirection( ), g4WorldToCamera);
			vec3 *= LARGE_NUMBER;

			GXInitLightPos(	&gxlo,
									vec3.x( ),
									vec3.y( ),
									vec3.z( ));

			GXInitLightAttn(	&gxlo,
									1.0f,
									0.0f,
									0.0f,
									1.0f,
									0.0f,
									0.0f);
			break;

		default:
			ASSERT( 0 );			// unsupported light type

	}
	
	gxLightIDs |= GX_LIGHT0 << index;

	GXLoadLightObjImm( &gxlo, (GXLightID)( GX_LIGHT0 << index));
}

////////////////////////////////////////////////////////////////////////////

void RenderState__DisableHardwareLightRange( u32 from)
{
	for( int i = from; i < RS_MAXLIGHTS; i++)
	{
		gxLightIDs &= ~( GX_LIGHT0 << i);
	}
}

////////////////////////////////////////////////////////////////////////////

void RenderState__RemoveHardwareLight(ts_Light* pLight)
{
	
	int i;
	int j;

	/* find light in active list, remove */

	j = 0;
	for( i = 0; i < _RenderState_NumLights; i++ )
	{
		if( _RenderState_Lights[i] != pLight)
		{
			/* copy down */
			if( i != j)
			{
				_RenderState_Lights[j] = _RenderState_Lights[i];
			}
			j++;
		}
	}

	/* may not find it (light may be off, and so not in hardware light list, even though it's in light manager list) */
	if( j == _RenderState_NumLights)
	{
		return;	// no lights removed
	}

	/* update light count */
	_RenderState_NumLights = j;

	/* update enable/disable state */
}

////////////////////////////////////////////////////////////////////////////

bool RenderState__HardwareLightsEmpty(void)
{
	return( _RenderState_NumLights == 0);
}

////////////////////////////////////////////////////////////////////////////

void _RenderState__SendHardwareLights( void )
{

	GXLightObj	gxlo;

	Vector3		vec3;

	int i;
	
	bool bLightColorMultDirty;

	float fLightColorMult;

	if( RenderMgr__LightingOn() == FALSE || _RenderState_NumLights == 0)
	{
		return;
	}
	
	bLightColorMultDirty = g_lightSet.getLightColorMult( fLightColorMult );

	if( ( !bLightColorMultDirty) && ( !_RenderState_LightPositionDirty))
	{
		return;
	}

//	fLightColorMult *= 0.80f;

	gxLightIDs = GX_LIGHT_NULL;

	for( i = 0; i < _RenderState_NumLights; i++ )
	{
		ts_Light *pLight = _RenderState_Lights[i];

		if( ( pLight->u32Flags & (LIGHT_POINT | LIGHT_DIRECTIONAL)) == 0)	// all lights in list should be local or infinite directional
		{
			continue;
		}
		if( ( pLight->u32Flags & LIGHT_ON) == 0)
		{
			continue;
		}

		gxLightIDs |= GX_LIGHT0 << i;

		GXColor gxcolor;
		
		gxcolor.r = pLight->Color.R * fLightColorMult * 255.0f;
		gxcolor.g = pLight->Color.G * fLightColorMult * 255.0f;
		gxcolor.b = pLight->Color.B * fLightColorMult * 255.0f;
		gxcolor.a = 0xff;

		GXInitLightColor( &gxlo, gxcolor);

		switch( pLight->u32Flags & LIGHT_LIGHTING_TYPE)
		{
			case LIGHT_DIRECTIONAL:
				vec3.Set( pLight->CameraPos);
				vec3.Normalize( );
				vec3 *= LARGE_NUMBER;
				GXInitLightPos(	&gxlo,
										vec3.x( ),
										vec3.y( ),
										vec3.z( ));

				GXInitLightAttn(	&gxlo,
										1.0f,
										0.0f,
										0.0f,
										1.0f,
										0.0f,
										0.0f);
				break;

			case LIGHT_POINT:
				GXInitLightPos(	&gxlo,
										pLight->CameraPos.x( ),
										pLight->CameraPos.y( ),
										pLight->CameraPos.z( ));
				GXInitLightAttn(	&gxlo,
										1.0f,
										0.0f,
										0.0f,
										1.0f,
										0.0f,
										0.0f);
				GXInitLightDistAttn(	&gxlo,
											pLight->fFalloff,
											0.01,
											GX_DA_MEDIUM);
				break;

			default:
				break;
		}

		GXLoadLightObjImm( &gxlo, (GXLightID)( GX_LIGHT0 << i));
	}

	/* light color multipler is no longer dirty */

	LightMgr__ClearLightColorMultDirty();

	/* light position is no longer dirty */

	_RenderState_LightPositionDirty = FALSE;
}

////////////////////////////////////////////////////////////////////////////

void RenderState__PrepareHardwareLights( void )
{

	int i;
	
	// In OpenGL, light position (direction) is specified in camera space, 
	// so transform from world to camera space 
	
	for (i = 0; i < _RenderState_NumLights; i++)
		(*(Vector3 *)&_RenderState_Lights[ i ]->CameraPos).FastBasisTransform( _RenderState_Lights[ i ]->Pos, g4WorldToCamera);

	/* new positions need to be sent next call to _RenderState__SendHardwareLights */
	_RenderState_LightPositionDirty = TRUE;
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

	GXBlendMode			gxbm;
	
	GXBlendFactor		gxbfSrc = GX_BL_ZERO,
							gxbfDst = GX_BL_ZERO;
							 
	switch ( value )
	{
		case RS_BLENDFUNC_NONE:
			gxbm = GX_BM_NONE;
			break;
			
		case RS_BLENDFUNC_DIFFUSE:
			gxbm = GX_BM_BLEND;
			gxbfSrc = GX_BL_DSTCLR;
			gxbfDst = GX_BL_ZERO;
			break;

		case RS_BLENDFUNC_FIXEDALPHA:	// used for semi-transparent environment mapping on ps2
			ASSERT( 0);	// this is a ps2-specific blend mode!
			break;
			
		case RS_BLENDFUNC_SUBTRACTIVE:	// used for shadow rendering on ps2
			ASSERT(0);	// this is currently only used on ps2, though it is a reasonable mode
			break;
		
		case RS_BLENDFUNC_ADDITIVE:
			// frame texel = src_texel * src_alpha + frame_texel * 1-- for additive particle effects
			gxbm = GX_BM_BLEND;
			gxbfSrc = GX_BL_SRCALPHA;
			gxbfDst = GX_BL_ONE;
			break;

		case RS_BLENDFUNC_NEGATIVE:
			// frame texel = src_texel * (1 - frame_texel);
			gxbm = GX_BM_BLEND;
			gxbfSrc = GX_BL_INVDSTCLR;
			gxbfDst = GX_BL_ZERO;
			break;

		case RS_BLENDFUNC_DEFAULT:
		default:
			gxbm = GX_BM_BLEND;
			gxbfSrc = GX_BL_SRCALPHA;
			gxbfDst = GX_BL_INVSRCALPHA;
			break;
			
	}

	GXSetBlendMode( gxbm, gxbfSrc, gxbfDst, GX_LO_NOOP);
	
	_RenderState__Change( RS_BLENDFUNC, value );
}

////////////////////////////////////////////////////////////////////////////

void _RenderState__ZFunc( int value )
{
	ASSERT( value >= 0 && value < sizeof ( _RenderState_ZFunc ) / sizeof ( int ) );
	
	_RenderState__Change( RS_ZFUNC, value );

	_RenderState__ZMode( );
}

////////////////////////////////////////////////////////////////////////////

void _RenderState__ZTest( int value )
{
	ASSERT( value == RS_TRUE || value == RS_FALSE );
	
	_RenderState__Change( RS_ZTEST, value );

	_RenderState__ZMode( );
}

////////////////////////////////////////////////////////////////////////////

void _RenderState__ZWrite( int value )
{
	ASSERT( value == RS_TRUE || value == RS_FALSE );
	
	_RenderState__Change( RS_ZWRITE, value );

	_RenderState__ZMode( );
}

////////////////////////////////////////////////////////////////////////////

void _RenderState__PolygonMode( int value )
{
	ASSERT( value >= 0 && value < sizeof ( _RenderState_PolygonMode ) / sizeof ( int ) );
	
	_RenderState__Change( RS_POLYGONMODE, value );
}

////////////////////////////////////////////////////////////////////////////

void _RenderState__FrontFace( int value )
{
	ASSERT( value >= 0 && value < sizeof ( _RenderState_FrontFace ) / sizeof ( int ) );
	
	_RenderState__Change( RS_FRONTFACE, value );

	_RenderState__Culling( );
}

////////////////////////////////////////////////////////////////////////////

void _RenderState__Cull( int value )
{
	ASSERT( value >= 0 && value < sizeof ( _RenderState_Cull ) / sizeof ( int ) );
	
	_RenderState__Change( RS_CULL, value );

	_RenderState__Culling( );
}

////////////////////////////////////////////////////////////////////////////

void _RenderState__BaseColor( int value )
{
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
		if( value)
			fFarClip = pScene->d_sceneView.GetFarClip( );
		else
			fFarClip = 10000000.0f;	// Disable by setting the far clip WAY out there.
		MatrixStack__Mode( MS_PROJECTION);
		MatrixStack__Identity( );
		MatrixStack__Perspective(
			pScene->d_sceneView.GetFieldOfViewY(),
			pScene->d_sceneView.GetAspectRatio(),
			pScene->d_sceneView.GetNearClip(),
			fFarClip);
		MatrixStack__Mode( MS_MODELVIEW);

		/* only update value if we changed projection matrix */

		_RenderState__Change( RS_FAR_CLIPPING, value );
	}
}

////////////////////////////////////////////////////////////////////////////

void _RenderState__Fog( int value )
{
	ASSERT( value == RS_TRUE || value == RS_FALSE );
	
	_RenderState__Change( RS_FOG, value );

	_RenderState__Fogging( );
}

////////////////////////////////////////////////////////////////////////////

void _RenderState__FogMode( int value )
{
	ASSERT( value >= 0 && value < sizeof ( _RenderState_FogMode ) / sizeof ( int ) );
	
	_RenderState__Change( RS_FOGMODE, value );

	_RenderState__Fogging( );
}

////////////////////////////////////////////////////////////////////////////

void _RenderState__FogStart( int value )
{
	_RenderState__Change( RS_FOGSTART, value );

	_RenderState__Fogging( );
}

////////////////////////////////////////////////////////////////////////////

void _RenderState__FogEnd( int value )
{
	_RenderState__Change( RS_FOGEND, value );

	_RenderState__Fogging( );
}

////////////////////////////////////////////////////////////////////////////

void _RenderState__FogColor( int value )
{
	_RenderState__Change( RS_FOGCOLOR, value );

	_RenderState__Fogging( );
}

////////////////////////////////////////////////////////////////////////////

void _RenderState__FogDensity( int value )
{
	_RenderState__Change( RS_FOGDENSITY, value );

	_RenderState__Fogging( );
}

////////////////////////////////////////////////////////////////////////////

void _RenderState__AlphaTest( int value )
{
	ASSERT( value == RS_TRUE || value == RS_FALSE );
	
	_RenderState__Change( RS_ALPHATEST, value );

	_RenderState__AlphaCompare( );
}

////////////////////////////////////////////////////////////////////////////

void _RenderState__AlphaFunc( int value )
{
	ASSERT( value >= 0 && value < sizeof ( _RenderState_AlphaFunc ) / sizeof ( int ) );
	
	_RenderState__Change( RS_ALPHAFUNC, value );

	_RenderState__AlphaCompare( );
}

////////////////////////////////////////////////////////////////////////////

void _RenderState__AlphaRef( int value )
{
	ASSERT( value >= 0 && value < 256 );
	
	_RenderState__Change( RS_ALPHAREF, value );

	_RenderState__AlphaCompare( );
}

////////////////////////////////////////////////////////////////////////////

void _RenderState__Lighting( int value )
{
	ASSERT( value == RS_TRUE || value == RS_FALSE );
	
	// If the render manager says no lights, disable them regardless.
	if( !RenderMgr__LightingOn( ))
	{
		value = RS_FALSE;
	}
	
	_RenderState__Change( RS_LIGHTING, value );
}

////////////////////////////////////////////////////////////////////////////

void _RenderState__Skinning( int value )
{
	ASSERT( value == RS_TRUE || value == RS_FALSE );
	
	_RenderState__Change( RS_SKINNING, value );
}

////////////////////////////////////////////////////////////////////////////

void _RenderState__ZBiasZDist( float zbias, float zdist )
{
	//set Z-bias - may be dependent upon ZDist from the camera (depends on implementation of Z-buffering)
	if ( zdist == 0.0f )	//prevent zero-divides
	{
		zdist = MINFLOAT32;
	}
	
//	#ifdef PS2
//	{
//		//glDepthRangePlusOffsetPSX2(0.0, 1.0, fZBias * (200000.0f / (fZCamDist * fZDist)));//for 24bit Z-buffers, just enough to eliminate "Z-fighting" in co-incident LOD models
//		glDepthRangePlusOffsetPSX2( 0.0, 1.0, zbias * ( 3200.0f / ( zdist * zdist ) ) );//for 16bit Z-buffers, just enough to eliminate "Z-fighting" in co-incident LOD models
//	}
//	#else
//	{
//		glPolygonOffset( 1.0f, ( 20.0f / zdist ) * zbias );//for 16bit Z-buffers, just enough to eliminate "Z-fighting" in co-incident LOD models
//	}
//	#endif
}

////////////////////////////////////////////////////////////////////////////

void _RenderState__ZBias( int value )
{
	float zdist = RenderState__itof( RenderState__Get( RS_ZDIST ) );
	float zbias = RenderState__itof( value );

    _RenderState__ZBiasZDist( zbias, zdist );
	
	_RenderState__Change( RS_ZBIAS, value );
}

////////////////////////////////////////////////////////////////////////////

void _RenderState__ZDist( int value )
{
	float zdist = RenderState__itof( value );
	float zbias = RenderState__itof( RenderState__Get( RS_ZBIAS ) );

    _RenderState__ZBiasZDist( zbias, zdist );
	
	_RenderState__Change( RS_ZBIAS, value );
}

////////////////////////////////////////////////////////////////////////////

void _RenderState__TextureBrightness( int value )
{

	float brightness = RenderState__itof( value );
	
	ASSERT( ( brightness == 1.0f ) || ( brightness == 2.0f ) || ( brightness == 4.0f ) );
	
	_RenderState__Change( RS_TEXTURE_BRIGHTNESS, value );
}

////////////////////////////////////////////////////////////////////////////

void _RenderState__TextureConstantAlpha( int value )
{

	GXColor		gxcolor;

	gxcolor.r = 0xff;
	gxcolor.g = 0xff;
	gxcolor.b = 0xff;
	gxcolor.a = (u8)( RenderState__itof( value) * 255.0f);

	GXSetTevKColor( GX_KCOLOR1, gxcolor);
	_RenderState__Change( RS_TEXTURE_CONSTANTALPHA, value );
}

////////////////////////////////////////////////////////////////////////////

void _RenderState__ObjectConstantAlpha( int value )
{

	GXColor		gxcolor;

	gxcolor.r = 0xff;
	gxcolor.g = 0xff;
	gxcolor.b = 0xff;
	gxcolor.a = (u8)( RenderState__itof( value) * 255.0f);

	GXSetTevKColor( GX_KCOLOR0, gxcolor);
	_RenderState__Change( RS_OBJECT_CONSTANTALPHA, value );
}

////////////////////////////////////////////////////////////////////////////

void _RenderState__EnvMapping( int value )
{
	ASSERT( value == RS_TRUE || value == RS_FALSE );
	
	_RenderState__Change( RS_ENV_MAPPING, false && value );
}

////////////////////////////////////////////////////////////////////////////

void _RenderState__OverrideColor( int value )
{
	ASSERT( value == RS_TRUE || value == RS_FALSE );
	
	_RenderState__Change( RS_OVERRIDE_COLOR, value );
}

////////////////////////////////////////////////////////////////////////////

static void _RenderState__ZMode( void)
{

	GXBool		gxbTestEnable,
					gxbWriteEnable;

	GXCompare	gxCompare;

	int			iFunc;

	gxbTestEnable = ( RenderState__Get( RS_ZTEST) == RS_TRUE) ? GX_ENABLE : GX_DISABLE;
	gxbWriteEnable = ( RenderState__Get( RS_ZWRITE) == RS_TRUE) ? GX_ENABLE : GX_DISABLE;

	iFunc = RenderState__Get( RS_ZFUNC);

	if( iFunc >= 0 && iFunc < sizeof(_RenderState_ZFunc) / sizeof(int) )
	{
		gxCompare = (GXCompare)_RenderState_ZFunc[iFunc];
	}
	else
	{
		gxCompare = GX_NEVER;
	}

	GXSetZMode( gxbTestEnable, gxCompare, gxbWriteEnable);
}

static void _RenderState__Culling( void)
{

	GXCullMode		gxCullMode;

	switch( RenderState__Get( RS_CULL))
	{
		case RS_CULL_FRONT:
			if( RenderState__Get( RS_FRONTFACE) == RS_FRONTFACE_CW)
			{
				gxCullMode = GX_CULL_FRONT;
			}
			else
			{
				gxCullMode = GX_CULL_BACK;
			}
			break;
		case RS_CULL_BACK:
			if( RenderState__Get( RS_FRONTFACE) == RS_FRONTFACE_CW)
			{
				gxCullMode = GX_CULL_BACK;
			}
			else
			{
				gxCullMode = GX_CULL_FRONT;
			}
			break;
		case RS_CULL_NONE:
			gxCullMode = GX_CULL_NONE;
			break;
		default:
			gxCullMode = GX_CULL_BACK;
			break;
	}

	g_ShaderMgr.SetCullMode( gxCullMode);
}

static void _RenderState__Fogging( void)
{

	GXFogType		gxfogtype;

	int				iFogType;

	f32				f32Start = 0.0f,
						f32End = 0.0f,
						f32Near = 0.0f,
						f32Far = 0.0f;

	GXColor			gxcolor;

	unsigned char	aucColor[4];

	CScene *pScene = SceneMgr::GetCurrentScene();
	if( pScene == NULL)
	{
		return;
	}

	if( RenderState__Get( RS_FOG) == RS_TRUE)
	{
		iFogType = RenderState__Get( RS_FOGMODE);

		if( iFogType >= 0 && iFogType < sizeof(_RenderState_FogMode) / sizeof(int) )
		{
			gxfogtype = (GXFogType)_RenderState_FogMode[iFogType];
			f32Start  = pScene->d_sceneView.GetNearFog( );
			f32End    = pScene->d_sceneView.GetFarFog( );
			f32Near   = pScene->d_sceneView.GetNearClip( );
			f32Far    = pScene->d_sceneView.GetFarClip( );
		}
		else
		{
			gxfogtype = GX_FOG_NONE;
		}
	}
	else
	{
		gxfogtype = GX_FOG_NONE;
	}

	ts_bRGBA rgbaFog = pScene->d_sceneView.GetFogColor( );

	gxcolor.r = rgbaFog.R;
	gxcolor.g = rgbaFog.G;
	gxcolor.b = rgbaFog.B;
	gxcolor.a = 0xff;

	GXSetFog( gxfogtype, f32Start, f32End, f32Near, f32Far, gxcolor);
}

static void _RenderState__AlphaCompare( void)
{
	
	GXCompare		gxOp0;

	int				iFunc,
						iValue;

	u8					u8Value;

	if( RenderState__Get( RS_ALPHATEST) == RS_TRUE)
	{
		iFunc = RenderState__Get( RS_ALPHAFUNC);
	
		if( iFunc >= 0 && iFunc < sizeof(_RenderState_AlphaFunc) / sizeof(int) )
		{
			gxOp0 = (GXCompare)_RenderState_ZFunc[iFunc];
		}
		else
		{
			gxOp0 = GX_ALWAYS;
		}

		iValue = RenderState__Get( RS_ALPHAREF);

		if( iValue < 0)
		{
			iValue = 0;
		}
		else
		if( iValue > 255)
		{
			iValue = 255;
		}
		u8Value = (u8)iValue;

		GXSetZCompLoc( GX_FALSE);
		GXSetAlphaCompare( gxOp0, u8Value, GX_AOP_AND, GX_ALWAYS, 0);
	}
	else
	{
		GXSetZCompLoc( GX_TRUE);
		GXSetAlphaCompare( GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
	}
}
