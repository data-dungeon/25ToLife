///////////////////////////////////////////////////////////////////////////
//
//	DrawList.c
//
//	DrawList Rendering Module.
//
//	Nate Robins, October 2001.
//
//	Copyright (c) 2001 Avalanche Software.
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
// DA_TRISTRIP is the only primitive type supported.
//
////////////////////////////////////////////////////////////////////////////

#ifndef DIRECTX
#pragma message( "WARNING: DIRECTX should be defined when compiling this module!" )
#endif


////////////////////////////////////////////////////////////////////////////
//	Includes
////////////////////////////////////////////////////////////////////////////

/******************* includes ****************************/

#include "Display/DisplayPCH.h"





////////////////////////////////////////////////////////////////////////////
//	Defines
////////////////////////////////////////////////////////////////////////////

#define MAX_UNITS	   ( 2 )					// maximum number of texture units.
#define MAX_LISTS		( 100000 )			// maximum number of display lists.


////////////////////////////////////////////////////////////////////////////
//	Types
////////////////////////////////////////////////////////////////////////////

typedef struct
{
	int 	enabled;

	int 	size;
	int 	type;
	int 	stride;

	int 	bytes;	// size * type

	unsigned char*	pointer;

} DrawArray;

////////////////////////////////////////////////////////////////////////////

typedef struct
{
	int	bind;
	int	method;
	int	wrapu;
	int	wrapv;
	int	filter;

	int	blend;
	int	env;
	int	alpha_ref;
	int	effect;

} DrawStatex;

////////////////////////////////////////////////////////////////////////////

typedef struct _DrawIndices
{
	LPDIRECT3DINDEXBUFFER8	ib;
	DrawStatex					state[ 2 ];

	int                     num_indices;
	short*						indices;
	int							byte_indices;

	int							min;
	int							max;

	struct _DrawIndices*		next;

} DrawIndices;

////////////////////////////////////////////////////////////////////////////

typedef struct
{
	LPDIRECT3DVERTEXBUFFER8	vb;

	DWORD							state;

	int                     num_vertices;
	unsigned char*          vertices;
	int							byte_vertices;

	DrawIndices*				indices_head;
	DrawIndices*				indices_tail;

	int							primitive;
	int							size;
	int							type;

} DrawList;


////////////////////////////////////////////////////////////////////////////
//	Globals
////////////////////////////////////////////////////////////////////////////

int _DrawList_TextureUnit = 0;

DrawArray _DrawList_Vector					   = { 0, 0, 0, 0, 0, 0 };
DrawArray _DrawList_Color 					   = { 0, 0, 0, 0, 0, 0 };
DrawArray _DrawList_Normal					   = { 0, 0, 0, 0, 0, 0 };
DrawArray _DrawList_Texture[ MAX_UNITS ]	= 
{ 
	{ 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0 }, 
};

DrawList _DrawLists[ MAX_LISTS ];
int _DrawList_Num = 0;
int _DrawList_Current = 0;

bool _DrawList_SkinSupport = false;


////////////////////////////////////////////////////////////////////////////
//	Functions
////////////////////////////////////////////////////////////////////////////

void DrawList__Initialize( void )
{
	for ( int i = 0; i < MAX_LISTS; i++ )
	{
		DrawList* list = &( _DrawLists[ i ] );
		
		memset( list, 0, sizeof ( DrawList ) );
	}

	// Skin support?
	D3DCAPS8 caps;
	IDirect3DDevice8_GetDeviceCaps( DirectX_Device, &caps );
	if ( caps.MaxVertexBlendMatrices >= 4 )
	{
		_DrawList_SkinSupport = true;
	}
}

////////////////////////////////////////////////////////////////////////////

void DrawList__Terminate( void )
{
	for ( int i = 0; i < MAX_LISTS; i++ )
	{
		DrawList* list = &( _DrawLists[ i ] );
		
		if ( list->vb )
		{
			IDirect3DVertexBuffer8_Release( list->vb );
		}
		
		DrawIndices* indices = list->indices_head;
		while ( indices )
		{
			if ( indices->ib )
			{
				IDirect3DIndexBuffer8_Release( indices->ib );
			}

			DrawIndices* nuke = indices;

			indices = indices->next;

			free( nuke );
		}

		if ( list->state )
		{
			IDirect3DDevice8_DeleteStateBlock( DirectX_Device, list->state );
		}
	}
}

////////////////////////////////////////////////////////////////////////////

bool DrawList__Skinning( void )
{
	return _DrawList_SkinSupport;
}

////////////////////////////////////////////////////////////////////////////

int DrawList__GenList( void )
{
	_DrawList_Num++;

    ASSERT( _DrawList_Num < MAX_LISTS );

	return _DrawList_Num;
}

////////////////////////////////////////////////////////////////////////////

void DrawList__NewList( int list_index )
{
	ASSERT( list_index > 0 && list_index <= _DrawList_Num );
	
	_DrawList_Current = list_index;
	
	DrawList* list = &( _DrawLists[ _DrawList_Current ] );
	
    ASSERT( list->num_vertices == 0 );

	memset( list, 0, sizeof ( DrawList ) );
	
	// Start recording state.
	if( FAILED( IDirect3DDevice8_BeginStateBlock( DirectX_Device ) ) )
	{
		OutputDebugString( "IDirect3DDevice8_BeginStateBlock() failed.\n" );
	}
	
	//TRACE( "DrawList__NewList( %d )\n", list_index );
}

////////////////////////////////////////////////////////////////////////////

void DrawList__EndList( void )
{
	ASSERT( _DrawList_Current != 0 );

	DrawList* list = &( _DrawLists[ _DrawList_Current ] );

	// Stop recording state (but just throw it away, since we've already cached it).
	if( FAILED( IDirect3DDevice8_EndStateBlock( DirectX_Device, &list->state ) ) )
	{
		OutputDebugString( "IDirect3DDevice8_EndStateBlock() failed.\n" );
	}

	// Create a vertex buffer.
	{
		ASSERT( list->vb == NULL );

		if( FAILED( IDirect3DDevice8_CreateVertexBuffer( DirectX_Device, list->byte_vertices, 0, list->type, D3DPOOL_DEFAULT, &list->vb ) ) )
		{
			OutputDebugString( "IDirect3DVertexBuffer8_CreateVertexBuffer() failed.\n" );
			return;
		}

		unsigned char* vertices;

		if( FAILED( IDirect3DVertexBuffer8_Lock( list->vb, 0, list->byte_vertices, &vertices, 0 ) ) )
		{
			OutputDebugString( "IDirect3DVertexBuffer8_Lock() failed.\n" );
			IDirect3DVertexBuffer8_Release( list->vb );
			ASSERT( 0 );
			return;
		}

		memcpy( vertices, list->vertices, list->byte_vertices );

		IDirect3DVertexBuffer8_Unlock( list->vb );
	}

	// Create an index buffer.
	DrawIndices* i = list->indices_head;
	while ( i )
	{
		ASSERT( i->ib == NULL );

		if( FAILED( IDirect3DDevice8_CreateIndexBuffer( DirectX_Device, i->byte_indices, 0, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &i->ib ) ) )
		{
			OutputDebugString( "IDirect3DIndexBuffer8_CreateIndexBuffer() failed.\n" );
			ASSERT( 0 );
			return;
		}

		unsigned char* indices;

		if( FAILED( IDirect3DIndexBuffer8_Lock( i->ib, 0, i->byte_indices, &indices, 0 ) ) )
		{
			OutputDebugString( "IDirect3DIndexBuffer8_Lock() failed.\n" );
			IDirect3DIndexBuffer8_Release( i->ib );
			ASSERT( 0 );
			return;
		}

		memcpy( indices, i->indices, i->byte_indices );

		IDirect3DIndexBuffer8_Unlock( i->ib );

		if ( i->indices )
		{
			free( i->indices );
		}

		i = i->next;
	}

	if ( list->vertices )
	{
		free( list->vertices );
	}

	_DrawList_Current = 0;
}

////////////////////////////////////////////////////////////////////////////

int _DrawList__Sizeof( int type )
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

void DrawList__Enable( int array )
{
	switch ( array )
	{
	case DA_VECTOR:
		_DrawList_Vector.enabled	= 1;
		break;
	case DA_COLOR:
		_DrawList_Color.enabled	= 1;
		break;
	case DA_NORMAL:
		_DrawList_Normal.enabled	= 1;
		break;
	case DA_TEXTURE:
		_DrawList_Texture[ _DrawList_TextureUnit ].enabled = 1;
		break;
	default:
		ASSERT( 0 );
		break;
	}
}

////////////////////////////////////////////////////////////////////////////

void DrawList__Disable( int array )
{
	switch ( array )
	{
	case DA_VECTOR:
		_DrawList_Vector.enabled	= 0;
		break;
	case DA_COLOR:
		_DrawList_Color.enabled	= 0;
		break;
	case DA_NORMAL:
		_DrawList_Normal.enabled	= 0;
		break;
	case DA_TEXTURE:
		_DrawList_Texture[ _DrawList_TextureUnit ].enabled = 0;
		break;
	default:
		ASSERT( 0 );
		break;
	}
}

////////////////////////////////////////////////////////////////////////////

void DrawList__TextureUnit( int unit )
{
	ASSERT( unit >= 0 || unit < MAX_UNITS );

	_DrawList_TextureUnit = unit;
}

////////////////////////////////////////////////////////////////////////////

void DrawList__VectorPointer( int size, int type, int stride, void* pointer )
{
	ASSERT( size == 2 || size == 3 || size == 4 );
	ASSERT( type == DA_FLOAT );
	ASSERT( pointer );

	_DrawList_Vector.size    = size;
	_DrawList_Vector.type    = type;
	_DrawList_Vector.bytes   = ( size * _DrawList__Sizeof( type ) );
	_DrawList_Vector.pointer = ( unsigned char* )pointer;

	if ( stride == 0 )
	{
		_DrawList_Vector.stride = _DrawList_Vector.bytes;
	}
	else
	{
		_DrawList_Vector.stride = stride;
	}
}

////////////////////////////////////////////////////////////////////////////

void DrawList__ColorPointer( int size, int type, int stride, void* pointer )
{
	ASSERT( size >= 3 && size <= 4 );
	ASSERT( type == DA_UBYTE || type == DA_FLOAT );
	ASSERT( pointer );

	_DrawList_Color.size	 = size;
	_DrawList_Color.type	 = type;
	_DrawList_Color.bytes   = ( size * _DrawList__Sizeof( type ) );
	_DrawList_Color.pointer = ( unsigned char* )pointer;

	if ( stride == 0 )
	{
		_DrawList_Color.stride = _DrawList_Color.bytes;
	}
	else
	{
		_DrawList_Color.stride = stride;
	}
}

////////////////////////////////////////////////////////////////////////////

void DrawList__NormalPointer( int type, int stride, void* pointer )
{
	ASSERT( type == DA_FLOAT );
	ASSERT( pointer );

	_DrawList_Normal.size    = 3;
	_DrawList_Normal.type    = type;
	_DrawList_Normal.bytes   = ( 3 * _DrawList__Sizeof( type ) );
	_DrawList_Normal.pointer = ( unsigned char* )pointer;

	if ( stride == 0 )
	{
		_DrawList_Normal.stride = _DrawList_Normal.bytes;
	}
	else
	{
		_DrawList_Normal.stride = stride;
	}
}

////////////////////////////////////////////////////////////////////////////

void DrawList__TexturePointer( int size, int type, int stride, void* pointer )
{
	int unit = _DrawList_TextureUnit;

	// N8 - !!! - currently no support for anything other than 2 texture coords!
	ASSERT( size == 2 );
	ASSERT( type == DA_FLOAT );
	ASSERT( pointer );

	_DrawList_Texture[ unit ].size    = size;
	_DrawList_Texture[ unit ].type    = type;
	_DrawList_Texture[ unit ].bytes   = ( size * _DrawList__Sizeof( type ) );
	_DrawList_Texture[ unit ].pointer = ( unsigned char* )pointer;

	if ( stride == 0 )
	{
		_DrawList_Texture[ unit ].stride = _DrawList_Texture[ unit ].bytes;
	}
	else
	{
		_DrawList_Texture[ unit ].stride = stride;
	}
}

////////////////////////////////////////////////////////////////////////////

void _DrawList__SizeAndType( int* size, int* type )
{
	int s = 0;
	int t = 0;

	if ( _DrawList_Vector.enabled )
	{
		switch ( _DrawList_Vector.size )
		{
		case 2:
			// N8 - !!! - this isn't quite right, but it will work -- 
			// N8 - !!! - how do I send a 2 component vector to D3D?
			// N8 - !!! - can it be done?
			s += sizeof ( D3DXVECTOR3 );
			t |= D3DFVF_XYZ;
			break;

		case 3:
			s += sizeof ( D3DXVECTOR3 );
			if ( _DrawList_SkinSupport )
			{
				t |= D3DFVF_XYZB4;
				s += sizeof ( float ) * 4;
			}
			else
			{
				t |= D3DFVF_XYZ;
			}
			break;

		case 4:
			// N8 - !!! - this isn't quite right, but it will work -- 
			// N8 - !!! - how do I send a 4 component vector to D3D?
			// N8 - !!! - ...do I ever need to?
			s += sizeof ( D3DXVECTOR3 );
			if ( _DrawList_SkinSupport )
			{
				t |= D3DFVF_XYZB4;
				s += sizeof ( float ) * 4;
			}
			else
			{
				t |= D3DFVF_XYZ;
			}
			break;

		default:
			ASSERT( 0 );
			return;
			break;
		}
	}

	if ( _DrawList_Color.enabled )
	{
		s += sizeof ( DWORD );
		t |= D3DFVF_DIFFUSE;
	}
	else
	{
		// N8 - !!! - always include a color in the vertex, because
		// N8 - !!! - we'll just stuff the current color in.
		s += sizeof ( DWORD );
		t |= D3DFVF_DIFFUSE;
	}

	if ( _DrawList_Normal.enabled )
	{
		s += sizeof ( D3DXVECTOR3 );
		t |= D3DFVF_NORMAL;
	}

	{
		int j;
		int n;

		n = 0;
		for ( j = 0; j < MAX_UNITS; j++ )
		{
			if ( _DrawList_Texture[ j ].enabled )
			{
				switch ( _DrawList_Texture[ j ].size )
				{
				case 2:
					s += sizeof ( D3DXVECTOR2 );
					t |= D3DFVF_TEXCOORDSIZE2( n );
					break;

				default:
					// N8 - !!! - currently no support for more than 2 texture coords!
					ASSERT( 0 );
					return;
					break;
				}

				n++;
			}
		}

		t |= n << D3DFVF_TEXCOUNT_SHIFT;
	}

	*size = s;
	*type = t;
}

////////////////////////////////////////////////////////////////////////////

void _DrawList__CopyVertices( unsigned char* vertices, int size, int first, int count )
{
	int i, j;
	unsigned char* v;
	unsigned char* c;
	unsigned char* n;
	unsigned char* t[ MAX_UNITS ];

	// Set pointers to the first index to render in each array.
	v = _DrawList_Vector.pointer + ( first * _DrawList_Vector.stride );
	c = _DrawList_Color.pointer  + ( first * _DrawList_Color.stride );
	n = _DrawList_Normal.pointer + ( first * _DrawList_Normal.stride );
	for ( j = 0; j < MAX_UNITS; j++ )
	{
		t[ j ] = _DrawList_Texture[ j ].pointer  + ( first * _DrawList_Texture[ j ].stride );
	}

	for ( i = 0; i < count; i++ )
	{
		unsigned char* p = &( vertices[ i * size ] );
		
      // Because of the way DirectX expects the vertex data, the order of 
      // these copies is important. The vector must be copied in first, 
      // then any normal, then any color and finally the texture 
      // coordinates. 
		
		// Vector
		if ( _DrawList_Vector.enabled )
		{
			if ( _DrawList_Vector.size == 2 )
			{
				memcpy( p, v, sizeof ( D3DXVECTOR2 ) );
				
				( ( D3DXVECTOR3* )p )->z = 0.0f;
				
				p += sizeof ( D3DXVECTOR3 );
			}
			else if ( _DrawList_Vector.size == 3 && _DrawList_Vector.stride == 3 * sizeof ( float ) )
			{
				memcpy( p, v, sizeof ( D3DXVECTOR3 ) );
				
				p += sizeof ( D3DXVECTOR3 );
			}
			else // if ( _DrawList_Vector.size == 3 && _DrawList_Vector.stride == 4 * sizeof ( float ) )
			{
				memcpy( p, v, sizeof ( D3DXVECTOR3 ) );
				
				p += sizeof ( D3DXVECTOR3 );
				
				if ( _DrawList_SkinSupport )
				{
					float* f = ( float* )p;
					
					Vector4* vector = ( Vector4* )v;
					unsigned int m = ( *( ( int* )&( vector->w ) ) & 0x03 );
					f[ 0 ] = ( m == 0 ) ? 1.0f : 0.0f;
					f[ 1 ] = ( m == 1 ) ? 1.0f : 0.0f;
					f[ 2 ] = ( m == 2 ) ? 1.0f : 0.0f;
					f[ 3 ] = ( m == 3 ) ? 1.0f : 0.0f;

					p += sizeof ( float ) * 4;
				}
			}
		}
		
		// Normal
		if ( _DrawList_Normal.enabled )
		{
			memcpy( p, n, sizeof ( D3DXVECTOR3 ) );

			p += sizeof ( D3DXVECTOR3 );
		}
		
		// Color
		if ( _DrawList_Color.enabled )
		{
			DWORD d;
			float* f = ( float* )c;
			unsigned char r;
			unsigned char g;
			unsigned char b;
			unsigned char a;
			
			if ( _DrawList_Color.type == DA_FLOAT )
			{
				r = ( unsigned char )( f[ 0 ] * 255.0f );
				g = ( unsigned char )( f[ 1 ] * 255.0f );
				b = ( unsigned char )( f[ 2 ] * 255.0f );

				if ( _DrawList_Color.size == 4 )
				{
					a = ( unsigned char )( f[ 3 ] * 255.0f );
				}
				else
				{
					a = 255;
				}
			}
			else
			{
            // Always include a color, because if there isn't an explicit 
            // color array, the current color will be used for all the 
            // vertices (ala OpenGL).
				
				r = c[ 0 ];
				g = c[ 1 ];
				b = c[ 2 ];
				
				if ( _DrawList_Color.size == 4 )
				{
					a = c[ 3 ];
				}
				else
				{
					a = 255;
				}
			}

			d = D3DCOLOR_RGBA( r, g, b, a );

			memcpy( p, &d, sizeof ( DWORD ) );

			p += sizeof ( DWORD );
		}
		else
		{
			DWORD d = RenderState__Get( RS_BASECOLOR );

			memcpy( p, &d, sizeof ( DWORD ) );

			p += sizeof ( DWORD );
		}

		// Texture
		{
			int j;

			for ( j = 0; j < MAX_UNITS; j++ )
			{
				if ( _DrawList_Texture[ j ].enabled )
				{
					memcpy( p, t[ j ], sizeof ( D3DXVECTOR2 ) );

					p += sizeof ( D3DXVECTOR2 );
				}
			}
		}

		// Skip past the data just copied.
		v += _DrawList_Vector.stride;
		c += _DrawList_Color.stride;
		n += _DrawList_Normal.stride;
		for ( j = 0; j < MAX_UNITS; j++ )
		{
			t[ j ] += _DrawList_Texture[ j ].stride;
		}
	}
}

////////////////////////////////////////////////////////////////////////////

void DrawList__Render( int primitive, int first, int count )
{
	ASSERT( primitive == DA_TRISTRIP );

	DrawList* list = &( _DrawLists[ _DrawList_Current ] );

	int size;
	int type;
	_DrawList__SizeAndType( &size, &type );

	int base_vertex;

	// Vertices.
	{
		int            nv;
		int            bv;
		unsigned char* v;
		if ( list->vertices == NULL )
		{
			// No vertices, create some new ones.

			nv = count;
			bv = nv * size;
			v = ( unsigned char* )malloc( bv );

			list->num_vertices = nv;
			list->byte_vertices = bv;
			list->vertices = v;

			base_vertex = 0;
		}
		else
		{
			// Expand the current vertices.

			ASSERT( size == list->size );
			ASSERT( type == list->type );
			ASSERT( primitive == list->primitive );

			nv = list->num_vertices + count;
			bv = nv * size;
			v = ( unsigned char* )malloc( bv );
			memcpy( v, list->vertices, list->byte_vertices );
			free( list->vertices );

			list->vertices = v;
			v = list->vertices + list->byte_vertices;

			base_vertex = list->num_vertices;

			list->num_vertices = nv;
			list->byte_vertices = bv;
		}
		
		_DrawList__CopyVertices( v, size, first, count );
	}
	
	// Indices.
	{
		int    ni;
		int    bi;
		short* i;
		
		DrawStatex state[ 2 ];
		state[ 0 ].bind = TextureState__Get( 0, TS_BIND );
		state[ 0 ].method = TextureState__Get( 0, TS_METHOD );
		state[ 0 ].wrapu = TextureState__Get( 0, TS_WRAPU );
		state[ 0 ].wrapv = TextureState__Get( 0, TS_WRAPV );
		state[ 0 ].filter = TextureState__Get( 0, TS_FILTER );
		state[ 0 ].blend = RenderState__Get( RS_BLENDFUNC );
		state[ 0 ].env = RenderState__Get( RS_ENV_MAPPING );
		state[ 0 ].alpha_ref = RenderState__Get( RS_ALPHAREF );
		state[ 0 ].effect = TextureState__Get( 0, TS_EFFECT );
		state[ 1 ].bind = TextureState__Get( 1, TS_BIND );
		state[ 1 ].method = TextureState__Get( 1, TS_METHOD );
		state[ 1 ].wrapu = TextureState__Get( 1, TS_WRAPU );
		state[ 1 ].wrapv = TextureState__Get( 1, TS_WRAPV );
		state[ 1 ].filter = TextureState__Get( 1, TS_FILTER );
		state[ 1 ].blend = RenderState__Get( RS_BLENDFUNC );
		state[ 1 ].env = RenderState__Get( RS_ENV_MAPPING );
		state[ 1 ].alpha_ref = RenderState__Get( RS_ALPHAREF );
		state[ 1 ].effect = TextureState__Get( 1, TS_EFFECT );
		
		/* change of state? we can test lots of state settings. but this will result in more sub-lists. */
		/* ned added repeat/clamp state to fix big juju issues-- a single model, only one texture, but */
		/* different repeat/clamp settings */

		if (( list->indices_head == NULL ) ||
			 ( state[ 0 ].bind  != list->indices_tail->state[ 0 ].bind  ) ||
			 ( state[ 0 ].wrapu != list->indices_tail->state[ 0 ].wrapu ) ||
			 ( state[ 0 ].wrapv != list->indices_tail->state[ 0 ].wrapv ) ||
			 ( state[ 1 ].bind  != list->indices_tail->state[ 1 ].bind  ) ||
			 ( state[ 1 ].wrapu != list->indices_tail->state[ 1 ].wrapu ) ||
			 ( state[ 1 ].wrapv != list->indices_tail->state[ 1 ].wrapv )
			)
		{
			// Create new indices.

			DrawIndices* indices = ( DrawIndices* )malloc( sizeof ( DrawIndices ) );
			memset( indices, 0, sizeof ( DrawIndices ) );
			
			if ( list->indices_head == NULL )
			{
				list->indices_head = indices;
			}
			else
			{
				list->indices_tail->next = indices;
			}
			list->indices_tail = indices;
			
			ni = ( count - 2 ) * 3;
			bi = ni * sizeof ( short );
			i = ( short* )malloc( bi );
			
			indices->num_indices = ni;
			indices->byte_indices = bi;
			indices->indices = i;

			indices->state[ 0 ] = state[ 0 ];
			indices->state[ 1 ] = state[ 1 ];
		}
		else
		{
			// Expand the current indices.
			DrawIndices* indices = list->indices_tail;
			
			ni = indices->num_indices + ( count - 2 ) * 3;
			bi = ni * sizeof ( short );
			i = ( short* )malloc( bi );
			memcpy( i, indices->indices, indices->byte_indices );
			free( indices->indices );
			
			indices->indices = i;
			i = indices->indices + indices->num_indices;
			
			indices->num_indices = ni;
			indices->byte_indices = bi;
		}
		
		for ( int n = base_vertex, j = 0; n < base_vertex + ( count - 2 ); n++, j++ )
		{
			ASSERT( n + 2 < 32768 );
			
			// Every other triangle is wound opposite in a strip, so when they are 
			// indexed, the winding must be switched.  Switch all odd triangles.
			if ( ( j % 2 ) == 0 )
			{
				i[ 0 ] = n;
				i[ 1 ] = n + 1;
				i[ 2 ] = n + 2;
			}
			else
			{
				i[ 0 ] = n;
				i[ 1 ] = n + 2;
				i[ 2 ] = n + 1;
			}
			
			i += 3;
		}
	}
	
	list->primitive = primitive;
	list->size = size;
	list->type = type;
}

////////////////////////////////////////////////////////////////////////////

void DrawList__CallList( int list_index )
{
	ASSERT( list_index > 0 && list_index <= _DrawList_Num );
	
	DrawList* list = &( _DrawLists[ list_index ] );
	
	// Set state as it was for this list.
	if( FAILED( IDirect3DDevice8_ApplyStateBlock( DirectX_Device, list->state ) ) )
	{
		OutputDebugString( "IDirect3DDevice8_ApplyStateBlock() failed.\n" );
	}
	
	// Render!
	IDirect3DDevice8_SetStreamSource( DirectX_Device, 0, list->vb, list->size );
	IDirect3DDevice8_SetVertexShader( DirectX_Device, list->type );
	
	TextureState__ForceAll();

	DrawIndices* i = list->indices_head;
	while ( i )
	{
		DrawStatex* unit0 = &( i->state[ 0 ] );
		DrawStatex* unit1 = &( i->state[ 1 ] );

		// Set correct texture state for this set of indices.
		if ( unit0->bind == 0 )
		{
			TextureState__RequestAll( 0, TS_TYPE_NONE, 0, 0, 0, 0, 0 );
		}
		else
		{
			TextureState__RequestAll( 0, TS_TYPE_2D, unit0->bind, unit0->method, unit0->wrapu, unit0->wrapv, unit0->filter );
			RenderState__Request( RS_BLENDFUNC, unit0->blend );
			RenderState__Request( RS_ENV_MAPPING, unit0->env );
			RenderState__Request( RS_ALPHAREF, unit0->alpha_ref );
			TextureState__Request( 0, TS_EFFECT, unit0->effect );
		}
		// Set correct texture state for this set of indices.
		if ( unit1->bind == 0 )
		{
			TextureState__RequestAll( 1, TS_TYPE_NONE, 0, 0, 0, 0, 0 );
		}
		else
		{
			TextureState__RequestAll( 1, TS_TYPE_2D, unit1->bind, unit1->method, unit1->wrapu, unit1->wrapv, unit1->filter );
			RenderState__Request( RS_BLENDFUNC, unit1->blend );
			RenderState__Request( RS_ENV_MAPPING, unit1->env );
			RenderState__Request( RS_ALPHAREF, unit1->alpha_ref );
			TextureState__Request( 1, TS_EFFECT, unit1->effect );
		}
		
		IDirect3DDevice8_SetIndices( DirectX_Device, i->ib, 0 );
		
		// Initial tests show that it is faster (13fps vs 15fps) to act as if 
		// all vertices are used (and in our case, they are).  Apparently the 
		// DirectX driver caches which vertices are already transformed in a
		// vertex buffer.
		IDirect3DDevice8_DrawIndexedPrimitive( DirectX_Device, D3DPT_TRIANGLELIST, 0, list->num_vertices, 0, i->num_indices / 3 );
		//IDirect3DDevice8_DrawIndexedPrimitive( DirectX_Device, D3DPT_TRIANGLELIST, i->min, i->max, 0, i->num_indices / 3 );
		
		i = i->next;
	}
}

////////////////////////////////////////////////////////////////////////////
