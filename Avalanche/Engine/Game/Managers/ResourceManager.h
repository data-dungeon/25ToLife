//============================================================================
//=
//= ResourceManager.h
//=
//= Loads files and unloads files when they're no longer needed.  Loads
//= are done asynchronously.
//=
//============================================================================

#ifndef _RESOURCE_MANAGER_H
#define _RESOURCE_MANAGER_H

#include "container/Darray.h"

//============================================================================

class CResourceManager
{
public:
	CResourceManager();
	~CResourceManager();

	/// Loads a file asynchronously.  GroupID is given so many files
	/// can be queried or purged at once.  Must be 0..31
	void		LoadFile( const char* filename, int groupId );

	/// Checks a specific file if it has finished loading.
	bool		IsLoaded( const char* filename );

	/// Checks if all files of a specified groupId have finished loading.
	bool		IsLoaded( int groupId );

	/// Checks to see if this file has a load pending, but not completed
	bool		IsLoadPending( const char* filename );

	/// Checks to see if any files in this group are pending, but not
	/// completed.
	bool		IsLoadPending( int groupId );

	/// Cancels any files yet to be loaded
	bool		CancelLoads();

	/// Unload all files of a specified groupId, frees memory
	void		UnloadGroup( int groupId, bool purge = true );

	/// Unload all files managed by the resource manager.
	void		UnloadAll();

	/// Purge all "unowned" files
	void		Purge();

	/// Get the buffer of the loaded file.  If file hasn't finished loading
	/// this will return NULL.
	void*    FileBuffer( const char* filename, int *size = NULL );

	/// Get the buffer size of a loaded file.  If file hasn't finished
	/// loading this will return 0.
	int      FileSize( const char* filename );
	
	/// Get the DBL handle of a dbl file.  If file hasn't finished loading
	/// this will return INVALID_HANDLE.
	t_Handle FileHandle( const char* filename );

	/// Main update routine to be processed every frame
	void		Update();

	/// Master reset - unloads all files and frees all resources
	void		Reset();

	/// Return information about a group mask
	void		GroupInfo( int* filesInGroup, int* filesInGroupLoaded, u32 mask );

	/// Return the loaded percentage
	float		GetLoadedPercentage();

	// Are there loading pending?
	bool		LoadsPending()
		{ return d_loadsPending; }

	/// Spin waiting for loads to finish
	void		WaitUntilIdle();

private:
	int  GetLoadingIndex();
	void StartNextFile();
	void FinishFileLoad( int index );
	void UnloadFile( int index );
	int  FindFile( const char* filename );
	static bool IsDBL( const char* filename );
	static bool IsText( const char* filename );

	struct LoadedResource
	{
		char			filename[ 64 ];
		u32			groupMask;
		void*			buffer;
		int			bufferSize;
		MediaFile*	fileHandle;
		char			loaded;
		t_Handle		dblHandle;
	};
	DynamicArray<LoadedResource>	d_files;
	bool									d_loadsPending;
};

/// Global singleton for CResourceManager
extern CResourceManager g_resourceManager;

//============================================================================

#endif //_RESOURCE_MANAGER_H

//============================================================================
