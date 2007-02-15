///////////////////////////////////////////////////////////////////////////
//
//	DrawArray.c
//
//	DrawArray Rendering Module.
//
//	Nate Robins, September 2000.
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

#ifndef DIRECTX
#pragma message( "WARNING: DIRECTX should be defined when compiling this module!" )
#endif


////////////////////////////////////////////////////////////////////////////
//	Includes
////////////////////////////////////////////////////////////////////////////

/*** pre-compiled headers through .h file (visual studio), can't be inside #if statement. ***/
/*** see .h file for more info.for codewarrior, pre-compiled header is handled through    ***/
/*** xcl compile/link interpreter, and doesn't need any explicit include                  ***/

#include "Display/DisplayPCH.h"





////////////////////////////////////////////////////////////////////////////
//	Defines
////////////////////////////////////////////////////////////////////////////


#ifdef DIRECTX_PC
#define MAX_VBS		4
#define MAX_BYTES_REAL	( 1024 * 512 )
#define MAX_BYTES		( 1024 * 512 )
#else
#define MAX_VBS		3
#define MAX_BYTES		( 1024 * 196 )		// maximum size (in bytes) of the vertex buffer.
#endif // DIRECTX_PC
#define MAX_UNITS		( 2 )					// maximum number of texture units.


////////////////////////////////////////////////////////////////////////////
//	Types
////////////////////////////////////////////////////////////////////////////

typedef struct _DrawArray
{
	bool 					enabled;

	int 					size;
	int 					type;
	int 					stride;

	int 					bytes;	// size * type

	unsigned char *	pointer;
} DrawArray;

//typedef struct
//{
//	LPDIRECT3DVERTEXBUFFER8	vb;
//	int							primitive;
//	int							count;
//	int							size;
//	int							type;
//
//} DrawArrayCache;

////////////////////////////////////////////////////////////////////////////
//	Globals
////////////////////////////////////////////////////////////////////////////
unsigned int	_DrawArray_VB = 0;
int				_DrawArray_TextureUnit = 0;

DrawArray _DrawArray_Vector					= { 0, 0, 0, 0, 0, 0 };
DrawArray _DrawArray_Color 					= { 0, 0, 0, 0, 0, 0 };
DrawArray _DrawArray_Normal					= { 0, 0, 0, 0, 0, 0 };
DrawArray _DrawArray_Texture[ MAX_UNITS ]	= 
{ 
	{ 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0 }, 
};

//DrawArrayCache _DrawArray_Cache[ MAX_CACHED ];
//int _DrawArray_NumCached = 0;

LPDIRECT3DVERTEXBUFFER8 _DrawArray_VertexBuffers[MAX_VBS] = { 0 };
int							_DrawArray_VertexBufferPos  = 0;


////////////////////////////////////////////////////////////////////////////
//	Functions
////////////////////////////////////////////////////////////////////////////

void DrawArray__Initialize( void )
{
	ASSERT( _DrawArray_VertexBuffers[0] == NULL );

	for( unsigned int i = 0; i < MAX_VBS; i++ )
	{
#ifdef DIRECTX_PC
		if( FAILED( IDirect3DDevice8_CreateVertexBuffer( DirectX_Device, MAX_BYTES_REAL, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 0, D3DPOOL_DEFAULT, &_DrawArray_VertexBuffers[i] ) ) )
#else
		if( FAILED( IDirect3DDevice8_CreateVertexBuffer( DirectX_Device, MAX_BYTES, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 0, D3DPOOL_DEFAULT, &_DrawArray_VertexBuffers[i] ) ) )
#endif
		{
			OutputDebugString( "IDirect3DVertexBuffer8_CreateVertexBuffer() failed.\n" );
			return;
		}
	}

	_DrawArray_VB = 0;
}

////////////////////////////////////////////////////////////////////////////

void DrawArray__Terminate( void )
{
	for( unsigned int i = 0; i < MAX_VBS; i++ )
	{
		if( _DrawArray_VertexBuffers[i] )
		{
			ULONG useCount = IDirect3DVertexBuffer8_Release( _DrawArray_VertexBuffers[i] );
			ASSERT( useCount == 0 );

			_DrawArray_VertexBuffers[i] = NULL;
		}
	}

}

////////////////////////////////////////////////////////////////////////////
void DrawArray__SwapBuffers( void )
{
	if( ++_DrawArray_VB == MAX_VBS )
		_DrawArray_VB = 0;

	_DrawArray_VertexBufferPos = 0;
}

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
		_DrawArray_Vector.enabled	= true;
		break;
	case DA_COLOR:
		_DrawArray_Color.enabled	= true;
		break;
	case DA_NORMAL:
		_DrawArray_Normal.enabled	= true;
		break;
	case DA_TEXTURE:
		_DrawArray_Texture[ _DrawArray_TextureUnit ].enabled = true;
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
		_DrawArray_Vector.enabled	= false;
		break;
	case DA_COLOR:
		_DrawArray_Color.enabled	= false;
		break;
	case DA_NORMAL:
		_DrawArray_Normal.enabled	= false;
		break;
	case DA_TEXTURE:
		_DrawArray_Texture[ _DrawArray_TextureUnit ].enabled = false;
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
	_DrawArray_Vector.bytes   = ( size * _DrawArray__Sizeof( type ) );
	_DrawArray_Vector.pointer = ( unsigned char* )pointer;

	if ( stride == 0 )
	{
		_DrawArray_Vector.stride = _DrawArray_Vector.bytes;
	}
	else
	{
		_DrawArray_Vector.stride = stride;
	}
}

////////////////////////////////////////////////////////////////////////////

void DrawArray__ColorPointer( int size, int type, int stride, void* pointer )
{
	ASSERT( size >= 3 && size <= 4 );
	ASSERT( type == DA_UBYTE || type == DA_FLOAT );
	ASSERT( pointer );

	_DrawArray_Color.size	 = size;
	_DrawArray_Color.type	 = type;
	_DrawArray_Color.bytes   = ( size * _DrawArray__Sizeof( type ) );
	_DrawArray_Color.pointer = ( unsigned char* )pointer;

	if ( stride == 0 )
	{
		_DrawArray_Color.stride = _DrawArray_Color.bytes;
	}
	else
	{
		_DrawArray_Color.stride = stride;
	}
}

////////////////////////////////////////////////////////////////////////////

void DrawArray__NormalPointer( int type, int stride, void* pointer )
{
	ASSERT( type == DA_FLOAT );
	ASSERT( pointer );

	_DrawArray_Normal.size    = 3;
	_DrawArray_Normal.type    = type;
	_DrawArray_Normal.bytes   = ( 3 * _DrawArray__Sizeof( type ) );
	_DrawArray_Normal.pointer = ( unsigned char* )pointer;

	if ( stride == 0 )
	{
		_DrawArray_Normal.stride = _DrawArray_Normal.bytes;
	}
	else
	{
		_DrawArray_Normal.stride = stride;
	}
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
	_DrawArray_Texture[ unit ].bytes   = ( size * _DrawArray__Sizeof( type ) );
	_DrawArray_Texture[ unit ].pointer = ( unsigned char* )pointer;

	if ( stride == 0 )
	{
		_DrawArray_Texture[ unit ].stride = _DrawArray_Texture[ unit ].bytes;
	}
	else
	{
		_DrawArray_Texture[ unit ].stride = stride;
	}
}

////////////////////////////////////////////////////////////////////////////

void _DrawArray__SizeAndType( int* size, int* type )
{
	int s = 0;
	int t = 0;

	if ( _DrawArray_Vector.enabled )
	{
		switch ( _DrawArray_Vector.size )
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
			t |= D3DFVF_XYZ;
			break;

		case 4:
			// N8 - !!! - this isn't quite right, but it will work -- 
			// N8 - !!! - how do I send a 4 component vector to D3D?
			// N8 - !!! - ...do I ever need to?
			s += sizeof ( D3DXVECTOR3 );
			t |= D3DFVF_XYZ;
			break;

		default:
			ASSERT( 0 );
			return;
			break;
		}
	}

	if ( _DrawArray_Color.enabled )
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

	if ( _DrawArray_Normal.enabled )
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
			if ( _DrawArray_Texture[ j ].enabled )
			{
				switch ( _DrawArray_Texture[ j ].size )
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
void _DrawArray__CopyFVFOpt( unsigned char * vertices, int size, int first, int count )
{

	DWORD * pVerts = (DWORD *)		( _DrawArray_Vector.pointer + ( first * _DrawArray_Vector.stride ) );
	char * pColors = (char *)		( _DrawArray_Color.pointer  + ( first * _DrawArray_Color.stride ) );
	DWORD * pTCs = (DWORD *)		(_DrawArray_Texture[0].pointer + ( first * _DrawArray_Texture[0].stride ) );

	DWORD * pOut = (DWORD *)		vertices;

	for( int i = 0; i < count; i++ )
	{
		*pOut++ = *pVerts++;	// Position.x
		*pOut++ = *pVerts++; // Position.y
		*pOut++ = *pVerts++; // Position.z
		
		((char *) pOut)[2] = *pColors++; // Diffuse
		((char *) pOut)[1] = *pColors++; // Diffuse
		((char *) pOut)[0] = *pColors++; // Diffuse
		((char *) pOut)[3] = *pColors++; // Diffuse

		pOut++;

		*pOut++ = *pTCs++;
		*pOut++ = *pTCs++;
	}
}

void _DrawArray__Copy( unsigned char* vertices, int size, int first, int count )
{
	int i, j;
	unsigned char* v;
	unsigned char* c;
	unsigned char* n;
	unsigned char* t[ MAX_UNITS ];

	if( _DrawArray_Vector.enabled  &&  (_DrawArray_Vector.size == 3)  &&
		!_DrawArray_Normal.enabled  &&
		_DrawArray_Color.enabled  &&  (_DrawArray_Color.type != DA_FLOAT)  && (_DrawArray_Color.size == 4)  &&
		_DrawArray_Texture[0].enabled  &&  (_DrawArray_Texture[0].size == 2)  &&  (_DrawArray_Texture[0].size == 2)  &&
		(_DrawArray_Texture[1].enabled == false) )
		return _DrawArray__CopyFVFOpt( vertices, size, first, count );


	// Set pointers to the first index to render in each array.
	v = _DrawArray_Vector.pointer + ( first * _DrawArray_Vector.stride );
	c = _DrawArray_Color.pointer  + ( first * _DrawArray_Color.stride );
	n = _DrawArray_Normal.pointer + ( first * _DrawArray_Normal.stride );

	for ( j = 0; j < MAX_UNITS; j++ )
	{
		t[ j ] = _DrawArray_Texture[ j ].pointer  + ( first * _DrawArray_Texture[ j ].stride );
	}

	for ( i = 0; i < count; i++ )
	{
		unsigned char* p = &( vertices[ i * size ] );

      // Because of the way DirectX expects the vertex data, the order of 
      // these copies is important. The vector must be copied in first, 
      // then any normal, then any color and finally the texture 
      // coordinates. 

		// Vector
		if ( _DrawArray_Vector.enabled )
		{
			if ( _DrawArray_Vector.size == 2 )
			{
				memcpy( p, v, sizeof ( D3DXVECTOR2 ) );

				( ( D3DXVECTOR3* )p )->z = 0;
			}
			else
			{
				memcpy( p, v, sizeof ( D3DXVECTOR3 ) );
			}

			p += sizeof ( D3DXVECTOR3 );
		}

		// Normal
		if ( _DrawArray_Normal.enabled )
		{
			memcpy( p, n, sizeof ( D3DXVECTOR3 ) );

			p += sizeof ( D3DXVECTOR3 );
		}

		// Color
		if ( _DrawArray_Color.enabled )
		{
			DWORD d;
			float* f = ( float* )c;
			unsigned char r;
			unsigned char g;
			unsigned char b;
			unsigned char a;

			if ( _DrawArray_Color.type == DA_FLOAT )
			{
				r = ( unsigned char )( f[ 0 ] * 255.0f );
				g = ( unsigned char )( f[ 1 ] * 255.0f );
				b = ( unsigned char )( f[ 2 ] * 255.0f );

				if ( _DrawArray_Color.size == 4 )
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

				if ( _DrawArray_Color.size == 4 )
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
				if ( _DrawArray_Texture[ j ].enabled )
				{
					memcpy( p, t[ j ], sizeof ( D3DXVECTOR2 ) );

					p += sizeof ( D3DXVECTOR2 );
					ASSERT( ( int )p <= ( int )&( vertices[ i * size ] ) + size );
				}
			}
		}

		// Skip past the data just copied.
		v += _DrawArray_Vector.stride;
		c += _DrawArray_Color.stride;
		n += _DrawArray_Normal.stride;
		for ( j = 0; j < MAX_UNITS; j++ )
		{
			t[ j ] += _DrawArray_Texture[ j ].stride;
		}
	}
}

////////////////////////////////////////////////////////////////////////////
void DrawArray__Render( int primitive, int first, int count )
{
	unsigned char *	vertices = NULL;
	int					first_vertex;
	int					size;
	int					type;
	
	LPDIRECT3DVERTEXBUFFER8	pVB = 0;

	ASSERT( first >= 0 );
	ASSERT( count >= 0 );
	
	if( count == 0 )
		return;

	_DrawArray__SizeAndType( &size, &type );
	
	ASSERT( count * size <= MAX_BYTES );
	HRESULT result = D3D_OK;

	// Round the offset into the vertex buffer to the nearest vertex boundary,
	// because the DrawPrimitive() call doesn't allow us to skip by bytes,
	// just by vertices.
	if ( _DrawArray_VertexBufferPos % size != 0 )
	{
		_DrawArray_VertexBufferPos += size - ( _DrawArray_VertexBufferPos % size );
	}
	

	if ( (_DrawArray_VertexBufferPos + (count * size)) >= MAX_BYTES )
	{
		// No more space in the vertex buffer, so reset it!
		DrawArray__SwapBuffers( );

		pVB = _DrawArray_VertexBuffers[_DrawArray_VB];

		
		// Lock the vertex buffer memory (reset to the beginning).
#ifdef _XBOX
		result = pVB->Lock( _DrawArray_VertexBufferPos, count * size, &vertices, 0 );
#else
		result = pVB->Lock( _DrawArray_VertexBufferPos, count * size, &vertices, D3DLOCK_DISCARD );
#endif
		ASSERT( result == D3D_OK );
	}
	else
	{
		pVB = _DrawArray_VertexBuffers[_DrawArray_VB];

		// Enough space in the vertex buffer, continue filling it.
		// Lock the vertex buffer memory.
#ifdef DIRECTX_PC
		result = pVB->Lock( _DrawArray_VertexBufferPos, count * size, &vertices, D3DLOCK_NOOVERWRITE | D3DLOCK_NOSYSLOCK );		
#else
		result = pVB->Lock( _DrawArray_VertexBufferPos, count * size, &vertices, D3DLOCK_NOOVERWRITE );
#endif
		ASSERT( result == D3D_OK );
	}
	
	// Fill in the vertex buffer memory.
	_DrawArray__Copy( vertices, size, first, count );
	
	// Unlock the vertex buffer memory.
	
	result = pVB->Unlock( );
	ASSERT( result == D3D_OK );
	
	// Render!
	result = DirectX_Device->SetStreamSource( 0, pVB, size );
	ASSERT( result == D3D_OK );

	result = DirectX_Device->SetPixelShader( NULL );
	ASSERT( result == D3D_OK );

	result = DirectX_Device->SetVertexShader( type );
	ASSERT( result == D3D_OK );
	
	RenderState__Request( RS_LIGHTING, RS_FALSE );

	RenderState__Request( RS_FOGMODE, RS_FALSE );

	for( unsigned int unit = 0; unit < MAX_UNITS; unit++ )
	{
		if( _DrawArray_Texture[ unit ].enabled )
		{
			if( MatrixStack__IsIdentity( MS_TEXTURE + unit ) )
			{
				result = DirectX_Device->SetTextureStageState( unit, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
				ASSERT( result == D3D_OK );
			}
			else
			{
				result = DirectX_Device->SetTextureStageState( unit, D3DTSS_TEXTURETRANSFORMFLAGS, _DrawArray_Texture[ unit ].size );
				ASSERT( result == D3D_OK );
			}

			result = DirectX_Device->SetTextureStageState( unit, D3DTSS_TEXCOORDINDEX, unit );
			ASSERT( result == D3D_OK );
		}
		else
		{
			result = DirectX_Device->SetTextureStageState( unit, D3DTSS_COLOROP, D3DTOP_DISABLE );
			ASSERT( result == D3D_OK );
			result = DirectX_Device->SetTextureStageState( unit, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
			ASSERT( result == D3D_OK );

			break;
//			result = DirectX_Device->SetTextureStageState( unit, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
//			ASSERT( result == D3D_OK );

//			result = DirectX_Device->SetTexture( unit, NULL );
//			ASSERT( result == D3D_OK );
		}

	}


	first_vertex = _DrawArray_VertexBufferPos / size;
	
	switch ( primitive )
	{
	case DA_LINES:
		ASSERT( ( count % 2 ) == 0 );
		result = DirectX_Device->DrawPrimitive( D3DPT_LINELIST, first_vertex, count / 2 );
		ASSERT( result == D3D_OK );
		break;

	case DA_LINESTRIP:
		result = DirectX_Device->DrawPrimitive( D3DPT_LINELIST, first_vertex, count - 1 );
		ASSERT( result == D3D_OK );
		break;

	case DA_TRIANGLES:
		ASSERT( ( count % 3 ) == 0 );
		result = DirectX_Device->DrawPrimitive( D3DPT_TRIANGLELIST, first_vertex, count / 3 );
		ASSERT( result == D3D_OK );
		break;

	case DA_TRISTRIP:
		result = DirectX_Device->DrawPrimitive( D3DPT_TRIANGLESTRIP, first_vertex, count - 2 );
		ASSERT( result == D3D_OK );
		break;

	case DA_QUADS:
		ASSERT( ( count % 4 ) == 0 );
		{
#ifdef _XBOX
			result = DirectX_Device->DrawPrimitive( D3DPT_QUADLIST, first_vertex, count / 4 );
			ASSERT( result == D3D_OK );
#else
			int i;
			for ( i = 0; i < count; i += 4 )
			{
				result = DirectX_Device->DrawPrimitive( D3DPT_TRIANGLEFAN, first_vertex + i, 2 );
				ASSERT( result == D3D_OK );
			}
#endif
		}
		break;

	default:
		ASSERT( 0 );
		break;
	}
	
	_DrawArray_VertexBufferPos += count * size;
}

////////////////////////////////////////////////////////////////////////////

//void DrawArray__RenderCached( int cache_index )
//{
//	if ( cache_index == 0 || cache_index > _DrawArray_NumCached )
//	{
//		return;
//	}
//	
//	DrawArrayCache* cache = &( _DrawArray_Cache[ cache_index ] );
//	
//	// Render!
//	IDirect3DDevice8_SetStreamSource( DirectX_Device, 0, cache->vb, cache->size );
//	IDirect3DDevice8_SetVertexShader( DirectX_Device, cache->type );
//	
//	switch ( cache->primitive )
//	{
//	case DA_LINES:
//		ASSERT( ( cache->count % 2 ) == 0 );
//		IDirect3DDevice8_DrawPrimitive( DirectX_Device, D3DPT_LINELIST, 0, cache->count / 2 );
//		break;
//
//	case DA_LINESTRIP:
//		IDirect3DDevice8_DrawPrimitive( DirectX_Device, D3DPT_LINELIST, 0, cache->count - 1 );
//		break;
//
//	case DA_TRIANGLES:
//		ASSERT( ( cache->count % 3 ) == 0 );
//		IDirect3DDevice8_DrawPrimitive( DirectX_Device, D3DPT_TRIANGLELIST, 0, cache->count / 3 );
//		break;
//
//	case DA_TRISTRIP:
//		IDirect3DDevice8_DrawPrimitive( DirectX_Device, D3DPT_TRIANGLESTRIP, 0, cache->count - 2 );
//		break;
//
//	case DA_QUADS:
//		ASSERT( ( cache->count % 4 ) == 0 );
//		{
//			int i;
//
//			for ( i = 0; i < cache->count; i += 4 )
//			{
//				IDirect3DDevice8_DrawPrimitive( DirectX_Device, D3DPT_TRIANGLEFAN, i, 2 );
//			}
//		}
//		break;
//
//	default:
//		ASSERT( 0 );
//		break;
//	}
//}
//
//////////////////////////////////////////////////////////////////////////////
//
//int DrawArray__RenderAndCache( int primitive, int first, int count )
//{
//	ASSERT( first >= 0 );
//	ASSERT( count >= 0 );
//	ASSERT( _DrawArray_NumCached + 1 < MAX_CACHED );
//	
//	// Get a new cache spot.
//	_DrawArray_NumCached++;
//	DrawArrayCache* cache = &( _DrawArray_Cache[ _DrawArray_NumCached ] );
//
//	// Fill out the cache.
//	cache->primitive = primitive;
//	cache->count	  = count;
//	_DrawArray__SizeAndType( &( cache->size ), &( cache->type ) );
//
//	// Create the vertex buffer.
//	if( FAILED( IDirect3DDevice8_CreateVertexBuffer( DirectX_Device, cache->count * cache->size, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 0, D3DPOOL_DEFAULT, &( cache->vb ) ) ) )
//	{
//		OutputDebugString( "IDirect3DVertexBuffer8_CreateVertexBuffer() failed.\n" );
//		return 0;
//	}
//
//	// Lock the vertex buffer memory.
//	unsigned char* vertices = NULL;
//	if( FAILED( IDirect3DVertexBuffer8_Lock( cache->vb, 0, cache->count * cache->size, &vertices, D3DLOCK_DISCARD ) ) )
//	{
//		OutputDebugString( "IDirect3DVertexBuffer8_Lock() failed.\n" );
//		IDirect3DVertexBuffer8_Release( cache->vb );
//		return 0;
//	}
//	
//	// Fill in the vertex buffer memory.
//	_DrawArray__Copy( vertices, cache->size, first, cache->count );
//	
//	// Unlock the vertex buffer memory.
//	IDirect3DVertexBuffer8_Unlock( cache->vb );
//	
//	// Render the cache.
//	DrawArray__RenderCached( _DrawArray_NumCached );
//
//	return _DrawArray_NumCached;
//}

////////////////////////////////////////////////////////////////////////////
