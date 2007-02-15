/***************************************************************************/
// Manage a list of streams stored in hogs
/***************************************************************************/
#include "Audio/AudioPCH.h"

/***************************************************************************/
/***************************************************************************/
StreamVolume::StreamVolume()
{
	valid = false;
}

/***************************************************************************/
// Mount us
/***************************************************************************/
bool StreamVolume::Mount(
const char *libName)
{
	ASSERT(!valid);

	// The filename
	char pathFile[256];
	sprintf(pathFile, "stream\\%s.hog", libName);

	// Mount the data hog
	//TRACE("Mounting stream hog %s.\n", pathFile);
	if (!hog.Mount(pathFile, false))
		return false;

	// Never check local
	hog.OverrideCheckLocal(CHECKLOCAL_NEVER);
	valid = true;

	// Do some testing
#ifdef _DEBUG
	// For each file, create a voice set
	for (int f = 0; f < hog.Files(); f++)
	{
		// Open the file
		const char *hogFile = hog.FileName(f);

		// Get the size and offset in one fell swoop
		// returns false if it doesn't exist
		int offset, size;
		bool ret = hog.FileOffsetSize(hogFile, offset, size);
		ASSERT(ret);

		if (ret)
		{
			ImbeddedStreamInfo info;
			info.file = (HANDLE)hog.GetPlatformFileHandle();
			info.offset = (DWORD)offset;
			info.size = (DWORD)size;
			WMAStream::Test(info, hogFile);
		} 
	}
#endif

	// Out of here
	return true;
}

/***************************************************************************/
// Unmount us
/***************************************************************************/
void StreamVolume::Unmount()
{
	hog.Unmount();
	valid = false;
}

/***************************************************************************/
// Find an entry, returns NULL if not there
/***************************************************************************/
bool StreamVolume::Find(
const char *name,
ImbeddedStreamInfo *info)
{
	// This never got mounted
	if (!valid)
		return false;

	// Get the size and offset in one fell swoop
	// returns false if it doesn't exist
	int offset, size;
	if (!hog.FileOffsetSize(name, offset, size))
		return false;

	if (info)
	{
		info->file = (HANDLE)hog.GetPlatformFileHandle();
		info->offset = (DWORD)offset;
		info->size = (DWORD)size;
	}

	return true;
}

/***************************************************************************/
// Find a const ref to a string, NULL if not there
/***************************************************************************/
const char *StreamVolume::GetTokenPtr(const char *name)
{
	return hog.GetConstFileName(name);
}

/***************************************************************************/
// The stream library (a list of stream volumes)
/***************************************************************************/
StreamLibrary::StreamLibrary() :
volumeList(false)
{
}

/***************************************************************************/
// Mount us
/***************************************************************************/
bool StreamLibrary::Mount(
const char *libName)
{
	// Create a new one
	StreamVolume *volume = new StreamVolume();
	if (!volume)
		return false;

	// Mount it
	if (!volume->Mount(libName))
	{
		delete volume;
		return false;
	}

	// Add it to the list
	volumeList.AddTail(volume);
	return true;
}

/***************************************************************************/
// Unmount everything
/***************************************************************************/
void StreamLibrary::UnmountAll()
{
	StreamVolume *volume = volumeList.Head();
	while (volume)
	{
		// Clean it up
		volumeList.Unlink(volume);
		volume->Unmount();
		delete volume;

		// next!
		volume = volumeList.Head();
	}
}

/***************************************************************************/
// Find an entry, returns NULL if not there
/***************************************************************************/
bool StreamLibrary::Find(
const char *name,
ImbeddedStreamInfo *info)
{
	StreamVolume *volume = volumeList.Head();
	while (volume)
	{
		// Here?
		if (volume->Find(name, info))
		  return true;

		// Try the next one
		volume = volume->next;
	}

	// Not in any of the volumes
	return false;
}

/***************************************************************************/
// Find a const ref to a string, NULL if not there
/***************************************************************************/
const char *StreamLibrary::GetTokenPtr(
const char *name)
{
	StreamVolume *volume = volumeList.Head();
	while (volume)
	{
		// Here?
		const char *token = volume->GetTokenPtr(name);
		if (token)
			return token;

		// Try the next one
		volume = volume->next;
	}

	// Not in any of the volumes
	return NULL;
}


