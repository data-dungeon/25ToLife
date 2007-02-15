////////////////////////////////////////////////////////////////////////////
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

////////////////////////////////////////////////////////////////////////////
//  Includes
////////////////////////////////////////////////////////////////////////////

/******************* includes ****************************/

#include "Display/DisplayPCH.h"





////////////////////////////////////////////////////////////////////////////
//  Defines
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
//  Types
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
//  Macros
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
//  Local variables
////////////////////////////////////////////////////////////////////////////

static int _MatrixStack_Mode = MS_PROJECTION;
static int _MatrixStack_Level[ MS_NUM_STACKS ] = { 0 };
static Matrix4x4 _MatrixStack[ MS_NUM_STACKS ][ MS_MATRIX_STACK_DEPTH ];

/* the perspective transform matrices for use by joe's renderer and particle renderer */

static Graphics4x4 mViewport __attribute__ ((aligned(BYTES_PER_QUAD_WORD)));
static Matrix4x4 mCameraToScreen __attribute__ ((aligned(BYTES_PER_QUAD_WORD)));
static bool bCameraToScreenDirty = TRUE;

////////////////////////////////////////////////////////////////////////////
//  Local prototypes
////////////////////////////////////////////////////////////////////////////

static void MatrixStack__UpdateViewportMatrix(u32 u32XPos, u32 u32YPos, u32 u32Width, u32 u32Height);

////////////////////////////////////////////////////////////////////////////
//  Globals
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
//  Functions
////////////////////////////////////////////////////////////////////////////

void MatrixStack__InitViewportMatrix(void)
{
	/* z-buffer max depends on z-buffer bit depth. in vu renderer we use a ftoi4 instruction to convert z value from */
	/* float to integer, so that does an effective multiply by 16, so the z-buffer max value that goes in the ClipToScreen */
	/* matrix should adjust to 1/16 the value. for example, if the max z-buffer value was 1000.0, then a point at z = 1.0 */
	/* in clip space (at the far clip plane) should be transformed to 250.0. then the ftoi4 will convert 250.0 to 1000 */

	float fZBufferFar = 0.0f;
//hack-- this is part of a fix to work around an apparent clipping bug in the vu code. large distant polys were being
// drawn instead of clipped. they went away if we used the cull microcode path instead of the clip path, but there also
// seems to be a bug in the cull code? we were getting small polys (also apparently distant) that would poke through the
// geometry. so provide some headroom in the far z-buffer
fZBufferFar = 10.0f;
	float fZBufferNear = 0.0f;
	switch(DisplayState.u32ZPixelFormat)
	{
	case SCE_GS_PSMZ32:
		fZBufferNear = (float) 0x7fffffff / 16.0f;	// ftoi instruction is signed, so no way to get that high bit (so far, anyway)
		break;
	case SCE_GS_PSMZ24:
//		ASSERT(FALSE);	// 24-bit works fine, but we have no way to take advantage of wasted bytes, so might as well use 32-bit z-buffer
		fZBufferNear = (float) 0x0fffff / 16.0f;	// ftoi instruction is signed, so no way to get that high bit (so far, anyway)
		break;
	case SCE_GS_PSMZ16S:
		fZBufferNear = (float) 0x7fff / 16.0f;		// ftoi instruction is signed, so no way to get that high bit (so far, anyway)
		break;
#ifdef _DEBUG
	default:
		ASSERT(FALSE);	// shouldn't be any unhandled cases
		break;
#endif //_DEBUG
	}

	/* build viewport matrix (aka clip to screen matrix). clip space is -1.0 < x < 1.0, -1.0 < y < 1.0. we want */
	/* to expand to the screen resolution, and to expand the z-range from 0.0 to 1.0 to zBufferMin to zBufferMax */

	mViewport.Identity();
	mViewport.r00 = (float) DisplayState.nScreenWidth / 2.0f;
	mViewport.r11 = (float) DisplayState.nScreenHeight / 2.0f;
	mViewport.r22 = (-fZBufferNear + fZBufferFar) / 2.0f;
	mViewport.tz = (fZBufferNear + fZBufferFar) / 2.0f;
	mViewport.tx = 2048.0f;	// this adds 2048 to the x-component, centering in the 4096 x 4096 render space
	mViewport.ty = 2048.0f;	// this adds 2048 to the y-component, centering in the 4096 x 4096 render space
	mViewport.w = 1.0f;

	PS2Renderer::SetViewToScreen(*(PS2Matrix4x4*)&mViewport);
}

////////////////////////////////////////////////////////////////////////////

void MatrixStack__Identity( void )
{
	_MatrixStack[_MatrixStack_Mode][_MatrixStack_Level[_MatrixStack_Mode]].Identity();

	/* camera-to-screen needs to be calculated next time it is requested */

	if (_MatrixStack_Mode == MS_PROJECTION)
	{
		PS2Renderer::SetCameraToView(*(PS2Matrix4x4*)MatrixStack__Get(MS_PROJECTION));
		bCameraToScreenDirty = TRUE;
	}
	else if(_MatrixStack_Mode == MS_TEXTURE)
		PS2Renderer::SetTexture(*(PS2Matrix4x4*)MatrixStack__Get(MS_TEXTURE));
}

////////////////////////////////////////////////////////////////////////////

void MatrixStack__Load( Matrix4x4* m )
{
	_MatrixStack[_MatrixStack_Mode][_MatrixStack_Level[_MatrixStack_Mode]] = *m;
	
	/* camera-to-screen needs to be calculated next time it is requested */

	if (_MatrixStack_Mode == MS_PROJECTION)
	{
		PS2Renderer::SetCameraToView(*(PS2Matrix4x4*)MatrixStack__Get(MS_PROJECTION));
		bCameraToScreenDirty = TRUE;
	}
	else if(_MatrixStack_Mode == MS_TEXTURE)
		PS2Renderer::SetTexture(*(PS2Matrix4x4*)MatrixStack__Get(MS_TEXTURE));
}

////////////////////////////////////////////////////////////////////////////

void MatrixStack__Multiply( Matrix4x4* m )
{
	// C = A x B
	_MatrixStack[_MatrixStack_Mode][_MatrixStack_Level[_MatrixStack_Mode]] = *m * _MatrixStack[_MatrixStack_Mode][_MatrixStack_Level[_MatrixStack_Mode]];

	/* camera-to-screen needs to be calculated next time it is requested */

	if (_MatrixStack_Mode == MS_PROJECTION)
	{
		PS2Renderer::SetCameraToView(*(PS2Matrix4x4*)MatrixStack__Get(MS_PROJECTION));
		bCameraToScreenDirty = TRUE;
	}
	else if(_MatrixStack_Mode == MS_TEXTURE)
		PS2Renderer::SetTexture(*(PS2Matrix4x4*)MatrixStack__Get(MS_TEXTURE));
}

////////////////////////////////////////////////////////////////////////////

void MatrixStack__Scale( float x, float y, float z )
{
	g4Temp.SetScale(x, y, z);
	_MatrixStack[_MatrixStack_Mode][_MatrixStack_Level[_MatrixStack_Mode]] = (Matrix4x4) g4Temp * _MatrixStack[_MatrixStack_Mode][_MatrixStack_Level[_MatrixStack_Mode]];

	/* camera-to-screen needs to be calculated next time it is requested */

	if (_MatrixStack_Mode == MS_PROJECTION)
	{
		PS2Renderer::SetCameraToView(*(PS2Matrix4x4*)MatrixStack__Get(MS_PROJECTION));
		bCameraToScreenDirty = TRUE;
	}
	else if(_MatrixStack_Mode == MS_TEXTURE)
		PS2Renderer::SetTexture(*(PS2Matrix4x4*)MatrixStack__Get(MS_TEXTURE));
}

////////////////////////////////////////////////////////////////////////////

void MatrixStack__Translate( float x, float y, float z )
{
	g4Temp.SetTranslation(x, y, z);
	_MatrixStack[_MatrixStack_Mode][_MatrixStack_Level[_MatrixStack_Mode]] = (Matrix4x4) g4Temp * _MatrixStack[_MatrixStack_Mode][_MatrixStack_Level[_MatrixStack_Mode]];

	/* camera-to-screen needs to be calculated next time it is requested */

	if (_MatrixStack_Mode == MS_PROJECTION)
	{
		PS2Renderer::SetCameraToView(*(PS2Matrix4x4*)MatrixStack__Get(MS_PROJECTION));
		bCameraToScreenDirty = TRUE;
	}
	else if(_MatrixStack_Mode == MS_TEXTURE)
		PS2Renderer::SetTexture(*(PS2Matrix4x4*)MatrixStack__Get(MS_TEXTURE));
}

////////////////////////////////////////////////////////////////////////////

void MatrixStack__LookAt( float eyex, float eyey, float eyez, float lookx, float looky, float lookz, float upx, float upy, float upz )
{
	ASSERT(0);	// not supported yet
}

////////////////////////////////////////////////////////////////////////////

void MatrixStack__Perspective( float fovy, float aspect, float fNearClip, float fFarClip )
{
	/* build perspective projection matrix (aka view (camera) to clip space matrix) */

	float fFieldOfViewYRadians = Math::Deg2Rad(fovy);
	float fSin, fCos;
	float fCotangent;
	Math::SinCos(fFieldOfViewYRadians / 2.0f, fSin, fCos);
	fCotangent = fCos / fSin;

	/* the purpose of this matrix is to transform from camera space (the view frustum) to canonical clip space, */
	/* which is a 4-sided pyramid where x and y range from -1.0 to 1.0 and z ranges from 0.0 to 1.0. points on the */
	/* near clip plane end up at z = 0.0 in clip space, points on the far clip plane end up at z = 1.0 */

	Matrix4x4 *pCameraToClip = &_MatrixStack[MS_PROJECTION][_MatrixStack_Level[MS_PROJECTION]];
	pCameraToClip->Identity();
	pCameraToClip->r00 = fCotangent / aspect;
	pCameraToClip->r11 = -fCotangent;
	pCameraToClip->r22 = (fNearClip + fFarClip) / (fNearClip - fFarClip);
	pCameraToClip->e23 = -1.0f;	//yuch-- flip in y? world-to-camera matrix is calculated to work with opengl, which uses y = 0 to be bottom of screen
	pCameraToClip->tz = (2 * fNearClip * fFarClip) / (fNearClip - fFarClip);
	pCameraToClip->w = 0.0f;

	/* camera-to-screen needs to be calculated next time it is requested */

	PS2Renderer::SetCameraToView(*(PS2Matrix4x4*)MatrixStack__Get(MS_PROJECTION));
	bCameraToScreenDirty = TRUE;
}

////////////////////////////////////////////////////////////////////////////

void MatrixStack__Orthographic( float left, float right, float top, float bottom, float znear, float zfar )
{
	Matrix4x4 m;

	// TJC - stolen from appendix F of the OpenGL Programming Guide
	// I know this needs an adjustment for Z somewhere in here... just dont
	// remember what off-hand..  something about 0-1 instead of -1 to 1...
	m.rx = 2.0f / (right - left);
	m.ux = 0.0f;
	m.dx = 0.0f;
	m.px = -(right + left)/(right - left);
	m.ry = 0.0f;
	m.uy = 2.0f / (top - bottom);
	m.dy = 0.0f;
	m.py = -(top + bottom)/(top - bottom);
	m.rz = 0.0f;
	m.uz = 0.0f;
	m.dz = -2.0f / (zfar - znear);
	m.pz = (zfar + znear) / (zfar - znear);
	m.x0 = 0.0f;
	m.x1 = 0.0f;
	m.x2 = 0.0f;
	m.mw = 1.0f;

	
	MatrixStack__Multiply( &m );
}

////////////////////////////////////////////////////////////////////////////

static void MatrixStack__UpdateViewportMatrix(u32 u32XPos, u32 u32YPos, u32 u32Width, u32 u32Height)
{
	/* update proper matrix elements */

	u32 screenXCenter = DisplayState.u32FrameBufferWidth >> 1;
	u32 screenYCenter = DisplayState.u32FrameBufferHeight >> 1;

	u32 viewXCenter = u32Width >> 1;
	u32 viewYCenter = u32Height >> 1;

	mViewport.r00 = (float) viewXCenter;
	mViewport.r11 = (float) viewYCenter;

	mViewport.tx = 2048.0f + u32XPos - screenXCenter + viewXCenter;
	mViewport.ty = 2048.0f + u32YPos - screenYCenter + viewYCenter;

	/* send new viewport matrix to vu */

	//Renderer.initVUMem();

#ifdef PS2
	PS2Renderer::SetViewToScreen(*(PS2Matrix4x4*)&mViewport);
#endif //PS2

	/* camera-to-screen needs to be calculated next time it is requested */

	bCameraToScreenDirty = TRUE;
}

////////////////////////////////////////////////////////////////////////////

void MatrixStack__Viewport(ts_Viewport *pViewport)
{
	/* convert to pixels. x = 0.0 means left edge, y = 0.0 means top edge */

	float fWidth = (float) Display__GetScreenWidth();
	float fHeight = (float) Display__GetScreenHeight();
	u32 u32X = (u32) (pViewport->fX * fWidth);
	u32 u32Y = (u32) (pViewport->fY * fHeight);
	u32 u32Width = (u32) (pViewport->fWidth * fWidth);
	u32 u32Height = (u32) (pViewport->fHeight * fHeight);

	/* build viewport matrix (aka clip to screen matrix). clip space is -1.0 < x < 1.0, -1.0 < y < 1.0. we want */
	/* to expand to the screen resolution, and to expand the z-range from 0.0 to 1.0 to zBufferMin to zBufferMax */

	MatrixStack__UpdateViewportMatrix(u32X, u32Y, u32Width, u32Height);

	/* set scissor region */

	GSState.SCISSOR_1 = SCE_GS_SET_SCISSOR(u32X, u32X + u32Width - 1, u32Y, u32Y + u32Height - 1);
	GSState__Set(SCE_GS_SCISSOR_1, GSState.SCISSOR_1);
}

void MatrixStack__FullScreenViewport(void)
{
	MatrixStack__ViewportPixels(0, 0, Display__GetScreenWidth(), Display__GetScreenHeight());
}

/* allow the user to set viewport in pixel units for exact support for shadow creation */

void MatrixStack__ViewportPixels(u32 u32X, u32 u32Y, u32 u32Width, u32 u32Height)
{
	/* build viewport matrix (aka clip to screen matrix). clip space is -1.0 < x < 1.0, -1.0 < y < 1.0. we want */
	/* to expand to the screen resolution, and to expand the z-range from 0.0 to 1.0 to zBufferMin to zBufferMax */

	MatrixStack__UpdateViewportMatrix(u32X, u32Y, u32Width, u32Height);

	/* set scissor region */

	GSState.SCISSOR_1 = SCE_GS_SET_SCISSOR(u32X, u32X + u32Width - 1, u32Y, u32Y + u32Height - 1);
	GSState__Set(SCE_GS_SCISSOR_1, GSState.SCISSOR_1);
}

////////////////////////////////////////////////////////////////////////////

void MatrixStack__Push( void )
{
	ASSERT(_MatrixStack_Level[_MatrixStack_Mode] < MS_MATRIX_STACK_DEPTH - 1);	// if this hits, increase MS_MATRIX_STACK_DEPTH
	_MatrixStack_Level[_MatrixStack_Mode]++;

	/* new top matrix should be copy of just pushed matrix */

	_MatrixStack[_MatrixStack_Mode][_MatrixStack_Level[_MatrixStack_Mode]] = _MatrixStack[_MatrixStack_Mode][_MatrixStack_Level[_MatrixStack_Mode] - 1];
}

////////////////////////////////////////////////////////////////////////////

void MatrixStack__Pop( void )
{
	ASSERT(_MatrixStack_Level[_MatrixStack_Mode] > 0);	// if this hits, trying to pop past top matrix
	_MatrixStack_Level[_MatrixStack_Mode]--;

	/* camera-to-screen needs to be calculated next time it is requested */

	if (_MatrixStack_Mode == MS_PROJECTION)
	{
		PS2Renderer::SetCameraToView(*(PS2Matrix4x4*)MatrixStack__Get(MS_PROJECTION));
		bCameraToScreenDirty = TRUE;
	}
	else if(_MatrixStack_Mode == MS_TEXTURE)
		PS2Renderer::SetTexture(*(PS2Matrix4x4*)MatrixStack__Get(MS_TEXTURE));
}

////////////////////////////////////////////////////////////////////////////

void MatrixStack__Mode( int mode )
{
	ASSERT((mode == MS_PROJECTION) || (mode == MS_MODELVIEW) || (mode == MS_TEXTURE));
	_MatrixStack_Mode = mode;
}

////////////////////////////////////////////////////////////////////////////

Matrix4x4 *MatrixStack__Get( int mode )
{
	ASSERT((mode == MS_PROJECTION) || (mode == MS_MODELVIEW) || (mode == MS_TEXTURE));
	return(&_MatrixStack[mode][_MatrixStack_Level[mode]]);
}

////////////////////////////////////////////////////////////////////////////

/* access to perspective transform matrices */

Graphics4x4 *MatrixStack__GetViewport(void)
{
	return(&mViewport);
}

Matrix4x4 *MatrixStack__GetCameraToScreen(void)
{
	if (bCameraToScreenDirty)
	{
		/* geometry that doesn't need to be clipped can be transformed directly from camera to screen space */

		mCameraToScreen = *MatrixStack__Get(MS_PROJECTION) * (Matrix4x4) mViewport;
		bCameraToScreenDirty = FALSE;
	}
	return(&mCameraToScreen);
}
