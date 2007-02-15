#include "pch.h"
#include "thumbnail.h"

//=============================================================================
// ThumbnailPic::ThumbnailPic
//
//		The constructor sets the thumbnail image to null.
//
//=============================================================================


ThumbnailPic::ThumbnailPic( void )
{
	m_hbmPic = 0;
}

//=============================================================================
// ThumbnailPic::~ThumbnailPic
//
//		The destructor releases the thumbnail image.
//
//=============================================================================

ThumbnailPic::~ThumbnailPic( void )
{
	ReleaseDIB( m_hbmPic );
}

//=============================================================================
// The IMPLEMENT_DYNCREATE macro is required if this view is to be used by
// the document/view architecture.
//=============================================================================

IMPLEMENT_DYNCREATE( ThumbnailView, CScrollView )

//=============================================================================
// The message map for the ThumbnailView
//=============================================================================

BEGIN_MESSAGE_MAP( ThumbnailView, CScrollView )
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_SIZE()
END_MESSAGE_MAP()

//=============================================================================
// ThumbnailView::ThumbnailView
//
//		The constructor sets the image explorer to null, initializes the scroll
//		sizes, sets the default thumbnail size and initializes various member
//		data.
//
//=============================================================================


ThumbnailView::ThumbnailView( void )
{
	SetImageExplorer(0);

	SetScrollSizes( MM_TEXT, CSize(0, 0) );
	SetThumbnailSize( 128, 128 );

	m_BorderSize = 8;
	m_WindowColor = 0xddeeff;
	m_SelectedItem = -1;
	m_HoveredItem = 0;

	m_hbmDraw = 0;
}

//=============================================================================
// ThumbnailView::~ThumbnailVew
//=============================================================================

ThumbnailView::~ThumbnailView()
{
	DestroyDrawBitmap();
}

ImageExplorer *ThumbnailView::GetImageExplorer( void )
{
	return m_pImageExplorer;
}

void ThumbnailView::SetImageExplorer( ImageExplorer *pImageExplorer )
{
	m_pImageExplorer = pImageExplorer;
}

void ThumbnailView::SetThumbnailSize( int Width, int Height )
{
	m_ThumbnailWidth = Width;
	m_ThumbnailHeight = Height;
}

void ThumbnailView::SetWindowColor( COLOR32 c )
{
	m_WindowColor = c;
}

BOOL ThumbnailView::AddPicture( HBITMAP hbm, int Id )
{
	ThumbnailPic *pPic = new ThumbnailPic;

	if( !pPic )
		return FALSE;

	pPic->m_hbmPic = CloneDIB( hbm );
	pPic->SetObjectId( Id );

	if( !pPic->m_hbmPic )
	{
		delete pPic;
		return FALSE;
	}

	m_ThumbnailPics.AddChild( pPic );

	return TRUE;
}

void ThumbnailView::Clear( void )
{
	while( m_ThumbnailPics.Child() )
		delete m_ThumbnailPics.Child();

	return;
}

void ThumbnailView::Select( int Index )
{
	if( m_SelectedItem != Index )
	{
		m_SelectedItem = Index;

		if( GetParent() )
		{
			THUMBNAILNOTIFYSTRUCT notify;

			notify.nmhdr.hwndFrom = GetSafeHwnd();
			notify.nmhdr.idFrom = GetDlgCtrlID();
			notify.nmhdr.code = TNN_SELECTIONCHANGED;

			notify.SelectedItem = m_SelectedItem;

			GetParent()->SendMessage( WM_NOTIFY, (WPARAM) notify.nmhdr.idFrom, (LPARAM) &notify );
		}

		Invalidate( FALSE );
	}
	
	return;
}

void ThumbnailView::Hover( int Index )
{
	if( m_HoveredItem != Index )
	{
		m_HoveredItem = Index;

		if( GetParent() )
		{
			THUMBNAILNOTIFYSTRUCT NotifyStruct;

			NotifyStruct.nmhdr.hwndFrom = GetSafeHwnd();
			NotifyStruct.nmhdr.idFrom = GetDlgCtrlID();
			NotifyStruct.nmhdr.code = TNN_HOVERCHANGED;

			NotifyStruct.SelectedItem = m_SelectedItem;

			GetParent()->SendMessage( WM_NOTIFY, (WPARAM) 0, (LPARAM) &NotifyStruct );
		}
		Invalidate( FALSE );
	}

	ImageExplorer *ie = GetImageExplorer();

	if( ie )
	{
		DirectoryImage *pImage = ie->GetDirectoryImage( Index );

		if( pImage )
		{
			CPoint p;
			GetCursorPos( &p );

			CToolInfo ti;
			m_ToolTip.GetToolInfo( ti, this, 0x101 );

			filename FileName;
			FileName.getfilename( pImage->m_FilePath.get() );

			ti.lpszText = FileName.get();
			m_ToolTip.SetToolInfo( &ti );
		}
	}

	return;
}

int ThumbnailView::GetSelectedItem( void )
{
	return m_SelectedItem;
}

int ThumbnailView::GetSelectionAtPosition( int x, int y )
{
	RECT rc;
	int Bs;
	int Tw, Th;
	int Dw, Dh;
	int Columns;
	int Rows;
	int c, r;
	int Sel;
	int cx, cy;

	CPoint scroll = GetScrollPosition();

	x += scroll.x;
	y += scroll.y;

	Bs = m_BorderSize;
	Tw = m_ThumbnailWidth;
	Th = m_ThumbnailHeight;
	Dw = Tw + Bs;
	Dh = Th + Bs;

	GetClientRect( &rc );

	Rows = rc.bottom / Dh;
	Columns = rc.right / Dw;

	if( Rows < 1 )
		Rows = 1;

	if( Columns < 1 )
		Columns = 1;

	BOOL LeftToRight = FALSE;

	if( Columns > Rows )
		LeftToRight = TRUE;

	cx = cy = 0;

	if( LeftToRight )
		cy = (rc.bottom - Rows * Dh - Bs) / 2;
	else
		cx = (rc.right - Columns * Dw - Bs) / 2;

	c = (x-cx) / Dw;
	r = (y-cy) / Dh;

	if( LeftToRight )
		Sel = c * Rows + r;
	else
		Sel = r * Columns + c;

	return Sel;
}

BOOL ThumbnailView::PreTranslateMessage( MSG *pMsg )
{
	if(pMsg->message == WM_LBUTTONDOWN ||
		pMsg->message == WM_LBUTTONUP ||
		pMsg->message == WM_MOUSEMOVE ||
		pMsg->message == WM_RBUTTONDOWN ||
		pMsg->message == WM_RBUTTONUP )
	{
		m_ToolTip.RelayEvent( pMsg );
	}

	return CScrollView::PreTranslateMessage( pMsg );
}

void ThumbnailView::OnDraw( CDC *pdc )
{
	ImageExplorer *ie = m_pImageExplorer;
	DirectoryImage *image;
	ThumbnailPic *pic;

	CPoint scroll = GetScrollPosition();

	HBITMAP hbm;
	BITMAP bm;
	RECT rc;

	GetClientRect( &rc );

	int Bs = m_BorderSize;
	int Tw = m_ThumbnailWidth;
	int Th = m_ThumbnailHeight;
	int Dw = Tw + Bs;
	int Dh = Th + Bs;

	int i, x, y;
	int x1, y1;
	int x2, y2;
	int mx, my;
	int cx, cy;

	int Rows = rc.bottom / Dh;
	int Columns = rc.right / Dw;

	if( Rows < 1 )
		Rows = 1;

	if( Columns < 1 )
		Columns = 1;

	int r, c;

	BOOL LeftToRight = FALSE;

	if( Columns > Rows )
		LeftToRight = TRUE;

	cx = cy = 0;

	if( LeftToRight )
		cy = (rc.bottom - Rows * Dh - Bs) / 2;
	else
		cx = (rc.right - Columns * Dw - Bs) / 2;

	mx = rc.right;
	my = rc.bottom;

	HBITMAP hbm2 = 0;
	int sx1, sy1;
	int sx2, sy2;

	HDC hdcDraw = CreateCompatibleDC( pdc->m_hDC );

	if( !m_hbmDraw )
		m_hbmDraw = CreateCompatibleBitmap( pdc->m_hDC, rc.right, rc.bottom );

	m_hbmDraw = (HBITMAP) SelectObject( hdcDraw, m_hbmDraw );

	GdiFillRect( hdcDraw, 0, 0, rc.right, rc.bottom, m_WindowColor );

	image = 0;
	pic = 0;
	hbm = 0;

	if( ie )
	{
		image = (DirectoryImage *) ie->m_FileTree.Child();

		if( image )
			hbm = image->m_Picture.PictureHandle();
	}
	else
	{
		 pic = (ThumbnailPic*) m_ThumbnailPics.Child();

		 if( pic )
			 hbm = pic->m_hbmPic;
	}

	for( i = 0; hbm; i++ )
	{
		if( LeftToRight )
		{
			r = i % Rows;
			c = i / Rows;
		}
		else
		{
			r = i / Columns;
			c = i % Columns;
		}

		x = Bs + c * Dw;
		y = Bs + r * Dh;

		bm = GetBitmapStruct( hbm );

		x1 = x - scroll.x + cx;
		y1 = y - scroll.y + cy;
		x2 = x1 + Tw;
		y2 = y1 + Th;

		if( i == GetSelectedItem() )
		{
			hbm2 = hbm;
			sx1 = x1 - Bs*5/3;
			sy1 = y1 - Bs*5/3;
			sx2 = x2 + Bs*5/3;
			sy2 = y2 + Bs*5/3;
		}
		else
		{
			if( x2 >= 0 && y2 >= 0 && x1 < rc.right && y1 < rc.bottom )
			{
				if( i == m_HoveredItem )
					OnErasePic( hdcDraw, x1-4, y1-4, x2+5, y2+5, ColorBlend( m_WindowColor, 0x000000, 64 ) );
//							GdiFillRect( hdcDraw, x1-4, y1-4, (x2-x1)+9, (y2-y1)+9, ColorBlend( m_WindowColor, 0x000000, 64 ) );

				OnErasePic( hdcDraw, x1, y1, x2, y2, 0xffffff );
				OnDrawPic( hdcDraw, x1, y1, (x2 - x1), (y2 - y1), hbm, 0, 0, bm.bmWidth, bm.bmHeight );
				OnDrawFrame( hdcDraw, x1, y1, x2, y2, 0x000000 );
			}
		}

		if( mx < x2 + Bs )
			mx = x2 + Bs;

		if( my < y2 + Bs )
			my = y2 + Bs;

		hbm = 0;

		if( image )
		{
			image = (DirectoryImage*) image->Next();

			if( image )
				hbm = image->m_Picture.PictureHandle();
		}
		else
		if( pic )
		{
			pic = (ThumbnailPic*) pic->Next();

			if( pic )
				hbm = pic->m_hbmPic;
		}
	}

	if( hbm2 )
	{
		int sw = sx2 - sx1;
		int sh = sy2 - sy1;

		GdiFillRect( hdcDraw, sx1, sy1, (sx2-sx1), (sy2-sy1), 0xffffff );

		StretchBltPreserveAspectRatio( hdcDraw, sx1, sy1, (sx2 - sx1), (sy2 - sy1), hbm2, 0, 0, GetBitmapWidth(hbm2), GetBitmapHeight(hbm2) );

		GdiLine( hdcDraw, sx1, sy1, sx2, sy1, 0x000000 );
		GdiLine( hdcDraw, sx2, sy1, sx2, sy2, 0x000000 );
		GdiLine( hdcDraw, sx2, sy2, sx1, sy2, 0x000000 );
		GdiLine( hdcDraw, sx1, sy2, sx1, sy1, 0x000000 );
	}

	m_hbmDraw = (HBITMAP) SelectObject( hdcDraw, m_hbmDraw );

	DeleteDC( hdcDraw );

	BltBitmap( pdc->m_hDC, scroll.x, scroll.y, rc.right, rc.bottom, m_hbmDraw, 0, 0, rc.right, rc.bottom );

	mx += scroll.x;
	my += scroll.y;

	x1 = y1 = 0;
	x2 = x1 + mx - 1;
	y2 = y1 + my - 1;

	GdiLine( pdc->m_hDC, x1, y1, x2, y1, 0x000000 );
	GdiLine( pdc->m_hDC, x2, y1, x2, y2, 0x000000 );
	GdiLine( pdc->m_hDC, x2, y2, x1, y2, 0x000000 );
	GdiLine( pdc->m_hDC, x1, y2, x1, y1, 0x000000 );

	SetScrollSizes( MM_TEXT, CSize( mx, my ), CSize( Dw * Columns, Dh * Rows ), CSize( Dw, Dh ) );
}

void ThumbnailView::OnErasePic( HDC hdc, int x1, int y1, int x2, int y2, COLORREF c )
{
	GdiFillRect( hdc, x1, y1, (x2-x1), (y2-y1), 0xffffff );
}

void ThumbnailView::OnDrawPic( HDC hdc, int Dx, int Dy, int Dw, int Dh, HBITMAP hbm, int Sx, int Sy, int Sw, int Sh )
{
	StretchBltPreserveAspectRatio( hdc, Dx, Dy, Dw, Dh, hbm, Sx, Sy, Sw, Sh );
}

void ThumbnailView::OnDrawFrame( HDC hdc, int x1, int y1, int x2, int y2, COLORREF c )
{
	GdiLine( hdc, x1, y1, x2, y1, c );
	GdiLine( hdc, x2, y1, x2, y2, c );
	GdiLine( hdc, x2, y2, x1, y2, c );
	GdiLine( hdc, x1, y2, x1, y1, c );
}

void ThumbnailView::DestroyDrawBitmap( void )
{
	m_hbmDraw = ReleaseDIB( m_hbmDraw );
}

int ThumbnailView::OnCreate( LPCREATESTRUCT lpcs )
{
	if( -1 == CScrollView::OnCreate( lpcs ) )
		return -1;

	RECT rc;
	GetClientRect( &rc );

	DWORD dwStyle = WS_VISIBLE|WS_POPUP|TTS_ALWAYSTIP;

	m_ToolTip.Create( this, dwStyle );
	m_ToolTip.AddTool( this, "Filename goes here.", &rc, 0x101 );

	TOOLINFO ti;
   
	ti.uFlags = 0;
	ti.hwnd = GetSafeHwnd();
	ti.uId = 0x101;
	ti.rect = rc;
	ti.hinst = AfxGetInstanceHandle();
	ti.lpszText = "";
	ti.lParam = 0;

	m_ToolTip.SetToolInfo( &ti );
	m_ToolTip.SetDelayTime( TTDT_INITIAL, 200 );

	return 0;
}
BOOL ThumbnailView::OnEraseBkgnd( CDC *pdc )
{
	return TRUE;
}

void ThumbnailView::OnLButtonDown( UINT Flags, CPoint p )
{
	Select( GetSelectionAtPosition( p.x, p.y ) );
}

void ThumbnailView::OnMouseMove( UINT Flags, CPoint p )
{
	CScrollView::OnMouseMove( Flags, p );

	Hover( GetSelectionAtPosition( p.x, p.y ) );
}

void ThumbnailView::OnSize( UINT Type, int x, int y )
{
	DestroyDrawBitmap();

	RECT rc;
	GetClientRect( &rc );

	m_ToolTip.SetToolRect( this, 0x101, &rc );
}