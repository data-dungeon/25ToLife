/*************************************/
/*                                   */
/*   DisplayState.c                  */
/*   new engine  04/11/00            */
/*   ned martin  avalanche software  */
/*   visual c++ 6.0                  */
/*   display state and init          */
/*                                   */
/*************************************/

/******************* includes ****************************/

#include "Display/DisplayPCH.h"

#include "Layers/Timing.h"
#ifdef DIRECTX
#include "Display/DirectX/StripBufferPlatform.h"
#include "Display/DirectX/PatchHelper.h"
#endif



/******************* defines *****************************/

#ifdef WIN32

#ifdef DIRECTX
#include "Display/DirectX/DirectX.h"
#include "Display/DirectX/DXPublished.h"
#endif

#define DEFAULT_SCREEN_WIDTH			640
#define DEFAULT_SCREEN_HEIGHT			480

#elif defined(PS2)

#define DEFAULT_SCREEN_WIDTH			512
#define DEFAULT_SCREEN_HEIGHT			448

#elif defined(GCN)

#define DEFAULT_SCREEN_WIDTH			640
#define DEFAULT_SCREEN_HEIGHT			480

#endif //WIN32/PS2

#define DEFAULT_FIELD_OF_VIEW_Y		65.0f
#define DEFAULT_NEAR_FOG				50.0f
#define DEFAULT_FAR_FOG					90.0f
#define DEFAULT_FOG_ENABLE				TRUE
#define DEFAULT_FOG_MODE				RS_FOGMODE_LINEAR
#define DEFAULT_FOG_DENSITY			0.5f
#define DEFAULT_CLEAR_MODE				(RS_COLOR_BUFFER_BIT | RS_DEPTH_BUFFER_BIT)
#define DEFAULT_NEAR_CLIP				1.0f
#define DEFAULT_FAR_CLIP				(DEFAULT_FAR_FOG + 100.0f)	// if fogging is on, we get some benefit out of having the far clip plane substantially farther than far fog plane

/******************* macros ******************************/

/******************* structures **************************/

/******************* externs *****************************/

/******************* local variables *********************/

/******************* global variables ********************/

ts_DisplayState DisplayState = {

	/* screen size */

	DEFAULT_SCREEN_WIDTH,	// int		nScreenWidth;
	DEFAULT_SCREEN_HEIGHT,	// int		nScreenHeight;
	DEFAULT_SCREEN_WIDTH,	// int		nOrigScreenWidth;
	DEFAULT_SCREEN_HEIGHT,	// int		nOrigScreenHeight;

	/* scene view defaults */

	DEFAULT_NEAR_CLIP,		// float			fNearClip;				// in meters
	DEFAULT_FAR_CLIP,			// float			fFarClip;				// in meters
	DEFAULT_FIELD_OF_VIEW_Y,// float			fFieldOfViewY;			// in degrees
	DEFAULT_NEAR_FOG,			// float			fNearFog;				// in meters
	DEFAULT_FAR_FOG,			// float			fFarFog;					// in meters
	DEFAULT_FOG_ENABLE,		// bool			bFog;						// fog on or off
	{0, 0, 0, 0},				// ts_bRGBA		FogColor;
	DEFAULT_FOG_MODE,			// int			nFogMode;				// RS_FOGMODE_LINEAR, for example
	DEFAULT_FOG_DENSITY,		// float			fFogDensity;			// 0.0 to 1.0
	{0, 0, 0, 0},				// ts_bRGBA		ClearColor;
	DEFAULT_CLEAR_MODE,		// u32			u32ClearMode;			// combination of RS_COLOR_BUFFER_BIT and RS_DEPTH_BUFFER_BIT

	/* frame counter */

	0, 							// u32		u32FrameCount;

	/* screen snap */

	1,								// int			nScreenSnapReduceFactor;	// 1 (default, no effect), 2, or 4, may want to reduce resolution of screen snap

	/* windows-specific */

#ifdef WIN32
	NULL, 						// HWND			 hWnd;
	NULL, 						// HDC			 hDC;
	NULL, 						// HGLRC 		 hRC;
	NULL, 						// HWND			 hParentWnd;
	NULL, 						// HINSTANCE	 hInstance;
	NULL, 						// s32 (FAR PASCAL *winProc)( HWND, UINT, WPARAM, LPARAM);
#ifdef DIRECTX_PC
	"25 To Life",				// char			 *szClassName;
#else
	"New Engine",				// char			 *szClassName;
#endif // DIRECTX_PC
#ifdef _XBOX
	0, 							// u32			 u32WindowState;
#else
	SW_SHOWNORMAL, 			// u32			 u32WindowState;
#endif
	0, 							// u32			 u32LastWinError;
	NULL, 						// HICON 		 hIcon;
	TRUE,							// bool			bDisplayNeeded;
	TRUE,							// bool			fullScreen;
#endif //WIN32

	 /* ps2-specific */

#ifdef PS2
	0, 							// u32			u32ZBufferBytesPerPixel;
	{0, 0},						// u32			u32FrameBufferBytesPerPixel[2];
	{0, 0},						// u32			u32FrameBufferAddr[2];
	0, 							// u32			u32FrontBufferIndex;
	0, 							// u32			u32ZBufferAddr;
	0,								// u32			u32ShadowAddr;
	0, 							// u32			u32TextureOrigBaseAddr;
	0, 							// u32			u32TextureBaseAddr;
	0, 							// u32			u32TextureAddr;
	0,								// u32			u32SubSet0TextureAddr;
	VRAM_NUMBER_OF_BLOCKS,	// u32DynamicTextureAddr; top of vram.. 4MB / size of a block.
	TRUE, 						// bool			bInterlaced;
	FALSE,						// bool			bFrameMode;
	TRUE, 						// bool			b2Circuit
	TRUE, 						// bool			bNTSCMode
	{0, 0},						// u32			u32FramePixelFormat[2]
	0, 							// u32			u32ZPixelFormat
	0, 							// u32			MB strength
	0, 							// u32			MB trippiness
	0,								// u32			u32DisplayWidth;
	0,								// u32			u32DisplayHeight;
	0, 							// u32			u32FrameBufferWidth;
	0, 							// u32			u32FrameBufferHeight;
	0, 							// u32			u32FrameBufferMagHoriz;
	0, 							// u32			u32FrameBufferMagVert;
	0, 							// u32			u32FrameBufferDeltaY;
	NULL,							// t_VSyncGameCallback	VSyncGameCallback;
#endif //PS2

	 /* directx-specific */

#ifdef DIRECTX
	NULL, 						// LPDIRECT3D8			 pD3D;
	NULL, 						// LPDIRECT3DDEVICE8	 pD3DDevice;
	0, 							// u32			MB strength
	0, 							// u32			MB trippiness
	D3DFMT_X8R8G8B8,			// D3DFORMAT	backBufferFormat
#endif

	 /* GameCube-specific */
#ifdef GCN
	( 640 * 1024),				// u32						u32FIFOSize;
	NULL,							// void						*pFIFOBuffer;
	NULL,							// GXFifoObj				*pGXFIFO;
	NULL,							// GXRenderModeObj		*pGXRenderMode;
	0, 							// u32			MB strength
	0, 							// u32			MB trippiness
#endif //GCN

	 /* init status, and to make sure we don't get out of synch with structure definition */

	DISPLAY_NO_INIT			// u32  u32InitStatus;
};

#ifdef PS2

/* semaphore to flag that frame buffer is ready for swapping. declared volatile so that a while (bSwapBuffers) ; line works */

//volatile SCRATCHPAD(int, nNumVBlanks);
volatile bool bReadyForSwap = false;

#endif //PS2

/******************* local prototypes ********************/

#ifdef PS2
static void VSyncCallback( void );

/* handle to our vsync handler (the movie player needs to disable this temporarily) */
static int VSyncCallbackHandle = -1;

// handle gs memory management
GSMemManager gsMemMgr;

#endif //PS2

/******************* functions ***************************/

/* initialize display stuff. if you want to change any of the default values, use the macros provided in d_DisplayState.h before
calling GameEngine__Init (which calls this routine) */

bool Display__Init(void)
{
#if defined (WIN32) && !defined (_XBOX)
#ifndef CONSUMER_BUILD
	/* this assert is intended to make sure that this module is compiled with the same MMX_OK setting as the math */
	/* module. the mmx instructions require some alignment of matrices and vectors (__declspec(align(SIMD_ALIGN))). */
	/* if one module assumes alignment and another doesn't, all hell breaks loose. all modules that use math must */
	/* be compiled with same MMX_OK setting. the MMX_OK setting is done through project->settings->c/c++: the */
	/* preprocessor definitions should include MMX_OK=$(MMX_OK), and you should have the environment variable MMX_OK */
	/* set to 1 if your machine can handle SIMD, and 0 if not (older athlons) */

	ASSERT(Math::MMXOK() == MMX_OK);
#endif //CONSUMER_BUILD
#endif //WIN32

	/* make sure that our declaration of DisplayState values matches struct definition */

// TJC - Nate told me to comment this out... so if you want it in there, feel free to fix his hack...
//	ASSERT(DisplayState.u32InitStatus == DISPLAY_NO_INIT);	// if this hits, declaration doesn't match definition (not enough values filled in)
	DisplayState.u32InitStatus = DISPLAY_INIT;

#ifdef WIN32
	/* display may not be needed (for example, for tools like terraintoola) */

	if (DisplayState.bDisplayNeeded == FALSE)
		return(TRUE);
#endif //WIN32

#ifdef PS2

//	nNumVBlanks = 0;

	// Get memory for frame buffers & z buffer
	gsMemMgr.Reset();
	
	/* first frame buffer is at start of VRAM */
	GSMemManager::allocationInfo gsAllocInfo;

	/* calculate frame buffer size */

	switch(DisplayState.u32FrameBufferBPP[0])
	{
	default:
		DisplayState.u32FrameBufferBPP[0] = 16;
 
		/* and fall through to 16-bit case */
	case 16:
		DisplayState.u32FramePixelFormat[0] = SCE_GS_PSMCT16S;
		break;
	case 24:
		DisplayState.u32FramePixelFormat[0] = SCE_GS_PSMCT24;
		break;
	case 32:
		DisplayState.u32FramePixelFormat[0] = SCE_GS_PSMCT32;
		break;
	}

	switch(DisplayState.u32FrameBufferBPP[1])
	{
	default:
		DisplayState.u32FrameBufferBPP[1] = 16;

		/* and fall through to 16-bit case */
	case 16:
		DisplayState.u32FramePixelFormat[1] = SCE_GS_PSMCT16S;
		break;
	case 24:
		DisplayState.u32FramePixelFormat[1] = SCE_GS_PSMCT24;
		break;
	case 32:
		DisplayState.u32FramePixelFormat[1] = SCE_GS_PSMCT32;
		break;
	}

	/* calculate z-buffer size */
	switch(DisplayState.u32ZBufferBPP)
	{
	default:
		DisplayState.u32ZBufferBPP = 32;

		/* and fall through to 32-bit case */
	case 32:
		DisplayState.u32ZPixelFormat = SCE_GS_PSMZ32;
		break;
	case 24:
		DisplayState.u32ZPixelFormat = SCE_GS_PSMZ24;
		break;
	case 16:
		DisplayState.u32ZPixelFormat = SCE_GS_PSMZ16S;
		break;
	}

	gsMemMgr.GetAllocationInfo(DisplayState.u32DisplayWidth, 
										DisplayState.u32DisplayHeight,
										DisplayState.u32DisplayWidth,
										DisplayState.u32FramePixelFormat[0], 
										true, 
										gsAllocInfo);
	DisplayState.u32FrameBufferAddr[0] = gsMemMgr.BlockAlloc(gsAllocInfo);

	gsMemMgr.GetAllocationInfo(DisplayState.nScreenWidth, 
										DisplayState.nScreenHeight,
										DisplayState.nScreenWidth, 
										DisplayState.u32FramePixelFormat[1], 
										true, 
										gsAllocInfo);
	DisplayState.u32FrameBufferAddr[1] = gsMemMgr.BlockAlloc(gsAllocInfo);

	gsMemMgr.GetAllocationInfo(DisplayState.nScreenWidth, 
										DisplayState.nScreenHeight, 
										DisplayState.nScreenWidth, 
										DisplayState.u32ZPixelFormat,
										true, 
										gsAllocInfo);
	DisplayState.u32ZBufferAddr = gsMemMgr.BlockAlloc(gsAllocInfo);

	// fixed shadow buffer
	gsMemMgr.GetAllocationInfo(128, 128, 128, SCE_GS_PSMCT16, true, gsAllocInfo);
	DisplayState.u32ShadowAddr = gsMemMgr.BlockAlloc(gsAllocInfo);

	/* set texture starting point (in vram words). texture starting address has a looser alignment than frame */
	/* and z-buffers, so we're guaranteed to be aligned here */

	DisplayState.u32TextureOrigBaseAddr = gsMemMgr.GetHighestBlockUsed() + 1;	// start at the first free block
	DisplayState.u32TextureBaseAddr = DisplayState.u32TextureOrigBaseAddr;
	DisplayState.u32TextureAddr = DisplayState.u32TextureOrigBaseAddr;

	/* set up display parameters. first magnification */

	switch (DisplayState.u32DisplayWidth)
	{
   case 384:
		DisplayState.u32FrameBufferMagHoriz = 6;
      break;
   case 512:
		DisplayState.u32FrameBufferMagHoriz = 4;
      break;
	case 640:
   default:
		DisplayState.u32FrameBufferMagHoriz = 3;
      break;
   }

	/* for some reason there are constant offsets added in eestruct.h, so DeltaY has to account for them */

	if (DisplayState.u32DisplayHeight == 224)
	{
		if (DisplayState.bInterlaced == FALSE)
		{
			/* non-interlaced, don't care about frame/field mode */

			DisplayState.u32FrameBufferDeltaY = (u32)-25;		/* constant 25 in eestruct.h (?) */
			DisplayState.u32FrameBufferMagVert = 0;
		}
		else if (DisplayState.bFrameMode)
		{
			/* interlaced, frame mode */

			DisplayState.u32FrameBufferDeltaY = 0;
			DisplayState.u32FrameBufferMagVert = 0;
		}
		else
		{
			/* interlaced, field mode */

			DisplayState.u32FrameBufferDeltaY = 0;
			DisplayState.u32FrameBufferMagVert = 1;
		}
	}
	else
	{
		/* screen height other than 224 */
		DisplayState.u32FrameBufferDeltaY = (!DisplayState.bNTSCMode) ? 25 : 0;	//djr: to letterbox PAL mode, move the display down  a little
		DisplayState.u32FrameBufferMagVert = 0;
	}

#endif //PS2

	/* do platform-specific graphics initialization(e.g. for pc, create a window) */

	if (Display__InitGraphics() == FALSE)
		return FALSE;

	/* setting for tesselator */

//yuch-- obsolete-- removed SplitPatch.cpp from engine 6/13/02
//	SetNegativeNearClip(DisplayState.fNearClip);

	/* build default viewport matrix (must be done after determining z-buffer format) */

	MatrixStack__InitViewportMatrix();

#ifndef CONSUMER_BUILD
	/* output debug info about graphics initialization */

	gfxDebug( );
#endif //CONSUMER_BUILD

	/* these gl states are not in the RenderState state machine, and they never change. if we want to */
	/* be able to change them during runtime, add them to the state machine */

#if defined(PS2)

	/* no dynamic texture slot yet */

	DisplayState.u32DynamicTextureAddr = (VRAM_TOTAL_BYTES / VRAM_BYTES_PER_BLOCK); // top of vram.. 4MB / size of a block.
//	DisplayState.u32AfterDynamicAddr = 0;

	/* set scissor region */
	GSState.SCISSOR_1 = SCE_GS_SET_SCISSOR(0, DisplayState.nScreenWidth, 0, DisplayState.nScreenHeight);
	GSState__Set(SCE_GS_SCISSOR_1, GSState.SCISSOR_1);
	
	/* register a vblank callback for swapping frame buffers when ready */

//	DisableIntc(INTC_VBLANK_S);
//	VSyncCallbackHandle = AddIntcHandler(INTC_VBLANK_S, VSyncCallback, 0);
//	EnableIntc(INTC_VBLANK_S);
	MasterList::SetVBlankInterruptCallback(VSyncCallback);

#endif PS2

	/* init the render state manager */

	RenderState__Init();

	MatrixStack__FullScreenViewport();

	MatrixStack__Mode( MS_PROJECTION );
	MatrixStack__Identity();

#ifdef DIRECTX
    // Setup default DirectX state.
    IDirect3DDevice8_SetRenderState( DirectX_Device, D3DRS_LIGHTING, FALSE );
    IDirect3DDevice8_SetRenderState( DirectX_Device, D3DRS_CULLMODE, D3DCULL_CW );
#endif

#ifdef PS2
	GSState__Send();
#endif //PS2

	/* success */

	return(TRUE);
}

void Display__Shutdown(bool complete /* = true */)
{
	ASSERT(DisplayState.u32InitStatus == DISPLAY_INIT);	// if this hits, it means Display__Init was never called by GameEngine__Init

#ifdef PS2

	sceGsSyncPath(0, 0);

	// TJC - remove the intc handler
	MasterList::SetVBlankInterruptCallback(NULL);
//	if(VSyncCallbackHandle != -1)
//	{
//		DisableIntc(INTC_VBLANK_S);
//		RemoveIntcHandler(INTC_VBLANK_S, VSyncCallbackHandle);
//		VSyncCallbackHandle = -1;
//		EnableIntc(INTC_VBLANK_S);
//	}

	if(complete)
	{
		PS2Renderer::Cleanup();
	}
	MasterList::Cleanup();
#endif

#ifdef DIRECTX
	StripBufferPlatformData::deleteDeviceObjects( );
	VtxBufferPlatformData::deleteDeviceObjects( );
	DXPublished::releaseDXDevice(  );
#endif

	DisplayState.u32InitStatus = DISPLAY_NO_INIT;

}

#ifdef PS2
/* we have the need to disable our engine interrupt handler (e.g., when a movie is playing) */

void Display__ToggleVSyncInterruptHandler(void)
{
	if (MasterList::GetVBlankInterruptCallback() == NULL)
	{
		MasterList::SetVBlankInterruptCallback(VSyncCallback);
	}
	else
	{
		MasterList::SetVBlankInterruptCallback(NULL);
	}
}

/* register a game-specific routine to be called from the vblank interrupt */

void Display__SetVSyncGameCallback(t_VSyncGameCallback VSyncGameCallback)
{
	DisplayState.VSyncGameCallback = VSyncGameCallback;
}

#endif

/* routines to update default initialization values before init is called */

void Display__SetScreenWidth(int nPixels)
{
	ASSERT(DisplayState.u32InitStatus == DISPLAY_NO_INIT);	// if this hits, then Display__Init has already been called, and the update has no effect
	DisplayState.nScreenWidth = nPixels;
}
void Display__SetScreenHeight(int nPixels)
{
	ASSERT(DisplayState.u32InitStatus == DISPLAY_NO_INIT);	// if this hits, then Display__Init has already been called, and the update has no effect
	DisplayState.nScreenHeight = nPixels;
}

#ifdef WIN32
void Display__SetWinInfo(HWND hParentWnd, HINSTANCE hInstance, s32 (FAR PASCAL *winProc)( HWND, UINT, WPARAM, LPARAM), int nCmdShow, HICON hIcon)
{
	ASSERT(DisplayState.u32InitStatus == DISPLAY_NO_INIT);	// if this hits, then Display__Init has already been called, and the update has no effect

	/* set the things that are passed to WinMain as system info */

	DisplayState.hParentWnd = hParentWnd;
	DisplayState.hInstance = hInstance;
	DisplayState.winProc = winProc;
	DisplayState.u32WindowState = nCmdShow;
	DisplayState.hIcon = hIcon;
}

/* this is so tools like terraintoola can turn off display, skip initialization. one issue is that since display
initialization assumes the existence of the nvidia drivers (needed for gl extensions), tools that call Display__Init
can't run on machines that don;t have the nvidia drivers, even though the tool does not use the display code */

void Display__SetDisplayNeeded(bool bNeeded)
{
	DisplayState.bDisplayNeeded = bNeeded;
}
void Display__SetFullScreen(bool fullScreen)
{
	DisplayState.fullScreen = fullScreen;
	#ifdef DIRECTX
	DirectX__Fullscreen(fullScreen);
	#endif
}
#endif //WIN32

#ifdef PS2
void Display__SetDisplayDimensions(u32 u32DisplayWidth, u32 u32DisplayHeight)
{
	DisplayState.u32DisplayWidth = u32DisplayWidth;
	DisplayState.u32DisplayHeight = u32DisplayHeight;
}

void Display__SetFrameBufferBPP(u32 u32DrawBufferBPP, u32 u32DisplayBufferBPP)
{
	ASSERT(DisplayState.u32InitStatus == DISPLAY_NO_INIT);	// if this hits, then Display__Init has already been called, and the update has no effect
	DisplayState.u32FrameBufferBPP[0] = u32DisplayBufferBPP ? u32DisplayBufferBPP : u32DrawBufferBPP;
	DisplayState.u32FrameBufferBPP[1] = u32DrawBufferBPP;
}
void Display__SetZBufferBPP(u32 u32ZBufferBPP)
{
	ASSERT(DisplayState.u32InitStatus == DISPLAY_NO_INIT);	// if this hits, then Display__Init has already been called, and the update has no effect
	DisplayState.u32ZBufferBPP = u32ZBufferBPP;
}
u32 Display__GetDisplayBufferBPP(void)
{
	// TJC - technically, if both buffers are identical, then both are
	// display buffers, but this one is the display buffer if they are
	// different. 
	return DisplayState.u32FrameBufferBPP[0];
}
u32 Display__GetDrawBufferBPP(void)
{
	// TJC - technically, if both buffers are identical, then both are
	// draw buffers, but this one is the draw buffer if they are
	// different.
	return DisplayState.u32FrameBufferBPP[1];
}
void Display__SetInterlace(bool bInterlaced)
{
	ASSERT(DisplayState.u32InitStatus == DISPLAY_NO_INIT);	// if this hits, then Display__Init has already been called, and the update has no effect
	DisplayState.bInterlaced = bInterlaced;
}
void Display__SetFrameMode(bool bFrameMode)
{
	ASSERT(DisplayState.u32InitStatus == DISPLAY_NO_INIT);	// if this hits, then Display__Init has already been called, and the update has no effect
	DisplayState.bFrameMode = bFrameMode;
}
void Display__Set2Circuit(bool b2Circuit)
{
	ASSERT(DisplayState.u32InitStatus == DISPLAY_NO_INIT);	// if this hits, then Display__Init has already been called, and the update has no effect
	DisplayState.b2Circuit = b2Circuit;
}

/* reset vram address for ps2 */

void DisplayState__ResetTextureAddr(void)
{
	DisplayState.u32TextureAddr = DisplayState.u32TextureBaseAddr;

	/* no dynamic texture slot yet */

	DisplayState.u32DynamicTextureAddr = (VRAM_TOTAL_BYTES / VRAM_BYTES_PER_BLOCK); // top of vram.. 4MB / size of a block.
//	DisplayState.u32AfterDynamicAddr = 0;
}

/* vblank callback routine-- swap frame buffer if ready */

static void VSyncCallback( void )
{
//NumVBlanks++;

	/* keep timer honest during long file loads */

	EngineTimer__CheckWrap();

	/* maybe the game has requested a callback (e.g. to render loading screen) */

	if (DisplayState.VSyncGameCallback)
		DisplayState.VSyncGameCallback();

	/* clean exit */

   return;
}

#endif //PS2

#ifdef GCN
void Display_SetFIFOSize( u32 u32FIFOSize)
{
	ASSERT( ( u32FIFOSize & ( ~31)) == 0);
	DisplayState.u32FIFOSize = u32FIFOSize;
}
#endif

/* setup the frame buffer for rendering the scene(s)/sprites/particles-- the data that will displayed */

void Display__SetFrameBuffer(void)
{
#ifdef PS2
	/* set the target frame buffer to render into */

	if(Display__DownSampling())
	{
		// if downsampling, always fix to the same buffer...
		gfxSetFrameBuffer(
			DisplayState.u32FrameBufferAddr[1], 
			DisplayState.nScreenWidth, 
			DisplayState.nScreenHeight, 
			DisplayState.u32FramePixelFormat[1]);
	}
	else
		gfxSetFrameBuffer(DisplayState.u32FrameBufferAddr[DisplayState.u32FrontBufferIndex], DisplayState.nScreenWidth, DisplayState.nScreenHeight, DisplayState.u32FramePixelFormat[DisplayState.u32FrontBufferIndex]);

	/* flush gs commands */

	GSState__Send();
#endif //PS2
}

#ifdef _XBOX
void Display__SetXboxOptions(int width, int height, int format, int multisampletype)
{
	DirectX__Options(width, height, format, multisampletype);
}
#endif

void Display__SetNTSCMode(bool bNTSC)
{
#ifdef PS2
	ASSERT(DisplayState.u32InitStatus == DISPLAY_NO_INIT);	// if this hits, then Display__Init has already been called, and the update has no effect
	DisplayState.bNTSCMode = bNTSC;
#endif
}

/* this routine is here for pigpen to use-- what happens is that in a windows build, u32ScreenWidth and u32ScreenHeight
get set to the actual client area of the window, not the size of the window itself. so if you start up the game engine
multiple times, the window size changes each time */

void Display__ResetScreenDimensions(void)
{
	/* refresh working screen dimensions */

   DisplayState.nScreenWidth = DisplayState.nOrigScreenWidth;
   DisplayState.nScreenHeight = DisplayState.nOrigScreenHeight;
}

/* set default values for scene view associated with each scene */

void Display__SetNearClip(float fMeters)
{
	DisplayState.fNearClip = fMeters;
}
void Display__SetFarClip(float fMeters)
{
	DisplayState.fFarClip = fMeters;
}
void Display__SetFieldOfViewY(float fDegrees)
{
	DisplayState.fFieldOfViewY = fDegrees;
}
void Display__SetNearFog(float fMeters)
{
	DisplayState.fNearFog = fMeters;
}
void Display__SetFarFog(float fMeters)
{
	DisplayState.fFarFog = fMeters;
}
void Display__SetFog(bool bFog)
{
	DisplayState.bFog = bFog;
}
void Display__SetFogColor(ts_bRGBA FogColor)
{
	DisplayState.FogColor = FogColor;
}
void Display__SetFogMode(int nMode)
{
	ASSERT((nMode == RS_FOGMODE_LINEAR) ||
			 (nMode == RS_FOGMODE_EXP) ||
			 (nMode == RS_FOGMODE_EXP2) ||
			 (nMode == RS_FOGMODE_NONE));
	DisplayState.nFogMode = nMode;
}
void Display__SetFogDensity(float fDensity)
{
	ASSERT((fDensity >= 0.0f) && (fDensity <= 1.0f));
	DisplayState.fFogDensity = fDensity;
}
void Display__SetClearColor(ts_bRGBA ClearColor)
{
	DisplayState.ClearColor = ClearColor;
}
void Display__SetClearMode(bool bClearColorBuffer)
{
	if (bClearColorBuffer)
		DisplayState.u32ClearMode = RS_DEPTH_BUFFER_BIT | RS_COLOR_BUFFER_BIT;
	else
		DisplayState.u32ClearMode = RS_DEPTH_BUFFER_BIT;
}


/* set passed SceneView record to default values in DisplayState. this is called by the scene manager when
a new scene is registered-- it starts off with the default scene view values, which are typically filled in by
processing the light.var file */

void Display__SetSceneViewToDefault(CSceneView *pSceneView)
{
	memset(pSceneView, 0, sizeof(CSceneView));

	/* copy in values */
		
	pSceneView->SetNearClip(DisplayState.fNearClip);
	pSceneView->SetFarClip(DisplayState.fFarClip);
	pSceneView->SetFieldOfViewY(DisplayState.fFieldOfViewY);
	pSceneView->SetNearFog(DisplayState.fNearFog);
	pSceneView->SetFarFog(DisplayState.fFarFog);
	pSceneView->SetFog(DisplayState.bFog);
	pSceneView->SetFogColor(DisplayState.FogColor);
	pSceneView->SetFogMode(DisplayState.nFogMode);
	pSceneView->SetFogDensity(DisplayState.fFogDensity);
	pSceneView->SetClearColor(DisplayState.ClearColor);
	pSceneView->SetClearMode((DisplayState.u32ClearMode & RS_COLOR_BUFFER_BIT) != 0);
	
	/* set to default viewport-- entire frame buffer */

	pSceneView->SetViewport(0.0f, 0.0f, 1.0f, 1.0f);
}

/* calculate the aspect ratio of entire frame buffer */

float Display__GetScreenAspectRatio()
{
	/* for game platforms, aspect ratio is always 4/3 (aspect ratio of tv screen). for pc running in a window, */
	/* aspect ratio is a bit more complicated */

	float fAspectRatio = 4.0f / 3.0f;

#if defined ( WIN32 ) && ! defined ( _XBOX )
	/* pc may be running in a window. this calculation isn't quite accurate. it works as long as we are in a display */
	/* resolution where the screen pixel dimension aspect ratio is 4/3 (e.g. 640 x 480, 800 x 600, 1280 x 724, */
	/* 1600 x 1200) which is most of the time. but if we are in a non-4/3 mode, the accurate calculation would be: */
	/*																																		 */
	/*    aspect ratio = (4.0f / 3.0f) * ((window width / screen width) / (window height / screen height)) */

	if (DisplayState.u32WindowState != SW_SHOWMAXIMIZED)
		fAspectRatio = (float) DisplayState.nScreenWidth / (float) (DisplayState.nScreenHeight ? DisplayState.nScreenHeight : 1);
#endif //WIN32

#if defined (_XBOX)
	if (DirectX_PresentParams.Flags & D3DPRESENTFLAG_WIDESCREEN)
		fAspectRatio = 16.0f / 9.0f;
#endif

	return(fAspectRatio);
}

// TJC - because of the way we cheat with PS2 builds, these resolve warnings.
#undef DEFAULT_FAR_CLIP
#undef DEFAULT_NEAR_FOG
#undef DEFAULT_FAR_FOG
