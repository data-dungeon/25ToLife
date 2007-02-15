/***************************************************************************/
// A simple jukebox interface for playing playingStreaming sounds.  This API should
// improve as time goes on.
/***************************************************************************/
#include "Audio/AudioPCH.h"
#include "Math/fastrand.h"

// Update rates
#define NEXTUPDATE_EMPTY		30000
#define NEXTUPDATE_ERROR		5000
#define NEXTUPDATE_FADE			30
#define NEXTUPDATE_NOFADE		100

/***************************************************************************/
// These don't do much, as they can't error report
/***************************************************************************/
StreamQueue::StreamQueue() :
playList(true)
{
	driver = NULL;

	overridePaths = false;
	overridePath[0] = '\0';

	volume = 1.0f;
	volumeFactor = 1.0f;
	fadeState = FADENONE;

	playingStream = NULL;

	playStartedCallback = NULL;
	playEndedCallback = NULL;
	playCallbackContext = NULL;
	calledPlayStarted = false;

	repeatList = false;
	randomSkip = false;

	paused = false;
	autoPauseFrame = 0;

	nextProcess = 0;
}

/***************************************************************************/
/***************************************************************************/
StreamQueue::~StreamQueue()
{
	Terminate();
}

/***************************************************************************/
// Initialize everything.  Load the irx, init the rpc
/***************************************************************************/
bool StreamQueue::Initialize(
SoundDriver &soundDriver)
{
	// Save off the driver
	ASSERT(driver == NULL);
	driver = &soundDriver;

	// we are locked an loaded
	return true;
}

/***************************************************************************/
// Shutdown everything, probably don't ever need to call this
/***************************************************************************/
void StreamQueue::Terminate()
{
	// is there really anything to do here?
	Clear();
}

/***************************************************************************/
// Override all paths from Queue commands
/***************************************************************************/
void StreamQueue::OverridePath(
const char *overrideWithPath)
{
	if (!overrideWithPath)
		overridePaths = false;
	else
	{
		overridePaths = true;
		ASSERT(strlen(overrideWithPath) < sizeof(overridePath) - 1);
		strcpy(overridePath, overrideWithPath);
		if (overridePath[strlen(overridePath) - 1] != '\\')
			strcat(overridePath, "\\");
	}
}

/***************************************************************************/
/***************************************************************************/
bool StreamQueue::Exists(
const char *pathFile)
{
	// Make the real name
	char stream[STREAMNAME_SIZE];

	// Copy everything in
	ASSERT(strlen(pathFile) < sizeof(stream));
	if (!overridePaths)
		strcpy(stream, pathFile);
	else
	{
		strcpy(stream, overridePath);
		StripPath(pathFile, &stream[strlen(stream)]);
	}

	// Error check!
	return driver->StreamExist(stream);
}

/***************************************************************************/
// Queue some silence
/***************************************************************************/
bool StreamQueue::QueueSilence(
int milliSeconds)
{
	// Create a record for this song
	Stream *stream = new Stream;
	if (!stream)
		return false;

	// Seutp the song
	stream->type = SILENCE;
	stream->name[0] = '\0';
	stream->repeat = false;
	stream->silenceTime = milliSeconds;
	stream->fadeInTime = 0.0f;
	stream->wet = 0.0f;
	stream->playStarted = NULL;
	stream->playEnded = NULL;
	stream->callbackContext = NULL;

	// We can override...
	if (Queued() == 0)
		nextProcess = 0;

	// Add it to the tail
	playList.AddTail(stream);

	// No way this can fail now
	return true;
}

/***************************************************************************/
// Randomly play this list
/***************************************************************************/
void StreamQueue::RandomListSkip(bool random)
{
	randomSkip = random;
	if (randomSkip)
		RandomizeHead();
}

/***************************************************************************/
// Stop the playing stream (removing it from the list)
/***************************************************************************/
bool StreamQueue::Stop(
float fadeOutTime)
{
	// Is there a playing stream?
	if (!playingStream)
		return false;

	// Were we fading in?
	float startVolume;
	if (fadeState == FADEIN)
		startVolume = rampUp(driver->Time());
	else if (fadeState == FADEOUT)
		startVolume = rampDown(driver->Time());
	else
		startVolume = 1.0f;

	// Set it up
	fadeState = FADEOUT;
	rampDown.SetPeriod(fadeOutTime);
	rampDown.SetAmplitude(startVolume);
	rampDown.Reset(driver->Time());

	// Success!
	return true;
}

/***************************************************************************/
// Clear the queue and stop all playing sounds
/***************************************************************************/
void StreamQueue::Clear()
{
	// Stop any playing streams
	KillStream();

	// Flush the queue
	playList.RemoveAll();

	// Clear these
	paused = false;
	fadeState = FADENONE;

	// We are empty
	nextProcess = 0;
}

/***************************************************************************/
/***************************************************************************/
void StreamQueue::EmptyQueue()
{
	// Flush the queue
	playList.RemoveAll();
}

/***************************************************************************/
/***************************************************************************/
void StreamQueue::Next()
{
	// Kill the current
	Stop(1.0f);
}

/***************************************************************************/
// Set the volume (from [0.0, 1.0])
/***************************************************************************/
bool StreamQueue::SetVolume(
float newVolume)
{
	// Save it
	newVolume = Math::Clamp(newVolume);

	// Did it change?
	if (volume != newVolume)
	{
		volume = newVolume;
		ApplyVolume();
	}

	// Usless return and comment
	return true;
}

/***************************************************************************/
// Manipulate the volume to create drama!
/***************************************************************************/
bool StreamQueue::SetVolumeFactor(
float newVolumeFactor)
{
	if (volumeFactor != newVolumeFactor)
	{
		volumeFactor = newVolumeFactor;
		ApplyVolume();
	}

	return true;
}

/***************************************************************************/
// Pause playing stream
/***************************************************************************/
bool StreamQueue::Pause(
bool pause)
{
	// duh?
	if (pause == paused)
		return paused;

	// Save the state
	paused = pause;

	// Is there a current stream?
	if (playingStream)
	{
		if (paused)
			playingStream->Pause();
		else
			playingStream->Unpause();
	}

	// Return the current state
	return paused;
}

/***************************************************************************/
// The daemon (must be called periodically)
/***************************************************************************/
void StreamQueue::Daemon()
{
	// Better have a driver if we are in here)
	ASSERT(driver != NULL);

	// always check this as fast as possible
	if (playStartedCallback && !calledPlayStarted &&
			playingStream && playingStream->CurrentPlayPos() >= 0)
	{
		playStartedCallback(playCallbackContext, playingStream);
		calledPlayStarted = true;
		playStartedCallback = NULL;
	}

	// Are we paused?
	if (paused)
		return;

	// In an auto pause?
	if (autoPauseFrame > 0)
	{
		--autoPauseFrame;
		return;
	}

	// Ready to do work?
	if (driver->Time() < nextProcess)
		return;

	// Is there a playing stream that is done?
	if ((playingStream && !playingStream->IsPlaying()) ||
			(fadeState == FADEOUT && rampDown.Done(driver->Time())))
		KillStream();

	// Do we have a stream queued?
	if (!playingStream)
		nextProcess = PlayNextStream();
	else
	{
		// Adjust the volume
#ifndef WIN32
		if (fadeState != FADENONE)
#endif
			ApplyVolume();

		// Go neutral?
		if (fadeState == FADEIN && rampUp.Done(driver->Time()))
			fadeState = FADENONE;

		// Update the next update
		nextProcess = driver->Time() + (fadeState == FADENONE ? NEXTUPDATE_NOFADE : NEXTUPDATE_FADE);
	}

	// Make sure the 12-billion code paths set nextProcess
	ASSERT(nextProcess >= driver->Time());
}

/***************************************************************************/
// Queue a stream for playback
/***************************************************************************/
bool StreamQueue::Queue(
const char *pathFile,
float fadeInTime,
bool repeat,
float wet,
STREAMCALLBACK playStarted,
STREAMCALLBACK playEnded,
void *callbackContext,
STREAMTYPE type)
{
	// have they specified a name?
	if (pathFile == NULL || *pathFile == '\0')
		return false;

	// Create a record for this song
	Stream *stream = new Stream;
	if (!stream)
		return false;

	// Copy everything in
	stream->type = type;
	ASSERT(strlen(pathFile) < sizeof(stream->name));
	if (!overridePaths)
		strcpy(stream->name, pathFile);
	else
	{
		strcpy(stream->name, overridePath);
		StripPath(pathFile, &stream->name[strlen(stream->name)]);
	}
	stream->repeat = repeat;
	stream->wet = wet;
	stream->silenceTime = 0;
	stream->fadeInTime = fadeInTime;

	// callback shiznit
	stream->playStarted = playStarted;
	stream->playEnded = playEnded;
	stream->callbackContext = callbackContext;

	// Error check!
	if (!driver->StreamExist(stream->name))
	{
		delete stream;
		return false;
	}

	// We can override...
	if (Queued() == 0)
		nextProcess = 0;

	// Add it to the tail
	playList.AddTail(stream);

	// No way this can fail now
	return true;
}

/***************************************************************************/
/***************************************************************************/
void StreamQueue::StripPath(
const char *pathFile,
char *file)
{
	// Save the length
	int len = strlen(pathFile) - 1;

	// Find the beginning of the name, or a slash
	while (len > 0 && pathFile[len] != '\\')
		--len;
	if (pathFile[len] == '\\')
		++len;

	// Now copy to the dot or the end
	int fileLen = 0;
	while (pathFile[len] != '\0' && pathFile[len] != '.')
		file[fileLen++] = pathFile[len++];
	file[fileLen] = '\0';
}

/***************************************************************************/
// Set the volume
/***************************************************************************/
void StreamQueue::ApplyVolume()
{
	float v = volume * volumeFactor;
	if (fadeState == FADEIN)
		v *= rampUp(driver->Time());
	else if (fadeState == FADEOUT)
		v *= rampDown(driver->Time());

	if (playingStream)
		playingStream->SetVolume(v);
}

/***************************************************************************/
/***************************************************************************/
void StreamQueue::RandomizeHead()
{
	int skip = g_random.UpToExcludeHigh(playList.Items());
	while (skip)
	{
		Stream *stream = playList.Head();
		playList.Unlink(stream);
		playList.AddTail(stream);
		--skip;
	}
}

/***************************************************************************/
/***************************************************************************/
uint StreamQueue::PlayNextStream()
{
	// Do we have a stream?
	Stream *stream = playList.Head();
	if (!stream)
		return driver->Time() + NEXTUPDATE_EMPTY;

	// Set up the stream
	uint nextUpdate;
	if (stream->type == SILENCE)
	  	nextUpdate = stream->silenceTime; // Just sleep!
	else
	{
		// Setup play callback shiznit
		playStartedCallback = stream->playStarted;
		playEndedCallback = stream->playEnded;
		playCallbackContext = stream->callbackContext;
		calledPlayStarted = false;

		// Queue it
		playingStream = CreateStream(*stream);

		// Did the stream get created
		if (!playingStream)
			nextUpdate = NEXTUPDATE_ERROR; // Error playing stream wait for a second
		else
		{
			// Do they want a fade in?
			if (Math::Zero(stream->fadeInTime))
			{
				fadeState = FADENONE;
				nextUpdate = NEXTUPDATE_NOFADE;
			}
			else
			{
				fadeState = FADEIN;
				rampUp.SetPeriod(stream->fadeInTime);
				rampUp.Reset(driver->Time());
				nextUpdate = NEXTUPDATE_FADE;
			}

			// Apply the volume
			ApplyVolume();

			// Play!
			playingStream->Play();
		}
	}

	// pull it off the queue
	if (!stream->repeat)
	{
		if (!repeatList)
			playList.Remove(stream);
		else
		{
			playList.Unlink(stream);
			playList.AddTail(stream);

#if 0
			if (randomSkip)
				RandomizeHead();
#endif
		}
	}

	return driver->Time() + nextUpdate;
}

/***************************************************************************/
/***************************************************************************/
void StreamQueue::KillStream()
{
	// Kill it
	if (playingStream)
	{
		// Call the callback
		if (playEndedCallback)
		{
			playEndedCallback(playCallbackContext, playingStream);
			playEndedCallback = NULL;
		}

		playingStream->SetVolume(0.0f);
		playingStream->Stop();
		delete playingStream;
		playingStream = NULL;
	}

	fadeState = FADENONE;
}

/***************************************************************************/
/***************************************************************************/
t_Error StreamQueue::StreamError()
{
	if (playingStream)
		return playingStream->StreamError();

	return MED_ERR_NONE;
}

/***************************************************************************/
/***************************************************************************/
CommonStreamInterface *StreamQueue::CreateStream(
Stream &stream)
{
	// Create the stream
	CommonStreamInterface *playingStream = NULL;
	if (stream.type == ONCD)
		playingStream = g_audio.CreateStream2D(stream.name, stream.repeat);
	else if (stream.type == ONHD)
		playingStream = g_audio.CreateStream2D(stream.id, stream.repeat);
	else if (stream.type == CDTOSRAM)
		playingStream = g_audio.CreateStream2D(stream.name, stream.repeat, true);

	// Set the wet/dry mix
	if (playingStream)
		playingStream->SetWetDry(stream.wet, 1.0f);

	return playingStream;
}



