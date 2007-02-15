////////////////////////////////////////////////////////////////////////////
//
//	d_DirectX.h
//
//	Direct X Rendering Module.
//
//	Nate Robins, September 2000.
//
//	Copyright (c) 2000 Avalanche Software.
//
////////////////////////////////////////////////////////////////////////////

#ifndef D_DIRECTX_H
#define D_DIRECTX_H

#ifdef DIRECTX


////////////////////////////////////////////////////////////////////////////
//	Includes
////////////////////////////////////////////////////////////////////////////

#include "Math/MathUndeprecate.h"
#ifdef _XBOX
#include <xtl.h>
#else
#include <d3d8.h>
#include <d3dx8.h>
#include <dxerr8.h>
#endif
#include "Math/MathDeprecate.h"


////////////////////////////////////////////////////////////////////////////
// Macros
////////////////////////////////////////////////////////////////////////////

#define D3DSRS( state, value )                                             	\
IDirect3DDevice8_SetRenderState( DirectX_Device, state, value )

#define D3DST( stage, texture )	\
IDirect3DDevice8_SetTexture( DirectX_Device, stage, (LPDIRECT3DBASETEXTURE8)texture )

#define D3DSTSS( stage, state, value )	\
IDirect3DDevice8_SetTextureStageState( DirectX_Device, stage, state, value )


////////////////////////////////////////////////////////////////////////////
//	Globals
////////////////////////////////////////////////////////////////////////////

extern LPDIRECT3D8				DirectX_D3D;
extern LPDIRECT3DDEVICE8		DirectX_Device;
extern D3DCOLOR					DirectX_ClearColor;
extern D3DPRESENT_PARAMETERS	DirectX_PresentParams; 
////////////////////////////////////////////////////////////////////////////
//	Functions
////////////////////////////////////////////////////////////////////////////

void  Platform__Init( void );
void	Platform__ProcessCommandLine( const char * pCommandLine );
int   Platform__Framebuffer( HWND hWnd, int Color, int Depth, int Stencil, int Alpha, int Doublebuffer );
void  Platform__Destroy( void );
void  Platform__Resize( unsigned int width, unsigned int height );

void  DirectX__ClearColor( int r, int g, int b, int a );
void  DirectX__Fullscreen( bool fullscreen );

float DirectX__FrameRate( void );

void  DirectX__Debug( char* format, ... );

#ifdef _XBOX
void DirectX__Options( int width, int height, int format, int multisampletype );
#endif

////////////////////////////////////////////////////////////////////////////

#endif	// DIRECTX

#endif	// D_DIRECTX_H
