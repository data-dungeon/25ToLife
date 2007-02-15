/***************************************************************************/
/* Low level (platform independent) file I/O
/***************************************************************************/
#ifndef __MEDIALOW_H
#define __MEDIALOW_H

#include "Layers/LayersStruct.h"
#include "container/dllist.h"

/* each platform has a different root directory */
#define COMMON_DATA_ROOT	"DATA\\"
#define GCN_DATA_ROOT		"DATA_GCN\\"
#define PS2_DATA_ROOT		"DATA_PS2\\"
#define XBOX_DATA_ROOT		"DATA_XBOX\\"

#if defined (DIRECTX)			// temp PAB
#define PC_DATA_ROOT			"DATA_DX\\"
#else
#define PC_DATA_ROOT			"DATA_PC\\"
#endif

// Opaque handle for a file
typedef void *t_FileHandle;

// Each platform has it's own twiddly way of defining an error code
#if defined(WIN32)
typedef DWORD			t_FileError;
#elif defined(PS2)
typedef int				t_FileError;
#elif defined(GCN)
typedef s32				t_FileError;
#endif

// little data structure to hold timestamps
typedef struct
{
	int nYear;		// 0-based (e.g. this year is 2003)
	int nMonth;		// 0..11
	int nDay;		// 0..30
	int nHour;		// 0..23
	int nMinute;	// 0..59
	int nSecond;	// 0..59
} ts_TimeStamp;

// Pending IO types
typedef enum
{
	MED_IO_TYPE_NONE,
	MED_IO_TYPE_SEEK,
	MED_IO_TYPE_READ,
	MED_IO_TYPE_WRITE
} MED_IO_TYPE;

// This holds the data for an open file
class HALFile
{
public:
	// these fields are common to all platforms
	u32		 		openFlags;
	s32				filePos;
	s32				fileSize;
	void				*ioBuffer;
	u32				ioSize;
	bool				ioPending;
	MED_IO_TYPE		ioType;
	t_Error			error;
	void				*context;

	// We are in a list
	HALFile			*prev, *next;
};

// The media hardware abstraction layer
class MediaHAL
{
public:
	// Set the app title
	static void SetTitle(const char *title);

	// Initialize
	static void Init(int maxOpenFiles);
	static void Terminate();
	static void Daemon();

	static bool	Exists(const char *fileName);
	static bool Delete(const char *fileName);
	static bool TimeStamp(const char *fileName, ts_TimeStamp *pTimeStamp);
	static int CompareTimeStamps(ts_TimeStamp *pTimeStamp1, ts_TimeStamp *pTimeStamp2);
	static bool	Open(const char *fileName, t_FileHandle *fileHandle, u32 openFlags);
	static u32	GetOpenFlags(t_FileHandle fileHandle);
	static void	Close(t_FileHandle fileHandle);
	static bool	Seek(t_FileHandle fileHandle, s32 pos);
	static s32	Length(t_FileHandle fileHandle);
	static t_Error	TranslateError(t_FileError platformError, t_Error defaultError);
	static const char *GetErrorText(t_Error error);
	static bool	Read(void *buffer, s32 len, t_FileHandle fileHandle);
	static bool	Write(const void *buffer, s32 len, t_FileHandle fileHandle);
	static bool	AwaitIO(t_FileHandle fileHandle, s32 timeOut, void **buffer, s32 *ioSize, t_Error *error);
	static void	SetRootDir(const char *newRootDir);
	static void	MakeFileName(const char *baseFileName, char *realFileName, bool platformDir);
	static char	*FileName83(const char *fileName);

	static bool SetContext(t_FileHandle fileHandle, void *context);
	static void *GetContext(t_FileHandle fileHandle);

	// you better know what the hell you are doing here. Do not
	// pass in INVALID_HANDLE and expect a consistent result (although
	// I would guess -1 would come back...but who knows, the gamecube
	// probably uses negative file numbers just for the fuck of it.)
	static long PlatformFileHandle(t_FileHandle fileHandle);

	#if defined (_XBOX)
	static void MakeUtilityFileName(const char *baseFileName, char *realFileName, bool platformDir);
	#endif

	// used for the PS2 to change the driver we are talking to... ie, the host0: part of the file name
	static void SetDriverName(const char *driverstring)
		{ strcpy(driverName, driverstring); }
	static const char *GetDriverName()
		{ return driverName; }

	// Set and Get the state of name-mangling of file names
	static void	EnableNameMangling(bool state)
		{ nameMangling = state; }
	static bool	GetNameManglingState(void)
		{ return(nameMangling); }

	// Are there any pending IO's?
	static int PendingIO();

	// Install an IO blocker
	typedef bool (*BlockIO)(void *context);
	static void InstallIOBlocker(BlockIO blockFN, void *blockContext);
	static bool IOBlocked();
	static inline void WaitForReadyIO()
		{ while(IOBlocked()); }

	// The last error (this is public)
	static t_Error 	lastError;
	static void ShouldCheckTray( bool trueOrFalse );

private:
	static bool			checkDiskTray;
	static bool 		nameMangling;
	static char 		rootDir[256];
	static char			appTitle[64];

	// used to hold the host0: or cdrom0: or atfile:xxx.xxx.xxx.xxx, for the PS2...
	static char			driverName[32];

	// The list of open files
	static DoublyLinkedList<HALFile>	openFiles;

	// Stuff for IO block
	static BlockIO							blockIO;
	static void								*blockIOContext;

	// Create/destroy the HALFile record
	static HALFile *CreateHALFile();
	static void DestroyHALFile(HALFile *file);

	// Platform specific allocate for HALFile subclass
	static HALFile *NewHALFile();

	// Manipulate filenames
	static void MakePlatformFileName(const char *baseFileName, char *realFileName, const char *dataDir, const char *rootDir);
	#if defined (_XBOX)
	static void MakePlatformCacheFileName(const char *baseFileName, char *realFileName, const char *dataDir, const char *rootDir);
	#endif

	static char	*Fix83(char *pName);
	static void RemoveSpaces(char *str);

	// Completion check
	static bool	CompletionCheck(HALFile *halFile, s32 *transferSize);
	static void CompleteAllPending(t_FileHandle ignore = INVALID_HANDLE);
	static bool CheckTray();
};

#endif
