#include "pch.h"
#include "picture.h"

Picture::Picture( void )
{
	m_hPicture = 0;
	m_hPreview = 0;
}

Picture::~Picture()
{
	Destroy();
}

void Picture::SetFilePath( TCHAR *pFilePath )
{
	m_FilePath.set( pFilePath );
}

BOOL Picture::LoadPicture( void )
{
	Release();

	TCHAR *pFilePath = m_FilePath.get();

	TCHAR szType[16];
	strcpy( szType, &pFilePath[strlen(pFilePath)-4] );

	if(tlower(szType[0]) == '.' &&
		tlower(szType[1]) == 'b' &&
		tlower(szType[2]) == 'm' &&
		tlower(szType[3]) == 'p' )
	{
		m_hPicture = (HBITMAP) LoadImage( NULL, m_FilePath.get(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE|LR_CREATEDIBSECTION );
	}
	else
	if(tlower(szType[0]) == '.' &&
		tlower(szType[1]) == 't' &&
		tlower(szType[2]) == 'g' &&
		tlower(szType[3]) == 'a' )
	{
		m_hPicture = LoadTGA( m_FilePath.get() );
	}

	if( !m_hPicture )
		return FALSE;

	return TRUE;
}

BOOL Picture::SavePicture(	void )
{
	return SavePicture( m_FilePath.get() );
}

BOOL Picture::SavePicture( TCHAR *pFilePath )
{
	return SaveDIB( pFilePath, m_hPicture, 0 );
}

BOOL Picture::CreatePreview( int Width, int Height )
{
	HBITMAP hbm = m_hPicture;

	if( !hbm )
	{
		hbm = (HBITMAP) LoadImage( NULL, m_FilePath.get(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE|LR_CREATEDIBSECTION );

		if( !hbm )
			return FALSE;
	}

	if( m_hPreview )
	{
		BITMAP bm = GetBitmapStruct( m_hPreview );

		if( bm.bmWidth != Width || bm.bmHeight != Height )
			m_hPreview = ReleaseDIB( m_hPreview );
	}

	if( !m_hPreview )
	{
		m_hPreview = CreateDIB( Width, Height, 32 );

		if( !m_hPreview )
			return FALSE;
	}

	CopyBitmapPreserveAspectRatio( m_hPreview, 0, 0, Width, Height, hbm, 0, 0, GetBitmapWidth( hbm ), GetBitmapHeight( hbm ) );

	if( hbm != m_hPicture )
		hbm = ReleaseDIB( hbm );

	return TRUE;
}

void Picture::Release( void )
{
	if( m_hPicture )
		m_hPicture = ReleaseDIB( m_hPicture );

	if( m_hPreview )
		m_hPreview = ReleaseDIB( m_hPreview );

	return;
}

void Picture::Destroy( void )
{
	Release();
	SetFilePath(0);
}