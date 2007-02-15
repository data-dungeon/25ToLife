// media.cpp

//****************** includes ***************************

//** pre-compiled headers through .h file (visual studio), can't be inside #if statement. **
//** see .h file for more info.for codewarrior, pre-compiled header is handled through    **
//** xcl compile/link interpreter, and doesn't need any explicit include                  **

#include "Layers/LayersPCH.h"
#include "Layers/DiscErrorHandler/DiscError.h"
#include "Layers/loadprofile.h"
#include "EngineHelper/Hash.h"
#include <new.h>
#include "Game/Managers/ResourceManager.h"

#ifdef PS2
#include "Patcher/PatchExt/PatchExtractor.h"
#endif

// Local Data
int					Media::maxOpen = 0;
MediaFile			*Media::openList = NULL;

Media::OpenHog		*Media::openHog = NULL;
int					Media::openHogs = 0;

// Profiling
MediaFile			*Media::openProfile = NULL;
s32					Media::readTotal = 0;

bool				Media::m_patchingEnabled = false;
char				*Media::m_pPatchBuffer = NULL;
PatchExtractor		*Media::m_pPatchExtractor = NULL;

extern void ProcessIOPResume( bool force );

// ------------------------------------------------------------------------
// init the file system-- set up a list of open files
// ------------------------------------------------------------------------
void Media::Init(int maxOpenFiles)
{
	// Let the lower layer know about this
	MediaHAL::Init(maxOpenFiles);

	maxOpen = maxOpenFiles;
	if (maxOpen == 0)
		openList = NULL;
	else
	{
		MEM_SET_ALLOC_NAME("med open list");
		void *temp = memAlloc(maxOpen * sizeof(MediaFile));
		openList = new (temp) MediaFile[maxOpen];
	}

	// Create the hog stuff
	openHogs = Hog_MaxVolumes();
	if (openHogs == 0)
		openHog = NULL;
	else
	{
		MEM_SET_ALLOC_NAME("med hog list");
		openHog = (OpenHog *)memAlloc(openHogs * sizeof(OpenHog));
		ASSERT(openHog);
		for (int i = 0; i < openHogs; i++)
			openHog[i].volume = NULL;
	}

	// init error code
	MediaHAL::lastError = MED_ERR_NONE;
}

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
void Media::Terminate()
{
	// Let the lower layer know about this
	MediaHAL::Terminate();
}

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
void Media::Daemon()
{
	MediaHAL::Daemon();
}

// ------------------------------------------------------------------------
// Set the game title name for error messages
// ------------------------------------------------------------------------
void Media::SetTitle(const char *title)
{
	// Just pass it on to the HAL
	MediaHAL::SetTitle(title);
}

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
bool Media::CheckCaps(Caps cap)
{
	switch (cap)
	{
		case SHORT_READ:
			return(true);
	}

	return(false);
}

// ------------------------------------------------------------------------
// Mount a HOG volume into the media system
// ------------------------------------------------------------------------
bool Media::MountHog(const char *hogFile)
{
	// Get a hog
	OpenHog *hog = GetOpenHog();
	if (!hog)
		return(false);

	// Open the hog
	hog->volume = HogVolume_Mount(hogFile, false, false);
	if (!hog->volume)
		return(false);

	// save off the name
	strcpy(hog->name, hogFile);
	return(true);
}

// ------------------------------------------------------------------------
// Mount a HOG volume into the media system in memory,
//	be very careful with this, it loads the entire hog
// ------------------------------------------------------------------------
bool Media::MountMemoryHog(const char *hogFile)
{
	// Get a hog
	OpenHog *hog = GetOpenHog();
	if (!hog)
		return(false);

	// Open the hog
	hog->volume = HogVolume_Mount(hogFile, true, false);
	if (!hog->volume)
		return(false);

	// save off the name
	strcpy(hog->name, hogFile);
	return(true);
}

// ------------------------------------------------------------------------
// Unmount a HOG volume into the media system
// ------------------------------------------------------------------------
bool Media::UnmountHog(const char *hogFile)
{
	// Get a hog
	OpenHog *hog = FindOpenHog(hogFile);
	if (!hog)
		return(false);

	// Close the hog
	HogVolume_Unmount(hog->volume);
	hog->volume = NULL;

	return(true);
}

// ------------------------------------------------------------------------
// start an open "profile", basically a log of file opens
// Writes to openNNNN.lst, where NNNN increments to avoid collisions
// ------------------------------------------------------------------------
bool Media::StartOpenProfile(void)
{
	// Only one at a time...
	EndOpenProfile();

	// Find the next open file name
	int fileTest = 0;
	do {
		// Build the hashed file name
		char	fileNum[32];
		char	fileName[64];
		strcpy(fileName, "open");
		sprintf(fileNum, "%d", fileTest);
		int padLen = 4 - strlen(fileNum);
		for (; padLen > 0; --padLen)
			strcat(fileName, "0");
		strcat(fileName, fileNum);
		strcat(fileName, ".lst");

		// Does this file exist?
		if (FileExists(fileName))
			++fileTest;
		else
		{
			// Create the file
			t_Error		Error;
			openProfile = Open(fileName, &Error, MEDOPEN_FLAG_WRITE);

			// If this fails, we are done
			if (!openProfile)
				return(false);
		}
	} while (openProfile == NULL);

	// Wee, it is open
	return(true);
}

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
bool Media::StartOpenProfile(const char *worldName)
{
	// Only one at a time...
	EndOpenProfile();

	char fileName[64];
	strcpy(fileName, worldName);
	strcat(fileName, ".lst");

	// Create the file
	t_Error Error;
	openProfile = Open(fileName, &Error, MEDOPEN_FLAG_WRITE);

	// If this fails, we are done
	if (!openProfile)
		return(false);

	// Wee, it is open
	return(true);
}

// ------------------------------------------------------------------------
// End the profile
// ------------------------------------------------------------------------
bool Media::EndOpenProfile(void)
{
	if (!openProfile)
		return(false);

	openProfile->Close(NULL);
	openProfile = NULL;
	return(true);
}

// ------------------------------------------------------------------------
// Clear the read counter
// ------------------------------------------------------------------------
bool Media::ClearReadTotal(void)
{
	readTotal = 0;
	return(true);
}

// ------------------------------------------------------------------------
// Get the read counter
// ------------------------------------------------------------------------
s32 Media::GetReadTotal(void)
{
	return(readTotal);
}

// ------------------------------------------------------------------------
// set up root directory name
// ------------------------------------------------------------------------
void Media::SetRootDir(const char *newRootDir)
{
	// ensure that new root directory is either empty or ends with a backslash
	ASSERT((strlen(newRootDir)) ? (newRootDir[strlen(newRootDir)-1] == '\\') : true);
	MediaHAL::SetRootDir(newRootDir);
}

// ------------------------------------------------------------------------
// make platform independent name into a platform dependent version
// ------------------------------------------------------------------------
void Media::MakeFileName(const char *baseFileName, char *realFileName)
{
	MediaHAL::MakeFileName(baseFileName, realFileName, true);
}

// ------------------------------------------------------------------------
// make platform independent name into a platform dependent version
// ------------------------------------------------------------------------
#if defined (_XBOX)
char *Media::GetUtilityFileName(const char *baseFileName)
{
	static char realGetFileName[MAX_FILENAME_LEN];
	MediaHAL::MakeUtilityFileName(baseFileName, realGetFileName, true);
	return((char *)&realGetFileName);
}

char *Media::GetUtilityCommonFileName(const char *baseFileName)
{
	static char realCommonFileName[MAX_FILENAME_LEN];
	MediaHAL::MakeUtilityFileName(baseFileName, realCommonFileName, false);
	return((char *)&realCommonFileName);
}

#endif
// ------------------------------------------------------------------------
// make platform independent name into a platform dependent version
// ------------------------------------------------------------------------
char *Media::GetFileName(const char *baseFileName)
{
	static char realGetFileName[MAX_FILENAME_LEN];
	MediaHAL::MakeFileName(baseFileName, realGetFileName, true);
	return((char *)&realGetFileName);
}

// ------------------------------------------------------------------------
// make platform independent name into a platform independent version
// ------------------------------------------------------------------------
char *Media::GetCommonFileName(const char *baseFileName)
{
	static char realCommonFileName[MAX_FILENAME_LEN];
	MediaHAL::MakeFileName(baseFileName, realCommonFileName, false);
	return((char *)&realCommonFileName);
}

// ------------------------------------------------------------------------
// allow external programs to turn off data directory name mangling. this is
//	so pigpen can use arbitrary files, and so contexts can save to the
//	data independent directory
// ------------------------------------------------------------------------
void Media::EnableNameMangling(bool state)
{
	MediaHAL::EnableNameMangling(state);
}

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
bool Media::GetNameManglingState(void)
{
	return(MediaHAL::GetNameManglingState());
}

// ------------------------------------------------------------------------
// check if a file exists
// ------------------------------------------------------------------------
bool Media::FileExists(const char *fileName)
{
	return(Hog_Exists(NULL, fileName));
}

// ------------------------------------------------------------------------
// Does this media stream exist?  This bypasses all Hog caching since most of
//	the platforms use a separate I/O path for media streams.
// ------------------------------------------------------------------------
bool Media::StreamExists(const char *fileName)
{
	char nativeFileName[MAX_FILENAME_LEN];
	MediaHAL::MakeFileName(fileName, nativeFileName, true);
	return(MediaHAL::Exists(nativeFileName));
}

// ------------------------------------------------------------------------
// What is the size of this file
// ------------------------------------------------------------------------
s32 Media::FileSize(const char *fileName)
{
	return(Hog_Size(NULL, fileName));
}

// ------------------------------------------------------------------------
// read in entire file
// ------------------------------------------------------------------------
void *Media::Load(
const char *fileName,
s32 *size,
t_Error *error,
int extraAlloc)
{
	// DAS 8/27/05 this completely sucks my ass, but I have no choice right now.
	// Because of DNAS error display requirements we are not able to resume the IOP
	// if an error occured during DNAS, since it hangs until the tray is shut.  So we
	// queue the resume for when the tray does shut, but if we try to load something
	// from CD before the IOP is resumed, we're screwed. Forcing a resume will cause
	// us to hang until the user shuts the tray and then we can do our load.
#ifdef PS2
	ProcessIOPResume( true );
#endif

	// built-in profiling
	Profiler__Start(PROFILE_FILE_LOAD);

	// Open the file
	HOGFILE hogFile = Hog_Open(NULL, fileName, false, 0);
	if (!hogFile)
	{
		if (size)
			*size = 0;	// indicate file not found
		SetError(error);
		Profiler__Stop(PROFILE_FILE_LOAD);
		return(NULL);
	}

	// Notify the system
	LoadProfile::NotifyFileLoad(fileName);

	// Do they care about the size?
	s32 tempSize;
	if (!size)
		size = &tempSize;

	// how long is file?
	*size = HogFile_Size(hogFile);
	if (*size <= 0)
	{
		HogFile_Close(hogFile);
		SetError(error);
		Profiler__Stop(PROFILE_FILE_LOAD);
		return(NULL);
	}

	// --- DAS 12/16/2004 , we need to check for patching now so we can allocate our memory first
	// --- so we don't have any fragmentation if we need to patch and free the original buffer
#ifdef PS2
	int patchedSize = 0;
	void *pPatchedFile = NULL;
	if( m_patchingEnabled )
	{
		if( m_pPatchExtractor->PatchExists( fileName, patchedSize ) )
		{
			if( patchedSize > 0 )
			{
				pPatchedFile = memAlloc( patchedSize + extraAlloc, 128 );
				ASSERTS( pPatchedFile, "failed to allocate memory for patched file" );
				if( !pPatchedFile )
				{
					HogFile_Close(hogFile);
					Profiler__Stop(PROFILE_FILE_LOAD);
					return(NULL);
				}
			}
		}
	}
#endif

	// Allocate the buffer
	MEM_SET_ALLOC_FILENAME("FS ", (char *)fileName);
	void *buffer;
#ifdef PS2
	if( isDLL( fileName ) )
		buffer = memAllocForIO(*size + extraAlloc, 128);
	else
#endif
		buffer = memAllocForIO(*size + extraAlloc);

	if (!buffer)
	{
		HogFile_Close(hogFile);
		Profiler__Stop(PROFILE_FILE_LOAD);
		return(NULL);
	}

	// read it in
	if (HogFile_Read(buffer, *size, hogFile) != *size)
	{
		memFree(buffer);
#ifdef PS2
		if( pPatchedFile )
		{
			memFree( pPatchedFile );
		}
#endif
		HogFile_Close(hogFile);
		SetError(error);
		Profiler__Stop(PROFILE_FILE_LOAD);
		return(NULL);
	}

	// done
	HogFile_Close(hogFile);
	Profiler__Stop(PROFILE_FILE_LOAD);

	// --- DAS 12/16/2004 , we need to check for patching now
#ifdef PS2
	if( buffer && m_patchingEnabled && pPatchedFile )
	{
		// DFS -- Set source for patch to our original data
		m_pPatchExtractor->SetSourceBuffer(buffer);
		if( m_pPatchExtractor->PatchItem( ( char * )pPatchedFile, patchedSize ) )
		{
			memFree( buffer );
			*size = patchedSize;
			return( pPatchedFile );
		}
		else
		{
			// ERROR applying patch.
			memFree( pPatchedFile );
			scePrintf( "ERROR: couldn't patch '%s'\n", fileName );
		}
	}
#endif

#if defined(CRCFILECHECK)
	TRACE("Computed CRC for file \"%s\" (size %d) = 0x%x\n", fileName, *size, Hash::CalcChecksum(buffer, *size));
#endif

	return(buffer);
}

// ------------------------------------------------------------------------
// Load in the entire file into a block
// ------------------------------------------------------------------------
bool Media::LoadStatic(
const char *fileName,
void *buffer,
s32 maxSize,
s32 *size,
t_Error *error)
{
	// Open the file
	HOGFILE hogFile = Hog_Open(NULL, fileName, false, 0);
	if (!hogFile)
	{
		SetError(error);
		return(false);
	}

	// Notify the system
	LoadProfile::NotifyFileLoad(fileName);

	// Do they care about the size?
	s32 tempSize;
	if (!size)
		size = &tempSize;

	// how long is file?
	*size = HogFile_Size(hogFile);
	if (*size < 0)
	{
		HogFile_Close(hogFile);
		SetError(error);
		return(false);
	}

	// How much can we read?
	s32 readSize;
	if (maxSize < *size)
		readSize = maxSize;
	else
		readSize = *size;

	// read it in
	if (HogFile_Read(buffer, readSize, hogFile) != readSize)
	{
		HogFile_Close(hogFile);
		SetError(error);
		return(false);
	}

	// done
	HogFile_Close(hogFile);
	return(true);
}

// ------------------------------------------------------------------------
// open passed file and record it in the open file list
// ------------------------------------------------------------------------
MediaFile *Media::Open(
const char *fileName,
t_Error *error,
u32 openFlags)
{
	// get an entry in open file list
	MediaFile *file = GetOpenFile();
	if (!file)
	{
		// something went wrong
		SetError(error);
		return(NULL);
	}

	// Pass it down for the actual file to do the work
	if (!file->Open(fileName, error, openFlags))
		return(NULL);
	else
	{
		LoadProfile::NotifyFileLoad(fileName);
		return(file);
	}
}

// ------------------------------------------------------------------------
// Start asynchronous load of a file
// ------------------------------------------------------------------------
MediaFile *Media::StartAsyncLoad(const char *fileName, void **ppBuffer, int *pnFileSize, int extraAlloc)
{
	/* open up the file-- we need to get its size and malloc a buffer for it before we can read it asynchronously */

	MediaFile *pMediaFile = Media::Open(fileName, NULL, MEDOPEN_FLAG_NOWAIT);
	if (pMediaFile == NULL)
	{
		ASSERTF(false, ("Can't open file %s for async load", fileName));
		DiscErrorHandler::DisplayError( (char *)fileName, DiscErrorHandler::OPEN_FILE_FAILED ); // error warning for failed load
		return(NULL);
	}

	/* malloc a buffer. memAlloc code copied from Media::Load. change allocation method to put loaded file at */
	/* bottom of the heap, since it will likely be collapsed or go away. this is a bit better for avoiding */
	/* memory holes */

#ifdef GCN
	BOOL	enabled;

	enabled = OSDisableInterrupts( );
	OSDisableScheduler( );
	OSRestoreInterrupts( enabled);
#endif

	*pnFileSize = pMediaFile->Size();
	MEM_SET_ALLOC_FILENAME("FS ", (char *) fileName);
	memSetAllocMethod( ALC_BTM );
	memSetFitMethod( FIT_FIRST );
	*ppBuffer = memAllocForIO(*pnFileSize + extraAlloc);
	memSetAllocMethod( ALC_TOP );
	memSetFitMethod( FIT_LAST );

#ifdef GCN
	enabled = OSDisableInterrupts( );
	OSEnableScheduler( );
	OSRestoreInterrupts( enabled);
#endif

	if (*ppBuffer == NULL)
	{
		ASSERTF(false, ("Not enough memory available for file %s async load", fileName));
		pMediaFile->Close();
		return(NULL);	// no memory available!
	}

	/* start the asynchronous load (asynchronous because we passed MEDOPEN_FLAG_NOWAIT above) */

	pMediaFile->Read(*ppBuffer, *pnFileSize, NULL);

	/* caller is responsible for waiting for file load to complete, closing file, freeing buffer */

	return(pMediaFile);
}

// ------------------------------------------------------------------------
// close all files in open file list
// ------------------------------------------------------------------------
void Media::CloseAll(void)
{
	for (int i = 0; i < maxOpen; i++)
	{
		if (openList[i].IsOpen())
			openList[i].Close();
	}
}

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
t_Error Media::GetLastError(void)
{
	return(MediaHAL::lastError);
}

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
void Media::ResetError(void)
{
	MediaHAL::lastError = MED_ERR_NONE;
}

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
const char *Media::GetErrorText(t_Error error)
{
	return(MediaHAL::GetErrorText(error));
}

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
MediaFile *Media::GetOpenFile(void)
{
	int i;

	for (i = 0; i < maxOpen; i++)
	{
		if (!openList[i].IsOpen())
			return(&openList[i]);
	}

	MediaHAL::lastError = MED_ERR_MAXOPEN;
	return(NULL);
}

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
void Media::ConvertSlashes(char *fileName)
{
	char *ptr;
	while ((ptr = strchr(fileName, '/')) )
		*ptr = '\\';
}

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
Media::OpenHog *Media::GetOpenHog(void)
{
	for (int i = 0; i < openHogs; i++)
	{
		if (openHog[i].volume == NULL)
			return(&openHog[i]);
	}

	// This is bad, we are out of hog volumes
	ASSERT(false);
	return(NULL);
}

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
Media::OpenHog *Media::FindOpenHog(
const char *hogFile)
{
	for (int i = 0; i < openHogs; i++)
	{
		if (strcmpi(openHog[i].name, hogFile) == 0)
			return(&openHog[i]);
	}

	// This is bad, we can't find this hog
	return(NULL);
}

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
void Media::LogFileOpen(const char *fileName)
{
	if (openProfile)
	{
		// Clean yourself up!!!
		if (fileName[0] == '\\')
			++fileName;
		else if (fileName[0] == '.' && fileName[1] == '\\')
			fileName += 2;

		// clean it up
		int len = strlen(fileName);
		char clean[256];
		ASSERT(len < sizeof(clean));
		int i;
		for (i = 0; i < len; i++)
		{
			if (fileName[i] == '/')
				clean[i] = '\\';
			else
				clean[i] = tolower(fileName[i]);
		}
		clean[i] = '\0';

		// We don't care too much about errors here
		openProfile->Write((char *)clean, len, NULL);
		openProfile->Write((char *)"\r\n", 2, NULL);
	}
}

// ------------------------------------------------------------------------
// MediaFile class
// ------------------------------------------------------------------------

// ------------------------------------------------------------------------
// get size of file
// ------------------------------------------------------------------------
s32 MediaFile::Size(void)
{
	// this would be bad
	ASSERT(IsOpen());

	// Are we in hog mode or not?
	if (IsHog())
		return(HogFile_Size(hog));
	else
		return(MediaHAL::Length(file));
}

// ------------------------------------------------------------------------
// Seek to an offset on the file
// ------------------------------------------------------------------------
bool MediaFile::Seek(s32 pos)
{
	// this would be bad
	ASSERT(IsOpen());

	// Are we in hog mode or not?
	if (IsHog())
		return(HogFile_Seek(hog, pos, HOGSEEK_SET));
	else
		return(MediaHAL::Seek(file, pos));
}

// ------------------------------------------------------------------------
// read requested section of file into buffer
// ------------------------------------------------------------------------
bool MediaFile::Read(
void *buffer,
s32 len,
t_Error *error)
{
	// this would be bad
	ASSERT(IsOpen());

	// read data
	bool ret;
	if (IsHog())
		ret = (HogFile_Read(buffer, len, hog) == len);
	else
		ret = MediaHAL::Read(buffer, len, file);

	// success?
	if (ret)
		return(true);
	else
	{
		SetError(error);
		return(false);
	}
}

// ------------------------------------------------------------------------
// write data from the buffer into a file
// ------------------------------------------------------------------------
bool MediaFile::Write(
const void *buffer,
s32 len,
t_Error *error)
{
	// Doh!
	ASSERT(IsOpen());

	// Hogs do not support writes
	if (IsHog())
		return(false);

	// read data
	if (!MediaHAL::Write(buffer, len, file))
	{
		SetError(error);
		return(false);
	}

	// success!
	return(true);
}

// ------------------------------------------------------------------------
// wait for an io completion
// ------------------------------------------------------------------------
bool MediaFile::AwaitIO(
s32 timeOut,
void **buffer,
s32 *ioSize,
t_Error *error)
{
	// this would be bad
	ASSERT(IsOpen());

	// cAll the appropriate HAL function
	if (IsHog())
		return(MediaHAL::AwaitIO(HogFile_GetFileHandle(hog), timeOut, buffer, ioSize, error));
	else
		return(MediaHAL::AwaitIO(file, timeOut, buffer, ioSize, error));
}

// ------------------------------------------------------------------------
// close passed file-- should be in open file list
// ------------------------------------------------------------------------
bool MediaFile::Close(
t_Error *error)
{
	// this would be bad
	ASSERT(IsOpen());

	// Are we in hog mode or not?
	if (IsHog())
		HogFile_Close(hog);
	else
		MediaHAL::Close(file);

	// Free this guy up
	mode = OPENFILE_UNUSED;
	return(true);
}

// ------------------------------------------------------------------------
// open passed file and record it in the open file list
// ------------------------------------------------------------------------
bool MediaFile::Open(
const char *fileName,
t_Error *error,
u32 openFlags)
{
	// Can't use the hog system for writes
	bool success;
	if (openFlags & MEDOPEN_FLAG_WRITE)
	{
		#if defined (_XBOX)
		if (openFlags & MEDOPEN_FLAG_CACHE)
		{
			// Open using the low level calls
			success = MediaHAL::Open(Media::GetUtilityFileName(fileName), &file, openFlags);
		}
		else
		#endif
		{
			// Open using the low level calls
			success = MediaHAL::Open(Media::GetFileName(fileName), &file, openFlags);
		}
		if (success)
			mode = OPENFILE_FILE;
	}
	else
	{
		// Open us in HOG mode
		hog = Hog_Open(NULL, fileName, false, openFlags);
		if (!hog)
			success = false;
		else
		{
			// save our state
			success = true;
			mode = OPENFILE_HOG;
		}
	}

	// Did stuff work?
	if (success)
		return(true);
	else
	{
		// something went wrong
		//dbgPrint("Could not open file: %s\n", fileName);
		SetError(error);
		return(false);
	}
}

// ----------------------------------------------------------------------------
// Allow files to be patched for Online Play
// ----------------------------------------------------------------------------
void Media::EnablePatching( char *pPatchBuf, int size )
{
#ifdef PS2
	m_patchingEnabled = true;
	m_pPatchBuffer = pPatchBuf;
	if( pPatchBuf )
	{
		m_pPatchExtractor = new PatchExtractor( pPatchBuf, size );
		ASSERTS( m_pPatchExtractor, "failed to create Patch Extractor" );
		if( m_pPatchExtractor )
		{
			// --- make sure the patch is good
			if( m_pPatchExtractor->IsGood() )
			{
				return;
			}
		}
	}
#endif

	// --- if we get here something went wrong, so turn off patching
	m_patchingEnabled = false;
}


// ----------------------------------------------------------------------------
// Disable files to be patched for Online Play
// ----------------------------------------------------------------------------
void Media::DisablePatching( void )
{
#ifdef PS2
	if( m_pPatchBuffer )
	{
		// --- we'll want to free up the patch buffer somehow
		if( m_pPatchExtractor )
		{
			delete m_pPatchExtractor;
			m_pPatchExtractor = NULL;
		}

		if( m_patchingEnabled )
		{
			delete m_pPatchBuffer;
			m_pPatchBuffer = NULL;
		}
	}
#endif
	m_patchingEnabled = false;
}

// ----------------------------------------------------------------------------
// Determine if the file being loaded is a DLL executable
// ----------------------------------------------------------------------------
bool Media::isDLL(  const char *filename )
{
	char *p;

	p = strstr( filename, "." );
	if( p )
	{
		p++;
		if( toupper( *p++) != 'R' ) return( false );
		if( toupper( *p++ ) != 'E' ) return( false );
		if( toupper( *p   ) != 'L' ) return( false );
		return( true );
	}
	return( false );
}

// ----------------------------------------------------------------------------
// Return patch extractor
// ----------------------------------------------------------------------------
PatchExtractor* Media::GetPatchExtractor()
{
	if ( m_patchingEnabled )
		return m_pPatchExtractor;
	else
		return NULL;
}
