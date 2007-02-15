///////////////////////////////////////////////////////////////////////////
//
//  Draw.c
//
//  Draw Rendering Module (immediate mode drawing).
//
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
//  Defines
////////////////////////////////////////////////////////////////////////////

#define DRAW_EXPANSION		( 100 )


////////////////////////////////////////////////////////////////////////////
//  Types
////////////////////////////////////////////////////////////////////////////

struct Draw
{
	float* vectors;
	float* normals;
	float* texcoords;
	float* colors;
};


////////////////////////////////////////////////////////////////////////////
//  Globals
////////////////////////////////////////////////////////////////////////////

int _Draw_Begin         = FALSE;
int _Draw_EnableVector  = FALSE;
int _Draw_EnableNormal  = FALSE;
int _Draw_EnableColor   = FALSE;
int _Draw_EnableTexture = FALSE;
int _Draw_Primitive     = 0;

int _Draw_NumVertices  = 0;
int _Draw_NumAllocated = 0;

float _Draw_Normal[ 3 ]   = { 0, 0, 1 };
float _Draw_TexCoord[ 2 ] = { 0, 0 };
float _Draw_Color[ 4 ]    = { 1, 1, 1, 1 };

Draw _Draw;


////////////////////////////////////////////////////////////////////////////
//  Functions
////////////////////////////////////////////////////////////////////////////

void _Draw__Expand( int expansion )
{
	int n = _Draw_NumVertices + expansion;
	Draw copy = _Draw;
	
	_Draw.vectors   = new float[ n * 4 ]; //( float* )memAlloc( n * 4 * sizeof ( float ) );
	_Draw.normals   = new float[ n * 3 ]; //( float* )memAlloc( n * 3 * sizeof ( float ) );
	_Draw.texcoords = new float[ n * 2 ]; //( float* )memAlloc( n * 2 * sizeof ( float ) );
	_Draw.colors    = new float[ n * 4 ]; //( float* )memAlloc( n * 4 * sizeof ( float ) );
	ASSERT( _Draw.vectors   != NULL );
	ASSERT( _Draw.normals   != NULL );
	ASSERT( _Draw.texcoords != NULL );
	ASSERT( _Draw.colors    != NULL );
	
	if ( copy.vectors )
	{
		memcpy( _Draw.vectors,   copy.vectors,   _Draw_NumVertices * 4 * sizeof ( float ) );
		memcpy( _Draw.normals,   copy.normals,   _Draw_NumVertices * 3 * sizeof ( float ) );
		memcpy( _Draw.texcoords, copy.texcoords, _Draw_NumVertices * 2 * sizeof ( float ) );
		memcpy( _Draw.colors,    copy.colors,    _Draw_NumVertices * 4 * sizeof ( float ) );

		delete copy.vectors;
		delete copy.normals;
		delete copy.texcoords;
		delete copy.colors;
	}
	
	_Draw_NumAllocated = n;
}

////////////////////////////////////////////////////////////////////////////

void Draw__Begin( int primitive )
{
	ASSERT( ! _Draw_Begin );
	ASSERT(	primitive == D_LINES     ||
		primitive == D_LINELOOP  ||
		primitive == D_LINESTRIP ||
		primitive == D_TRIANGLES ||
		primitive == D_TRISTRIP  ||
		primitive == D_QUADS );
	
	_Draw_Begin = TRUE;
	
	_Draw_Primitive = primitive;
}

////////////////////////////////////////////////////////////////////////////

void Draw__End( void )
{
	ASSERT( _Draw_Begin );
	ASSERT( _Draw_EnableVector );
	
	if ( _Draw_Primitive == D_LINELOOP )
	{
		Draw__Vector( _Draw.vectors[ 0 ], _Draw.vectors[ 1 ], _Draw.vectors[ 2 ] );
	}
	
	_Draw_Begin = FALSE;
	
	DrawArray__Enable( DA_VECTOR );
	DrawArray__VectorPointer( 4, DA_FLOAT, 0, _Draw.vectors );
	
	if ( _Draw_EnableNormal )
	{
		DrawArray__Enable( DA_NORMAL );
		DrawArray__NormalPointer( DA_FLOAT, 0, _Draw.normals );
	}
	
	if ( _Draw_EnableColor )
	{
		DrawArray__Enable( DA_COLOR );
		DrawArray__ColorPointer( 4, DA_FLOAT, 0, _Draw.colors );
	} 
	
	if ( _Draw_EnableTexture )
	{
		DrawArray__TextureUnit( 0 );
		DrawArray__Enable( DA_TEXTURE );
		DrawArray__TexturePointer( 2, DA_FLOAT, 0, _Draw.texcoords );
	}
	
	DrawArray__Render( _Draw_Primitive, 0, _Draw_NumVertices );
	
	_Draw_EnableVector  = FALSE;
	_Draw_EnableNormal  = FALSE;
	_Draw_EnableColor   = FALSE;
	_Draw_EnableTexture = FALSE;
	
	if ( _Draw.vectors )
	{
		delete [] _Draw.vectors;
		delete [] _Draw.normals;
		delete [] _Draw.colors;
		delete [] _Draw.texcoords;
	}
	
	_Draw.vectors   = NULL;
	_Draw.normals   = NULL;
	_Draw.colors    = NULL;
	_Draw.texcoords = NULL;
	
	_Draw_NumVertices  = 0;
	_Draw_NumAllocated = 0;

	DrawArray__Disable( DA_VECTOR );
	DrawArray__Disable( DA_NORMAL );
	DrawArray__Disable( DA_COLOR );
	DrawArray__Disable( DA_TEXTURE );
}

////////////////////////////////////////////////////////////////////////////

void Draw__Vector( float x, float y, float z )
{
	ASSERT( _Draw_Begin );
	
	_Draw_EnableVector = TRUE;
	
	if ( _Draw_NumVertices + 1 > _Draw_NumAllocated )
	{
		_Draw__Expand( DRAW_EXPANSION );
	}
	
	_Draw.vectors[ _Draw_NumVertices * 4 + 0 ] = x;
	_Draw.vectors[ _Draw_NumVertices * 4 + 1 ] = y;
	_Draw.vectors[ _Draw_NumVertices * 4 + 2 ] = z;
#ifdef PS2
	_Draw.vectors[ _Draw_NumVertices * 4 + 3 ] = 0.0f;
#else
	_Draw.vectors[ _Draw_NumVertices * 4 + 3 ] = 1.0f;
#endif
	
	_Draw.colors[ _Draw_NumVertices * 4 + 0 ] = _Draw_Color[ 0 ];
	_Draw.colors[ _Draw_NumVertices * 4 + 1 ] = _Draw_Color[ 1 ];
	_Draw.colors[ _Draw_NumVertices * 4 + 2 ] = _Draw_Color[ 2 ];
	_Draw.colors[ _Draw_NumVertices * 4 + 3 ] = _Draw_Color[ 3 ];
	
	_Draw.normals[ _Draw_NumVertices * 3 + 0 ] = _Draw_Normal[ 0 ];
	_Draw.normals[ _Draw_NumVertices * 3 + 1 ] = _Draw_Normal[ 1 ];
	_Draw.normals[ _Draw_NumVertices * 3 + 2 ] = _Draw_Normal[ 2 ];
	
	_Draw.texcoords[ _Draw_NumVertices * 2 + 0 ] = _Draw_TexCoord[ 0 ];
	_Draw.texcoords[ _Draw_NumVertices * 2 + 1 ] = _Draw_TexCoord[ 1 ];
	
	_Draw_NumVertices++;
}

////////////////////////////////////////////////////////////////////////////

void Draw__Normal( float i, float j, float k )
{
	_Draw_EnableNormal = TRUE;
	
	_Draw_Normal[ 0 ] = i;
	_Draw_Normal[ 1 ] = j;
	_Draw_Normal[ 2 ] = k;
}

////////////////////////////////////////////////////////////////////////////

void Draw__Color( float red, float green, float blue, float alpha )
{
	_Draw_EnableColor = TRUE;
	
	_Draw_Color[ 0 ] = red;
	_Draw_Color[ 1 ] = green;
	_Draw_Color[ 2 ] = blue;
	_Draw_Color[ 3 ] = alpha;
}

////////////////////////////////////////////////////////////////////////////

void Draw__TexCoord( float u, float v )
{
	_Draw_EnableTexture = TRUE;
	
	_Draw_TexCoord[ 0 ] = u;
	_Draw_TexCoord[ 1 ] = v;
}

////////////////////////////////////////////////////////////////////////////
