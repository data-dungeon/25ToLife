#include "pch.h"
#include "application.h"
#include "textentry.h"

#define IDC_EDIT_CONTROL		(0x101)

BEGIN_MESSAGE_MAP( TextEntryWindow, CWnd )
	ON_EN_CHANGE( IDC_EDIT_CONTROL, OnTextEditChange )
	ON_EN_UPDATE( IDC_EDIT_CONTROL, OnTextEditUpdate )
	ON_WM_CREATE()
	ON_WM_MOUSEACTIVATE()
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_WM_SIZE()
	ON_NOTIFY( EN_WINDOWMESSAGE, IDC_EDIT_CONTROL, OnEditNotify )
END_MESSAGE_MAP()

TextEntryWindow::TextEntryWindow( void )
{
	m_NotifyWnd = 0;
	m_NotifyControlId = 0;
}

TextEntryWindow::~TextEntryWindow()
{
}

void TextEntryWindow::SetNotification( HWND hWnd, UINT ControlId )
{
	m_NotifyWnd = hWnd;
	m_NotifyControlId = ControlId;
}

int TextEntryWindow::OnCreate( LPCREATESTRUCT pcs )
{
	if( -1 == CWnd::OnCreate( pcs ) )
		return -1;

	RECT rc;
	GetClientRect( &rc );

	DWORD dwStyle = WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL|ES_AUTOVSCROLL|ES_MULTILINE;

	BOOL Result = m_EditCtrl.Create( dwStyle, rc, this, IDC_EDIT_CONTROL );

	if( Result )
	{
		m_EditCtrl.SetFont( App.DefaultGuiFont() );
		m_EditCtrl.SetWindowText( "" );
		m_EditCtrl.ShowWindow( SW_SHOW );
		m_EditCtrl.UpdateWindow();
	}

	return 0;
}

int TextEntryWindow::OnMouseActivate( CWnd *pDesktopWnd, UINT HitTest, UINT message )
{
	return CWnd::OnMouseActivate( pDesktopWnd, HitTest, message );
}

void TextEntryWindow::OnPaint( void )
{
	PAINTSTRUCT ps;
	CDC *pdc = BeginPaint( &ps );

	EndPaint( &ps );
}

void TextEntryWindow::OnSetFocus( CWnd *pOldWnd )
{
	CWnd::OnSetFocus( pOldWnd );

	m_EditCtrl.SetSel( 0, -1 );
	m_EditCtrl.SetFocus();
}

void TextEntryWindow::OnSize( UINT Type, int x, int y )
{
	RECT rc;
	GetClientRect( &rc );

	m_EditCtrl.MoveWindow( &rc, FALSE );
}

void TextEntryWindow::OnTextEditUpdate( void )
{
	HWND NotifyWnd = m_NotifyWnd;

	if( !NotifyWnd )
		NotifyWnd = ::GetParent( m_hWnd );

	::SendNotifyMessage( NotifyWnd, WM_COMMAND, MAKEWPARAM( m_NotifyControlId, EN_UPDATE ), (LPARAM) m_hWnd );
}

void TextEntryWindow::OnTextEditChange( void )
{
	HWND NotifyWnd = m_NotifyWnd;

	if( !NotifyWnd )
		NotifyWnd = ::GetParent( m_hWnd );

	::SendNotifyMessage( NotifyWnd, WM_COMMAND, MAKEWPARAM( m_NotifyControlId, EN_CHANGE ), (LPARAM) m_hWnd );
}

void TextEntryWindow::OnEditNotify( NMHDR *pNotify, LRESULT *pResult )
{
	if( pNotify )
	{
		NOTIFYEDITSTRUCT *n = (NOTIFYEDITSTRUCT *) pNotify;

		if( n->message == WM_KEYDOWN )
		{
			if( n->wParam == VK_SHIFT )
				m_ShiftKeyDown = TRUE;
		}

		if( n->message == WM_KEYUP )
		{
			if( n->wParam == VK_SHIFT )
				m_ShiftKeyDown = FALSE;
		}

		if( n->message == WM_CHAR && n->wParam == '\r' )
		{
			DWORD dwStyle = ::GetWindowLong( n->nmhdr.hwndFrom, GWL_STYLE );

			if( !(dwStyle & ES_WANTRETURN) && !m_ShiftKeyDown )
			{
				n->message = WM_NULL;
				n->wParam = 0;
				n->lParam = 0;
			
				SendMessage( WM_CLOSE, 0, 0 );
			}
		}
	}

	return;
}