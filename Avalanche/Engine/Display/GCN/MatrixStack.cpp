///////////////////////////////////////////////////////////////////////////
//
//  MatrixStack.c
//
//  MatrixStack Rendering Module.
//
//  Todd Blackburn, June 2001.
//
//  Copyright (c) 2000 Avalanche Software.
//
////////////////////////////////////////////////////////////////////////////

#ifndef GCN
#pragma message( "ERROR: GCN should be defined when compiling this module!" )
#endif


////////////////////////////////////////////////////////////////////////////
//  Includes
////////////////////////////////////////////////////////////////////////////

#include "Display/DisplayPCH.h"





////////////////////////////////////////////////////////////////////////////
//  Defines
////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#define Round( val) ( Math::Abs( val) > 1e-5 ? ( val) : 0.0f)
#else
#define Round( val) ( val)
#endif

struct ts_MtxStack
{
	Matrix4x4			*pStackBase,
							*pTopOfStack;
	GXProjectionType	*pGXPT,
							*pGXPTTop;
	u32					u32StackSize,
							u32StackPos;
};

#define CurrStack aMtxStacks[_MatrixStack_Mode]

////////////////////////////////////////////////////////////////////////////
//  Local variables
////////////////////////////////////////////////////////////////////////////

static ts_MtxStack	aMtxStacks[MS_NUM_STACKS];

static int				aiStackSize[MS_NUM_STACKS] = 
{
	8,				//MS_PROJECTION
	16,			//MS_MODELVIEW
	16				//MS_TEXTURE
};

static int gxMtxID[] =
{
	GX_PNMTX0,
	GX_PNMTX1,
	GX_PNMTX2,
	GX_PNMTX3
};

static int gxTexMtxID[] =
{
	GX_TEXMTX0,
	GX_TEXMTX1,
	GX_TEXMTX2,
	GX_TEXMTX3,
	GX_TEXMTX4,
	GX_TEXMTX5,
	GX_TEXMTX6,
	GX_TEXMTX7
};

////////////////////////////////////////////////////////////////////////////
//  Globals
////////////////////////////////////////////////////////////////////////////

int _MatrixStack_Mode = MS_PROJECTION;

GXProjectionType _MatrixStack_ProjectionType;

////////////////////////////////////////////////////////////////////////////
//  Macros
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
//  Functions
////////////////////////////////////////////////////////////////////////////

static void _MatrixStack__Send( void);
static void _MatrixStack__Send( MtxPtr pMtx, MtxPtr pMtxInv, GXPosNrmMtx gxpnm);
static void _MatrixStack__Mtx2Matrix44( Mtx mtx, Matrix4x4 *pMtx44);
static void _MatrixStack__Matrix442Mtx( Matrix4x4 *pMtx44, Mtx mtx);

void MatrixStack__Init( void)
{

	ts_MtxStack		*pMtxStack;

	Matrix4x4		*pMatrices;

	int		iStack,
				nMatrices;

	for( iStack = 0, nMatrices = 0; iStack < MS_NUM_STACKS; iStack++)
	{
		nMatrices += aiStackSize[iStack];
	}

	MEM_SET_ALLOC_NAME( "Matrix Stack");
	pMatrices = (Matrix4x4 *)memAlloc( nMatrices * sizeof(Matrix4x4));
	ASSERT( pMatrices != NULL);

	for( iStack = 0, pMtxStack = aMtxStacks; iStack < MS_NUM_STACKS; iStack++, pMtxStack++)
	{
		pMtxStack->pStackBase = pMatrices;
		pMatrices += aiStackSize[iStack];

		pMtxStack->u32StackSize = aiStackSize[iStack];
		pMtxStack->u32StackPos = 0;
		pMtxStack->pTopOfStack = pMtxStack->pStackBase + pMtxStack->u32StackPos;

		pMtxStack->pGXPT = NULL;
		pMtxStack->pGXPTTop = NULL;
	}

	aMtxStacks[MS_PROJECTION].pGXPT =
		(GXProjectionType *)memAlloc( aiStackSize[MS_PROJECTION] * sizeof(GXProjectionType));
	aMtxStacks[MS_PROJECTION].pGXPTTop = aMtxStacks[MS_PROJECTION].pGXPT;
}

void MatrixStack__InitViewportMatrix(void)
{
	/* nothing to do for gcn */
}

////////////////////////////////////////////////////////////////////////////

void MatrixStack__Identity( void )
{
	CurrStack.pTopOfStack->Identity( );
    
	_MatrixStack__Send( );
}

////////////////////////////////////////////////////////////////////////////

void MatrixStack__Load( Matrix4x4 *pMtx)
{
	memcpy( CurrStack.pTopOfStack, pMtx, sizeof(Matrix4x4));

	_MatrixStack__Send( );
}

void MatrixStack__LoadMany( u32 iMatrices, Mtx *am, Mtx *mi)
{

	int	i;

	for( i = 0; i < 4; i++)
	{
		_MatrixStack__Send( am[iMatrices & 0xFF], mi[iMatrices & 0xFF], (GXPosNrmMtx)gxMtxID[i]);
		iMatrices >>= 8;
	}
}

////////////////////////////////////////////////////////////////////////////

void MatrixStack__Multiply( Matrix4x4 *pMtx)
{
	*(CurrStack.pTopOfStack) = *pMtx * *( CurrStack.pTopOfStack);
   
	_MatrixStack__Send( );
}

////////////////////////////////////////////////////////////////////////////

void MatrixStack__Scale( float x, float y, float z )
{

	Matrix4x4		mtx;

	ASSERT( CurrStack.u32StackSize > ( CurrStack.u32StackPos + 1));

	mtx.Identity( );

	mtx.r00 = x;
	mtx.r11 = y;
	mtx.r22 = z;
    
	MatrixStack__Multiply( &mtx);
}

////////////////////////////////////////////////////////////////////////////

void MatrixStack__Translate( float x, float y, float z )
{

	Matrix4x4		mtx;

	ASSERT( CurrStack.u32StackSize > ( CurrStack.u32StackPos + 1));

	mtx.Identity( );

	mtx.tx = x;
	mtx.ty = y;
	mtx.tz = z;
    
	MatrixStack__Multiply( &mtx);
}

////////////////////////////////////////////////////////////////////////////

void MatrixStack__LookAt( float eyex, float eyey, float eyez, float lookx, float looky, float lookz, float upx, float upy, float upz )
{
    
	Mtx				m;

	Matrix4x4		mtx44;

	Point3d			p3dEye,
						p3dTarget;

	Vec				vecUp;

	p3dEye.x = eyex;
	p3dEye.y = eyey;
	p3dEye.z = eyez;

	p3dTarget.x = lookx;
	p3dTarget.y = looky;
	p3dTarget.z = lookz;

	vecUp.x = upx;
	vecUp.y = upy;
	vecUp.z = upz;

	MTXLookAt( m, &p3dEye, &vecUp, &p3dTarget);

	_MatrixStack__Mtx2Matrix44( m, &mtx44);

	MatrixStack__Multiply( &mtx44);
}

////////////////////////////////////////////////////////////////////////////

void MatrixStack__Perspective( float fovy, float aspect, float znear, float zfar )
{

	Matrix4x4 m;

	if( znear == zfar)
	{
		return;
	}
    
	MTXPerspective( (Mtx44Ptr)&m, fovy, aspect, znear, zfar);
	_MatrixStack_ProjectionType = GX_PERSPECTIVE;
	*CurrStack.pGXPTTop = _MatrixStack_ProjectionType;

	m.Transpose( );
    
	MatrixStack__Multiply( &m);
}

////////////////////////////////////////////////////////////////////////////

void MatrixStack__Orthographic( float left, float right, float top, float bottom, float znear, float zfar )
{

	Matrix4x4 m;

	MTXOrtho( (Mtx44Ptr)&m, top, bottom, left, right, znear, zfar);
	_MatrixStack_ProjectionType = GX_ORTHOGRAPHIC;
	*CurrStack.pGXPTTop = _MatrixStack_ProjectionType;
    
	m.Transpose( );

	MatrixStack__Multiply( &m);
}

////////////////////////////////////////////////////////////////////////////

void MatrixStack__Viewport( ts_Viewport *pViewport)
{

	f32		fWidth,
				fHeight;

	/* convert to pixels for use */
	fWidth = (f32)Display__GetScreenWidth();
	fHeight = (f32)Display__GetScreenHeight();
	GXSetViewport(	pViewport->fX * fWidth,
						pViewport->fY * fHeight,
						pViewport->fWidth * fWidth,
						pViewport->fHeight * fHeight, 
						0.0F,
						1.0F);
	GXSetScissor(	(u32)( pViewport->fX * fWidth),
						(u32)( pViewport->fY * fHeight),
						(u32)( pViewport->fWidth * fWidth),
						(u32)( pViewport->fHeight * fHeight));
}

void MatrixStack__FullScreenViewport(void)
{
	MatrixStack__ViewportPixels(0, 0, Display__GetScreenWidth(), Display__GetScreenHeight());
}

void MatrixStack__ViewportPixels( u32 u32X, u32 u32Y, u32 u32Width, u32 u32Height)
{
	GXSetViewport(	(f32)u32X,
						(f32)u32Y,
						(f32)u32Width,
						(f32)u32Height, 
						0.0F,
						1.0F);
	GXSetScissor( u32X, u32Y, u32Width, u32Height);	// viewport alone is not enough on ps2
}

////////////////////////////////////////////////////////////////////////////

void MatrixStack__Push( void )
{
	ASSERT( CurrStack.u32StackSize > ( CurrStack.u32StackPos + 1));

	memcpy( CurrStack.pTopOfStack + 1, CurrStack.pTopOfStack, sizeof(Matrix4x4));
	if( _MatrixStack_Mode == MS_PROJECTION)
	{
		CurrStack.pGXPTTop++;
		*CurrStack.pGXPTTop = _MatrixStack_ProjectionType;
	}
	
	CurrStack.pTopOfStack++;
	CurrStack.u32StackPos++;
}

////////////////////////////////////////////////////////////////////////////

void MatrixStack__Pop( void )
{
	ASSERT( CurrStack.u32StackPos > 0 );
	CurrStack.u32StackPos--;
	CurrStack.pTopOfStack--;
	if( _MatrixStack_Mode == MS_PROJECTION)
	{
		CurrStack.pGXPTTop--;
		_MatrixStack_ProjectionType = *CurrStack.pGXPTTop;
	}

	_MatrixStack__Send( );
}

////////////////////////////////////////////////////////////////////////////

void MatrixStack__Mode( int mode )
{
	_MatrixStack_Mode = mode;
}

////////////////////////////////////////////////////////////////////////////
Matrix4x4 *MatrixStack__Get( int mode )
{
	return( aMtxStacks[mode].pTopOfStack);
}

////////////////////////////////////////////////////////////////////////////
void MatrixStack__Matrix4x4ToMtx( Matrix4x4 *pMtx44, MtxPtr mtx)
{
	_MatrixStack__Matrix442Mtx( pMtx44, mtx);
}

////////////////////////////////////////////////////////////////////////////
static void _MatrixStack__Send( void)
{

	Mtx				mtx;

	Matrix4x4		Mtx44,
						*pMtx44;

	if( _MatrixStack_Mode == MS_PROJECTION)
	{
		Mtx44 = *CurrStack.pTopOfStack;

		Mtx44.r00 = Round( Mtx44.r00);
		Mtx44.r01 = Round( Mtx44.r01);
		Mtx44.r02 = Round( Mtx44.r02);
		Mtx44.e03 = Round( Mtx44.e03);

		Mtx44.r10 = Round( Mtx44.r10);
		Mtx44.r11 = Round( Mtx44.r11);
		Mtx44.r12 = Round( Mtx44.r12);
		Mtx44.e13 = Round( Mtx44.e13);

		Mtx44.r20 = Round( Mtx44.r20);
		Mtx44.r21 = Round( Mtx44.r21);
		Mtx44.r22 = Round( Mtx44.r22);
		Mtx44.e23 = Round( Mtx44.e23);

		Mtx44.tx = Round( Mtx44.tx);
		Mtx44.ty = Round( Mtx44.ty);
		Mtx44.tz = Round( Mtx44.tz);
		Mtx44.w = Round( Mtx44.w);

		Mtx44.Transpose( );

		GXSetProjection( (Mtx44Ptr)&Mtx44, _MatrixStack_ProjectionType);
	}
	else
	if( _MatrixStack_Mode == MS_MODELVIEW)
	{
		Mtx44 = *CurrStack.pTopOfStack;

		_MatrixStack__Matrix442Mtx( &Mtx44, mtx);
		GXLoadPosMtxImm( mtx, GX_PNMTX0);

		Mtx44.Invert( );
		Mtx44.Transpose( );
		_MatrixStack__Matrix442Mtx( &Mtx44, mtx);

		GXLoadNrmMtxImm( mtx, GX_PNMTX0);
	}
	else
	if( _MatrixStack_Mode == MS_TEXTURE)
	{
		pMtx44 = CurrStack.pTopOfStack;

		_MatrixStack__Matrix442Mtx( pMtx44, mtx);
		GXLoadTexMtxImm( mtx, gxTexMtxID[TextureState__ActiveUnit( )], GX_MTX3x4);
	}
}

static void _MatrixStack__Send( MtxPtr pMtx, MtxPtr pMtxInv, GXPosNrmMtx gxpnm)
{
	if( _MatrixStack_Mode == MS_MODELVIEW)
	{
		GXLoadPosMtxImm( pMtx, gxpnm);

		GXLoadNrmMtxImm( pMtxInv, gxpnm);
	}
}

static void _MatrixStack__Mtx2Matrix44( Mtx mtx, Matrix4x4 *pMtx44)
{

	pMtx44->Identity( );

	pMtx44->r00 = mtx[0][0];
	pMtx44->r10 = mtx[0][1];
	pMtx44->r20 = mtx[0][2];
	pMtx44->tx = mtx[0][3];

	pMtx44->r01 = mtx[1][0];
	pMtx44->r11 = mtx[1][1];
	pMtx44->r21 = mtx[1][2];
	pMtx44->ty = mtx[1][3];

	pMtx44->r02 = mtx[2][0];
	pMtx44->r12 = mtx[2][1];
	pMtx44->r22 = mtx[2][2];
	pMtx44->tz = mtx[2][3];
}

static void _MatrixStack__Matrix442Mtx( Matrix4x4 *pMtx44, Mtx mtx)
{
	mtx[0][0] = Round( pMtx44->r00);
	mtx[0][1] = Round( pMtx44->r10);
	mtx[0][2] = Round( pMtx44->r20);
	mtx[0][3] = Round( pMtx44->tx);

	mtx[1][0] = Round( pMtx44->r01);
	mtx[1][1] = Round( pMtx44->r11);
	mtx[1][2] = Round( pMtx44->r21);
	mtx[1][3] = Round( pMtx44->ty);

	mtx[2][0] = Round( pMtx44->r02);
	mtx[2][1] = Round( pMtx44->r12);
	mtx[2][2] = Round( pMtx44->r22);
	mtx[2][3] = Round( pMtx44->tz);
}
