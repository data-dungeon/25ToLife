#ifndef _THUMBNAIL_H_
#define _THUMBNAIL_H_

#include <afxwin.h>
#include "imageexplorer.h"

typedef struct
{
	NMHDR nmhdr;
	int SelectedItem;
}
THUMBNAILNOTIFYSTRUCT;

#define TNN_SELECTIONCHANGED (0x100)
#define TNN_HOVERCHANGED	  (0x101)

class ThumbnailPic : public CTreeNode
{
	public:
		ThumbnailPic( void );
		~ThumbnailPic( void );
		HBITMAP m_hbmPic;
};

class ThumbnailView : public CScrollView
{
	DECLARE_DYNCREATE( ThumbnailView );

	public:
		ThumbnailView( void );
		virtual ~ThumbnailView();

		//////////////////////////////////////////////////////////////////////////
		
		ImageExplorer *GetImageExplorer( void );
		void SetImageExplorer( ImageExplorer *pImageExplorer );
		void SetThumbnailSize( int Width, int Height );
		void SetWindowColor( COLOR32 c );
		BOOL AddPicture( HBITMAP hbm, int Id );
		void Clear( void );
		
		//////////////////////////////////////////////////////////////////////////
		
		void Select( int Index );
		void Hover( int Index );
		
		//////////////////////////////////////////////////////////////////////////
		
		int GetSelectedItem( void );
		int GetSelectionAtPosition( int x, int y );

		//////////////////////////////////////////////////////////////////////////

		virtual BOOL PreTranslateMessage( MSG *pMsg );

	public:
		ImageExplorer *m_pImageExplorer;
		int m_ThumbnailWidth;
		int m_ThumbnailHeight;
		int m_BorderSize;
		COLORREF m_WindowColor;
		int m_SelectedItem;
		int m_HoveredItem;
		ThumbnailPic m_ThumbnailPics;

		CToolTipCtrl m_ToolTip;

	protected:
		virtual void OnDraw( CDC *pdc );
		virtual void OnErasePic( HDC hdc, int x1, int y1, int x2, int y2, COLORREF c );
		virtual void OnDrawPic( HDC hdc, int Dx, int Dy, int Dw, int Dh, HBITMAP hbm, int Sx, int Sy, int Sw, int Sh );
		virtual void OnDrawFrame( HDC hdc, int x1, int y1, int x2, int y2, COLORREF c );

		void DestroyDrawBitmap( void );

	protected:
		afx_msg int OnCreate( LPCREATESTRUCT lpcs );
		afx_msg BOOL OnEraseBkgnd( CDC *pdc );
		afx_msg void OnLButtonDown( UINT Flags, CPoint p );
		afx_msg void OnMouseMove( UINT Flags, CPoint p );
		afx_msg void OnSize( UINT Type, int x, int y );

	private:
		HBITMAP m_hbmDraw;

	protected:
		DECLARE_MESSAGE_MAP();

};

#endif // _THUMBNAIL_H_