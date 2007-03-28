// ----------------------------------------------------------------------------
// File				-> CDllLoader.cpp
// Created			-> Dec 22, 2004
// Author			-> Dwain Skinner
// Description		-> SN DLL loader implementation
// ----------------------------------------------------------------------------
#if defined(NETWORKING)
#include "platform/PlatformPCH.h"
#include "CDllLoader.h"
#include "Layers/Media.h"
#include <eekernel.h>
#include <sifdev.h>
#include <libsn.h>

// --- our singleton
CDllLoader g_DllLoader;

// ----------------------------------------------------------------------------

#if defined(CONSUMER_BUILD)
#define DLL_DIRECTORY "rel/consumer"
#elif defined(CDROM) && defined(_DEBUG)
#define DLL_DIRECTORY "rel/debugcdrom"
#elif defined(CDROM)
#define DLL_DIRECTORY "rel/cdrom"
#elif defined(_DEBUG)
#define DLL_DIRECTORY "rel/debug"
#else
#define DLL_DIRECTORY "rel/release"
#endif

// ----------------------------------------------------------------------------
CDllLoader::CDllLoader( )
{
	m_memUsed	= 0;
	m_numDLL		= 0;
	memset( m_loadedDLLs, 0, sizeof( m_loadedDLLs ) );
}

// ----------------------------------------------------------------------------
CDllLoader::~CDllLoader( )
{
}

// ----------------------------------------------------------------------------
bool CDllLoader::Load( const char* filename, bool trayCheck )
{
	bool success = false;

	// --- should we look for open tray errors
	MediaHAL::ShouldCheckTray( trayCheck );

	if ( !IsLoaded( filename ) )
	{
#ifdef CDROM
		if( trayCheck )
		{
			sceCdSync(0x10);						// Wait for completion of any other CD commands..BLOCKING
			int temp = sceCdGetDiskType();
			if( temp == SCECdNODISC )			// No-Disc, Tray opened
			{
				MediaHAL::ShouldCheckTray( false );
				return( false );
			}
		}
#endif

		char path[ 64 ];
		sprintf( path, "%s/%s", DLL_DIRECTORY, filename );

		s32 size;
		t_Error error;
		memSetAllocMethod( ALC_BTM );
		memSetFitMethod( FIT_FIRST );
		void* buf = Media::Load( path, &size, &error );
		MediaHAL::ShouldCheckTray( false );
		memSetAllocMethod( ALC_TOP );
		memSetFitMethod( FIT_LAST );

		if ( buf )
		{
			FlushCache(0);
			if( ( m_lastError = snDllLoaded( buf, 0 ) ) == SN_DLL_SUCCESS )
			{
				// release the relocation table
				int keepSize = (int)snDllGetFreeMemory( buf ) - (int)buf;
				ASSERT( keepSize > 0 && keepSize <= size );
				memShrink( buf, keepSize );

				AddLoaded( filename, buf, keepSize );
				g_console.Echo( "loaded DLL->%s (%d rel, %d kept, %d saved)\n", path, size, keepSize, size - keepSize );
				success = true;
			}
			else
			{
				ASSERTF( false, ( "snDllLoaded failed on %s, error:%d", filename, m_lastError ) );
				memFree( buf );
			}
		}
		else
		{
			ASSERTF( false, ( "unable to load DLL '%s'", path ) );
		}
	}
	else
	{
		success = true;
	}

	return success;
}

// ----------------------------------------------------------------------------
bool CDllLoader::Unload( const char* filename )
{
	int index = GetLoadedIndex( filename );
	if ( index >= 0 )
	{
		snDllUnload( m_loadedDLLs[ index ].addr );
		memFree( m_loadedDLLs[ index ].addr );
		RemoveLoaded( index );
		return true;
	}
	else
	{
		ASSERTF( false, ( "DLL Unload '%s' failed - not loaded", filename ) );
		return false;
	}
}

// ----------------------------------------------------------------------------
bool CDllLoader::IsLoaded( const char* filename )
{
	return GetLoadedIndex( filename ) >= 0;
}

// ----------------------------------------------------------------------------
int CDllLoader::GetLoadedIndex( const char* filename )
{
	for ( int i = 0 ; i < m_numDLL ; i++ )
		if ( stricmp( filename, m_loadedDLLs[ i ].name ) == 0 )
			return i;
	return -1;
}

// ----------------------------------------------------------------------------
void CDllLoader::AddLoaded( const char* filename, void* addr, u32 size )
{
	ASSERT( m_numDLL < DLL_MAX_LOADED );
	ASSERT( strlen( filename ) < DLL_MAX_NAME_LEN );
	strcpy( m_loadedDLLs[ m_numDLL ].name, filename );
	m_loadedDLLs[ m_numDLL ].addr = addr;
	m_loadedDLLs[ m_numDLL ].size = size;
	m_numDLL++;
	m_memUsed += size;
}

// ----------------------------------------------------------------------------
void CDllLoader::RemoveLoaded( int index )
{
	m_memUsed -= m_loadedDLLs[ index ].size;
	m_numDLL--;
	while ( index < m_numDLL )
		m_loadedDLLs[ index ] = m_loadedDLLs[ index + 1 ];
	memset( &m_loadedDLLs[ index ], 0, sizeof( m_loadedDLLs[ index ] ) );
}

// ----------------------------------------------------------------------------
int CDllLoader::GetLoadedDLLs( LoadedDLLInfo** info, u32* totalSize )
{
	if ( info )			*info			= &m_loadedDLLs[ 0 ];
	if ( totalSize )	*totalSize	= m_memUsed;
	return m_numDLL;
}

// ----------------------------------------------------------------------------
#endif