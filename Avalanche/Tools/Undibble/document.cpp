#include "pch.h"
#include "application.h"
#include "document.h"
#include "mainframe.h"

IMPLEMENT_DYNCREATE( UndibbleDoc, CDocument );

BEGIN_MESSAGE_MAP( UndibbleDoc, CDocument )
END_MESSAGE_MAP()

//=============================================================================
// UndibbleDoc::UndibbleDoc
//=============================================================================

UndibbleDoc::UndibbleDoc()
{
}

//=============================================================================
// UndibbleDoc::~UndibbleDoc
//=============================================================================

UndibbleDoc::~UndibbleDoc()
{
	Destroy();
}

//=============================================================================
// UndibbleDoc::Destroy
//=============================================================================

void UndibbleDoc::Destroy( void )
{
	return;
}

//=============================================================================
// UndibbleDoc::OnNewDocument
//=============================================================================

BOOL UndibbleDoc::OnNewDocument()
{
	if( !CDocument::OnNewDocument() )
		return FALSE;

	return TRUE;
}

//=============================================================================
// UndibbleDoc::OnOpenDocument
//=============================================================================

BOOL UndibbleDoc::OnOpenDocument( LPCTSTR pPathName )
{
	m_pMyDibble = dm.LoadDibble( pPathName );

	if( !m_pMyDibble )
		return FALSE;

	UpdateThumbnailView();

	return TRUE;
}

//=============================================================================
// UndibbleDoc::OnSaveDocument
//=============================================================================

BOOL UndibbleDoc::OnSaveDocument( LPCTSTR pPathName )
{
	if( m_pMyDibble )
	{
		if( pPathName )
		{
			HANDLE hFile = FileCreate( pPathName );

			if( hFile != INVALID_HANDLE_VALUE )
			{
				UNDIBBLEFILEHEADER fh;
				UNDIBBLEIMAGEHEADER ih;

				BITMAP2 *pbm;
				int c;

				c = m_pMyDibble->m_TextureCount;

				fh.marker[0] = 'u';
				fh.marker[1] = 'n';
				fh.marker[2] = 'd';
				fh.marker[3] = 'i';
				fh.marker[4] = 'b';
				fh.marker[5] = 'b';
				fh.marker[6] = 'l';
				fh.marker[7] = 'e';
				fh.count = c;

				FileWrite( hFile, &fh, sizeof(fh) );

		//////////////////////////////////////////////////////////////////////////
		//		Write this data to the file if you want the raw texture chunk info
		//		FileWrite( hFile, m_pMyDibble->m_pChunkData, m_pMyDibble->m_ChunkSize );
		//////////////////////////////////////////////////////////////////////////

				DWORD *offsets = new DWORD[c];

				if( offsets )
				{
					DWORD FilePosition = GetFilePointer( hFile );

					FileWrite( hFile, offsets, sizeof(DWORD)*c );

					for( int i = 0; i < c; i++ )
					{
						pbm = m_pMyDibble->m_ppTextures[i];

						if( pbm )
						{
							ih.width  = (DWORD) pbm->Width;
							ih.height = (DWORD) pbm->Height;
							ih.format = (DWORD) pbm->Format;
							ih.stride = (DWORD) pbm->Stride;
							ih.bpp = PixelFormatToBPP( pbm->Format );

							ih.imagesize = ih.stride * ih.height;

							if( ih.bpp == 4 )
								ih.palettesize = sizeof(DWORD)*16;
							else
							if( ih.bpp == 8 )
								ih.palettesize = sizeof(DWORD)*256;
							else
								ih.palettesize = 0;
						}

						offsets[i] = GetFilePointer( hFile );

						FileWrite( hFile, &ih, sizeof(ih) );
						FileWrite( hFile, pbm->Palette, ih.palettesize );
						FileWrite( hFile, pbm->pBits, ih.imagesize );
					}

					FileWrite( hFile, offsets, sizeof(DWORD)*c, FilePosition );
				}

				FileClose( hFile );
			}
		}
	}

	return TRUE;
}

//=============================================================================
// UndibbleDoc::UpdateThumbnailView
//=============================================================================

void UndibbleDoc::UpdateThumbnailView( void )
{
	if( m_pThumbnailView )
	{
		m_pThumbnailView->Clear();

		if( m_pMyDibble )
		{
			int Count = m_pMyDibble->m_TextureCount;
			BITMAP2 *pbm;
			BITMAP2 bm2;
			HBITMAP hbm;
			int w, h;
			int BPP;

			for( int i = 0; i < Count; i++ )
			{
				pbm = m_pMyDibble->m_ppTextures[i];

				if( pbm )
				{
					w = pbm->Width;
					h = pbm->Height;
					BPP = PixelFormatToBPP( pbm->Format );

					hbm = CreateDIB( w, h, BPP );

					if( hbm )
					{
						if( BPP <= 8 )
							GdiSetColorTable( hbm, (RGBQUAD*) pbm->Palette, 256 );

						bm2 = GetBitmap2Struct( hbm, FALSE );

						BitmapToBitmap( &bm2, 0, 0, w, h, pbm, 0, 0, w, h );

						m_pThumbnailView->AddPicture( hbm, i );
					}

					ReleaseDIB( hbm );
				}
			}
		}

		m_pThumbnailView->InvalidateRect( 0, TRUE );
	}

	return;
	return;
}