///////////////////////////////////////////////////////////////////////////
//
//	DrawArray.h
//
//	DrawArray Rendering Module.
//
//	Nate Robins, September 2000.
//
//	Copyright (c) 2000 Avalanche Software.
//
////////////////////////////////////////////////////////////////////////////

#ifndef DRAWARRAY_H
#define DRAWARRAY_H


////////////////////////////////////////////////////////////////////////////
//	Includes
////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif

////////////////////////////////////////////////////////////////////////////
//	Defines
////////////////////////////////////////////////////////////////////////////

enum
{
	DA_VECTOR  = 0,
	DA_COLOR   = 1,
	DA_NORMAL  = 2,
	DA_TEXTURE = 3,
};

enum
{
	DA_UBYTE = 0,
	DA_FLOAT = 1,
	DA_RGB5_A1 = 2,
};

enum
{
	DA_LINES     = 0,
	DA_LINESTRIP = 1,
	DA_LINELOOP  = 2,
	DA_TRIANGLES = 3,
	DA_TRISTRIP  = 4,
	DA_QUADS     = 5,
};


////////////////////////////////////////////////////////////////////////////
//	Types
////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////
//	Macros
////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////
//	Functions
////////////////////////////////////////////////////////////////////////////

#ifdef PS2

/* ps2 does not support the DrawArray interface-- it supports the CTriStrip class instead */

#define DrawArray__Enable(array) {}
#define DrawArray__Disable(array) {}
#define DrawArray__TextureUnit(unit) {}
#define DrawArray__VectorPointer(size, type, stride, pointer) {}
#define DrawArray__ColorPointer(size, type, stride, pointer) {}
#define DrawArray__NormalPointer(type, stride, pointer) {}
#define DrawArray__TexturePointer(size, type, stride, pointer) {}
#define DrawArray__Render(primitive, first, count) {}

#else //PS2

void DrawArray__Initialize( void );
void DrawArray__Terminate( void );
void DrawArray__Enable( int array );
void DrawArray__Disable( int array );
void DrawArray__TextureUnit( int unit );
void DrawArray__VectorPointer( int size, int type, int stride, void* pointer );
void DrawArray__ColorPointer( int size, int type, int stride, void* pointer );
void DrawArray__NormalPointer( int type, int stride, void* pointer );
void DrawArray__TexturePointer( int size, int type, int stride, void* pointer );
void DrawArray__Render( int primitive, int first, int count );

#if defined ( DIRECTX )
void DrawArray__SwapBuffers( void );
#endif

#endif //PS2

////////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
}
#endif

#endif	// DRAWARRAY_H
