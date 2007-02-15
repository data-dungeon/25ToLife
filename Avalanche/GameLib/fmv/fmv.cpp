/////////////////////////////////////////////////////////////////////////////
// Simple class that allows you to play an FMV
//
// Platform must provide an FMV driver
/////////////////////////////////////////////////////////////////////////////

#include "FMVPCH.h"
#include <string.h>
#include "fmv/fmv.h"
#include "platform/systemi.h"
#include "Audio/audiosys.h"
#include "platform/gameapp.h"

// Include the appropriate FMV driver
#include "platform/CompilerControl.h"	// for NAMED_SECTION
#include "Display/Common/RenderState.h"
#include "Display/Common/DisplayPlatform.h"
#include "Display/Common/DisplayState.h"
#if defined(_XBOX)
#include "fmv/xbox/fmvdriver.h"
#elif defined(DIRECTX_PC)
#include "fmv/win32/fmvdriver_wmv.h"
#elif defined(WIN32)
#include "fmv/win32/fmvdriver.h"
#elif defined(PS2)
#include "fmv/ps2/fmvdriver.h"
#elif defined(GCN)
#include "fmv/gcn/fmvdriver.h"
#endif

// Define this to get the FMV driver to fail
//#define FMVDRIVERFAIL

#ifdef PS2
bool FMVPlayer::s_preserveVRAM = false;
#endif

int FMVPlayer::s_isPlaying = 0;

// Translation stuff
DynamicArray<FMVPlayer::PathKey>			FMVPlayer::s_pathKeyList(4, 4);

FMVSoundTrack				 					FMVPlayer::s_soundTrack = FMVST_DEFAULT;

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
FMVPlayer::FMVPlayer() :
m_list(true)
{
	m_driver = NULL;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
FMVPlayer::~FMVPlayer()
{
	// Make sure they cleaned up
	ASSERT(m_driver == NULL);
	RenderState__ForceAll();
	RenderState__Default();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void FMVPlayer::SetPathKey(
int key,
const char *value)
{
	ASSERT(value);
	ASSERT(strlen(value) < FMV_PATH_LENGTH);

	bool found = false;
	for (int i = 0; i < s_pathKeyList.Count(); i++)
	{
		if (s_pathKeyList[i].key == key)
		{
			strcpy(s_pathKeyList[i].value, value);
			found = true;
		}
	}

	if (!found)
	{
		PathKey pathKey;
		pathKey.key = key;
		strcpy(pathKey.value, value);
		s_pathKeyList.Add(pathKey);
	}
}

#ifdef PS2
/////////////////////////////////////////////////////////////////////////////
// TJC - toggle this to preserve vram
/////////////////////////////////////////////////////////////////////////////
void FMVPlayer::PreserveVRAM(bool preserve)
{
	s_preserveVRAM = preserve;
}
#endif

/////////////////////////////////////////////////////////////////////////////
// Initialize to play a bunch of movies
/////////////////////////////////////////////////////////////////////////////
bool FMVPlayer::Begin(DiskErrorTextCallback diskErrorTextFunc)
{
#ifndef FMVDRIVERFAIL
	// Tell the audio system we are going in
	g_audio.PrepareForFMV();

	// Create the driver
	m_driver = new FMVDriver;
	if (!m_driver)
		return(false);

	// Initialize the m_driver
	if (!m_driver->Create(g_audio.GetVolume(Audio::SFX)
#ifdef PS2
								,s_preserveVRAM
#endif
#ifdef GCN
								,diskErrorTextFunc
#endif
			 ))
	{
		delete m_driver;
		m_driver = NULL;
		return(false);
	}
#endif

	// success!
	return(true);
}

/////////////////////////////////////////////////////////////////////////////
// Queue a movie to be played (no extensions or dir's please)
// You can queue as many as you want
/////////////////////////////////////////////////////////////////////////////
bool FMVPlayer::Queue(
const char *movieFile,
bool allowInterruptions)
{
#ifndef FMVDRIVERFAIL
	// Make the real movie file!
	char translated[128];
	TranslateMovieFile(movieFile, translated);

	// ask the m_driver if this is a valid stream
	if (!FMVDriver::Valid(translated))
		return(false);

	// Create a new entry
	FMV *fmv = new FMV;
	if (!fmv)
		return(false);

	// fill it out
	strcpy(fmv->movieFile, movieFile);
	fmv->allowInterruptions = allowInterruptions;

	// Put it in the list
	m_list.AddTail(fmv);
#endif

	return(true);
}

/////////////////////////////////////////////////////////////////////////////
// Play a file in the queue, call this until it returns false
// If you want to see if the user aborted playback, pass in
// a boolean for aborted
/////////////////////////////////////////////////////////////////////////////
bool FMVPlayer::Play(
bool &aborted)
{
	// duh
	if (!m_driver)
		return(false);

	// Currently playing FMV
	FMV *play = m_list.Head();
	if (!play)
		return(false);
	m_list.Unlink(play);

	// set flag that fmv is in progress
	++s_isPlaying;

	// Make the real movie file!
	char translated[128];
	TranslateMovieFile(play->movieFile, translated);

	// Play it in the m_driver
	if (play->allowInterruptions)
		m_driver->Play(translated, s_soundTrack, aborted, Abort, this);
	else
		m_driver->Play(translated, s_soundTrack, aborted);

	// clear fmv flag
	--s_isPlaying;

	// kill this memory
	delete play;

	// Are there any entries left?
	return(m_list.Head() != NULL);
}

/////////////////////////////////////////////////////////////////////////////
// Play a file in the queue, call this until it returns false
// If you want to see if the user aborted playback, pass in
// a boolean for aborted
/////////////////////////////////////////////////////////////////////////////
bool FMVPlayer::PlayDemo(
bool &aborted)
{
	// duh
	if (!m_driver)
		return(false);

	// Currently playing FMV
	FMV *play = m_list.Head();
	if (!play)
		return(false);
	m_list.Unlink(play);

	// set flag that fmv is in progress
	++s_isPlaying;

	// Make the real movie file!
	char translated[128];
	TranslateMovieFile(play->movieFile, translated);

	// Play it in the m_driver
	if (play->allowInterruptions)
		m_driver->Play(translated, s_soundTrack, aborted, AbortDemo, this);
	else
		m_driver->Play(translated, s_soundTrack, aborted);

	// clear fmv flag
	--s_isPlaying;

	// kill this memory
	delete play;

	// Are there any entries left?
	return(m_list.Head() != NULL);
}

/////////////////////////////////////////////////////////////////////////////
// Shutdown & allow normal graphics to resume
/////////////////////////////////////////////////////////////////////////////
void FMVPlayer::End()
{
	// Clear the list
	m_list.RemoveAll();

	// Trash the m_driver
	if (m_driver)
	{
		m_driver->Destroy();
		delete m_driver;
		m_driver = NULL;
	}

	// Tell the audio system we back up for air
	g_audio.RecoverFromFMV();
}

/////////////////////////////////////////////////////////////////////////////
// Some nice helpers
/////////////////////////////////////////////////////////////////////////////
bool FMVPlayer::Play(
const char *movieFile,
bool allowInterruptions)
{
	return Play(&movieFile, 1, allowInterruptions);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool FMVPlayer::Play(
const char *movieFile[],
int count,
bool allowInterruptions)
{
	FMVPlayer *fmvPlayer = new FMVPlayer;
	if (!fmvPlayer)
		return false;

	// Setup the queue
	for (int i = 0; i < count; i++)
		fmvPlayer->Queue(movieFile[i], allowInterruptions);

	// Prepare to play
	fmvPlayer->Begin();

	// set flag that fmv is in progress
	++s_isPlaying;

	// play the movie
	bool more = true;
	while (more)
	{
		bool aborted = false;
		more = fmvPlayer->Play(aborted);
	}

	// clear fmv flag
	--s_isPlaying;

	// Cleanup
	fmvPlayer->End();
	delete fmvPlayer;
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// Abort functions
/////////////////////////////////////////////////////////////////////////////
bool FMVPlayer::Abort(
void *context)
{
	// Did they have a context
	if (!context)
		return(false);

	// Call the real one
	FMVPlayer *me = (FMVPlayer *)context;
	return(me->Abort());
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool FMVPlayer::Abort()
{
	bool buttonPressed = false;

	g_input.BeginInput();
#ifdef _XBOX
	bool foundController = false;
	uint i;
	// first look at the locked controllers if we have any
	for(i = 0; i < g_input.Controllers(); i++)
	{
		if (!g_input.ControllerLocked(i))
			continue;

		foundController = true;
		ControllerInputDriver *controller = g_input.GetController(i);

		if (controller->ButtonPressed(CB_START ) || controller->ButtonPressed(CB_X ) )
			buttonPressed = true;
	}

	// if we didn't find a locked controller, just look at them all
	if (!foundController )
	{
		for(i = 0; i < g_input.Controllers(); i++)
		{
			ControllerInputDriver *controller = g_input.GetUnlockedController(i);

			if (controller->Connected() && (controller->ButtonPressed(CB_START ) || controller->ButtonPressed(CB_X )) )
				buttonPressed = true;
		}
	}
#else

	//  TGS - TEMPORARY FIX (per Dave R. 8/18/2003 ).
	//  This FOR loop should only query active controllers (no set number of controllers )
	//  This will have to be done later (active meaning number of players )
	for(int i = 0; i < 1; i++)
	{

		ControllerInputDriver *controller = g_input.GetController(i);

		if(controller != NULL)
		{
#ifdef PS2
			buttonPressed = (controller->ButtonPressed(CB_X ) || controller->ButtonPressed(CB_START ) ) ? true : buttonPressed;
#else
				buttonPressed = (!controller->AnyButtonPressed() ) ? buttonPressed : true;
#endif // PS2
		}
	}
#endif //_XBOX

	g_input.DoneInput();

	return(buttonPressed);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool FMVPlayer::AbortDemo(
void *context)
{
	// Did they have a context
	if (!context)
		return(false);

	// Call the real one
	FMVPlayer *me = (FMVPlayer *)context;
	return(me->AbortDemo());
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool FMVPlayer::AbortDemo()
{
	bool buttonPressed = false;

	g_input.BeginInput();
#ifdef _XBOX
	bool foundController = false;
	uint i;
	// first look at the locked controllers if we have any
	for(i = 0; i < g_input.Controllers(); i++)
	{
		if (!g_input.ControllerLocked(i))
			continue;

		foundController = true;
		ControllerInputDriver *controller = g_input.GetController(i);

		if (controller->ButtonPressed(CB_START ) || controller->ButtonPressed(CB_X ) )
			buttonPressed = true;



	}
	// if we didn't find a locked controller, just look at them all
	if (!foundController )
	{
		for(i = 0; i < g_input.Controllers(); i++)
		{
			ControllerInputDriver *controller = g_input.GetUnlockedController(i);

			if (controller->Connected() && (controller->ButtonPressed(CB_START ) || controller->ButtonPressed(CB_X )) )
				buttonPressed = true;
		}
	}
#else

	//  TGS - TEMPORARY FIX (per Dave R. 8/18/2003 ).
	//  This FOR loop should only query active controllers (no set number of controllers )
	//  This will have to be done later (active meaning number of players )
	for(int i = 0; i < 1; i++)
	{

		ControllerInputDriver *controller = g_input.GetController(i);

		if(controller != NULL)
		{
			buttonPressed = (!controller->AnyButtonPressed() ) ? buttonPressed : true;
		}
	}
#endif //_XBOX

	g_input.DoneInput();

	return(buttonPressed);
}

////////////////////////////////////////////////////////
const char *FMVPlayer::FindKey(
int key)
{
	for (int i = 0; i < s_pathKeyList.Count(); i++)
	{
		if (key == s_pathKeyList[i].key)
			return s_pathKeyList[i].value;
	}

	return "";
}

////////////////////////////////////////////////////////
void FMVPlayer::TranslateMovieFile(
const char *movieFile,
char *translated)
{
	const char *source = movieFile;
	const char *dest = translated;

	// Build the translation
	*translated = '\0';
	while (*movieFile)
	{
		if (*movieFile == '/')
		{
			*translated++ = '\\';
			++movieFile;
		}
		else if (*movieFile != '%')
			*translated++ = *movieFile++;
		else
		{
			// We are at a key
			++movieFile;
			ASSERT(*movieFile);
			strcpy(translated, FindKey((int)*movieFile - (int)'0'));
			++movieFile;
			while (*translated)
				++translated;
		}
	}
	*translated = '\0';

	TRACE("Translated FMV from %s to %s\n", source, dest);
}
