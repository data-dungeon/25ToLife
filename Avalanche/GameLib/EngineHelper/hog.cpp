/***************************************************************************/
// Nice C++ wrapper around the HOG api.  Mostly for making private
// volumes...(see l_hog.h)
/***************************************************************************/
#include "EngineHelper/EngineHelperPCH.h"

/***************************************************************************/
// these don't do anything for error reporting reasons
/***************************************************************************/
Hog::Hog(void)
{
	hv = NULL;
}

/***************************************************************************/
/***************************************************************************/
Hog::~Hog(void)
{
	// Make sure we clean up
	Unmount();
}

/***************************************************************************/
// Mount/unmount the hog (if you aren't setting isPrivate, you
// are probably using this for the wrong reasons)
/***************************************************************************/
bool Hog::Mount(
const char *hogFile,
bool inMemory,
bool isPrivate)
{
	// Only one at a time!
	Unmount();

	// Mount it
	hv =  HogVolume_Mount(hogFile, inMemory, isPrivate);
	return(hv != NULL);
}

bool Hog::MountBuffer(
void *dataBuffer,
bool isPrivate)
{
	// Only one at a time!
	Unmount( );

	// Mount it
	hv = HogVolume_Mount( dataBuffer, isPrivate);
	return(hv != NULL);
}

/***************************************************************************/
/***************************************************************************/
bool Hog::Unmount(void)
{
	if (hv)
	{
		HogVolume_Unmount(hv);
		hv = NULL;
	}

	return(true);
}

/***************************************************************************/
// Override the checklocal behavior
/***************************************************************************/
bool Hog::OverrideCheckLocal(
HOG_CHECKLOCAL checkLocal)
{
	if (!hv)
		return false;

	HogVolume_OverrideCheckLocal(hv, checkLocal);
	return true;
}

/***************************************************************************/
/***************************************************************************/
HOG_CHECKLOCAL Hog::CheckLocal()
{
	return HogVolume_CheckLocal(hv);
}

/***************************************************************************/
// How many files are in this hog?
/***************************************************************************/
int Hog::Files(void)
{
	return(Hog_Files(hv));
}

/***************************************************************************/
// Get a file name
/***************************************************************************/
const char *Hog::FileName(
int whichFile)
{
	return(Hog_GetFileName(hv, whichFile));
}

/***************************************************************************/
// Get a file size
/***************************************************************************/
int Hog::FileSize(
int whichFile)
{
	return(Hog_GetFileSize(hv, whichFile));
}

/***************************************************************************/
// Does this file exist in the hog
/***************************************************************************/
bool Hog::FileExists(
const char *file)
{
	// Make sure we have a volume
	ASSERT(hv);
	if (!hv)
		return(false);

	// Pass it down
	return(Hog_Exists(hv, file));
}

/***************************************************************************/
// What is the size of this file
/***************************************************************************/
int Hog::FileSize(
const char *file)
{
	// Make sure we have a volume
	ASSERT(hv);
	if (!hv)
		return(false);

	// Pass it down
	return(Hog_Size(hv, file));
}

/***************************************************************************/
// What is the disk offset? (-1 if it doesn't exist)
/***************************************************************************/
int Hog::FileOffset(
const char *file)
{
	// Make sure we have a volume
	ASSERT(hv);
	if (!hv)
		return(false);

	// Pass it down
	return(Hog_Offset(hv, file));
}

/***************************************************************************/
// Get a const char * to the TOC entry for this file. This allows you NOT to
// store the filename twice. Returns NULL if it does not exist.
/***************************************************************************/
const char *Hog::GetConstFileName(const char *pathFile)
{
	return Hog_GetConstFileName(hv, pathFile);
}

/***************************************************************************/
// Get the size and offset in one fell swoop
// returns false if it doesn't exist
/***************************************************************************/
bool Hog::FileOffsetSize(
const char *file,
int &offset,
int &size)
{
	// Make sure we have a volume
	ASSERT(hv);
	if (!hv)
		return(false);

	// Pass it down
	return(Hog_OffsetSize(hv, file, offset, size));
}

/***************************************************************************/
// Open a file
/***************************************************************************/
HOGFILE Hog::FileOpen(
const char *file,
bool inMemory,
u32 openFlags)
{
	// Make sure we have a volume
	ASSERT(hv);
	if (!hv)
		return(false);

	// Pass it down
	return(Hog_Open(hv, file, inMemory, openFlags));
}

/***************************************************************************/
// Get a pointer to a file (only works if inMemory was set)
/***************************************************************************/
void *Hog::FilePtr(
HOGFILE hf,
int &size)
{
	// Pass it down
	return(HogFile_Ptr(hf, &size));
}

/***************************************************************************/
// How big is the file
/***************************************************************************/
int Hog::FileSize(
HOGFILE hf)
{
	// Pass it down
	return(HogFile_Size(hf));
}

/***************************************************************************/
// Read from a file.  Returns the number of bytes read
/***************************************************************************/
int Hog::FileRead(
void *buff,
int bytes,
HOGFILE hf)
{
	// Pass it down
	return(HogFile_Read(buff, bytes, hf));
}

/***************************************************************************/
// Seek in a file
/***************************************************************************/
bool Hog::FileSeek(
HOGFILE hf,
int offset,
HOGSEEK origin)
{
	// Pass it down
	return(HogFile_Seek(hf, offset, origin));
}

/***************************************************************************/
// What is the file pointer for this file
/***************************************************************************/
int Hog::FileTell(
HOGFILE hf)
{
	// Pass it down
	return(HogFile_Tell(hf));
}

/***************************************************************************/
// Close the hogfile (if an in memory, the memory goes away)
/***************************************************************************/
bool Hog::FileClose(
HOGFILE hf)
{
	// Pass it down
	return(HogFile_Close(hf));
}
