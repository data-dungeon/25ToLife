// ------------------------------------------------------------------------
// Wrapper for ATWinMon communication
// ------------------------------------------------------------------------
#ifndef AWMFILE_H
#define AWMFILE_H

#include "Layers/LayersPCH.h"

// Turn this all off for CDROM
#ifndef CDROM
#define ATMONSUPPORT
#endif 

class AtWinMonFile
{
public:
#ifdef ATMONSUPPORT
	// Always call this before any other calls, if not connected
	// do not use these calls
	static bool Connected();

	// Are we connected and have a remote file (call this before open)
	static bool IsRemoteFile(const char *file);

	// Does this file exist
	static bool Exists(const char *fileName);

	// Get the timestamp (watch out, always returns false)
	static bool TimeStamp(const char *fileName, ts_TimeStamp *pTimeStamp);

	// open a file (call IsRemoteFile first)
	// returns the handle or -1
	static int Open(const char *fileName, u32 openFlags, int *size);

	// close an open file
	static void Close(int handle);

	// seek in an already-open file, from beginning
	static bool Seek(int handle, s32 pos);

	// read from an already-open file
	static bool Read(void *buffer, s32 len, int handle);

	// write to an already-open file
	static bool Write(const void *buffer, s32 len, int handle);
#else
	// Always call this before any other calls, if not connected
	// do not use these calls
	static bool Connected()
		{ return false; }

	// Are we connected and have a remote file (call this before open)
	static bool IsRemoteFile(const char *file)
		{ return false; }

	// Does this file exist
	static bool Exists(const char *fileName)
		{ return false; }

	// Get the timestamp (watch out, always returns false)
	static bool TimeStamp(const char *fileName, ts_TimeStamp *pTimeStamp)
		{ return false; }

	// open a file (call IsRemoteFile first)
	// returns the handle or -1
	static int Open(const char *fileName, u32 openFlags, int *size)
		{ return -1; }

	// close an open file
	static void Close(int handle) {}

	// seek in an already-open file, from beginning
	static bool Seek(int handle, s32 pos)
		{ return false; }

	// read from an already-open file
	static bool Read(void *buffer, s32 len, int handle)
		{ return false; }

	// write to an already-open file
	static bool Write(const void *buffer, s32 len, int handle)
		{ return false; }
#endif
};

#endif