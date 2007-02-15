/////////////////////////////////////////////////////////////////////////////
// Simple class that allows you to play an FMV
//
// Platform must provide an FMV driver
/////////////////////////////////////////////////////////////////////////////
#ifndef FMV_H
#define FMV_H

#include "container/dllist.h"
#include "container/darray.h"
#include "Layers/LayersStruct.h"
#include "fmv/fmvst.h"

typedef const char * (*DiskErrorTextCallback)( t_Error diskError);

// Forward declare the driver (which is platform specific)
class FMVDriver;

class FMVPlayer
{
public:
	FMVPlayer();
	~FMVPlayer();

	// Setup a translation path key
	// This works like this:
	// SetPathKey(1, "pal/");
	// When you Play("%1intro") it will substitute "pal/intro" as the fmv name.
	static void SetPathKey(int key, const char *value);

	static void SetSoundTrack(FMVSoundTrack soundTrackID)
		{ s_soundTrack = soundTrackID; }

	// Initialize to play a bunch of movies
	bool Begin(DiskErrorTextCallback diskErrorTextFunc = NULL);

	// Queue a movie to be played (no extensions or dir's please)
	// You can queue as many as you want
	bool Queue(const char *movieFile, bool allowInterruptions = true);

	// Play an entry in the queue
	// Returns true if there are still items in the
	// queue, false when empty.  call this until it returns false
	// If you want to see if the user aborted playback, check aborted
	bool Play(bool &aborted);

	//  TGS - more options that end the FMV
	bool PlayDemo(bool &aborted);

	// Shutdown & allow normal graphics to resume
	void End();

	// Some nice helpers
	static bool Play(const char *movieFile, bool allowInterruptions = true);
	static bool Play(const char *movieFile[], int count, bool allowInterruptions = true);

	static bool	IsPlaying(void)
  		{ return s_isPlaying > 0; }

#ifdef PS2
	// TJC - toggle this to preserve vram
	static void PreserveVRAM(bool preserve);
#endif

private:
	// The FMV driver (platform specific)
	FMVDriver							*m_driver;

	// A movie!
	struct FMV
	{
		char 				movieFile[128];
		bool				allowInterruptions;
		FMV				*prev, *next;
	};
	DoublyLinkedList<FMV>			m_list;

	static int							s_isPlaying;

#ifdef PS2
	static bool 						s_preserveVRAM;
#endif

	// Abort functions
	static bool Abort(void *context);
	bool Abort();

	// Abort functions for demo
	static bool AbortDemo(void *context);
	bool AbortDemo();

	// The translation stuff
	#define FMV_PATH_LENGTH 16
	struct PathKey
	{
		int key;
		char value[FMV_PATH_LENGTH];
	};
	static DynamicArray<PathKey>		s_pathKeyList;
	static const char *FindKey(int key);
	static void TranslateMovieFile(const char *movieFile, char *translated);

	static FMVSoundTrack					s_soundTrack;
};

#endif
