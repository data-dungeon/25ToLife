// ------------------------------------------------------------------------
// This is a sort of cheesy way of doing this, but I don't feel like setting
// up all kinds of directories for this stuff.  Someday we will have a
// better way of doing all of this instead of #ifdef's
#include "Layers/LayersPCH.h"

#include "EngineHelper/timer.h"
#include "Layers/DiscErrorHandler/DiscError.h"

// The error
t_Error MediaHAL::lastError = 0;

bool MediaHAL::checkDiskTray = false;

// the state of name mangling enable
bool MediaHAL::nameMangling = true;

// root dir of the game - is prepended to all filenames generated
char MediaHAL::rootDir[256] = "\0";

// The name of this title for error messages
char MediaHAL::appTitle[64] = "App Title";

// The name of the driver to use (for the PS2) cdrom0: host0: etc..
char MediaHAL::driverName[32] = "host0:";

// The list of open files
DoublyLinkedList<HALFile> MediaHAL::openFiles(false);

// The IO blocking stuff
MediaHAL::BlockIO	MediaHAL::blockIO = NULL;
void *MediaHAL::blockIOContext = NULL;

// Hog needs to know when IO is complete
extern bool Hogi_AsyncReadComplete(void *ioBuffer, u32 ioSize, t_Error error, void *context);

// include the appropriate platform implementation
#if defined(WIN32) && !defined(_XBOX)
#include "Layers/mhal_w32.cpp"
#elif defined(_XBOX)
#include "Layers/mhal_xb.cpp"
#elif defined(PS2)
#include "Layers/mhal_ps2.cpp"
#elif defined(GCN)
#include "Layers/mhal_gcn.cpp"
#endif

// These functions have proven to be platform independent

// ------------------------------------------------------------------------
// Set the app title
// ------------------------------------------------------------------------
void MediaHAL::SetTitle(const char *title)
{
	ASSERT(strlen(title) < sizeof(appTitle));
	strcpy(appTitle, title);
}

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
void MediaHAL::SetRootDir(const char *newRootDir)
{
	ASSERT(strlen(newRootDir) < sizeof(rootDir));
	strcpy(rootDir, newRootDir);
	strupr(rootDir);
}

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
void MediaHAL::MakeFileName(const char *baseFileName, char *realFileName, bool platformDir)
{
	if (MediaHAL::GetNameManglingState() == false)
	{
		strcpy(realFileName, baseFileName);	//use filename as supplied
		return;
	}

	MakePlatformFileName(baseFileName, realFileName, platformDir ? PLATFORM_DATA_ROOT : COMMON_DATA_ROOT, rootDir);
}

#if defined (_XBOX)
void MediaHAL::MakeUtilityFileName(const char *baseFileName, char *realFileName, bool platformDir )
{
	MakePlatformCacheFileName(baseFileName, realFileName, platformDir ? PLATFORM_DATA_ROOT : COMMON_DATA_ROOT, rootDir);
}
#endif

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
char *MediaHAL::FileName83(const char *fileName)
{
	// convert filename to 8.3 compatible format

	// NOTE: this routine only converts directory and file names to 8.3 (ie, appends "~1").  It does NOT count non-unique names to
	// attempt unique 8.3 names (i.e. no ~2 or ~3 etc. names will be generated).

	static char fileName83[256];
	const char *namePtr;
	char *cPtr, *pFixName83;

	memset(fileName83, '\0', sizeof(fileName83));

	namePtr = fileName;//assume the filename has no path data
	pFixName83 = fileName83;

	while((cPtr = strchr(namePtr, '\\')) || (cPtr = strchr(namePtr, '/')) || (cPtr = strchr(namePtr, '.')))
	{
		strncpy(pFixName83, namePtr, cPtr - namePtr);
		pFixName83 = MediaHAL::Fix83(pFixName83);

		if (*cPtr == '.')
			*pFixName83++ = '.';
		else
			*pFixName83++ = '\\';

		*pFixName83 = '\0';

		namePtr = cPtr + 1;
	}

	//all directory names are fixed, finish off the file name
	strcat(pFixName83, namePtr);
	pFixName83 = Fix83(pFixName83);

	return(strupr(fileName83));
}

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
bool MediaHAL::SetContext(
t_FileHandle fileHandle,
void *context)
{
	ASSERT(fileHandle != NULL);
	if (!fileHandle)
		return false;

	HALFile *halFile = (HALFile *)fileHandle;
	halFile->context = context;
	return true;
}

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
void *MediaHAL::GetContext(t_FileHandle fileHandle)
{
	ASSERT(fileHandle != NULL);
	if (!fileHandle)
		return NULL;

	HALFile *halFile = (HALFile *)fileHandle;
	return halFile->context;
}

// ------------------------------------------------------------------------
// Are there any pending IO's?
// ------------------------------------------------------------------------
int MediaHAL::PendingIO()
{
#if 0
	return openFiles.Items();
#else
	int pending = 0;

	HALFile *file = openFiles.Head();
	while (file)
	{
		if (file->ioPending)
			++pending;

		file = file->next;
	}

	return pending;
#endif
}

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
void MediaHAL::InstallIOBlocker(
BlockIO blockFN,
void *blockContext)
{
	blockIO = blockFN;
	blockIOContext = blockContext;
}

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
bool MediaHAL::IOBlocked()
{
	if (blockIO)
		return blockIO(blockIOContext);
	else
		return false;
}

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
void MediaHAL::RemoveSpaces(char *str)
{
	char *space;
	while(space = strchr(str, ' '))
	{
		//copy over the space - can't use strcpy since strings overlap
		int len = strlen(space);
		for(int i = 0; i < len; i++)
			space[i] = space[i + 1];
	}
}

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
char *MediaHAL::Fix83(char *pName)
{
	uint nameLen = strlen(pName);

	if (nameLen <= 8)
	{
		RemoveSpaces(pName);
		return pName + nameLen;
	}
	else
	{
		RemoveSpaces(pName);
		pName[6] = '~';
		pName[7] = '1';
		pName[8] = '\0';
		return pName + 8;
	}
}

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
bool MediaHAL::AwaitIO(
t_FileHandle fileHandle,
s32 timeOut,
void **buffer,
s32 *ioSize,
t_Error *error)
{
	HALFile *halFile = (HALFile *)fileHandle;

	if (halFile->ioType == MED_IO_TYPE_NONE)
	{
		halFile->error = lastError = MED_ERR_NO_IO;
		if (error != NULL)
			*error = lastError;
		return(true);
	}

	if (!halFile->ioPending)
	{
		if (halFile->ioType == MED_IO_TYPE_READ)
		{
			bool retry = Hogi_AsyncReadComplete(halFile->ioBuffer, halFile->ioSize, halFile->error, halFile->context);
			if (retry)
			{
				halFile->ioType = MED_IO_TYPE_NONE;
				bool ret = Read(halFile->ioBuffer, halFile->ioSize, fileHandle);
				ASSERT(ret);
				return false;
			}
		}

		if (buffer != NULL)
			*buffer = halFile->ioBuffer;
		if (ioSize	!= NULL)
			*ioSize = halFile->ioSize;
		if (error)
			*error = halFile->error;

		halFile->ioType = MED_IO_TYPE_NONE;
		return(true);
	}

	if (timeOut == 0)
	{
		if (MediaHAL::CompletionCheck(halFile, ioSize))
			return(MediaHAL::AwaitIO(fileHandle, timeOut, buffer, ioSize, error));
		else
		{
			if (error)
				*error = halFile->error;

			return(false);
		}
	}
	else if (timeOut < 0)
	{
		while( !MediaHAL::CompletionCheck(halFile, NULL) )
		{
			if( checkDiskTray )
			{
				if( !MediaHAL::CheckTray() )
					return( false );
			}
		}
		return(MediaHAL::AwaitIO(fileHandle, timeOut, buffer, ioSize, error));
	}
	else
	{
		uint32  	startTime = g_timer.GetOSTime(),
					endTime;

		endTime = startTime + timeOut;

		while(!MediaHAL::CompletionCheck(halFile, NULL))
		{
			if (g_timer.GetOSTime() > endTime)
			{
				// Is anyone using this?
				ASSERT(false);
				break;
			}
		}

		return(MediaHAL::AwaitIO(fileHandle, 0, buffer, ioSize, error));
	}
}

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
void MediaHAL::CompleteAllPending(
t_FileHandle ignore)
{
	HALFile *ignoreFile = (HALFile *)ignore;

	HALFile *file = openFiles.Head();
	while (file)
	{
		if (file != ignoreFile && file->ioPending)
			AwaitIO((t_FileHandle)file, -1, NULL, NULL, NULL);
		file = file->next;
	}
}

// ------------------------------------------------------------------------
// compare two timestamps, return negative if first is before 2nd, 0
// if equal, positive if first is later than second
// ------------------------------------------------------------------------
int MediaHAL::CompareTimeStamps(ts_TimeStamp *pTimeStamp1, ts_TimeStamp *pTimeStamp2)
{
	if (pTimeStamp1->nYear != pTimeStamp2->nYear)
		return(pTimeStamp1->nYear - pTimeStamp2->nYear);
	if (pTimeStamp1->nMonth != pTimeStamp2->nMonth)
		return(pTimeStamp1->nMonth - pTimeStamp2->nMonth);
	if (pTimeStamp1->nDay != pTimeStamp2->nDay)
		return(pTimeStamp1->nDay - pTimeStamp2->nDay);
	if (pTimeStamp1->nHour != pTimeStamp2->nHour)
		return(pTimeStamp1->nHour - pTimeStamp2->nHour);
	if (pTimeStamp1->nMinute != pTimeStamp2->nMinute)
		return(pTimeStamp1->nMinute - pTimeStamp2->nMinute);
	return(pTimeStamp1->nSecond - pTimeStamp2->nSecond);
}

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
HALFile *MediaHAL::CreateHALFile()
{
	// Each platform writes this
	HALFile *file = NewHALFile();
	if (!file)
		return NULL;
	file->context = NULL;

	// Put it in the list
	openFiles.AddTail(file);

	return file;
}

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
void MediaHAL::DestroyHALFile(
HALFile *file)
{
	if (!file)
		return;

	openFiles.Unlink(file);
	delete file;
}

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
void MediaHAL::ShouldCheckTray( bool trueOrFalse )
{
	checkDiskTray = trueOrFalse;
}