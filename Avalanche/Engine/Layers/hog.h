/***************************************************************************/
/* HOG files (built by the HOG tool) are a simple virtual file system that
	consist of a alphabetcially sorted table of contents followed by all
	the file data.  A hog file is mounted as a "volume", and then the
	individual files within the volume can be operated on using standard
	C/stdio-type functions.  This mode is called "streaming".

	Alternately, a HOG can be mounted "in memory" meaning the entire file
	is loaded at mount time.  Then all the files can be accessed using
	ptrs.  When the volume is unmounted, the memory is freed.

	If a HOG is opened in "streaming" mode, the "checkLocal" flag will cause
	the system to check for a real local file.  If the local file exists,
	it will be used instead of the HOG'd file.  The dates of the files are
	not compared, simply if a real OS version of the file exists, it overrides
	the hog (if the flag is set).

	HOG's do not support writing in any way.

	HOG's do not support an fancy sscanf type stuff.
*/
/***************************************************************************/
#ifndef __HOG_H
#define __HOG_H

#include "platform/BaseType.h"
#include "Layers/mediahal.h"

// A hog volume is a single disk file that contains multiple files
typedef void *HOGVOLUME;

// A hog file is a pointer to a file
typedef void *HOGFILE;

// How do we look for local files?
// Before checking the hog, after checking the hog or ner?
typedef enum
{
	CHECKLOCAL_BEFORE = 0,		// check local files before hog
	CHECKLOCAL_AFTER,				// check local files after hog
	CHECKLOCAL_NEVER,				// never check local files
	CHECKLOCAL_DEFAULT			// Use the current global default
} HOG_CHECKLOCAL;

// Intialize the hog system
// If checkLocal is set, a local (non-hogged) version is scanned for.
// returns 0 on failure, 1 if okay.
bool Hog_Initialize(int maxVolumes, int maxOpenFiles, HOG_CHECKLOCAL checkLocal);

// Shut down the hog system (closes and frees everything)
void Hog_Terminate();

// Query static stats
int Hog_MaxVolumes();
int Hog_MaxOpenFiles();

// Change/Get the check local state
HOG_CHECKLOCAL Hog_CheckLocal();
void Hog_SetCheckLocal(HOG_CHECKLOCAL checkLocal);

// Mount a virtual file system in to the master toc
// returns true if all went okay.
// If inMemory is set, the entire volume is loaded into
// memory (memory is discarded when Hog_UnmountVolume is called)
//HOGVOLUME HogVolume_Mount(const char *hogFile, bool inMemory, bool isPrivate);
// Just to get it to compile.
HOGVOLUME HogVolume_Mount(const char *hogFile, bool inMemory, bool isPrivate);
HOGVOLUME HogVolume_Mount(void *dataBuffer, bool isPrivate);

// Unmount a volume.
void HogVolume_Unmount(HOGVOLUME hv);

// Override the checklocal for a specific volume
void HogVolume_OverrideCheckLocal(HOGVOLUME hv, HOG_CHECKLOCAL checkLocal);
HOG_CHECKLOCAL HogVolume_CheckLocal(HOGVOLUME hv);

// Does this file exist in the hog?
// Pass in NULL to search all public volumes.
bool Hog_Exists(HOGVOLUME volume, const char *pathFile);

// How big is this file? (returns < 0 if it doesn't exist)
// This has some overhead, so if you can open the file
// then get it's size, that would be more efficient.
// Pass in NULL to search all public volumes.
int Hog_Size(HOGVOLUME volume, const char *pathFile);

// What is the offset this file?
// ONLY WORKS FOR DISK HOGS AND NOT REPLACED FILES
// (returns < 0 if it doesn't exist in hog)
// Pass in NULL to search all public volumes.
int Hog_Offset(HOGVOLUME volume, const char *pathFile);

// What is the offset and size of this file?
// ONLY WORKS FOR DISK HOGS AND NOT REPLACED FILES
// (returns false if it doesn't exist)
// Pass in NULL to search all public volumes.
bool Hog_OffsetSize(HOGVOLUME volume, const char *pathFile, int &offset, int &size);

// Open a file.  You can force it to open a memory copy
// Pass in NULL to search all public volumes.
HOGFILE Hog_Open(HOGVOLUME volume, const char *pathFile, bool inMemory, u32 openFlags);

// Get a const char * to the TOC entry for this file. This allows you NOT to
// store the filename twice. Returns NULL if it does not exist.
const char *Hog_GetConstFileName(HOGVOLUME volume, const char *pathFile);

// Get a pointer to a file.  Will return NULL if the file
// is a streaming file.
// If the volume containing this file is mounted inMemory, and the is
// unmounted, this data will go away.  Size returns the size of
// the file.
void *HogFile_Ptr(HOGFILE hf, int *size);

// Read from a file.  Returns the number of bytes read
int HogFile_Read(void *buff, int bytes, HOGFILE hf);

// Types of seeks (mimic's stdio)
typedef enum
{
	HOGSEEK_CUR	= 0,
	HOGSEEK_END	= 1,
	HOGSEEK_SET	= 2
} HOGSEEK;

// Seek in a file
bool HogFile_Seek(HOGFILE hf, int offset, HOGSEEK origin);

// What is the file pointer for this file
int HogFile_Tell(HOGFILE hf);

// How big is this file? 0 length files are allowed
int HogFile_Size(HOGFILE hf);

// Close the hogfile (if an in memory, the memory goes away)
bool HogFile_Close(HOGFILE hf);

// Get the low level file handle
t_FileHandle HogFile_GetFileHandle(HOGFILE hf);

//---------------------------------------------
// Enumeration functions
//---------------------------------------------

// How many public volumes are there?
int Hog_Volumes();

// Get the volume
HOGVOLUME Hog_GetVolume(int whichVolume);

// How many files are in this hog? (NO NULL Volume HERE!)
int Hog_Files(HOGVOLUME volume);

// Get a specific file name (NO NULL Volume HERE!)
const char *Hog_GetFileName(HOGVOLUME volume, int whichFile);

// Get a specific file size (NO NULL Volume HERE!)
int Hog_GetFileSize(HOGVOLUME volume, int whichFile);

// Get the "Raw" file handle for the volume in question.
// Returns INVALID_HANDLE if this is not a disk volume
t_FileHandle Hog_GetVolumeFileHandle(HOGVOLUME hv);

#endif
