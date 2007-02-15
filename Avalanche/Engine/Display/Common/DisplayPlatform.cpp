/*************************************/
/*                                   */
/*   DisplayPlatform.c               */
/*   new engine  04/11/00            */
/*   ned martin  avalanche software  */
/*   visual c++ 6.0                  */
/*   platform-specific display stuff */
/*                                   */
/*************************************/

/* this file contains display routines that are specific to the current platform, but not specific to a display subsystem
(like OpenGL). or at least mostly non-specific */

/******************* includes ****************************/

#include "Display/DisplayPCH.h"

#ifdef PS2
#include "Display/PS2/GSTricks.h"
#endif

#ifdef DIRECTX
#include "Display/DirectX/DirectX.h"
#include "Display/DirectX/DXPublished.h"
#include "Display/DirectX/StripBufferPlatform.h"
#include "Display/DirectX/PatchHelper.h"

#ifdef DIRECTX_PC
// I think we want to initialize this variable to false. yes/no?
// I ran into a place where I wanted to do some work that relied
// on the D3D device being available, but it hadn't been initialized
// and this var was still set to true.  Just a heads up
bool		bDeviceAvailable = false;
#else
bool		bDeviceAvailable = true;
#endif

// ATMon
extern const char *g_RemoteIPAddress;

#endif

#include "Layers/Timing.h"
#include "Layers/Media.h"
#include "Layers/DiscErrorHandler/DiscError.h"

#ifdef GCN
#include "Display/GCN/FrameData.h"
#ifdef DVDETH
#include "EngineHelper/filestream.h"
#endif
#endif

#if defined (_XBOX)
#include "platform/Xbox/xboxapp.h"
#include "Display/TextureUtil.h"
#include "Layers/FileCache/cache_xbox.h"
#include "platform/Xbox/resident_font.cpp"
#include "platform/Xbox/XboxLive.h"
#include <UIX.h>

#pragma pack(push, 8)       // Make sure structure packing is set properly
#include <d3d8perf.h>
#pragma pack(pop)

#endif

/******************* defines *****************************/

#ifdef PS2
#define DOWNSAMPLE_PACKET_SIZE	15
#endif

#if defined (_XBOX) || defined (DIRECTX_PC)
#define GAMMA_CORRECTION_ON

#endif // XBOX


/******************* macros ******************************/

/******************* structures **************************/

/******************* externs *****************************/

//yuch-- in joe's gfxInit.cpp
void 					gfxInit( void );
void 					gfxSwap( void );

/******************* global variables ********************/

/******************* local variables *********************/

bool bSnapScreen = FALSE;

#ifdef GCN
#define GCN_NUM_XFBS		1
static void					*apXFB[GCN_NUM_XFBS];
static void					*g_pClearFrameCopyBuffer;
static void					*g_pNextFrameBuffer = NULL;
static u8					g_CurrDrawBuffer;
static u16					g_CurrDrawToken;
static volatile u32		u32GCNTimeout = 0;
static volatile u32		u32GCNVBlankCount;
Graphics4x4					mtxEnvBase;
CFrameData					*g_pPrevFrameData = NULL,
								*g_pFrameData = NULL;
static CFrameData			*g_apFrameData[2];

u16							lastToken = 0;
#endif

#ifdef PS2
static u_long128 u128GfxDownSamplePacket[DOWNSAMPLE_PACKET_SIZE];
// we use sCurrentFramesDisplayState instead of DisplayState in 
// some PS2 functions because they happen during interrupts which
// can occur during RenderToTexture stuff, which temporarily modifies
// DisplayState so that stuff like the MatrixStack code works as
// is without having to know about rendering to a texture.
static ts_DisplayState sCurrentFramesDisplayState;
#endif

/******************* local prototypes ********************/

#if defined (WIN32) && !defined (_XBOX)
static HWND Display__CreateWindow(void);
static void Display__UpdateWindow(void);
static bool gfxSetPixelFormat(HDC hDC);
#endif //WIN32
static void _gfxSnapScreen(void);

#ifdef GCN
static void		gcnPreRetraceCallback( u32 u32RetraceCount);
static void		gcnVerifyCallback( GXWarningLevel level, u32 id, const char *msg);
static void		__DEMODiagnoseHang( void);
static void		DEMOSetGPHangMetric( GXBool enable);
static void		gxDrawSyncCallback( u16 token);
#endif

#ifdef PS2
static void gfxRenderCompleted(void);
#endif

/******************* functions ***************************/

/* install a function to be called when rendering has stalled for some reason */
void Display__SetIdleCallback(void (*callback)(void))
{
	// Only the PS2 stalls.
#ifdef PS2
	MasterList::SetDMAWaitCallback(callback);
#endif
}

/* switch to the selected video mode, etc... */
/* gets called at the end of movies just incase they changed the resolution */
void Display__ResetGraphics( void )
{
#ifdef PS2
	/* set up graphics mode. in interlace, you can specify this half-offset thing to get apparent double vertical */
	/* resolution. this technique only works if you are running at 60hz and can supply the CRTC with valid */
	/* buffers for even and odd fields. that is, the CRTC polls the GS at 60hz, alternately receiving data for the
		/* even and odd fields. if the frame buffer is updating at 30hz, then the CRTC receives the same data for both */
	/* even and odd fields, and there's no gain (except for some interlace flicker!) */

	sceGsResetGraph(0, DisplayState.bInterlaced ? SCE_GS_INTERLACE : SCE_GS_NOINTERLACE, DisplayState.bNTSCMode ? SCE_GS_NTSC : SCE_GS_PAL, DisplayState.bFrameMode ? SCE_GS_FRAME : SCE_GS_FIELD);

	/* reverse engineered from sony libs */

//yuch-- necessary?
//		SetGsCrt(DisplayState.bInterlaced, DisplayState.bNTSCMode ? SCE_GS_NTSC : SCE_GS_PAL, DisplayState.bFrameMode);

	/* wait for gs to say ready */

//yuch-- necessary?
//		sceGsSyncPath(0,0);

	/* set global display settings (don't change frame-to-frame) */

	/* background color */

	DPUT_GS_BGCOLOR(0x00000000);

	/* build downsample packet */

	u8* downSamplePacket = (u8*)u128GfxDownSamplePacket;
	u32 destAddr = DisplayState.u32FrameBufferAddr[0];
	u32 destFormat = DisplayState.u32FramePixelFormat[0];
	u32 destWidth = DisplayState.u32DisplayWidth;
	u32 destHeight = DisplayState.u32DisplayHeight;
	u32 srcAddr = DisplayState.u32FrameBufferAddr[1];
	u32 srcFormat = DisplayState.u32FramePixelFormat[1];
	u32 srcWidth = DisplayState.nScreenWidth;
	u32 srcHeight = DisplayState.nScreenHeight;
	u32 u32X = 2048 - (DisplayState.u32DisplayWidth>>1);
	u32 u32Y = 2048 - (DisplayState.u32DisplayHeight>>1);

	GIFTag tag(DOWNSAMPLE_PACKET_SIZE - 1,
				  true,
				  true,
				  SCE_GS_SET_PRIM(SCE_GS_PRIM_SPRITE, 0, 1, 0, 0, 0, 1, 0, 0),
				  0,
				  1,
				  0xE,
				  0x0);

	MasterList::Append16(downSamplePacket, *(u128*)&tag);

	MasterList::Append8(downSamplePacket, SCE_GS_SET_DTHE(1));
	MasterList::Append8(downSamplePacket, SCE_GS_DTHE);

	MasterList::Append8(downSamplePacket, GSState.DIMX);
	MasterList::Append8(downSamplePacket, SCE_GS_DIMX);

	MasterList::Append8(downSamplePacket, SCE_GS_SET_FRAME_1(destAddr/32, destWidth/64, destFormat, 0));
	MasterList::Append8(downSamplePacket, SCE_GS_FRAME_1 );

	MasterList::Append8(downSamplePacket, SCE_GS_SET_SCISSOR_1(0, destWidth, 0, destHeight));
	MasterList::Append8(downSamplePacket, SCE_GS_SCISSOR_1 );

	MasterList::Append8(downSamplePacket, SCE_GS_SET_TEST_1(0, 0, 0, 0, 0, 0, 1, 1));
	MasterList::Append8(downSamplePacket, SCE_GS_TEST_1 );

	MasterList::Append8(downSamplePacket, SCE_GS_SET_ALPHA_1(0, 2, 2, 2, 128));
	MasterList::Append8(downSamplePacket, SCE_GS_ALPHA_1);

	MasterList::Append8(downSamplePacket, SCE_GS_SET_TEX0_1(srcAddr, srcWidth/64, srcFormat, 10, 9, 0, 1, 0, 0, 0, 0, 0));
	MasterList::Append8(downSamplePacket, SCE_GS_TEX0_1 );

	MasterList::Append8(downSamplePacket, SCE_GS_SET_XYOFFSET((2048 - (destWidth / 2)) << 4,		// ofx (12.4 fixed point)
																				 (2048 - (destHeight / 2)) << 4));	// ofy (12.4 fixed point)
	MasterList::Append8(downSamplePacket, SCE_GS_XYOFFSET_1);

	// TJC - no zbuffer writes while we do this...
	MasterList::Append8(downSamplePacket, 1L << GS_ZBUF_ZMSK_O);
	MasterList::Append8(downSamplePacket, SCE_GS_ZBUF_1);

	// TJC - go to point sampling instead of bilerp
	MasterList::Append8(downSamplePacket, 0);
	MasterList::Append8(downSamplePacket, SCE_GS_TEX1_1 );

	MasterList::Append8(downSamplePacket, SCE_GS_SET_UV(0, 0));
	MasterList::Append8(downSamplePacket, SCE_GS_UV );

	MasterList::Append8(downSamplePacket, SCE_GS_SET_XYZ2(u32X << 4, u32Y << 4, 0));
	MasterList::Append8(downSamplePacket, SCE_GS_XYZ2 );

	MasterList::Append8(downSamplePacket, SCE_GS_SET_UV(srcWidth << 4, srcHeight << 4));
	MasterList::Append8(downSamplePacket, SCE_GS_UV );

	MasterList::Append8(downSamplePacket, SCE_GS_SET_XYZ2((u32X + destWidth) << 4, (u32Y + destHeight) << 4, 0));
	MasterList::Append8(downSamplePacket, SCE_GS_XYZ2 );

	FlushCache(0);

	if(!MasterList::IsInited())
	{
		/* init gs state handler */
		GSState__Init();

		MasterList::Init();
	}

#endif
}

/* do the graphics initialization for the current platform */

#ifdef PS2
void Display__ClearGraphics(void)
{
	Display__SetFrameBuffer();
	GSTricks::Prep();

	GSTricks::Color(0,0,0,0);

	if(Display__DownSampling())
	{
		// TJC - back buffer
		GSState__Set(SCE_GS_SCISSOR_1, SCE_GS_SET_SCISSOR_1(
							 0, (DisplayState.nScreenWidth - 1),
							 0, (DisplayState.nScreenHeight - 1)));
		GSTricks::SolidPoly(DisplayState.u32FrameBufferAddr[1],
								  DisplayState.nScreenWidth,
								  DisplayState.nScreenHeight,
								  DisplayState.u32FramePixelFormat[1],
								  false);
		// TJC - front buffer
		GSState__Set(SCE_GS_SCISSOR_1, SCE_GS_SET_SCISSOR_1(
							 0, (DisplayState.u32DisplayWidth - 1),
							 0, (DisplayState.u32DisplayHeight - 1)));
		GSTricks::SolidPoly(DisplayState.u32FrameBufferAddr[0],
								  DisplayState.u32DisplayWidth,
								  DisplayState.u32DisplayHeight,
								  DisplayState.u32FramePixelFormat[0],
								  false);
	}
	else
	{
		// TJC - both buffers at once
		GSState__Set(SCE_GS_SCISSOR_1, SCE_GS_SET_SCISSOR_1(
							 0, (DisplayState.u32FrameBufferWidth - 1),
							 0, (DisplayState.u32FrameBufferHeight - 1)));
		GSTricks::SolidPoly(0,
								  DisplayState.u32FrameBufferWidth,
								  DisplayState.u32FrameBufferHeight,
								  // TJC - this is safe since downsample is off
								  DisplayState.u32FramePixelFormat[0],
								  false);
	}
	GSTricks::Cleanup();
	DMA_Wait(D1_CHCR);
}
#endif

bool Display__InitGraphics( void )
{
	// I know this shouldnt go here, but the PS2 display stuff is the only stuff that used to use this...
	// if you find it here, please feel free to move it to a better spot.
	FastVars__Init();

	/***************************** ps2 version ********************************/
	#ifdef PS2
	{
		/* initialize the ps2 renderer */
		PS2Renderer::Init(0, 2*1024);

		Display__ResetGraphics();

		// ask masterlist to notify us when the dma completes..
		MasterList::SetEndOfDMACallback(&gfxRenderCompleted);

		// TJC - set framebuffer info
		Display__SetFrameBuffer();

		// TJC - clear gs memory!
		Display__ClearGraphics();

		// TJC - swap to the front buffer (this causes the graphics to
		// turn on.
		gfxSwapBuffers(1);

		/* success */

		return(TRUE);
	}
	#endif //PS2

	/***************************** win32 version ********************************/
	#if defined (WIN32) && !defined (_XBOX)
	{
		DisplayState.hWnd = Display__CreateWindow( );
		if ( DisplayState.hWnd == NULL  )
			return( FALSE);

		Display__UpdateWindow( );
		#ifdef DIRECTX
		DXPublished::initDXDevice( DirectX_Device );
		StripBufferPlatformData::createDeviceObjects( );
		VtxBufferPlatformData::createDeviceObjects( );
		#endif

		return(TRUE);
	}
	#else
	{

		#if defined (_XBOX)
		extern bool gfxBootStrapXBOX( void );
		gfxBootStrapXBOX();

		DXPublished::initDXDevice( DirectX_Device );
		StripBufferPlatformData::createDeviceObjects( );
		VtxBufferPlatformData::createDeviceObjects( );

		// load debug font
		if( FAILED( g_Font.CreateFromMemory((const char*)&xbox_resident_font )))
			return FALSE;

		// Create the Default Arial TrueTypeFont
		if( FAILED( XFONT_OpenDefaultFont( &g_pDefaultTrueTypeFont ) ) )
			return FALSE;

		// mount the utility drive
		#if defined (_XBOX) && defined(CONSUMER_BUILD)
		CFileCache::InitCacheSystem("Z:",10486760, false);
		#else
 		CFileCache::InitCacheSystem("Z:",10486760, true);
		#endif

		// profiling
		#ifdef    PROFILE
		D3DPERF_SetShowFrameRateInterval( 5000 );
		#endif

		return(TRUE);
		#endif
	}
	#endif

	/***************************** gcn version ********************************/
	#ifdef GCN
	{

		u32					u32FBSize;

		GXRenderModeObj	*pGXRenderMode;

		GXColor				gxcolorClear;

		switch( VIGetTvFormat( ))
		{
			case VI_NTSC:
				pGXRenderMode = &GXNtsc480IntDf;
				break;
			case VI_PAL:
				pGXRenderMode = &GXPal528IntDf;
				break;
			case VI_MPAL:
				pGXRenderMode = &GXMpal480IntDf;
				break;
		}

		DisplayState.pGXRenderMode = pGXRenderMode;

		DisplayState.nScreenWidth = pGXRenderMode->fbWidth;
		DisplayState.nScreenHeight = pGXRenderMode->xfbHeight;

		u32FBSize =	VIPadFrameBufferWidth( pGXRenderMode->fbWidth) *
						pGXRenderMode->xfbHeight *
						(u32)VI_DISPLAY_PIX_SZ;

		MEM_SET_ALLOC_NAME("GCN EFB FIFO");
		DisplayState.pFIFOBuffer = memAlloc( DisplayState.u32FIFOSize + ( u32FBSize * GCN_NUM_XFBS), 32);
		ASSERT( DisplayState.pFIFOBuffer != NULL);

		MEM_SET_ALLOC_NAME("GCN FRAME CLEAR");
		g_pClearFrameCopyBuffer = memAlloc( pGXRenderMode->fbWidth * sizeof(short), 32);
		ASSERT( g_pClearFrameCopyBuffer != NULL);

		DCInvalidateRange( DisplayState.pFIFOBuffer, DisplayState.u32FIFOSize + ( u32FBSize * GCN_NUM_XFBS));

		apXFB[0] = (void *)( (u32)DisplayState.pFIFOBuffer + DisplayState.u32FIFOSize);
#if GCN_NUM_XFBS == 2
		apXFB[1] = (void *)( (u32)apXFB[0] + u32FBSize);
#endif

		VIConfigure( pGXRenderMode);

		DisplayState.pGXFIFO = GXInit( DisplayState.pFIFOBuffer, DisplayState.u32FIFOSize);
		ASSERT( DisplayState.pGXFIFO != NULL);

		GXSetPixelFmt( GX_PF_RGB8_Z24, GX_ZC_LINEAR);
		GXSetViewport(	0.0F, 0.0F,
							(f32)pGXRenderMode->fbWidth, (f32)pGXRenderMode->xfbHeight,
							0.0F, 1.0F);
		GXSetScissor( 0, 0, (u32)pGXRenderMode->fbWidth, (u32)pGXRenderMode->efbHeight);

		GXSetDispCopySrc( 0, 0, pGXRenderMode->fbWidth, pGXRenderMode->efbHeight);
		GXSetDispCopyDst( pGXRenderMode->fbWidth, pGXRenderMode->xfbHeight);
		GXSetDispCopyYScale( (f32)( pGXRenderMode->xfbHeight) / (f32)( pGXRenderMode->efbHeight));
		GXSetCopyFilter( pGXRenderMode->aa, pGXRenderMode->sample_pattern, GX_TRUE, pGXRenderMode->vfilter);
		GXSetCopyClamp( (GXFBClamp)( GX_CLAMP_TOP | GX_CLAMP_BOTTOM));

		GXSetColorUpdate( GX_ENABLE);
		GXSetZMode( GX_ENABLE, GX_LEQUAL, GX_ENABLE);
		GXSetCullMode( GX_CULL_FRONT);

		// Clear embedded framebuffer
		gxcolorClear.r = 0;
		gxcolorClear.g = 0;
		gxcolorClear.b = 0;
		gxcolorClear.a = 0xff;

		GXSetCopyClear( gxcolorClear, GX_MAX_Z24);
		//we must copy twice, first to clear EFB, next to copy cleared EFB into XFB
		GXCopyDisp( apXFB[0], GX_TRUE);
		GXCopyDisp( apXFB[0], GX_FALSE);
#if GCN_NUM_XFBS == 2
		GXDrawDone( );

		GXCopyDisp( apXFB[1], GX_FALSE);
#endif

		GXDrawDone( );

		DCInvalidateRange( apXFB[0], ( u32FBSize * GCN_NUM_XFBS));

		g_pNextFrameBuffer = apXFB[0];
		g_CurrDrawBuffer = 0;

		VISetBlack( GX_FALSE);

		VISetPreRetraceCallback( gcnPreRetraceCallback);

		VIWaitForRetrace( );

		g_ShaderMgr.ResetCurrentMatrixIndices( );

		MatrixStack__Init( );

		//settings for vertex format 0
		GXSetVtxAttrFmt(	GX_VTXFMT0,
								GX_VA_POS,
								GX_POS_XYZ,
								GX_F32,
								0);
		GXSetVtxAttrFmt(	GX_VTXFMT0,
								GX_VA_NRM,
								GX_NRM_XYZ,
								GX_F32,
								0);
		GXSetVtxAttrFmt(	GX_VTXFMT0,
								GX_VA_CLR0,
								GX_CLR_RGBA,
								GX_RGBA8,
								0);
		GXSetVtxAttrFmt(	GX_VTXFMT0,
								GX_VA_TEX0,
								GX_TEX_ST,
								GX_F32,
								0);
		GXSetVtxAttrFmt(	GX_VTXFMT0,
								GX_VA_TEX1,
								GX_TEX_ST,
								GX_F32,
								0);
		GXSetVtxAttrFmt(	GX_VTXFMT0,
								GX_VA_TEX2,
								GX_TEX_ST,
								GX_F32,
								0);
		GXSetVtxAttrFmt(	GX_VTXFMT0,
								GX_VA_TEX3,
								GX_TEX_ST,
								GX_F32,
								0);
		GXSetVtxAttrFmt(	GX_VTXFMT0,
								GX_VA_TEX4,
								GX_TEX_ST,
								GX_F32,
								0);
		GXSetVtxAttrFmt(	GX_VTXFMT0,
								GX_VA_TEX5,
								GX_TEX_ST,
								GX_F32,
								0);
		GXSetVtxAttrFmt(	GX_VTXFMT0,
								GX_VA_TEX6,
								GX_TEX_ST,
								GX_F32,
								0);
		GXSetVtxAttrFmt(	GX_VTXFMT0,
								GX_VA_TEX7,
								GX_TEX_ST,
								GX_F32,
								0);

		//settings for vertex format 1
		GXSetVtxAttrFmt(	GX_VTXFMT1,
								GX_VA_POS,
								GX_POS_XYZ,
								GX_F32,
								0);
		GXSetVtxAttrFmt(	GX_VTXFMT1,
								GX_VA_NRM,
								GX_NRM_XYZ,
								GX_S8,
								6);
		GXSetVtxAttrFmt(	GX_VTXFMT1,
								GX_VA_CLR0,
								GX_CLR_RGBA,
								GX_RGBA8,
								0);
		GXSetVtxAttrFmt(	GX_VTXFMT1,
								GX_VA_TEX0,
								GX_TEX_ST,
								GX_F32,
								0);
		GXSetVtxAttrFmt(	GX_VTXFMT1,
								GX_VA_TEX1,
								GX_TEX_ST,
								GX_F32,
								0);
		GXSetVtxAttrFmt(	GX_VTXFMT1,
								GX_VA_TEX2,
								GX_TEX_ST,
								GX_F32,
								0);
		GXSetVtxAttrFmt(	GX_VTXFMT1,
								GX_VA_TEX3,
								GX_TEX_ST,
								GX_F32,
								0);
		GXSetVtxAttrFmt(	GX_VTXFMT1,
								GX_VA_TEX4,
								GX_TEX_ST,
								GX_F32,
								0);
		GXSetVtxAttrFmt(	GX_VTXFMT1,
								GX_VA_TEX5,
								GX_TEX_ST,
								GX_F32,
								0);
		GXSetVtxAttrFmt(	GX_VTXFMT1,
								GX_VA_TEX6,
								GX_TEX_ST,
								GX_F32,
								0);
		GXSetVtxAttrFmt(	GX_VTXFMT1,
								GX_VA_TEX7,
								GX_TEX_ST,
								GX_F32,
								0);

		//settings for vertex format 2
		GXSetVtxAttrFmt(	GX_VTXFMT2,
								GX_VA_POS,
								GX_POS_XYZ,
								GX_F32,
								0);
		GXSetVtxAttrFmt(	GX_VTXFMT2,
								GX_VA_NRM,
								GX_NRM_XYZ,
								GX_F32,
								0);
		GXSetVtxAttrFmt(	GX_VTXFMT2,
								GX_VA_CLR0,
								GX_CLR_RGB,
								GX_RGB565,
								0);
		GXSetVtxAttrFmt(	GX_VTXFMT2,
								GX_VA_TEX0,
								GX_TEX_ST,
								GX_F32,
								0);
		GXSetVtxAttrFmt(	GX_VTXFMT2,
								GX_VA_TEX1,
								GX_TEX_ST,
								GX_F32,
								0);
		GXSetVtxAttrFmt(	GX_VTXFMT2,
								GX_VA_TEX2,
								GX_TEX_ST,
								GX_F32,
								0);
		GXSetVtxAttrFmt(	GX_VTXFMT2,
								GX_VA_TEX3,
								GX_TEX_ST,
								GX_F32,
								0);
		GXSetVtxAttrFmt(	GX_VTXFMT2,
								GX_VA_TEX4,
								GX_TEX_ST,
								GX_F32,
								0);
		GXSetVtxAttrFmt(	GX_VTXFMT2,
								GX_VA_TEX5,
								GX_TEX_ST,
								GX_F32,
								0);
		GXSetVtxAttrFmt(	GX_VTXFMT2,
								GX_VA_TEX6,
								GX_TEX_ST,
								GX_F32,
								0);
		GXSetVtxAttrFmt(	GX_VTXFMT2,
								GX_VA_TEX7,
								GX_TEX_ST,
								GX_F32,
								0);

		//settings for vertex format 3
		GXSetVtxAttrFmt(	GX_VTXFMT3,
								GX_VA_POS,
								GX_POS_XYZ,
								GX_F32,
								0);
		GXSetVtxAttrFmt(	GX_VTXFMT3,
								GX_VA_NRM,
								GX_NRM_XYZ,
								GX_S8,
								6);
		GXSetVtxAttrFmt(	GX_VTXFMT3,
								GX_VA_CLR0,
								GX_CLR_RGB,
								GX_RGB565,
								0);
		GXSetVtxAttrFmt(	GX_VTXFMT3,
								GX_VA_TEX0,
								GX_TEX_ST,
								GX_F32,
								0);
		GXSetVtxAttrFmt(	GX_VTXFMT3,
								GX_VA_TEX1,
								GX_TEX_ST,
								GX_F32,
								0);
		GXSetVtxAttrFmt(	GX_VTXFMT3,
								GX_VA_TEX2,
								GX_TEX_ST,
								GX_F32,
								0);
		GXSetVtxAttrFmt(	GX_VTXFMT3,
								GX_VA_TEX3,
								GX_TEX_ST,
								GX_F32,
								0);
		GXSetVtxAttrFmt(	GX_VTXFMT3,
								GX_VA_TEX4,
								GX_TEX_ST,
								GX_F32,
								0);
		GXSetVtxAttrFmt(	GX_VTXFMT3,
								GX_VA_TEX5,
								GX_TEX_ST,
								GX_F32,
								0);
		GXSetVtxAttrFmt(	GX_VTXFMT3,
								GX_VA_TEX6,
								GX_TEX_ST,
								GX_F32,
								0);
		GXSetVtxAttrFmt(	GX_VTXFMT3,
								GX_VA_TEX7,
								GX_TEX_ST,
								GX_F32,
								0);

		GXSetTevOrder( GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
		GXSetTevOrder( GX_TEVSTAGE1, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
		GXSetTevOrder( GX_TEVSTAGE2, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
		GXSetTevOrder( GX_TEVSTAGE3, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
		GXSetTevOrder( GX_TEVSTAGE4, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
		GXSetTevOrder( GX_TEVSTAGE5, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
		GXSetTevOrder( GX_TEVSTAGE6, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
		GXSetTevOrder( GX_TEVSTAGE7, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
		GXSetTevOrder( GX_TEVSTAGE8, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
		GXSetTevOrder( GX_TEVSTAGE9, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
		GXSetTevOrder( GX_TEVSTAGE10,GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
		GXSetTevOrder( GX_TEVSTAGE11,GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
		GXSetTevOrder( GX_TEVSTAGE12,GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
		GXSetTevOrder( GX_TEVSTAGE13,GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
		GXSetTevOrder( GX_TEVSTAGE14,GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
		GXSetTevOrder( GX_TEVSTAGE15,GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);

		GXSetTexCoordScaleManually( GX_TEXCOORD0, GX_ENABLE, 1, 1);
		GXSetTexCoordScaleManually( GX_TEXCOORD1, GX_ENABLE, 1, 1);
		GXSetTexCoordScaleManually( GX_TEXCOORD2, GX_ENABLE, 1, 1);
		GXSetTexCoordScaleManually( GX_TEXCOORD3, GX_ENABLE, 1, 1);
		GXSetTexCoordScaleManually( GX_TEXCOORD4, GX_ENABLE, 1, 1);
		GXSetTexCoordScaleManually( GX_TEXCOORD5, GX_ENABLE, 1, 1);
		GXSetTexCoordScaleManually( GX_TEXCOORD6, GX_ENABLE, 1, 1);
		GXSetTexCoordScaleManually( GX_TEXCOORD7, GX_ENABLE, 1, 1);

		GXSetVerifyLevel( GX_WARN_NONE);
		GXSetVerifyCallback( gcnVerifyCallback);

		mtxEnvBase.Identity( );
		mtxEnvBase.CatTranslation( 1.0f, -1.0f, 0.0f);
		mtxEnvBase.CatScale( 0.5f, -0.5f, 1.0);

		DEMOSetGPHangMetric( GX_TRUE);
		GXSetDrawSyncCallback( gxDrawSyncCallback);
		g_CurrDrawToken = 0x8000;
		GXSetDrawSync( g_CurrDrawToken);
		GXFlush( );
		GXFlush( );

		void		*pNextBreakpoint,
					*pRead;

		GXGetFifoPtrs( DisplayState.pGXFIFO, &pRead, &pNextBreakpoint);
		GXEnableBreakPt( pNextBreakpoint);

		memSetOneShotNewFlags( MEM_FLG_PERMANENT);
		MEM_SET_ONE_SHOT_NEW_NAME( "CFrameData0");
		g_apFrameData[0] = new CFrameData;

		memSetOneShotNewFlags( MEM_FLG_PERMANENT);
		MEM_SET_ONE_SHOT_NEW_NAME( "CFrameData1");
		g_apFrameData[1] = new CFrameData;

		g_pFrameData = g_apFrameData[0];

		return( TRUE);
	}
	#endif

	// init disc error handler
	DiscErrorHandler::Init();
}

#ifdef PS2
bool Display__DownSampling(void)
{
	return
		sCurrentFramesDisplayState.u32FramePixelFormat[0] != sCurrentFramesDisplayState.u32FramePixelFormat[1] ||
		sCurrentFramesDisplayState.u32DisplayHeight != sCurrentFramesDisplayState.nScreenHeight ||
		sCurrentFramesDisplayState.u32DisplayWidth != sCurrentFramesDisplayState.nScreenWidth;
}
#endif

#ifdef DIRECTX_PC
bool gfxStartRender(bool toTexture)
#else
bool gfxStartRender(void)
#endif
{
	// notify the texture manager that we are handling a new frame.
	TextureMgr::NewFrame();

#if !defined(CDROM) && !defined(CONSUMER_BUILD)
#ifndef GCN
	/* take a screen-shot? */

	if (bSnapScreen)
	{
		_gfxSnapScreen();
		bSnapScreen = FALSE;;
	}
#endif //GCN
#endif //CONSUMER_BUILD

	/* update frame counter */

	DisplayState.u32FrameCount++;

	/***************************** pc version ********************************/
#ifdef WIN32
#endif //WIN32
#ifdef DIRECTX_PC
	if (RenderToTexture::renderSurface && toTexture)
	{
		RenderToTexture::renderingToTexture = true;
	}
	else
		RenderToTexture::renderingToTexture = false;
#endif // DIRECTX_PC
	/***************************** ps2 version ********************************/
#ifdef PS2

	// start master list
	// the texturemanager may have already done this for us... (if fact probably has)
	// so we need to check first...
	if(!MasterList::CurrentlyActive())
		MasterList::Start();

	// generate a clip matrix... this goes from view cannonical volume to clip volume..
	// build it for the whole screen...
	// TJC TODO - these probably should be moved to matrix stack...
	PS2Renderer::SetViewToClip(DisplayState.nScreenWidth, DisplayState.nScreenHeight);
	PS2Renderer::SetViewToCull(DisplayState.nScreenWidth, DisplayState.nScreenHeight);

	GSState.DIMX = SCE_GS_SET_DIMX(4, 2, 5, 3, 0, 6, 1, 7, 5, 3, 4, 2, 1, 7, 0, 6);	// from jeff g
//		GSState.DIMX = SCE_GS_SET_DIMX(-4, 2, -3, 3, 0, -2, 1, -1, -3, 3, -4, 2, 1, -1, 0, -2);	// from sony docs, looks like doodie
//		GSState.DIMX = SCE_GS_SET_DIMX(0, 8, 2, 10, 12, 4, 14, 6, 3, 11, 1, 9, 15, 7, 13, 5);	// The standard dither matrix from FvD- not useable here
	GSState__Set(SCE_GS_DIMX, GSState.DIMX);
	GSState__Set(SCE_GS_DTHE, GSState.DTHE);

	// TJC -  clear scanmask and
	GSState__Set(SCE_GS_SCANMSK, 0);
	GSState__Set(SCE_GS_PRMODECONT, GSState.PRMODECONT);
	GSState__Set(SCE_GS_COLCLAMP, GSState.COLCLAMP);
	GSState__Set(SCE_GS_TEXA, GSState.TEXA);


	// clear label register
	GSState__Set(SCE_GS_LABEL, SCE_GS_SET_LABEL(NULL, 0xFFFFFFFF));

#endif //PS2

    /***************************** directx version ****************************/
#ifdef DIRECTX
#ifndef _XBOX
	if( bDeviceAvailable == false )
	{
		HRESULT result = DirectX_Device->TestCooperativeLevel( );
		switch( result )
		{
		case D3D_OK:
			bDeviceAvailable = true;
			break;
		case D3DERR_DEVICELOST:
			return false;
		case D3DERR_DEVICENOTRESET:
			DXPublished::releaseDeviceObjects( );
			DrawArray__Terminate( );

			result = DirectX_Device->Reset( & DirectX_PresentParams );
			ASSERT( result == D3D_OK );

			DXPublished::restoreDeviceObjects( );
			DrawArray__Initialize( );

			bDeviceAvailable = true;
			break;
		default:
			ASSERT( 0 );
		}
	}
#endif

	// Begin the scener
	HRESULT result = DirectX_Device->BeginScene( );
	ASSERT( result == D3D_OK );
#endif

	/****************************** gamecube version ***************************/
#ifdef GCN
	GXInvalidateVtxCache( );
#endif

#ifdef PS2
	sCurrentFramesDisplayState = DisplayState;
#endif

	/* success */

	return(TRUE);
}

#ifdef DIRECTX
void gfxDisableDevice( void )
{
	bDeviceAvailable = false;
}
void gfxEnableDevice( void )
{
	bDeviceAvailable = true;
}

bool gfxDeviceAvailable( void )
{
	return bDeviceAvailable;
}
#endif

#ifdef PS2
void gfxRenderCompleted(void)
{
	bReadyForSwap = true;
}

void gfxStallForVBlank(void)
{
	// make interrupt swap the buffers...
	// really this doenst have to be 1 per frame, but I'll do it anyway...
	MasterList::SetMinVBlanks(RenderMgr__GetNumVBlanksToSyncTo());

//	u8 syncTo = RenderMgr__GetNumVBlanksToSyncTo();
// 	if(nNumVBlanks >= syncTo) syncTo = nNumVBlanks + 1;
// 	if (RenderMgr__VSyncOn())
// 	{
// 		while(nNumVBlanks < syncTo);
// 	}
//	nNumVBlanks = 0;

	// ok, now we need to wait for the frame buffer swap to complete... this probably
	// will just fall through...
//	while(bReadyForSwap);

	// determine if we are downsampling or not... (ie.. if buffer[0] is always displayed)
	bool downSample = Display__DownSampling();
	if(downSample)
		sCurrentFramesDisplayState.u32FrontBufferIndex = 
			DisplayState.u32FrontBufferIndex = 0;
	else
		sCurrentFramesDisplayState.u32FrontBufferIndex = 
			DisplayState.u32FrontBufferIndex = MasterList::GetFrameToggle();

}
#endif

#ifdef PS2
static int gDOFBlurModeDown = GSTricks::CONVOLVE_11;
static int gDOFBlurModeUp = GSTricks::CONVOLVE_11;
static int gDOFBlurDepth = 1;
bool gDOFOn = false;
bool gDOFGreen = false;
bool gDOFShowBlured = false;
#endif
static int fifoUsed;

//#define DRAW_GRID

bool gfxEndRender( bool bImmediateMode)
{
/***************************** pc version ********************************/
#ifdef WIN32

#ifdef DRAW_GRID
	gfxDrawGrid( );
#endif //DRAW_GRID

#endif  // WIN32

/***************************** ps2 version ********************************/
#ifdef PS2

	// if we are using depth bluring
	if(gDOFOn)
	{
		// make sure the z buffer is big enough
		ASSERT(DisplayState.u32ZPixelFormat == SCE_GS_PSMZ32 ||
			DisplayState.u32ZPixelFormat == SCE_GS_PSMZ24);

		u8 bufferIdx = DisplayState.u32FrontBufferIndex; // why is this backwards?
		u32 backBufferAddr = DisplayState.u32FrameBufferAddr[bufferIdx];
		u32 backBufferFormat = DisplayState.u32FramePixelFormat[bufferIdx];

		// ok, we'll do this simply for the first pass....
		// its just a downsample and upsample...
		GSTricks::Prep();

		// ok, this is pretty complicated... to
		// avoid using too much texture memory, we
		// do an extra copy here (I think)...

		// First, copy ZBuf->G to Back->A
		GSTricks::Mask(0x3FFF);
		GSTricks::RGToBA(
			DisplayState.u32ZBufferAddr,
			DisplayState.nScreenWidth,
			DisplayState.nScreenHeight,
			DisplayState.u32ZPixelFormat,
			backBufferAddr,
			backBufferFormat);
		GSTricks::Mask(0x00000000);

		// invert the z buffer
		GSTricks::InvertBuffer(
			DisplayState.u32ZBufferAddr,
			DisplayState.nScreenWidth,
			DisplayState.nScreenHeight,
			DisplayState.u32ZPixelFormat);

		// clamp the Back->A values where Z->RG > 0x00FF
		GSTricks::Mask(0x00FFFFFF);
		GSTricks::ZValue(0x008000FF); // TJC - strange value here... not sure why..
		GSState__Set(SCE_GS_TEST_1, SCE_GS_SET_TEST_1(false, 0, 0x00, 0, false, false, true, SCE_GS_ZGREATER));
		GSTricks::SolidPoly(
			backBufferAddr,
			DisplayState.nScreenWidth,
			DisplayState.nScreenHeight,
			backBufferFormat);
		GSState__Set(SCE_GS_TEST_1, SCE_GS_SET_TEST_1(false, 0, 0x00, 0, false, false, true, SCE_GS_ZALWAYS));
		GSTricks::Mask(0x00000000);

		// ok, now duplicate the back buffer over the z buffer...
		GSTricks::CopyBuffer(
			backBufferAddr,
			DisplayState.nScreenWidth,
			DisplayState.nScreenHeight,
			backBufferFormat,
			DisplayState.u32ZBufferAddr,
			DisplayState.nScreenWidth,
			DisplayState.nScreenHeight,
			SCE_GS_PSMCT32);

		// turn on the bilinear filter
		GSState__Set(SCE_GS_TEX1_1, SCE_GS_SET_TEX1_1( 0, 0, 1, 1, 0, 0, 0 ));

		// now blur the zbuffer image
		GSTricks::Mask(0x00000000);
		u32 buffer = GSTricks::BlurBuffer(
			DisplayState.u32ZBufferAddr,
			DisplayState.nScreenWidth,
			DisplayState.nScreenHeight,
			SCE_GS_PSMCT32,
			gDOFBlurDepth,
			gDOFBlurModeDown,
			gDOFBlurModeUp);

		// now copy the zbuffer image over the frame buffer
		// using the destination alpha channel...
		if(gDOFShowBlured)
			GSState__Set(SCE_GS_ALPHA_1, SCE_GS_SET_ALPHA_1(0, 2, 2, 2, 0x80));
		else
			GSState__Set(SCE_GS_ALPHA_1, SCE_GS_SET_ALPHA_1(1, 0, 1, 0, 0x80));

		if(gDOFGreen)
			GSTricks::Color(0x00, 0x80, 0x00, 0x80);
		GSTricks::CopyBuffer(
			buffer,
			DisplayState.nScreenWidth >> 1,
			DisplayState.nScreenHeight >> 1,
			SCE_GS_PSMCT32,
			backBufferAddr,
			DisplayState.nScreenWidth,
			DisplayState.nScreenHeight,
			backBufferFormat,
			true);
		GSTricks::Color(0x80, 0x80, 0x80, 0x80);
		GSState__Set(SCE_GS_ALPHA_1, SCE_GS_SET_ALPHA_1(0, 1, 0, 1, 0x80));

		// turn off the bilinear filter
		GSState__Set(SCE_GS_TEX1_1, SCE_GS_SET_TEX1_1( 0, 0, 0, 0, 0, 0, 0 ));

		// release the blurred buffer
		GSTricks::PopMark();

		GSTricks::Cleanup();

	}

	/* finish off the master list & wait for completion of prior frame */
	MasterList::End();

	/* little bit of timing info... */
#ifndef CONSUMER_BUILD
	static u_int processCycleStart = 0;
	u_int processCycleEnd = 0;

	asm __volatile__("mfc0 %0,$9" : "=r" (processCycleEnd));	// value in cpu cycles (300MHz)

	u_int cyclesWaitingForSync = MasterList::GetDMAWaitStallCycleCount();
	u_int cyclesWaitingForMemory = MasterList::GetBlockWaitStallCycleCount();

	SceneMgr::GetStats().IncRenderuSec( MasterList::GetRecentCycleCount() / 295 );
	SceneMgr::GetStats().IncProcessuSec(((processCycleEnd - processCycleStart) - cyclesWaitingForSync) / 295);
	SceneMgr::GetStats().IncProcessBlockeduSec(cyclesWaitingForMemory / 295);
	SceneMgr::GetStats().IncProcessWaituSec(cyclesWaitingForSync / 295);
#endif

	Profiler__Start(PROFILE_VSYNC);
	gfxStallForVBlank();
	Profiler__Stop(PROFILE_VSYNC);

	/* finish off our timing info... (without counting the vblank period)*/
#ifndef CONSUMER_BUILD
	asm __volatile__("mfc0 %0,$9" : "=r" (processCycleStart));	// value in cpu cycles (300MHz)
#endif

	/* start the new scene rendering */
//	MasterList::Render();

#endif //PS2

	/***************************** directx version ****************************/
#ifdef DIRECTX
	gfxSwapBuffers();
#endif // DIRECTX

#if defined(WIN32) && !defined(_XBOX) && !defined(DIRECTX_PC)
	// Fake VBlank syncing: sleep till an appropriate amount of time goes by
	// This will cause the PC to run at frame-rates similar to consoles, AND will allow more multitasking to happen...

	if( RenderMgr__VSyncOn( ))
	{
		static uint lastTimeMS = EngineTimer__Time();
		uint minTimeMS = (uint) (RenderMgr__GetNumVBlanksToSyncTo( ) * 16.668335f);

		Sleep(1);	//allow OS to get a time slice (in case we're running to slow to sleep() below)

		uint curTimeMS = EngineTimer__Time();

#if 1

		//this version allows the most CPU time to be given back to the OS, but doesn't maintain as accurate a framerate

		if( (curTimeMS - lastTimeMS) < (minTimeMS) - 4 )
			Sleep(minTimeMS - (curTimeMS - lastTimeMS) - 4);

#else

		//this version maintains a higly accurate framerate, but gives very little CPU time to the OS

		while ( (curTimeMS - lastTimeMS) < minTimeMS)
		{
			Sleep(0);
			curTimeMS = EngineTimer__Time();
		}

#endif

		lastTimeMS = EngineTimer__Time();

	}
#endif

	/***************************** gcn version ****************************/
#ifdef GCN
	{

		Profiler__Start( PROFILE_RENDER_ASYNC);

		GXRenderModeObj	*pGXRenderMode = DisplayState.pGXRenderMode;

		u8						nextDrawBuffer;

		u16					nextDrawToken;

		void					*pNextBreakpoint,
								*pRead;

		if( DisplayState.u32MBstrength != 0)
		{
			TextureMgr::CopyFrontBuffer( );
		}
		else
		{
			TextureMgr::SetFrontBufferValid( false);
		}

#if GCN_NUM_XFBS == 1
		nextDrawBuffer = 0;
#else
		if( !bImmediateMode)
		{
			nextDrawBuffer = g_CurrDrawBuffer ^ 0x01;
		}
		else
		{
			nextDrawBuffer = g_CurrDrawBuffer;
		}
#endif

		nextDrawToken = ( g_CurrDrawToken + 1) | 0x8000;

#if GCN_NUM_XFBS == 1
		GXFlush( );
		GXSetDrawSync( nextDrawToken);
		GXFlush( );
		GXFlush( );

		GXGetFifoPtrs( DisplayState.pGXFIFO, &pRead, &pNextBreakpoint);

		if( !bImmediateMode)
		{
			GXSetDispCopySrc( 0, 0, pGXRenderMode->fbWidth, pGXRenderMode->efbHeight);
			GXSetDispCopyDst( pGXRenderMode->fbWidth, pGXRenderMode->xfbHeight);
			GXSetDispCopyYScale( (f32)( pGXRenderMode->xfbHeight) / (f32)( pGXRenderMode->efbHeight));
			GXSetCopyFilter( pGXRenderMode->aa, pGXRenderMode->sample_pattern, GX_TRUE, pGXRenderMode->vfilter);

			GXCopyDisp( apXFB[nextDrawBuffer], GX_FALSE);
			GXFlush( );
		}
#endif

#if GCN_NUM_XFBS == 2
		GXSetDispCopySrc( 0, 0, pGXRenderMode->fbWidth, pGXRenderMode->efbHeight);
		GXSetDispCopyDst( pGXRenderMode->fbWidth, pGXRenderMode->xfbHeight);
		GXSetDispCopyYScale( (f32)( pGXRenderMode->xfbHeight) / (f32)( pGXRenderMode->efbHeight));
		GXSetCopyFilter( pGXRenderMode->aa, pGXRenderMode->sample_pattern, GX_TRUE, pGXRenderMode->vfilter);

		GXCopyDisp( apXFB[nextDrawBuffer], GX_FALSE);

		GXSetDrawSync( nextDrawToken);
		GXFlush( );
		GXFlush( );

		GXGetFifoPtrs( DisplayState.pGXFIFO, &pRead, &pNextBreakpoint);
#endif

		if( bImmediateMode)
		{
			g_CurrDrawToken = nextDrawToken;
		}

		u32 u32Start = u32GCNTimeout;
		while( GXReadDrawSync( ) != g_CurrDrawToken)
		{
			if( ( u32GCNTimeout - u32Start) > 360)
			{
				void					*pRead,
										*pWrite;

				GXBool				gxbOverhi,
										gxbUnderlow,
										gxbReadIdle,
										gxbCmdIdle,
										gxbBrkpt;

				GXGetFifoPtrs( DisplayState.pGXFIFO, &pRead, &pWrite);
				GXGetGPStatus( &gxbOverhi, &gxbUnderlow, &gxbReadIdle, &gxbCmdIdle, &gxbBrkpt );
				OSReport( "Graphics FIFO hang at Read = 0x%08X, Write = 0x%08X\n", pRead, pWrite);
				OSReport( 	"  OverHi = %d, Underlow = %d, ReadIdle = %d, CmdIdle = %d, Brkpt = %d\n",
								gxbOverhi, gxbUnderlow, gxbReadIdle, gxbCmdIdle, gxbBrkpt );
				__DEMODiagnoseHang( );
				LOCKUP( );
			}
		}

		Profiler__Stop( PROFILE_RENDER_ASYNC);

#if 0
		fifoUsed = (int)((u32)pNextBreakpoint - (u32)pRead);
		if( fifoUsed < 0)
		{
			fifoUsed += DisplayState.u32FIFOSize;
		}

		ASSERT( fifoUsed <= ( DisplayState.u32FIFOSize / 2));
#endif

#if GCN_NUM_XFBS == 2
		g_pNextFrameBuffer = apXFB[g_CurrDrawBuffer];
#endif
//		OSReport( "Displaying XFB 0x%08x, vbcount == %d\n", apXFB[g_CurrDrawBuffer], VIGetRetraceCount( ));

		if( g_pPrevFrameData != NULL)
		{
			g_pPrevFrameData->Clear( );
		}

		Profiler__Start( PROFILE_VSYNC);
		if( RenderMgr__GetNumVBlanksToSyncTo( ) > 0)
		{
			do
			{
				VIWaitForRetrace( );
			} while( u32GCNVBlankCount < RenderMgr__GetNumVBlanksToSyncTo( ));
			u32GCNVBlankCount = 0;
		}
		Profiler__Stop( PROFILE_VSYNC);

#if GCN_NUM_XFBS == 1
		if( bImmediateMode)
		{
			GXSetDispCopySrc( 0, 0, pGXRenderMode->fbWidth, pGXRenderMode->efbHeight);
			GXSetDispCopyDst( pGXRenderMode->fbWidth, pGXRenderMode->xfbHeight);
			GXSetDispCopyYScale( (f32)( pGXRenderMode->xfbHeight) / (f32)( pGXRenderMode->efbHeight));
			GXSetCopyFilter( pGXRenderMode->aa, pGXRenderMode->sample_pattern, GX_TRUE, pGXRenderMode->vfilter);

			GXCopyDisp( apXFB[nextDrawBuffer], GX_FALSE);
			GXFlush( );
		}
#endif

		g_pPrevFrameData = g_pFrameData;

		g_pFrameData = ( g_pFrameData == g_apFrameData[0]) ? g_apFrameData[1] : g_apFrameData[0];

#if !defined(CDROM) && !defined(CONSUMER_BUILD)
		// take a screen-shot?
		if( bSnapScreen)
		{
			_gfxSnapScreen( );
			bSnapScreen = FALSE;
		}
#endif //CONSUMER_BUILD
		if( bImmediateMode)
		{
			nextDrawBuffer ^= 0x01;
		}
		g_CurrDrawBuffer = nextDrawBuffer;
		g_CurrDrawToken = nextDrawToken;
		if( !bImmediateMode)
		{
			GXEnableBreakPt( pNextBreakpoint);
		}
	}
#endif

	/* success */

	return(TRUE);
}

#ifdef DIRECTX
#include "Game/Managers/TextureMgr/DirectX/PlatformImage.h"
PlatformImage * pDebugImage = 0;
#endif

#ifdef PS2
void gfxSwapBuffers(int bufferidx)
#else
void gfxSwapBuffers(void)
#endif
{
	/***************************** pc opengl version **************************/
#if defined ( WIN32 ) && ! defined ( DIRECTX )

	Profiler__Start(PROFILE_VSYNC);
	SwapBuffers( DisplayState.hDC);
	Profiler__Stop(PROFILE_VSYNC);

#endif //WIN32 && ! DIRECTX

	/***************************** ps2 version ********************************/
#ifdef PS2

	/* show new front buffer. on ps2, this does not tell the gs where the new back buffer is-- that is handled separately */
	/* by dma'ing some register values to the gs before rendering the next scene */

	u32 u32FrameBufferAddr, u32FramePixelFormat;

	/* make frame buffer address GS-ready */

	// please note that the opposite buffer is really the one thats ready.  thats because of when
	// we occur and the frame lag between generate dl & display dl (namely render dl)
	bufferidx ^= 1;

	// if we are downsampling, we dont have to account for the 1 frame lag since we
	// always get the same buffer #...
	if(Display__DownSampling())
		bufferidx = 0;

	// allow us to see the back buffer
#ifndef CONSUMER_BUILD
	if(RenderMgr__ViewBackBuffer())
		bufferidx ^= 1;
#endif
	u32FrameBufferAddr = sCurrentFramesDisplayState.u32FrameBufferAddr[bufferidx] / 32;
	u32FramePixelFormat = sCurrentFramesDisplayState.u32FramePixelFormat[bufferidx];


	if(Display__DownSampling())
	{
		gfxDownSampleFrameBuffer();
	}

	// mark in the sn tuner where the buffer swap happened...
	/* set transient display settings */

	DPUT_GS_BGCOLOR(0x00000000);

	if (sCurrentFramesDisplayState.b2Circuit == FALSE)
	{
		DPUT_GS_PMODE(
			SCE_GS_SET_PMODE(
				1, 		// en1      - read circuit 1 (on/off)
				0, 		// en2      - read circuit 2 (on/off)
				0, 		// crtmd    - CRT output switch (always 0)
				1, 		// mmod     - Alpha value selection (circuit 1/fixed)
				0, 		// amod     - Alpha out selection (circuit 1/2)
				0, 		// slbg     - blend method
				0xff));	// alp		- fixed alpha value
	}
	else
	{
		DPUT_GS_PMODE(
			SCE_GS_SET_PMODE(
				1, 		// en1      - read circuit 1 (on/off)
				1, 		// en2      - read circuit 2 (on/off)
				1, 		// crtmd    - CRT output switch (always 0)
				1, 		// mmod     - Alpha value selection (circuit 1/fixed)
				1, 		// amod     - Alpha out selection (circuit 1/2)
				0, 		// slbg     - blend method
				0x80));	// alp		- fixed alpha value
	}
	DPUT_GS_SMODE2(
		SCE_GS_SET_SMODE2(
			sCurrentFramesDisplayState.bInterlaced,		// intr - interlace on/off
			sCurrentFramesDisplayState.u32DisplayHeight == 224 ? 1 : sCurrentFramesDisplayState.bFrameMode,		// ffmd - frame/field
			0));									// dpms - Mode on.

	/* ntsc or pal? */

	int nHeight = sCurrentFramesDisplayState.bNTSCMode ? 448 : 512;

   /* DISPFB1 Read Output Circuit 1. */

	if (sCurrentFramesDisplayState.b2Circuit == FALSE)
	{
		/* circuit 2 is disabled, so just set circuit 1 */

		DPUT_GS_DISPFB1(
			SCE_GS_SET_DISPFB(
				u32FrameBufferAddr,						// fbp  - base address/2048
				sCurrentFramesDisplayState.u32DisplayWidth / 64,  // fbw  - buffer width/64
				u32FramePixelFormat,						// psm  - pixel storage format
				0, 											// dbx  - xpos in buffer
				0)); 											// dby  - ypos in buffer

		/* DISPLAY1  Set rectangular area for output circuit 1 */
		/* dx and dy have some additional hidden offsets... see eestruct.h */

		DPUT_GS_DISPLAY1(
			SCE_GS_SET_DISPLAY1(
				0,			 									// dx   - x position in display buffer in VCK units
				sCurrentFramesDisplayState.u32FrameBufferDeltaY,	// dy   - y position in display buffer in raster units
				sCurrentFramesDisplayState.u32FrameBufferMagHoriz,// magh - H magnification
				sCurrentFramesDisplayState.u32FrameBufferMagVert,	// magv - V magnification
				(640 * 4) - 1,	 							// dw   - display area width in VCK units (see 5.2 in GS docs)
				nHeight - 1));								// dh   - display area height in pixel units
	}
	else if(sCurrentFramesDisplayState.u32DisplayHeight == 224)
	{
		if(!(*GS_CSR & (1<<13)))
		{
			DPUT_GS_DISPFB1(
				SCE_GS_SET_DISPFB(
					u32FrameBufferAddr,						// fbp  - base address/2048
					sCurrentFramesDisplayState.u32DisplayWidth / 64,	// fbw  - buffer width/64
					u32FramePixelFormat,						// psm  - pixel storage format
					0, 											// dbx  - xpos in buffer
					0)); 											// dby  - ypos in buffer

			DPUT_GS_DISPLAY1(
				SCE_GS_SET_DISPLAY1(
					0,		 										// dx   - x position in display buffer in VCK units
					sCurrentFramesDisplayState.u32FrameBufferDeltaY,	// dy   - y position in display buffer in raster units
					sCurrentFramesDisplayState.u32FrameBufferMagHoriz,// magh - H magnification
					sCurrentFramesDisplayState.u32FrameBufferMagVert,	// magv - V magnification
					(640 * 4) - 1,	 							// dw   - display area width in VCK units (see 5.2 in GS docs)
					nHeight - 1));								// dh   - display area height in pixel units

			DPUT_GS_DISPFB2(
				SCE_GS_SET_DISPFB(
					u32FrameBufferAddr,						// fbp  - base address/2048
					sCurrentFramesDisplayState.u32DisplayWidth / 64,	// fbw  - buffer width/64
					u32FramePixelFormat,						// psm  - pixel storage format
					0, 											// dbx  - xpos in buffer
					0)); 											// dby  - ypos in buffer

			DPUT_GS_DISPLAY2(
				SCE_GS_SET_DISPLAY2(
					0,		 										// dx   - x position in display buffer in VCK units
					sCurrentFramesDisplayState.u32FrameBufferDeltaY,	// dy   - y position in display buffer in raster units
					sCurrentFramesDisplayState.u32FrameBufferMagHoriz,// magh - H magnification
					sCurrentFramesDisplayState.u32FrameBufferMagVert,	// magv - V magnification
					(640 * 4) - 1,	 							// dw   - display area width in VCK units (see 5.2 in GS docs)
					nHeight - 1));								// dh   - display area height in pixel units (TJC - must be -2 because dby==1 in DISPFB2)
		}
		else
		{
			DPUT_GS_DISPFB1(
				SCE_GS_SET_DISPFB(
					u32FrameBufferAddr,						// fbp  - base address/2048
					sCurrentFramesDisplayState.u32DisplayWidth / 64,	// fbw  - buffer width/64
					u32FramePixelFormat,						// psm  - pixel storage format
					0, 											// dbx  - xpos in buffer
					0)); 											// dby  - ypos in buffer

			DPUT_GS_DISPLAY1(
				SCE_GS_SET_DISPLAY1(
					0,		 										// dx   - x position in display buffer in VCK units
					sCurrentFramesDisplayState.u32FrameBufferDeltaY,	// dy   - y position in display buffer in raster units
					sCurrentFramesDisplayState.u32FrameBufferMagHoriz,// magh - H magnification
					sCurrentFramesDisplayState.u32FrameBufferMagVert,	// magv - V magnification
					(640 * 4) - 1,	 							// dw   - display area width in VCK units (see 5.2 in GS docs)
					nHeight - 1));								// dh   - display area height in pixel units

			DPUT_GS_DISPFB2(
				SCE_GS_SET_DISPFB(
					u32FrameBufferAddr,						// fbp  - base address/2048
					sCurrentFramesDisplayState.u32DisplayWidth / 64,	// fbw  - buffer width/64
					u32FramePixelFormat,						// psm  - pixel storage format
					0, 											// dbx  - xpos in buffer
					1)); 											// dby  - ypos in buffer

			DPUT_GS_DISPLAY2(
				SCE_GS_SET_DISPLAY2(
					0,		 										// dx   - x position in display buffer in VCK units
					sCurrentFramesDisplayState.u32FrameBufferDeltaY,	// dy   - y position in display buffer in raster units
					sCurrentFramesDisplayState.u32FrameBufferMagHoriz,// magh - H magnification
					sCurrentFramesDisplayState.u32FrameBufferMagVert,	// magv - V magnification
					(640 * 4) - 1,	 							// dw   - display area width in VCK units (see 5.2 in GS docs)
					nHeight - 2));								// dh   - display area height in pixel units (TJC - must be -2 because dby==1 in DISPFB2)
		}
	}
	else
	{
		DPUT_GS_DISPFB1(
			SCE_GS_SET_DISPFB(
				u32FrameBufferAddr,						// fbp  - base address/2048
				sCurrentFramesDisplayState.u32DisplayWidth / 64,	// fbw  - buffer width/64
				u32FramePixelFormat,						// psm  - pixel storage format
				0, 											// dbx  - xpos in buffer
				0)); 											// dby  - ypos in buffer

		DPUT_GS_DISPLAY1(
			SCE_GS_SET_DISPLAY1(
				0,		 										// dx   - x position in display buffer in VCK units
				sCurrentFramesDisplayState.u32FrameBufferDeltaY,	// dy   - y position in display buffer in raster units
				sCurrentFramesDisplayState.u32FrameBufferMagHoriz,// magh - H magnification
				sCurrentFramesDisplayState.u32FrameBufferMagVert,	// magv - V magnification
				(640 * 4) - 1,	 							// dw   - display area width in VCK units (see 5.2 in GS docs)
				nHeight - 1));								// dh   - display area height in pixel units

		DPUT_GS_DISPFB2(
			SCE_GS_SET_DISPFB(
				u32FrameBufferAddr,						// fbp  - base address/2048
				sCurrentFramesDisplayState.u32DisplayWidth / 64,	// fbw  - buffer width/64
				u32FramePixelFormat,						// psm  - pixel storage format
				0, 											// dbx  - xpos in buffer
				1)); 											// dby  - ypos in buffer

		DPUT_GS_DISPLAY2(
			SCE_GS_SET_DISPLAY2(
				4,		 										// dx   - x position in display buffer in VCK units
				sCurrentFramesDisplayState.u32FrameBufferDeltaY,	// dy   - y position in display buffer in raster units
				sCurrentFramesDisplayState.u32FrameBufferMagHoriz,// magh - H magnification
				sCurrentFramesDisplayState.u32FrameBufferMagVert,	// magv - V magnification
				(640 * 4) - 1,	 							// dw   - display area width in VCK units (see 5.2 in GS docs)
				nHeight - 2));								// dh   - display area height in pixel units (TJC - must be -2 because dby==1 in DISPFB2)
	}
#endif //PS2

	/***************************** directx version ****************************/
#ifdef DIRECTX
	{
		HRESULT	result = D3D_OK;
		DWORD		dwFlags;
#ifdef _XBOX
		if (CXboxLive::IsUIXStarted() == true)
		{
			CXboxLive::GetLiveEngine()->DoWork(&dwFlags);
			CXboxLive::UIXUpdate(dwFlags);
		}
#endif
		// End the scene
		result = DirectX_Device->EndScene( );
		ASSERT( result == D3D_OK );

		// Present the backbuffer contents to the display
		result = DirectX_Device->Present( NULL, NULL, NULL, NULL );
		if( result == D3DERR_DEVICELOST )
			bDeviceAvailable = false;

		DrawArray__SwapBuffers( );

		//DirectX__Debug( "%f FPS\n", DirectX__FrameRate() );
	}
#endif //DIRECTX
}


/* record a motion blur request for later rendering */

void gfxMotionBlur( u32 strength, u32 timothyLeary)
{
#if defined(PS2) || defined(_XBOX) || defined(GCN) || defined(DIRECTX_PC)
	DisplayState.u32MBstrength = strength;
	DisplayState.u32MBtimothyLeary = timothyLeary;
#endif //PS2

#if defined(PS2)
	DisplayState.u32MBtimothyLeary = timothyLeary * 2;
#endif

#if defined(GCN)
	if( DisplayState.u32MBstrength != 0)
	{
		TextureMgr::RequireFrontBufferCopy( );
	}
#endif
}

/* render the motion blur. this is done by drawing the previous frame buffer on top of the currently-being-rendered
frame buffer with some alpha value. the alpha value is the u32MBstrength parameter, 0..128 (0 means no effect, 128 means
full opacity). you can also specify an "enlargement" value. the is iteh u32MBtimothyLeary value. 0 means no enlargment,
so the pixels match up between the frame buffers. a value of 8 enlarges the previous frame buffer by one pixel (the
uv's are calculated using the enlargement value). this value can be large if you want a really trippy effect. the motion
blur gets inserted into the master list since bMasterListCaptureDMA is TRUE during the render, so the gs commands are
captured and inserted into the dma chain */

#ifdef GCN
static Vector3 vMBPos[4] = {Vector3( 0.0f, 0.0f, 0.0f), Vector3( 1.0f, 0.0f, 0.0f), Vector3( 1.0f, 1.0f, 0.0f), Vector3( 0.0f, 1.0f, 0.0f)};
static Vector2 vMBUV[4] = {Vector2( 0.0f, 0.0f), Vector2( 1.0f, 0.0f), Vector2( 1.0f, 1.0f), Vector2( 0.0f, 1.0f)};
#endif

void gfxRenderMotionBlur(void)
{
#if defined _XBOX || defined DIRECTX_PC

	/* anything to do? */

	if (DisplayState.u32MBstrength == 0)
	{
		return;
	}

	LPDIRECT3DDEVICE8 d = DirectX_Device;
	ASSERT( d );

	int width = DisplayState.nScreenWidth;
	int height = DisplayState.nScreenHeight;

#ifndef DIRECTX_PC
	// Snag the front buffer into a texture.
	LPDIRECT3DSURFACE8 front_buffer = NULL;
	if ( FAILED( IDirect3DDevice8_GetBackBuffer( d, -1, D3DBACKBUFFER_TYPE_MONO, &front_buffer ) ) )
	{
		TRACE( "FAILED: IDirect3DDevice8_GetBackBuffer().\n" );
		return;
	}

	IDirect3DTexture8 front_texture;
	XGSetTextureHeader( width, height, 1, 0, D3DFMT_LIN_A8R8G8B8, 0, &front_texture, 0, width * 4 );

	front_texture.Data = front_buffer->Data;
#endif

	// Scale alpha from [0,128] range into [0,1] range.
	float alpha = (float)DisplayState.u32MBstrength / 128.0f;

	// Compute uv enlargement factor.
#ifdef DIRECTX_PC
	static float scalingFactor = 1/255.0f;
	float blur = ( DisplayState.u32MBtimothyLeary * scalingFactor);	// enlargement in texel space.
#else
	float blur = ( DisplayState.u32MBtimothyLeary / 8.0f );	
#endif
	// Draw a quad over the whole screen with "enlargement".
#ifdef DIRECTX_PC
	float left = 0.0f + blur;
	float top = 0.0f + blur;
	float right = RenderToTexture::renderSurface->GetImage()->GetImageUScale() - blur;
	float bottom =RenderToTexture::renderSurface->GetImage()->GetImageVScale() - blur;
#else
	float left = 0.0f + blur;
	float top = 0.0f + blur;
	float right = ( float )width - blur;
	float bottom = ( float )height - blur;
#endif

	RenderState__Default( );
	TextureState__Default( );

	MatrixStack__Mode( MS_PROJECTION );
	MatrixStack__Push( );
	MatrixStack__Identity( );
	MatrixStack__Orthographic( 0, 1, 1, 0, -1.0f, 1.0f );

	MatrixStack__Mode(MS_MODELVIEW);
	MatrixStack__Push( );
	MatrixStack__Identity( );

	MatrixStack__Mode( MS_TEXTURE );
	MatrixStack__Push( );
	MatrixStack__Identity( );

	RenderState__Request(RS_CULL, RS_CULL_NONE);
	RenderState__Request(RS_ZTEST, RS_FALSE);
	RenderState__Request(RS_LIGHTING, RS_FALSE);

#ifndef DIRECTX_PC
	IDirect3DDevice8_SetTexture( DirectX_Device, 0, &front_texture );
#else
	if (RenderToTexture::renderSurface)
		IDirect3DDevice8_SetTexture( DirectX_Device, 0, RenderToTexture::renderSurface->GetImage()->GetPlatformImage()->getD3DTexture());
#endif
	IDirect3DDevice8_SetTextureStageState( DirectX_Device, 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
	IDirect3DDevice8_SetTextureStageState( DirectX_Device, 0, D3DTSS_TEXCOORDINDEX, 0 );

	IDirect3DDevice8_SetTextureStageState( DirectX_Device, 0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
	IDirect3DDevice8_SetTextureStageState( DirectX_Device, 0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );
	IDirect3DDevice8_SetTextureStageState( DirectX_Device, 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
	IDirect3DDevice8_SetTextureStageState( DirectX_Device, 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
	IDirect3DDevice8_SetTextureStageState( DirectX_Device, 0, D3DTSS_MIPFILTER, D3DTEXF_NONE );
   IDirect3DDevice8_SetTextureStageState( DirectX_Device, 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
   IDirect3DDevice8_SetTextureStageState( DirectX_Device, 0, D3DTSS_COLORARG2, D3DTA_CURRENT );
   IDirect3DDevice8_SetTextureStageState( DirectX_Device, 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
   IDirect3DDevice8_SetTextureStageState( DirectX_Device, 0, D3DTSS_ALPHAARG2, D3DTA_CURRENT );
   IDirect3DDevice8_SetTextureStageState( DirectX_Device, 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
   IDirect3DDevice8_SetTextureStageState( DirectX_Device, 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2 );

	//TRACE("gfxRenderMotionBlur(): alpha = %f, blur = %f\n", alpha, blur);

	Draw__Color(1.0f, 1.0f, 1.0f, alpha);
	Draw__Begin(D_QUADS);

	Draw__TexCoord(left, top);
	Draw__Vector(     0,   0, 0);

	Draw__TexCoord(left, bottom);
	Draw__Vector(     0,      1, 0);

	Draw__TexCoord(right, bottom);
	Draw__Vector(      1,      1, 0);

	Draw__TexCoord(right, top);
	Draw__Vector(      1,   0, 0);

	Draw__End();

	MatrixStack__Mode( MS_PROJECTION );
	MatrixStack__Pop();

	MatrixStack__Mode( MS_MODELVIEW );
	MatrixStack__Pop();

	MatrixStack__Mode( MS_TEXTURE );
	MatrixStack__Pop();

	// Direct3D doesn't like to release a texture if it is set, so set the
	// current texture to NULL before going out of scope.
	IDirect3DDevice8_SetTexture( DirectX_Device, 0, NULL );
#ifndef DIRECTX_PC
	IDirect3DSurface8_Release( front_buffer );
#endif  //DIRECTX_PC

#endif

#ifdef PS2

	/* anything to do? */

	if (DisplayState.u32MBstrength == 0)
		return;

	// Does this need to be here?  Yes it does, says Food & Ned
//yuch-- this needs to be removed for master list version-- the issue is that the direct-to-gs commands are overwriting
// gs registers during rendering of something else-- the usual problem
//	sceGsSyncPath(0, 0);

	int frontbuffer = DisplayState.u32FrontBufferIndex^1;
	if(Display__DownSampling())
		frontbuffer = 0;

	/* set alpha mode */

	RenderState__Request(RS_ZWRITE, false);
	GSState__Set(SCE_GS_ALPHA_1, SCE_GS_SET_ALPHA(0, 1, 2,  1, DisplayState.u32MBstrength));
//other possibilities:
//	GSState__Set(SCE_GS_ALPHA_1, SCE_GS_SET_ALPHA(0, 2, 2,  2, DisplayState.u32MBstrength));
//	GSState__Set(SCE_GS_ALPHA_1, SCE_GS_SET_ALPHA(0, 2, 2,  1, DisplayState.u32MBstrength));
	GSState__Set(SCE_GS_TEXA, SCE_GS_SET_TEXA(128, 0, 128));
	GSState__Set(SCE_GS_PABE, 0);
	GSState__Set(SCE_GS_CLAMP_1, SCE_GS_SET_CLAMP_1(1, 1, 0, 0, 0, 0));
	GSState__Set(SCE_GS_FBA_1, SCE_GS_SET_FBA(0));

	/* set up to blit the previous frame buffer into the current frame buffer as a sprite with some alpha */

	GSState__Set(SCE_GS_TEST_1, SCE_GS_SET_TEST_1(0,0,0,0,0,0,1,1));	// all tests off
	GSState__Set(SCE_GS_TEX0_1,
			 SCE_GS_SET_TEX0_1(
				DisplayState.u32FrameBufferAddr[frontbuffer],	// vram address-- the other buffer from what the master list is rendering into
				((DisplayState.nScreenWidth + 63) >> 6) & 0x3f,	// texture page width, i think
				DisplayState.u32FramePixelFormat[frontbuffer],						// pixel format
				10, 10,															// width, height (1024x1024)-- as long as it's bigger than frame buffer dimensions
				0,																	// 0=RGB(no texture alpha), 1=RGBA(Alpha texA for <32bit)
				0,																	// 0=Modulate, 1=Decal, 2 and 3 = useless highlight modes
				0, 0, 0, 0, 0 ));												// CLUT
	GSState__Set(SCE_GS_TEX1_1,
			 SCE_GS_SET_TEX1_1(
				1, 0,		// LOD=K, Max MIP level=0
				1,			// expand filter
				1,			// reduce filter
				1,
				0, 1));	// L, K
	GSState__Set(SCE_GS_PRIM, (u_long) (SCE_GS_PRIM_SPRITE | SCE_GS_PRIM_TME | SCE_GS_PRIM_FST | SCE_GS_PRIM_ABE));
	GSState__Set(SCE_GS_RGBAQ, SCE_GS_SET_RGBAQ(128, 128, 128, 128, 0x3f800000));
	GSState__Set(SCE_GS_UV, SCE_GS_SET_UV(DisplayState.u32MBtimothyLeary + 8, DisplayState.u32MBtimothyLeary + 8));	// in 12.4 fixed point, "+ 8" for center of pixel
	GSState__Set(SCE_GS_XYZ2, SCE_GS_SET_XYZ2(32768 - (DisplayState.nScreenWidth << 3), 32768 - (DisplayState.nScreenHeight << 3), 10000));
	GSState__Set(SCE_GS_UV, SCE_GS_SET_UV(-DisplayState.u32MBtimothyLeary + (DisplayState.u32DisplayWidth << 4) + 8, -DisplayState.u32MBtimothyLeary + (DisplayState.u32DisplayHeight << 4) + 8));	// in 12.4 fixed point, "+ 8" for center of pixel
	GSState__Set(SCE_GS_XYZ2, SCE_GS_SET_XYZ2(32768 + (DisplayState.nScreenWidth << 3), 32768 + (DisplayState.nScreenHeight << 3), 10000));	// this one causes draw kick

	/* and restore gs state */

	GSState__Set(SCE_GS_TEST_1, GSState.TEST_1);
	GSState__Set(SCE_GS_ALPHA_1, GSState.ALPHA_1);
	GSState__Set(SCE_GS_TEXA, GSState.TEXA);
	GSState__Send();

#endif //PS2

#ifdef GCN
	if( DisplayState.u32MBstrength == 0)
	{
		return;
	}

	LOCKUP(); // New texture manager needs this functionality, but I dont know GCN

	MatrixStack__Mode( MS_PROJECTION);
	MatrixStack__Push( );
	MatrixStack__Identity( );
	MatrixStack__Orthographic( 0.0f, 1.0f, 0.0f, 1.0f, -1.0f, 1.0f);

	MatrixStack__Mode( MS_MODELVIEW);
	MatrixStack__Push( );
	MatrixStack__Identity( );

	TextureState__RequestUnit( 0);
	MatrixStack__Mode( MS_TEXTURE);
	MatrixStack__Push( );
	MatrixStack__Identity( );

	DrawArray__VectorPointer( 3, DA_FLOAT, 0, vMBPos);
	DrawArray__TexturePointer( 2, DA_FLOAT, 0, vMBUV);
	DrawArray__Enable( DA_VECTOR);
	DrawArray__Enable( DA_TEXTURE);
	DrawArray__Disable( DA_COLOR);
	DrawArray__Disable( DA_NORMAL);

	RenderState__Request( RS_CULL, RS_CULL_NONE);
	RenderState__Request( RS_ZTEST, RS_FALSE);
	RenderState__Request( RS_LIGHTING, RS_FALSE);

	unsigned char color[4];

	color[0] = 255;	//r
	color[1] = 255;	//g
	color[2] = 255;	//b
	color[3] = Math::Min( (int)( DisplayState.u32MBstrength * 2), (int)255);	//a

	RenderState__Request( RS_BASECOLOR, RenderState__Colorubv( color));

	float blur = ( DisplayState.u32MBtimothyLeary / 8.0f);	// enlargement in texel space.
	float blurWidth = blur / DisplayState.nScreenWidth;
	float blurHeight = blur / DisplayState.nScreenHeight;

	float left = 0.0f + blurWidth;
	float top = 0.0f + blurHeight;
	float right = 1.0 - blurWidth;
	float bottom = 1.0 - blurHeight;

	vMBUV[0].Set( left, top);
	vMBUV[1].Set( right, top);
	vMBUV[2].Set( right, bottom);
	vMBUV[3].Set( left, bottom);

	DCFlushRange( vMBUV, sizeof vMBUV);

	s16 bind;
	LOCKUP(); // New texture manager needs this functionality
	TextureState__RequestAll( 0, TS_TYPE_2D, bind, TS_METHOD_MODULATE, TS_WRAP_CLAMP, TS_WRAP_CLAMP, TS_FILTER_BILINEAR);
	TextureState__RequestAll( 1, TS_TYPE_NONE, 0, 0, 0, 0, 0 );

	g_ShaderMgr.ResetCurrentMatrixIndices( );

	DrawArray__Render( DA_QUADS, 0, 4);
	MatrixStack__Mode( MS_PROJECTION);
	MatrixStack__Pop( );

	MatrixStack__Mode( MS_MODELVIEW);
	MatrixStack__Pop( );

	TextureState__RequestUnit( 0);
	MatrixStack__Mode( MS_TEXTURE);
	MatrixStack__Pop( );
#endif //GCN
}


#ifdef _XBOX
bool gfxBootStrapXBOX( void )
{
	if ( ! Platform__Framebuffer( 0, 24, 24, 0, 0, 1 ) )
	{
		return FALSE;
	}

	DisplayState.pD3D 		= DirectX_D3D;
	DisplayState.pD3DDevice = DirectX_Device;

	return TRUE;
}
#endif

#ifdef WIN32
#ifndef _XBOX
static HWND Display__CreateWindow( void )
{
    HWND hWnd;
    WNDCLASS wc;
    DWORD style;
    int width, height;

    // Register the window class
    ZeroMemory( &wc, sizeof ( WNDCLASS ) );
    wc.lpfnWndProc   = DisplayState.winProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = DisplayState.hInstance;
    if (DisplayState.hIcon == NULL)
        wc.hIcon     = LoadIcon(NULL, IDI_APPLICATION);
    else
        wc.hIcon     = DisplayState.hIcon;
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = DisplayState.szClassName;

    wc.style         = CS_CLASSDC | CS_DBLCLKS;

    if ( RegisterClass( &wc ) == 0 )
    {
        DisplayState.u32LastWinError = GetLastError();
        return NULL;
    }

    style = WS_OVERLAPPEDWINDOW;

    // Adjust the window size so that the client area is the desired size
    // (otherwise the whole window including any decorations (i.e.,
    // borders) will be the size specified).
    {
        RECT r = { 0, 0, DisplayState.nScreenWidth, DisplayState.nScreenHeight };

        AdjustWindowRect( &r, style, FALSE );
        width  = r.right - r.left;
        height = r.bottom - r.top;
    }

    hWnd = CreateWindow( DisplayState.szClassName, DisplayState.szClassName,
                         style, 0, 0, width, height,
                         DisplayState.hParentWnd, NULL, DisplayState.hInstance, NULL );

    if ( hWnd == NULL )
    {
        DisplayState.u32LastWinError = GetLastError();
        return NULL;
    }

    DisplayState.u32WindowState = SW_SHOWNORMAL; //indicate that the window was created as a normal window

    return hWnd;
}

static void Display__UpdateWindow(void)
{
	RECT Rect;

	ShowWindow(DisplayState.hWnd, DisplayState.u32WindowState);
	GetClientRect(DisplayState.hWnd, &Rect);
	DisplayState.nScreenWidth = Rect.right;
	DisplayState.nScreenHeight = Rect.bottom;
	MatrixStack__FullScreenViewport();
	UpdateWindow(DisplayState.hWnd);
}

/* this just cleans up after the window has been destroyed */

bool Display__DestroyWindow(void)
{
	return(UnregisterClass(DisplayState.szClassName, DisplayState.hInstance) != FALSE);
}

/* pc only -- handle windows messages */

void gfxSetWindowText( char* s )
{
	char *	t = "DirectX 0wNz";
	char		p[ 256 ];

#ifndef DIRECTX_PC
	sprintf( p, "%s (%s)", s, t );
#else
	sprintf( p, "%s", s );
#endif

	SetWindowText( DisplayState.hWnd, p );
}

bool gfxOnCreate( HWND hWnd )
{
	return TRUE;
}

bool gfxOnCreated( HWND hWnd )
{
	if ( ! Platform__Framebuffer( hWnd, 24, 24, 0, 0, 1 ) )
	{
		return FALSE;
	}

	DisplayState.pD3D 		= DirectX_D3D;
	DisplayState.pD3DDevice = DirectX_Device;

	return TRUE;
}

void gfxOnDestroy( void)
{
	Platform__Destroy();
}

void gfxOnResize( short sWidth, short sHeight)
{
	/* record new size of window */

#ifdef DIRECTX
	if( (DisplayState.nScreenWidth != sWidth)  ||  (DisplayState.nScreenHeight != sHeight) )
	{
		DisplayState.nScreenWidth = sWidth;
		DisplayState.nScreenHeight = sHeight;

		DXPublished::releaseDeviceObjects( );
		DrawArray__Terminate( );

		Platform__Resize( sWidth, sHeight );

		DXPublished::restoreDeviceObjects( );
		DrawArray__Initialize( );
	}
#else
	DisplayState.nScreenWidth = sWidth;
	DisplayState.nScreenHeight = sHeight;
#endif

#ifdef DIRECTX_PC
	// delete and re init our render textures
	RenderToTexture::DeleteRenderTexture();
	RenderToTexture::CreateRenderTexture(DisplayState.nScreenWidth, DisplayState.nScreenHeight, DisplayState.backBufferFormat );
#endif
	/* update game view scenes for new aspect ratio */
	CScene *pScene = SceneMgr::GetFirstScene(SCENE_TYPE_ALL);
	while (pScene)
	{
		pScene->d_sceneView.ResetViewport( );	// only acts if scene has a valid viewport
		pScene = SceneMgr::GetNextScene(pScene, SCENE_TYPE_ALL);
	}
}
#endif
#endif //WIN32

/* screen shot */

#ifndef CONSUMER_BUILD
void gfxSnapScreen(void)
{
	bSnapScreen = TRUE;	// get it next time around-- checked in gfxStartRender
}

//===== TGA support ==========================================================

#ifdef WIN32

// TGA File Header
#pragma pack (push,1)
struct t_TGAHEADER
{
    u8      IdLength;       // Image ID Field Length
    u8      CmapType;       // Color Map Type
    u8      ImageType;      // Image Type

    // Color Map Specification
    u16     CmapIndex;      // First Entry Index
    u16     CmapLength;     // Color Map Length
    u8      CmapEntrySize;  // Color Map Entry Size

    // Image Specification
    u16     X_Origin;       // X-origin of Image
    u16     Y_Origin;       // Y-origin of Image
    u16     ImageWidth;     // Image Width
    u16     ImageHeight;    // Image Height
    u8      PixelDepth;     // Pixel Depth
    u8      ImagDesc;       // Image Descriptor
};
#pragma pack (pop)

void _gfxWriteTga( FILE* fp, char* pixelData, int width, int height )
{
	t_TGAHEADER tgaHead;

	ASSERT_PTR( fp );
	ASSERT_PTR( pixelData );
	ASSERT( width > 0 );
	ASSERT( height > 0 );

	memset( &tgaHead, 0, sizeof( tgaHead ) );
	tgaHead.IdLength      = 0;
	tgaHead.CmapType      = 0;
	tgaHead.ImageType     = 2;       // unencoded RGB
	tgaHead.CmapIndex     = 0;
	tgaHead.CmapLength    = 0;
	tgaHead.CmapEntrySize = 0;
	tgaHead.X_Origin      = 0;
	tgaHead.Y_Origin      = 0;
	tgaHead.ImageWidth    = width;
	tgaHead.ImageHeight   = height;
	tgaHead.PixelDepth    = 24;
	tgaHead.ImagDesc      = 0;		// 24 bits/pixel

	fwrite( &tgaHead, sizeof(t_TGAHEADER), 1, fp );

	// tga has inverted data
	for ( int y = 0 ; y < height ; y++ )
	{
		char* row = pixelData + ( height - 1 - y ) * width * 3;
		int x;

		// write out, toggle R and B.
		for ( x = 0 ; x < width ; x++ ) { char t = row[ x * 3 + 0 ]; row[ x * 3 + 0 ] = row[ x * 3 + 2 ]; row[ x * 3 + 2 ] = t; }
		fwrite( row, width * 3, 1, fp );
		for ( x = 0 ; x < width ; x++ ) { char t = row[ x * 3 + 0 ]; row[ x * 3 + 0 ] = row[ x * 3 + 2 ]; row[ x * 3 + 2 ] = t; }
	}
}
#endif

static void _gfxSnapScreen(void)
{
/***************************** pc version ********************************/

#ifdef WIN32
	// Generate a unique filename (don't clobber screens already snapped).
	char filename[ 256 ];
	{
		#ifndef _XBOX
			char* directory = "ScreenSnaps";
		#else
			char* directory = "D:\\snaps";
		#endif

		CreateDirectory( directory, NULL );

		strcpy( filename, directory );
		strcat( filename, "\\" );

		#if defined (DIRECTX)
		strcat( filename, "screen*.bmp" );
		#else
		strcat( filename, "screen*.tga" );
		#endif

		WIN32_FIND_DATA fd;
		HANDLE h = FindFirstFile( filename, &fd );

		int n = 0;
		if ( h != INVALID_HANDLE_VALUE )
		{
			n++;
			while ( FindNextFile( h, &fd ) )
			{
				n++;
			}
		}

		#if defined (DIRECTX)
		sprintf( filename, "%s\\screen%04d.bmp", directory, n );
		#else
		sprintf( filename, "%s\\screen%04d.tga", directory, n );
		#endif
	}


	#if defined (DIRECTX)
	HRESULT hr;

	// get display dimensions
	// this will be the dimensions of the front buffer
	D3DDISPLAYMODE mode;
	hr = DirectX_Device->GetDisplayMode(&mode);

	if (hr != D3D_OK)
		return;

	// create the image surface to store the front buffer image
	// note that call to GetFrontBuffer will always convert format to A8R8G8B8
	LPDIRECT3DSURFACE8	surf;
	#if defined (_XBOX)
	hr = DirectX_Device->CreateImageSurface(mode.Width,mode.Height,D3DFMT_LIN_A8R8G8B8,&surf);
	#else
	hr = DirectX_Device->CreateImageSurface(mode.Width,mode.Height,D3DFMT_A8R8G8B8,&surf);
	#endif

	if (hr != D3D_OK)
		return;

	// read the front buffer into the image surface
	#if defined (_XBOX)
	hr = DirectX_Device->GetBackBuffer(-1, D3DBACKBUFFER_TYPE_MONO, &surf);
	#else
	hr = DirectX_Device->GetFrontBuffer(surf);
	#endif

	if (hr != D3D_OK)
	{
		surf->Release();
		return;
	}

	// write the entire surface to the requested file
	#if !defined (_XBOX)
//	hr = D3DXSaveSurfaceToFile(filename,D3DXIFF_BMP,surf,NULL,NULL);		// TO check with I can run DX
	#else
	hr = XGWriteSurfaceToFile(surf, filename);
	#endif

	if (hr != D3D_OK)
	{
		surf->Release();
		return;
	}

	surf->Release();
	return;
	#endif

	// Grab the image data.
	int width = DisplayState.nScreenWidth;
	int height = DisplayState.nScreenHeight;
	unsigned char* image = new unsigned char[ width * height * 3 ];

	FILE* f = fopen( filename, "wb" );
	if ( f )
	{
		/* may want to reduce resolution */

		if (DisplayState.nScreenSnapReduceFactor != 1)
		{
			ASSERT(DisplayState.nScreenWidth % DisplayState.nScreenSnapReduceFactor == 0);	// must be evenly divisible
			ASSERT(DisplayState.nScreenHeight % DisplayState.nScreenSnapReduceFactor == 0);	// must be evenly divisible
			int nNewWidth = width / DisplayState.nScreenSnapReduceFactor;
			int nNewHeight = height / DisplayState.nScreenSnapReduceFactor;
			unsigned char *pReducedBuffer = new unsigned char[nNewWidth * nNewHeight * 3];
			unsigned char *pDest = pReducedBuffer;
			for (int y = 0; y < height; y += DisplayState.nScreenSnapReduceFactor)
			{
				unsigned char *pSource = &image[y * width * 3];
				for (int x = 0; x < width; x += DisplayState.nScreenSnapReduceFactor)
				{
					memcpy(pDest, pSource, 3);
					pSource += DisplayState.nScreenSnapReduceFactor * 3;
					pDest += 3;
				}
			}

			/* update to new (reduced) image */

			delete [] image;
			image = pReducedBuffer;
			width = nNewWidth;
			height = nNewHeight;
		}

//		fprintf( f, "P6\n%d %d\n255\n", width, height );
//		fwrite( image, width * height * 3, 1, f );
		_gfxWriteTga( f, (char*)image, width, height );
		fclose( f );

		#ifndef _XBOX
		SetWindowText(DisplayState.hWnd, filename);
		#endif

		TRACE( "_gfxSnapScreen(): snapped screen to \"%s\".\n", filename );
	}
	else
	{
		TRACE( "_gfxSnapScreen(): can't open file \"%s\"\n", filename );
	}

	delete [] image;

#endif //WIN32


#ifdef PS2

	static int snapSource = 0;	// 0 = back-buffer (draw), 1 = front-buffer (display), 2 = z-buffer

	int format, CBWidth, CBHeight, nBitDepth, frameBufferAddr;
	int stride, shiftR, shiftG, shiftB, shiftUp, mask;

	if ( snapSource == 2 ) // ZBuffer
	{
		CBWidth         = DisplayState.u32FrameBufferWidth;
		CBHeight        = DisplayState.u32FrameBufferHeight;
		nBitDepth       = DisplayState.u32ZBufferBPP;
		frameBufferAddr = DisplayState.u32ZBufferAddr;
		switch ( nBitDepth )
		{
			default: ASSERTS( false, "unknown bit depth" );
			case 16:	format = SCE_GS_PSMZ16; stride = 2; shiftR = shiftG = shiftB = 8;  shiftUp = 0; mask = 0x0F; break;
			case 24: format = SCE_GS_PSMZ24; stride = 3; shiftR = shiftG = shiftB = 16; shiftUp = 0; mask = 0xFF; break;
			case 32: format = SCE_GS_PSMZ32; stride = 4; shiftR = shiftG = shiftB = 24; shiftUp = 0; mask = 0xFF; break;
		}
	}
	else
	{
		if ( snapSource == 0 )	// Back Buffer
		{
			CBWidth         = DisplayState.u32FrameBufferWidth;
			CBHeight        = DisplayState.u32FrameBufferHeight;
			nBitDepth       = DisplayState.u32FrameBufferBPP[ DisplayState.u32FrontBufferIndex ^ 1 ];
			frameBufferAddr = DisplayState.u32FrameBufferAddr[ DisplayState.u32FrontBufferIndex ^ 1 ];
		}
		else // Front Buffer
		{
			CBWidth         = DisplayState.u32DisplayWidth;
			CBHeight        = DisplayState.u32DisplayHeight;
			nBitDepth       = DisplayState.u32FrameBufferBPP[ DisplayState.u32FrontBufferIndex ];
			frameBufferAddr = DisplayState.u32FrameBufferAddr[ DisplayState.u32FrontBufferIndex ];
		}
		switch ( nBitDepth )
		{
			default: ASSERTS( false, "unknown bit depth" );
			case 16:	format = SCE_GS_PSMCT16S; stride = 2; shiftR =  0; shiftG = 5; shiftB = 10; shiftUp = 3; mask = 0x1F; break;
			case 24: format = SCE_GS_PSMCT24;  stride = 4; shiftR =  0; shiftG = 8; shiftB = 16; shiftUp = 0; mask = 0xFF; break;
			case 32: format = SCE_GS_PSMCT32;  stride = 4; shiftR =  0; shiftG = 8; shiftB = 16; shiftUp = 0; mask = 0xFF; break;
		}
	}

	extern const char* g_RemoteIPAddress;
	bool bTTY = ( g_RemoteIPAddress == NULL ); // if running under ATWINMON then g_RemoteIPAddress is not null.

	// Generate a new file name
	static int nSnaps = 0;
	char cName[ 128 ];
	sprintf( cName, "%sScreenSnaps\\Screen%04d.ppm", bTTY ? "host:" : "", nSnaps++ );

	if(bTTY)
		printf("\nStarting screen dump to file %s\n", cName);
	else
		dbgPrint("\nStarting screen dump to file %s\n", cName);

	t_Error error;
	int fd = sceOpen(cName, SCE_WRONLY | SCE_TRUNC | SCE_CREAT);
	MediaFile* mHandle = Media::Open(cName, &error, MEDOPEN_FLAG_WRITE);

	// to create image file.
	if ( ( fd >= 0 ) || mHandle )
	{
		// write PPM header
		char cHdrBuf[64];
		sprintf( cHdrBuf, "P6\n%d %d\n255\n", CBWidth, CBHeight);
		if ( bTTY )
			sceWrite( fd, cHdrBuf, strlen(cHdrBuf) );
		else
			mHandle->Write( cHdrBuf, strlen(cHdrBuf), &error );

		Display__Shutdown( false );

		// grab the frame buffer in multiple blocks, upper half and lower half.
		#define BLOCKS 2
		int blockHeight = CBHeight / BLOCKS;
		u_long128*     dst1    = (u_long128*) malloc( CBWidth * blockHeight * 4 );
		unsigned char* pRGBBuf = (unsigned char*) malloc( CBWidth * blockHeight * 3 );
		for ( int block = 0 ; block < BLOCKS ; block++ )
		{
			// grab the data from the frame buffer
			sceGsStoreImage gs_simage;
			sceGsSetDefStoreImage( &gs_simage, frameBufferAddr, CBWidth / 64, format, 0, block * blockHeight, CBWidth, blockHeight );
			FlushCache( WRITEBACK_DCACHE );
			sceGsExecStoreImage( &gs_simage, dst1 );
			sceGsSyncPath( 0, 0 );

			// convert RGBA (frame buffer format) to RGB (file format)
			u8* pDstBuf = pRGBBuf;
			u8* pSrcBuf = (u8*) dst1;
			for ( int y = 0; y < blockHeight ; y++ )
			{
				for ( int x = 0; x < CBWidth ; x++ )
				{
					u32 data = ( pSrcBuf[0] ) | ( pSrcBuf[1] << 8 ) | ( pSrcBuf[2] << 16 ) | ( pSrcBuf[3] << 24 );
					pDstBuf[ 0 ] = ( ( data >> shiftR ) & mask ) << shiftUp;
					pDstBuf[ 1 ] = ( ( data >> shiftG ) & mask ) << shiftUp;
					pDstBuf[ 2 ] = ( ( data >> shiftB ) & mask ) << shiftUp;
					pSrcBuf += stride;
					pDstBuf += 3;
				}
			}

			if ( bTTY )
			{
				// tgps2run seems to have problems with scan line at a time writes - do a 3rd of a line at a time
				for ( int y = 0 ; y < blockHeight ; y++ )
				{
					sceWrite( fd, (void*) &pRGBBuf[ CBWidth * y * 3 + CBWidth * 0 ], CBWidth );
					sceWrite( fd, (void*) &pRGBBuf[ CBWidth * y * 3 + CBWidth * 1 ], CBWidth );
					sceWrite( fd, (void*) &pRGBBuf[ CBWidth * y * 3 + CBWidth * 2 ], CBWidth );
				}
			}
			else
			{
				// write the screen buffer out
				mHandle->Write( pRGBBuf, CBWidth * blockHeight * 3, &error );
			}
		}

		if ( bTTY )
		{
			sceClose( fd );
			printf( "%s completed\n", cName );
		}
		else
		{
			mHandle->Close( &error );
			dbgPrint( "%s completed\n", cName );
		}

		Display__Init();

		free( dst1 );
		free( pRGBBuf );
	}
	else
	{
		if ( bTTY )
			printf( "\ncan't open %s\n", cName );
		else
			dbgPrint( "\ncan't open %s\n", cName );
	}
#endif //PS2


#ifdef DIRECTX
    // DXTODO
#endif

#ifdef GCN
#if defined(DVDETH)

	static int			nSnaps = 0;

	char					cName[20];

	u16					CBWidth = (u16)DisplayState.nScreenWidth,
							CBHeight = (u16)DisplayState.nScreenHeight;

	unsigned char		*pImage = (unsigned char *)memAlloc( CBWidth * CBHeight * 3),
							*pDest;

	strcpy( cName, "Scrn");
	sprintf( cName + strlen(cName), "%04d", nSnaps++ );	// Append tick count
	strcat( cName, ".ppm" );					// Append .PPM extension

	FileStream			*pScreenFile = FileStream::NewAndOpen( cName, FileStream::WRITE);
	ASSERT( pScreenFile);					// this had better succeed

	pDest = pImage;
	for( u16 row = 0; row < CBHeight; row++)
	{
		for( u16 col = 0; col < CBWidth; col++)
		{
			u32	color;

			GXPeekARGB( col, row, &color);
			pDest[0] = ( color >> 16) & 0xFF;
			pDest[1] = ( color >>  8) & 0xFF;
			pDest[2] = ( color >>  0) & 0xFF;
			pDest += 3;
		}
	}

	char	acHeader[40];

	bool	bResult;

	sprintf( acHeader, "P6\n%d %d\n255\n", CBWidth, CBHeight);
	bResult = pScreenFile->Write( acHeader, strlen( acHeader));
	ASSERT( bResult);

	u32 writeCount = CBWidth * CBHeight * 3;

	bResult = pScreenFile->Write( pImage, writeCount);
	ASSERT( bResult);

	pScreenFile->Close( );
	delete pScreenFile;
	memFree( pImage);
#endif //dvdeth
#endif //gcn
}

/* ps2 only at this point, written to test if the image we get when we read a texture back from vram is in "clear"
format, that is, not swizzled. may also be useful for checking generated textures... */

#ifdef PS2

#if 0
void gfxSnapTexture(int bind)
{
	static int nTextureSnaps = 0;
	char cName[128];

	/* get texture info */

	u32 u32Width = TextureBind__GetWidth(bind);
	u32 u32Height = TextureBind__GetHeight(bind);
	u32 u32Format = TextureBind__GetFormat(bind);
	u32 u32Bytes = TextureBind__GetSize(bind);
	if ((u32Format != SCE_GS_PSMCT32) && (u32Format != SCE_GS_PSMCT16))
	{
		printf("Can only snap 16- and 32-bit textures\n");
		return;
	}

	/* generate a new file name */

	strcpy( cName, "host:ScreenSnaps\\Texture" );
	sprintf( cName + strlen(cName), "%04d", nTextureSnaps++ );	// Append tick count
	strcat( cName, ".ppm" );					// Append .PPM extension

	/* open image file */

	int fd;
	if ((fd = sceOpen(cName, SCE_WRONLY | SCE_TRUNC | SCE_CREAT)) >= 0)
	{
		/* malloc buffer to hold texture image dma'd from vram */

		void *pImage = memAlloc(u32Bytes);

		/* grab texture image */

		TextureBind__Read(bind, pImage);

		/* malloc file buffer (rgb) */

		unsigned char *pRGBBuf;
		pRGBBuf = (unsigned char *) memAlloc(u32Width * u32Height * 3);

		/* convert to rgb file format */

		unsigned char *pSrcBuf, *pDstBuf;
		int x,y;

		switch (u32Format)
		{
		case SCE_GS_PSMCT32:
			/* convert 32-bit texture image to 24-bit RGB (file format) */

			pDstBuf = pRGBBuf;
			pSrcBuf = (unsigned char *) pImage;
			for( y = 0; y < (signed) u32Height; y++)
			{
				for( x = 0; x < (signed) u32Width; x++)
				{
					*(pDstBuf) = *(pSrcBuf);
					*(pDstBuf+1) = *(pSrcBuf+1);
					*(pDstBuf+2) = *(pSrcBuf+2);
					pDstBuf += 3;
					pSrcBuf += 4;
				}
			}
			break;
		case SCE_GS_PSMCT16:
			/* convert 16-bit texture image to 24-bit RGB (file format) */

			pDstBuf = pRGBBuf;
			u16 *pu16SrcBuf = (u16 *) pImage;
			for( y = 0; y < (signed) u32Height; y++)
			{
				for( x = 0; x < (signed) u32Width; x++)
				{
					*(pDstBuf++) = (*(pu16SrcBuf) << 3) & 0xf8;
					*(pDstBuf++) = (*(pu16SrcBuf) >> 2) & 0xf8;
					*(pDstBuf++) = (*(pu16SrcBuf) >> 7) & 0xf8;
					pu16SrcBuf += 1;
				}
			}
			break;
		}

		char cHdrBuf[64];
		sprintf( cHdrBuf, "P6\n%d %d\n255\n", u32Width, u32Height);
		sceWrite(fd, cHdrBuf, strlen(cHdrBuf));
		//tgps2run seems to have problems with scan line at a time writes - do a 3rd of a line at a time
		for( y = 0; y < (signed) u32Height; y++)
		{
			sceWrite(fd, (void *)&(pRGBBuf[u32Width * y * 3]), u32Width);
			sceWrite(fd, (void *)&(pRGBBuf[u32Width * y * 3 + u32Width]), u32Width);
			sceWrite(fd, (void *)&(pRGBBuf[u32Width * y * 3 + 2 * u32Width]), u32Width);
		}

		sceClose(fd);
		memFree(pImage);
		memFree(pRGBBuf);
		printf("%s completed\n", cName);
	}
	else
		printf("\ncan't open %s\n", cName);

}
#endif	// not turned on in new texture manager

#endif //PS2

#endif //CONSUMER_BUILD

#if 0
void gfxReadTextureMemory(u32 u32TMemAddress, u32 u32Height, void **pImage, char *TSName)
{
#ifdef PS2

	sceGsStoreImage gs_simage;
	u32 u32ChunkLength, u32ChunkHeight;
	s32 s32Height = u32Height;
	char *pcImage;

	MEM_SET_ALLOC_FILENAME("SNAP", TSName);
	*pImage = pcImage = (char*) memAlloc(PSMCT16_VRAM_PAGE_WIDTH * u32Height * PSMCT16_BYTES_PER_PIXEL);

	/* can only do 2048 high chunks */
	while (s32Height > 0)
	{
		/*prepare to read this chunk*/
		u32ChunkHeight = Math::Min(2048, s32Height);
		sceGsSetDefStoreImage(&gs_simage, (short)u32TMemAddress, (64/64), SCE_GS_PSMCT16, 0, 0, 64, u32ChunkHeight);
		FlushCache(WRITEBACK_DCACHE);

		/*read teh chunk*/
		sceGsExecStoreImage(&gs_simage, (u_long128 *)pcImage);
		sceGsSyncPath(0, 0);

		/* prepare for the next chunk */
		u32ChunkLength = u32ChunkHeight * PSMCT16_VRAM_PAGE_WIDTH * PSMCT16_BYTES_PER_PIXEL;//Hx64Wx2Bpp in
		u32TMemAddress += u32ChunkLength / 256;//inc VRAM ptr, VRAM addresses are in units of 256 bytes
		pcImage += u32ChunkLength;				//inc SysRAM ptr
		s32Height -= u32ChunkHeight;				//dec amount left to do
	}

#endif //PS2
}
#endif	// not turned on in new texture manager

#if 0
void gfxWriteTextureMemory(u32 u32TMemAddress, u32 u32Height, void *pImage)
{
#ifdef PS2
	u32 u32ChunkLength, u32ChunkHeight;
	s32 s32Height = u32Height;
	char *pcImage = (char *)pImage;

	/* can only do 2048 high chunks */
	while (s32Height > 0)
	{
		/* download current chunk */
		u32ChunkHeight = Math::Min(2048, s32Height);
		TextureBind__DMAImageToVRAM(u32TMemAddress, 64, u32ChunkHeight, pcImage, SCE_GS_PSMCT16);

		/* prepare for the next chunk */

		u32ChunkLength = u32ChunkHeight * PSMCT16_VRAM_PAGE_WIDTH * PSMCT16_BYTES_PER_PIXEL;//Hx64Wx2Bpp in
		u32TMemAddress += u32ChunkLength / 256;//inc VRAM ptr, VRAM addresses are in units of 256 bytes
		pcImage += u32ChunkLength;					//inc SysRAM ptr
		s32Height -= u32ChunkHeight;				//dec amount left to do
	}

	SceneMgr::GetStats().IncTextureLoadCount( 1 );
	SceneMgr::GetStats().IncTextureLoadBytes( PSMCT16_VRAM_PAGE_WIDTH * u32Height * PSMCT16_BYTES_PER_PIXEL );
#endif //PS2
}
#endif		// not turned on in new texture manager

 /* -----------------7/29/98 4:45PM-------------------
 * Local Data
 * --------------------------------------------------*/

#if 0
GLfloat light0_position[] = {1.0f, 1.0f, 1.0f, 0.0f};		// infinite directional light. last value == 0.0 means directional light
//GLfloat light_color[] = {0.5f, 1.0f, 0.5f, 0.0f};		// infinite directional light's color
GLfloat light0_color[] = {1.5f, 1.5f, 1.5f, 0.0f};			// infinite directional light's diffuse color
GLfloat light0a_color[] = {4.0f, 4.0f, 4.0f, 0.0f};		// infinite directional light's ambient color
GLfloat light1_position[] = {-1.0f, 1.0f, -1.0f, 0.0f};	// infinite directional light. last value == 0.0 means directional light
GLfloat light1_color[] = {0.25f, 0.25f, 0.5f, 0.0f};		// infinite directional light's color
#endif

/* -----------------7/29/98 4:38PM-------------------
 * Exported Code
 * --------------------------------------------------*/

#define DRAW_GRID
#ifdef DRAW_GRID
//djr: I'd sure like to see a grid sometimes...
void gfxDrawGrid()
{
#define GRID_XMIN (-500)
#define GRID_XMAX (-GRID_XMIN)
#define GRID_XINTERVAL (50)

#define GRID_YMIN (-500)
#define GRID_YMAX (-GRID_YMIN)
#define GRID_YINTERVAL (50)


	float x,y;
	float fColor, fColorXInterval, fColorYInterval;

	//glPushAttrib( GL_ALL_ATTRIB_BITS );

	//undo the last local transform (put grid at world origin)
    MatrixStack__Mode( MS_MODELVIEW );
    MatrixStack__Load( &g4WorldToCamera );

	//make the lines colored (instead of textured)
	//glDisable( GL_TEXTURE_1D);
	//glDisable( GL_TEXTURE_2D);

    TextureState__Default();

	//the shade of the line will show the negative and positive axes
	fColorXInterval = 1.0f / ((GRID_XMAX - GRID_XMIN) / GRID_XINTERVAL);
	fColorYInterval = 1.0f / ((GRID_YMAX - GRID_YMIN) / GRID_YINTERVAL);

	//glBegin( GL_LINES);
	Draw__Begin( D_LINES );

	for( x=GRID_XMIN, fColor = 0.0f; x<=GRID_XMAX; x+=GRID_XINTERVAL, fColor+=fColorXInterval)
	{
		//glColor3f( fColor, 0.0f, 0.0f);	//x-axis grows redder in positive dir
		Draw__Color( fColor, 0, 0, 1 );
//#define XYFLOOR 1
#ifdef XYFLOOR
		//glVertex3f( x, GRID_YMIN, 0.0f);
		//glVertex3f( x, GRID_YMAX, 0.0f);
		Draw__Vector( x, GRID_YMIN, 0 );
		Draw__Vector( x, GRID_YMAX, 0 );
#else
		//glVertex3f( x, 0.0f, GRID_YMIN);
		//glVertex3f( x, 0.0f, GRID_YMAX);
		Draw__Vector( x, 0, GRID_YMIN );
		Draw__Vector( x, 0, GRID_YMAX );
#endif
	}

	for( y=GRID_YMIN, fColor = 0.0f; y<=GRID_YMAX; y+=GRID_YINTERVAL, fColor+=fColorYInterval)
	{
		//glColor3f( 0.0f, fColor, 0.0f);	//y-axis grows greener in positive dir
		Draw__Color( 0, fColor, 0, 1 );
#ifdef XYFLOOR
		//glVertex3f( GRID_XMIN, y, 0.0f);
		//glVertex3f( GRID_XMAX, y, 0.0f);
		Draw__Vector( GRID_XMIN, y, 0 );
		Draw__Vector( GRID_XMAX, y, 0 );
#else
		//glVertex3f( GRID_XMIN, 0.0f, y);
		//glVertex3f( GRID_XMAX, 0.0f, y);
		Draw__Vector( GRID_XMIN, 0, y );
		Draw__Vector( GRID_XMAX, 0, y );
#endif
	}

	//draw major axes
	//X axis
	//glColor3f( 0.5f, 0.5f, 0.5f);	//do negative major axis in gray
	//glVertex3f( 2*GRID_XMIN, 0.0f, 0.0f);
	//glVertex3f( 0.0f, 0.0f, 0.0f);
	//glColor3f( 1.0f, 1.0f, 1.0f);	//do positive major axis in white
	//glVertex3f( 0.0f, 0.0f, 0.0f);
	//glVertex3f( 2*GRID_XMAX, 0.0f, 0.0f);
	Draw__Color( 0.5f, 0.5f, 0.5f, 1.0f );	//do negative major axis in gray
	Draw__Vector( 2*GRID_XMIN, 0.0f, 0.0f );
	Draw__Vector( 0.0f, 0.0f, 0.0f );
	Draw__Color( 1.0f, 1.0f, 1.0f, 1.0f );	//do positive major axis in white
	Draw__Vector( 0.0f, 0.0f, 0.0f );
	Draw__Vector( 2*GRID_XMAX, 0.0f, 0.0f );

	//Y axis
#ifdef XYFLOOR
	//glColor3f( 0.5f, 0.5f, 0.5f);	//do negative major axis in gray
	//glVertex3f( 0.0f, 2.0f*GRID_YMIN, 0.0f);
	//glVertex3f( 0.0f, 0.0f, 0.0f);
	//glColor3f( 1.0f, 1.0f, 1.0f);	//do positive major axis in white
	//glVertex3f( 0.0f, 0.0f, 0.0f);
	//glVertex3f( 0.0f, 2.0f*GRID_YMAX, 0.0f);
	Draw__Color( 0.5f, 0.5f, 0.5f, 1.0f );	//do negative major axis in gray
	Draw__Vector( 0.0f, 2.0f*GRID_YMIN, 0.0f );
	Draw__Vector( 0.0f, 0.0f, 0.0f );
	Draw__Color( 1.0f, 1.0f, 1.0f, 1.0f );	//do positive major axis in white
	Draw__Vector( 0.0f, 0.0f, 0.0f );
	Draw__Vector( 0.0f, 2.0f*GRID_YMAX, 0.0f );
#else
	//glColor3f( 0.5f, 0.5f, 0.5f);	//do negative major axis in gray
	//glVertex3f( 0.0f, 0.0f, 2.0f*GRID_YMIN);
	//glVertex3f( 0.0f, 0.0f, 0.0f);
	//glColor3f( 1.0f, 1.0f, 1.0f);	//do positive major axis in white
	//glVertex3f( 0.0f, 0.0f, 0.0f);
	//glVertex3f( 0.0f, 0.0f, 2.0f*GRID_YMAX);
	Draw__Color( 0.5f, 0.5f, 0.5f, 1.0f );	//do negative major axis in gray
	Draw__Vector( 0.0f, 0.0f, 2.0f*GRID_YMIN );
	Draw__Vector( 0.0f, 0.0f, 0.0f );
	Draw__Color( 1.0f, 1.0f, 1.0f, 1.0f );	//do positive major axis in white
	Draw__Vector( 0.0f, 0.0f, 0.0f );
	Draw__Vector( 0.0f, 0.0f, 2.0f*GRID_YMAX );
#endif

	//draw up/down axis
#ifdef XYFLOOR
	//glVertex3f( 0.0f, 0.0f, 2.0f*GRID_XMIN);
	//glVertex3f( 0.0f, 0.0f, 0.0f);
	//glColor3f( 1.0f, 1.0f, 1.0f);	//do positive major axis in white
	//glVertex3f( 0.0f, 0.0f, 0.0f);
	//glVertex3f( 0.0f, 0.0f, 2.0f*GRID_XMAX);
	Draw__Color( 0.5f, 0.5f, 0.5f, 1.0f );	//do negative major axis in gray
	Draw__Vector( 0.0f, 0.0f, 2.0f*GRID_XMIN );
	Draw__Vector( 0.0f, 0.0f, 0.0f );
	Draw__Color( 1.0f, 1.0f, 1.0f, 1.0f );	//do positive major axis in white
	Draw__Vector( 0.0f, 0.0f, 0.0f );
	Draw__Vector( 0.0f, 0.0f, 2.0f*GRID_XMAX );
#else
	//glColor3f( 0.5f, 0.5f, 0.5f);	//do negative major axis in gray
	//glVertex3f( 0.0f, 2.0f*GRID_XMIN, 0.0f);
	//glVertex3f( 0.0f, 0.0f, 0.0f);
	//glColor3f( 1.0f, 1.0f, 1.0f);	//do positive major axis in white
	//glVertex3f( 0.0f, 0.0f, 0.0f);
	//glVertex3f( 0.0f, 2.0f*GRID_XMAX, 0.0f);
	Draw__Color( 0.5f, 0.5f, 0.5f, 1.0f );	//do negative major axis in gray
	Draw__Vector( 0.0f, 2.0f*GRID_XMIN, 0.0f );
	Draw__Vector( 0.0f, 0.0f, 0.0f );
	Draw__Color( 1.0f, 1.0f, 1.0f, 1.0f );	//do positive major axis in white
	Draw__Vector( 0.0f, 0.0f, 0.0f );
	Draw__Vector( 0.0f, 2.0f*GRID_XMAX, 0.0f );
#endif

	//glEnd( );
	Draw__End();
//yuch-- according to open gl mailing list, SwapBuffers calls glFlush
//  	glFlush( );

	//glPopAttrib();
	RenderState__Default();
}
#endif

//yuch-- this will be pc-specific implementation
#ifdef WIN32
#ifndef _XBOX
void gfxToggleWindowMaximize( )
{
	if(DisplayState.u32WindowState == SW_SHOWMAXIMIZED)
		gfxWindowNormal();
	else
		gfxWindowMaximize();
}

void gfxWindowMaximize()
{
	while(ShowCursor(FALSE) >= 0)
		;

	DisplayState.u32WindowState = SW_SHOWMAXIMIZED;
	Display__UpdateWindow();
}

void gfxWindowNormal( )
{
	while(ShowCursor(TRUE) < 0)
		;

	DisplayState.u32WindowState = SW_SHOWNORMAL;
	Display__UpdateWindow();
}
#endif
#endif //WIN32

/* -----------------7/29/98 5:14PM-------------------
 * Local Routines
 * --------------------------------------------------*/

#ifndef CONSUMER_BUILD
/* output debug info about graphics initialization */
void gfxDebug( void )
{
}
#endif //CONSUMER_BUILD

/* set the frame buffer-- not needed for pc, since swapping buffers keeps track of back buffer */

void gfxSetFrameBuffer(u32 u32VRAMAddr, u32 u32Width, u32 u32Height, u32 u32Format)
{
/***************************** ps2 version ********************************/
#ifdef PS2
	ASSERT((u32VRAMAddr & 31) == 0);	// frame buffer must be on 2048 vram-word boundary
	//ASSERT((u32Width & 63) == 0);			// width must be divisible by 64
	// naw, just wastes space if it isnt.. should probably be >= 64 through..
	GSState.FRAME_1 = SCE_GS_SET_FRAME_1(
		u32VRAMAddr / 32,			// fbp, frame buffer base pointer
		(u32Width+63) / 64,					// fbw, frame buffer width
		u32Format,						// psm, pixel format
		0);								// fbmsk, frame buffer drawing mask, 32 bits, 0 means enable all writes
	GSState__Set(SCE_GS_FRAME_1, GSState.FRAME_1);

	/* update frame buffer size (this helps support rendering to a texture) */

	DisplayState.u32FrameBufferWidth = u32Width;
	DisplayState.u32FrameBufferHeight = u32Height;
	GSState.XYOFFSET_1 = SCE_GS_SET_XYOFFSET((2048 - (u32Width / 2)) << 4,		// ofx (12.4 fixed point)
														  (2048 - (u32Height / 2)) << 4);	// ofy (12.4 fixed point)
	GSState__Set(SCE_GS_XYOFFSET_1, GSState.XYOFFSET_1);
#endif //PS2
}

int DisplayPlatform__GetClearMode(u32 u32ClearMode)
{
	int c = u32ClearMode;

#ifdef DIRECTX
	c = 0;
	if (u32ClearMode & RS_COLOR_BUFFER_BIT)
	{
		c |= D3DCLEAR_TARGET;
	}
	if (u32ClearMode & RS_DEPTH_BUFFER_BIT)
	{
		c |= D3DCLEAR_ZBUFFER;
	}

#ifdef _XBOX
	// On Xbox, it is _ALWAYS_ faster to set the clear stencil bit even if 
	// there is no stencil buffer.
	c |= D3DCLEAR_STENCIL;
#endif

#endif

	return c;
}

#ifdef PS2
void gfxDownSampleFrameBuffer()
{
	DMA_Wait(D1_CHCR);
	DMA_Wait(D2_CHCR);
	DPUT_D2_MADR((u32)&u128GfxDownSamplePacket[0]);
	DPUT_D2_QWC(DOWNSAMPLE_PACKET_SIZE);
	DPUT_D2_CHCR(D_CHCR_DIR_M | D_CHCR_STR_M);
	DMA_Wait(D2_CHCR);
}
#endif

/* clear frame buffer */

void gfxClearFrameBuffer(u32 u32ClearMode, ts_bRGBA *pColor, ts_Viewport *pViewport)
{
//#ifndef CONSUMER_BUILD
	/* always clear frame buffer in wireframe mode */

	if (RenderMgr__WireframeOn())
		u32ClearMode |= RS_COLOR_BUFFER_BIT;
//#endif //CONSUMER_BUILD

	/* maybe nothing to do */

	if (u32ClearMode == 0)
		return;

	/* make sure we can write to the z-buffer */

	if (u32ClearMode & RS_DEPTH_BUFFER_BIT)
		RenderState__Request(RS_ZWRITE, RS_TRUE);

/***************************** ps2 version ********************************/
#ifdef PS2

	u64 FRAME_1;
	u64 ZBUF_1;
	u64 TEST_1;

	/* if not clearing frame buffer, disable writes to it */

	if ((u32ClearMode & RS_COLOR_BUFFER_BIT) == 0)
	{
		FRAME_1 = GSState.FRAME_1 | (0xffffffffL << GS_FRAME_FBMSK_O);	// disable frame buffer writes
		GSState__Set(SCE_GS_FRAME_1, FRAME_1);
	}

	/* set up z-buffer clear */

   if (u32ClearMode & RS_DEPTH_BUFFER_BIT)
	{
		/* enable z-buffer writes, pixels always pass z-test */

		ZBUF_1 = GSState.ZBUF_1 & ~GS_ZBUF_ZMSK_M;	// enable z-buffer writes
		TEST_1 = SCE_GS_SET_TEST(
			0,		// ate, alpha test enable, 0 = off, 1 = on
			0,		// atst, alpha test method, 3 bits
			0,		// aref, alpha reference value for alpha test, 8 bits
			0,		// afail, what to do when alpha test fails, 2 bits
			0,		// date, destination alpha test, 0 = off, 1 = on
			0,		// datm, destination alpha test mode, 1 bit
			1,		// zte, depth test, 0 = off, 1 = on
			1);	// ztst, depth test method, 0 = never, 1 = always, 2 = gequal, 3 = greater
	}
	else
	{
		/* disable z-buffer writes, turn off z-test for possible frame buffer clear */

		ZBUF_1 = GSState.ZBUF_1 | (1L << GS_ZBUF_ZMSK_O);	// disable z-buffer writes
		TEST_1 = SCE_GS_SET_TEST(
			0,		// ate, alpha test enable, 0 = off, 1 = on
			0,		// atst, alpha test method, 3 bits
			0,		// aref, alpha reference value for alpha test, 8 bits
			0,		// afail, what to do when alpha test fails, 2 bits
			0,		// date, destination alpha test, 0 = off, 1 = on
			0,		// datm, destination alpha test mode, 1 bit
			1,		// zte, depth test, 0 = off, 1 = on
			1);	// ztst, depth test method, 0 = never, 1 = always, 2 = gequal, 3 = greater
	}
   GSState__Set(SCE_GS_ZBUF_1, ZBUF_1);
   GSState__Set(SCE_GS_TEST_1, TEST_1);

	// TJC - we set these every frame just incase they got changed and someone
	// forgot to change them back...  In addition, Sony has never said that these
	// have a default state when the system starts up (that I know of).
	GSState__Set(SCE_GS_PABE, 0);
	GSState__Set(SCE_GS_PRMODECONT, 1);

	GSState__Set(SCE_GS_SCISSOR_1, SCE_GS_SET_SCISSOR_1(0, 
																		 DisplayState.nScreenWidth-1, 
																		 0, 
																		 DisplayState.nScreenHeight-1));

	/* draw a rectangle in correct color and at proper z-depth to clear frame buffer and z-buffer */

	u32 u32X = 2048 - (DisplayState.u32FrameBufferWidth / 2);
	u32 u32Y = 2048 - (DisplayState.u32FrameBufferHeight / 2);

	/* a sprite is top left and bottom right corner, and colors to go with each */

	GSState__Set(
		SCE_GS_PRIM,
		SCE_GS_SET_PRIM(
			SCE_GS_PRIM_SPRITE,			// prim(3)-- primitive type
			0,									// iip(1)-- shading method (0 = flat, 1 = gouraud)
			0,									// tme(1)-- texture mapping enable
			0,									// fge(1)-- fogging enable
			0,									// abe(1)-- alpha blending enable
			0,									// aa1(1)-- antialiasing enable
			0,									// fst(1)-- texture coord (0 = STQ, 1 = UV)
			0,									// ctxt(1)-- context
			0)									// fix(1)-- fragment value control(0 = normal, 1 = fixed)
			);
	GSState__Set(
		SCE_GS_RGBAQ,
		SCE_GS_SET_RGBAQ(
			pColor->R,						// r
			pColor->G,						// g
			pColor->B,						// b
			pColor->A,						// a (128 = 1.0)
			0)									// q
			);
	GSState__Set(
		SCE_GS_XYZ2,
		SCE_GS_SET_XYZ2(
			(u32X << 4),					// x (12.4 fixed point)
			(u32Y << 4),					// y (12.4 fixed point)
			GSState.u32ZBufferClear)	// z (32.0 fixed point)
			);
	GSState__Set(
		SCE_GS_RGBAQ,
		SCE_GS_SET_RGBAQ(
			pColor->R,						// r
			pColor->G,						// g
			pColor->B,						// b
			pColor->A,						// a (128 = 1.0)
			0)									// q
			);
	GSState__Set(
		SCE_GS_XYZ2,
		SCE_GS_SET_XYZ2(
			((u32X + DisplayState.u32FrameBufferWidth) << 4),	// x (12.4 fixed point)
			((u32Y + DisplayState.u32FrameBufferHeight) << 4),	// y (12.4 fixed point)
			GSState.u32ZBufferClear)									// z (32.0 fixed point)
			);

	/* restore original states */

	GSState__Set(SCE_GS_FRAME_1, GSState.FRAME_1);
	GSState__Set(SCE_GS_ZBUF_1, GSState.ZBUF_1);
	GSState__Set(SCE_GS_TEST_1, GSState.TEST_1);

	/* and send it */

	GSState__Send();

#endif //PS2

	/***************************** directx version ****************************/
#ifdef DIRECTX
	// DX_TODO - jlb Clear Color Buffer until rendering workds
	u32ClearMode |= RS_COLOR_BUFFER_BIT;

	if (u32ClearMode & RS_COLOR_BUFFER_BIT)
		DirectX__ClearColor(pColor->R, pColor->G, pColor->B, pColor->A);

	LPDIRECT3DDEVICE8 d = DisplayState.pD3DDevice;
	ASSERT( d );

	// Clear the backbuffer
	HRESULT result = IDirect3DDevice8_Clear( d, 0, NULL, DisplayPlatform__GetClearMode(u32ClearMode), DirectX_ClearColor, 1.0f, 0 );
	ASSERT( result == D3D_OK );
#endif

#ifdef GCN
	GXColor				gxcolorClear;

	GXRenderModeObj	*pGXRenderMode;

	pGXRenderMode = DisplayState.pGXRenderMode;

	if( pViewport == NULL)		//assume we are clearing the entire frame buffer
	{
		GXSetDispCopySrc( 0, 0, pGXRenderMode->fbWidth, pGXRenderMode->efbHeight);
	}
	else
	{
		GXSetDispCopySrc(	pGXRenderMode->fbWidth * pViewport->fX,
								pGXRenderMode->efbHeight * pViewport->fY,
								pGXRenderMode->fbWidth * pViewport->fWidth,
								pGXRenderMode->efbHeight * pViewport->fHeight);
	}
	
	
	GXSetDispCopyDst( 0, 0);

	gxcolorClear.r = pColor->R;
	gxcolorClear.g = pColor->G;
	gxcolorClear.b = pColor->B;
	gxcolorClear.a = pColor->A;

	GXSetCopyClear( gxcolorClear, GX_MAX_Z24);

	if( !( u32ClearMode & RS_COLOR_BUFFER_BIT))
	{
		GXSetColorUpdate( GX_FALSE);
	}

	if( u32ClearMode & RS_DEPTH_BUFFER_BIT)
	{
		RenderState__Request( RS_ZWRITE, RS_TRUE);
	}
	else
	{
		RenderState__Request( RS_ZWRITE, RS_FALSE);
	}

	GXCopyDisp( g_pClearFrameCopyBuffer, GX_TRUE);

	GXSetColorUpdate( GX_TRUE);
#endif //GCN
}

#ifdef GCN
static void gcnPreRetraceCallback( u32 u32RetraceCount)
{
	u32GCNVBlankCount++;
	u32GCNTimeout++;
	if( g_pNextFrameBuffer != NULL)
	{
		VISetNextFrameBuffer( g_pNextFrameBuffer);
		g_pNextFrameBuffer = NULL;
		VIFlush( );
	}
}

static void gcnVerifyCallback( GXWarningLevel level, u32 id, const char *msg)
{
	if( id == 94 || id == 28)
	{
		return;
	}
	OSReport( "Level %d, Warning %03d: %s\n", level, id, msg);
	if( id == 113)
	{
		while( 1);
	}
}

static void DEMOSetGPHangMetric( GXBool enable)
{
#ifdef EMU
    #pragma unused (enable)
#else
    if (enable)
    {
        // Ensure other counters are off
        GXSetGPMetric( GX_PERF0_NONE, GX_PERF1_NONE );

        // Set up RAS Ready counter
        GXWGFifo.u8  = GX_LOAD_BP_REG;
        GXWGFifo.u32 = 0x2402c004; // ... 101 10000 00000 00100

        // Set up SU Ready counter
        GXWGFifo.u8  = GX_LOAD_BP_REG;
        GXWGFifo.u32 = 0x23000020; // ... 100 000
    
        // Set up XF TOP and BOT busy counters
        GXWGFifo.u8  = GX_LOAD_XF_REG;
        GXWGFifo.u16 = 0x0000;
        GXWGFifo.u16 = 0x1006;
        GXWGFifo.u32 = 0x00084400; // 10000 10001 00000 00000
    }
    else
    {
        // Disable RAS counters
        GXWGFifo.u8  = GX_LOAD_BP_REG;
        GXWGFifo.u32 = 0x24000000;

        // Disable SU counters
        GXWGFifo.u8  = GX_LOAD_BP_REG;
        GXWGFifo.u32 = 0x23000000;

        // Disable XF counters
        GXWGFifo.u8  = GX_LOAD_XF_REG;
        GXWGFifo.u16 = 0x0000;
        GXWGFifo.u16 = 0x1006;
        GXWGFifo.u32 = 0x00000000;
    }
#endif
}

/*---------------------------------------------------------------------------*
    Name:           __DEMODiagnoseHang
    
    Description:    Reads performance counters (which should have been set
                    up appropriately already) in order to determine why the
                    GP hung.  The counters must be set as follows:

                    GXSetGPHangMetric( GX_TRUE );
    
                    The above call actually sets up multiple counters, which
                    are read using a non-standard method.

    Arguments:      None
    
    Returns:        None
 *---------------------------------------------------------------------------*/
static void __DEMODiagnoseHang( void)
{
    u32 xfTop0, xfBot0, suRdy0, r0Rdy0;
    u32 xfTop1, xfBot1, suRdy1, r0Rdy1;
    u32 xfTopD, xfBotD, suRdyD, r0RdyD;
    GXBool readIdle, cmdIdle, junk;

    // Read the counters twice in order to see which are changing.
    // This method of reading the counters works in this particular case.
    // You should not use this method to read GPMetric counters.
    GXReadXfRasMetric( &xfBot0, &xfTop0, &r0Rdy0, &suRdy0 );
    GXReadXfRasMetric( &xfBot1, &xfTop1, &r0Rdy1, &suRdy1 );

    // XF Top & Bot counters indicate busy, others indicate ready.
    // Convert readings into indications of who is ready/idle.
    xfTopD = (xfTop1 - xfTop0) == 0;
    xfBotD = (xfBot1 - xfBot0) == 0;
    suRdyD = (suRdy1 - suRdy0) > 0;
    r0RdyD = (r0Rdy1 - r0Rdy0) > 0;

    // Get CP status
    GXGetGPStatus(&junk, &junk, &readIdle, &cmdIdle, &junk);

    OSReport("GP status %d%d%d%d%d%d --> ",
             readIdle, cmdIdle, xfTopD, xfBotD, suRdyD, r0RdyD);

    // Depending upon which counters are changing, diagnose the hang.
    // This may not be 100% conclusive, but it's what we've observed so far.
    if (!xfBotD && suRdyD)
    {
        OSReport("GP hang due to XF stall bug.\n");
    }
    else if (!xfTopD && xfBotD && suRdyD)
    {
        OSReport("GP hang due to unterminated primitive.\n");
    }
    else if (!cmdIdle && xfTopD && xfBotD && suRdyD)
    {
        OSReport("GP hang due to illegal instruction.\n");
    }
    else if (readIdle && cmdIdle && xfTopD && xfBotD && suRdyD && r0RdyD)
    {
        OSReport("GP appears to be not hung (waiting for input).\n");
    }
    else
    {
        OSReport("GP is in unknown state.\n");
    }
}

static void gxDrawSyncCallback( u16 token)
{
	lastToken = token;
}

#endif //GCN

#ifdef DIRECTX_PC	

void gfxBlitTextureToScreen(IDirect3DTexture8 *texture, float left,
														float top,
														float right,
														float bottom )
{
	if (texture == NULL)
		return;
	LPDIRECT3DDEVICE8 d = DirectX_Device;
	ASSERT( d );

	RenderState__Default( );
	TextureState__Default( );

	MatrixStack__Mode( MS_PROJECTION );
	MatrixStack__Push( );
	MatrixStack__Identity( );
	MatrixStack__Orthographic( 0, 1, 1, 0, -1.0f, 1.0f );

	MatrixStack__Mode(MS_MODELVIEW);
	MatrixStack__Push( );
	MatrixStack__Identity( );

	MatrixStack__Mode( MS_TEXTURE );
	MatrixStack__Push( );
	MatrixStack__Identity( );

	RenderState__Request(RS_CULL, RS_CULL_NONE);
	RenderState__Request(RS_ZTEST, RS_FALSE);
	RenderState__Request(RS_LIGHTING, RS_FALSE);

	// set our texture to draw with
	IDirect3DDevice8_SetTexture( DirectX_Device, 0, texture);

	IDirect3DDevice8_SetTextureStageState( DirectX_Device, 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
	IDirect3DDevice8_SetTextureStageState( DirectX_Device, 0, D3DTSS_TEXCOORDINDEX, 0 );

	IDirect3DDevice8_SetTextureStageState( DirectX_Device, 0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
	IDirect3DDevice8_SetTextureStageState( DirectX_Device, 0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );
	IDirect3DDevice8_SetTextureStageState( DirectX_Device, 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
	IDirect3DDevice8_SetTextureStageState( DirectX_Device, 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
	IDirect3DDevice8_SetTextureStageState( DirectX_Device, 0, D3DTSS_MIPFILTER, D3DTEXF_NONE );
	IDirect3DDevice8_SetTextureStageState( DirectX_Device, 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	IDirect3DDevice8_SetTextureStageState( DirectX_Device, 0, D3DTSS_COLORARG2, D3DTA_CURRENT );
	IDirect3DDevice8_SetTextureStageState( DirectX_Device, 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	IDirect3DDevice8_SetTextureStageState( DirectX_Device, 0, D3DTSS_ALPHAARG2, D3DTA_CURRENT );
	IDirect3DDevice8_SetTextureStageState( DirectX_Device, 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
	IDirect3DDevice8_SetTextureStageState( DirectX_Device, 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2 );

	Draw__Color(1.0f, 1.0f, 1.0f, 1.0f);
	Draw__Begin(D_QUADS);

	Draw__TexCoord(left, top);
	Draw__Vector(     0,   0, 0);

	Draw__TexCoord(left, bottom);
	Draw__Vector(     0,      1, 0);

	Draw__TexCoord(right, bottom);
	Draw__Vector(      1,      1, 0);

	Draw__TexCoord(right, top);
	Draw__Vector(      1,   0, 0);

	Draw__End();

	MatrixStack__Mode( MS_PROJECTION );
	MatrixStack__Pop();

	MatrixStack__Mode( MS_MODELVIEW );
	MatrixStack__Pop();

	MatrixStack__Mode( MS_TEXTURE );
	MatrixStack__Pop();

	// Direct3D doesn't like to release a texture if it is set, so set the
	// current texture to NULL before going out of scope.
	IDirect3DDevice8_SetTexture( DirectX_Device, 0, NULL );
}


Texture *RenderToTexture::renderSurface = NULL;
bool	RenderToTexture::renderingToTexture = false;
bool	RenderToTexture::isRenderingToTextureOn = false;
long	RenderToTexture::renderSurfaceUsedHeight = 0;
long	RenderToTexture::renderSurfaceUsedWidth = 0;

void	RenderToTexture::SetUpForRenderToTexture() 
{

}

bool RenderToTexture::CreateRenderTexture( int height, int width, D3DFORMAT backBufferFormat)
{
	// eventually we should blow away the other one and make a new one
	if (renderSurface)
	{
		delete renderSurface;
		renderSurface = NULL;
	}

	u16 imageFormat;

	switch(backBufferFormat)
	{
	case D3DFMT_R5G6B5:
		imageFormat = Image::SIZE_16_BIT;
		imageFormat |= Image::TYPE_RGB;		
		break;
	case D3DFMT_X8R8G8B8:
		imageFormat = Image::SIZE_32_BIT;
		imageFormat |= Image::TYPE_RGB;		
		break;
	default:
		imageFormat = Image::SIZE_32_BIT;
		imageFormat |= Image::TYPE_RGBA;
		break;
	}

	renderSurface = TextureMgr::CreateSpecialTexture(
														0,
														Image::RENDER_TARGET,
														imageFormat,
														height,
														width, "RenderTexture" );
	if (renderSurface)
		return true; 
	return false;
}

bool RenderToTexture::DeleteRenderTexture()
{
	if (renderSurface)
	{
		delete renderSurface;
		renderSurface = NULL;
	}

	return true;

}

void RenderToTexture::BlitTextureToScreen()
{
	// copy our backbuffer contents to our texture
	LPDIRECT3DSURFACE8 tempSurface;
	HRESULT result = DirectX_Device->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &tempSurface);
	ASSERT( result == D3D_OK );
	LPDIRECT3DSURFACE8 renderTargetSurface = NULL;
	result = RenderToTexture::renderSurface->GetImage()->GetPlatformImage()->getD3DTexture()->GetSurfaceLevel(0, &renderTargetSurface);
	
	// get some info
	D3DSURFACE_DESC backbufferDesc;
	D3DSURFACE_DESC renderSurfaceDesc;
	tempSurface->GetDesc(&backbufferDesc);
	renderTargetSurface->GetDesc(&renderSurfaceDesc);
	ASSERT( backbufferDesc.Format == renderSurfaceDesc.Format );
	ASSERT( backbufferDesc.Usage == renderSurfaceDesc.Usage );

	ASSERT( result == D3D_OK );
	ASSERT( renderTargetSurface );
	RECT sizeRect;
	sizeRect.top = 0;
	sizeRect.left = 0;
	Image * tempRenderSurfaceImage = RenderToTexture::renderSurface->GetImage();
	sizeRect.bottom = backbufferDesc.Height;
	sizeRect.right = backbufferDesc.Width;
	result = DirectX_Device->CopyRects( tempSurface, &sizeRect, 1, renderTargetSurface, NULL );
	ASSERT( result != D3DERR_INVALIDCALL );
	tempSurface->Release();
	renderTargetSurface->Release();

	//gfxBlitTextureToScreen(RenderToTexture::renderSurface->GetImage()->GetPlatformImage()->getD3DTexture());
}

#endif // DIRECTX_PC