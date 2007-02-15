#include "pch.h"
#include "formattedfile.h"

#if 0

FormattedFile::FormattedFile( void )
{
	m_FileHandle = INVALID_HANDLE_VALUE;
}

FormattedFile::~FormattedFile()
{
	CloseFile();
}

BOOL FormattedFile::OpenFile( const TCHAR *pFileName, DWORD FileAccess, DWORD ShareMode, DWORD OpenMode )
{
	CloseFile();

	m_FileHandle = CreateFile( pFileName, FileAccess, ShareMode, 0, OpenMode, FILE_ATTRIBUTE_NORMAL, 0 );

	if( m_FileHandle == INVALID_HANDLE_VALUE )
		return FALSE;

	m_FileName.set( pFileName );
	m_FileAccess = FileAccess;
	m_ShareMode = ShareMode;
	m_OpenMode = OpenMode;

	return TRUE;
}

void FormattedFile::CloseFile( void )
{
	while( m_FileMarkers.Child() )
		Return();

	if( m_FileHandle != INVALID_HANDLE_VALUE )
		CloseHandle( m_FileHandle );

	m_FileHandle = INVALID_HANDLE_VALUE;
	m_FileAccess = 0;
	m_ShareMode = 0;
	m_OpenMode = 0;

	m_FileName.release();
}

BOOL FormattedFile::Read( void *pBuffer, int BufferSize )
{
	if( m_FileHandle == INVALID_HANDLE_VALUE )
		return FALSE;

	return FileRead( m_FileHandle, pBuffer, BufferSize );
}

BOOL FormattedFile::Read( MemoryBank &m )
{
	if( m_FileHandle == INVALID_HANDLE_VALUE )
		return FALSE;

	FILEMARKER fm = GetFileMarker();

	m.BeginWrite();
	m.Write( 0, fm.Length - sizeof(fm) );

	MemoryBlock *p = (MemoryBlock *) m.m_MemoryBlocks.Child();

	if( p )
		Read( p->m_pBuffer, p->m_BufferSize );

	return TRUE;
}

BOOL FormattedFile::Write( void *pBuffer, int BufferSize )
{
	if( m_FileHandle == INVALID_HANDLE_VALUE )
		return FALSE;

	return FileWrite(	m_FileHandle, pBuffer, BufferSize );
}

BOOL FormattedFile::Write( MemoryBank &m )
{
	MemoryBlock *p = (MemoryBlock *) m.m_MemoryBlocks.Child();

	while( p )
	{
		if( p->m_pBuffer )
		{
			if( !Write( p->m_pBuffer, p->m_BufferSize ) )
				return FALSE;
		}

		p = (MemoryBlock *) p->Next();
	}

	return TRUE;
}

int FormattedFile::FileSize( void )
{
	return (int) GetFileSize( m_FileHandle, 0 );
}

int FormattedFile::GetFilePosition( void )
{
	if( m_FileHandle == INVALID_HANDLE_VALUE )
		return 0;

	return GetFilePointer( m_FileHandle );
}

BOOL FormattedFile::SetFilePosition( int Position )
{
	if( m_FileHandle == INVALID_HANDLE_VALUE )
		return FALSE;

	int NewPosition = SetFilePointer( m_FileHandle, Position, 0, FILE_BEGIN );

	if( Position != NewPosition )
		return FALSE;

	return TRUE;
}

FILEMARKER FormattedFile::GetFileMarker( void )
{
	FileMarker *pFileMarker = (FileMarker *) m_FileMarkers.Child();

	if( !pFileMarker )
	{
		FILEMARKER fm;

		fm.PreMarker = 0;
		fm.Marker = 0;
		fm.Version = 0;
		fm.Length = 0;

		return fm;
	}

	return pFileMarker->m_FileMarker;
}

BOOL FormattedFile::EndOfFile( void )
{
	if( GetFilePosition() == FileSize() )
		return TRUE;

	return FALSE;
}

BOOL FormattedFile::Branch( DWORD Marker, DWORD Version )
{
	if( m_FileAccess == GENERIC_WRITE )
	{
		FileMarker *p = new FileMarker;

		if( !p )
			return FALSE;

		m_FileMarkers.SetChild( p );

		//////////////////////////////////////////////////////////////////////////

		p->m_FileMarker.PreMarker = FILEMARKERCODE;
		p->m_FileMarker.Marker = Marker;
		p->m_FileMarker.Version = Version;
		p->m_FileMarker.Length = 0;
		p->m_FilePosition = GetFilePosition();

		Write( &p->m_FileMarker, sizeof(FILEMARKER) );
	}
	else
	if( m_FileAccess == GENERIC_READ )
	{
		FileMarker *p = new FileMarker;

		if( !p )
			return FALSE;

		p->m_FilePosition = GetFilePosition();

		if( !Read( &p->m_FileMarker, sizeof(FILEMARKER) ) )
		{
			delete p;
			return FALSE;
		}

		if(p->m_FileMarker.PreMarker != FILEMARKERCODE ||
			p->m_FileMarker.Marker != Marker ||
			p->m_FileMarker.Version != Version )
		{
			SetFilePosition( p->m_FilePosition );

			delete p;
			return FALSE;
		}

		m_FileMarkers.SetChild( p );
	}

	return TRUE;
}

BOOL FormattedFile::Return( void )
{
	if( m_FileAccess == GENERIC_WRITE )
	{
		FileMarker *p = (FileMarker *) m_FileMarkers.Child();

		if( !p )
			return FALSE;

		int FilePosition = GetFilePosition();
		int Length = FilePosition - p->m_FilePosition;

		if( p->m_FileMarker.Length != Length )
		{
			p->m_FileMarker.Length = Length;

			SetFilePosition( p->m_FilePosition );
			Write( &p->m_FileMarker, sizeof(FILEMARKER) );
			SetFilePosition( FilePosition );
		}

		delete p;
	}
	else
	if( m_FileAccess == GENERIC_READ )
	{
		FileMarker *p = (FileMarker *) m_FileMarkers.Child();

		if( !p )
			return FALSE;

		SetFilePosition( p->m_FilePosition + p->m_FileMarker.Length );

		delete p;
	}
	
	return TRUE;
}
#endif //0