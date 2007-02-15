/////////////////////////////////////////////////////////////////////////////
// This is the driver for FMV video
/////////////////////////////////////////////////////////////////////////////
#include "Platform/CompilerControl.h"	// for NAMED_SECTION
#include "Platform/BaseType.h"
#include "layers/media.h"
#include "Display/Common/DisplayState.h"
#include "Display/Common/TextureState.h"
#include "Game/Managers/TextureMgr/TextureMgr.h"
#include "Game/Managers/TextureMgr/Texture.h"
#include "Game/Managers/TextureMgr/PlatformImage.h"
#include "Game/Managers/SpriteMgr.h"
#include "Display/Common/DisplayPlatform.h"
#include "fmv/win32/fmvdriver_wmv.h"
#include "platform/Win32/Winapp.h"

FMVDriver::FMVDriver(void)
{
}

FMVDriver::~FMVDriver(void)
{
}

// Make a full filename of what an FMV would be on this platform
/*static*/ void FMVDriver::MakeFile(const char *fmv, char *fullFile)
{
	strcpy(fullFile, Directory());
	strcat(fullFile, fmv);
	strcat(fullFile, ".");
	strcat(fullFile, Extension());
}

// Is this a valid/existing fmv for this driver?
// Note, pass in fmv name, not the full path
/*static*/ bool FMVDriver::Valid(const char *fmv)
{
	// Build the file name
	char pathFile[128];
	MakeFile(fmv, pathFile);

	// We probably should validate it a little
	return(Media::StreamExists(pathFile));
}

// Create the device
bool FMVDriver::Create(float initVolume)
{
	return true;
}

// Destroy the device
void FMVDriver::Destroy(void)
{
}

// switching to/from the movie players video mode
void FMVDriver::InitMode(void)
{
}

void FMVDriver::CleanupMode(void)
{
	Display__ResetGraphics();
}

// Play an FMV
bool FMVDriver::Play(const char *fmv, bool &aborted, AbortFn abort, void *abortContext)
{
	// Build the file name
	char pathFile[128];
	char nativeFileName[MAX_FILENAME_LEN];
	MakeFile(fmv, pathFile);
	MediaHAL::MakeFileName(pathFile, nativeFileName, true);

	MSG  msg;
	WindowsApp *winApp = WindowsApp::IAm();

	m_pMoviePlayer = new MoviePlayer();

	// Start the movie player and the movie
	if (!m_pMoviePlayer->Initialize())
	{
		return false;
	}

	m_pMoviePlayer->Play(nativeFileName);

	// Wait for movie to finish playing
	do
	{
		if (abort && abort(abortContext))
		{
			m_pMoviePlayer->Stop();
			break;
		}

		// Pump the windows message pump
		if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
			if (!GetMessage(&msg, NULL, 0, 0))
				break;

			if (!winApp->HandleMessage(msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}
	while (!m_pMoviePlayer->IsMovieDone());
	
	delete m_pMoviePlayer;

	return true;
}
