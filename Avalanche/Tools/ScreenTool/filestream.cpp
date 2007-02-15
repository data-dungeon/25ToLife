#include "pch.h"
#include "filestream.h"

//=============================================================================
// FileManager::FileManager
//=============================================================================

FileManager::FileManager( void )
{
	for( int i = 0; i < MAX_FILE_HANDLES; i++ )
	{
		m_FileHandles[i].Handle = INVALID_HANDLE_VALUE;
		m_FileHandles[i].pStream = 0;
	}

	return;
}

//=============================================================================
// FileManager::~FileManager
//=============================================================================

FileManager::~FileManager()
{
	CloseAllFiles();
}

//=============================================================================
// FileManager::CloseAllFiles
//=============================================================================

void FileManager::CloseAllFiles( void )
{
	for( int i = 0; i < MAX_FILE_HANDLES; i++ )
	{
		if( m_FileHandles[i].Handle != INVALID_HANDLE_VALUE )
			CloseFile( &m_FileHandles[i] );
	}

	return;
}

//=============================================================================
// FileManager::RegisterFile
//=============================================================================

FILESTREAMHANDLE * FileManager::RegisterFile( HANDLE FileHandle, FileStream *pStream )
{
	FILESTREAMHANDLE * pStreamHandle = FileStreamHandle( pStream );

	if( pStreamHandle )
	{
		if( pStreamHandle->Handle != INVALID_HANDLE_VALUE &&
			pStreamHandle->Handle != FileHandle )
		{
			CloseHandle( pStreamHandle->Handle );
		}

		pStreamHandle->Handle = FileHandle;
	}
	else
	{
		pStreamHandle = FileStreamHandle( INVALID_HANDLE_VALUE );

		if( pStreamHandle )
		{
			pStreamHandle->Handle = FileHandle;
			pStreamHandle->pStream = pStream;
		}
	}

	return pStreamHandle;
}

//=============================================================================
// FileManager::FileStreamHandle
//=============================================================================

FILESTREAMHANDLE * FileManager::FileStreamHandle( HANDLE FileHandle )
{
	for( int i = 0; i < MAX_FILE_HANDLES; i++ )
	{
		if( m_FileHandles[i].Handle == FileHandle )
			return &m_FileHandles[i];
	}

	return 0;
}

//=============================================================================
// FileManager::FileStreamHandle
//=============================================================================

FILESTREAMHANDLE * FileManager::FileStreamHandle( FileStream *pStream )
{
	if( pStream )
	{
		for( int i = 0; i < MAX_FILE_HANDLES; i++ )
		{
			if( m_FileHandles[i].pStream == pStream )
				return &m_FileHandles[i];
		}
	}

	return 0;
}

//=============================================================================
// FileManager::AsFileHandle
//=============================================================================

HANDLE FileManager::AsFileHandle( FileStream *pStream )
{
	FILESTREAMHANDLE * pStreamHandle = FileStreamHandle( pStream );

	if( pStreamHandle )
		return pStreamHandle->Handle;

	return INVALID_HANDLE_VALUE;
}

//=============================================================================
// FileManager::AsFileStream
//=============================================================================

FileStream * FileManager::AsFileStream( HANDLE FileHandle )
{
	FILESTREAMHANDLE * pStreamHandle = FileStreamHandle( FileHandle );

	if( pStreamHandle )
		return pStreamHandle->pStream;

	return 0;
}

//=============================================================================
// FileManager::CloseFile
//=============================================================================

void FileManager::CloseFile( FILESTREAMHANDLE *pStreamHandle )
{
	if( pStreamHandle )
	{
		if( pStreamHandle->Handle != INVALID_HANDLE_VALUE )
			CloseHandle( pStreamHandle->Handle );

		pStreamHandle->Handle = INVALID_HANDLE_VALUE;
		pStreamHandle->pStream = 0;
	}

	return;
}

//=============================================================================
// FileManager::CloseFile
//=============================================================================

void FileManager::CloseFile( FileStream *pStream )
{
	CloseFile( FileStreamHandle( pStream ) );
}

//=============================================================================
// FileManager::CloseFile
//=============================================================================

void FileManager::CloseFile( HANDLE FileHandle )
{
	CloseFile( FileStreamHandle( FileHandle ) );
}

FileManager fm;

//=============================================================================
// FileStream::FileStream
//=============================================================================

FileStream::FileStream( void )
{
}

//=============================================================================
// FileStream::~FileStream
//=============================================================================

FileStream::~FileStream( void )
{
	Close();
}

//=============================================================================
// FileStream::Open
//=============================================================================

bool FileStream::Open( const char *pFileName, OPENMODE AccessMode, bool TextMode )
{
	TCHAR szFilePath[_MAX_PATH];

	sprintf( szFilePath, "data\\%s", pFileName );

	HANDLE FileHandle = INVALID_HANDLE_VALUE;
	DWORD dwAccessMode = GENERIC_READ;
	DWORD dwShareMode = FILE_SHARE_READ;
	DWORD dwOpenMode = OPEN_EXISTING;
	DWORD dwAttributes = 0;

	if( AccessMode == WRITE )
	{
		dwAccessMode = GENERIC_WRITE;
		dwShareMode = 0;
		dwOpenMode = CREATE_ALWAYS;
		dwAttributes = FILE_ATTRIBUTE_NORMAL;
	}

	FileHandle = CreateFile( pFileName, dwAccessMode, dwShareMode, 0, dwOpenMode, dwAttributes, 0 );

	if( FileHandle == INVALID_HANDLE_VALUE )	
	{
		FileHandle = CreateFile( szFilePath, dwAccessMode, dwShareMode, 0, dwOpenMode, dwAttributes, 0 );

		if( FileHandle == INVALID_HANDLE_VALUE )
			return false;
	}

	FILESTREAMHANDLE *pStreamHandle = fm.RegisterFile( FileHandle, this );

	if( pStreamHandle )
		return true;

	CloseHandle( FileHandle );

	return false;
}

//=============================================================================
// FileStream::Size
//=============================================================================

int FileStream::Size( void )
{
	HANDLE FileHandle = fm.AsFileHandle( this );

	if( FileHandle == INVALID_HANDLE_VALUE )
		return 0;
	
	return (int) GetFileSize( FileHandle, 0 );
}

//=============================================================================
// FileStream::Read
//=============================================================================

int FileStream::Read( void *data, int length )
{
	DWORD dwBytesRead = 0;
	HANDLE FileHandle = fm.AsFileHandle( this );

	if( FileHandle != INVALID_HANDLE_VALUE )
		ReadFile( FileHandle, data, length, &dwBytesRead, 0 );

	return dwBytesRead;
}

//=============================================================================
// FileStream::Write
//=============================================================================

bool FileStream::Write( const void *buf, int length )
{
	HANDLE FileHandle = fm.AsFileHandle( this );

	if( FileHandle == INVALID_HANDLE_VALUE )
		return false;

	if( !FileWrite( FileHandle, (void*) buf, length ) )
		return false;

	return true;
}

//=============================================================================
// FileStream::Seek
//=============================================================================

bool FileStream::Seek( int seekTo, SEEKMODE mode )
{
	HANDLE FileHandle = fm.AsFileHandle( this );

	if( FileHandle == INVALID_HANDLE_VALUE )
		return false;

	DWORD dwMode = FILE_BEGIN;

	switch( mode )
	{
	case FileStream::CUR:
		dwMode = FILE_CURRENT;
		break;
	case FileStream::END:
		dwMode = FILE_END;
		break;
	case FileStream::SET:
		dwMode = FILE_BEGIN;
		break;
	}

	SetFilePointer( FileHandle, seekTo, 0, dwMode );

	return true;
}

//=============================================================================
// FileStream::Tell
//=============================================================================

int FileStream::Tell( void )
{
	HANDLE FileHandle = fm.AsFileHandle( this );

	if( FileHandle == INVALID_HANDLE_VALUE )
		return 0;

	return (int) GetFilePointer( FileHandle );
}

//=============================================================================
// FileStream::Close
//=============================================================================

void FileStream::Close( void )
{
	fm.CloseFile( this );
}
