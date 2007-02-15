#include "pch.h"
#include "application.h"
#include "dibblefile.h"
#include "document.h"
#include "view.h"
#include "resource.h"

IMPLEMENT_DYNCREATE( UndibbleView, CView );

BEGIN_MESSAGE_MAP( UndibbleView, CView )
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()	
	ON_MESSAGE( FXN_DIRECTORYCHANGED, OnDirectoryChanged )
	ON_NOTIFY( FVN_SELECTIONCHANGED, 0x100, OnFileChanged )
END_MESSAGE_MAP();

UndibbleView::UndibbleView()
{
	m_IconSize = 64;
	m_IconRows = 1;
	m_IconBorder = 32;
}

UndibbleView::~UndibbleView()
{
}

UndibbleDoc * UndibbleView::GetDocument( void )
{
	return (UndibbleDoc *) CView::GetDocument();
}

RECT UndibbleView::GetFileViewRect( void )
{
	RECT rc;
	GetClientRect( &rc );

	int RectHeight = m_IconSize * m_IconRows + m_IconBorder + 32;
	int ScrollBarSize = GetSystemMetrics( SM_CYHSCROLL );

	rc.top = rc.bottom - RectHeight - ScrollBarSize;

	return rc;
}

RECT UndibbleView::GetThumbnailRect( void )
{
	RECT rc;
	GetClientRect( &rc );

	RECT rcFileView = GetFileViewRect();

	rc.bottom = rcFileView.top;

	return rc;
}

void UndibbleView::SetFileExplorer( FileExplorer *pFileExplorer )
{
	if( m_pFileView )
		m_pFileView->SetFileExplorer( pFileExplorer );

	return;
}

FileExplorer* UndibbleView::GetFileExplorer( void )
{
	if( m_pFileView )
		return m_pFileView->GetFileExplorer();

	return 0;
}

DirectoryFile* UndibbleView::GetFile( void )
{
	FileExplorer *pFileExplorer = GetFileExplorer();

	if( pFileExplorer )
		return pFileExplorer->GetDirectoryFile( m_pFileView->GetSelectedItem() );

	return 0;
}

BOOL UndibbleView::LoadMyDibble( char *pFilePath )
{
	CDocument *pDoc = GetDocument();

	if( pDoc )
		return pDoc->OnOpenDocument( pFilePath );

	return FALSE;
}

BOOL UndibbleView::CreateFileView( void )
{
	m_pFileView = new FileView;

	if( !m_pFileView )
		return FALSE;

	RECT rc = GetFileViewRect();

	if( !m_pFileView->Create( 0, 0, WS_VISIBLE, rc, this, 0x100, 0 ) )
		return FALSE;

	HBITMAP hbmIcon = (HBITMAP) LoadImage( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_DBLFILE), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION );

	m_pFileView->SetIconSize( m_IconSize, m_IconSize );
	m_pFileView->SetBorderSize( m_IconBorder );
	m_pFileView->SetWindowColor( GetSysColor( COLOR_WINDOW ) );
	m_pFileView->SetDefaultIcon( hbmIcon );
	m_pFileView->SetGuiFont( App.DefaultGuiFont() );

	return TRUE;
}

BOOL UndibbleView::CreateThumbnailView( void )
{
	m_pThumbnailView = new ThumbnailView;

	if( !m_pThumbnailView )
		return FALSE;

	RECT rc = GetThumbnailRect();

	if( !m_pThumbnailView->Create( 0, 0, WS_VISIBLE, rc, this, 0, 0 ) )
		return FALSE;

	m_pThumbnailView->SetThumbnailSize( 128, 128 );
	m_pThumbnailView->SetWindowColor( 0xffffff );

	UndibbleDoc *pDoc = GetDocument();

	if( pDoc )
		pDoc->m_pThumbnailView = m_pThumbnailView;

	return TRUE;
}

void UndibbleView::OnDraw( CDC *pdc )
{
	return;
}

int UndibbleView::OnCreate( LPCREATESTRUCT pcs )
{
	if( -1 == CView::OnCreate( pcs ) )
		return -1;

	ModifyStyle( 0, WS_CLIPCHILDREN|WS_CLIPSIBLINGS );

	CreateFileView();
	CreateThumbnailView();

	return 0;
}

BOOL UndibbleView::OnEraseBkgnd( CDC *pdc )
{
	return CView::OnEraseBkgnd( pdc );
}

void UndibbleView::OnMouseMove( UINT Flags, CPoint p )
{
}

void UndibbleView::OnSize( UINT Type, int x, int y )
{
	RECT rc;

	if( m_pFileView )
	{
		rc = GetFileViewRect();
		m_pFileView->MoveWindow( &rc, TRUE );
	}

	if( m_pThumbnailView )
	{
		rc = GetThumbnailRect();
		m_pThumbnailView->MoveWindow( &rc, TRUE );
	}

	CView::OnSize( Type, x, y );
}

LRESULT UndibbleView::OnDirectoryChanged( WPARAM wParam, LPARAM lParam )
{
	if( m_pFileView )
		m_pFileView->Invalidate( FALSE );

	return 0;
}

void UndibbleView::OnFileChanged( NMHDR *pNotify, LRESULT *pResult )
{
	if( pNotify && m_pFileView )
	{
		DirectoryFile *pFile = GetFile();

		if( pFile )
			LoadMyDibble( pFile->m_FilePath.get() );
	}

	return;
}