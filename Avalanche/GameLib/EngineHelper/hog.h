/***************************************************************************/
// Nice C++ wrapper around the HOG api.  Mostly for making private
// volumes...(see l_hog.h)
/***************************************************************************/
#ifndef HOG_H
#define HOG_H

#include "Layers/hog.h"

class Hog
{
public:
	// these don't do much for error reporting reasons
	Hog(void);
	~Hog(void);

	// Mount/unmount the hog (if you aren't setting isPrivate, you
	// are probably using this for the wrong reasons)
	bool Mount(const char *hogFile, bool inMemory = true, bool isPrivate = true);
	bool MountBuffer( void *dataBuffer, bool isPrivate = true);
	bool Unmount(void);

	// Override the checklocal behavior
	bool OverrideCheckLocal(HOG_CHECKLOCAL checkLocal);
	HOG_CHECKLOCAL CheckLocal();

	// How many files are in this hog?
	int Files(void);

	// Get a file name/size
	const char *FileName(int whichFile);
	int FileSize(int whichFile);

	// Does this file exist in the hog
	bool FileExists(const char *file);

	// What is the size of this file
	int FileSize(const char *file);

	// What is the disk offset? (-1 if it doesn't exist)
	int FileOffset(const char *file);

	// Get the size and offset in one fell swoop
	// returns false if it doesn't exist
	bool FileOffsetSize(const char *file, int &offset, int &size);

	// Get a const char * to the TOC entry for this file. This allows you NOT to
	// store the filename twice. Returns NULL if it does not exist.
	const char *GetConstFileName(const char *pathFile);

	// Get some lower level information about the file under this
	// see the chained calls for more info
	t_FileHandle GetVolumeFileHandle()
		{ return Hog_GetVolumeFileHandle(hv); }
	long GetPlatformFileHandle()
		{ return MediaHAL::PlatformFileHandle(GetVolumeFileHandle()); }

	// Open a file
	HOGFILE FileOpen(const char *file, bool inMemory = true, u32 openFlags = 0);

	// Get a pointer to a file (only works if inMemory was set)
	static void *FilePtr(HOGFILE hf, int &size);

	// How big is the file
	static int FileSize(HOGFILE hf);

	// Read from a file.  Returns the number of bytes read
	static int FileRead(void *buff, int bytes, HOGFILE hf);

	// Seek in a file
	static bool FileSeek(HOGFILE hf, int offset, HOGSEEK origin = HOGSEEK_SET);

	// What is the file pointer for this file
	static int FileTell(HOGFILE hf);

	// Close the hogfile (if an in memory, the memory goes away)
	static bool FileClose(HOGFILE hf);

private:
	// Our volume!
	HOGVOLUME	hv;
};

#endif
