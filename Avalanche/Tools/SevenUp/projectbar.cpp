#include "pch.h"
#include "projectbar.h"

ToolBarButton::ToolBarButton( void )
{
	m_hbmButton = 0;
	m_Width = 0;
	m_Height = 0;
	m_ButtonDown = FALSE;
}

ToolBarButton::~ToolBarButton()
{
	DestroyButtonImage();
}

int ToolBarButton::GetButtonId( void )
{
	return m_ObjectId;
}

void ToolBarButton::SetButtonId( int ButtonId )
{
	m_ObjectId = ButtonId;
}

BOOL ToolBarButton::SetButtonImage( HBITMAP hbm )
{
	DestroyButtonImage();

	m_hbmButton = CloneDIB( hbm, FALSE );

	if( !m_hbmButton )
		return FALSE;

	return TRUE;
}

void ToolBarButton::SetButtonSize( int Width, int Height )
{
	m_Width = Width;
	m_Height = Height;
}

void ToolBarButton::SetButtonDown( BOOL TrueOrFalse )
{
	m_ButtonDown = TrueOrFalse;
}

void ToolBarButton::DestroyButtonImage( void )
{
	if( m_hbmButton )
		m_hbmButton = ReleaseDIB( m_hbmButton );

	return;
}

BEGIN_MESSAGE_MAP( ToolBar, CWnd )
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_PAINT()
END_MESSAGE_MAP()

ToolBar::ToolBar( void )
{
	m_pButtonHovered = 0;
	m_pButtonSelected = 0;
	m_pButtonDown = 0;
}

ToolBar::~ToolBar()
{
}

void ToolBar::AddButton( int ButtonId, HBITMAP ButtonImage )
{
	ToolBarButton *pButton = NewButton();

	if( pButton )
	{
		pButton->SetButtonId( ButtonId );
		pButton->SetButtonImage( ButtonImage );
		pButton->SetButtonSize( 32, 32 );

		m_Buttons.AddChild( pButton );
	}

	return;
}

void ToolBar::RemoveButton( int ButtonId )
{
	ToolBarButton *pButton = (ToolBarButton *) m_Buttons.FindChild( ButtonId );

	if( pButton )
		delete pButton;

	return;
}

ToolBarButton * ToolBar::GetButtonAtPosition( int x, int y )
{
	RECT rc;
	GetClientRect( &rc );

	int x1, y1;
	int x2, y2;
	int width;
	int height;
	int maxheight;

	x1 = 0;
	y1 = 0;
	maxheight = 0;

	ToolBarButton *pButton = (ToolBarButton *) m_Buttons.Child();

	while( pButton )
	{
		width = pButton->m_Width;
		height = pButton->m_Height;

		if( maxheight < height )
			maxheight = height;

		if( x1 + width > rc.right )
		{
			x1 = 0;
			y1 = y1 + maxheight;
			maxheight = 0;
		}

		x2 = x1 + width;
		y2 = y1 + height;

		if( x >= x1 && x < x2 && y >= y1 && y < y2 )
			return pButton;

		x1 = x1 + width;

		pButton = (ToolBarButton *) pButton->Next();
	}

	return 0;
}

ToolBarButton * ToolBar::NewButton( void )
{
	return new ToolBarButton;
}

void ToolBar::OnMouseMove( UINT Flags, CPoint p )
{
	m_MousePos = p;

	ToolBarButton *pButton = GetButtonAtPosition( p.x, p.y );

	if( m_pButtonDown )
	{
		if( m_pButtonDown == pButton )
			m_pButtonDown->SetButtonDown( FALSE );
		else
			m_pButtonDown->SetButtonDown( TRUE );
	}

	if( m_pButtonHovered != pButton )
		Invalidate( FALSE );

	m_pButtonHovered = pButton;

	return;
}

void ToolBar::OnLButtonDown( UINT Flags, CPoint p )
{
	SetCapture();

	OnMouseMove( Flags, p );

	m_pButtonDown = m_pButtonHovered;

	if( m_pButtonDown )
		m_pButtonDown->SetButtonDown( TRUE );

	Invalidate( FALSE );
}

void ToolBar::OnLButtonUp( UINT Flags, CPoint p )
{
	OnMouseMove( Flags, p );

	if( m_pButtonDown )
	{
		m_pButtonDown->SetButtonDown( FALSE );
	
		if( m_pButtonDown == m_pButtonHovered )
		{
			CWnd *pParent = GetParent();

			if( pParent )
			{
			}
		}

		m_pButtonDown = 0;

		Invalidate();
	}
	
	ReleaseCapture();

	return;
}

void ToolBar::OnRButtonDown( UINT Flags, CPoint p )
{
}

void ToolBar::OnPaint( void )
{
	PAINTSTRUCT ps;
	CDC *pdc = BeginPaint( &ps );

	RECT rc;
	GetClientRect( &rc );

	GdiFillRect( pdc->m_hDC, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, 0xddeeff );

	BITMAP bm;
	HBITMAP hbm;
	int x1, y1;
	int x2, y2;
	int width;
	int height;
	int maxheight;

	int Sx, Sy;
	int Sw, Sh;
	int Dx, Dy;
	int Dw, Dh;

	x1 = 0;
	y1 = 0;
	maxheight = 0;

	ToolBarButton *pButton = (ToolBarButton *) m_Buttons.Child();

	while( pButton )
	{
		hbm = pButton->m_hbmButton;
		bm = GetBitmapStruct( hbm );

		width = pButton->m_Width;
		height = pButton->m_Height;

		if( maxheight < height )
			maxheight = height;

		if( x1 + width > rc.right )
		{
			x1 = 0;
			y1 = y1 + maxheight;
			maxheight = 0;
		}

		x2 = x1 + width;
		y2 = y1 + height;

		if( pButton == m_pButtonHovered )
			GdiFillRect( pdc->m_hDC, x1, y1, width, height, ColorSub( 0xddeeff, 0x222222 ) );

		if( pButton == m_pButtonSelected )
			GdiFillRect( pdc->m_hDC, x1, y1, width, height, ColorSub( 0xddeeff, 0x666666 ) );

		Dx = x1, Dy = y1;
		Dw = width;
		Dh = height;

		Sx = 0, Sy = 0;
		Sw = bm.bmWidth;
		Sh = bm.bmHeight;

		if( pButton->m_ButtonDown )
		{
			Dx += 2;
			Dy += 2;
			Dw -= 2;
			Dh -= 2;
			Sw -= 2;
			Sh -= 2;
		}

		BltBitmapTransparent( pdc->m_hDC, Dx, Dy, Dw, Dh, hbm, Sx, Sy, Sw, Sh, 0x00ff00 );

		x1 = x1 + width;

		pButton = (ToolBarButton *) pButton->Next();
	}

	EndPaint( &ps );
}