#include "pch.h"
#include "fileinfo.h"

FileNameEx::FileNameEx( void )
{
}

FileNameEx::FileNameEx( const char *pFileName )
{
	Parse( pFileName );
}

void FileNameEx::SetFileName( const char *pFileName )
{
	Parse( pFileName );
}

void FileNameEx::Parse( const char *pFileName )
{
	m_FullPath.set( pFileName );
	m_FileName.getfilename( pFileName );
	m_FileExt.getext( pFileName );
	m_FileTitle.delext( m_FileName.get() );
	m_FilePath.filepath( pFileName );
}