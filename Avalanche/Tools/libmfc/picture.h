#ifndef _PICTURE_H_
#define _PICTURE_H_

#include <afxwin.h>

class Picture
{
	public:
		Picture( void );
		virtual ~Picture();

		virtual void SetFilePath( TCHAR *pFilePath );
		virtual BOOL LoadPicture( void );
		virtual BOOL SavePicture( void );
		virtual BOOL SavePicture( TCHAR *pFilePath );
		virtual BOOL CreatePreview( int Width, int Height );
		virtual void Release( void );
		virtual void Destroy( void );
		
		HBITMAP PictureHandle( void ) { return m_hPicture; }
		HBITMAP PreviewHandle( void ) { return m_hPreview; }

	public:
		tstring m_FilePath;
		HBITMAP m_hPicture;
		HBITMAP m_hPreview;
};

#endif // _PICTURE_H_