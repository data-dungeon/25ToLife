/*************************************/
/*                                   */
/*   d_DisplayPlatform.h             */
/*   new engine  04/11/00            */
/*   ned martin  avalanche software  */
/*   visual c++ 6.0                  */
/*   platform-specific display stuff */
/*                                   */
/*************************************/

#ifndef __D_DISPLAYPLATFORM_H
#define __D_DISPLAYPLATFORM_H

#include "platform/BaseType.h"		// low-level include
#include "platform/CompilerControl.h"	// for NAMED_SECTION
#ifdef DIRECTX_PC
#include "Display/DirectX/DXPublished.h"
#endif

#if defined(__cplusplus)
extern "C" {
#endif // __cplusplus

/******************* global prototypes *******************/

void Display__SetIdleCallback(void (*callback)(void));
bool Display__InitGraphics(void);
void Display__ResetGraphics(void);
#ifdef WIN32
bool Display__DestroyWindow(void);
bool gfxOnCreate(HWND hwnd);
void gfxOnDestroy(void);
void gfxOnResize(short sWidth, short sHeight);
void gfxSetWindowText( char* text );
#ifdef DIRECTX_PC
void gfxBlitTextureToScreen(IDirect3DTexture8 *texture, float left = 0.0f,
														float top = 0.0f,
														float right = 1.0f,
														float bottom = 1.0f );
#endif //DIRECTX_PC
#endif //WIN32

void gfxReadTextureMemory(u32 u32TMemAddress, u32 u32Height, void **ppImage, char *name);
void gfxWriteTextureMemory(u32 u32TMemAddress, u32 u32Height, void *pImage);

#ifndef CONSUMER_BUILD
void gfxSnapScreen(void);
#ifdef PS2
void gfxSnapTexture(int bind);
#endif //PS2
#endif //CONSUMER_BUILD

/* Do some motion blur! strength 0-128, timothyLeary 0 - whatever dude (div 8 is # pixels shifted) */
void gfxMotionBlur(u32 strength, u32 timothyLeary);
void gfxRenderMotionBlur(void);

/* fog on ps2 is done as a post-process on frame buffer, using a palette texture derived from the z-buffer */
void gfxRenderPS2Fog(void);

#ifdef PS2
void Display__ClearGraphics(void);
bool Display__DownSampling(void);
void gfxDownSampleFrameBuffer(void);
#endif


// render target texture for back buffer effect emulation
#ifdef DIRECTX_PC
class Texture;
namespace RenderToTexture 
{
	extern bool renderingToTexture;										// can we render to a texture?
	extern Texture * renderSurface;										// our actual surface
	extern bool isRenderingToTextureOn;									// did the user turn it on?
	extern long	renderSurfaceUsedHeight;								// used height in pixels
	extern long renderSurfaceUsedWidth;									// used width in pixels
	void SetUpForRenderToTexture();										// do what we need to render to texture
	bool CreateRenderTexture( int height, int width, D3DFORMAT backBufferFormat );	// make our texture for RTT
	bool DeleteRenderTexture();
	void BlitTextureToScreen();											// and not the frame buffer
}

#endif


#if defined(__cplusplus)
}
#endif // __cplusplus

#endif // __D_DISPLAYPLATFORM_H
