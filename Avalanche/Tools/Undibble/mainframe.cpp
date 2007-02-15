#include "pch.h"
#include "application.h"
#include "mainframe.h"
#include "view.h"
#include "resource.h"

IMPLEMENT_DYNCREATE( MainFrame, CFrameWnd );

BEGIN_MESSAGE_MAP( MainFrame, CFrameWnd )
	ON_WM_CREATE()
END_MESSAGE_MAP();

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
// MainFrame::CreateToolBar
//=============================================================================

BOOL MainFrame::CreateToolBar( void )
{
	if( !m_ToolBar.Create( this ) )
		return FALSE;

	if( !m_ToolBar.LoadToolBar( IDR_MAINFRAME ) )
		return FALSE;

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
	m_SplitterWnd.CreateView( 0, 0, RUNTIME_CLASS( FileExplorer ), size, pContext );
	m_SplitterWnd.CreateView( 0, 1, RUNTIME_CLASS( UndibbleView ), size, pContext );

	FileExplorer *pFileExplorer = (FileExplorer *) m_SplitterWnd.GetPane( 0, 0 );

	if( pFileExplorer )
	{
		pFileExplorer->SetFilePattern( "*.dbl" );
		pFileExplorer->ScanForFolders();
	}

	UndibbleView *pView = (UndibbleView *) m_SplitterWnd.GetPane( 0, 1 );

	if( pView )
		pView->SetFileExplorer( pFileExplorer );

	return TRUE;
}