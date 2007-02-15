#ifndef _VIEW_H_
#define _VIEW_H_

#include <fileview.h>
#include "document.h"
#include "dibblefile.h"

class UndibbleView : public CView
{
	DECLARE_DYNCREATE( UndibbleView );

	public:
		UndibbleView();
		virtual ~UndibbleView();

		//////////////////////////////////////////////////////////////////////////
		
		UndibbleDoc* GetDocument( void );
		RECT GetFileViewRect( void );
		RECT GetThumbnailRect( void );
		void SetFileExplorer( FileExplorer* pFileExplorer );
		FileExplorer* GetFileExplorer( void );
		DirectoryFile* GetFile();

		//////////////////////////////////////////////////////////////////////////
		
		BOOL LoadMyDibble( char *pFilePath );

	protected:
		//////////////////////////////////////////////////////////////////////////

		BOOL CreateFileView( void );
		BOOL CreateThumbnailView( void );

		FileView* m_pFileView;
		ThumbnailView* m_pThumbnailView;
		int m_IconSize;
		int m_IconRows;
		int m_IconBorder;

		virtual void OnDraw( CDC *pdc );
		
	protected:
		afx_msg int OnCreate( LPCREATESTRUCT pcs );
		afx_msg BOOL OnEraseBkgnd( CDC *pdc );
		afx_msg void OnMouseMove( UINT Flags, CPoint p );
		afx_msg void OnSize( UINT Type, int x, int y );
		afx_msg LRESULT OnDirectoryChanged( WPARAM wParam, LPARAM lParam );
		afx_msg void OnFileChanged( NMHDR *pNotify, LRESULT *pResult );

	protected:
		DECLARE_MESSAGE_MAP();
};

#endif // _VIEW_H_