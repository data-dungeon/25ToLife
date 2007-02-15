/////////////////////////////////////////////////////////////////////////////
// This is the driver for FMV video
/////////////////////////////////////////////////////////////////////////////
#include "platform/CompilerControl.h"	// for NAMED_SECTION
#include "platform/BaseType.h"
#include "Layers/Media.h"
#include "Display/Common/DisplayState.h"
#include "Display/Common/DisplayPlatform.h"
#include "fmv/win32/fmvdriver.h"
#include "fmv/win32/bink/bink.h"

#if defined (DIRECTX)
#define WIDTH  640
#define HEIGHT 480
static	float	X_adjust = 0.0f;
static	float	Y_adjust = 0.0f;
static	float	Width_scale = 1.0f;
static	float	Height_scale = 1.0f;
static	D3DPRESENT_PARAMETERS Presentation = { 0 };

extern D3DPRESENT_PARAMETERS	DirectX_PresentParams;
extern LPDIRECT3DDEVICE8 DirectX_Device;
#endif

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
FMVDriver::FMVDriver(void)
{
#if defined (DIRECTX)
	m_Bink = 0;
	m_Bink_buffer = 0;

	// No device
	d3dDevice = NULL;

	// No direct sound yet
	directSound = NULL;

	// Init this shizit
	surfaceWidth = 0;
	surfaceHeight = 0;

	// Start on surface 0
	currentSurface = 0;
#endif
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
FMVDriver::~FMVDriver(void)
{
#if defined (DIRECTX)
	// Make absolutely sure everything is gone
	Destroy();
#endif
}

/////////////////////////////////////////////////////////////////////////////
// Make a full filename of what an FMV would be on this platform
/////////////////////////////////////////////////////////////////////////////
void FMVDriver::MakeFile(
const char *fmv,
char *fullFile)
{
	strcpy(fullFile, Directory());
	strcat(fullFile, fmv);
	strcat(fullFile, ".");
	strcat(fullFile, Extension());
}

/////////////////////////////////////////////////////////////////////////////
// Is this a valid/existing fmv for this driver?
// Note, pass in fmv name, not the full path
/////////////////////////////////////////////////////////////////////////////
bool FMVDriver::Valid(
const char *fmv)
{
	// Build the file name
	char pathFile[128];
	MakeFile(fmv, pathFile);

	// We probably should validate it a little
	return(Media::StreamExists(pathFile));
}

/////////////////////////////////////////////////////////////////////////////
// Create the device
/////////////////////////////////////////////////////////////////////////////
bool FMVDriver::Create(float volume)
{
#if defined (DIRECTX)

	// Get a poitner to the device
	d3dDevice = DirectX_Device;
	d3dDevice->AddRef( );

	BinkSoundUseDirectSound( 0 );

#endif
	return(true);
}

/////////////////////////////////////////////////////////////////////////////
// Destroy the device
/////////////////////////////////////////////////////////////////////////////
void FMVDriver::Destroy(void)
{
#if defined (DIRECTX)
	if (m_Bink)
	{
		BinkSetSoundOnOff(m_Bink, 0);
		DestroyBink();
	}

	if( directSound )
	{
		directSound->Release( );
		directSound = 0;
	}

	if( d3dDevice )
	{
		d3dDevice->Release( );

		d3dDevice = 0;
	}

#endif
	Display__ResetGraphics();
}


/////////////////////////////////////////////////////////////////////////////
// Play a file
/////////////////////////////////////////////////////////////////////////////
bool FMVDriver::Play(
const char *fmv,
FMVSoundTrack soundTrack,
bool &aborted,
AbortFn abort,
void *abortContext)
{
#if defined (DIRECTX)
	// clear this
	aborted = false;

	// Build the file name
	char pathFile[128];
	MakeFile(fmv, pathFile);

	// Open the video file to be played
	m_Bink = BinkOpen( Media::GetFileName( pathFile ), 0 );
	if (m_Bink == 0)
	{
		DestroyBink();
		return(false);
	}

	m_Bink_buffer = NULL; //BinkBufferOpen( DirectX_PresentParams.hDeviceWindow, m_Bink->Width, m_Bink->Height, 0 );
	if (!m_Bink_buffer)
	{
		DestroyBink();
		return(false);
	}

	for ( ;; )
	{
		// Check for playback abortion
		if ( m_Bink->FrameNum == m_Bink->Frames )
		{
			BinkSetSoundOnOff(m_Bink, 0);
			break;
		}

		PlayBink();

		if (abort && abort(abortContext))
		{
			BinkSetSoundOnOff(m_Bink, 0);
			break;
		}
	 }

#endif

	// We are done
	return(true);
}

/////////////////////////////////////////////////////////////////////////////
// Play Bink
/////////////////////////////////////////////////////////////////////////////
void FMVDriver::PlayBink()
{
#if defined (DIRECTX)
	static	u32 current_image = 0;

	if( !BinkWait( m_Bink ))
		ShowFrame(m_Bink, m_Bink_buffer);
	else
		Sleep(1);
#endif
}

/////////////////////////////////////////////////////////////////////////////
// Destroy Bink
/////////////////////////////////////////////////////////////////////////////
void FMVDriver::DestroyBink()
{
#if defined (DIRECTX)
	if (m_Bink)
	{
		BinkClose(m_Bink);
		m_Bink = 0;
	}

	if (m_Bink_buffer)
	{
		BinkBufferClose( m_Bink_buffer);
		m_Bink_buffer = 0;
	}
#endif
}

/////////////////////////////////////////////////////////////////////////////
// Decompress Bink Frame
/////////////////////////////////////////////////////////////////////////////

#if defined (DIRECTX)
void	FMVDriver::ShowFrame(  HBINK bink, HBINKBUFFER bink_buffer)
{

	BinkDoFrame( bink );

	if ( BinkBufferLock( bink_buffer ) )
	{
		BinkCopyToBuffer(	bink,
								bink_buffer->Buffer,
								bink_buffer->BufferPitch,
								bink_buffer->Height,
								0,0,
								bink_buffer->SurfaceType);
		BinkBufferUnlock( bink_buffer);
	}

	// Tell the BinkBuffer to blit the pixels onto the screen (if the
	//   BinkBuffer is using an off-screen blitting style).
	BinkBufferBlit( bink_buffer,
                  bink->FrameRects,
                  BinkGetRects( bink, bink_buffer->SurfaceType ) );


	// Are we at the end of the movie?
	if ( bink->FrameNum == bink->Frames )
	{
	}
	else
	{
    // Nope, advance to the next frame.
    BinkNextFrame( bink );
	}
}
#endif








