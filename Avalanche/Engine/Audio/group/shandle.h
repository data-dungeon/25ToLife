/***************************************************************************/
// Sound handles MUST BE delete'd or else.
//
// As an API user, NEVER call SoundHandleList functions, always rely on
// the emitter functions
/***************************************************************************/
#ifndef SHANDLE_H
#define SHANDLE_H

#include "container/pool.h"
#include "container/dllist.h"
#include "container/fpool.h"

// Turn this on for some reporting on handle leaks
#ifdef _DEBUG
#define TRACKSOUNDHANDLES
#endif

class SoundEmitter;
class SoundChannel;
class SoundHandleData;

// Make a shortcut for all the world to use
typedef SoundHandleData *SoundHandle;

// The data for the sound handle
class SoundHandleData
{
public:
	// Some handy functions that need to be called to get this thing going
	static void Initialize();
	static void Terminate();
	static void Reset();

	// Create me
	SoundHandleData(SoundChannel *channel);
	~SoundHandleData();

	// Get the channel
	SoundChannel *Channel();

	// Clear the channel
	void LostChannel();

	// Special control since we use a pool
	void *operator new(size_t size);
	void operator delete(void *me);

private:
	// Save a pointer to channel, set to NULL if we lost it
	SoundChannel		*channel;

	// We have a pool of user data structures
	static Pool<SoundHandleData> *pool;

#ifdef TRACKSOUNDHANDLES
public:
	uint					uid;
#endif
};

#ifdef TRACKSOUNDHANDLES
#define DEBUGSTUB 	;
#else
#define DEBUGSTUB 	{}
#endif

class SoundHandleTracker
{
public:
	static void Add(SoundHandle handle, const char *emitterName, const char *soundName) DEBUGSTUB
	static void Remove(SoundHandle handle) DEBUGSTUB
	static void Clear() DEBUGSTUB
	static void Report(bool assertIfNotEmpty = true) DEBUGSTUB

private:
	struct HandleTrack
	{
		char 					emitterName[32];
		char			  		soundName[32];
		SoundHandle		   handle;
		uint					uid;

		struct HandleTrack	*prev, *next;
	};

#ifdef TRACKSOUNDHANDLES
	static DoublyLinkedList<HandleTrack>	handleList;
	static FixedPool<HandleTrack, 1024>		handlePool;
	static uint										uid;
#endif
};

#endif

