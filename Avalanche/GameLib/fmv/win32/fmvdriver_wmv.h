/////////////////////////////////////////////////////////////////////////////
// This is the driver for FMV video
/////////////////////////////////////////////////////////////////////////////
#ifndef FMVDRIVER_H
#define FMVDRIVER_H

#include "fmv/win32/dshowtextures.h"
#include <dsound.h>

extern void Display__ResetGraphics(void);

class FMVDriver
{
	public:
		FMVDriver(void);
		~FMVDriver(void);

		// Some statics for how video is stored on this platform
		static const char *Directory(void)
			{ return("fmv\\"); }

		static const char *Extension(void)
			{ return("src"); }

		// Make a full filename of what an FMV would be on this platform
		static void MakeFile(const char *fmv, char *fullFile);

		// Is this a valid/existing fmv for this driver?
		// Note, pass in fmv name, not the full path
		static bool Valid(const char *fmv);

		// Create the device
		bool Create(float initVolume);

		// Destroy the device
		void Destroy(void);

		// switching to/from the movie players video mode
		void InitMode(void);
		void CleanupMode(void);

		// Play an FMV
		typedef bool (*AbortFn)(void *context);
		bool Play(const char *fmv, bool &aborted, AbortFn abort = NULL, void *abortContext = NULL);
		bool Play(const char *fmv, int soundTrack, bool &aborted, AbortFn abort = NULL, void *abortContext = NULL) 
			{ return Play(fmv, aborted, abort, abortContext); }


	private:
		MoviePlayer *m_pMoviePlayer;
};

#endif
