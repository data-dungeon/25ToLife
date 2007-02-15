#include "pch.h"
#include "fileview.h"

IMPLEMENT_DYNCREATE( FileView, CScrollView )

BEGIN_MESSAGE_MAP( FileView, CScrollView )
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_SIZE()
END_MESSAGE_MAP()

FileView::FileView( void )
{
	SetFileExplorer(0);

	SetScrollSizes( MM_TEXT, CSize(0, 0) );
	SetIconSize( 128, 128 );

	m_SelectedItem = -1;
	m_HoveredItem = 0;
	m_BorderSize = 8;
	m_WindowColor = 0xddeeff;
	m_hbmDefault = 0;
	m_hbmDraw = 0;
	m_hGuiFont = 0;
}

FileView::~FileView()
{
}

FileExplorer *FileView::GetFileExplorer( void )
{
	return m_pFileExplorer;
}

void FileView::SetFileExplorer( FileExplorer *pFileExplorer )
{
	m_pFileExplorer = pFileExplorer;
}

void FileView::SetIconSize( int Width, int Height )
{
	m_IconWidth = Width;
	m_IconHeight = Height;
}

void FileView::SetBorderSize( int Bs )
{
	m_BorderSize = Bs;
}

void FileView::SetWindowColor( COLORREF c )
{
	m_WindowColor = c;
}

void FileView::SetDefaultIcon( HBITMAP hbm )
{
	m_hbmDefault = hbm;
}

void FileView::SetGuiFont( CFont *pFont )
{
	m_hGuiFont = (HFONT) pFont->m_hObject;
}

void FileView::Select( int Index )
{
	if( m_SelectedItem != Index )
	{
		m_SelectedItem = Index;

		if( GetParent() )
		{
			FILEVIEWNOTIFYSTRUCT notify;

			notify.nmhdr.hwndFrom = GetSafeHwnd();
			notify.nmhdr.idFrom = GetDlgCtrlID();
			notify.nmhdr.code = FVN_SELECTIONCHANGED;

			notify.SelectedItem = m_SelectedItem;

			GetParent()->SendMessage( WM_NOTIFY, (WPARAM) notify.nmhdr.idFrom, (LPARAM) &notify );
		}

		Invalidate( FALSE );
	}
	
	return;
}

void FileView::Hover( int Index )
{
	if( m_HoveredItem != Index )
	{
		m_HoveredItem = Index;

		if( GetParent() )
		{
			FILEVIEWNOTIFYSTRUCT NotifyStruct;

			NotifyStruct.nmhdr.hwndFrom = GetSafeHwnd();
			NotifyStruct.nmhdr.idFrom = GetDlgCtrlID();
			NotifyStruct.nmhdr.code = FVN_HOVERCHANGED;

			NotifyStruct.SelectedItem = m_SelectedItem;

			GetParent()->SendMessage( WM_NOTIFY, (WPARAM) 0, (LPARAM) &NotifyStruct );
		}
		Invalidate( FALSE );
	}

	FileExplorer *fe = GetFileExplorer();

	if( fe )
	{
		DirectoryFile *pFile = fe->GetDirectoryFile( Index );

		if( pFile )
		{
			CPoint p;
			GetCursorPos( &p );

			CToolInfo ti;
			m_ToolTip.GetToolInfo( ti, this, 0x101 );

			filename FileName;
			FileName.getfilename( pFile->m_FilePath.get() );

			ti.lpszText = FileName.get();
			m_ToolTip.SetToolInfo( &ti );
		}
	}

	return;
}

int FileView::GetSelectedItem( void )
{
	return m_SelectedItem;
}

int FileView::GetSelectionAtPosition( int x, int y )
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
	Tw = m_IconWidth;
	Th = m_IconHeight;
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

BOOL FileView::PreTranslateMessage( MSG *pMsg )
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

void FileView::OnDraw( CDC *pdc )
{
	FileExplorer *fe = m_pFileExplorer;
	DirectoryFile *file;

	CPoint scroll = GetScrollPosition();

	HBITMAP hbm;
	BITMAP bm;
	RECT rc;

	GetClientRect( &rc );

	int Bs = m_BorderSize;
	int Tw = m_IconWidth;
	int Th = m_IconHeight;
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
//	int sx1, sy1;
//	int sx2, sy2;

	HDC hdcDraw = CreateCompatibleDC( pdc->m_hDC );

	if( !m_hbmDraw )
		m_hbmDraw = CreateCompatibleBitmap( pdc->m_hDC, rc.right, rc.bottom );

	m_hbmDraw = (HBITMAP) SelectObject( hdcDraw, m_hbmDraw );
	HFONT hOldFont = (HFONT) SelectObject( hdcDraw, m_hGuiFont );

	GdiFillRect( hdcDraw, 0, 0, rc.right, rc.bottom, m_WindowColor );

	if( fe )
	{
		file = (DirectoryFile *) fe->m_FileTree.Child();

		for( i = 0; file; i++ )
		{
			hbm = m_hbmDefault;

			if( hbm )
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

//				if( i == GetSelectedItem() )
//				{
//					hbm2 = hbm;
//					sx1 = x1 - Bs*5/3;
//					sy1 = y1 - Bs*5/3;
//					sx2 = x2 + Bs*5/3;
//					sy2 = y2 + Bs*5/3;
//				}
//				else
				{
					if( x2 >= 0 && y2 >= 0 && x1 < rc.right && y1 < rc.bottom )
					{
						if( i == m_HoveredItem )
							GdiFillRect( hdcDraw, x1-4, y1-4, (x2-x1)+9, (y2-y1)+9, ColorBlend( m_WindowColor, 0x000000, 64 ) );

						if( i == m_SelectedItem )
							GdiFillRect( hdcDraw, x1-4, y1-4, (x2-x1)+9, (y2-y1)+9, ColorBlend( m_WindowColor, 0x440000, 192 ) );

						GdiFillRect( hdcDraw, x1, y1, (x2-x1), (y2-y1), 0xffffff );

						StretchBltPreserveAspectRatio( hdcDraw, x1, y1, (x2 - x1), (y2 - y1), hbm, 0, 0, bm.bmWidth, bm.bmHeight );

						GdiLine( hdcDraw, x1, y1, x2, y1, 0x000000 );
						GdiLine( hdcDraw, x2, y1, x2, y2, 0x000000 );
						GdiLine( hdcDraw, x2, y2, x1, y2, 0x000000 );
						GdiLine( hdcDraw, x1, y2, x1, y1, 0x000000 );

						RECT rc = GdiMakeTextRect( hdcDraw, x1, y2+6, file->m_DisplayName.get() );

						rc.left += 1;
						rc.top += 1;
						rc.right += 2;
						rc.bottom += 2;

						GdiTextInRect( hdcDraw, rc.left, rc.top, rc.right, rc.bottom, file->m_DisplayName.get(), 0.0f, 0.0f );
					}
				}

				if( mx < x2 + Bs )
					mx = x2 + Bs;

				if( my < y2 + Bs )
					my = y2 + Bs;
			}

			file = (DirectoryFile *) file->Next();
		}

//		if( hbm2 )
//		{
//			int sw = sx2 - sx1;
//			int sh = sy2 - sy1;
//
//			GdiFillRect( hdcDraw, sx1, sy1, (sx2-sx1), (sy2-sy1), 0xffffff );
//
//			StretchBltPreserveAspectRatio( hdcDraw, sx1, sy1, (sx2 - sx1), (sy2 - sy1), hbm2, 0, 0, GetBitmapWidth(hbm2), GetBitmapHeight(hbm2) );
//
//			GdiLine( hdcDraw, sx1, sy1, sx2, sy1, 0x000000 );
//			GdiLine( hdcDraw, sx2, sy1, sx2, sy2, 0x000000 );
//			GdiLine( hdcDraw, sx2, sy2, sx1, sy2, 0x000000 );
//			GdiLine( hdcDraw, sx1, sy2, sx1, sy1, 0x000000 );
//		}
	}
	else
	{
	}

	hOldFont = (HFONT) SelectObject( hdcDraw, hOldFont );
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

int FileView::OnCreate( LPCREATESTRUCT lpcs )
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
BOOL FileView::OnEraseBkgnd( CDC *pdc )
{
	return TRUE;
}

void FileView::OnLButtonDown( UINT Flags, CPoint p )
{
	Select( GetSelectionAtPosition( p.x, p.y ) );
}

void FileView::OnMouseMove( UINT Flags, CPoint p )
{
	CScrollView::OnMouseMove( Flags, p );

	Hover( GetSelectionAtPosition( p.x, p.y ) );
}

void FileView::OnSize( UINT Type, int x, int y )
{
	m_hbmDraw = ReleaseDIB( m_hbmDraw );

	RECT rc;
	GetClientRect( &rc );

	m_ToolTip.SetToolRect( this, 0x101, &rc );
}