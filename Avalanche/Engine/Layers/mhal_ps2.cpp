// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
#include "Layers/mstream/mstream.h"
#include "Layers/awmfile.h"

// Define our platform root
#define PLATFORM_DATA_ROOT	PS2_DATA_ROOT

// Platform info for an open file
class HALFilePS2 : public HALFile
{
public:
	int				handle;
	PS2FileStream	*stream;
};

// ------------------------------------------------------------------------
// Initialize
// ------------------------------------------------------------------------
void MediaHAL::Init(int maxOpenFiles)
{
	MultiStream::InitializeFileSystem(maxOpenFiles);
}

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
void MediaHAL::Terminate()
{
	MultiStream::TerminateFileSystem();
}

// ------------------------------------------------------------------------
// Called once per frame
// ------------------------------------------------------------------------
void MediaHAL::Daemon()
{
	MultiStream::Daemon();
}

// ------------------------------------------------------------------------
// Return our specific HALFile
// ------------------------------------------------------------------------
HALFile *MediaHAL::NewHALFile()
{
	return new HALFilePS2;
}

// ------------------------------------------------------------------------
// Does this file exist
// ------------------------------------------------------------------------
bool MediaHAL::Exists(const char *fileName)
{
	if (AtWinMonFile::Connected())
		return AtWinMonFile::Exists(fileName);
	else
		return MultiStream::FileExists(fileName);
}

// ------------------------------------------------------------------------
// delete a file
// ------------------------------------------------------------------------
bool MediaHAL::Delete(const char *fileName)
{
#ifdef CDROM
	// no support for cdrom build
	return false;
#else
	// Make sure we are allowed to use the IO device
	MediaHAL::CompleteAllPending();
	MediaHAL::WaitForReadyIO();
	return (sceRemove(fileName) == 0);
#endif
}

// ------------------------------------------------------------------------
// what's the time and date stamp?
// ------------------------------------------------------------------------
bool MediaHAL::TimeStamp(const char *fileName, ts_TimeStamp *pTimeStamp)
{
	if (AtWinMonFile::Connected())
		return AtWinMonFile::TimeStamp(fileName, pTimeStamp);
	else // Sorry no support for this yet
		return false;
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

	// Only have one pending IO at a time on the ps2
	MediaHAL::CompleteAllPending();

	HALFilePS2 *halFile = (HALFilePS2 *)CreateHALFile();
	if (halFile == NULL)
	{
		lastError = MED_ERR_OPEN;
		return false;
	}

	// Fill it out
	halFile->filePos = 0;
	halFile->ioPending = false;
	halFile->ioType = MED_IO_TYPE_NONE;
	halFile->openFlags = openFlags;
	halFile->handle = -1;
	halFile->stream = NULL;
	halFile->fileSize = 0;

	bool async = (openFlags & MEDOPEN_FLAG_NOWAIT) != 0;
	bool result = true;

	// First try atwinmon
	if (AtWinMonFile::IsRemoteFile(fileName))
	{
		halFile->handle = AtWinMonFile::Open(fileName, openFlags, &halFile->fileSize);
		result = (halFile->handle >= 0);
	}
	else if ((openFlags & MEDOPEN_FLAG_WRITE) == 0)
	{
		// Next, if it isn't a writeable....try multistream
		halFile->stream = MultiStream::OpenFile(fileName, async);
		if (!halFile->stream)
			result = false;
		else
			halFile->fileSize = halFile->stream->Length();
	}
	else
	{
		// Writeable file!
		int openFlags = SCE_WRONLY | SCE_CREAT | SCE_TRUNC;
		if (async)
			openFlags |= SCE_NOWAIT;

		// Open it
		halFile->fileSize = 0;
		halFile->handle = sceOpen((char *)fileName, openFlags);
		if (halFile->handle < 0)
			result = false;
	}

	if (!result)
	{
		DestroyHALFile(halFile);
		lastError = MED_ERR_OPEN;
		return false;
	}

	*fileHandle = (t_FileHandle)halFile;
	Media::LogFileOpen(fileName);

	return true;
}

// ------------------------------------------------------------------------
// close an open file
// ------------------------------------------------------------------------
void MediaHAL::Close(t_FileHandle fileHandle)
{
	ASSERT(fileHandle != NULL);
	HALFilePS2 *halFile = (HALFilePS2 *)fileHandle;

	// Only have one pending IO at a time on the ps2
	MediaHAL::CompleteAllPending(fileHandle);

	lastError = MED_ERR_NONE;

	if (AtWinMonFile::Connected())
		AtWinMonFile::Close(halFile->handle);
	else if (halFile->stream)
		MultiStream::CloseFile(halFile->stream);
	else
		sceClose(halFile->handle);

	// Clean it up
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
	HALFilePS2 *halFile = (HALFilePS2 *)fileHandle;

	// Only have one pending IO at a time on the ps2
	MediaHAL::CompleteAllPending();

	// is there io pending?
	if (halFile->ioPending)
	{
		halFile->error = lastError = MED_ERR_IO_PENDING;
		return false;
	}

	// Make sure we don't go past the end
	if (pos > halFile->fileSize)
	{
		halFile->ioType = MED_IO_TYPE_NONE;
		halFile->error = lastError = MED_ERR_SEEK;
		return false;
	}

	// Setup our io operation
	halFile->ioType = MED_IO_TYPE_SEEK;
	halFile->ioPending = false;
	lastError = halFile->error = MED_ERR_NONE;

	// Are we in a stream?
	if (halFile->stream)
	{
		// This is NOT async...it does nothing
		bool ret = halFile->stream->Seek(pos);
		if (!ret)
			halFile->error = lastError = MED_ERR_SEEK;
		else
		{
			halFile->filePos = pos;
			if (halFile->openFlags & MEDOPEN_FLAG_NOWAIT)
				halFile->ioPending = true;
			else
				halFile->ioType = MED_IO_TYPE_NONE;
		}

		return ret;
	}

	// Don't do work we don't have to
	if (pos == halFile->filePos)
	{
		if (!(halFile->openFlags & MEDOPEN_FLAG_NOWAIT))
			halFile->ioType = MED_IO_TYPE_NONE;
		return true;
	}

	// sneak in atwinmon support
	if (AtWinMonFile::Connected())
	{
		if (!AtWinMonFile::Seek(halFile->handle, pos))
			return false;
		else
		{
			halFile->filePos = pos;
			return true;
		}
	}

	// Do it for real
	int error = sceLseek(halFile->handle, pos, SCE_SEEK_SET);
	if (error < 0)
	{
		halFile->error = lastError = TranslateError(error, MED_ERR_SEEK);
		return false;
	}

	halFile->filePos = pos;
	if (halFile->openFlags & MEDOPEN_FLAG_NOWAIT)
		return true;

  	return(AwaitIO(fileHandle, -1, NULL, NULL, NULL));
}

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
s32 MediaHAL::Length(t_FileHandle fileHandle)
{
	ASSERT(fileHandle != NULL);
	HALFilePS2 *halFile = (HALFilePS2 *)fileHandle;

	halFile->ioPending = false;
	halFile->error = lastError = MED_ERR_NONE;
	halFile->ioType = MED_IO_TYPE_NONE;

	return halFile->fileSize;
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
	HALFilePS2 *halFile = (HALFilePS2 *)fileHandle;

	// Only have one pending IO at a time on the ps2
	MediaHAL::CompleteAllPending();

	if (halFile->ioPending)
	{
		halFile->error = lastError = MED_ERR_IO_PENDING;
		return false;
	}

	halFile->ioPending = false;
	halFile->ioType = MED_IO_TYPE_READ;
	lastError = halFile->error = MED_ERR_NONE;
	halFile->ioBuffer = buffer;
	halFile->ioSize = len;

	int error = MED_ERR_NONE;

	// Do it!
	if (halFile->stream)
	{
		bool ret = halFile->stream->Read(buffer, len);
		if (!ret)
			error = MED_ERR_READ;
		else if (halFile->stream->PendingIO())
		{
			halFile->ioPending = true;
			return ret;
		}
	}
	else
	{
		// AtWinMon support (doesn't support async)
		if (AtWinMonFile::Connected())
			return AtWinMonFile::Read(buffer, len, halFile->handle);

		// We should never really be down here...
		MediaHAL::WaitForReadyIO();

		error = sceRead(halFile->handle, buffer, len);

		if (error >= 0 && (halFile->openFlags & MEDOPEN_FLAG_NOWAIT))
		{
			halFile->ioPending = true;
			return true;
		}
	}

	// Handle errors
	if (error < 0)
	{
		halFile->ioType = MED_IO_TYPE_NONE;
		halFile->error = lastError = TranslateError(error, MED_ERR_READ);
		return false;
	}

	// Do the await
	return AwaitIO(fileHandle, -1, NULL, NULL, NULL);
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

#ifdef CDROM
	return false;
#else
	HALFilePS2 *halFile = (HALFilePS2 *)fileHandle;

	// Can't write to a stream!
	ASSERT(halFile->stream == NULL);
	if (halFile->stream)
		return false;

	// Only have one pending IO at a time on the ps2
	MediaHAL::CompleteAllPending(fileHandle);

	if (halFile->ioPending)
	{
		halFile->error = lastError = MED_ERR_IO_PENDING;
		return false;
	}

	lastError = MED_ERR_NONE;

	halFile->ioPending = false;
	halFile->ioType = MED_IO_TYPE_WRITE;
	halFile->error = MED_ERR_NONE;

	halFile->ioBuffer = (void *)buffer;
	halFile->ioSize = len;

	// AtWinMon support (doesn't support async)
	if (AtWinMonFile::Connected())
		return AtWinMonFile::Write(buffer, len, halFile->handle);

	// Do the real thing
	int error = sceWrite(halFile->handle, buffer, len);
	if (error < 0)
	{
		halFile->ioType = MED_IO_TYPE_NONE;
		halFile->error = lastError = TranslateError(error, MED_ERR_WRITE);
		return false;
	}

	/* update file position and size (this assumes we'll succeed) */

	halFile->filePos += len;
	if (halFile->filePos > halFile->fileSize)
		halFile->fileSize = halFile->filePos;

	/* done */

	if (halFile->openFlags & MEDOPEN_FLAG_NOWAIT)
		return true;
	else
		halFile->ioPending = true;

	return(AwaitIO(fileHandle, -1, NULL, NULL, NULL));
#endif
}

// ------------------------------------------------------------------------
// Make a PS2 file name
// ------------------------------------------------------------------------
void MediaHAL::MakePlatformFileName(
const char *baseFileName,
char *realFileName,
const char *dataDir,
const char *rootDir)
{
	strcpy(realFileName, driverName);
	strcat(realFileName, rootDir);
	strcat(realFileName, dataDir);

#ifdef CDROM
	strcat(realFileName, strupr(FileName83(baseFileName)));//PS2 CDROM supports only 8.3
	strcat(realFileName, ";1");
#else
	strcat(realFileName, baseFileName);
#endif
}

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
t_Error MediaHAL::TranslateError(
t_FileError platformError,
t_Error defaultError)
{
	t_Error error = defaultError;

#ifdef CDROM
	switch(platformError)
	{
		case SCECdErNO:
		case SCECdErNORDY:
			error = MED_ERR_NONE;
			break;
		case SCECdErFAIL: // failure to get error!
		case SCECdErEOM: // outermost track reached during playback!
		case SCECdErABRT: // abort command recieved!
			// various errors that the default is probably best for.
			break;
		case SCECdErTRMOPN:
			error = MED_ERR_COVEROPEN;
			break;
		case SCECdErREADCF: // read command failed to issue!
		case SCECdErREAD:
			error = MED_ERR_READ;
			break;
		case SCECdErCUD:
			error = MED_ERR_WRONGDISK;
			break;
	}
#else
	switch(platformError)
	{
		case SCE_ENXIO:  // No such device or address
		case SCE_EBADF:  // Bad file number
		case SCE_ENODEV: // No such device
		case SCE_EINVAL: // Invalid argument
			// these cases are all bad argument cases and should never happen in a release game
			ASSERT(false);
			break;

		case SCE_EMFILE: // Too many open files
			error = MED_ERR_MAXOPEN;
			break;

		default:
			// ok.. so now we need to query the system to find out why it failed... there is no sce*IO call do that.
			// we need to fall back on the sceCd* calls... which are what we should be using anyway...
			break;
	}
#endif

	return(error);
}

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
const char *MediaHAL::GetErrorText(t_Error error)
{
	static char text[256];

	switch (error)
	{
		// 'disc tray' is the TRC compliant term.
		case MED_ERR_COVEROPEN:

			sprintf( text, "The disc tray is open. If you want to continue %s, please close the disc tray.", appTitle );
			break;

		case MED_ERR_NODISK:

			sprintf( text, "Please insert the %s game disc.", appTitle );
			break;

		case MED_ERR_WRONGDISK:
			sprintf(text, "Please insert the %s game disc and close the disc tray.", appTitle);
			break;

		// all disc errors should retry indefinitely..
		case MED_ERR_RETRY:
		default:
			sprintf(text, "A disc error has occurred. Retrying.  Please wait.");
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
	HALFilePS2 *file = (HALFilePS2 *)halFile;

	t_Error defaultError = MED_ERR_NONE;
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

	int isPending;
	int result;
	if (file->stream)
		isPending = !file->stream->CompletionCheck(transferSize, &result);
	else
		result = sceIoctl(file->handle, SCE_FS_EXECUTING, &isPending);

	if (result < 0)
	{
		file->ioPending = false;
		file->error = lastError = TranslateError(result, defaultError);
		return true;
	}

	file->ioPending = (isPending) ? true : false;

	return(!file->ioPending);
}

// ------------------------------------------------------------------------
// you better know what the hell you are doing here. Do not
// pass in INVALID_HANDLE and expect a consistent result (although
// I would guess -1 would come back...but who knows, the gamecube
// probably uses negative file numbers just for the fuck of it.)
// ------------------------------------------------------------------------
long MediaHAL::PlatformFileHandle(t_FileHandle fileHandle)
{
	HALFilePS2 *halFile = (HALFilePS2 *)fileHandle;
	if (halFile)
		return (long)halFile->handle;
	else
		return -1;
}

// ----------------------------------------------------------------------------
bool MediaHAL::CheckTray()
{
#ifdef CDROM
	sceCdSync(0x10);						// Wait for completion of any other CD commands..BLOCKING
	int temp = sceCdGetDiskType();
	if( temp == SCECdNODISC )			// No-Disc, Tray opened
	{
		return( false );
	}
#endif
	return( true );
}