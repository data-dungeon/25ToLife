///////////////////////////////////////////////////////////////////////////
//
//  MatrixStack.c
//
//  MatrixStack Rendering Module.
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

/******************* includes ****************************/

/*** pre-compiled headers through .h file (visual studio), can't be inside #if statement. ***/
/*** see .h file for more info.for codewarrior, pre-compiled header is handled through    ***/
/*** xcl compile/link interpreter, and doesn't need any explicit include                  ***/

#include "Display/DisplayPCH.h"





////////////////////////////////////////////////////////////////////////////
//  Globals
////////////////////////////////////////////////////////////////////////////

D3DTRANSFORMSTATETYPE _MatrixStack_Modes[] =
{
    D3DTS_PROJECTION,
    D3DTS_VIEW,
	 D3DTS_TEXTURE0,
	 D3DTS_TEXTURE1,
	 D3DTS_TEXTURE2,
	 D3DTS_TEXTURE3,
};


////////////////////////////////////////////////////////////////////////////
//  Globals
////////////////////////////////////////////////////////////////////////////

D3DXMATRIX					_MatrixStack_Identity;
D3DXMATRIX					_MatrixStack_RHProjectionToTexture;

D3DTRANSFORMSTATETYPE	_MatrixStack_ModeName = D3DTS_PROJECTION;

int							_MatrixStack_Mode = MS_PROJECTION;
int							_MatrixStack_Level[ MS_NUM_STACKS ] = { 0, 0 };

Matrix4x4					_MatrixStack[ MS_NUM_STACKS ][ MS_MATRIX_STACK_DEPTH ];


////////////////////////////////////////////////////////////////////////////
//  Functions
////////////////////////////////////////////////////////////////////////////

void MatrixStack__InitViewportMatrix( void )
{
	D3DXMATRIX mScale, mBias;


   D3DXMatrixIdentity( &mScale );
   mScale._11 = -.5f;
   mScale._22 = .5f;
	mScale._33 = -1.0f;

   D3DXMatrixIdentity( &mBias );
   mBias._31 = .5f;
   mBias._32 = .5f;

	D3DXMatrixMultiply( & _MatrixStack_RHProjectionToTexture, & mScale, & mBias );

	D3DXMatrixIdentity( & _MatrixStack_Identity );

	MatrixStack__Mode( MS_TEXTURE );
	MatrixStack__Identity( );
	MatrixStack__Mode( MS_TEXTURE1 );
	MatrixStack__Identity( );
	MatrixStack__Mode( MS_TEXTURE2 );
	MatrixStack__Identity( );
	MatrixStack__Mode( MS_TEXTURE3 );
	MatrixStack__Identity( );

	MatrixStack__Mode( MS_PROJECTION );

}

////////////////////////////////////////////////////////////////////////////

void _MatrixStack__Set( Matrix4x4* m )
{
    int level = _MatrixStack_Level[ _MatrixStack_Mode ];

    _MatrixStack[ _MatrixStack_Mode ][ level ] = *m;
}

////////////////////////////////////////////////////////////////////////////

void MatrixStack__Identity( void )
{
	IDirect3DDevice8_SetTransform( DirectX_Device, _MatrixStack_ModeName, &_MatrixStack_Identity );
	
	_MatrixStack__Set( ( Matrix4x4* ) & _MatrixStack_Identity );
}

////////////////////////////////////////////////////////////////////////////

bool MatrixStack__IsIdentity( int mode  )
{
	if( mode > MS_TEXTURE3 )
		return true;

	int level = _MatrixStack_Level[ mode ];

	return _MatrixStack[ mode ][ level ].IsIdentity( );	
}

////////////////////////////////////////////////////////////////////////////

void MatrixStack__Load( Matrix4x4* m )
{
	IDirect3DDevice8_SetTransform( DirectX_Device, _MatrixStack_ModeName, ( D3DXMATRIX* )m );
	
	_MatrixStack__Set( m );
}

////////////////////////////////////////////////////////////////////////////

void MatrixStack__LoadMany( unsigned int matrix_index, Matrix4x4* matrices )
{
	// We're overriding the normal (single) matrix stack with many matrices.
	MatrixStack__Identity( );

	for ( int i = 0; i < 4; i++ )
	{
		// there are four bone weights contained in the matrix_index.  in fact,
		// it really isn't a matrix index at all and should probably be a 
		// struct of four uchar's instead, because that is how it is used.  so, 
		// this code basically snags the right byte and masks that off into an 
		// actual index into the matrix list.
		unsigned int index = ( matrix_index >> ( i * 8 ) ) & 0xFF;

		IDirect3DDevice8_SetTransform( DirectX_Device, D3DTS_WORLDMATRIX( i ), ( D3DXMATRIX* )&( matrices[ index ] ) );
	}
}

////////////////////////////////////////////////////////////////////////////

void MatrixStack__IdentityMany( )
{
	Matrix4x4 m;
	m.Identity();

	for ( int i = 0; i < 4; i++ )
	{
		IDirect3DDevice8_SetTransform( DirectX_Device, D3DTS_WORLDMATRIX( i ), ( D3DXMATRIX* )&m );
	}
}

////////////////////////////////////////////////////////////////////////////

void MatrixStack__Multiply( Matrix4x4* m )
{
    IDirect3DDevice8_MultiplyTransform( DirectX_Device, _MatrixStack_ModeName, ( D3DXMATRIX* )m );

    _MatrixStack__Set( m );
}

////////////////////////////////////////////////////////////////////////////

void MatrixStack__Scale( float x, float y, float z )
{
    D3DXMATRIX m;
    
    D3DXMatrixScaling( &m, x, y, z );
    
    IDirect3DDevice8_MultiplyTransform( DirectX_Device, _MatrixStack_ModeName, &m );
}

////////////////////////////////////////////////////////////////////////////

void MatrixStack__Translate( float x, float y, float z )
{
    D3DXMATRIX m;
    
    D3DXMatrixTranslation( &m, x, y, z );
    
    IDirect3DDevice8_MultiplyTransform( DirectX_Device, _MatrixStack_ModeName, &m );
}

////////////////////////////////////////////////////////////////////////////

void MatrixStack__LookAt( float eyex, float eyey, float eyez, float lookx, float looky, float lookz, float upx, float upy, float upz )
{
    D3DXMATRIX m;

    D3DXVECTOR3 eye  = D3DXVECTOR3(  eyex,  eyey,  eyez );
    D3DXVECTOR3 look = D3DXVECTOR3( lookx, looky, lookz );
    D3DXVECTOR3 up   = D3DXVECTOR3(   upx,   upy,   upz );

    D3DXMatrixLookAtLH( &m, &eye, &look, &up );

    MatrixStack__Multiply( ( Matrix4x4* )&m );
}

////////////////////////////////////////////////////////////////////////////

void MatrixStack__Perspective( float fovy, float aspect, float znear, float zfar )
{
    D3DXMATRIX m;
    
    D3DXMatrixPerspectiveFovRH( &m, D3DXToRadian( fovy ), aspect, znear, zfar );

    MatrixStack__Multiply( ( Matrix4x4* )&m );
}

////////////////////////////////////////////////////////////////////////////

void MatrixStack__Orthographic( float left, float right, float top, float bottom, float znear, float zfar )
{
    D3DXMATRIX m;
    
    D3DXMatrixOrthoOffCenterRH( &m, left, right, top, bottom, znear, zfar );
    
    MatrixStack__Multiply( ( Matrix4x4* )&m );
}

////////////////////////////////////////////////////////////////////////////

void MatrixStack__Viewport( ts_Viewport *pViewport )
{
	D3DVIEWPORT8 v;

	/* convert to pixels for use with glViewport */
	v.Width = Display__GetScreenWidth();
	v.Height = Display__GetScreenHeight();
	v.X = (u32) (pViewport->fX * v.Width);
	v.Y = (u32) (pViewport->fY * v.Height);
#ifndef DIRECTX_PC
	v.Width = (u32) (pViewport->fWidth * v.Width);
	v.Height = (u32) (pViewport->fHeight * v.Height);
#endif
	v.MinZ   = 0.0f;
	v.MaxZ   = 1.0f;

	HRESULT result = IDirect3DDevice8_SetViewport( DirectX_Device, &v );
	ASSERT( result == D3D_OK );
}

////////////////////////////////////////////////////////////////////////////

void MatrixStack__FullScreenViewport( void )
{
	MatrixStack__ViewportPixels(0, 0, Display__GetScreenWidth(), Display__GetScreenHeight());
}

/* allow the user to set viewport in pixel units for exact support for shadow creation */

void MatrixStack__ViewportPixels(u32 u32X, u32 u32Y, u32 u32Width, u32 u32Height)
{
	D3DVIEWPORT8 v;
	v.X = u32X;
	v.Y = u32Y;
	v.Width = u32Width;
	v.Height = u32Height;
	v.MinZ   = 0.0f;
	v.MaxZ   = 1.0f;

	HRESULT result = IDirect3DDevice8_SetViewport( DirectX_Device, &v );
	ASSERT( result == D3D_OK );
}

////////////////////////////////////////////////////////////////////////////

void MatrixStack__Push( void )
{
	ASSERT(  _MatrixStack_Level[ _MatrixStack_Mode ] < MS_MATRIX_STACK_DEPTH );

	Matrix4x4 *m = &( _MatrixStack[ _MatrixStack_Mode ][ _MatrixStack_Level[ _MatrixStack_Mode ] ] );
	IDirect3DDevice8_GetTransform( DirectX_Device, _MatrixStack_ModeName, ( D3DXMATRIX* )m );
	_MatrixStack__Set( m );
	_MatrixStack_Level[ _MatrixStack_Mode ]++;

}

////////////////////////////////////////////////////////////////////////////

void MatrixStack__Pop( void )
{

	ASSERT( _MatrixStack_Level[ _MatrixStack_Mode ] > 0 );

	_MatrixStack_Level[ _MatrixStack_Mode ]--;
	Matrix4x4 *m = &( _MatrixStack[ _MatrixStack_Mode ][ _MatrixStack_Level[ _MatrixStack_Mode ] ] );
	IDirect3DDevice8_SetTransform( DirectX_Device, _MatrixStack_ModeName, ( D3DXMATRIX* )m );
}

////////////////////////////////////////////////////////////////////////////

void MatrixStack__Mode( int mode )
{
	_MatrixStack_Mode = mode;
   _MatrixStack_ModeName = _MatrixStack_Modes[ mode ];
}

////////////////////////////////////////////////////////////////////////////

Matrix4x4 *MatrixStack__Get( int mode )
{
	ASSERT( mode < MS_NUM_STACKS );
	return(&_MatrixStack[mode][_MatrixStack_Level[mode]]);
}

////////////////////////////////////////////////////////////////////////////
