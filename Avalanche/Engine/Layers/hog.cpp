/***************************************************************************/
/* The HOG virtual file system (see l_hog.h for details)							*/
/***************************************************************************/
#include "Layers/LayersPCH.h"

#include "Layers/hogtoc.h"
#include "Layers/mediahal.h"

// CRC & Encryption!
#include "EngineHelper/Hash.h"
#include "GameHelper/encryption/Helix/Helix.h"

// File cache for Xbox
#if defined (_XBOX)
#include "Layers/FileCache/cache_xbox.h"
#include "Layers/DiscErrorHandler/DiscError.h"
#endif

#define VOLUME_DISK		0x0001
#define VOLUME_HOG		0x0002
#define VOLUME_MEMORY	0x0004
#define VOLUME_PRIVATE	0x1000

#define VOLUME_ONDISK(t)		(((t) & VOLUME_DISK) != 0)
#define VOLUME_INHOG(t)	 		(((t) & VOLUME_HOG) != 0)
#define VOLUME_INMEMORY(t)		(((t) & VOLUME_MEMORY) != 0)
#define VOLUME_ISPRIVATE(t)	(((t) & VOLUME_PRIVATE) != 0)
#define VOLUME_ISPUBLIC(t)		(((t) & VOLUME_PRIVATE) == 0)
#define VOLUME_ISENCRYPTED(v)	(v->hdr.encrypt != 0)

#define VOLUME_READ_RETRIES	64

// A mounted volume
struct ts_HogVolume
{
	HOGTOC_HDR				hdr;

	uint						type;
	HOG_CHECKLOCAL			checkLocal;	// Overrde for global checklocal
	union
	{
		t_FileHandle		file;
		HOGFILE				hog;
	} handle;
	HOGTOC_ENTRY			*contents;
	void						*data;		// non-null if in memory
};

// The possible types for a file

// Bitflags for the types
#define HOGFILE_STREAM			0x10
#define HOGFILE_MEMORY			0x20
#define HOGFILE_LOCAL			0x01
#define HOGFILE_VOLUME  	 	0x02
#define HOGFILE_HOG				0x03

// Some tests
#define HOGFILE_ISSTREAM(t)	(((t) & HOGFILE_STREAM) != 0)
#define HOGFILE_ISMEMORY(t)	(((t) & HOGFILE_MEMORY) != 0)

#define HOGFILE_ISLOCAL(t)		(((t) & HOGFILE_LOCAL) != 0)
#define HOGFILE_ISVOLUME(t)	(((t) & HOGFILE_VOLUME) != 0)

// The actual types
#define HOGFILE_FREE 			0
#define HOGFILE_LOCALSTREAM 	(HOGFILE_STREAM | HOGFILE_LOCAL)
#define HOGFILE_LOCALMEMORY 	(HOGFILE_MEMORY | HOGFILE_LOCAL)
#define HOGFILE_VOLUMESTREAM	(HOGFILE_STREAM | HOGFILE_VOLUME)
#define HOGFILE_VOLUMEMEMORY	(HOGFILE_MEMORY | HOGFILE_VOLUME)
#define HOGFILE_HOGSTREAM 		(HOGFILE_STREAM | HOGFILE_HOG)
#define HOGFILE_HOGMEMORY 		(HOGFILE_MEMORY | HOGFILE_HOG)

// An open file
struct ts_HogFile
{
	int						type;			// The location/type of this file
	int						size;			// size of file

	u32						openFlags;

	// Is this file associated with a volume?
	ts_HogVolume			*volume;
	HOGTOC_ENTRY			*tocEntry;

	union
	{
		t_FileHandle		file;			// File on disk
		HOGFILE				hog;			// another hog
	} handle;
	union
	{
		void					*buffer;		// data pointer (offset if fileHandle is valid)
		int					offset;
	} data;

	// In an ideal world (where this file is in the volume) this stuff is all valid
	int						pos;			// position in file
};

// Information for the volumes
static ts_HogVolume		*hogVolume = 0;
static int					hogMaxVolumes = 0;

// Information for the open files
static ts_HogFile			*hogFile = 0;
static int					hogMaxFiles = 0;
static int					hogNextFreeFile = 0;

// Do we check for local files?
const  HOG_CHECKLOCAL	defaultCheckLocal = CHECKLOCAL_NEVER;
static HOG_CHECKLOCAL	hogCheckLocal = defaultCheckLocal;

// Local functions
static ts_HogVolume *Hogi_AllocateVolume();
static void Hogi_FreeVolume(ts_HogVolume *volume);

static ts_HogFile *Hogi_OpenFile(HOGVOLUME volume, const char *pathFile, bool forceMemory, u32 openFlags);
static ts_HogFile *Hogi_OpenLocalFile(const char *pathFile, bool forceMemory, u32 openFlags);
static ts_HogFile *Hogi_OpenTOCEntry(ts_HogVolume *volume, HOGTOC_ENTRY *tocEntry, bool forceMemory);

static ts_HogFile *Hogi_AllocFile();
static void Hogi_FreeFile(ts_HogFile *file);

// Clean this file name up for a toc compare
void Hogi_PrepFileName(const char *pathFile, char *cleanFile);

// Does this file exist in this hog?
static HOGTOC_ENTRY *Hogi_FindFile(ts_HogVolume *volume, const char *pathFile);

// For volume op's
static bool _VolumeOpen(ts_HogVolume *volume, const char *hogName, bool inMemory);
static bool _VolumeClose(ts_HogVolume *volume);
static int _VolumeSize(ts_HogVolume *volume);
static bool _VolumeSeek(ts_HogVolume *volume, int pos);
static bool _VolumeRead(void *data, int size, ts_HogVolume *volume, bool sync);
static bool _VolumeReadCRC(void *data, int size, ts_HogVolume *volume, uint crc, bool sync);
static bool _VolumeReadHDR(ts_HogVolume *volume);

/***************************************************************************/
// Intialize the hog system
// If checkLocal is set, a local (non-hogged) version is scanned for.
/***************************************************************************/
bool Hog_Initialize(
int maxVolumes,
int maxOpenFiles,
HOG_CHECKLOCAL checkLocal)
{
	int size;

	ASSERT(maxVolumes > 0);
	ASSERT(maxOpenFiles > 0);
	ASSERT(hogVolume == 0);
	ASSERT(hogFile == 0);

	// Allocate the hog volume array
	hogMaxVolumes = maxVolumes;
	size = sizeof(ts_HogVolume) * hogMaxVolumes;
	if (size)
	{
		MEM_SET_ALLOC_NAME("Hog Volume Array");
		hogVolume = (ts_HogVolume *)memAlloc(size);
		ASSERT(hogVolume);
		if (!hogVolume)
			return false;
		memset(hogVolume, '\0', size);
	}

	// Allocate the files area
	hogMaxFiles = maxOpenFiles;
	size = sizeof(ts_HogFile) * hogMaxFiles;
	MEM_SET_ALLOC_NAME("Hog Files Area");
	hogFile = (ts_HogFile *)memAlloc(size);
	ASSERT(hogFile);
	if (!hogFile)
		return false;
	memset(hogFile, '\0', size);

	// save the check local flag
	Hog_SetCheckLocal(checkLocal);

	// Verify encryption assumptions are valid
	DWORD betterBeZero = g_helix.GetEncryptionHeaderSize();
	ASSERT(betterBeZero == 0);

	return true;
}

/***************************************************************************/
// Shut down the hog system (closes and frees everything)
/***************************************************************************/
void Hog_Terminate()
{
	int i;

	// Shut down the files
	if (hogFile)
	{
		// Close all the files
		for (i = 0; i < hogMaxFiles; i++)
			Hogi_FreeFile(&hogFile[i]);

		// Fre the array
		memFree(hogFile);
		hogFile = NULL;
	}
	hogMaxFiles = 0;
	hogNextFreeFile = 0;

	// free up the volumes
	if (hogVolume)
	{
		// Step through the volumes
		for (i = 0; i < hogMaxVolumes; i++)
			Hogi_FreeVolume(&hogVolume[i]);

		// clear it out
		memFree(hogVolume);
		hogVolume = NULL;
	}
	hogMaxVolumes = 0;

	// Revert this
	hogCheckLocal = defaultCheckLocal;
}

/***************************************************************************/
// Query static stats
/***************************************************************************/
int Hog_MaxVolumes()
{
	return hogMaxVolumes;
}

/***************************************************************************/
/***************************************************************************/
int Hog_MaxOpenFiles()
{
	return hogMaxFiles;
}

/***************************************************************************/
/***************************************************************************/
HOG_CHECKLOCAL Hog_CheckLocal()
{
	return hogCheckLocal;
}

/***************************************************************************/
/***************************************************************************/
void Hog_SetCheckLocal(
HOG_CHECKLOCAL checkLocal)
{
	if (checkLocal == CHECKLOCAL_DEFAULT)
		hogCheckLocal = defaultCheckLocal;
	else
		hogCheckLocal = checkLocal;
}

/***************************************************************************/
// Mount a virtual file system in to the master toc
// returns true if all went okay.
// If inMemory is set, the entire volume is loaded into
// memory (memory is discarded when Hog_UnmountVolume is called)
/***************************************************************************/
HOGVOLUME HogVolume_Mount(
const char *hogFile,
bool inMemory,
bool isPrivate)
{
	ts_HogVolume *volume;
	u32 i;
	unsigned int dataOffset;

	ASSERT(hogFile);
	ASSERT(*hogFile);

	// Find an open volume
	volume = Hogi_AllocateVolume();
	if (!volume)
		return NULL;

	// Set this to default
	volume->checkLocal = CHECKLOCAL_DEFAULT;

	// Open it
	if (!_VolumeOpen(volume, hogFile, inMemory))
		return NULL;

	// save the private
	if (isPrivate)
		volume->type |= VOLUME_PRIVATE;

	// load in the header or the whole shebang?
	if (!inMemory)
	{
		// Read in the header
		if (!_VolumeReadHDR(volume))
		{
			Hogi_FreeVolume(volume);
			return NULL;
		}

		// make the table of contents
		MEM_SET_ALLOC_FILENAME("HOGVMH", (char *)hogFile);
		volume->contents = (HOGTOC_ENTRY *)memAllocForIO(volume->hdr.tocSize);
		ASSERT(volume->contents);
		if (!volume->contents)
		{
			Hogi_FreeVolume(volume);
			return NULL;
		}

		// read in the TOC
		if (!_VolumeReadCRC(volume->contents, volume->hdr.tocSize, volume, volume->hdr.tocCRC, true))
		{
			Hogi_FreeVolume(volume);
			return NULL;
		}

		// Decrypt the contents
		if (VOLUME_ISENCRYPTED(volume))
			g_helix.Decrypt((const BYTE *)volume->contents, volume->hdr.tocSize, (BYTE *)volume->contents);

		// What is the offset of all the strings
		dataOffset = (unsigned int)volume->contents - volume->hdr.hdrSize;
	}
	else
	{
		// How big is the entire file?
		int size = _VolumeSize(volume);
		ASSERT(size > 0);

		// Allocate the chunk (lies on a 16, but needs be on a 64)
		MEM_SET_ALLOC_FILENAME("HOGVMF", (char *)hogFile);
		volume->data = memAllocForIO(size);

		// Make sure we didn't get a NULL
		ASSERT(volume->data);

		// failure would be bad
		if (!volume->data)
		{
			Hogi_FreeVolume(volume);
			return NULL;
		}

		// Read in the entire volume
		if (!_VolumeRead(volume->data, size, volume, true))
		{
			Hogi_FreeVolume(volume);
			return NULL;
		}

		// close the file
		_VolumeClose(volume);

		// setup the other data in the volume
		memcpy(&volume->hdr, volume->data, sizeof(HOGTOC_HDR));

		// Duh
		ASSERT(volume->hdr.version == HOGTOC_CURRENTVERSION);

		// setup the contents poitner
		volume->contents = (HOGTOC_ENTRY *)((unsigned int)volume->data + volume->hdr.hdrSize);

		// Decrypt the contents
		if (VOLUME_ISENCRYPTED(volume))
			g_helix.Decrypt((const BYTE *)volume->contents, volume->hdr.tocSize, (BYTE *)volume->contents);

		// What is the offset of all the strings
		dataOffset = (unsigned int)volume->data;

		// Fixup all the data ptr's in the toc
		for (i = 0; i < volume->hdr.entries; i++)
		{
			volume->contents[i].data = (char *)((unsigned int)volume->contents[i].data + dataOffset);

			// Decrypt the contents
			if (VOLUME_ISENCRYPTED(volume))
				g_helix.Decrypt((const BYTE *)volume->contents[i].data, volume->contents[i].size, (BYTE *)volume->contents[i].data);
		}
	}

	// Fixup all the strings in the toc
	for (i = 0; i < volume->hdr.entries; i++)
		volume->contents[i].name = (char *)((unsigned int)volume->contents[i].name + dataOffset);

#ifdef FULLHOGDUMP
	TRACE("Opening HOG \"%s\" %d entries.\n", hogFile, volume->hdr.entries);
	for (i = 0; i < volume->hdr.entries; i++)
	{
		const HOGTOC_ENTRY &entry = volume->contents[i];
		TRACE("Entry %d: \"%s\", offset %d size %d crc 0x%x\n", i, entry.name, entry.data, entry.size, entry.crc);
	}
#endif
	// Return the volume!
	return (HOGVOLUME)volume;
}

/***************************************************************************/
// Mount a virtual file system in to the master toc
// returns true if all went okay.
// File data is already in memory if this interface is used.
// Memory is NOT discarded when Hog_UnmountVolume is called,
// it is the responsibility of the caller to free the data buffer
/***************************************************************************/
HOGVOLUME HogVolume_Mount(
void *dataBuffer,
bool isPrivate)
{
	ts_HogVolume *volume;
	u32 i;
	unsigned int dataOffset;

	// Find an open volume
	volume = Hogi_AllocateVolume();
	if (!volume)
		return NULL;

	// Open it
	volume->type = VOLUME_MEMORY;
	volume->checkLocal = CHECKLOCAL_DEFAULT;

	// save the private
	if (isPrivate)
		volume->type |= VOLUME_PRIVATE;

	// setup the data pointer to be 64 byte aligned
	volume->data = dataBuffer;

	// setup the other data in the volume
	memcpy(&volume->hdr, volume->data, sizeof(HOGTOC_HDR));

	// Duh
	ASSERT(volume->hdr.version == HOGTOC_CURRENTVERSION);

	// setup the contents poitner
	volume->contents = (HOGTOC_ENTRY *)((unsigned int)volume->data + volume->hdr.hdrSize);

	// Decrypt the contents
	if (VOLUME_ISENCRYPTED(volume))
		g_helix.Decrypt((const BYTE *)volume->contents, volume->hdr.tocSize, (BYTE *)volume->contents);

	// What is the offset of all the strings
	dataOffset = (unsigned int)volume->data;

	// Fixup all the data ptr's in the toc
	for (i = 0; i < volume->hdr.entries; i++)
	{
	 	volume->contents[i].data = (char *)((unsigned int)volume->contents[i].data + dataOffset);

		// Decrypt the contents
		if (VOLUME_ISENCRYPTED(volume))
			g_helix.Decrypt((const BYTE *)volume->contents[i].data, volume->contents[i].size, (BYTE *)volume->contents[i].data);
	}

	// Fixup all the strings in the toc
	for (i = 0; i < volume->hdr.entries; i++)
		volume->contents[i].name = (char *)((unsigned int)volume->contents[i].name + dataOffset);

	// Return the volume!
	return (HOGVOLUME)volume;
}

/***************************************************************************/
// Unmount a volume.
/***************************************************************************/
void HogVolume_Unmount(
HOGVOLUME hv)
{
	// Did they pass us a null?
	ts_HogVolume *volume = (ts_HogVolume *)hv;
	if (!volume)
		return;

	// Close it down
	Hogi_FreeVolume(volume);
}

/***************************************************************************/
// Override the checklocal for a specific volume
/***************************************************************************/
void HogVolume_OverrideCheckLocal(
HOGVOLUME hv,
HOG_CHECKLOCAL checkLocal)
{
	// Did they pass us a null?
	ts_HogVolume *volume = (ts_HogVolume *)hv;
	if (!volume)
		return;

	// Set it!
	volume->checkLocal = checkLocal;
}

/***************************************************************************/
/***************************************************************************/
HOG_CHECKLOCAL HogVolume_CheckLocal(
HOGVOLUME hv)
{
	// Resolve the volume
	ts_HogVolume *volume = (ts_HogVolume *)hv;

	// What is the value?
	if (!volume || volume->checkLocal == CHECKLOCAL_DEFAULT)
		return hogCheckLocal;
	else
		return volume->checkLocal;
}

/***************************************************************************/
// Does this file exist?
// Pass in NULL to search all public volumes.
/***************************************************************************/
bool Hog_Exists(
HOGVOLUME volume,
const char *pathFile)
{
	// Find the file
	ts_HogFile *file = Hogi_OpenFile(volume, pathFile, false, 0);
	if (!file)
		return false;

	// close the file
	Hogi_FreeFile(file);
	return true;
}

/***************************************************************************/
// How big is this file? (returns < 0 if it doesn't exist)
// Pass in NULL to search all public volumes.
/***************************************************************************/
int Hog_Size(
HOGVOLUME volume,
const char *pathFile)
{
	int size;

	// Find the file
	ts_HogFile *file = Hogi_OpenFile(volume, pathFile, false, 0);
	if (!file)
		return -1;

	// Save off the size
	size = file->size;

	// close the file
	Hogi_FreeFile(file);

	return size;
}

/***************************************************************************/
// What is the offset this file?
// ONLY WORKS FOR DISK HOGS AND NOT REPLACED FILES
// (returns < 0 if it doesn't exist in hog)
// Pass in NULL to search all public volumes.
/***************************************************************************/
int Hog_Offset(
HOGVOLUME volume,
const char *pathFile)
{
	int offset;

	// Find the file
	ts_HogFile *file = Hogi_OpenFile(volume, pathFile, false, 0);
	if (!file)
		return -1;

	// Only works for streams in a disk hogfile
	if (file->type != HOGFILE_VOLUMESTREAM)
	{
		Hogi_FreeFile(file);
		return -1;
	}

	// Save off the offset
	offset = file->data.offset;

	// close the file
	Hogi_FreeFile(file);

	return offset;
}

/***************************************************************************/
// What is the offset and size of this file?
// ONLY WORKS FOR DISK HOGS AND NOT REPLACED FILES
// (returns false if it doesn't exist)
// Pass in NULL to search all public volumes.
/***************************************************************************/
bool Hog_OffsetSize(
HOGVOLUME volume,
const char *pathFile,
int &offset,
int &size)
{
	// Find the file
	ts_HogFile *file = Hogi_OpenFile(volume, pathFile, false, 0);
	if (!file)
		return false;

	// Only works for streams in a disk hogfile
	if (file->type != HOGFILE_VOLUMESTREAM)
	{
		Hogi_FreeFile(file);
		return false;
	}

	// Save off the stuff
	size = file->size;
	offset = file->data.offset;

	// close the file
	Hogi_FreeFile(file);

	return true;
}

/***************************************************************************/
// Open a file.  You can force it to open a memory copy.
// Pass in NULL to search all public volumes.
/***************************************************************************/
HOGFILE Hog_Open(
HOGVOLUME volume,
const char *pathFile,
bool inMemory,
u32 openFlags)
{
	// Find the file
	ts_HogFile *file = Hogi_OpenFile(volume, pathFile, inMemory, openFlags);

	// return the handle
	return (HOGFILE)file;
}

/***************************************************************************/
// Get a const char * to the TOC entry for this file. This allows you NOT to
// store the filename twice. Returns NULL if it does not exist.
/***************************************************************************/
const char *Hog_GetConstFileName(
HOGVOLUME volume, 
const char *pathFile)
{
	// Are they searching a specific volume?
	ts_HogVolume *checkVolume = (ts_HogVolume *)volume;

	// prep the filename for the search
	char cleanFile[128];
	ASSERT(strlen(pathFile) < sizeof(cleanFile));
	Hogi_PrepFileName(pathFile, cleanFile);

	// Searching a specific volume?
	HOGTOC_ENTRY *tocEntry;
	if (checkVolume)
		tocEntry = Hogi_FindFile(checkVolume, cleanFile);
	else
	{
		int i;

		// loop through each volume
		tocEntry = NULL;
		for (i = 0; i < hogMaxVolumes && !tocEntry; i++)
		{
			checkVolume = &hogVolume[i];

			// Only check public volumes
			if (VOLUME_ISPUBLIC(checkVolume->type))
				tocEntry = Hogi_FindFile(checkVolume, cleanFile);
		}
	}

	if (!tocEntry)
		return NULL;
	else
		return tocEntry->name;
}

/***************************************************************************/
// Get a pointer to a file.  Will return NULL if the file
// is a streaming file.
// If the volume containing this file is mounted inMemory, and the is
// unmounted, this data will go away.  Size returns the size of
// the file.
/***************************************************************************/
void *HogFile_Ptr(
HOGFILE hf,
int *size)
{
	// Did the pass us a null?
	ts_HogFile *file = (ts_HogFile *)hf;
	if (!file)
		return NULL;

	// Is this file already in memory?
	if (HOGFILE_ISMEMORY(file->type))
	{
		if (size)
			*size = file->size;
		return file->data.buffer;
	}

	// Okay, setup memory for this file
	ASSERT(false); // THIS DOESN'T WORK YET
	return NULL;
}

/***************************************************************************/
// Read from a file.  Returns the number of bytes read
/***************************************************************************/
int HogFile_Read(
void *buff,
int bytes,
HOGFILE hf)
{
	// Did the pass us a null?
	ts_HogFile *file = (ts_HogFile *)hf;
	if (!file)
		return 0;

	// Synchronous reads?
	bool sync = ((file->openFlags & MEDOPEN_FLAG_NOWAIT) == 0);

	// How much data can we read?
	int bytesToRead;
	if (file->pos + bytes > file->size)
		bytesToRead = file->size - file->pos;
	else
		bytesToRead = bytes;

	// Is this an in memory read?
	t_Error error;
	bool readRet;
	switch (file->type)
	{
		case HOGFILE_HOGSTREAM:
			// seek to the right spot
			HogFile_Seek(file->handle.hog, file->data.offset + file->pos, HOGSEEK_SET);

			// This would be bad
			if (HogFile_Read(buff, bytesToRead, file->handle.hog) != bytesToRead)
			{
				ASSERT(false);
				return 0;
			}
			break;

		case HOGFILE_LOCALSTREAM:
			// seek to the right spot
			error = MED_ERR_NONE;
			if (!MediaHAL::Seek(file->handle.file, file->data.offset + file->pos))
				error = Media::GetLastError();
			if (file->openFlags & MEDOPEN_FLAG_NOWAIT)
				MediaHAL::AwaitIO(file->handle.file, -1, NULL, NULL, &error);
			if (error != MED_ERR_NONE)
				return 0;

			// This would be bad
			if (!MediaHAL::Read(buff, bytesToRead, file->handle.file))
				return 0;
			break;

		case HOGFILE_VOLUMESTREAM:
			// This file should have a volume
			ASSERT(file->volume && file->tocEntry);

			// Seek to the right spot in the volume
			if (!_VolumeSeek(file->volume, file->data.offset + file->pos))
				return 0;

			// Are we reading the whole thing? If so, verify CRC
			if ((uint)bytesToRead < file->tocEntry->size)
			{
				ASSERT(!VOLUME_ISENCRYPTED(file->volume));
			  	readRet = _VolumeRead(buff, bytesToRead, file->volume, sync);
			}
			else
			{
				// Associate the hog file info with the disk file for async
				if (!sync)
					MediaHAL::SetContext(file->volume->handle.file, file);
				else
					MediaHAL::SetContext(file->volume->handle.file, NULL);

				// Read it
				readRet = _VolumeReadCRC(buff, bytesToRead, file->volume, file->tocEntry->crc, sync);

				// Decrypt the contents
				if (readRet && sync && VOLUME_ISENCRYPTED(file->volume))
					g_helix.Decrypt((const BYTE *)buff, bytesToRead, (BYTE *)buff);
			}

			// Did the read work?
			if (!readRet)
				return 0;
			break;

		case HOGFILE_HOGMEMORY:
		case HOGFILE_LOCALMEMORY:
		case HOGFILE_VOLUMEMEMORY:
			// Just blit it in
			memcpy(buff, (unsigned char *)file->data.buffer + file->pos, bytesToRead);
			break;

		default:
			// This is really bad
			ASSERT(0);
			break;
	}

	// For profiling
	Media::LogRead(bytesToRead);

	// Advance the file pointer
	file->pos += bytesToRead;

	return bytesToRead;
}

/***************************************************************************/
// Seek in a file, returns 0 on success, non-zero for failure
/***************************************************************************/
bool HogFile_Seek(
HOGFILE hf,
int offset,
HOGSEEK origin)
{
	ts_HogFile *file = (ts_HogFile *)hf;
	if (!file)
		return false;

	switch (origin)
	{
		case HOGSEEK_CUR:
			// Can we seek?
			if (file->pos + offset > file->size)
				return false;

			// move the pos
			file->pos += offset;
			break;

		case HOGSEEK_END:
			// Can't seek past end
			if (file->size + offset > file->size || file->size + offset < 0)
				return false;

			file->pos = file->size + offset;
			break;

		case HOGSEEK_SET:
			if (offset < 0 || offset > file->size)
				return false;

			file->pos = offset;
			break;
	}

	// we worked
	return true;
}

/***************************************************************************/
// What is the file pointer for this file
/***************************************************************************/
int HogFile_Tell(
HOGFILE hf)
{
	// Duh
	ts_HogFile *file = (ts_HogFile *)hf;
	if (!file)
		return -1;

	// what is our position
	return file->pos;
}

/***************************************************************************/
// How big is this file? 0 length files are allowed
/***************************************************************************/
int HogFile_Size(
HOGFILE hf)
{
	ts_HogFile *file = (ts_HogFile *)hf;
	if (!file)
		return -1;

	// return the size
	return file->size;
}

/***************************************************************************/
// Close the hogfile
/***************************************************************************/
bool HogFile_Close(
HOGFILE hf)
{
	ts_HogFile *file = (ts_HogFile *)hf;
	if (!file)
		return false;

	// Clear it out
	Hogi_FreeFile(file);
	return true;
}

/***************************************************************************/
/* Get the media fileHandle associated with this hog */
/***************************************************************************/
t_FileHandle HogFile_GetFileHandle(HOGFILE hf)
{
	ts_HogFile *file = (ts_HogFile *)hf;

	switch (file->type)
	{
		case HOGFILE_HOGSTREAM:
			return HogFile_GetFileHandle(file->handle.hog);

		case HOGFILE_LOCALSTREAM:
		case HOGFILE_VOLUMESTREAM:
			return file->handle.file;

		case HOGFILE_HOGMEMORY:
		case HOGFILE_LOCALMEMORY:
		case HOGFILE_VOLUMEMEMORY:
			return NULL;

		default:
			// This is really bad
			ASSERT(0);
			return NULL;
	}
}

/***************************************************************************/
// How many public volumes are there?
/***************************************************************************/
int Hog_Volumes()
{
	int i, count;

	count = 0;
	for (i = 0; i < hogMaxVolumes; i++)
	{
		// Non-private used volume?
		if (hogVolume[i].contents && VOLUME_ISPUBLIC(hogVolume[i].type))
			++count;
	}

	return count;
}

/***************************************************************************/
// Get the volume
/***************************************************************************/
HOGVOLUME Hog_GetVolume(
int whichVolume)
{
	int i, count;

	count = 0;
	for (i = 0; i < hogMaxVolumes; i++)
	{
		// Non-private used volume?
		if (hogVolume[i].contents && VOLUME_ISPUBLIC(hogVolume[i].type))
		{
			if (count == whichVolume)
				return (HOGVOLUME)&hogVolume[i];
			++count;
		}
	}

	// Can't find what they are looking for
	return NULL;
}

/***************************************************************************/
// How many files are in this hog? (NO NULL Volume HERE!)
/***************************************************************************/
int Hog_Files(
HOGVOLUME hv)
{
	// Did they pass us a null?
	ts_HogVolume *volume = (ts_HogVolume *)hv;
	if (!volume)
		return 0;

	// How many files are there
	return volume->hdr.entries;
}

/***************************************************************************/
// Get a specific file name (NO NULL Volume HERE!)
/***************************************************************************/
const char *Hog_GetFileName(
HOGVOLUME hv,
int whichFile)
{
	// Did they pass us a null?
	ts_HogVolume *volume = (ts_HogVolume *)hv;
	if (!volume)
		return NULL;

	// In range
	if (whichFile < 0 || whichFile >= (int)volume->hdr.entries)
		return NULL;

	// Give them the name
	return volume->contents[whichFile].name;
}

/***************************************************************************/
// Get a specific file size (NO NULL Volume HERE!)
/***************************************************************************/
int Hog_GetFileSize(
HOGVOLUME hv,
int whichFile)
{
	// Did they pass us a null?
	ts_HogVolume *volume = (ts_HogVolume *)hv;
	if (!volume)
		return -1;

	// In range
	if (whichFile < 0 || whichFile >= (int)volume->hdr.entries)
		return -1;

	// Give them the name
	return (int)volume->contents[whichFile].size;
}

/***************************************************************************/
// Get the "Raw" file handle for the volume in question.
// Returns INVALID_HANDLE if this is not a disk volume
/***************************************************************************/
t_FileHandle Hog_GetVolumeFileHandle(
HOGVOLUME hv)
{
	// Did they pass us a null?
	ts_HogVolume *volume = (ts_HogVolume *)hv;
	if (!volume || !VOLUME_ONDISK(volume->type))
		return INVALID_HANDLE;
	else
		return volume->handle.file;
}

/***************************************************************************/
// Find an open volume
/***************************************************************************/
ts_HogVolume *Hogi_AllocateVolume()
{
	int i;
	for (i = 0; i < hogMaxVolumes; i++)
	{
		if (hogVolume[i].contents == NULL)
			return &hogVolume[i];
	}

	// Crap, out of volumes
	ASSERT(0);
	return NULL;
}

/***************************************************************************/
/***************************************************************************/
void Hogi_FreeVolume(
ts_HogVolume *volume)
{
	if (!volume)
		return;

	// Is this an streaming volume?
	if (volume->data == NULL)
	{
		// free the contents
		if (volume->contents)
		{
			memFree(volume->contents);
			volume->contents = NULL;
		}
	}
	else // an in memory
	{
		if (volume->data)
		{
			memFree(volume->data);
			volume->data = NULL;
		}
		volume->contents = NULL;
	}

	// close the file (for an in memory will only be open during init)
	_VolumeClose(volume);
}

/***************************************************************************/
// Find the file in all the mounted volumes
/***************************************************************************/
ts_HogFile *Hogi_OpenFile(
HOGVOLUME volume,
const char *pathFile,
bool forceMemory,
u32 openFlags)
{
	// Are they searching a specific volume?
	ts_HogVolume *checkVolume = (ts_HogVolume *)volume;

	// Are we in memory?
	bool inMemory = (checkVolume && checkVolume->data);

	// What checklocal are we going to use?
	HOG_CHECKLOCAL checkLocal;
	if (!checkVolume || checkVolume->checkLocal == CHECKLOCAL_DEFAULT)
		checkLocal = hogCheckLocal;
	else
		checkLocal = checkVolume->checkLocal;

	// Cannot check local for memory hogs!!
	if (inMemory)
		checkLocal = CHECKLOCAL_NEVER;

	// Are we allowed to open the file locally?
	if (checkLocal == CHECKLOCAL_BEFORE)
	{
		ts_HogFile *file = Hogi_OpenLocalFile(pathFile, forceMemory, openFlags);
		if (file)
			return file;
	}

	// prep the filename for the search
	char cleanFile[128];
	ASSERT(strlen(pathFile) < sizeof(cleanFile));
	Hogi_PrepFileName(pathFile, cleanFile);

	// Searching a specific volume?
	HOGTOC_ENTRY *tocEntry;
	if (checkVolume)
		tocEntry = Hogi_FindFile(checkVolume, cleanFile);
	else
	{
		int i;

		// loop through each volume
		tocEntry = NULL;
		for (i = 0; i < hogMaxVolumes && !tocEntry; i++)
		{
			checkVolume = &hogVolume[i];

			// Only check public volumes
			if (VOLUME_ISPUBLIC(checkVolume->type))
				tocEntry = Hogi_FindFile(checkVolume, cleanFile);
		}
	}

	// Damn, no file do they want to check localy?
	if (!tocEntry)
	{
		if (checkLocal == CHECKLOCAL_AFTER)
		{
			ts_HogFile *file = Hogi_OpenLocalFile(pathFile, forceMemory, openFlags);
			if (file)
				return file;
		}
		return NULL;
	}

	// We have it! (this shouldn't fail unless we run out handles or something
	// hence the checklocal_after is before this)
	ts_HogFile *hogFile = Hogi_OpenTOCEntry(checkVolume, tocEntry, forceMemory);
	if (hogFile)
	{
		hogFile->openFlags = openFlags;
		Media::LogFileOpen(pathFile);
	}

	return hogFile;
}

/***************************************************************************/
// open the file locally
/***************************************************************************/
ts_HogFile *Hogi_OpenLocalFile(
const char *pathFile,
bool forceMemory,
u32 openFlags)
{
	ts_HogFile *file;

	// attempt to open the file
	t_FileHandle fileHandle;

#if defined (_XBOX)
//	char		*Cache_Fname;
//	char		*Source_Fname;
	bool		Drive = false;
//	BOOL		Err;
	// check utility drive first for cached files
	if (!MediaHAL::Open(Media::GetUtilityFileName(pathFile), &fileHandle, openFlags))
	{
		if (!MediaHAL::Open(Media::GetUtilityCommonFileName(pathFile), &fileHandle, openFlags))
		{
			// not found on Z:... so lets make a copy of it on Z if it finds the file
			if (!MediaHAL::Open(Media::GetFileName(pathFile), &fileHandle, openFlags))
			{
				#ifndef DONT_SEARCH_DUAL_DIRECTORIES
				if (!MediaHAL::Open(Media::GetCommonFileName(pathFile), &fileHandle, openFlags))
				{
					#if defined (CDROM)
//					DiscErrorHandler::DisplayError((char*)pathFile, DiscErrorHandler::READ_FILE_FAILED);		// error warning for failed load
					#endif
					return NULL;
				}
				#endif
			}
		}
	}

	// copy it to the hard drive first..
//	CFileCache::CacheFileAsync((char*)pathFile);

#else
	if (!MediaHAL::Open(Media::GetFileName(pathFile), &fileHandle, openFlags))
	{
#ifndef DONT_SEARCH_DUAL_DIRECTORIES
		if (!MediaHAL::Open(Media::GetCommonFileName(pathFile), &fileHandle, openFlags))
			return NULL;
#endif
	}
#endif		// XBOX

	// Allocate a file structure
	file = Hogi_AllocFile();
	if (!file)
	{
		MediaHAL::Close(fileHandle);
		return NULL;
	}

	// setup the size and position
	file->size = MediaHAL::Length(fileHandle);
	file->pos = 0;			// position relative to file start
	file->openFlags = openFlags;

	// We are not associated with a volume
	file->volume = NULL;
	file->tocEntry = NULL;

	// It exists, are we forcing memory?
	if (!forceMemory)
	{
		file->type = HOGFILE_LOCALSTREAM;
		file->handle.file = fileHandle;
		file->data.offset = 0;
	}
	else
	{
		// local memory file
		file->type = HOGFILE_LOCALMEMORY;

		// Allocate a buffer

		MEM_SET_ALLOC_FILENAME("HOGOPEN", (char *)pathFile);
		file->data.buffer = memAllocForIO(file->size);
		if (!file->data.buffer)
		{
			MediaHAL::Close(fileHandle);
			Hogi_FreeFile(file);
			return NULL;
		}

		// Read us in
		if (!MediaHAL::Read(file->data.buffer, file->size, fileHandle))
		{
			MediaHAL::Close(fileHandle);
			Hogi_FreeFile(file);
			return NULL;
		}

		if (openFlags & MEDOPEN_FLAG_NOWAIT)
			MediaHAL::AwaitIO(fileHandle, -1, NULL, NULL, NULL);

		// Close the file
		MediaHAL::Close(fileHandle);
	}

	// return the file, ready to go
	return file;
}

/***************************************************************************/
// Open this file based on a TOC entry
/***************************************************************************/
ts_HogFile *Hogi_OpenTOCEntry(
ts_HogVolume *volume,
HOGTOC_ENTRY *tocEntry,
bool forceMemory)
{
	// Allocate the file structure
	ts_HogFile *file = Hogi_AllocFile();
	if (!file)
		return NULL;

	// setup the file info
	file->size = tocEntry->size;
	file->pos = 0;

	// We are associated with a volume
	file->volume = volume;
	file->tocEntry = tocEntry;

	// This volume in memory?
	if (volume->data)
	{
		// memory volume
		if (VOLUME_INHOG(volume->type))
			file->type = HOGFILE_HOGMEMORY;
		else
			file->type = HOGFILE_VOLUMEMEMORY;
		file->handle.file = 0;
		file->data.buffer = tocEntry->data;
	}
	else
	{
		// streaming volume
		if (VOLUME_INHOG(volume->type))
		{
			file->type = HOGFILE_HOGSTREAM;
			file->handle.hog = volume->handle.hog;
		}
		else
		{
			file->type = HOGFILE_VOLUMESTREAM;
			file->handle.file = volume->handle.file;
		}
		file->data.offset = (int)tocEntry->data;
	}

	return file;
}

/***************************************************************************/
// Find a free file structure
/***************************************************************************/
ts_HogFile *Hogi_AllocFile()
{
	int i;
	for (i = 0; i < hogMaxFiles; i++)
	{
		if (hogFile[i].type == HOGFILE_FREE)
			return &hogFile[i];
	}

	// No file open structures left
	ASSERT(0);
	return NULL;
}

/***************************************************************************/
// Free the file
/***************************************************************************/
void Hogi_FreeFile(
ts_HogFile *file)
{
	// Is this a local file?
	switch (file->type)
	{
		case HOGFILE_FREE:
			return;

		case HOGFILE_LOCALSTREAM:
			MediaHAL::Close(file->handle.file);
			break;

		case HOGFILE_LOCALMEMORY:
			memFree(file->data.buffer);
			break;

		case HOGFILE_VOLUMESTREAM:
		case HOGFILE_VOLUMEMEMORY:
			// nothing special about volume files
			break;

		case HOGFILE_HOGSTREAM:
		case HOGFILE_HOGMEMORY:
			// Don't do squat
			break;

		default:
			// This would be really bad
			ASSERT(false);
			break;
	}

	// We are now unused
	file->type = HOGFILE_FREE;
}

/***************************************************************************/
// Clean this file name up for a toc compare
/***************************************************************************/
void Hogi_PrepFileName(
const char *pathFile,
char *cleanFile)
{
	int i, len;

	// Unbogart the name
	if (pathFile[0] == '.' && (pathFile[1] == '\\' || pathFile[1] == '/'))
		pathFile += 2;
	else if (pathFile[0] == '\\' || pathFile[0] == '/')
		++pathFile;

	// Translate slashes
	len = strlen(pathFile);
	for (i = 0; i < len; i++)
	{
		if (pathFile[i] == '/')
			cleanFile[i] = '\\';
		else
			cleanFile[i] = toupper(pathFile[i]);
	}
	cleanFile[i] = '\0';
}

/***************************************************************************/
// Does this file exist in this hog?
/***************************************************************************/
HOGTOC_ENTRY *Hogi_FindFile(
ts_HogVolume *volume,
const char *pathFile)
{
	int low, high, mid, cmp;

	// Do a binary pathFile
	low = 0;
	high = volume->hdr.entries - 1;
	while (low <= high)
	{
		mid = (low + high) >> 1;

		cmp = strcmp(pathFile, volume->contents[mid].name);
		if (cmp == 0)
			return &volume->contents[mid];
		else if (cmp < 0)
			high = mid - 1;
		else if (cmp > 0)
			low = mid + 1;
	}

	// no luck
	return NULL;
}

/***************************************************************************/
// For volume op's
/***************************************************************************/
static bool _VolumeOpen(
ts_HogVolume *volume,
const char *hogName,
bool inMemory)
{
	// Try to open it in the hog first
	HOG_CHECKLOCAL checkLocalSave = hogCheckLocal;
	hogCheckLocal = CHECKLOCAL_NEVER;
	volume->handle.hog = Hog_Open(NULL, hogName, inMemory, 0);
	hogCheckLocal = checkLocalSave;

	if (volume->handle.hog)
		volume->type = VOLUME_HOG;
	else
	{
	 	// open the file (doesn't claim hog if it fails)
		if (!MediaHAL::Open(Media::GetFileName(hogName), &volume->handle.file, MEDOPEN_FLAG_NOWAIT))
			if (!MediaHAL::Open(Media::GetCommonFileName(hogName), &volume->handle.file, MEDOPEN_FLAG_NOWAIT))
				return false;
		volume->type = VOLUME_DISK;
	}

	return true;
}

/***************************************************************************/
/***************************************************************************/
static bool _VolumeClose(
ts_HogVolume *volume)
{
	if (VOLUME_ONDISK(volume->type))
	{
		if (volume->handle.file)
			MediaHAL::Close(volume->handle.file);
		volume->handle.file = 0;
	}
	else if (VOLUME_INHOG(volume->type))
	{
		HogFile_Close(volume->handle.hog);
		volume->handle.hog = NULL;
	}
	else
	{
		volume->handle.file = 0;
	}

	return true;
}

/***************************************************************************/
/***************************************************************************/
static int _VolumeSize(
ts_HogVolume *volume)
{
	if (VOLUME_ONDISK(volume->type))
		return MediaHAL::Length(volume->handle.file);
	else
		return HogFile_Size(volume->handle.hog);
}

/***************************************************************************/
/***************************************************************************/
static bool _VolumeSeek(
ts_HogVolume *volume,
int pos)
{
	if (!VOLUME_ONDISK(volume->type))
		return HogFile_Seek(volume->handle.hog, pos, HOGSEEK_SET);
	else
	{
		MediaHAL::Seek(volume->handle.file, pos);
		return MediaHAL::AwaitIO(volume->handle.file, -1, NULL, NULL, NULL);
	}
}

/***************************************************************************/
/***************************************************************************/
static bool _VolumeRead(
void *data,
int size,
ts_HogVolume *volume,
bool sync)
{
	// Read in the header
	if (!VOLUME_ONDISK(volume->type))
      return (HogFile_Read(data, size, volume->handle.hog) == size);
	else
	{
		MediaHAL::Read(data, size, volume->handle.file);
		if (sync)
			return MediaHAL::AwaitIO(volume->handle.file, -1, NULL, NULL, NULL);
		else
			return true;
	}
}

/***************************************************************************/
/***************************************************************************/
static bool _VolumeReadCRC(
void *data,
int size,
ts_HogVolume *volume,
uint crc,
bool sync)
{
	int retries = VOLUME_READ_RETRIES;
	while (retries > 0)
	{
		if (!_VolumeRead(data, size, volume, sync))
			return false;

		// We can only check the crc if the read was synchronus
		if (!sync)
			return true;
		else
		{
			// Check the crc
			uint diskCRC = Hash::CalcChecksum(data, size);
			ASSERT(diskCRC == crc);
			if (diskCRC == crc)
				return true;
		}
		--retries;
	}

	// failure hurts!
	return false;
}

/***************************************************************************/
/***************************************************************************/
static bool _VolumeReadHDR(
ts_HogVolume *volume)
{
	// Read in the header
	void *diskHeader = memAllocForIO(sizeof(HOGTOC_HDR));
	if (!diskHeader)
		return false;

	int retries = VOLUME_READ_RETRIES;
	bool ret = false;
	while (retries > 0)
	{
		// Read it in
		if (_VolumeRead(diskHeader, sizeof(HOGTOC_HDR), volume, true))
		{
			// Save it off
			memcpy(&volume->hdr, diskHeader, sizeof(HOGTOC_HDR));
			uint readCRC = volume->hdr.crc;
			volume->hdr.crc = 0;

			// IS the CRC right
			uint diskCRC = Hash::CalcChecksum(&volume->hdr, sizeof(HOGTOC_HDR));
			ASSERT(diskCRC == readCRC);
			if (diskCRC == readCRC)
			{
				// Verify the version
				ASSERT(volume->hdr.version == HOGTOC_CURRENTVERSION);
				if (volume->hdr.version != HOGTOC_CURRENTVERSION)
					break;

				// skip the padding on the header
				_VolumeSeek(volume, volume->hdr.hdrSize);
				ret = true;
				break;
			}
		}

		--retries;
	}

	// We're outta here
	memFree(diskHeader);
	return ret;
}

/***************************************************************************/
// Hog needs to know when IO is complete
/***************************************************************************/
bool Hogi_AsyncReadComplete(
void *ioBuffer,
u32 ioSize,
t_Error error,
void *context)
{
	// If this is a hog file, we need to do more work
	ts_HogFile *file = (ts_HogFile *)context;
	if (!file)
		return false;

	// Verify the CRC
	if (ioSize == file->tocEntry->size)
	{
		uint diskCRC = Hash::CalcChecksum(ioBuffer, file->tocEntry->size);
		ASSERT(diskCRC == file->tocEntry->crc);
		if (diskCRC != file->tocEntry->crc)
			return true;
	}

	// Decrypt the contents
	if (VOLUME_ISENCRYPTED(file->volume))
		g_helix.Decrypt((const BYTE *)ioBuffer, ioSize, (BYTE *)ioBuffer);

	return false;
}



