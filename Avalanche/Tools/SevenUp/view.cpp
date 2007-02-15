#include "pch.h"
#include "application.h"
#include "document.h"
#include "view.h"
#include "resource.h"

IMPLEMENT_DYNCREATE( SevenupView, CView );

BEGIN_MESSAGE_MAP( SevenupView, CView )
	ON_MESSAGE( FXN_DIRECTORYCHANGED, OnDirectoryChanged )
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_NOTIFY_EX_RANGE( TNN_SELECTIONCHANGED, 0, 0x7fffffff, OnThumbnailSelect )

END_MESSAGE_MAP();

SevenupView::SevenupView()
{
	m_ThumbnailPictureSize = 96;
	m_ThumbnailRectHeight = 2;
	m_SideBarSize = 192;
	m_RulerSize = 10;
}

SevenupView::~SevenupView()
{
}

SevenupDoc * SevenupView::GetDocument( void )
{
	return (SevenupDoc *) CView::GetDocument();
}

void SevenupView::OnNewDocument( void )
{
	if( m_pGameView )
		m_pGameView->OnNewDocument();

	return;
}

RECT SevenupView::GetThumbnailRect( void )
{
	RECT rc;
	GetClientRect( &rc );

	int RectHeight = m_ThumbnailPictureSize * m_ThumbnailRectHeight + 32;
	int ScrollBarSize = GetSystemMetrics( SM_CYHSCROLL );

	rc.top = rc.bottom - RectHeight - ScrollBarSize;

	return rc;
}

RECT SevenupView::GetGameViewRect( void )
{
	RECT rcThumb = GetThumbnailRect();
	RECT rcObject = GetProjectViewRect();
	
	RECT rcClient;
	GetClientRect( &rcClient );

	rcClient.left = rcClient.left + m_RulerSize;
	rcClient.top = rcClient.top + m_RulerSize;
	rcClient.bottom = rcThumb.top;
	rcClient.right = rcObject.left;

	int cw = rcClient.right - rcClient.left;
	int ch = rcClient.bottom - rcClient.top;

	if( ch > cw * 3 / 4 )
		ch = cw * 3 / 4;

	if( cw > ch * 4 / 3 )
		cw = ch * 4 / 3;

//	rcClient.left += (rcClient.right - rcClient.left - cw) / 2;
//	rcClient.top += (rcClient.bottom - rcClient.top - ch) / 2;
	rcClient.right = rcClient.left + cw;
	rcClient.bottom = rcClient.top + ch;
	
	return rcClient;
}

RECT SevenupView::GetRuler1Rect( void )
{
	RECT rcGameView = GetGameViewRect();

	int BarSize = GetSystemMetrics( SM_CXVSCROLL );

	RECT rc = GdiMakeRect( rcGameView.left, rcGameView.top - m_RulerSize, rcGameView.right - rcGameView.left - BarSize, m_RulerSize );

	return rc;
}

RECT SevenupView::GetRuler2Rect( void )
{
	RECT rcGameView = GetGameViewRect();

	int BarSize = GetSystemMetrics( SM_CYHSCROLL );

	RECT rc = GdiMakeRect( rcGameView.left - m_RulerSize, rcGameView.top, m_RulerSize, rcGameView.bottom - rcGameView.top - BarSize );

	return rc;
}

RECT SevenupView::GetProjectViewRect( void )
{
	RECT rcThumb = GetThumbnailRect();

	RECT rcClient;
	GetClientRect( &rcClient );

	rcClient.bottom = rcThumb.top;
	rcClient.left = rcClient.right - m_SideBarSize;

	return rcClient;
}

void SevenupView::DocumentSetTexture( t_Handle Handle, int TextureIndex )
{
	SevenupDoc *pDocument = (SevenupDoc *) GetDocument();

	if( pDocument )
		pDocument->SetTexture( Handle, TextureIndex );

	return;
}

t_Handle SevenupView::DocumentGetTextureHandle( void )
{
	SevenupDoc *pDocument = (SevenupDoc *) GetDocument();

	if( !pDocument )
		return INVALID_HANDLE;

	return pDocument->GetTextureHandle();
}

int SevenupView::DocumentGetTextureIndex( void )
{
	SevenupDoc *pDocument = (SevenupDoc *) GetDocument();

	if( !pDocument )
		return TS_NO_TEXTURE;

	return pDocument->GetTextureIndex();
}

t_Handle SevenupView::ThumbnailGetTextureHandle( void )
{
	DirectoryImage *pImage = ImageExplorerGetDirectoryImage();

	if( pImage )
		return dblLoad( pImage->m_FilePath.get() );

	return INVALID_HANDLE;
}

ImageExplorer *SevenupView::ThumbnailGetImageExplorer( void )
{
	if( m_pThumbnailView )
		return m_pThumbnailView->GetImageExplorer();

	return 0;
}

DirectoryImage *SevenupView::ImageExplorerGetDirectoryImage( void )
{
	ImageExplorer *pImageExplorer = ThumbnailGetImageExplorer();

	if( pImageExplorer )
		return pImageExplorer->GetDirectoryImage( m_pThumbnailView->GetSelectedItem() );

	return 0;
}

void SevenupView::OnDraw( CDC *pdc )
{
	RECT rc;
	GetClientRect( &rc );

	GdiFillRect( pdc->m_hDC, rc, 0xbbccee );
}

BOOL SevenupView::CreateThumbnailView( void )
{
	m_pThumbnailView = new ThumbnailView;

	if( !m_pThumbnailView )
		return FALSE;

	RECT rc = GetThumbnailRect();

	if( !m_pThumbnailView->Create( 0, 0, WS_VISIBLE, rc, this, 0, 0 ) )
		return FALSE;

	m_pThumbnailView->SetThumbnailSize( 96, 96 );

	return TRUE;
}

BOOL SevenupView::CreateProjectView( void )
{
	m_pProjectView = new ProjectView;

	if( !m_pProjectView )
		return FALSE;

	RECT rc = GetProjectViewRect();

	if( !m_pProjectView->CreateEx( 0, 0, "Project View", WS_VISIBLE|WS_CHILD|WS_CLIPCHILDREN, rc, this, 0, 0 ) )
		return FALSE;

	if( GetDocument() )
		GetDocument()->AddView( m_pProjectView );

	return TRUE;
}

BOOL SevenupView::CreateGameView( void )
{
	m_pGameView = new GameView;

	if( !m_pGameView )
		return FALSE;

	RECT rc = GetGameViewRect();

	if( !m_pGameView->Create( 0, 0, WS_VISIBLE|WS_CHILD|WS_CLIPCHILDREN, rc, this, 0, 0 ) )
		return FALSE;

	CDocument *pDocument = GetDocument();

	if( pDocument )
		pDocument->AddView( m_pGameView );

	return TRUE;
}

BOOL SevenupView::CreateRulerBars( void )
{
	RECT rcRuler1 = GetRuler1Rect();
	RECT rcRuler2 = GetRuler2Rect();

	if( !m_RulerBar1.Create( 0, "RulerBar1", WS_CHILD|WS_VISIBLE, rcRuler1, this, 0 ) )
		return FALSE;

	if( !m_RulerBar2.Create( 0, "RulerBar2", WS_CHILD|WS_VISIBLE, rcRuler2, this, 0 ) )
		return FALSE;

	return TRUE;
}

LRESULT SevenupView::OnDirectoryChanged( WPARAM wParam, LPARAM lParam )
{
	if( m_pThumbnailView )
	{
		m_pThumbnailView->Invalidate( FALSE );

		ImageExplorer* ie = m_pThumbnailView->GetImageExplorer();

		if( ie )
		{
			TCHAR szPathName[_MAX_PATH];

			ie->GetCurrentDirectory( szPathName, sizeof(szPathName)/sizeof(szPathName[0]) );
			App.SetThumbnailDirectory( szPathName );
		}
	}

	return 0;
}

int SevenupView::OnCreate( LPCREATESTRUCT pcs )
{
	if( -1 == CView::OnCreate( pcs ) )
		return -1;

	ModifyStyle( 0, WS_CLIPCHILDREN|WS_CLIPSIBLINGS );

	if( GetDocument() )
		GetDocument()->m_pPrimaryView = this;

	CreateThumbnailView();
	CreateProjectView();
	CreateGameView();
	CreateRulerBars();

	return 0;
}

BOOL SevenupView::OnEraseBkgnd( CDC *pdc )
{
	return TRUE;
}

void SevenupView::OnMouseMove( UINT Flags, CPoint p )
{
}

void SevenupView::OnSize( UINT Type, int x, int y )
{
	RECT rc;

	if( m_pThumbnailView )
	{
		rc = GetThumbnailRect();
		m_pThumbnailView->MoveWindow( &rc, TRUE );
	}

	if( m_pProjectView )
	{
		rc = GetProjectViewRect();
		m_pProjectView->MoveWindow( &rc, TRUE );
	}

	if( m_pGameView )
	{
		rc = GetGameViewRect();	
		m_pGameView->MoveWindow( &rc, TRUE );
	}

	rc = GetRuler1Rect();
	m_RulerBar1.MoveWindow( &rc, TRUE );

	rc = GetRuler2Rect();
	m_RulerBar2.MoveWindow( &rc, TRUE );

	CView::OnSize( Type, x, y );
}

void SevenupView::OnTextureLocate( void )
{
	if( m_pGameView )
	{
		Sprite *pSprite = m_pGameView->SelectedSprite();

		if( pSprite )
		{
			const TCHAR *pFileName = dblGetFileName( pSprite->GetTextureSet() );

			if( pFileName )
			{
				ImageExplorer *pImageExplorer = ThumbnailGetImageExplorer();
			}
		}
	}

	return;
}

BOOL SevenupView::OnThumbnailSelect( UINT Id, NMHDR *pNotify, LRESULT *pResult )
{
	if( pNotify && m_pThumbnailView )
	{
		if( pNotify->hwndFrom == m_pThumbnailView->m_hWnd )
		{
			DirectoryImage *pImage = ImageExplorerGetDirectoryImage();

			if( pImage )
				DocumentSetTexture( dblLoad( pImage->m_FilePath.get() ), 0 );
		}
	}

	return FALSE;
}

//=============================================================================
// SevenupView::OnNotifyClick
//=============================================================================

BOOL SevenupView::OnNotifyClick( UINT Id, NMHDR* pNotify, LRESULT* pResult )
{
	if( m_pProjectView )
	{
		if( pNotify && pNotify->hwndFrom == m_pProjectView->m_hWnd )
		{
			MessageBox( "You clicked!" );
		}
	}

	return FALSE;
}