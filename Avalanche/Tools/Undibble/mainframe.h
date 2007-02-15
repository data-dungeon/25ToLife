#ifndef _MAINFRAME_H_
#define _MAINFRAME_H_

#include <afxwin.h>
#include <afxext.h>

class MainFrame : public CFrameWnd
{
	DECLARE_DYNCREATE( MainFrame );
	
	public:
		CSplitterWnd m_SplitterWnd;
		CToolBar m_ToolBar;

	protected:
		afx_msg int OnCreate( LPCREATESTRUCT pcs );

	protected:
		virtual BOOL CreateToolBar( void );
		virtual BOOL OnCreateClient( LPCREATESTRUCT pcs, CCreateContext *pContext );

	protected:
		DECLARE_MESSAGE_MAP()
};

#endif // _MAINFRAME_H_