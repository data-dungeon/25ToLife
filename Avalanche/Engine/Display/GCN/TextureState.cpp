///////////////////////////////////////////////////////////////////////////
//
//  TextureState.c
//
//  Texture State Management Module.
//
//  Todd Blackburn, May, 2001.
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
//  Globals
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

int _TextureState_Type[] =
{
    GX_FALSE, //DISABLED
    GX_TRUE,
};

////////////////////////////////////////////////////////////////////////////

int _TextureState_Wrap[] =
{
    GX_CLAMP,
    GX_REPEAT,
};


////////////////////////////////////////////////////////////////////////////
//  Functions
////////////////////////////////////////////////////////////////////////////

void _TextureState__Unit( int value )
{
	
	ASSERT( value >= 0 && value < TextureState__GetNumUnits() );

	_TextureState_ActiveUnit = value;
}

////////////////////////////////////////////////////////////////////////////

void _TextureState__Type( int unit, int value)
{
	ASSERT( value == TS_TYPE_2D || value == TS_TYPE_NONE );

	_TextureState__Change( unit, TS_TYPE, value );
}

////////////////////////////////////////////////////////////////////////////

void _TextureState__Bind( int unit, int value )
{

	int type;

	ASSERT( unit == _TextureState_ActiveUnit );
	ASSERT( value < 10000 );

	type = TextureState__Get( unit, TS_TYPE ); 

	_TextureState__Change( unit, TS_BIND, value );
}

////////////////////////////////////////////////////////////////////////////

void _TextureState__Method( int unit, int value )
{
	ASSERT(	unit == _TextureState_ActiveUnit );
	ASSERT(	value == TS_METHOD_PASSTHRU      ||
				value == TS_METHOD_REPLACE       ||
				value == TS_METHOD_INTERPOLATE   ||
				value == TS_METHOD_CONSTANTALPHA ||
				value == TS_METHOD_MODULATE      || 
				value == TS_METHOD_DECAL          );

	_TextureState__Change( unit, TS_METHOD, value );
}

////////////////////////////////////////////////////////////////////////////

void _TextureState__WrapU( int unit, int value )
{
	ASSERT(	unit == _TextureState_ActiveUnit );
	ASSERT(	value == TS_WRAP_CLAMP || 
			   value == TS_WRAP_REPEAT );

	_TextureState__Change( unit, TS_WRAPU, value );
}

////////////////////////////////////////////////////////////////////////////

void _TextureState__WrapV( int unit, int value )
{
	ASSERT(	unit == _TextureState_ActiveUnit );
	ASSERT(	value == TS_WRAP_CLAMP || 
				value == TS_WRAP_REPEAT );

	_TextureState__Change( unit, TS_WRAPV, value );
}

////////////////////////////////////////////////////////////////////////////

void _TextureState__Filter( int unit, int value )
{
	ASSERT(	unit == _TextureState_ActiveUnit );
	ASSERT(	value == TS_FILTER_POINTSAMPLE				|| 
				value == TS_FILTER_BILINEAR					||
				value == TS_FILTER_NEAREST_MIPMAP_NEAREST	||
				value == TS_FILTER_NEAREST_MIPMAP_LINEAR	||
				value == TS_FILTER_LINEAR_MIPMAP_NEAREST	||
				value == TS_FILTER_LINEAR_MIPMAP_LINEAR	 );

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
	case TS_EFFECT_NONE:
		break;
		
	case TS_EFFECT_ENVMAP:
		break;
		
	default:
		ASSERT( 0 );
		break;
	}
	
	_TextureState__Change( unit, TS_EFFECT, value );
}
