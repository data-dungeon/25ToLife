#ifndef _IMAGEEXPLORER_H_
#define _IMAGEEXPLORER_H_

#include "fileexplorer.h"
#include "picture.h"

class DirectoryImage : public DirectoryFile
{
	public:
		DirectoryImage( void );
		virtual ~DirectoryImage();

		virtual void Destroy( void );
		virtual BOOL LoadPicture( int PreviewWidth, int PreviewHeight );
		virtual HBITMAP PictureHandle( void );
		virtual HBITMAP PreviewHandle( void );

	public:
		Picture m_Picture;
};

class ImageExplorer : public FileExplorer
{
	DECLARE_DYNCREATE( ImageExplorer );

	public:
		ImageExplorer( void );
		virtual ~ImageExplorer();

		void PreviewSettings( int Width, int Height, BOOL Antialiased = FALSE );
		DirectoryImage * GetDirectoryImage( int Index );

	public:
		virtual DirectoryFile * NewDirectoryFile( void );
		virtual BOOL ProcessFile( DirectoryFile *pFile );
		virtual void FinishDirectoryListing( DirectoryFile *pFileList );

	public:
		int m_PreviewWidth;
		int m_PreviewHeight;
		BOOL m_AntialiasedPreview;
};

#endif // _IMAGEXPLORER_H_