#include "hud/HudPCH.h"
#include "hud/formattedfile.h"

FormattedFile::FormattedFile( void )
{
}

FormattedFile::~FormattedFile()
{
	CloseFile();
}

bool FormattedFile::OpenFile( const char* pFileName, FileStream::OPENMODE OpenMode )
{
	CloseFile();

	if( !m_FileStream.Open( (char*) pFileName, OpenMode ) )
		return false;

	m_FileName.set( pFileName );
	m_OpenMode = OpenMode;

	return true;
}

void FormattedFile::CloseFile( void )
{
	while( m_FileMarkers.Child() )
		Return();

	m_FileStream.Close();
	m_FileName.release();
}

bool FormattedFile::Read( void *pBuffer, int BufferSize )
{
	int Size = m_FileStream.Read( pBuffer, BufferSize );

	if( Size != BufferSize )
		return false;

	return true;
}

bool FormattedFile::Read( MemoryBank &m )
{
	FILEMARKER fm = GetFileMarker();

	m.BeginWrite();
	m.Write( 0, fm.Length - sizeof(fm) );

	MemoryBlock *p = (MemoryBlock *) m.m_MemoryBlocks.Child();

	if( !p )
		return false;

	return Read( p->m_pBuffer, p->m_BufferSize );
}

bool FormattedFile::Write( void *pBuffer, int BufferSize )
{
	return m_FileStream.Write( pBuffer, BufferSize );
}

bool FormattedFile::Write( MemoryBank &m )
{
	MemoryBlock *p = (MemoryBlock *) m.m_MemoryBlocks.Child();

	while( p )
	{
		if( p->m_pBuffer )
		{
			if( !Write( p->m_pBuffer, p->m_BufferSize ) )
				return false;
		}

		p = (MemoryBlock *) p->Next();
	}

	return true;
}

int FormattedFile::FileSize( void )
{
	return m_FileStream.Size();
}

int FormattedFile::GetFilePosition( void )
{
	return m_FileStream.Tell();
}

bool FormattedFile::SetFilePosition( int Position )
{
	m_FileStream.Seek( Position );

	if( Position != m_FileStream.Tell() )
		return false;

	return true;
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

bool FormattedFile::EndOfFile( void )
{
	if( GetFilePosition() == FileSize() )
		return true;

	return false;
}

bool FormattedFile::Branch( unsigned int Marker, unsigned int Version )
{
	if( m_OpenMode == FileStream::WRITE )
	{
		FileMarker *p = new FileMarker;

		if( !p )
			return false;

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
	if( m_OpenMode == FileStream::READ )
	{
		FileMarker *p = new FileMarker;

		if( !p )
			return false;

		p->m_FilePosition = GetFilePosition();

		if( !Read( &p->m_FileMarker, sizeof(FILEMARKER) ) )
		{
			delete p;
			return false;
		}

		if(p->m_FileMarker.PreMarker != FILEMARKERCODE ||
			p->m_FileMarker.Marker != Marker ||
			p->m_FileMarker.Version != Version )
		{
			SetFilePosition( p->m_FilePosition );

			delete p;
			return false;
		}

		m_FileMarkers.SetChild( p );
	}

	return true;
}

bool FormattedFile::Return( void )
{
	if( m_OpenMode == FileStream::WRITE )
	{
		FileMarker *p = (FileMarker *) m_FileMarkers.Child();

		if( !p )
			return false;

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
	if( m_OpenMode == FileStream::READ )
	{
		FileMarker *p = (FileMarker *) m_FileMarkers.Child();

		if( !p )
			return false;

		SetFilePosition( p->m_FilePosition + p->m_FileMarker.Length );

		delete p;
	}
	
	return true;
}