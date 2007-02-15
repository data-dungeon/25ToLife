#ifndef _MAINFRAME_H_
#define _MAINFRAME_H_

#include <afxwin.h>
#include <afxext.h>
#include "maintoolbar.h"

class MainFrame : public CFrameWnd
{
	DECLARE_DYNCREATE( MainFrame );
	
	public:
		CSplitterWnd m_SplitterWnd;
		MainToolBar m_ToolBar;

		void AddFont( char *pFontName );

	protected:
		afx_msg int OnCreate( LPCREATESTRUCT pcs );
		afx_msg void OnEnterMenuLoop( BOOL IsPopupMenu );
		afx_msg void OnExitMenuLoop( BOOL IsPopupMenu );

	protected:
		virtual HACCEL GetDefaultAccelerator( void );
		virtual BOOL CreateToolBar( void );
		virtual BOOL OnCreateClient( LPCREATESTRUCT pcs, CCreateContext *pContext );

	protected:
		DECLARE_MESSAGE_MAP()
};

#endif // _MAINFRAME_H_