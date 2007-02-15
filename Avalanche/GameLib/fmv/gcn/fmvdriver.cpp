/////////////////////////////////////////////////////////////////////////////
// This is the driver for FMV video
/////////////////////////////////////////////////////////////////////////////

#include <string.h>
#include <dolphin/gx.h>
#include <dolphin/dvd.h>
#include <dolphin/thp.h>
#include "platform/BaseType.h"
#include "Audio/audiolib.h"
#include "Layers/Media.h"
#include "platform/CompilerControl.h"	// for NAMED_SECTION
#include "fmv/fmv.h"
#include "fmv/gcn/fmvdriver.h"
#include "fmv/gcn/THPPlayer.h"
#include "Layers/Memory.h"
#include "Display/Common/DisplayState.h"
#include "Display/Common/GfxFunc.h"
#include "Display/Common/RenderState.h"
#include "Display/Common/TextureState.h"
#include "Display/Common/MatrixStack.h"
#include "Display/Common/DrawArray.h"
#include "Display/GCN/Shader.h"
#include "Game/Managers/RenderMgr.h"
#include "hud/hudlib.h"

#if USE_THP == 0
#include "fmv/gcn/bink/bink.h"
#endif

void * PlayThread( void *pData);

static u8							*sg_pWorkArea;

static char							*sg_pFMVName;

static FMVDriver::AbortFn 		sg_Abort;

static void							*sg_pAbortContext;

static t_Error						sg_DiskError;

static bool							sg_bMovieInitComplete;

#define PLAYER_STACK_SIZE		( 32 * 1024)

#if USE_THP == 1
static s32							sg_volume;

#else
class CMovieScreenError
{
	public:
		CMovieScreenError( )
		{
			m_Bkgd.AddChild( &m_Text);
			m_Bkgd.SetTextureSet( Font::SystemFont( )->m_texture.handle, TS_NO_TEXTURE);
			m_Bkgd.SetVertexColor( 0, 0.0f, 0.0f, 0.0f, 1.0f);
			m_Bkgd.SetVertexColor( 1, 0.0f, 0.0f, 0.0f, 1.0f);
			m_Bkgd.SetVertexColor( 2, 0.0f, 0.0f, 0.0f, 1.0f);
			m_Bkgd.SetVertexColor( 3, 0.0f, 0.0f, 0.0f, 1.0f);
			m_Bkgd.SetPosition( 0.0f, 0.0f);
			m_Bkgd.SetWidth( Display__GetScreenWidth( ));
			m_Bkgd.SetHeight( Display__GetScreenHeight( ));
			m_Bkgd.SetPriority( 0xf00d);

			m_Text.SetOrigin( 0.5f, 0.5f);
			m_Text.ScreenPosition( 0.5f, 0.5f);
			m_Text.SetColor( 1.0f, 1.0f, 1.0f, 1.0f);
			m_Text.SetWordWrap( 640.0f * 0.80f);

			m_Bkgd.ShowAll( );
			m_Bkgd.Update( );
		}
	public:
		Sprite		m_Bkgd;
		Text			m_Text;
};

static float		sg_volume;

static HBINK		sg_hBink;

static u16			sg_binkTexBind;

static u32			sg_binkARAMBase;

static DiskErrorTextCallback	sg_DiskErrorTextFunc;

static Vector3 vBinkPos[4] = {Vector3( 0.0f, 0.0f, 0.0f), Vector3( 1.0f, 0.0f, 0.0f), Vector3( 1.0f, 1.0f, 0.0f), Vector3( 0.0f, 1.0f, 0.0f)};
static Vector2 vBinkUV[4] = {Vector2( 0.0f, 0.0f), Vector2( 1.0f, 0.0f), Vector2( 1.0f, 1.0f), Vector2( 0.0f, 1.0f)};

// added for sg_hBink stuff (PAB)
static void * binkMemAlloc( U32 bytes)
{
	return( memAlloc( bytes, 32));
}

static void binkMemFree( void *ptr)
{
	memFree( ptr);
}

static void * binkARAMAlloc( U32 nBytes)
{

	if( sg_binkARAMBase == 0xFFFFFFFF)
	{
		LOCKUP( );
	}

	void *pReturn = (void *)sg_binkARAMBase;

	nBytes = OSRoundUp32B( nBytes);
	sg_binkARAMBase += nBytes;

	return( pReturn);
}

static void binkARAMFree( void *ptr)
{
	return;
}

static void SetupFMVGraphicEnv( void);
#endif

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
FMVDriver::FMVDriver( void)
{
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
FMVDriver::~FMVDriver( void)
{
}

/////////////////////////////////////////////////////////////////////////////
// Make a full filename of what an FMV would be on this platform
/////////////////////////////////////////////////////////////////////////////
void FMVDriver::MakeFile( const char *fmv, char *fullFile)
{
	strcpy( fullFile, Directory( ));
	strcat( fullFile, fmv);
	strcat( fullFile, Extension( ));

	char *p = fullFile;
	while( *p)
	{
		if( *p == '\\')
			*p = '/';
		p++;
	}
}

/////////////////////////////////////////////////////////////////////////////
// Is this a valid/existing fmv for this driver?
// Note, pass in fmv name, not the full path
/////////////////////////////////////////////////////////////////////////////
bool FMVDriver::Valid( const char *fmv)
{
	// Build the file name
	char pathFile[128];

	MakeFile( fmv, pathFile);

	// We probably should validate it a little
	return( DVDConvertPathToEntrynum( pathFile) != -1);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool FMVDriver::Create( float volume,  DiskErrorTextCallback DiskErrorTextFunc)
{
#if USE_THP != 0
	sg_volume = (s32)Math::Min( 127.0f, 127.0f * volume);
	THPPlayerInit( WITH_AX);
#else
	RADARAMCALLBACKS binkARAMCallbacks = { binkARAMAlloc, binkARAMFree};

	// setup sg_hBink to use our memory functions
	RADSetMemory( binkMemAlloc, binkMemFree);

	sg_volume = volume;

	sg_binkARAMBase = 0xFFFFFFFF;

	sg_hBink = NULL;

	BinkSoundUseAX( &binkARAMCallbacks);

	sg_binkTexBind = NO_BIND;

	SetupFMVGraphicEnv( );

	sg_DiskErrorTextFunc = DiskErrorTextFunc;
#endif

	//remove breakpoints from double buffering system
	GXFlush( );
	GXDisableBreakPt( );

	//wait for GX to catch up with us
	GXDrawDone( );

	return( true);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void FMVDriver::Destroy( void)
{
	GXFlush( );

	void		*pNextBreakpoint,
				*pRead;

	GXGetFifoPtrs( DisplayState.pGXFIFO, &pRead, &pNextBreakpoint);
	GXEnableBreakPt( pNextBreakpoint);

	//re-establish breakpoint for double buffering
	gfxEndRender( );

#if USE_THP != 0
	THPPlayerQuit( );
#else
	MatrixStack__Mode( MS_PROJECTION);
	MatrixStack__Pop( );

	MatrixStack__Mode( MS_MODELVIEW);
	MatrixStack__Pop( );

	TextureState__RequestUnit( 0);
	MatrixStack__Mode( MS_TEXTURE);
	MatrixStack__Pop( );
#endif
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool FMVDriver::Play( const char *fmv, FMVSoundTrack soundTrack, bool &aborted, AbortFn abort, void *abortContext)
{

	u32					movieXPos,
							movieYPos;

	char					pathFile[128];

	OSThread				osthread;

	char					*pStack;

#ifdef DVDETH
	return( true);
#endif

#if USE_THP == 1
	sg_pWorkArea = NULL;
#else
	sg_binkARAMBase = OSRoundUp32B( SoundDriverGCN::GetSoundDriverGCN( )->GetAramTOS( ));
#endif

	aborted = false;

	MakeFile( fmv, pathFile);

	sg_pFMVName = pathFile;

	sg_Abort = abort;

	sg_pAbortContext = abortContext;

	sg_DiskError = MED_ERR_NONE;

	sg_bMovieInitComplete = false;

	pStack = (char *)memAlloc( PLAYER_STACK_SIZE);

	if( !OSCreateThread(	&osthread,
								PlayThread,
								NULL,
								(void *)( pStack + ( PLAYER_STACK_SIZE - 8)),
								PLAYER_STACK_SIZE,
								20,
								0))
	{
		memFree( pStack);
		aborted = true;
		return( false);
	}

	OSResumeThread( &osthread);

	while( true)
	{

		s32		dvdState;

		if( OSIsThreadTerminated( &osthread))
		{

			void	*pReturnValue;

			OSJoinThread( &osthread, &pReturnValue);

			memFree( pStack);
#if USE_THP != 0
			memFree( sg_pWorkArea);
#else
			if( sg_binkTexBind != NO_BIND)
			{
				TextureBind__Destroy( sg_binkTexBind);
			}
			sg_binkTexBind = NO_BIND;
#endif
			return( true);
		}

		if( sg_DiskError != MED_ERR_NONE && !sg_bMovieInitComplete)
		{
			OSCancelThread( &osthread);
#if USE_THP != 0
			THPPlayerStop( );
			THPPlayerClose( );
#else
			if( sg_hBink != NULL)
			{
				BinkClose( sg_hBink);
			}
			sg_hBink = NULL;
			if( sg_binkTexBind != NO_BIND)
			{
				TextureBind__Destroy( sg_binkTexBind);
			}
			sg_binkTexBind = NO_BIND;
#endif
			aborted = true;
			return( true);
		}

		dvdState = DVDGetDriveStatus( );

		t_Error	diskError;

		diskError = MediaHAL::TranslateError( dvdState, MED_ERR_NONE);

		if( diskError != MED_ERR_NONE)
		{
			sg_DiskError = diskError;
		}
		else
		if( sg_DiskError != MED_ERR_NONE && DVDCheckDisk( ))
		{
			sg_DiskError = diskError;
		}

		VIWaitForRetrace( );
	}
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void * PlayThread( void *pParam)
{

	u32					movieXPos,
							movieYPos;

	THPVideoInfo		thpVI;

	bool					localAborted;

#if USE_THP == 0
	RenderMgr__SetNumVBlanksToSyncTo( 1);
#endif

#if USE_THP != 0
	// Open THP file
	if( THPPlayerOpen( sg_pFMVName, false, sg_volume) == FALSE)
	{
		return( NULL);
	}

	THPPlayerGetVideoInfo( &thpVI);

	movieXPos = ( Display__GetScreenWidth( ) - thpVI.xSize) / 2;
	movieYPos = ( Display__GetScreenHeight( ) - thpVI.ySize) / 2;

	// Set aside work area
	sg_pWorkArea = (u8 *)memAlloc( THPPlayerCalcNeedMemory( ), 32);
	ASSERT( sg_pWorkArea);

	// Set work area
	THPPlayerSetBuffer( sg_pWorkArea);

	// Preparation before playback
	if( THPPlayerPrepare( 0, NONE) == FALSE)
	{
		return( NULL);
	}

	// Start playback
	THPPlayerPlay( );
#else

	// open bink file
	sg_hBink = BinkOpen( sg_pFMVName, 0);

	if( !sg_hBink)
	{
		return( NULL);
	}

	BinkSetVolume( sg_hBink, 0, (s32)( sg_volume * 32768.0f));

	s32	texWidth = sg_hBink->Width,
			texHeight = sg_hBink->Height;

	ts_TextureChunkRecord TextureChunkRecord;

	sg_binkTexBind = TextureBind__Create( );

	if( sg_binkTexBind == NO_BIND)
	{
		return( NULL);
	}

	memset( &TextureChunkRecord, 0, sizeof(ts_TextureChunkRecord));

	TextureChunkRecord.s16TextureWidth = texWidth;
	TextureChunkRecord.s16TextureHeight = texHeight;
	TextureChunkRecord.u32Flags = TEX_FLAG_FMT_RGBA | TEX_FLAG_SIZE_32_BIT;

	TextureBind__Cache( sg_binkTexBind, 0, &TextureChunkRecord, true);

	void		*pBinkTexAddr = TextureBind__GetAddress( sg_binkTexBind);

	u32		binkTexSize = TextureBind__GetSize( sg_binkTexBind);

	TextureState__RequestAll( 0, TS_TYPE_2D, sg_binkTexBind, TS_METHOD_REPLACE, TS_WRAP_CLAMP, TS_WRAP_CLAMP, TS_FILTER_BILINEAR);

	if(	texWidth < Display__GetScreenWidth( ) ||
			texHeight < Display__GetScreenHeight( ))
	{

		float		left,
					right,
					top,
					bottom;

		left = (float)Display__GetScreenWidth( ) - (float)texWidth;
		left = ( left / (float)Display__GetScreenWidth( )) / 2.0f;
		right = 1.0f - left;

		top = (float)Display__GetScreenHeight( ) - (float)texHeight;
		top = ( (float)top / (float)Display__GetScreenHeight( )) / 2.0f;
		bottom = 1.0f - top;

		vBinkPos[0].Set( left, top, 0.0f);
		vBinkPos[1].Set( right, top, 0.0f);
		vBinkPos[2].Set( right, bottom, 0.0f);
		vBinkPos[3].Set( left, bottom, 0.0f);
	}
	else
	{
		vBinkPos[0].Set( 0.0, 0.0, 0.0f);
		vBinkPos[1].Set( 1.0, 0.0, 0.0f);
		vBinkPos[2].Set( 1.0, 1.0, 0.0f);
		vBinkPos[3].Set( 0.0, 1.0, 0.0f);
	}

	DCFlushRange( vBinkPos, sizeof vBinkPos);

#endif

	sg_bMovieInitComplete = true;

	ts_bRGBA		color = { 0, 0, 0, 255 };

	bool	bEven = true;

	while( true)
	{

		int			iFrame;

		s32			thpStatus;

#if USE_THP != 0
		RenderState__ForceAll();
		RenderState__Default();
#endif

		gfxClearFrameBuffer( RS_COLOR_BUFFER_BIT | RS_DEPTH_BUFFER_BIT, &color);
		GXFlush( );

#if USE_THP != 0
		iFrame = THPPlayerDrawCurrentFrame(	DisplayState.pGXRenderMode,
														movieXPos,
														movieYPos,
														thpVI.xSize,
														thpVI.ySize);
		THPPlayerDrawDone( );

		gfxEndRender( true);

		if( sg_Abort != NULL)
		{
			localAborted = sg_Abort( sg_pAbortContext);
		}

		thpStatus = THPPlayerGetState( );

		if( thpStatus != PLAY || localAborted || sg_DiskError != MED_ERR_NONE)
		{
			THPPlayerStop( );
			THPPlayerClose( );
			return( NULL);
		}
#else
		//
		// Decompress the sg_hBink frame.
		//

		BinkDoFrame( sg_hBink);

		//
		// Draw the next frame.
		//

		BinkCopyToBuffer(	sg_hBink,
								pBinkTexAddr,
								texWidth * sizeof(ts_bRGBA),
								texHeight,
								0,
								0,
								BINKSURFACE32A);
		DCFlushRange( pBinkTexAddr, binkTexSize);

		DrawArray__Render( DA_QUADS, 0, 4);

		gfxEndRender( true);

		if( sg_hBink->FrameNum == sg_hBink->Frames)
		{
			BinkClose( sg_hBink);
			sg_hBink = NULL;
			return( NULL);
		}

		if( sg_Abort != NULL)
		{
			localAborted = sg_Abort( sg_pAbortContext);
		}

		if( localAborted)
		{
			BinkClose( sg_hBink);
			sg_hBink = NULL;
			return( NULL);
		}
		//
		// Keep playing the movie.
		//

		BinkNextFrame( sg_hBink);

		if( sg_hBink->ReadError != 0)
		{
			BinkClose( sg_hBink);
			sg_hBink = NULL;

			DVDFileInfo		dvdfi;
			if( !DVDOpen( "opening.bnr", &dvdfi))
			{
				return( NULL);
			}

			void	*pDummyBuffer = memAlloc( 32, 32);

			if( !DVDReadAsync( &dvdfi, pDummyBuffer, 32, 0, NULL))
			{
				DVDClose( &dvdfi);
				return( NULL);
			}

			VIWaitForRetrace( );

			MEM_SET_ONE_SHOT_NEW_NAME("CPreLoadScreenError");
			CMovieScreenError *pDiskErrorMessage = new CMovieScreenError;

			while( sg_DiskError != MED_ERR_NONE)
			{
				gfxClearFrameBuffer( RS_COLOR_BUFFER_BIT | RS_DEPTH_BUFFER_BIT, &color);

				pDiskErrorMessage->m_Text.SetText( sg_DiskErrorTextFunc( sg_DiskError));
				pDiskErrorMessage->m_Bkgd.Update( );

				Sprite::DrawAll( );

				SpriteMgr__Render( );

				gfxEndRender( true);
			}

			DVDCancel( (DVDCommandBlock *)&dvdfi);
			DVDClose( &dvdfi);

			delete pDiskErrorMessage;

			sg_hBink = BinkOpen( sg_pFMVName, 0);

			if( !sg_hBink)
			{
				return( NULL);
			}

			SetupFMVGraphicEnv( );
			TextureState__RequestAll( 0, TS_TYPE_2D, sg_binkTexBind, TS_METHOD_REPLACE, TS_WRAP_CLAMP, TS_WRAP_CLAMP, TS_FILTER_BILINEAR);

			continue;
		}

		while( BinkWait( sg_hBink));
#endif
	}
}

static void SetupFMVGraphicEnv( void)
{
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

	RenderState__Request( RS_CULL, RS_CULL_NONE);
	RenderState__Request( RS_ZTEST, RS_FALSE);
	RenderState__Request( RS_LIGHTING, RS_FALSE);

	unsigned char color[4];

	color[0] = 255;	//r
	color[1] = 255;	//g
	color[2] = 255;	//b
	color[3] = 255;	//a

	RenderState__Request( RS_BASECOLOR, RenderState__Colorubv( color));

	TextureState__RequestAll( 1, TS_TYPE_NONE, 0, 0, 0, 0, 0 );

	g_ShaderMgr.ResetCurrentMatrixIndices( );

	DrawArray__VectorPointer( 3, DA_FLOAT, 0, vBinkPos);
	DrawArray__TexturePointer( 2, DA_FLOAT, 0, vBinkUV);
	DrawArray__Enable( DA_VECTOR);
	DrawArray__Enable( DA_TEXTURE);
	DrawArray__Disable( DA_COLOR);
	DrawArray__Disable( DA_NORMAL);
}
