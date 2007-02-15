#include "pch.h"
#include "imageexplorer.h"

DirectoryImage::DirectoryImage( void )
{
}

DirectoryImage::~DirectoryImage()
{
	Destroy();
}

void DirectoryImage::Destroy( void )
{
	m_Picture.Destroy();
}

BOOL DirectoryImage::LoadPicture( int PreviewWidth, int PreviewHeight )
{
	m_Picture.SetFilePath( m_FilePath.get() );

	if( !m_Picture.LoadPicture() )
		return FALSE;

	HBITMAP hbm = PictureHandle();

	int BPP = GetBitmapBPP( hbm );

	if( hbm && BPP >= 16)
	{
		/*
		if( BPP == 24 )
		{
			int Width = 8192;
			int Height = 8192;
			BITMAP2* p1 = CreateBitmap2( Width, Height, PIXELFMT_R8G8B8, 0, 0 );
			BITMAP2* p2 = CreateBitmap2( Width, Height, PIXELFMT_B8G8R8, 0, 0 );

			BeginBitmap( 4, p1 );
			BeginBitmap( 5, p2 );

		//	int Width = GetBitmapWidth( hbm );
		//	int Height = GetBitmapHeight( hbm );
			int x, y;

			__int64 s1 = CpuTimeStamp();
			__int64 s2;
			__int64 s3;
			
			for( y = 0; y < Height; y++ )
				for( x = 0; x < Width; x++ )
					GxSetPixel( 4, x, y, GxGetPixel( 5, x, y ) );

			s2 = CpuTimeStamp();

			s3 = s2 - s1;

			double pps = (double) s3 / (double) (Width*Height);

			AfxMessageBox( Printf( "%f", pps ) );

			FinishBitmap(4);
			FinishBitmap(5);

			DeleteBitmap2( p1 );
			DeleteBitmap2( p2 );
		}
		else
		*/
		{
			BeginDIB( 4, hbm );

			int Width = GetBitmapWidth( hbm );
			int Height = GetBitmapHeight( hbm );
			int x, y;
			int u, v;
			COLOR32 c, d;

			COLOR32 pattern[2];

			pattern[0] = 0xffaaffaa;
			pattern[1] = 0xff88dd88;

			for( y = 0; y < Height; y++ )
			{
				for( x = 0; x < Width; x++ )
				{
					u = x >> 3;
					v = y >> 3;

					c = GxGetPixel( 4, x, y );
					d = pattern[(u+v)&1];

					GxSetPixel( 4, x, y, ColorBlend( d, c, c >> 24) );
				}
			}

			FinishBitmap(4);
		}
	}

	m_Picture.CreatePreview( PreviewWidth, PreviewHeight );

	return TRUE;
}

HBITMAP DirectoryImage::PictureHandle( void )
{
	return m_Picture.PictureHandle();
}

HBITMAP DirectoryImage::PreviewHandle( void )
{
	return m_Picture.PreviewHandle();
}

IMPLEMENT_DYNCREATE( ImageExplorer, FileExplorer );

ImageExplorer::ImageExplorer( void )
{
	PreviewSettings( 64, 64 );
}

ImageExplorer::~ImageExplorer()
{
}

void ImageExplorer::PreviewSettings( int Width, int Height, BOOL Antialiased )
{
	m_PreviewWidth = Width;
	m_PreviewHeight = Height;
	m_AntialiasedPreview = Antialiased;
}

DirectoryImage * ImageExplorer::GetDirectoryImage( int Index )
{
	return (DirectoryImage *) GetDirectoryFile( Index );
}

DirectoryFile * ImageExplorer::NewDirectoryFile( void )
{
	return new DirectoryImage;
}

BOOL ImageExplorer::ProcessFile( DirectoryFile *pFile )
{
	if( !FileExplorer::ProcessFile( pFile ) )
		return FALSE;

	DirectoryImage *pImage = (DirectoryImage *) pFile;

	if( !pImage->LoadPicture( m_PreviewWidth, m_PreviewHeight ) )
		return FALSE;

	PostMessageToAllViews( FXN_DIRECTORYCHANGED, 0, 0 );

	return TRUE;
}

void ImageExplorer::FinishDirectoryListing( DirectoryFile *pFileList )
{
}