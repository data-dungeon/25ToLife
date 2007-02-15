#include "pch.h"
#include "propertywnd.h"

#define IDC_EDIT_CONTROL	(0x101)

PropertyItem::PropertyItem( void )
{
	m_Type = PROPERTYTYPE_UNKNOWN;
	m_PropertyCode = 0;
	m_Quantity = 0;
	m_CellHeight = 0;
	m_ReadOnly = FALSE;
	m_Collapsed = FALSE;
}

PropertyItem::~PropertyItem()
{
}

void PropertyItem::SetProperty( const char *pName, const char *pDescription, PROPERTYTYPE Type, int PropertyCode, int Quantity, BOOL ReadOnly )
{
	SetName( pName );
	SetDescription( pDescription );

	m_Type = Type;
	m_PropertyCode = PropertyCode;
	m_Quantity = Quantity;
	m_CellHeight = 0;
	m_ReadOnly = ReadOnly;
}

void PropertyItem::SetName( const char *pName )
{
	m_Name.set( pName );
}

void PropertyItem::SetDescription( const char *pDescription )
{
	m_Description.set( pDescription );
}

void PropertyItem::SetValue( const char *pString )
{
	m_Value.set( pString );
}

BEGIN_MESSAGE_MAP( PropertyWnd, CWnd )
	ON_WM_ACTIVATE()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_MOVE()
	ON_WM_KEYDOWN()
	ON_WM_VSCROLL()
	ON_WM_SHOWWINDOW()
	ON_NOTIFY( EN_WINDOWMESSAGE, IDC_EDIT_CONTROL, OnEditNotify )
	ON_EN_UPDATE( IDC_EDIT_CONTROL, OnEditUpdate )
END_MESSAGE_MAP()

PropertyWnd::PropertyWnd( void )
{
	m_pSelectedProperty = 0;
	m_SeparatorPos = 80;
	m_SeparatorDrag = FALSE;
	m_CellHeight = 20;
	m_ShiftKey = FALSE;
	m_TabScroll = FALSE;
	m_pUserParam = 0;
	
	m_WindowRect = GdiMakeRect( 0, 0, 0, 0 );
}

PropertyWnd::~PropertyWnd()
{
	Clear();
}

PropertyItem * PropertyWnd::NewPropertyItem( void )
{
	return new PropertyItem;
}

void PropertyWnd::Clear( void )
{
	m_pSelectedProperty = 0;

	while( m_PropertyList.Child() )
		delete m_PropertyList.Child();

	if( m_hWnd )
		Invalidate( FALSE );

	return;
}

BOOL PropertyWnd::AddProperty( const char *pName, const char *pDescription, PROPERTYTYPE Type, int PropertyCode, int Quantity, BOOL ReadOnly )
{
	PropertyItem *pPropertyItem = NewPropertyItem();

	if( !pPropertyItem )
		return FALSE;

	if( Type == PROPERTYTYPE_BOOL )
		Quantity = 1;

	if( Type == PROPERTYTYPE_CATAGORY )
		Quantity = 0;

	pPropertyItem->SetProperty( pName, pDescription, Type, PropertyCode, Quantity, ReadOnly );

	m_PropertyList.AddChild( pPropertyItem );

	Invalidate( FALSE );

	return TRUE;
}

BOOL PropertyWnd::AddCatagory( const char *pName )
{
	return AddProperty( pName, 0, PROPERTYTYPE_CATAGORY, 0xcdefcdef, 0, TRUE );
}

PropertyItem* PropertyWnd::GetPropertyItem( int PropertyCode )
{
	PropertyItem *pPropertyItem = m_PropertyList.Child();

	while( pPropertyItem )
	{
		if( pPropertyItem->m_PropertyCode == PropertyCode )
			break;

		pPropertyItem = pPropertyItem->Next();
	}

	return pPropertyItem;
}

PropertyItem* PropertyWnd::GetPropertyItem( const char *pPropertyName )
{
	PropertyItem *pPropertyItem = m_PropertyList.Child();

	while( pPropertyItem )
	{
		if( 0 == pPropertyItem->m_Name.strcmp( pPropertyName ) )
			break;

		pPropertyItem = pPropertyItem->Next();
	}

	return pPropertyItem;
}

RECT PropertyWnd::GetPropertyRect( void )
{
	RECT rc;
	GetClientRect( &rc );

	RECT rcNonClient = GetNonClientRect();

	rc.left = rcNonClient.right;

	return rc;
}

RECT PropertyWnd::GetNonClientRect( void )
{
	RECT rc;
	GetClientRect( &rc );

	rc.right = rc.left + 16;

	return rc;
}

void PropertyWnd::SelectProperty( PropertyItem *pPropertyItem )
{
	if( m_pSelectedProperty != pPropertyItem )
	{
		m_pSelectedProperty = pPropertyItem;

		m_CurrentProperty = pPropertyItem->m_PropertyCode;
	
		if( !pPropertyItem->m_ReadOnly )
		{
			m_EditCtrl.SetWindowText( pPropertyItem->m_Value.get() );
			m_EditCtrl.SetSel( 0, -1 );
			m_EditCtrl.SetFocus();

			m_OriginalText.set( pPropertyItem->m_Value.get() );
		}

		Invalidate( TRUE );
	}

	return;
}

void PropertyWnd::SelectProperty( int PropertyCode )
{
	PropertyItem *pPropertyItem = GetPropertyItem( PropertyCode );

	if( pPropertyItem )
		SelectProperty( pPropertyItem );

	return;
}

void PropertyWnd::SelectProperty( const char *pPropertyName )
{
	PropertyItem *pPropertyItem = GetPropertyItem( pPropertyName );

	if( pPropertyItem )
		SelectProperty( pPropertyItem );

	return;
}

void PropertyWnd::GotoNextProperty( void )
{
	PropertyItem *p = m_pSelectedProperty;

	BOOL OnlyOnce = FALSE;
	BOOL Collapsed = FALSE;
	BOOL ValidChoice;

	m_TabScroll = TRUE;

	while( p )
	{
		ValidChoice = TRUE;

		if( p->m_Type == PROPERTYTYPE_CATAGORY )
		{
			Collapsed = p->m_Collapsed;
			ValidChoice = FALSE;
		}

		if( !Collapsed )
		{
			if( p == m_pSelectedProperty )
				ValidChoice = FALSE;

			if( p->m_ReadOnly )
				ValidChoice = FALSE;

			if( ValidChoice )
			{
				SelectProperty( p );
				return;
			}
		}

		p = p->Next();

		if( !p && !OnlyOnce )
		{
			OnlyOnce = TRUE;
			p = m_PropertyList.Child();
		}
	}

	return;
}

void PropertyWnd::GotoPrevProperty( void )
{
	PropertyItem *p = m_pSelectedProperty;
	PropertyItem *p2;

	BOOL OnlyOnce = FALSE;
	BOOL Collapsed = FALSE;
	BOOL ValidChoice;

	m_TabScroll = TRUE;

	while( p )
	{
		Collapsed = FALSE;
		ValidChoice = TRUE;

		if( p->m_Type != PROPERTYTYPE_CATAGORY )
		{
			p2 = p->Prev();

			while( p2 )
			{
				if( p2->m_Type == PROPERTYTYPE_CATAGORY )
				{
					Collapsed = p2->m_Collapsed;
					break;
				}

				p2 = p2->Prev();
			}

			if( !Collapsed )
			{
				if( p == m_pSelectedProperty )
					ValidChoice = FALSE;

				if( p->m_ReadOnly )
					ValidChoice = FALSE;

				if( ValidChoice )
				{
					SelectProperty( p );
					return;
				}
			}
		}

		p = p->Prev();

		if( !p && !OnlyOnce )
		{
			OnlyOnce = TRUE;
			p = (PropertyItem*) m_PropertyList.GetLastChild();
		}
	}

	return;
}

void PropertyWnd::Collapse( const char *pName, BOOL Collapsed )
{
	PropertyItem *pPropertyItem = m_PropertyList.Child();

	while( pPropertyItem )
	{
		if( pPropertyItem->m_Name.stricmp( pName ) == 0 )
			pPropertyItem->m_Collapsed = Collapsed;

		pPropertyItem = pPropertyItem->Next();
	}

	return;
}

int PropertyWnd::SelectedPropertyCode( void )
{
	if( m_pSelectedProperty )
		return m_pSelectedProperty->m_PropertyCode;

	return -1;
}

BOOL PropertyWnd::SelectedPropertyString( TCHAR *pString, int StringSize )
{
	if( !m_pSelectedProperty )
		return FALSE;

	tstrncpy( pString, m_pSelectedProperty->m_Value.get(), StringSize );

	return TRUE;
}

void PropertyWnd::SetPropertyString( int PropertyCode, const char *pString )
{
	PropertyItem *pPropertyItem = GetPropertyItem( PropertyCode );

	if( pPropertyItem )
		pPropertyItem->SetValue( pString );

	return;
}

BOOL PropertyWnd::GetPropertyString( int PropertyCode, TCHAR *pString, int StringSize )
{
	PropertyItem *pPropertyItem = GetPropertyItem( PropertyCode );

	if( !pPropertyItem )
		return FALSE;

	strncpy( pString, pPropertyItem->m_Value.get(), StringSize );

	return TRUE;
}

void PropertyWnd::SetUserParam( void* pParam )
{
	m_pUserParam = pParam;
}

void* PropertyWnd::GetUserParam( void )
{
	return m_pUserParam;
}

BOOL PropertyWnd::InitializeFonts( void )
{
	if( !m_GuiFont.m_hObject )
	{
		NONCLIENTMETRICS metrics;
		metrics.cbSize = sizeof(metrics);

		SystemParametersInfo( SPI_GETNONCLIENTMETRICS, sizeof(metrics), &metrics, 0 );

		HFONT hGuiFont = CreateFontIndirect( &metrics.lfMessageFont );

		if( !hGuiFont )
			return FALSE;

		m_GuiFont.Attach( hGuiFont );
	}

	if( !m_BoldFont.m_hObject )
	{
		HFONT hBoldFont = GdiCreateSimilarFont( (HFONT) m_GuiFont.m_hObject, 0, FW_BOLD, FALSE, FALSE );

		if( hBoldFont )
			m_BoldFont.Attach( hBoldFont );
	}

	return TRUE;
}

void PropertyWnd::PositionEditCtrl( void )
{
	PropertyItem *pPropertyItem = m_pSelectedProperty;

	if( !pPropertyItem || pPropertyItem->m_ReadOnly )
		m_EditCtrl.ShowWindow( SW_HIDE );
	else
	{
		RECT rc;

		rc.left   = pPropertyItem->m_RightRect.left + 1;
		rc.right  = pPropertyItem->m_RightRect.right - 1;
		rc.top    = pPropertyItem->m_RightRect.top + 1;
		rc.bottom = pPropertyItem->m_RightRect.bottom - 1;

		m_EditCtrl.MoveWindow( &rc );
		m_EditCtrl.ShowWindow( SW_SHOW );

		if( m_TabScroll )
		{
			RECT rcClient;
			GetClientRect( &rcClient );

			SCROLLINFO si;
			si.cbSize = sizeof(si);
			si.fMask = SIF_ALL;

			GetScrollInfo( SB_VERT, &si, si.fMask );

			int Change = 0;

			if( rc.bottom > rcClient.bottom )
			{
				Change = rc.bottom - rcClient.bottom;
			}
			else
			if( rc.top < rcClient.top )
			{
				Change = rc.top - rcClient.top;
			}

			if( Change != 0 )
			{
				int OldPos = si.nPos;
				si.nPos += Change;

				SetScrollInfo( SB_VERT, &si, TRUE );
				GetScrollInfo( SB_VERT, &si, si.fMask );

				if( si.nPos != OldPos )
				{
					ScrollWindow( 0, OldPos - si.nPos );
					UpdateWindow();
				}
			}

			m_TabScroll = FALSE;
		}
	}

	return;
}

void PropertyWnd::OnActivate( UINT State, CWnd *pWnd, BOOL Minimized )
{
	switch( State )
	{
	case WA_INACTIVE:
		break;
	case WA_ACTIVE:
	case WA_CLICKACTIVE:
		m_pSelectedProperty = 0;
		SelectProperty( m_CurrentProperty );
		break;
	}

	return;
}

int PropertyWnd::OnCreate( LPCREATESTRUCT pcs )
{
	if( -1 == CWnd::OnCreate( pcs ) ) 
		return -1;

	SCROLLINFO si;

	si.cbSize = sizeof(si);
	si.fMask = SIF_ALL;
	si.nMin = 0;
	si.nMax = 0;
	si.nPage = 0;
	si.nTrackPos = 0;

	SetScrollInfo( SB_VERT, &si, TRUE );

	InitializeFonts();

	RECT rc = GdiMakeRect( 0, 0, 0, 0 );

	DWORD dwStyle = WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL|ES_AUTOVSCROLL|ES_MULTILINE;

	if( m_EditCtrl.Create( dwStyle, rc, this, IDC_EDIT_CONTROL ) )
	{
		m_EditCtrl.SetFont( &m_BoldFont );
		m_EditCtrl.ShowWindow( SW_SHOW );
		m_EditCtrl.UpdateWindow();
	}

	return 0;
}

void PropertyWnd::OnDestroy( void )
{
	Clear();
}

void PropertyWnd::OnLButtonDown( UINT Flags, CPoint p )
{
	OnMouseMove( Flags, p );

	if( !m_SeparatorDrag )
	{
		PropertyItem *pPropertyItem = m_PropertyList.Child();

		BOOL Collapsed = FALSE;
		BOOL Selected = FALSE;

		while( pPropertyItem )
		{
			Selected = FALSE;

			if( !Collapsed || pPropertyItem->m_Type == PROPERTYTYPE_CATAGORY )
			{
				Collapsed = pPropertyItem->m_Collapsed;

				if( GdiPointInRect( p.x, p.y, pPropertyItem->m_CellRect ) )
					Selected = TRUE;

				if( GdiPointInRect( p.x, p.y, pPropertyItem->m_SideRect ) )
				{
					if( pPropertyItem->m_Collapsed )
						pPropertyItem->m_Collapsed = FALSE;
					else
						pPropertyItem->m_Collapsed = TRUE;

					Invalidate( FALSE );
				}
			}

			if( pPropertyItem->m_Type == PROPERTYTYPE_CATAGORY )
				Selected = FALSE;

			if( Selected )
			{
				SelectProperty( pPropertyItem );

				if( !pPropertyItem->m_ReadOnly )
				{
					if( GdiPointInRect( p.x, p.y, pPropertyItem->m_LeftRect ) )
						m_EditCtrl.SetSel( 0, -1 );
				}
			}

			pPropertyItem = pPropertyItem->Next();
		}
	}

	return;
}

void PropertyWnd::OnLButtonUp( UINT Flags, CPoint p )
{
	OnMouseMove( Flags, p );
}

void PropertyWnd::OnMouseMove( UINT Flags, CPoint p )
{
	RECT rc = GetPropertyRect();

	int CursorType = OCR_NORMAL;

	int cx = rc.left + m_SeparatorPos;

	int y1 = rc.top;
	int y2 = rc.bottom;
	int x1 = cx - 4;
	int x2 = cx + 4;

	int x = p.x;
	int y = p.y;

	if( m_SeparatorDrag == FALSE )
	{
		if( x >= x1 && x < x2 && y >= y1 && y < y2 )
		{
			if( Flags & MK_LBUTTON )
			{
				m_SeparatorDrag = TRUE;
				m_DragPosition.x = x - m_SeparatorPos;
				m_DragPosition.y = y;
			}

			CursorType = OCR_SIZEWE;
		}
	}
	else
	{
		if( Flags & MK_LBUTTON )
			m_SeparatorPos = x - m_DragPosition.x;
		else
		{
			m_SeparatorDrag = FALSE;
			m_DragPosition.x = 0;
			m_DragPosition.y = 0;
		}

		if( m_SeparatorPos < 0 )
			m_SeparatorPos = 0;

		CursorType = OCR_SIZEWE;

		Invalidate( FALSE );
	}

	HCURSOR hCursor = (HCURSOR) LoadImage( 0, MAKEINTRESOURCE(CursorType), IMAGE_CURSOR, 0, 0, LR_DEFAULTCOLOR|LR_DEFAULTSIZE|LR_SHARED );

	if( hCursor )
		SetCursor( hCursor );

	return;
}

void PropertyWnd::OnPaint( void )
{
	RECT rc;
	PAINTSTRUCT ps;
	CDC *pdc = BeginPaint( &ps );

	CRgn Clip;
	RECT CellRect;
	RECT LeftRect;
	RECT RightRect;
	RECT SideRect;
	int x = 0;
	int y = 0;
	int i;

	CFont *pGuiFont = 0;
	COLORREF WhiteColor = 0xffffff;
	COLORREF BlackColor = 0x000000;
	COLORREF LightColor = 0xddeeff;
	COLORREF DarkColor = ColorSub( LightColor, 0x707070 );
	COLORREF TextColor;
	COLORREF FillColor;
	COLORREF CellColor;
	int TextOffset;
	int CellHeight;
	BOOL DrawHeader;
	BOOL Collapsed;
	BOOL Selected;

	SCROLLINFO si;
	si.cbSize = sizeof(si);

	GetScrollInfo( SB_VERT, &si );

	rc = GetNonClientRect();
	GdiFillRect( pdc->m_hDC, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, LightColor );

	rc = GetPropertyRect();

	PropertyItem *pPropertyItem = m_PropertyList.Child();

	Collapsed = FALSE;

	y = -si.nPos;

	for( i = 0; pPropertyItem; i++ )
	{
		CellHeight = pPropertyItem->m_CellHeight;

		if( CellHeight == 0 )
			CellHeight = m_CellHeight;

		CellRect = GdiMakeRect( rc.left, y, rc.right - rc.left, CellHeight );
		LeftRect = GdiMakeRect( rc.left, y, m_SeparatorPos, CellHeight );
		RightRect = GdiMakeRect( LeftRect.right, y, rc.right - LeftRect.right, CellHeight );

		pGuiFont = &m_GuiFont;
		FillColor = WhiteColor;
		TextColor = BlackColor;
		CellColor = LightColor;
		TextOffset = 4;
	
		Selected = FALSE;

		if( pPropertyItem == m_pSelectedProperty )
			Selected = TRUE;

		DrawHeader = FALSE;

		if( pPropertyItem->m_Type == PROPERTYTYPE_CATAGORY )
			DrawHeader = TRUE;

		if( DrawHeader )
		{
			FillColor = LightColor;
			TextColor = DarkColor;
			pGuiFont = &m_BoldFont;
			Collapsed = FALSE;
			Selected = FALSE;
			TextOffset = 2;
		}

		if( !Collapsed )
		{
			if( Selected )
			{
				pGuiFont = &m_BoldFont;
				FillColor = DarkColor;
				TextColor = WhiteColor;
			}

		//	Clip.CreateRectRgnIndirect( &CellRect );

			pdc->SetTextColor( AssBackwardsColor( TextColor ) );
			pdc->SetBkColor( AssBackwardsColor( FillColor ) );
			pdc->SetBkMode( TRANSPARENT );
			pdc->SelectObject( pGuiFont );

			if( DrawHeader )
				GdiFillRect( pdc->m_hDC, RightRect, FillColor );

			GdiFillRect( pdc->m_hDC, LeftRect, FillColor );
			GdiTextInRect( pdc->m_hDC, LeftRect.left + TextOffset, LeftRect.top, LeftRect.right, LeftRect.bottom, pPropertyItem->m_Name.get(), 0.0f, 0.5f );

			SideRect = GetNonClientRect();
			SideRect.top = CellRect.top;
			SideRect.bottom = CellRect.bottom;

			if( DrawHeader )
			{				
				int cx = SideRect.left + (SideRect.right - SideRect.left) / 2;
				int cy = SideRect.top  + (SideRect.bottom - SideRect.top) / 2;
				int w = 9;
				int h = 9;

				RECT BoxRect = GdiMakeRect( cx - w/2, cy - h/2, w, h );

				GdiFrame( pdc->m_hDC, BoxRect, DarkColor );

				int x1 = BoxRect.left + 2;
				int x2 = BoxRect.right - 2;
				int x3 = x1 + (x2 - x1) / 2;

				int y1 = BoxRect.top + 2;
				int y2 = BoxRect.bottom - 2;
				int y3 = y1 + (y2 - y1) / 2;
					
				GdiLine( pdc->m_hDC, x1, y3, x2, y3, DarkColor );

				if( pPropertyItem->m_Collapsed )
					GdiLine( pdc->m_hDC, x3, y1, x3, y2, DarkColor );
			}
			else
			{
				if( pPropertyItem->m_ReadOnly )
					pdc->SetTextColor( AssBackwardsColor( ColorBlend( TextColor, FillColor, 128 ) ) );

				GdiFillRect( pdc->m_hDC, RightRect, FillColor );
				GdiTextInRect( pdc->m_hDC, RightRect.left + TextOffset, RightRect.top, RightRect.right, RightRect.bottom, pPropertyItem->m_Value.get(), 0.0f, 0.5f );
			}

			pPropertyItem->m_CellRect = CellRect;
			pPropertyItem->m_SideRect = SideRect;
			pPropertyItem->m_LeftRect = LeftRect;
			pPropertyItem->m_RightRect = RightRect;

			if( !DrawHeader )
			{
				GdiFrame( pdc->m_hDC, LeftRect, CellColor );
				GdiFrame( pdc->m_hDC, RightRect, CellColor );
			}
			else
			{
				Collapsed = pPropertyItem->m_Collapsed;
			}

			y = y + LeftRect.bottom - LeftRect.top;
		}

		pPropertyItem = pPropertyItem->Next();
	}

	m_ScrollWidth = 0;
	m_ScrollHeight = y + si.nPos;

	si.cbSize = sizeof(si);
	si.fMask = SIF_PAGE|SIF_RANGE;
	si.nMin = 0;
	si.nMax = m_ScrollHeight;
	si.nPage = rc.bottom - rc.top;

	SetScrollInfo( SB_VERT, &si, TRUE );

	m_ScrollInfo.cbSize = sizeof(m_ScrollInfo);
	m_ScrollInfo.fMask = SIF_ALL;

	GetScrollInfo( SB_VERT, &m_ScrollInfo, m_ScrollInfo.fMask );

	GdiFillRect( pdc->m_hDC, rc.left, y, rc.right - rc.left, rc.bottom - y, WhiteColor );

	EndPaint( &ps );

	PositionEditCtrl();
}

void PropertyWnd::OnSize( UINT Type, int x, int y )
{
	CWnd::OnSize( Type, x, y );

	GetWindowRect( &m_WindowRect );
}

void PropertyWnd::OnMove( int x, int y )
{
	CWnd::OnMove( x, y );

	GetWindowRect( &m_WindowRect );
}

void PropertyWnd::OnKeyDown( UINT Char, UINT RepeatCount, UINT Flags )
{
	switch( Char )
	{
	case VK_ESCAPE:
		SendMessage( WM_CLOSE, 0, 0 );
		break;
	}

	return;
}

void PropertyWnd::OnVScroll( UINT Code, UINT Pos, CScrollBar *pScrollBar )
{
	SCROLLINFO si;
	si.cbSize = sizeof(si);
	si.fMask = SIF_ALL;

	GetScrollInfo( SB_VERT, &si, si.fMask );

	int OldPos = si.nPos;

	switch( Code )
	{
	case SB_TOP:
		si.nPos = si.nMin;
		break;

	case SB_BOTTOM:
		si.nPos = si.nMax;
		break;

	case SB_LINEUP:
		si.nPos -= 20;
		break;

	case SB_LINEDOWN:
		si.nPos += 20;
		break;

	case SB_PAGEUP:
		si.nPos -= si.nPage;
		break;

	case SB_PAGEDOWN:
		si.nPos += si.nPage;
		break;

	case SB_THUMBTRACK:
		si.nPos = si.nTrackPos;
		break;
	}

	si.fMask = SIF_POS;

	SetScrollInfo( SB_VERT, &si, TRUE );
	GetScrollInfo( SB_VERT, &si, si.fMask );

	if( si.nPos != OldPos )
	{
		ScrollWindow( 0, OldPos - si.nPos );
		UpdateWindow();
	}

	Invalidate( FALSE );

	return;
}

void PropertyWnd::OnShowWindow( BOOL Show, UINT Status )
{
	if( Show == TRUE && Status == 0 )
		m_TabScroll = TRUE;

	CWnd::OnShowWindow( Show, Status );
}

void PropertyWnd::OnEditNotify( NMHDR *pNotify, LRESULT *pResult )
{
	if( pNotify )
	{
		NOTIFYEDITSTRUCT *n = (NOTIFYEDITSTRUCT *) pNotify;

		if( n->message == WM_KEYDOWN )
		{
			if( n->wParam == VK_SHIFT )
				m_ShiftKey = TRUE;

			if( n->wParam == VK_ESCAPE )
				SendMessage( WM_CLOSE, 0, 0 );
		}

		if( n->message == WM_KEYUP )
		{
			if( n->wParam == VK_SHIFT )
				m_ShiftKey = FALSE;
		}

		if( n->message == WM_CHAR )
		{
			int ch = n->wParam;
			BOOL Tabbed = FALSE;
			BOOL Notify = FALSE;

			DWORD dwStyle = ::GetWindowLong( n->nmhdr.hwndFrom, GWL_STYLE );

			if( ch == '\t' )
				Tabbed = TRUE;

			if( ch == '\r' )
			{
				if( !(dwStyle & ES_WANTRETURN) && !m_ShiftKey )
					Tabbed = TRUE;
			}

			if( Tabbed )
			{
				n->message = WM_NULL;
				n->wParam = 0;
				n->lParam = 0;

				CWnd *pParent = GetParent();

				if( pParent )
				{
					TCHAR szPropertyString[4096];
					SelectedPropertyString( szPropertyString, sizeof(szPropertyString)/sizeof(szPropertyString[0]) );

					if( ch == '\r' || m_OriginalText.strcmp( szPropertyString ) != 0 )
						Notify = TRUE;

					const char* pPropertyName = 0;

					if( m_pSelectedProperty )
						pPropertyName = m_pSelectedProperty->m_Name.get();

					if( Notify )
					{
						PROPERTYNOTIFYSTRUCT notify;

						notify.nmhdr.hwndFrom = m_hWnd;
						notify.nmhdr.idFrom = GetDlgCtrlID();
						notify.nmhdr.code = PROPNOTIFY_ITEMCHANGED;
						notify.PropertyCode = SelectedPropertyCode();
						notify.pPropertyKey = pPropertyName;
						notify.pPropertyString = szPropertyString;

						pParent->SendMessage( WM_NOTIFY, (WPARAM) notify.nmhdr.idFrom, (LPARAM) &notify );
					}
				}

				if( ch == '\r' )
				{
					m_EditCtrl.SetSel( 0, -1 );
				}
				else
				{
					if( m_ShiftKey )
						GotoPrevProperty();
					else
						GotoNextProperty();
				}
			}
		}
	}

	return;
}

void PropertyWnd::OnEditUpdate( void )
{
	PropertyItem *pPropertyItem = m_pSelectedProperty;

	if( pPropertyItem )
	{
		TCHAR szText[4096];
		m_EditCtrl.GetWindowText( szText, sizeof(szText)/sizeof(szText[0]) );

		pPropertyItem->m_Value.set( szText );
	}

	return;
}