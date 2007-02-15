#include "pch.h"
#include "application.h"
#include "mainframe.h"
#include "view.h"
#include "resource.h"

IMPLEMENT_DYNCREATE( MainFrame, CFrameWnd );

BEGIN_MESSAGE_MAP( MainFrame, CFrameWnd )
	ON_WM_CREATE()
	ON_WM_ENTERMENULOOP()
	ON_WM_EXITMENULOOP()
END_MESSAGE_MAP();

//=============================================================================
// MainFrame::AddFont
//=============================================================================

void MainFrame::AddFont( char *pFontName )
{
	CComboBox &c = m_ToolBar.m_FontList;

	int i = c.FindStringExact( 0, pFontName );

	if( i == CB_ERR )
	{
		c.AddString( pFontName );

		if( c.GetCurSel() == -1 )
			c.SetCurSel(0);

		int Count = c.GetCount();
		int Height = c.GetItemHeight(0);

		for( int i = 0; i < Count; i++ )
			Height += c.GetItemHeight(i);

		if( Height < 32 )
			Height = 32;

		COMBOBOXINFO cb;
		cb.cbSize = sizeof(cb);
		c.GetComboBoxInfo( &cb );

		RECT rc;
		::GetClientRect( cb.hwndList, &rc );

		rc.bottom = rc.top + Height;

		::MoveWindow( cb.hwndList, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, FALSE );
		::UpdateWindow( cb.hwndList );
	}
	
	return;
}

//=============================================================================
// MainFrame::OnCreate
//=============================================================================

int MainFrame::OnCreate( LPCREATESTRUCT pcs )
{
	if( -1 == CFrameWnd::OnCreate( pcs ) )
		return -1;

	MoveWindow( &App.m_StartupRect, FALSE );

	EnableDocking( CBRS_ALIGN_ANY );

	CreateToolBar();

	return 0;
}

//=============================================================================
// MainFrame::OnEnterMenuLoop
//=============================================================================

void MainFrame::OnEnterMenuLoop( BOOL IsPopupMenu )
{
	CFrameWnd::OnEnterMenuLoop( IsPopupMenu );
}

//=============================================================================
// MainFrame::OnExitPopupMenu
//=============================================================================

void MainFrame::OnExitMenuLoop( BOOL IsPopupMenu )
{
	CFrameWnd::OnExitMenuLoop( IsPopupMenu );
}

//=============================================================================
// MainFrame::GetDefaultAccelerator
//=============================================================================

HACCEL MainFrame::GetDefaultAccelerator( void )
{
	return CFrameWnd::GetDefaultAccelerator();
}

//=============================================================================
// MainFrame::CreateToolBar
//=============================================================================

BOOL MainFrame::CreateToolBar( void )
{
	DWORD dwStyle = WS_CHILD|WS_VISIBLE|CBRS_TOP|CBRS_TOOLTIPS;

	if( !m_ToolBar.Create( this, dwStyle ) )
		return FALSE;

	if( !m_ToolBar.LoadToolBar( IDR_MAINFRAME ) )
		return FALSE;

	m_ToolBar.CreateFontList( App.DefaultGuiFont() );
	
	return TRUE;
}

//=============================================================================
// MainFrame::OnCreateClient
//=============================================================================

BOOL MainFrame::OnCreateClient( LPCREATESTRUCT pcs, CCreateContext *pContext )
{
	RECT rc;
	GetClientRect( &rc );

	SIZE size;

	size.cx = 192;
	size.cy = rc.bottom;

	m_SplitterWnd.CreateStatic( this, 1, 2 );
	m_SplitterWnd.CreateView( 0, 0, RUNTIME_CLASS( ImageExplorer ), size, pContext );
	m_SplitterWnd.CreateView( 0, 1, RUNTIME_CLASS( SevenupView ), size, pContext );

	ImageExplorer *pImageExplorer = (ImageExplorer *) m_SplitterWnd.GetPane( 0, 0 );
	SevenupView *pView = (SevenupView *) m_SplitterWnd.GetPane( 0, 1 );

	if( pView )
		pView->m_pThumbnailView->SetImageExplorer( pImageExplorer );

	if( pImageExplorer )
	{
		pImageExplorer->PreviewSettings( 4, 4, FALSE );
		pImageExplorer->SetCurrentDirectory( App.m_ThumbnailPath.get() );
	}

	return TRUE;
}