// ------------------------------------------------------------------------
// ------------------------------------------------------------------------

// Define our platform root
#define PLATFORM_DATA_ROOT	PSP_DATA_ROOT

// Platform info for an open file
class HALFilePSP : public HALFile
{
public:
	SceUID				handle;
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
	return new HALFilePSP;
}

// ------------------------------------------------------------------------
// Does this file exist
// ------------------------------------------------------------------------
bool MediaHAL::Exists(const char *fileName)
{
	SceIoStat status;

	return sceIoGetstat(fileName, &status) == 0;
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
	return (sceIoRemove(fileName) == 0);
#endif
}

// ------------------------------------------------------------------------
// what's the time and date stamp?
// ------------------------------------------------------------------------
bool MediaHAL::TimeStamp(const char *fileName, ts_TimeStamp *pTimeStamp)
{
	// PSP TODO
	return false;
}

// ------------------------------------------------------------------------
// open a file
// ------------------------------------------------------------------------
bool MediaHAL::Open(const char *fileName, t_FileHandle *fileHandle, u32 openFlags)
{
	lastError = MED_ERR_NONE;
	*fileHandle = NULL;

	// PSP TEMP // PSP TODO - Implement async file IO
	openFlags &= ~MEDOPEN_FLAG_NOWAIT;

	// Only have one pending IO at a time on the ps2  // PSP TODO - Is this true on PSP?
	MediaHAL::CompleteAllPending();

	HALFilePSP *halFile = (HALFilePSP*)CreateHALFile(); 
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
	halFile->fileSize = 0;

	bool async = (openFlags & MEDOPEN_FLAG_NOWAIT) != 0;
	bool result = true;
	int sceOpenFlags;

	if ((openFlags & MEDOPEN_FLAG_WRITE))
	{
		sceOpenFlags = SCE_O_WRONLY | SCE_O_CREAT | SCE_O_TRUNC | SCE_O_RETRY_15;
	}
	else
	{
		sceOpenFlags = SCE_O_RDONLY | SCE_O_RETRY_15;
	}

	halFile->fileSize = 0;

	SceMode mode = SCE_STM_FREG | SCE_STM_RUSR | SCE_STM_WUSR | SCE_STM_RGRP | SCE_STM_WGRP | SCE_STM_ROTH;

	// Open it
	if (async)
	{
		ASSERTS(false, "PSP async file IO not ready"); // PSP TODO
		halFile->handle = sceIoOpenAsync((char *)fileName, sceOpenFlags, mode);
	}
	else
	{
		halFile->handle = sceIoOpen((char *)fileName, sceOpenFlags, mode);

		if(halFile->handle >= 0)
		{
			SceIoStat status;
			if(sceIoGetstat(fileName, &status) == 0)
			{
				halFile->fileSize = status.st_size;
			}
		}
	}
	if (halFile->handle < 0)
		result = false;

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
	HALFilePSP *halFile = (HALFilePSP*)fileHandle;

	// Only have one pending IO at a time on the ps2  // PSP TODO - Is this true on PSP?
	MediaHAL::CompleteAllPending(fileHandle);

	lastError = MED_ERR_NONE;

	sceIoClose(halFile->handle);

	// Clean it up
	DestroyHALFile(halFile);
}

// ------------------------------------------------------------------------
// seek in an already-open file, from beginning
// ------------------------------------------------------------------------
bool MediaHAL::Seek(t_FileHandle fileHandle, s32 pos)
{
	ASSERT(fileHandle != NULL);
	HALFilePSP *halFile = (HALFilePSP*)fileHandle;

	// Only have one pending IO at a time on the ps2  // PSP TODO - Is this true on PSP?
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

	// Don't do work we don't have to
	if (pos == halFile->filePos)
	{
		if (!(halFile->openFlags & MEDOPEN_FLAG_NOWAIT))
			halFile->ioType = MED_IO_TYPE_NONE;
		return true;
	}

	// Do it for real
	int error = sceIoLseek(halFile->handle, pos, SCE_SEEK_SET);
	if (error < 0)
	{
		halFile->error = lastError = TranslateError(error, MED_ERR_SEEK);
		return false;
	}

	halFile->filePos = pos;
	if (halFile->openFlags & MEDOPEN_FLAG_NOWAIT)
	{
		ASSERTS(false, "PSP async file IO not ready"); // PSP TODO
		return true;
	}

  	return(AwaitIO(fileHandle, -1, NULL, NULL, NULL));
}

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
s32 MediaHAL::Length(t_FileHandle fileHandle)
{
	ASSERT(fileHandle != NULL);
	HALFilePSP *halFile = (HALFilePSP*)fileHandle;

	halFile->ioPending = false;
	halFile->error = lastError = MED_ERR_NONE;
	halFile->ioType = MED_IO_TYPE_NONE;

	return halFile->fileSize;
}

// ------------------------------------------------------------------------
// read from an already-open file
// ------------------------------------------------------------------------
bool MediaHAL::Read(void *buffer, s32 len, t_FileHandle fileHandle)
{
	ASSERT(fileHandle != NULL);
	HALFilePSP *halFile = (HALFilePSP *)fileHandle;

	// Only have one pending IO at a time on the ps2  // PSP TODO - Is this true on PSP?
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

	// We should never really be down here...
	MediaHAL::WaitForReadyIO();

	error = sceIoRead(halFile->handle, buffer, len);

	if (error >= 0 && (halFile->openFlags & MEDOPEN_FLAG_NOWAIT))
	{
		ASSERTS(false, "PSP async file IO not ready"); // PSP TODO
		halFile->ioPending = true;
		return true;
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
bool MediaHAL::Write(const void *buffer, s32 len, t_FileHandle fileHandle)
{
	ASSERT(fileHandle != NULL);

#ifdef CDROM
	return false;
#else
	HALFilePSP *halFile = (HALFilePSP *)fileHandle;

	// Only have one pending IO at a time on the ps2   // PSP TODO - Is this true on PSP?
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

	int error = sceIoWrite(halFile->handle, buffer, len);
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
	{
		ASSERTS(false, "PSP async file IO not ready"); // PSP TODO
		return true;
	}
	else
		halFile->ioPending = true;

	return(AwaitIO(fileHandle, -1, NULL, NULL, NULL));
#endif
}

// ------------------------------------------------------------------------
// Make a PSP file name
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
	strcat(realFileName, baseFileName);
}

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
t_Error MediaHAL::TranslateError(
t_FileError platformError,
t_Error defaultError)
{
	t_Error error = defaultError;
#if 0 // PSP TEMP

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

#endif
	return(error);
}

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
const char *MediaHAL::GetErrorText(t_Error error)
{
	static char text[256];
#if 0 // PSP TEMP

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
#endif
	return(text);
}

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
bool MediaHAL::CompletionCheck(
HALFile *halFile,
s32 *transferSize)
{
	HALFilePSP *file = (HALFilePSP *)halFile;

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
	SceIores ioRes;
	result = sceIoPollAsync(file->handle, &ioRes);

	if (result < 0)
	{
		file->ioPending = false;
		file->error = lastError = TranslateError(result, defaultError);
		return true;
	}

	file->ioPending = result ? true : false;

	return(!file->ioPending);
}

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
long MediaHAL::PlatformFileHandle(t_FileHandle fileHandle)
{
	HALFilePSP *halFile = (HALFilePSP*)fileHandle;
	if (halFile)
		return (long)halFile->handle;
	else
		return -1;
}

// ----------------------------------------------------------------------------
bool MediaHAL::CheckTray()
{
#if 0 // PSP TEMP
#ifdef CDROM
	sceCdSync(0x10);						// Wait for completion of any other CD commands..BLOCKING
	int temp = sceCdGetDiskType();
	if( temp == SCECdNODISC )			// No-Disc, Tray opened
	{
		return( false );
	}
#endif
#endif
	return( true );
}

