//============================================================================
//=
//= ResourceManager.cpp
//=
//= Loads files and unloads files when they're no longer needed.  Loads
//= are done asynchronously.
//=
//============================================================================

#include "Game/GamePCH.h"
#include "ResourceManager.h"
#if defined(NETWORKING)
#ifdef PS2
#include "Patcher/PatchExt/PatchExtractor.h"
#endif
#include "Network/Connection.h"
#endif
#include "EngineHelper/pathfile.h"

CResourceManager g_resourceManager;

//============================================================================

CResourceManager::CResourceManager() : d_files( 0, 32 )
{
	d_loadsPending = false;
}

//============================================================================

CResourceManager::~CResourceManager()
{
	Reset();
}

//============================================================================

void CResourceManager::Reset()
{
	ASSERT( d_loadsPending == 0 );
	UnloadAll();
	d_files.Reset();
}

//============================================================================

void CResourceManager::Update()
{
	// purge any un-owned data
	Purge();

	if ( d_loadsPending )
	{
		// check currently loading file
		int loadingIndex = GetLoadingIndex();
		if ( loadingIndex >= 0 )
		{
			// if still loading, get out
			t_Error error;
			if ( !d_files[ loadingIndex ].fileHandle->AwaitIO( 0, NULL, NULL, &error ) )
				return;

			// finish the load
			FinishFileLoad( loadingIndex );
		}

		// jump start next file
		StartNextFile();
	}
}

//============================================================================

void CResourceManager::FinishFileLoad( int index )
{
	ASSERT( index >= 0 && index < d_files.Count() );
	ASSERT_PTR( d_files[ index ].fileHandle );

	d_files[ index ].fileHandle->Close();
	d_files[ index ].fileHandle = NULL;

#if defined(NETWORKING)
	// apply patch
	#ifdef PS2
	if ( Media::GetPatchExtractor() )
	{
		Media::GetPatchExtractor()->SetSourceBuffer( d_files[ index ].buffer );
		int patchedSize = 0;
		if ( Media::GetPatchExtractor()->PatchExists( d_files[ index ].filename, patchedSize ) )
		{
			int allocSize = patchedSize;
			if ( IsText( d_files[ index ].filename ) )
				++allocSize;
			char* patchedBuf = (char*) memAlloc( allocSize, 64 );
			if ( Media::GetPatchExtractor()->PatchItem( patchedBuf, patchedSize ) )
			{
				memFree( d_files[ index ].buffer );
				d_files[ index ].buffer = patchedBuf;
				d_files[ index ].bufferSize = patchedSize;
			}
			else
			{
				ASSERTF( false, ( "patch of %s failed", d_files[ index ].filename ) );
				memFree( patchedBuf );
			}
		}
		Media::GetPatchExtractor()->SetSourceBuffer( NULL );
	}
	#endif
#endif
	// if this file is a DBL, go dechunkify it.
	if ( IsDBL( d_files[ index ].filename ) )
	{
		d_files[ index ].dblHandle = dblLoad( d_files[ index ].filename,
														  (u8*) d_files[ index ].buffer,
														  d_files[ index ].bufferSize );
		ASSERT( d_files[ index ].dblHandle != INVALID_HANDLE );
		dblLock( d_files[ index ].dblHandle );
	}
	else if ( IsText( d_files[ index ].filename ) )
	{
		// Null terminate!
		((char *) d_files[ index ].buffer)[d_files[ index ].bufferSize] = '\0';
	}

	d_files[ index ].loaded = true;
}

//============================================================================

void CResourceManager::StartNextFile()
{
	ASSERT( GetLoadingIndex() < 0 );

	// find the first not loaded file
	int i;
	for ( i = 0 ; i < d_files.Count() ; i++ )
		if ( !d_files[ i ].loaded )
			break;

	if ( i < d_files.Count() )
	{
		// start load
		void*      buffer;
		int        fileSize;
		int        extraAlloc = IsText(d_files[ i ].filename) ? 1 : 0;
		MediaFile* fileHandle = Media::StartAsyncLoad( d_files[ i ].filename, &buffer, &fileSize, extraAlloc );
		if ( fileHandle )
		{
			d_files[ i ].fileHandle = fileHandle;
			d_files[ i ].buffer     = buffer;
			d_files[ i ].bufferSize = fileSize;
		}
		else
		{
			// file failed loading, nuke it from the list
			d_files.Delete( i );
		}

		d_loadsPending = true;
	}
	else
	{
		d_loadsPending = false;
	}
}

//============================================================================

int CResourceManager::GetLoadingIndex()
{
	for ( int i = 0 ; i < d_files.Count() ; i++ )
		if ( d_files[ i ].fileHandle )
			return i;
	return -1;
}

//============================================================================

bool CResourceManager::IsDBL( const char* filename )
{
	const char* end = filename + strlen( filename );
	return( !stricmp( end - 4, ".dbl" ) ||
		     !stricmp( end - 4, ".dba" ) ||
			  !stricmp( end - 4, ".mdb" ) );
}

//============================================================================

bool CResourceManager::IsText( const char* filename )
{
	const char* end = filename + strlen( filename );
	return( !stricmp( end - 4, ".txt" ) ||
		     !stricmp( end - 4, ".var" ) );
}

//============================================================================

void CResourceManager::LoadFile( const char* filename, int groupId )
{
	// check first to see if this file is already in the list
	int i = FindFile( filename );
	if ( i < 0 )
	{
		t_Handle dbl = dblFindFile( filename );
		if ( dbl != INVALID_HANDLE )
		{
			if ( dblIsPermanent( dbl ) )
				return;
			ASSERTF( false, ( "%s loaded and isn't permanent", filename ) );
		}

		// add a new record for this file
		LoadedResource r;
		memset( &r, 0, sizeof(r) );

		ASSERT( strlen( filename ) + 1 <= sizeof( r.filename ) );
		strcpy( r.filename, filename );

		i = d_files.Count();
		d_files.Add( r );
	}

	// set group bit
	ASSERT( groupId >= 0 && groupId <= 31 );
	d_files[ i ].groupMask |= ( 1 << groupId );
	ASSERT( d_files[ i ].groupMask != 0 );

	// kick the load process (so it will check next frame)
	d_loadsPending = true;

	// this kicks the load process, but starts loads immediately.
//	if ( !d_loadsPending )
//	{
//		StartNextFile();
//	}
}

//============================================================================

bool CResourceManager::IsLoaded( const char* filename )
{
	int i = FindFile( filename );
	return ( i >= 0 && d_files[ i ].loaded != 0 );
}

//============================================================================

bool CResourceManager::IsLoaded( int groupId )
{
	ASSERT( groupId >= 0 && groupId <= 31 );
	int count, loaded;
	GroupInfo( &count, &loaded, 1 << groupId );
	return ( count > 0 && count == loaded );
}

//============================================================================

bool CResourceManager::IsLoadPending( const char* filename )
{
	int i = FindFile( filename );
	return ( i >= 0 && d_files[ i ].loaded == 0 );
}

//============================================================================

bool CResourceManager::IsLoadPending( int groupId )
{
	ASSERT( groupId >= 0 && groupId <= 31 );
	int count, loaded;
	GroupInfo( &count, &loaded, 1 << groupId );
	return ( count > 0 && count != loaded );
}

//============================================================================

bool CResourceManager::CancelLoads()
{
	for ( int i = d_files.Count() - 1 ; i >= 0 ; i-- )
		if ( !d_files[ i ].loaded && d_files[ i ].fileHandle == NULL )
			d_files.Delete( i );
	return GetLoadingIndex() < 0;
}

//============================================================================

void CResourceManager::GroupInfo( int* filesInGroup, int* filesInGroupLoaded, u32 groupMask )
{
	int count = 0;
	int loaded = 0;
	for ( int i = 0 ; i < d_files.Count() ; i++ )
	{
		if ( d_files[ i ].groupMask & groupMask )
		{
			count++;
			if ( d_files[ i ].loaded )
				loaded++;
		}
	}
	if ( filesInGroup       ) *filesInGroup       = count;
	if ( filesInGroupLoaded ) *filesInGroupLoaded = loaded;
}

//============================================================================

void CResourceManager::UnloadFile( int index )
{
	ASSERT( index >= 0 && index < d_files.Count() );
	ASSERT( d_files[ index ].fileHandle == NULL );

	if ( d_files[ index ].loaded )
	{
		// if this is a DBL we need to free its resources
		if ( IsDBL( d_files[ index ].filename ) )
		{
			ASSERT( d_files[ index ].dblHandle != INVALID_HANDLE );
			dblUnlock( d_files[ index ].dblHandle );
			dblRemove( d_files[ index ].dblHandle );

			// DBL files will log into the texture cache, so we need to reset it
			TextureMgr::Cache().Reset();
		}
		else
		{
			ASSERT_PTR( d_files[ index ].buffer );
			memFree( d_files[ index ].buffer );
		}
	}

	d_files.Delete( index );

	// if we're empty, free our space
	if ( d_files.Count() == 0 )
		d_files.Reset();
}

//============================================================================

void CResourceManager::UnloadGroup( int groupId, bool purge /*=true*/ )
{
	ASSERT( groupId >= 0 && groupId <= 31 );
	u32 groupBit = ( 1 << groupId );
	for ( int i = d_files.Count() - 1 ; i >= 0 ; i-- )
	{
		if ( d_files[ i ].groupMask & groupBit )
		{
			d_files[ i ].groupMask &= ~groupBit;
			if ( d_files[ i ].groupMask == 0 )
			{
				// we purge it now if its not loading, otherwise we'll clean
				// it up in the Update routine.
				if ( purge && d_files[ i ].fileHandle == NULL )
					UnloadFile( i );
			}
		}
	}
}

//============================================================================

void CResourceManager::Purge()
{
	for ( int i = d_files.Count() - 1 ; i >= 0 ; i-- )
		if ( d_files[ i ].groupMask == 0 && d_files[ i ].fileHandle == NULL )
			UnloadFile( i );
}

//============================================================================

void CResourceManager::UnloadAll()
{
	for ( int i = d_files.Count() - 1 ; i >= 0 ; i-- )
		UnloadFile( i );
}

//============================================================================

void* CResourceManager::FileBuffer( const char* filename, int *size )
{
	int i = FindFile( filename );
	if ( i >= 0 && d_files[ i ].loaded )
	{
		if ( size ) *size = d_files[ i ].bufferSize;
		return d_files[ i ].buffer;
	}
	else
	{
		if ( size ) *size = 0;
		return  NULL;
	}
}

//============================================================================

int CResourceManager::FileSize( const char* filename )
{
	int i = FindFile( filename );
	return ( i >= 0 && d_files[ i ].loaded ) ? d_files[ i ].bufferSize : 0;
}

//============================================================================

t_Handle CResourceManager::FileHandle( const char* filename )
{
	int i = FindFile( filename );
	return ( i >= 0 && d_files[ i ].loaded ) ? d_files[ i ].dblHandle : INVALID_HANDLE;
}

//============================================================================

int CResourceManager::FindFile( const char* filename )
{
	for ( int i = 0 ; i < d_files.Count() ; i++ )
		if ( PathFile::FilenamesMatch( d_files[ i ].filename, filename ) )
			return i;
	return -1;
}

//============================================================================

float CResourceManager::GetLoadedPercentage()
{
	// TODO: should do this on file sizes, but use file count for now.
	if ( d_files.Count() > 0 )
	{
		int loaded  = 0;
		for ( int i = 0 ; i < d_files.Count() ; i++ )
			if ( d_files[ i ].loaded )
				loaded++;
		return (float)loaded / (float)d_files.Count() * 100.0f;
	}
	else
	{
		return 100.0f;
	}
}

//============================================================================

void CResourceManager::WaitUntilIdle()
{
	// block if we're busy
	while ( d_loadsPending )
	{
		Update();
#if defined(NETWORKING)
		if (ValidSingleton(CConnection))
			g_connection.Service();
#endif
	}
}

//============================================================================
