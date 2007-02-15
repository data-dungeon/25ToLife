#ifndef _FILEVIEW_H_
#define _FILEVIEW_H_

#include <afxwin.h>
#include "fileexplorer.h"

typedef struct
{
	NMHDR nmhdr;
	int SelectedItem;
}
FILEVIEWNOTIFYSTRUCT;

#define FVN_SELECTIONCHANGED (0x100)
#define FVN_HOVERCHANGED	  (0x101)

class FileView : public CScrollView
{
	DECLARE_DYNCREATE( FileView );

	public:
		FileView( void );
		virtual ~FileView();

		//////////////////////////////////////////////////////////////////////////
		
		FileExplorer *GetFileExplorer( void );
		void SetFileExplorer( FileExplorer *pFileExplorer );
		void SetIconSize( int Width, int Height );
		void SetBorderSize( int Bs );
		void SetWindowColor( COLORREF c );
		void SetDefaultIcon( HBITMAP hbm );
		void SetGuiFont( CFont *pFont );
		
		//////////////////////////////////////////////////////////////////////////
		
		void Select( int Index );
		void Hover( int Index );
		
		//////////////////////////////////////////////////////////////////////////
		
		int GetSelectedItem( void );
		int GetSelectionAtPosition( int x, int y );

		//////////////////////////////////////////////////////////////////////////

		virtual BOOL PreTranslateMessage( MSG *pMsg );

public:
		FileExplorer *m_pFileExplorer;
		int m_IconWidth;
		int m_IconHeight;
		int m_BorderSize;
		int m_SelectedItem;
		int m_HoveredItem;
		HBITMAP m_hbmDefault;
		HFONT m_hGuiFont;

		COLORREF m_WindowColor;

		CToolTipCtrl m_ToolTip;

	protected:
		virtual void OnDraw( CDC *pdc );

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

#endif // _FILEVIEW_H_