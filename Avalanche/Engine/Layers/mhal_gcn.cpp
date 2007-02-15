// ------------------------------------------------------------------------
// ------------------------------------------------------------------------

#ifdef DVDETH
#include <dolphin/dvdeth.h>
#endif

// Define our platform root
#define PLATFORM_DATA_ROOT	GCN_DATA_ROOT

// Platform info for an open file
class HALFileGCN : public HALFile
{
public:
	DVDFileInfo		dvdfi;
	s32				firstLen;
	s32				remainder;
};

// Callback for the dvd
static void gcnDVDReadCallback(s32 result, DVDFileInfo *pdvdfi);
static void gcnDVDWriteCallback(s32 result, DVDFileInfo *pdvdfi);

unsigned char acDVDExtra[64] ATTRIBUTE_ALIGN(32);

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
	return new HALFileGCN;
}

// ------------------------------------------------------------------------
// Does this file exist
// ------------------------------------------------------------------------
bool MediaHAL::Exists(const char *fileName)
{
#ifdef DVDETH
	DVDFileInfo		dvdfi;

	int result = DVDOpen( fileName, &dvdfi);
	if( result)
	{
		DVDClose( &dvdfi);
	}

	return( ( result) ? true : false);
#else
	return(DVDConvertPathToEntrynum((char *)fileName) != -1);
#endif
}

// ------------------------------------------------------------------------
// delete a file
// ------------------------------------------------------------------------
bool MediaHAL::Delete(const char *fileName)
{
#ifdef CDROM
	return(false);	// no support for cdrom build
#endif //CDROM
#ifndef DVDETH
	return( false);	// not implemented
#else
	return( false);	// not implemented
	return( DVDRemove( fileName, NULL) ? true : false);
#endif
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

	/* not supported yet */

	return(false);
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
	HALFileGCN *halFile = (HALFileGCN *)CreateHALFile();

	if (fileHandle == NULL)
	{
		lastError = MED_ERR_OPEN;
		return(false);
	}

	// Fill it out
	halFile->filePos = 0;
	halFile->ioPending = false;
	halFile->ioType = MED_IO_TYPE_NONE;
	halFile->openFlags = openFlags;

	bool result;
#ifdef DVDETH
	if( halFile->openFlags & MEDOPEN_FLAG_WRITE)
	{
		result = DVDCreate( fileName, &halFile->dvdfi);
	}
	else
	{
		result = DVDOpen( fileName, &halFile->dvdfi);
	}
#else
	result = DVDOpen( fileName, &halFile->dvdfi);
#endif
	if( result)
	{
		halFile->fileSize = DVDGetLength( &halFile->dvdfi);
		DVDSetUserData( (DVDCommandBlock *)&halFile->dvdfi, halFile);
	}

	if( !result)
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
	HALFileGCN *halFile = (HALFileGCN *)fileHandle;

	lastError = MED_ERR_NONE;

	DVDClose(&halFile->dvdfi);

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
	HALFileGCN *halFile = (HALFileGCN *)fileHandle;

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

	// for gamecube, position must be multiple of 4
	if (pos & 0x03)
	{
		lastError = MED_ERR_SEEK;
		halFile->ioType = MED_IO_TYPE_NONE;
		return(false);
	}

	halFile->filePos = pos;

	if (halFile->openFlags & MEDOPEN_FLAG_NOWAIT)
		return(true);
	else
		return(AwaitIO(fileHandle, -1, NULL, NULL, NULL));

	return(true);
}

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
s32 MediaHAL::Length(t_FileHandle fileHandle)
{
	ASSERT(fileHandle != NULL);
	HALFileGCN *halFile = (HALFileGCN *)fileHandle;

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
	ASSERT(fileHandle != NULL);
	CHECK_ALIGN_32B( buffer);
	HALFileGCN *halFile = (HALFileGCN *)fileHandle;

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

	halFile->firstLen = OSRoundDown32B(len);
	halFile->remainder = len - halFile->firstLen;

	//see if we are reading less than 32 bytes
	if (halFile->firstLen == 0)
	{
		halFile->remainder = 0;
		if (!DVDReadAsync(&halFile->dvdfi, acDVDExtra, 32, halFile->filePos, gcnDVDReadCallback))
		{
			halFile->ioType = MED_IO_TYPE_NONE;
			halFile->ioPending = false;
			halFile->error = lastError = TranslateError(DVDGetFileInfoStatus(&halFile->dvdfi), MED_ERR_READ);
		}
	}
	else if (!DVDReadAsync(&halFile->dvdfi, buffer, halFile->firstLen, halFile->filePos, gcnDVDReadCallback))
	{
		halFile->ioType = MED_IO_TYPE_NONE;
		halFile->ioPending = false;
		halFile->error = lastError = TranslateError(DVDGetFileInfoStatus(&halFile->dvdfi), MED_ERR_READ);
	}

	if (halFile->openFlags & MEDOPEN_FLAG_NOWAIT)
		return(true);
	else
		return(AwaitIO(fileHandle, -1, NULL, NULL, NULL));
}

// ------------------------------------------------------------------------
// write to an already-open file
// ------------------------------------------------------------------------
bool MediaHAL::Write(
const void *buffer,
s32 len,
t_FileHandle fileHandle)
{
	ASSERT(fileHandle != NULL);
	HALFileGCN *halFile = (HALFileGCN *)fileHandle;

	if( halFile->ioPending)
	{
		halFile->error = lastError = MED_ERR_IO_PENDING;
		return(false);
	}

	halFile->ioBuffer = (void *)buffer;
	halFile->ioSize = len;

#ifndef DVDETH
	halFile->ioPending = false;
	halFile->ioType = MED_IO_TYPE_NONE;
	halFile->error = lastError = MED_ERR_WRITE;

	return( false);
#else
	halFile->ioPending = true;
	halFile->ioType = MED_IO_TYPE_WRITE;
	halFile->error = lastError = MED_ERR_NONE;

	if( !DVDWriteAsync( &halFile->dvdfi, halFile->ioBuffer, len, halFile->filePos, gcnDVDWriteCallback))
	{
		halFile->ioType = MED_IO_TYPE_NONE;
		halFile->ioPending = false;
		halFile->error = lastError = TranslateError( DVDGetFileInfoStatus( &halFile->dvdfi), MED_ERR_WRITE);
	}

	if( halFile->openFlags & MEDOPEN_FLAG_NOWAIT)
		return( true);
	else
		return(AwaitIO(fileHandle, -1, NULL, NULL, NULL));
#endif
}

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
static void gcnDVDReadCallback(
s32 result,
DVDFileInfo *pdvdfi)
{
	HALFileGCN *halFile = (HALFileGCN *)DVDGetUserData((DVDCommandBlock *)pdvdfi);

	if (result < 0)
	{
		halFile->ioPending = false;
		if (result != DVD_RESULT_CANCELED)
			halFile->error = MediaHAL::lastError = MediaHAL::TranslateError(result, MED_ERR_READ);
		return;
	}

	if (halFile->remainder != 0)
	{
		DVDReadAsync(pdvdfi, acDVDExtra, 32, halFile->filePos + halFile->firstLen, gcnDVDReadCallback);

		halFile->remainder = 0;
		return;
	}
	else if (halFile->firstLen != halFile->ioSize)
	{
		s32	remainder = halFile->ioSize - halFile->firstLen;
		char	*buffer = (char *)halFile->ioBuffer;

		buffer += halFile->firstLen;
		memcpy(buffer, acDVDExtra, remainder);
		DCStoreRange(buffer, remainder);
	}

	halFile->ioPending = false;
	halFile->error = MediaHAL::lastError = MED_ERR_NONE;
}

#ifdef DVDETH
static void gcnDVDWriteCallback(
s32 result,
DVDFileInfo *pdvdfi)
{
	HALFileGCN *halFile = (HALFileGCN *)DVDGetUserData((DVDCommandBlock *)pdvdfi);

	halFile->ioPending = false;
	if( result < 0)
	{
		if( result != DVD_RESULT_CANCELED)
			halFile->error = MediaHAL::lastError = MediaHAL::TranslateError(result, MED_ERR_WRITE);
	}
	else
	{
		halFile->error = MediaHAL::lastError = MED_ERR_NONE;
	}
}
#endif

// ------------------------------------------------------------------------
// Make a GCN file name
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

	char *p = realFileName;
	while (*p)
	{
		if (*p == '\\')
			*p = '/';
		if (*p == '//')
			*p = '/';
		p++;
	}
}

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
t_Error MediaHAL::TranslateError(
t_FileError platformError,
t_Error defaultError)
{
	t_Error error = defaultError;

	switch(platformError)
	{
		case DVD_STATE_FATAL_ERROR:
			error = MED_ERR_FATAL;
			break;

		case DVD_STATE_END:
		case DVD_STATE_BUSY:
		case DVD_STATE_WAITING:
		case DVD_STATE_CANCELED:
		case DVD_STATE_IGNORED:
		case DVD_STATE_MOTOR_STOPPED:
			error = MED_ERR_NONE;
			break;

		case DVD_STATE_NO_DISK:
			error = MED_ERR_NODISK;
			break;

		case DVD_STATE_COVER_OPEN:
			error = MED_ERR_COVEROPEN;
			break;

		case DVD_STATE_WRONG_DISK:
			error = MED_ERR_WRONGDISK;
			break;

		case DVD_STATE_RETRY:
			error = MED_ERR_RETRY;
			break;

		default:
			error = defaultError;
			break;
	}

	return(error);
}

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------

const char *MediaHAL::GetErrorText(t_Error error)
{
	static char text[256];

	switch (error)
	{
		case MED_ERR_COVEROPEN:
			sprintf(text, "The Disc Cover is open. If you want to continue the game, please close the Disc Cover.");
			break;

		case MED_ERR_NODISK:
		case MED_ERR_WRONGDISK:
			sprintf(text, "Please insert the %s Game Disc.", appTitle);
			break;

		case MED_ERR_RETRY:
			sprintf(text, "The Game Disc could not be read. Please read the Nintendo GameCube Instruction Booklet for more information.");
			break;

		case MED_ERR_FATAL:
		default:
			sprintf(text, "An error has occurred. Turn the power off and refer to the Nintendo GameCube Instruction Booklet for further instructions.");
			break;
	}

	return(text);
}

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
bool MediaHAL::CompletionCheck(
HALFile *halFile,
s32 *transferSize)
{
	HALFileGCN *file = (HALFileGCN *)halFile;

	t_Error defaultError;
	switch( file->ioType)
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

	if( !file->ioPending)
	{
		file->filePos += file->ioSize;
		return( true);
	}

	s32 result = DVDGetDriveStatus( );
	if( result == DVD_STATE_BUSY || result == DVD_STATE_WAITING)
	{
		if( transferSize != NULL)
		{
			*transferSize = DVDGetTransferredSize( &file->dvdfi);
		}
	}

	file->error = TranslateError( result, defaultError);

	return( false);
}

// ------------------------------------------------------------------------
// you better know what the hell you are doing here. Do not
// pass in INVALID_HANDLE and expect a consistent result (although
// I would guess -1 would come back...but who knows, the gamecube
// probably uses negative file numbers just for the fuck of it.)
// ------------------------------------------------------------------------
long MediaHAL::PlatformFileHandle(t_FileHandle fileHandle)
{
	HALFileGCN *halFile = (HALFileGCN *)fileHandle;
	if (halFile)
		return (long)&halFile->dvdfi;
	else
		return -1;
}

// ----------------------------------------------------------------------------
bool MediaHAL::CheckTray()
{
	return( true );
}