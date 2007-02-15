#ifndef __MEDIA_H
#define __MEDIA_H
// -----------------7/22/98 2:29PM-------------------
// Includes
// --------------------------------------------------

// engine includes
#include "platform/BaseType.h"
#include "Layers/LayersStruct.h"
#include "Layers/hog.h"

class PatchExtractor;

// -----------------7/22/98 2:29PM-------------------
// Exported Defines
// --------------------------------------------------
typedef enum
{
	MED_ERR_NONE = 0,
	MED_ERR_OPEN,
	MED_ERR_SEEK,
	MED_ERR_READ,
	MED_ERR_MAXOPEN,
	MED_ERR_NOTOPEN,
	MED_ERR_NOT_SUPPORTED,
	MED_ERR_CRC_BAD,
	MED_ERR_BADPARAM,
	MED_ERR_WRITE,
	MED_ERR_TIMEOUT,
	MED_ERR_NO_IO,
	MED_ERR_IO_PENDING,

	// These are major async errors
	MED_ERR_COVEROPEN,
	MED_ERR_NODISK,
	MED_ERR_WRONGDISK,
	MED_ERR_RETRY,
	MED_ERR_FATAL,

	MED_ERR_MAX
} MediaError;

#define MEDOPEN_FLAG_WRITE				0x00000001
#define MEDOPEN_FLAG_NOWAIT			0x00000002
#define MEDOPEN_FLAG_NO_DISCERROR	0x00000004
#define MEDOPEN_FLAG_CACHE				0x00000008

// max string len for a file name on any platform
#define MAX_FILENAME_LEN 256

// Forwards
class MediaFile;

// Statis singleton class
class Media
{
public:
	// Init the media system, we have a limit on open files
	static void	Init(int maxOpenFiles);
	static void Terminate();

	// Set the game title name for error messages
	static void SetTitle(const char *title);

	// Call this every frame
	static void Daemon(void);

	// Query major errors
	static MediaError QueryError(void);

	// Check the caps on the media system
	typedef enum
	{
		SHORT_READ = 1,
	} Caps;
	static bool	CheckCaps(Caps cap);

	// Mount a HOG volume into the media system
	static bool	MountHog(const char *hogFile);

	// Mount a HOG volume into the media system in memory,
	// be very careful with this, it loads the entire hog
	static bool	MountMemoryHog(const char *hogFile);

	// Unmount a HOG volume into the media system
	static bool	UnmountHog(const char *hogFile);

	// start an open "profile", basically a log of file opens
	// Writes to openNNNN.lst, where NNNN increments to avoid collisions
	static bool	StartOpenProfile(void);
	static bool	StartOpenProfile(const char *worldName);

	// End the profile
	static bool	EndOpenProfile(void);

	// Clear the read counter
	static bool	ClearReadTotal(void);

	// Get the read counter
	static s32	GetReadTotal(void);

	// Set the root dir
	static void	SetRootDir(const char *newRootDir);

	// convert the file name into suitable format for whatever platform we're on in the platform dependent directory tree
	static void MakeFileName(const char *baseFileName, char *realFileName);
	static char	*GetFileName(const char *baseFileName);

	// convert the file name into suitable format for whatever platform we're on, but in the platform independent directory tree
	static char	*GetCommonFileName(const char *baseFileName);

	#if defined (_XBOX)
	// Xbox specific to get Utility drive for cached files
	static char *GetUtilityFileName(const char *baseFileName);
	static char *GetUtilityCommonFileName(const char *baseFileName);
	#endif

	// Set and Get the state of name-mangling of file names
	static void	EnableNameMangling(bool bState);
	static bool	GetNameManglingState(void);

	// Does this file exist
	static bool	FileExists(const char *fileName);

	// Does this media stream exist?  This bypasses all Hog caching since most of
	// the platforms use a separate I/O path for media streams.
	static bool	StreamExists(const char *fileName);

	// What is the size of this file
	static s32	FileSize(const char *fileName);

	// Load in the entire file into an memAlloc'd block
	static void *Load(const char *fileName, s32 *size, t_Error *pError, int extraAlloc = 0);

	// Load in the entire file into a block
	static bool	LoadStatic(const char *fileName, void *pBuffer, s32 maxSize, s32 *size, t_Error *pError);

	// Open a file
	static MediaFile *Open(const char *fileName, t_Error *pError, u32 openFlags = 0);

	// Start asynchronous load of a file
	static MediaFile *StartAsyncLoad(const char *fileName, void **ppBuffer, int *pnFileSize, int extraAlloc = 0);

	// Close all files
	static void	CloseAll(void);

	// Error handling stuff
	static t_Error GetLastError(void);
 	static void ResetError(void);

	// Change an error code into text
	static const char *GetErrorText(t_Error errorCode);

	// Patching
	static void EnablePatching( char *pPatchBuffer, int size );
	static void DisablePatching( void );
	static bool isDLL( const char *filename );
	static PatchExtractor* GetPatchExtractor();

private:
	typedef struct
	{
		char			name[128];
		HOGVOLUME	volume;
	} OpenHog;

	// Internals
	static MediaFile		*GetOpenFile(void);
	static void				ConvertSlashes(char *fileName);
	static OpenHog			*GetOpenHog(void);
	static OpenHog			*FindOpenHog(const char *hogFile);

 	// Local Data
	static int				maxOpen;
	static MediaFile		*openList;

	static OpenHog			*openHog;
	static int				openHogs;

	// Profiling
	static MediaFile		*openProfile;
	static s32				readTotal;

	static bool				m_patchingEnabled;
	static char				*m_pPatchBuffer;
	static PatchExtractor *m_pPatchExtractor;

	// Utilities
	static inline void SetError(t_Error *errorCode)
		{ if (errorCode) *errorCode = MediaHAL::lastError; }

	friend class MediaFile;

public:
	// These aren't intended for general use, just for other
	// systems that may access IO stuff directly
	static void				LogFileOpen(const char *fileName);
	static inline void 	LogRead(s32 read)
		{ readTotal += read; }
};

class MediaFile
{
public:
	// How big is the file?
	s32	Size(void);

	// Seek to an offset on the file
	bool 	Seek(s32 pos);

	// Read in some data
	bool	Read(void *buffer, s32 len, t_Error *error);

	// Write some data
	bool	Write(const void *buffer, s32 len, t_Error *error);

	// Wait for a file IO to finish
	// File must have been 'medOpen'ed with MEDOPEN_FLAG_NOWAIT
	// Returns true if IO has completed, you need to check pError to see if it completed successfully
	// if timeOut == 0, just does a completion check( i.e. returns immediately)
	// if timeOut > 0, waits for timeOut milliseconds for the IO to complete
	// if timeOut < 0, waits for the IO to complete
	bool	AwaitIO(s32 timeOut, void **buffer = NULL, s32 *ioSize = NULL, t_Error *error = NULL);

	// Close the file
	bool	Close(t_Error *error = NULL);

private:
	// Modes for an open file
	typedef enum
	{
		OPENFILE_UNUSED = 0,
		OPENFILE_HOG,
		OPENFILE_FILE
	} OPENFILE_MODE;

	// put more stuff in here if you like
	OPENFILE_MODE		mode;
	union
	{
		HOGFILE			hog;
		t_FileHandle	file;			// for write files only
	};

	// Only media can create us
	friend class Media;
	MediaFile(void) { mode = OPENFILE_UNUSED; }
	~MediaFile(void) { ASSERT(mode == OPENFILE_UNUSED); }

	// Set us up
	bool Open(const char *fileName, t_Error *error, u32 openFlags);

	// In use?
	bool IsOpen(void)
		{ return(mode != OPENFILE_UNUSED); }
	bool IsHog(void)
		{ return(mode == OPENFILE_HOG); }
	bool IsFile(void)
		{ return(mode == OPENFILE_FILE); }

	// Utilities
	static inline void SetError(t_Error *errorCode)
		{ if (errorCode) *errorCode = MediaHAL::lastError; }
};

#endif
