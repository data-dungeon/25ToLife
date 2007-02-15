/***************************************************************************/
// A simple interface for queueing voice/dialog
/***************************************************************************/
#ifndef VOICEQ_H
#define VOICEQ_H

#include "Audio/streamq.h"

class VoiceQueue : public StreamQueue
{
public:
	// These don't do much, as they can't error report
	VoiceQueue() {}
	~VoiceQueue() {}

	// Queue a stream for playback
	bool Queue(const char *pathFile, float fadeInTime = 0.0f, bool repeat = false, float wet = 0.0f);

	// Queue a stream for playback with callbacks
	bool QueueCallback(const char *pathFile, 
								STREAMCALLBACK playStarted, STREAMCALLBACK playEnded,	void *callbackContext,
								float fadeInTime = 0.0f, bool repeat = false, float wet = 0.0f);

private:
	static STREAMTYPE mode;
};

#endif
