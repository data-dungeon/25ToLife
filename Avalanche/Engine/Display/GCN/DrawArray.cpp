///////////////////////////////////////////////////////////////////////////
//
//	DrawArray.c
//
//	DrawArray Rendering Module.
//
//	Todd Blackburn, June 2001.
//
//	Copyright (c) 2000 Avalanche Software.
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
//
//	Notes
//
//	DA_FLOAT is the only type supported, except for ColorPointers, 
//	which can also be DA_UBYTE.
//
//	Texture coordinates can only have two components.
//
//	Two and Four component vectors are treated just like three component 
//	vectors (two component vectors Z == 0).
//
////////////////////////////////////////////////////////////////////////////

#ifndef GCN
#pragma message( "WARNING: GCN should be defined when compiling this module!" )
#endif


////////////////////////////////////////////////////////////////////////////
//	Includes
////////////////////////////////////////////////////////////////////////////

#include "Display/DisplayPCH.h"





////////////////////////////////////////////////////////////////////////////
//	Defines
////////////////////////////////////////////////////////////////////////////

#define MAX_UNITS	( 2 )

#define ATTR_MASK_CLR		0x01
#define ATTR_MASK_NRM		0x02
#define ATTR_MASK_TEX0		0x04
#define ATTR_MASK_TEX1		0x08

////////////////////////////////////////////////////////////////////////////
//	Types
////////////////////////////////////////////////////////////////////////////

typedef struct
{
	int 					enabled;

	int 					size;
	int 					type;
	int 					stride;

	int 					bytes;	// size * type
	u8 					skip;		// bytes + stride

	void					*pointer;

} DrawArray;


////////////////////////////////////////////////////////////////////////////
//	Globals
////////////////////////////////////////////////////////////////////////////

int _DrawArray_TextureUnit = 0;

DrawArray _DrawArray_Vector					= { 0, 0, 0, 0, 0, 0, 0 };
DrawArray _DrawArray_Color 					= { 0, 0, 0, 0, 0, 0, 0 };
DrawArray _DrawArray_Normal					= { 0, 0, 0, 0, 0, 0, 0 };
DrawArray _DrawArray_Texture[ MAX_UNITS ]	= 
{ 
	{ 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0 }, 
};

GXPrimitive _DrawArray_Primitives[] =
{
	GX_LINES,						// DA_LINES
	GX_LINESTRIP, 					// DA_LINESTRIP
	GX_LINESTRIP,	 				// DA_LINELOOP
	GX_TRIANGLES,					// DA_TRIANGLES
	GX_TRIANGLESTRIP,				// DA_TRISTRIP
	GX_QUADS,						// DA_QUADS
};

static int gxMtxID[] =
{
	GX_PNMTX0,
	GX_PNMTX1,
	GX_PNMTX2,
	GX_PNMTX3
};
////////////////////////////////////////////////////////////////////////////
//	Macros
////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////
//	Functions
////////////////////////////////////////////////////////////////////////////

int _DrawArray__Sizeof( int type )
{
	switch ( type )
	{
		case DA_FLOAT:
			return 4;
			break;

		case DA_UBYTE:
			return 1;
			break;

		default:
			ASSERT( 0 );
			return 0;
			break;
	}
}

////////////////////////////////////////////////////////////////////////////

void DrawArray__Enable( int array )
{
	switch ( array )
	{
		case DA_VECTOR:
			_DrawArray_Vector.enabled	= 1;
			break;
		case DA_COLOR:
			_DrawArray_Color.enabled	= 1;
			break;
		case DA_NORMAL:
			_DrawArray_Normal.enabled	= 1;
			break;
		case DA_TEXTURE:
			_DrawArray_Texture[ _DrawArray_TextureUnit ].enabled = 1;
			break;
		default:
			ASSERT( 0 );
			break;
	}
}

////////////////////////////////////////////////////////////////////////////

void DrawArray__Disable( int array )
{
	switch ( array )
	{
		case DA_VECTOR:
			_DrawArray_Vector.enabled	= 0;
			break;
		case DA_COLOR:
			_DrawArray_Color.enabled	= 0;
			break;
		case DA_NORMAL:
			_DrawArray_Normal.enabled	= 0;
			break;
		case DA_TEXTURE:
			_DrawArray_Texture[ _DrawArray_TextureUnit ].enabled = 0;
			break;
		default:
			ASSERT( 0 );
			break;
	}
}

////////////////////////////////////////////////////////////////////////////

void DrawArray__TextureUnit( int unit )
{
	ASSERT( unit >= 0 || unit < MAX_UNITS );

	_DrawArray_TextureUnit = unit;
}

////////////////////////////////////////////////////////////////////////////

void DrawArray__VectorPointer( int size, int type, int stride, void* pointer )
{
	ASSERT( size == 2 || size == 3 || size == 4 );
	ASSERT( type == DA_FLOAT );
	ASSERT( pointer );

	_DrawArray_Vector.size    = size;
	_DrawArray_Vector.type    = type;
	_DrawArray_Vector.stride  = stride;
	_DrawArray_Vector.pointer = (unsigned char *)pointer;

	_DrawArray_Vector.bytes = (size * _DrawArray__Sizeof( type));
	_DrawArray_Vector.skip  = _DrawArray_Vector.bytes + stride;
	GXSetArray( GX_VA_POS, _DrawArray_Vector.pointer, _DrawArray_Vector.skip);
}

////////////////////////////////////////////////////////////////////////////

void DrawArray__ColorPointer( int size, int type, int stride, void* pointer )
{
	ASSERT( size >= 3 && size <= 4 );
	ASSERT( type == DA_UBYTE || type == DA_FLOAT );
	ASSERT( pointer );

	_DrawArray_Color.size	 = size;
	_DrawArray_Color.type	 = type;
	_DrawArray_Color.stride  = stride;
	_DrawArray_Color.pointer = ( unsigned char* )pointer;

	_DrawArray_Color.bytes = ( size * _DrawArray__Sizeof( type ) );
	_DrawArray_Color.skip  = _DrawArray_Color.bytes + stride;
	GXSetArray( GX_VA_CLR0, _DrawArray_Color.pointer, _DrawArray_Color.skip);
}

////////////////////////////////////////////////////////////////////////////

void DrawArray__NormalPointer( int type, int stride, void* pointer )
{
	ASSERT( type == DA_FLOAT );
	ASSERT( pointer );

	_DrawArray_Normal.size    = 3;
	_DrawArray_Normal.type    = type;
	_DrawArray_Normal.stride  = stride;
	_DrawArray_Normal.pointer = ( unsigned char* )pointer;

	_DrawArray_Normal.bytes = ( 3 * _DrawArray__Sizeof( type ) );
	_DrawArray_Normal.skip  = _DrawArray_Normal.bytes + stride;
	GXSetArray( GX_VA_NRM, _DrawArray_Normal.pointer, _DrawArray_Normal.skip);
}

////////////////////////////////////////////////////////////////////////////

void DrawArray__TexturePointer( int size, int type, int stride, void* pointer )
{
	int unit = _DrawArray_TextureUnit;

	// N8 - !!! - currently no support for anything other than 2 texture coords!
	ASSERT( size == 2 );
	ASSERT( type == DA_FLOAT );
	ASSERT( pointer );

	_DrawArray_Texture[ unit ].size    = size;
	_DrawArray_Texture[ unit ].type    = type;
	_DrawArray_Texture[ unit ].stride  = stride;
	_DrawArray_Texture[ unit ].pointer = ( unsigned char* )pointer;

	_DrawArray_Texture[ unit ].bytes = ( size * _DrawArray__Sizeof( type ) );
	_DrawArray_Texture[ unit ].skip  = _DrawArray_Texture[ unit ].bytes + stride;
	GXSetArray(	(GXAttr)( GX_VA_TEX0 + unit), _DrawArray_Texture[unit].pointer, _DrawArray_Texture[unit].skip);
}

////////////////////////////////////////////////////////////////////////////

void DrawArray__Render( int primitive, int first, int count )
{

	GXBool						gxbLightsOn;
									
	GXDiffuseFn					gxdfn;
									
	ts_FullTextureContext	texContext;

	ShaderInst					*pShader;

	Vector4						*pVertex;
									
	int							iUnit,
									iBind,
									iTexMap,
									iAttrMask = 0,
									iPoint;

	texContext.TextureCount = 0;
	for( iUnit = 0; iUnit < MAX_UNITS; iUnit++)
	{
		
		int iTextureType = TextureState__Get( iUnit, TS_TYPE);

		if( _DrawArray_Texture[iUnit].enabled && iTextureType == TS_TYPE_2D)
		{
			texContext.TextureCount++;
			texContext.TextureLayer[iUnit].s16TextureBind = TextureState__Get( iUnit, TS_BIND);
			texContext.TextureLayer[iUnit].u8Method = TextureState__Get( iUnit, TS_METHOD);
			texContext.TextureLayer[iUnit].u8Flags = 0;
		}
	}

	pShader = g_ShaderMgr.GetShader( (ts_TextureContext *)&texContext, false);
	RenderState__Request( RS_CULL, RS_CULL_NONE);

	//First, lets set up our vertex data
	//we always have arrays for each component
	GXClearVtxDesc( );

	//we always have position info
	GXSetVtxDesc( GX_VA_POS, GX_INDEX16);
	pVertex = ( (Vector4 *)_DrawArray_Vector.pointer) + first;

	u32	channelFlags = 0;

	if(	RenderState__Get( RS_LIGHTING) == RS_TRUE &&
			gxLightIDs != GX_LIGHT_NULL &&
			_DrawArray_Normal.enabled)
	{
		channelFlags |= ShaderMgr::GCN_CHANNEL_HAS_LIGHTS;
		gxbLightsOn = GX_TRUE;
		gxdfn = GX_DF_CLAMP;
	}
	else
	{
		gxbLightsOn = GX_FALSE;
		gxdfn = GX_DF_NONE;
	}

	GXColor		gxColor;

	gxColor.r = 255;
	gxColor.g = 255;
	gxColor.b = 255;
	gxColor.a = 255;

	if( _DrawArray_Color.enabled)
	{
		iAttrMask |= ATTR_MASK_CLR;
		GXSetVtxDesc( GX_VA_CLR0, GX_INDEX16);
		channelFlags |= ShaderMgr::GCN_CHANNEL_HAS_VTXCOLOR;
	}
	else
	{

		unsigned char	aucColor[4];

		RenderState__Unpackubv( aucColor, RenderState__Get( RS_BASECOLOR));

		gxColor.r = aucColor[0];
		gxColor.g = aucColor[1];
		gxColor.b = aucColor[2];
		gxColor.a = aucColor[3];
	}

	g_ShaderMgr.SetupLightingChannel( channelFlags, gxColor);

	if( _DrawArray_Normal.enabled)
	{
		iAttrMask |= ATTR_MASK_NRM;
		GXSetVtxDesc( GX_VA_NRM, GX_INDEX16);
	}

	for( iUnit = 0; iUnit < MAX_UNITS; iUnit++)
	{
		
		int iTextureType = TextureState__Get( iUnit, TS_TYPE);
		
		if( _DrawArray_Texture[iUnit].enabled && iTextureType == TS_TYPE_2D)
		{
			GXSetVtxDesc( (GXAttr)( GX_VA_TEX0 + iUnit), GX_INDEX16);
			iAttrMask |= ( ATTR_MASK_TEX0 << iUnit);
		}
	}
	
	GXFlush( );

	for( iUnit = 0, iTexMap = 0; iUnit < MAX_UNITS; iUnit++)
	{
		
		int iTextureType = TextureState__Get( iUnit, TS_TYPE);

		if( _DrawArray_Texture[iUnit].enabled && iTextureType == TS_TYPE_2D)
		{
			iBind = TextureState__Get( iUnit, TS_BIND);

			TextureBind__SetWrap( iBind, TextureState__Get( iUnit, TS_WRAPU), TextureState__Get( iUnit, TS_WRAPV));
			TextureBind__SetFilter( iBind, TextureState__Get( iUnit, TS_FILTER));
			
			iTexMap = TextureBind__Load( iBind, iTexMap, (GXTexCoordID)iUnit);
		}
	}

	g_ShaderMgr.UseShader( pShader);

	GXBegin( _DrawArray_Primitives[primitive], GX_VTXFMT0, count);

	switch( iAttrMask)
	{
		case 0:
			for( iPoint = first; iPoint < ( count + first); iPoint++)
			{
				GXPosition1x16( (u16)iPoint);
			}
			break;
		case ATTR_MASK_CLR:
			for( iPoint = first; iPoint < ( count + first); iPoint++)
			{
				GXPosition1x16( (u16)iPoint);
				GXColor1x16( (u16)iPoint);
			}
			break;
		case ATTR_MASK_NRM:
			for( iPoint = first; iPoint < ( count + first); iPoint++)
			{
				GXPosition1x16( (u16)iPoint);
				GXNormal1x16( (u16)iPoint);
			}
			break;
		case ATTR_MASK_NRM | ATTR_MASK_CLR:
			for( iPoint = first; iPoint < ( count + first); iPoint++)
			{
				GXPosition1x16( (u16)iPoint);
				GXNormal1x16( (u16)iPoint);
				GXColor1x16( (u16)iPoint);
			}
			break;
		case ATTR_MASK_TEX0:
			for( iPoint = first; iPoint < ( count + first); iPoint++)
			{
				GXPosition1x16( (u16)iPoint);
				GXTexCoord1x16( (u16)iPoint);
			}
			break;
		case ATTR_MASK_TEX0 | ATTR_MASK_CLR:
			for( iPoint = first; iPoint < ( count + first); iPoint++)
			{
				GXPosition1x16( (u16)iPoint);
				GXColor1x16( (u16)iPoint);
				GXTexCoord1x16( (u16)iPoint);
			}
			break;
		case ATTR_MASK_TEX0 | ATTR_MASK_NRM:
			for( iPoint = first; iPoint < ( count + first); iPoint++)
			{
				GXPosition1x16( (u16)iPoint);
				GXNormal1x16( (u16)iPoint);
				GXTexCoord1x16( (u16)iPoint);
			}
			break;
		case ATTR_MASK_TEX0 | ATTR_MASK_NRM | ATTR_MASK_CLR:
			for( iPoint = first; iPoint < ( count + first); iPoint++)
			{
				GXPosition1x16( (u16)iPoint);
				GXNormal1x16( (u16)iPoint);
				GXColor1x16( (u16)iPoint);
				GXTexCoord1x16( (u16)iPoint);
			}
			break;
		case ATTR_MASK_TEX1 | ATTR_MASK_TEX0:
			for( iPoint = first; iPoint < ( count + first); iPoint++)
			{
				GXPosition1x16( (u16)iPoint);
				GXTexCoord1x16( (u16)iPoint);
				GXTexCoord1x16( (u16)iPoint);
			}
			break;
		case ATTR_MASK_TEX1 | ATTR_MASK_TEX0 | ATTR_MASK_CLR:
			for( iPoint = first; iPoint < ( count + first); iPoint++)
			{
				GXPosition1x16( (u16)iPoint);
				GXColor1x16( (u16)iPoint);
				GXTexCoord1x16( (u16)iPoint);
				GXTexCoord1x16( (u16)iPoint);
			}
			break;
		case ATTR_MASK_TEX1 | ATTR_MASK_TEX0 | ATTR_MASK_NRM:
			for( iPoint = first; iPoint < ( count + first); iPoint++)
			{
				GXPosition1x16( (u16)iPoint);
				GXNormal1x16( (u16)iPoint);
				GXTexCoord1x16( (u16)iPoint);
				GXTexCoord1x16( (u16)iPoint);
			}
			break;
		case ATTR_MASK_TEX1 | ATTR_MASK_TEX0 | ATTR_MASK_NRM | ATTR_MASK_CLR:
			for( iPoint = first; iPoint < ( count + first); iPoint++)
			{
				GXPosition1x16( (u16)iPoint);
				GXNormal1x16( (u16)iPoint);
				GXColor1x16( (u16)iPoint);
				GXTexCoord1x16( (u16)iPoint);
				GXTexCoord1x16( (u16)iPoint);
			}
			break;
	}

	GXEnd( );
	GXFlush( );
}


////////////////////////////////////////////////////////////////////////////
