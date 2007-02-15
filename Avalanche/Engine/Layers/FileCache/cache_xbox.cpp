//--------------------------------------------------------------//
// cache_xbox.cpp
//
// Caching Class.. allows files to be caches as loaded to
// alternate storage
//
//--------------------------------------------------------------//

#include "Layers/LayersPCH.h"
#include "cache_xbox.h"
#include "GameHelper/StringHelper.h"

//--------------------------------------------------------------//
// defines
#define FCACHE_TIME	__TIME__
#define FCACHE_DATE	__DATE__

//--------------------------------------------------------------//
// vars

typedef	void (CFileCache_Func)(void);

static	char*	CFileCache_Drive;
static	char*	CFileCache_SecondaryDrive;
static	int	CFileCache_MaxSize;
static	char	CFileCache_date[32];
static	char	CFileCache_time[32];
static	CFileCache_Func	*CFileCache_UpdateFunction;

//--------------------------------------------------------------//
// callback

DWORD	CALLBACK CFileCache_CopyProgress( LARGE_INTEGER	TotalFileSize,
													LARGE_INTEGER	TotalBytesTransferred,
													LARGE_INTEGER	StreamSize,
													LARGE_INTEGER	StreamBytesTransferred,
													DWORD				dwStreamNumber,
													DWORD				dwCallbackReason,
													HANDLE			hSourceFile,
													HANDLE			hDestinationFile,
													LPVOID			lpData)
{

	// call user defined func if we have one
	if (CFileCache_UpdateFunction != NULL)
		CFileCache_UpdateFunction();

	// continue
	if (TotalFileSize.QuadPart < CFileCache_MaxSize)
		return	PROGRESS_CONTINUE;
	else
		return	PROGRESS_CANCEL;
}

//--------------------------------------------------------------//
// methods

bool	CFileCache::InitCacheSystem(char *Drive, int MaxFilesize, bool format)
{
	bool	err;

	CFileCache_Drive = "none";
	CFileCache_SecondaryDrive = "none";
	CFileCache_MaxSize = MaxFilesize;
	CFileCache_UpdateFunction = NULL;

	err = SetMountDrive( Drive );
	ASSERTF(err != false, ("Drive %s not available for caching", Drive));

	#if 0
//	#if !defined (XBOX_SHIP_IT)
	// timestamp
   if (ReadTimestamp() == false)
	{
		CreateTimestamp();
		format = true;
	}
	else		// does it match our build
	{
		// compare with current build data & time
		if (!strcmp(__DATE__, CFileCache_date))
		{
			format = false;
		}
		else
		{
			CreateTimestamp();
			format = true;
		}
	}
	#endif

	MountDrive(format);

	return err;
}

//--------------------------------------------------------------//
bool	CFileCache::SetMountDrive(char *Drive)
{
	if (!strcmp(Drive, "Z:") && !strcmp(Drive, "N:"))
		return false;

	if (!strcmp(Drive, "Z:"))
		CFileCache_Drive = Drive;
	if (!strcmp(Drive, "N:"))
		CFileCache_SecondaryDrive = Drive;

	return	true;
}

//--------------------------------------------------------------//
void	CFileCache::MountDrive(bool format)
{
	BOOL	HR;

	// mount main drive
	if (strcmp(CFileCache_Drive, "none"))
	{
		HR = XMountUtilityDrive(format);
		ASSERTS(HR != false, "Couldnt mount Utility Drive");
	}

	// mount secondary drive
	if (strcmp(CFileCache_SecondaryDrive, "none"))
	{
		HR = XMountSecondaryUtilityDrive();
		ASSERTS(HR != false, "Couldnt mount Secondary Utility Drive");
	}
}

//--------------------------------------------------------------//
bool	CFileCache::UnmountDrive()
{
	CFileCache_Drive = "none";
	CFileCache_SecondaryDrive = "none";

	return true;
}

//--------------------------------------------------------------//
bool	CFileCache::CreateDirectories(char** DirList)
{
	int	index = 0;

	if (DirList[index] == NULL)
		return false;

	// cycle through list and create the directories
	while (DirList[index] != NULL)
	{
		CreateDirectory((LPCSTR)DirList[index], NULL);
		index++;
	}

	return true;
}

//--------------------------------------------------------------//
bool	CFileCache::CacheFile(char* Dst, char* Src, bool overwrite)
{
	BOOL	HR;

	HR = CopyFile(Src, Dst, overwrite);

	if (HR == false)
		return false;
	else
		return true;
}

//--------------------------------------------------------------//
bool	CFileCache::CacheFileProgress(char* Dst, char* Src, bool overwrite)
{
	BOOL	HR;

	if (overwrite == true)
		HR = CopyFileEx(Src, Dst, CFileCache_CopyProgress, NULL, NULL, NULL);
	else
		HR = CopyFileEx(Src, Dst, CFileCache_CopyProgress, NULL, NULL, COPY_FILE_FAIL_IF_EXISTS);

	if (HR == false)
		return false;
	else
		return true;
}

//--------------------------------------------------------------//
bool	CFileCache::CacheFileAsync(char* Dst, char* Src, bool overwrite)
{
	// TO DO
	return true;
}

//--------------------------------------------------------------//
void	CFileCache::RegisterUpdate(void* Function)
{
	CFileCache_UpdateFunction = (CFileCache_Func*)Function;
}

//--------------------------------------------------------------//
bool	CFileCache::CreateTimestamp(void)
{
	HANDLE	handle;
	char		dir[MAX_PATH];
	size_t	countWritten = 0;

	DWORD result = XCreateSaveGame( "U:\\", (WCHAR*)CStringHelper::AsciiToUnicode("Timestamp"), OPEN_ALWAYS, 0, dir, MAX_PATH);

	if (result != ERROR_SUCCESS)
		return false;

	// create filename
	strcat(dir, "timestamp.bin");
	handle = CreateFile(dir, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (handle == INVALID_HANDLE_VALUE)
		return false;

   // write some data
	char	d[32];
	char	t[32];
	strcpy (d, FCACHE_DATE);
	strcpy (t, FCACHE_TIME);

	result = WriteFile(handle, &d, sizeof (d),(LPDWORD)&countWritten, 0); 
	result = WriteFile(handle, &t, sizeof (t),(LPDWORD)&countWritten, 0); 
	if (result == 0)
	{
		CloseHandle(handle);
		return false;
	}

	CloseHandle(handle);
	return true;
}

//--------------------------------------------------------------//
bool	CFileCache::ReadTimestamp(void)
{
	HANDLE	handle;
	char		dir[MAX_PATH];
	size_t	countRead = 0;

	DWORD result = XCreateSaveGame( "U:\\", (WCHAR*)CStringHelper::AsciiToUnicode("Timestamp"), OPEN_EXISTING, 0, dir, MAX_PATH);

	if (result != ERROR_SUCCESS)
		return false;

	// create filename
	strcat(dir, "timestamp.bin");
	handle = CreateFile(dir, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (handle == INVALID_HANDLE_VALUE)
	{
		CloseHandle(handle);
		return false;
	}

	// read data & time
	result = ReadFile(handle, &CFileCache_date, sizeof(CFileCache_date),(LPDWORD)&countRead, 0);
	result = ReadFile(handle, &CFileCache_time, sizeof(CFileCache_time),(LPDWORD)&countRead, 0);

//	result = GetFileTime(handle, &CFileCache_FileTime, NULL, NULL);
	if (result == 0)
	{
		CloseHandle(handle);
		return false;
	}

	CloseHandle(handle);
	return true;
}



