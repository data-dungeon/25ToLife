/***************************************************************************/
// A simple interface for queueing a list of streaming sounds.
/***************************************************************************/
#ifndef STREAMQ_H
#define STREAMQ_H

#include "Layers/Media.h"
#include "platform/BaseType.h"
#include "container/dllist.h"
#include "container/Darray.h"
#include "Audio/sounddriver.h"
#include "Audio/function/ramp.h"

#if defined (_XBOX)
#include "Audio/Xbox/wmastream.h"
#endif

// forwards
class CommonStreamInterface;

class StreamQueue
{
public:
	// These don't do much, as they can't error report
	StreamQueue();
	virtual ~StreamQueue();

	// Callbacks work like this....
	typedef void (*STREAMCALLBACK)(void *context, CommonStreamInterface *stream);

	// Initialize everything.
	virtual bool Initialize(SoundDriver &soundDriver);

	// Shutdown everything
	virtual void Terminate();

	// Override all paths from Queue commands
	void OverridePath(const char *overrideWithPath);

	// Queue a stream for playback
	// Returns false if the stream does not exist!
	virtual bool Queue(const char *pathFile, float fadeInTime, bool repeat, float wet = 0.0f) = 0;

	// Does this stream exist?
	bool Exists(const char *pathFile);

	// Queue some silence
	bool QueueSilence(int milliSeconds);

	// Repeat the whole playlist?
	void RepeatPlayList(bool repeatPlayList)
		{ repeatList = repeatPlayList; }

	// Randomly play this list
	void RandomListSkip(bool random);

	// Set the autopause (unpauses when n times have occurred)
	void AutoPauseFrames(int frames)
		{ autoPauseFrame = frames; }

	// Stop the playing stream (removing it from the queue)
	bool Stop(float fadeOutTime = 0.0f);

	// How many streams are queued?
	int Queued()
		{ return (playList.Items() + (playingStream ? 1 : 0)); }

	// Is something playing?
	bool Playing()
		{ return (playingStream != NULL); }

	// What is the currently playing sound
	CommonStreamInterface *CurrentlyPlaying()
		{ return playingStream; }

	// Clear the queue and stop all playing sounds
	void Clear();

	// Just empty the song queue
	void EmptyQueue();

	// Next track
	void Next();

	// Set the volume (from [0.0, 1.0])
	bool SetVolume(float newVolume);

	// Manipulate the volume to create drama! (from [0.0, 1.0])
	bool SetVolumeFactor(float newVolumeFactor);

	// What is the volume?
	float GetVolume()
		{ return volume; }
	float GetVolumeFactor()
		{ return volumeFactor; }

	// Pause playing stream
	bool Pause(bool pause);
	bool Paused()
		{ return paused; }

	// The daemon (must be called periodically)
	virtual void Daemon();

	virtual t_Error StreamError(void);

protected:
	// Our sounddriver
	SoundDriver		*driver;

	// Do we strip paths?
	bool				overridePaths;
	char				overridePath[64];
	void				StripPath(const char *pathFile, char *file);

	// Volume to play music at
	float			 	volume;

	// Factor to monkey with the volume
	float				volumeFactor;

	enum FADESTATE
	{
		FADEIN,
		FADEOUT,
		FADENONE
	};
	FADESTATE	 	fadeState;
	RampUp		 	rampUp;
	RampDown		 	rampDown;
	void			 	ApplyVolume();

	// Repeat the whole playlist?
	bool				repeatList;
	bool				randomSkip;
	void RandomizeHead();

	// The current playing stream
	CommonStreamInterface *playingStream;
	uint				PlayNextStream();
	void 				KillStream();

	// Are we paused?
	bool				paused;
	int				autoPauseFrame;

	// A daemon timer so we don't hog the CPU
	uint				nextProcess;

public: // not sure why this enumerated type needs to be public
	// Type of stream
	typedef enum
	{
		ONCD,
		ONHD,
		SILENCE,
		CDTOSRAM		// From CD straight to sound ram
	} STREAMTYPE;

	// The streams we currently are working with
	#define STREAMNAME_SIZE		64
	struct Stream
	{
		STREAMTYPE		  		type;
		char 						name[STREAMNAME_SIZE];
		SoundDriver::SONGID	id;
		int						silenceTime;
		float						fadeInTime;
		bool						repeat;
		float						wet;

		STREAMCALLBACK			playStarted;
		STREAMCALLBACK			playEnded;
		void						*callbackContext;

		Stream					*prev, *next;
	};
	virtual CommonStreamInterface *CreateStream(Stream &stream);

protected:
	// Queue a stream for playback
	bool Queue(const char *pathFile,
					float fadeInTime,
					bool repeat,
					float wet,
					STREAMCALLBACK playStarted,
					STREAMCALLBACK playEnded,
					void *callbackContext,
					STREAMTYPE type);

	STREAMCALLBACK			playStartedCallback;
	STREAMCALLBACK			playEndedCallback;
	void						*playCallbackContext;
	bool						calledPlayStarted;

	// The queue of streams to be played
	DoublyLinkedList<Stream>  	playList;
};

#endif
