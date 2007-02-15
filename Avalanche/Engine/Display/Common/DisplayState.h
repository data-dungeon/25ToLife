/*************************************/
/*                                   */
/*   d_DisplayState.h                */
/*   new engine  04/11/00            */
/*   ned martin  avalanche software  */
/*   visual c++ 6.0                  */
/*   display state and init          */
/*                                   */
/*************************************/

#ifndef __D_DISPLAYSTATE_H
#define __D_DISPLAYSTATE_H

/* system includes */

#ifdef DIRECTX
#include "Display/DirectX/DirectX.h"
#endif

#ifdef GCN
#include <dolphin.h>
#endif

/* engine includes */

#include "platform/BaseType.h"		// low-level include
#include "Layers/LayersStruct.h"		// low-level include
#include "Game/Misc/FastVars.h"

/******************* forward declarations ****************/

class CSceneView;	// in Display/Common/SceneView.h

/******************* defines *****************************/

/* these values serve two purposes. first, nInitStatus is set to NO_INIT in the record declaration. this can be checked
to make sure that the record declaration doesn't get out of synch with the structure definition (i.e. a new field is
added to the structure but not to the record). second, the macros to set the default values can only be called before
the engine initialization-- they have no effect afterwards */

#define DISPLAY_NO_INIT	0x12345678
#define DISPLAY_INIT		0x87654321

/* for ps2 renderer */

#define RENDERER_SPAD_ADDR			((u_char *) 0x70000000)
#define RENDERER_VU_BUFFER_SIZE	338

/******************* macros ******************************/

/******************* structures **************************/

#ifdef PS2
/* can register a game-specific routine to call during vblank (in juju, used to render loading screen) */

typedef void (*t_VSyncGameCallback)(void);
#endif //PS2

/* record that tracks display attributes. many values are filled in with default values for display initialization.
macros are provided above for editing them before initialization call. other values are for tracking things that get
set during initialization and maybe game play */

struct ts_DisplayState
{
	/* screen dimensions */

	int			nScreenWidth;
	int			nScreenHeight;
	int			nOrigScreenWidth;		// on pc, dimensions are set to client area size once window is created
	int			nOrigScreenHeight;

	/* scene view defaults */

	float			fNearClip;				// in meters
	float			fFarClip;				// in meters
	float			fFieldOfViewY;			// in degrees
	float			fNearFog;				// in meters
	float			fFarFog;					// in meters
	bool			bFog;						// fog on or off
	ts_bRGBA		FogColor;
	int			nFogMode;				// RS_FOGMODE_LINEAR, for example
	float			fFogDensity;			// 0.0 to 1.0
	ts_bRGBA		ClearColor;
	u32			u32ClearMode;			// combination of RS_COLOR_BUFFER_BIT and RS_DEPTH_BUFFER_BIT

	/* frame counter */

	u32			u32FrameCount;

	/* screen snap */

	int			nScreenSnapReduceFactor;	// 1 (default, no effect), 2, or 4, may want to reduce resolution of screen snap

	/* windows-specific */

#ifdef WIN32
	HWND 			hWnd;
	HDC			hDC;
	HGLRC			hRC;
	HWND 			hParentWnd;
	HINSTANCE	hInstance; 													  // system info passed to main()
	s32			(FAR PASCAL *winProc)( HWND, UINT, WPARAM, LPARAM); // system info passed to main()
	char 			*szClassName;
	u32			u32WindowState;
	u32			u32LastWinError;
	HICON			hIcon;
	bool			bDisplayNeeded;		// so tools like terraintoola can turn off display stuff
	bool			fullScreen;
#endif //WIN32

	 /* ps2-specific */

#ifdef PS2
	u32			u32ZBufferBPP; 				// Z buffer bits per pixel
	u32			u32FrameBufferBPP[2];		// Frame buffers bits per pixel
	u32			u32FrameBufferAddr[2];
	u32			u32FrontBufferIndex;			// which frame buffer is currently being displayed
	u32			u32ZBufferAddr;
	u32			u32ShadowAddr;
	u32			u32TextureOrigBaseAddr; 	// original calculated value that follows frame- and z-buffers
	u32			u32TextureBaseAddr;			// gets adjusted by downloading of persistant all-white texture
	u32			u32TextureAddr;				// current address for sending texture to vram
	u32			u32SubSet0TextureAddr;		// current address for sending subset0 palettes to vram
	u32			u32DynamicTextureAddr;		// start addr of dynamic texture slot in VRAM - TJC: now allocated from top of VRAM
	bool			bInterlaced;
	bool			bFrameMode; 					// when bInterlace==TRUE, bFrameMode==TRUE -> repeat frame each field, ==FALSE -> alternate odd/even frame each field
	bool			b2Circuit;						// Want to use two circuit mode
	bool			bNTSCMode;						// true -> NTSC, false -> PAL
	u32			u32FramePixelFormat[2];		// Don't touch this (calculated)
	u32			u32ZPixelFormat;				// Don't touch this (calculated)
	u32			u32MBstrength; 				// Motion blur strength
	u32			u32MBtimothyLeary;			// Motion blur trippiness
	u32			u32DisplayWidth;				// current display buffer width in pixels -- helps support downsampling
	u32			u32DisplayHeight;				// current display buffer width in pixels -- helps support downsampling
	u32			u32FrameBufferWidth;			// current frame buffer width in pixels-- helps support render-to-a-texture
	u32			u32FrameBufferHeight;		// current frame buffer height in pixels-- helps support render-to-a-texture
	u32			u32FrameBufferMagHoriz;		// for setting frame buffer display registers
	u32			u32FrameBufferMagVert;		// for setting frame buffer display registers
	u32			u32FrameBufferDeltaY;		// for setting frame buffer display registers
	t_VSyncGameCallback	VSyncGameCallback;			// a game-specific routine that can be called during vblank interrupt

#endif //PS2

	 /* directx-specific */

#ifdef DIRECTX
	LPDIRECT3D8				pD3D;
	LPDIRECT3DDEVICE8		pD3DDevice;
	u32						u32MBstrength; 				// Motion blur strength
	u32						u32MBtimothyLeary;			// Motion blur trippiness
   D3DFORMAT		backBufferFormat;
#endif

	 /* GameCube-specific */
#ifdef GCN
	u32						u32FIFOSize;
	void						*pFIFOBuffer;
	GXFifoObj				*pGXFIFO;
	GXRenderModeObj		*pGXRenderMode;
	u32						u32MBstrength; 				// Motion blur strength
	u32						u32MBtimothyLeary;			// Motion blur trippiness
#endif //GCN

	 /* init flag, also used to make sure record declaration doesn't get out of synch with structure definition */

	 u32			 u32InitStatus;
};

#ifdef PS2

/* semaphore to flag that frame buffer is ready for swapping. declared volatile so that a while (bSwapBuffers) ; line works */

extern volatile SCRATCHPAD(int, nNumVBlanks);
extern volatile bool bReadyForSwap;

#endif //PS2

/******************* global prototypes *******************/

bool Display__Init(void);
void Display__Shutdown(bool complete = true);

/* default setting routines */

void Display__SetScreenWidth(int nPixels);
void Display__SetScreenHeight(int nPixels);
void Display__SetSplitScreen(int nSplitScreen);	//1=single (no split), 2=two player, 4=four player.
void Display__ResetScreenDimensions(void);

/* default SceneView values */

void Display__SetNearClip(float fMeters);
void Display__SetFarClip(float fMeters);
void Display__SetFieldOfViewY(float fDegrees);
void Display__SetNearFog(float fMeters);
void Display__SetFarFog(float fMeters);
void Display__SetFog(bool bFog);
void Display__SetFogColor(ts_bRGBA FogColor);
void Display__SetFogMode(int nMode);
void Display__SetFogDensity(float fDensity);
void Display__SetClearColor(ts_bRGBA ClearColor);
void Display__SetClearMode(bool bClearColorBuffer);
float Display__GetNearClip(void);
float Display__GetFarClip(void);
float Display__GetFieldOfViewY(void);
float Display__GetNearFog(void);
float Display__GetFarFog(void);
float Display__GetFogDensity(void);
int Display__GetFogMode(void);
ts_bRGBA *Display__GetFogColor(void);
void Display__SetSceneViewToDefault(CSceneView *pSceneView);

u32 Display__GetClearMode( void);
ts_bRGBA *Display__GetClearColor( void);

#ifdef WIN32
void Display__SetWinInfo(HWND hParentWnd, HINSTANCE hInstance, s32 (FAR PASCAL *winProc)( HWND, UINT, WPARAM, LPARAM), int nCmdShow, HICON hIcon);
void Display__SetDisplayNeeded(bool bDisplayNeeded);
void Display__SetFullScreen(bool fullScreen);
#endif //WIN32

#ifdef PS2
/* The FrameBuffer, DisplayBuffer and ZBuffer formats are now specified in BITS PER PIXEL (16, 24, 32) are acceptable
	if 0 is passed, a reasonable default is used. */
void Display__SetFrameBufferBPP(u32 u32DrawBufferBPP, u32 u32DisplayBufferBPP);
void Display__SetZBufferBPP(u32 u32ZBufferBPP);
void Display__SetDisplayDimensions(u32 u32DisplayWidth, u32 u32DisplayHeight);
void Display__SetInterlace(bool bInterlaced);
void Display__SetFrameMode(bool bFrameMode);
void Display__Set2Circuit(bool b2Circuit);
void DisplayState__ResetTextureAddr(void);
void Display__ToggleVSyncInterruptHandler(void);
void Display__SetVSyncGameCallback(t_VSyncGameCallback VSyncGameCallback);

u32 Display__GetDisplayBufferBPP(void);
u32 Display__GetDrawBufferBPP(void);

int Display__GetRenderTargetWidth(void);
int Display__GetRenderTargetHeight(void);
#endif //PS2

#ifdef _XBOX
void Display__SetXboxOptions(int width = 640, int height = 480, int format = 0, int multisampletype = 0);
#endif

/* routines that act on current DisplayState values */

void Display__SetFrameBuffer(void);

/* data access routines */

int Display__GetScreenWidth(void);
int Display__GetScreenHeight(void);
void Display__SetNTSCMode(bool bNTSC);
bool Display__GetNTSCMode(void);
float Display__GetScreenAspectRatio();

#ifdef WIN32
HWND Display__GetWnd(void);
HDC Display__GetDC(void);
#endif //WIN32

#ifdef GCN
void Display_SetFIFOSize( u32 u32FIFOSize);
#endif //GCN

/******************* global variables ********************/

/* only give access to DisplayState record to files that explicitly request by defining DISPLAY_STATE_ACCESS. this should
only be done by files in the display library. the reason is that the ts_DisplayState structure has some compile pre-define
dependencies that may not be set the same for all libraries (for example, the Display library may have DIRECTX defined, but
the Layers library doesn't. this leads to compile errors as the compiler does things one way for one library and another way
for another. so, any access to the record from outside the Display library has to be done via access routines */

//#ifdef DISPLAY_STATE_ACCESS
extern ts_DisplayState DisplayState;
//#endif //DISPLAY_STATE_ACCESS

#include "Display/Common/DisplayState.hpp"

#endif // __D_DISPLAYSTATE_H

