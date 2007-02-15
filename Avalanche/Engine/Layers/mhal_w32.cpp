#include <sys/types.h>
#include <sys/stat.h>

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------

// Define our platform root
#define PLATFORM_DATA_ROOT	PC_DATA_ROOT

// Platform info for an open file
class HALFileWin32 : public HALFile
{
public:
	HANDLE			handle;
	OVERLAPPED		overlapped;
};

// ------------------------------------------------------------------------
// Initialize
// ------------------------------------------------------------------------
void MediaHAL::Init(int maxOpenFiles)
{
}

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
void MediaHAL::Terminate()
{
}

// ------------------------------------------------------------------------
// Called once per frame
// ------------------------------------------------------------------------
void MediaHAL::Daemon()
{
}

// ------------------------------------------------------------------------
// Return our specific HALFile
// ------------------------------------------------------------------------
HALFile *MediaHAL::NewHALFile()
{
	return new HALFileWin32;
}

// ------------------------------------------------------------------------
// Does this file exist
// ------------------------------------------------------------------------
bool MediaHAL::Exists(const char *fileName)
{
	FILE *file = fopen(fileName, "rb");
	if (file == NULL)
		return(false);
	else
	{
		fclose(file);
		return(true);
	}
}

// ------------------------------------------------------------------------
// delete a file
// ------------------------------------------------------------------------
bool MediaHAL::Delete(const char *fileName)
{
#ifdef CDROM
	return(false);	// no support for cdrom build
#endif //CDROM
	return(remove(fileName) == 0);
}

// ------------------------------------------------------------------------
// what's the time and date stamp?
// ------------------------------------------------------------------------
bool MediaHAL::TimeStamp(const char *fileName, ts_TimeStamp *pTimeStamp)
{
#ifdef CDROM
	/* no timestamp support for cdrom build */

	return(false);
#endif //CDROM

	FILE *file = fopen(fileName, "rb");
	if (file == NULL)
		return(false);
	else
	{
		fclose(file);

		/* get file info */

		struct _stat FileStats;
	   int nResult = _stat(fileName, &FileStats);
		if (nResult != 0)
			return(false);

		/* get timestamp */

		struct tm *pEvaluatedTime;
	   pEvaluatedTime = gmtime(&FileStats.st_mtime);

		/* fill in return values */

		pTimeStamp->nYear = pEvaluatedTime->tm_year - 1900;	// windows return value is from 1900
		pTimeStamp->nMonth = pEvaluatedTime->tm_mon;
		pTimeStamp->nDay = pEvaluatedTime->tm_mday - 1;			// windows return value is 1-based, make it 0-based
		pTimeStamp->nHour = pEvaluatedTime->tm_hour;
		pTimeStamp->nMinute = pEvaluatedTime->tm_min;
		pTimeStamp->nSecond = pEvaluatedTime->tm_sec;

		/* success */

		return(true);
	}
}

// ------------------------------------------------------------------------
// open a file
// ------------------------------------------------------------------------
bool MediaHAL::Open(
const char *fileName,
t_FileHandle *fileHandle,
u32 openFlags)
{
	lastError = MED_ERR_NONE;
	*fileHandle = NULL;

	HALFileWin32 *halFile = (HALFileWin32 *)CreateHALFile();
	if (halFile == NULL)
	{
		lastError = MED_ERR_OPEN;
		return(false);
	}

	halFile->filePos = 0;
	halFile->ioPending = false;
	halFile->ioType = MED_IO_TYPE_NONE;
	halFile->openFlags = openFlags;

	if (openFlags & MEDOPEN_FLAG_WRITE)
		halFile->handle = CreateFile(fileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);
	else
		halFile->handle = CreateFile(fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);

	bool result;
	if (result = (halFile->handle != INVALID_HANDLE_VALUE))
	{
		halFile->fileSize = GetFileSize(halFile->handle, NULL);
		halFile->overlapped.Offset = 0;
		halFile->overlapped.OffsetHigh = 0;
		halFile->overlapped.hEvent = CreateEvent(NULL, true, false, NULL);

		result = (halFile->overlapped.hEvent != INVALID_HANDLE_VALUE);

		if (!result)
			CloseHandle(halFile->handle);
	}

	if (!result)
	{
		DestroyHALFile(halFile);
		lastError = MED_ERR_OPEN;
		return(false);
	}

	*fileHandle = (t_FileHandle)halFile;
	Media::LogFileOpen(fileName);
	return(true);
}

// ------------------------------------------------------------------------
// close an open file
// ------------------------------------------------------------------------
void MediaHAL::Close(t_FileHandle fileHandle)
{
	ASSERT(fileHandle != NULL);
	HALFileWin32 *halFile = (HALFileWin32 *)fileHandle;

	lastError = MED_ERR_NONE;

	CloseHandle(halFile->handle);
	CloseHandle(halFile->overlapped.hEvent);

	DestroyHALFile(halFile);
}

// ------------------------------------------------------------------------
// seek in an already-open file, from beginning
// ------------------------------------------------------------------------
bool MediaHAL::Seek(
t_FileHandle fileHandle,
s32 pos)
{
	ASSERT(fileHandle != NULL);
	HALFileWin32 *halFile = (HALFileWin32 *)fileHandle;

	if (halFile->ioPending)
	{
		halFile->error = lastError = MED_ERR_IO_PENDING;
		return(false);
	}

	lastError = MED_ERR_NONE;

	halFile->ioPending = false;
	halFile->ioType = MED_IO_TYPE_SEEK;
	halFile->error = MED_ERR_NONE;

	// Don't do work we don't have to
	if (pos == halFile->filePos)
	{
		if (!(halFile->openFlags & MEDOPEN_FLAG_NOWAIT))
			halFile->ioType = MED_IO_TYPE_NONE;
		return(true);
	}

	if (pos > halFile->fileSize)
	{
		halFile->ioType = MED_IO_TYPE_NONE;
		halFile->error = lastError = MED_ERR_SEEK;
		return(false);
	}

	halFile->filePos = pos;
	halFile->overlapped.Offset = pos;
	halFile->overlapped.OffsetHigh = 0;

	if (halFile->openFlags & MEDOPEN_FLAG_NOWAIT)
		return(true);
	else
		return(AwaitIO(halFile, -1, NULL, NULL, NULL));

	return(true);
}

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
s32 MediaHAL::Length(t_FileHandle fileHandle)
{
	ASSERT(fileHandle != NULL);
	HALFileWin32 *halFile = (HALFileWin32 *)fileHandle;

	lastError = MED_ERR_NONE;

	return(halFile->fileSize);
}

// ------------------------------------------------------------------------
// read from an already-open file
// ------------------------------------------------------------------------
bool MediaHAL::Read(
void *buffer,
s32 len,
t_FileHandle fileHandle)
{
	HALFileWin32 *halFile = (HALFileWin32 *)fileHandle;

	if (halFile->ioPending)
	{
		halFile->error = lastError = MED_ERR_IO_PENDING;
		return(false);
	}

	lastError = MED_ERR_NONE;

	halFile->ioPending = true;
	halFile->ioType = MED_IO_TYPE_READ;
	halFile->error = MED_ERR_NONE;

	halFile->ioBuffer = buffer;
	halFile->ioSize = len;

	int result = ReadFile(halFile->handle, buffer, len, NULL, &halFile->overlapped);

	if (result == 0)
	{
		t_FileError error = GetLastError();

		if (error != ERROR_IO_PENDING)
		{
			halFile->ioType = MED_IO_TYPE_NONE;
			halFile->ioPending = false;
			halFile->error = lastError = TranslateError(error, MED_ERR_READ);
			#if defined (CDROM)
			DiscErrorHandler::DisplayError(NULL, DiscErrorHandler::READ_FILE_FAILED);		// error warning for failed load
			#endif
			return(false);
		}
	}
	else
	{
		halFile->ioPending = false;
		DWORD	ioSize;
		GetOverlappedResult(halFile->handle, &halFile->overlapped, &ioSize, true);

		halFile->ioSize = ioSize;
		halFile->filePos += ioSize;
		halFile->overlapped.Offset += ioSize;
	}

	if (halFile->openFlags & MEDOPEN_FLAG_NOWAIT)
		return(true);
	else
		return(AwaitIO(halFile, -1, NULL, NULL, NULL));
}

// ------------------------------------------------------------------------
// write to an already-open file
// ------------------------------------------------------------------------
bool MediaHAL::Write(
const void *buffer,
s32 len,
t_FileHandle fileHandle)
{
	HALFileWin32 *halFile = (HALFileWin32 *)fileHandle;

	if (halFile->ioPending)
	{
		halFile->error = lastError = MED_ERR_IO_PENDING;
		return(false);
	}

	lastError = MED_ERR_NONE;

	halFile->ioPending = true;
	halFile->ioType = MED_IO_TYPE_WRITE;
	halFile->error = MED_ERR_NONE;

	halFile->ioBuffer = (void *)buffer;
	halFile->ioSize = len;

	int result = WriteFile(halFile->handle, buffer, len, NULL, &halFile->overlapped);
	if (result == 0)
	{
		t_FileError error = GetLastError();

		if (error != ERROR_IO_PENDING)
		{
			halFile->ioType = MED_IO_TYPE_NONE;
			halFile->ioPending = false;
			halFile->error = lastError = TranslateError(error, MED_ERR_WRITE);
			return(false);
		}
	}
	else
	{
		halFile->ioPending = false;
		DWORD ioSize;
		GetOverlappedResult(halFile->handle, &halFile->overlapped, &ioSize, true);

		halFile->ioSize = ioSize;
		halFile->overlapped.Offset += ioSize;
	}

	/* update file position and size (this assumes we'll succeed) */

	halFile->filePos += len;
	if (halFile->filePos > halFile->fileSize)
		halFile->fileSize = halFile->filePos;

	/* done */

	if (halFile->openFlags & MEDOPEN_FLAG_NOWAIT)
		return(true);
	return(AwaitIO(halFile, -1, NULL, NULL, NULL));
}

// ------------------------------------------------------------------------
// PC Version
// ------------------------------------------------------------------------
void MediaHAL::MakePlatformFileName(
const char *baseFileName,
char *realFileName,
const char *dataDir,
const char *rootDir)
{
	strcpy(realFileName, rootDir);
	strcat(realFileName, dataDir);
	strcat(realFileName, baseFileName);
}

// ------------------------------------------------------------------------
// Translate the error
// ------------------------------------------------------------------------
t_Error MediaHAL::TranslateError(
t_FileError platformError,
t_Error defaultError)
{
	t_Error error = defaultError;
	return(error);
}

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
const char *MediaHAL::GetErrorText(t_Error error)
{
	// Nothing specific for Win32
	char *text = "A disk error has occurred.";
	return(text);
}

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
bool MediaHAL::CompletionCheck(
HALFile *halFile,
s32 *transferSize)
{
	HALFileWin32 *file = (HALFileWin32 *)halFile;

	t_Error defaultError;
	switch (file->ioType)
	{
		case MED_IO_TYPE_SEEK:
			defaultError = MED_ERR_SEEK;
			break;

		case MED_IO_TYPE_WRITE:
			defaultError = MED_ERR_WRITE;
			break;

		case MED_IO_TYPE_READ:
			defaultError = MED_ERR_READ;
			break;
	}

	if (transferSize != NULL)
		*transferSize = 0;

	DWORD size;
	BOOL result = GetOverlappedResult(file->handle, &file->overlapped, &size, FALSE);
	if (result)
	{
		file->ioPending = false;
		file->ioSize = size;
		file->filePos += size;
		file->overlapped.Offset += size;
		return(true);
	}

	DWORD error = GetLastError();
	if (error != ERROR_IO_INCOMPLETE)
	{
		file->ioPending = false;
		file->error = lastError = TranslateError(error, defaultError);
		return(true);
	}

	return(false);
}

// ------------------------------------------------------------------------
// you better know what the hell you are doing here. Do not
// pass in INVALID_HANDLE and expect a consistent result (although
// I would guess -1 would come back...but who knows, the gamecube
// probably uses negative file numbers just for the fuck of it.)
// ------------------------------------------------------------------------
long MediaHAL::PlatformFileHandle(t_FileHandle fileHandle)
{
	HALFileWin32 *halFile = (HALFileWin32 *)fileHandle;
	if (halFile)
		return (long)halFile->handle;
	else
		return -1;
}

// ----------------------------------------------------------------------------
bool MediaHAL::CheckTray()
{
	return( true );
}