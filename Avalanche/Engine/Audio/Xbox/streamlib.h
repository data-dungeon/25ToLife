/***************************************************************************/
// Manage a list of streams stored in hogs
/***************************************************************************/
#ifndef STREAMLIB_H
#define STREAMLIB_H

#include "EngineHelper/hog.h"
#include "container/dllist.h"

// We create a whole list of these
struct ImbeddedStreamInfo
{
   HANDLE				file;
	DWORD					offset;
	DWORD					size;
};

// The stuff for a stream volume
class StreamVolume
{
public:
	StreamVolume();
	~StreamVolume()
		{ Unmount(); }

	// Mount us
	bool Mount(const char *libName);

	// Unmount us
	void Unmount();

	// Find an entry, returns false if not there otherwise true
	// with the structure filled out
	bool Find(const char *name, ImbeddedStreamInfo *info = NULL);

	// Find a const ref to a string, NULL if not there
	const char *GetTokenPtr(const char *name);

private:
	// Our hog!
	Hog						hog;
	bool						valid;

public:
	StreamVolume			*prev, *next;
};

// The stream library (a list of stream volumes)
class StreamLibrary
{
public:
	StreamLibrary();
	~StreamLibrary()
		{ UnmountAll(); }

	// Mount us
	bool Mount(const char *libName);

	// Unmount everything
	void UnmountAll();

	// Find an entry, returns false if not there otherwise true
	// with the structure filled out
	bool Find(const char *name, ImbeddedStreamInfo *info = NULL);

	// Find a const ref to a string, NULL if not there
	const char *GetTokenPtr(const char *name);

private:
	// The list of volumes
	DoublyLinkedList<StreamVolume>	volumeList;
};

#endif


