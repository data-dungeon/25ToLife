/***************************************************************************/
// A simple jukebox interface for playing playingStreaming sounds.  This API should
// improve as time goes on.
/***************************************************************************/
#include "Audio/AudioPCH.h"

#ifndef GCN
StreamQueue::STREAMTYPE VoiceQueue::mode = StreamQueue::ONCD;
#else
StreamQueue::STREAMTYPE VoiceQueue::mode = StreamQueue::CDTOSRAM;
#endif

/***************************************************************************/
// Queue a named song for playback
/***************************************************************************/
bool VoiceQueue::Queue(
const char *pathFile,
float fadeInTime,
bool repeat,
float wet)
{
	// Queue an track
	return StreamQueue::Queue(pathFile, fadeInTime, repeat, wet, NULL, NULL, NULL, mode);
}

/***************************************************************************/
// Queue a stream for playback with callbacks
/***************************************************************************/
bool VoiceQueue::QueueCallback(
const char *pathFile, 
STREAMCALLBACK playStarted, 
STREAMCALLBACK playEnded,	
void *callbackContext,
float fadeInTime, 
bool repeat, 
float wet)
{
	// Do this with notifications
	return StreamQueue::Queue(pathFile, fadeInTime, repeat, wet, 
										playStarted, playEnded, callbackContext, mode);
}


