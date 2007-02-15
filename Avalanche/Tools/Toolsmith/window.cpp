//=============================================================================//
//  Copyright ©2000 Avalanche Software.  All Rights Reserved.                  //
//=============================================================================//
//      File: window.cpp                                                       //
//    Author: Daniel Silver                                                    //
//   Created: 04/10/2000                                                       //
//=============================================================================//

#include "pch.h"
#include "thread.h"
#include "window.h"

//=============================================================================
// CHandler::CHandler()
//=============================================================================

CHandler::CHandler( void )
{
}

//=============================================================================
// CHandler::~CHandler()
//=============================================================================

CHandler::~CHandler( void )
{
}

CWindow *CWindow::m_pWindows = 0;

//=============================================================================
// CWindow::PreWindowProc() - This function is called when a message arrives
//							  at the message port.  It locates the destination
//							  window and sends the message to the it's message
//							  center.
//=============================================================================

LRESULT CALLBACK CWindow::PreWindowProc( HWND hWindow, UINT message, WPARAM wParam, LPARAM lParam )
{
	CWindow *pWindow = m_pWindows;

	while( pWindow )
	{
		if( pWindow->m_hWindow == hWindow )
			return pWindow->WindowProc( message, wParam, lParam );

		pWindow = pWindow->m_pNextWindow;
	}

	return ::DefWindowProc( hWindow, message, wParam, lParam );
}

//=============================================================================
// CWindow::CWindow() - The constructor of this class appends the window to a
//						singly-linked list of all known windows.
//=============================================================================

CWindow::CWindow()
{
	m_hWindow = 0;
	m_pBaseWndProc = 0;
	m_pNextWindow = 0;
	m_szWindowText[0] = 0;
	m_ptMouse.x = 0;
	m_ptMouse.y = 0;

	if( !m_pWindows )
		m_pWindows = this;
	else
	{
		CWindow *window = m_pWindows;

		while( window->m_pNextWindow )
			window = window->m_pNextWindow;

		window->m_pNextWindow = this;
	}

	return;
}

//=============================================================================
// CWindow::~CWindow() - The destructor of this class removes the window from
//						 the list of known windows and assigns the next window
//						 in the list to be the root should this window be the
//						 current root.
//=============================================================================

CWindow::~CWindow()
{
	DestroyWindow();

	if( m_pWindows == this )
		m_pWindows = m_pNextWindow;
	else
	{
		CWindow *window = m_pWindows;

		while( window )
		{
			if( window->m_pNextWindow == this )
				window->m_pNextWindow = m_pNextWindow;

			window = window->m_pNextWindow;
		}
	}

	return;
}

//=============================================================================
// CWindow::GetWindow()
//=============================================================================

CWindow *CWindow::GetWindow( HWND hWindow )
{
	CWindow *pWindow = m_pWindows;

	while( pWindow )
	{
		if( pWindow->m_hWindow == hWindow )
			return pWindow;

		pWindow = pWindow->m_pNextWindow;
	}

	return 0;
}

//=============================================================================
// CWindow::AttachWindow() - A very powerful method of sub-classing an existing
//							 window, this function removes the current message
//							 handler of a given window and replaces is with its
//							 own. Messages are subsequently directed to various
//							 functions within the class to for processing.  The
//							 original message handler is restored when the
//							 window is detached.
//=============================================================================

BOOL CWindow::AttachWindow( HWND hWindow )
{
	DetachWindow();

	if( (WNDPROC)::GetWindowLong( hWindow, GWL_WNDPROC ) != PreWindowProc )
		m_pBaseWndProc = (WNDPROC)::SetWindowLong( hWindow, GWL_WNDPROC, (LONG)PreWindowProc );

	if( (WNDPROC)::GetWindowLong( hWindow, GWL_WNDPROC ) != PreWindowProc )
		return FALSE;

	m_hWindow = hWindow;

	return TRUE;
}

//=============================================================================
// CWindow::AttachDialogItem() - Sub-classes a control within a dialog box.
//								 It is common to call this function for each
//								 complex control in a dialog after the dialog
//								 has been created.
//=============================================================================

BOOL CWindow::AttachDialogItem( HWND hDialog, int nDialogId )
{
	HWND hControl = ::GetDlgItem( hDialog, nDialogId );

	if( hControl )
		return AttachWindow( hControl );

	return FALSE;
}

//=============================================================================
// CWindow::DetatchWindow() - Removes the custom message handler and restores
//							  the original, allowing the window to operate
//							  normally without custom behaviors.
//=============================================================================

BOOL CWindow::DetachWindow()
{
	if( !m_pBaseWndProc )
		m_pBaseWndProc = ::DefWindowProc;

	if( m_hWindow && (WNDPROC)::GetWindowLong( m_hWindow, GWL_WNDPROC ) == PreWindowProc )
		::SetWindowLong( m_hWindow, GWL_WNDPROC, (LONG)m_pBaseWndProc );

	if( (WNDPROC)::GetWindowLong( m_hWindow, GWL_WNDPROC ) != m_pBaseWndProc )
		return FALSE;

	m_hWindow = 0;
	m_pBaseWndProc = 0;
	
	return TRUE;
}

//=============================================================================
// CWindow::CreateWindow() - Creates a new window of the specified class.  A
//							 hook is installed prior to creating the window,
//							 and removed after the window has been created. The
//							 hook is responsible for sub-classing the window
//							 and making sure the WM_CREATE message is directed
//							 to the message center.  Without the hook, a window
//							 cannot be subclassed.
//=============================================================================

CWindow *CWindow::m_pHookWindow = 0;
HHOOK CWindow::m_hHook = 0;

BOOL CWindow::CreateWindowEx( DWORD dwExStyle, LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle,
										int x, int y, int nWidth, int nHeight, HWND hParent, HMENU hMenu,
										HINSTANCE hInstance, LPVOID lpUserData )
{
	if( !hInstance )
		hInstance = AppInstanceHandle();

	DestroyWindow();

	// Make sure the common controls are initialized before creating a window.

	InitCommonControls();

	if( !lpszClassName )
	{
		WNDCLASS wndclass;

		if( !GetClassInfo( hInstance, Classname(), &wndclass ) )
		{
			wndclass.style					= CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
			wndclass.lpfnWndProc			= PreWindowProc;
			wndclass.cbClsExtra			= 0;
			wndclass.cbWndExtra			= 0;
			wndclass.hInstance			= hInstance;
			wndclass.hIcon					= LoadIcon( hInstance, IDI_APPLICATION );
			wndclass.hCursor				= LoadCursor( NULL, IDC_ARROW );
			wndclass.hbrBackground		= (HBRUSH)(COLOR_WINDOW+1);
			wndclass.lpszMenuName		= 0;
			wndclass.lpszClassName		= Classname();

			if( !RegisterClass( &wndclass ) )
				return FALSE;
		}
		lpszClassName = wndclass.lpszClassName;
	}

	// Hook the Window procedure so we can intercept the WM_CREATE message

	if( !PreCreateWindow() )
		return FALSE;

	// Start the creation of the window.  The hook function will attach the window
	// before this function returns.

	HWND hWindow = ::CreateWindowEx( dwExStyle,
												lpszClassName,
												lpszWindowName,
												dwStyle,
												x,
												y,
												nWidth,
												nHeight,
												hParent,
												hMenu,
												hInstance,
												lpUserData );

	// Uninstall the hook or else!

	PostCreateWindow();

	if( !hWindow )
		return FALSE;

	// Did we connect to the right window?

	if( hWindow == m_hWindow )
		return TRUE;

	// Uh oh, somehow we got the wrong window.  Maybe we can recover =)

	DetachWindow(), AttachWindow( hWindow );

	return TRUE;
}

//=============================================================================
// CWindow::DestroyWindow() - Destroy the window and all child windows.
//=============================================================================

BOOL CWindow::DestroyWindow()
{
	return ::DestroyWindow( m_hWindow );
}

//=============================================================================
// CWindow::ShowWindow() - Display the window.  See ShowWindow in the platform
//						   SDK for more information about this function.
//=============================================================================

BOOL CWindow::ShowWindow( int nCmdShow )
{
	return ::ShowWindow( m_hWindow, nCmdShow );
}

//=============================================================================
// CWindow::UpdateWindow() - Update the contents of the window.  See
//							 UpdateWindow in the platform SDK for more
//							 information about this function.
//=============================================================================

BOOL CWindow::UpdateWindow()
{
	return ::UpdateWindow( m_hWindow );
}

//=============================================================================
// CWindow::WindowHandle() - Get the handle of the current window.
//=============================================================================

HWND CWindow::WindowHandle()
{
	return m_hWindow;
}

//=============================================================================
// CWindow::GetParentHandle() - Get the handle of the parent window.
//=============================================================================

HWND CWindow::ParentHandle()
{
	return ::GetParent( m_hWindow );
}

//=============================================================================
// CWindow::InstanceHandle() - Get the instance handle that created the window.
//=============================================================================

HINSTANCE CWindow::InstanceHandle()
{
	return (HINSTANCE)GetWindowLong( GWL_HINSTANCE );
}

//=============================================================================
// CWindow::Invalidate() - Invalidate the client area, causing it to be
//						   repainted.
//=============================================================================

BOOL CWindow::Invalidate()
{
	return ::InvalidateRect( WindowHandle(), NULL, TRUE );
}

//=============================================================================
// CWindow::ClientWidth() - Get the width of the client area.
//=============================================================================

int CWindow::ClientWidth()
{
	RECT clientRect;
	GetClientRect( &clientRect );

	return clientRect.right - clientRect.left;
}

//=============================================================================
// CWindow::ClientHeight() - Get the height of the client area.
//=============================================================================

int CWindow::ClientHeight()
{
	RECT clientRect;
	GetClientRect( &clientRect );

	return clientRect.bottom - clientRect.top;
}

//=============================================================================
// CWindow::ClientRect() - Returns the client rect.
//=============================================================================

RECT CWindow::ClientRect()
{
	RECT rc;

	GetClientRect( &rc );

	return rc;
}

//=============================================================================
// CWindow::WindowWidth()
//=============================================================================

int CWindow::WindowWidth()
{
	RECT windowRect;
	GetWindowRect( &windowRect );

	return windowRect.right - windowRect.left;
}

//=============================================================================
// CWindow::WindowHeight()
//=============================================================================

int CWindow::WindowHeight()
{
	RECT windowRect;
	GetWindowRect( &windowRect );

	return windowRect.bottom - windowRect.top;
}

//=============================================================================
// CWindow::WindowRect()
//=============================================================================

RECT CWindow::WindowRect()
{
	RECT windowRect;

	GetWindowRect( &windowRect );

	return windowRect;
}

//=============================================================================
// CWindow::SetWindowText() - If the window is overlapped, the window is given
//							  a new name.  If the window is a child or child
//							  control, it is assigned new text.  See
//							  SetWindowText in the platform SDK for more
//							  information about this function.
//=============================================================================

BOOL CWindow::SetWindowText( LPCTSTR pText )
{
	return ::SetWindowText( m_hWindow, pText );
}

//=============================================================================
// CWindow::GetWindowText() - This function returns a temporary pointer to a
//							  string containing the name of the window or text
//							  of a child control.  The pointer is only valid
//							  until the next time this function is called, or
//							  the window is destroyed.  The pointer returned by
//							  this function should be used immediately, and
//							  should not be stored for later use.
//=============================================================================

LPCTSTR CWindow::GetWindowText()
{
	::GetWindowText( m_hWindow, m_szWindowText, sizeof( m_szWindowText ) / sizeof( TCHAR ) );

	return m_szWindowText;
}

//=============================================================================
// CWindow::GetWindowTextLength() - Get the number of characters in the string.
//=============================================================================

int CWindow::GetWindowTextLength()
{
	return ::GetWindowTextLength( m_hWindow );
}

//=============================================================================
// CWindow::CopyWindowText() - Copy the window name or the text of a child
//							   control into the buffer.
//=============================================================================

int CWindow::CopyWindowText( LPTSTR pText, int nMaxSize )
{
	return ::GetWindowText( m_hWindow, pText, nMaxSize );
}

//=============================================================================
// CWindow::GetClientRect() - See GetClientRect in the platform SDK for more
//							  information about this function.
//=============================================================================

BOOL CWindow::GetClientRect( RECT *pClientRect )
{
	return ::GetClientRect( m_hWindow, pClientRect );
}

//=============================================================================
// CWindow::GetWindowRect() - See GetWindowRect in the platform SDK for more
//							  information about this function.
//=============================================================================

BOOL CWindow::GetWindowRect( RECT *pWindowRect )
{
	return ::GetWindowRect( m_hWindow, pWindowRect );
}

//=============================================================================
// CWindow::SetWindowLong()
//=============================================================================

LONG CWindow::SetWindowLong( int nIndex, LONG dwNewLong )
{
	return ::SetWindowLong( m_hWindow, nIndex, dwNewLong );
}

//=============================================================================
// CWindow::GetWindowLong()
//=============================================================================

LONG CWindow::GetWindowLong( int nIndex )
{
	return ::GetWindowLong( m_hWindow, nIndex );
}

//=============================================================================
// CWindow::BeginPaint()
//=============================================================================

HDC CWindow::BeginPaint( PAINTSTRUCT *pPaintStruct )
{
	return ::BeginPaint( m_hWindow, pPaintStruct );
}

//=============================================================================
// CWindow::EndPaint()
//=============================================================================

BOOL CWindow::EndPaint( PAINTSTRUCT *pPaintStruct )
{
	return ::EndPaint( m_hWindow, pPaintStruct );
}

//=============================================================================
// CWindow::SendMessage()
//=============================================================================

LRESULT CWindow::SendMessage( UINT Msg, WPARAM wParam, LPARAM lParam )
{
	return ::SendMessage( m_hWindow, Msg, wParam, lParam );
}

//=============================================================================
// CWindow::PostMessage()
//=============================================================================

BOOL CWindow::PostMessage( UINT Msg, WPARAM wParam, LPARAM lParam )
{
	return ::PostMessage( m_hWindow, Msg, wParam, lParam );
}

//=============================================================================
// CWindow::EnableWindow()
//=============================================================================

BOOL CWindow::EnableWindow()
{
	return ::EnableWindow( WindowHandle(), TRUE );
}

//=============================================================================
// CWindow::DisableWindow()
//=============================================================================

BOOL CWindow::DisableWindow()
{
	return ::EnableWindow( WindowHandle(), FALSE );
}

//=============================================================================
// CWindow::MessageBox()
//=============================================================================

int CWindow::MessageBox( LPCTSTR lpText, UINT uType )
{
	return ::MessageBox( WindowHandle(), lpText, GetWindowText(), uType );
}

//=============================================================================
// CWindow::MoveWindow()
//=============================================================================

BOOL CWindow::MoveWindow( int x, int y, int nWidth, int nHeight, BOOL bRepaint )
{
	return ::MoveWindow( WindowHandle(), x, y, nWidth, nHeight, bRepaint );
}

//=============================================================================
// CWindow::SetMenu()
//=============================================================================

BOOL CWindow::SetMenu( HMENU hMenu )
{
	return ::SetMenu( m_hWindow, hMenu );
}

//=============================================================================
// CWindow::GetMenu()
//=============================================================================

HMENU CWindow::GetMenu( void )
{
	return ::GetMenu( m_hWindow );
}

//=============================================================================
// CWindow::LoadMenu()
//=============================================================================

BOOL CWindow::LoadMenu( int nResource )
{
	return SetMenu( ::LoadMenu( InstanceHandle(), MAKEINTRESOURCE(nResource) ) );
}

//=============================================================================
// CWindow::DestroyMenu()
//=============================================================================

BOOL CWindow::DestroyMenu( void )
{
	return ::DestroyMenu( GetMenu() );
}

//=============================================================================
// CWindow::Mouse()
//=============================================================================

POINT CWindow::Mouse( void )
{
	return m_ptMouse;
}

//=============================================================================
// CWindow::SendDlgItemMessage()
//=============================================================================

LRESULT CWindow::SendDlgItemMessage( int nID, UINT message, WPARAM wParam, LPARAM lParam )
{
	return ::SendDlgItemMessage( WindowHandle(), nID, message, wParam, lParam );
}

//=============================================================================
// CWindow::GetDlgItem()
//=============================================================================

HWND CWindow::GetDlgItem( int nID )
{
	return ::GetDlgItem( WindowHandle(), nID );
}

//=============================================================================
// CWindow::GetDlgItemText()
//=============================================================================

int CWindow::GetDlgItemText( int nID, LPTSTR lpszString, int nMaxCount )
{
	return ::GetDlgItemText( WindowHandle(), nID, lpszString, nMaxCount );
}

//=============================================================================
// CWindow::SetDlgItemText()
//=============================================================================

BOOL CWindow::SetDlgItemText( int nID, LPCTSTR lpszString )
{
	TCHAR szItemText[64] = "";

	GetDlgItemText( nID, szItemText, sizeof( szItemText ) / sizeof( szItemText[0] ) );

	if( strcmp( szItemText, lpszString ) )
		return ::SetDlgItemText( WindowHandle(), nID, lpszString );

	return TRUE;
}

//=============================================================================
// CWindow::GetDlgItemInt()
//=============================================================================

int CWindow::GetDlgItemInt( int nID )
{
	if( IsDlgItemClass( nID, "Scrollbar" ) ||
		IsDlgItemClass( nID, TRACKBAR_CLASS ) ||
		IsDlgItemClass( nID, PROGRESS_CLASS ) ||
		IsDlgItemClass( nID, UPDOWN_CLASS ) )
	{
		return GetDlgItemPos( nID );
	}

	return ::GetDlgItemInt( WindowHandle(), nID, NULL, TRUE );
}

//=============================================================================
// CWindow::SetDlgItemInt()
//=============================================================================

void CWindow::SetDlgItemInt( int nID, int nNumber )
{
	if( IsDlgItemClass( nID, "Scrollbar" ) ||
		IsDlgItemClass( nID, TRACKBAR_CLASS ) ||
		IsDlgItemClass( nID, PROGRESS_CLASS ) ||
		IsDlgItemClass( nID, UPDOWN_CLASS ) )
		SetDlgItemPos( nID, nNumber );
	else
		::SetDlgItemInt( WindowHandle(), nID, nNumber, TRUE );

	return;
}

//=============================================================================
// CWindow::GetDlgItemValue()
//=============================================================================

double CWindow::GetDlgItemValue( int nID )
{
	if( IsDlgItemClass( nID, "SCROLLBAR" ) ||
		IsDlgItemClass( nID, TRACKBAR_CLASS ) ||
		IsDlgItemClass( nID, PROGRESS_CLASS ) ||
		IsDlgItemClass( nID, UPDOWN_CLASS ) )
	{
		int nMin = 0, nMax = 0;

		GetDlgItemRange( nID, &nMin, &nMax );

		if( nMin != nMax )
			return (double)(GetDlgItemPos( nID ) - nMin) / (double)(nMax - nMin);
	}
	
	TCHAR szItemText[64];

	if( !GetDlgItemText( nID, szItemText, sizeof( szItemText ) / sizeof( szItemText[0] ) ) )
		return 0.0;

	return atof( szItemText );
}

//=============================================================================
// CWindow::SetDlgItemValue()
//=============================================================================

void CWindow::SetDlgItemValue( int nID, double dValue )
{
	if( IsDlgItemClass( nID, "SCROLLBAR" ) ||
		IsDlgItemClass( nID, TRACKBAR_CLASS ) ||
		IsDlgItemClass( nID, PROGRESS_CLASS ) ||
		IsDlgItemClass( nID, UPDOWN_CLASS ) )
	{
		int nMin = 0, nMax = 0;

		GetDlgItemRange( nID, &nMin, &nMax );

		SetDlgItemPos( nID, (int)(dValue * (double)(nMax - nMin) + 0.5) + nMin );

		return;
	}

	char szItemText[64] = "";

	sprintf( szItemText, "%.05f", dValue );

	SetDlgItemText( nID, szItemText );
}

//=============================================================================
// CWindow::GetDlgItemPos()
//=============================================================================

int CWindow::GetDlgItemPos( int nID )
{
	if( IsDlgItemClass( nID, "SCROLLBAR" ) )
		return SendDlgItemMessage( nID, SBM_GETPOS, 0, 0 );
	else if( IsDlgItemClass( nID, TRACKBAR_CLASS ) )
		return SendDlgItemMessage( nID, TBM_GETPOS, 0, 0 );
	else if( IsDlgItemClass( nID, PROGRESS_CLASS ) )
		return SendDlgItemMessage( nID, PBM_GETPOS, 0, 0 );
	else if( IsDlgItemClass( nID, UPDOWN_CLASS ) )
		return LOWORD(SendDlgItemMessage( nID, UDM_GETPOS, 0, 0 ));
	
	return 0;
}

//=============================================================================
// CWindow::SetDlgItemPos()
//=============================================================================
void CWindow::SetDlgItemPos( int nID, int nPosition )
{
	if( IsDlgItemClass( nID, "SCROLLBAR" ) )
		SendDlgItemMessage( nID, SBM_SETPOS, nPosition, TRUE );
	else if( IsDlgItemClass( nID, TRACKBAR_CLASS ) )
		SendDlgItemMessage( nID, TBM_SETPOS, TRUE, nPosition );
	else if( IsDlgItemClass( nID, PROGRESS_CLASS ) )
		SendDlgItemMessage( nID, PBM_SETPOS, nPosition, 0 );
	else if( IsDlgItemClass( nID, UPDOWN_CLASS ) )
		SendDlgItemMessage( nID, UDM_SETPOS, 0, MAKELPARAM( nPosition, 0 ) );

	return;
}

//=============================================================================
// CWindow::GetDlgItemRange()
//=============================================================================

BOOL CWindow::GetDlgItemRange( int nID, LPINT lpMin, LPINT lpMax )
{
	int nMin = 0, nMax = 0;

	if( !lpMin )
		lpMin = &nMin; // lpMin must be valid because there is no pointer validation later on.

	if( !lpMax )
		lpMax = &nMax; // lpMax must be valid because there is no pointer validation later on.

	if( IsDlgItemClass( nID, "Scrollbar" ) )
	{
		SendDlgItemMessage( nID, SBM_GETRANGE, (WPARAM)lpMin, (LPARAM)lpMax );
		return TRUE;
	}
	else if( IsDlgItemClass( nID, TRACKBAR_CLASS ) )
	{
		*lpMin = SendDlgItemMessage( nID, TBM_GETRANGEMIN, 0, 0 );
		*lpMax = SendDlgItemMessage( nID, TBM_GETRANGEMAX, 0, 0 );

		return TRUE;
	}
	else if( IsDlgItemClass( nID, PROGRESS_CLASS ) )
	{
		*lpMin = SendDlgItemMessage( nID, PBM_GETRANGE, TRUE, NULL );
		*lpMax = SendDlgItemMessage( nID, PBM_GETRANGE, FALSE, NULL );

		return TRUE;
	}
	else if( IsDlgItemClass( nID, UPDOWN_CLASS ) )
	{
		LRESULT lresult = SendDlgItemMessage( nID, UDM_GETRANGE, 0, 0 );

		*lpMin = LOWORD( lresult );
		*lpMax = LOWORD( lresult );

		return TRUE;
	}

	return FALSE;
}

//=============================================================================
// CWindow::SetDlgItemRange()
//=============================================================================

void CWindow::SetDlgItemRange( int nID, int nMin, int nMax )
{
	if( IsDlgItemClass( nID, "Scrollbar" ) )
		SendDlgItemMessage( nID, SBM_SETRANGE, (WPARAM)nMin, (LPARAM)nMax );
	else if( IsDlgItemClass( nID, TRACKBAR_CLASS ) )
		SendDlgItemMessage( nID, TBM_SETRANGE, TRUE, MAKELONG( nMin, nMax ) );
	else if( IsDlgItemClass( nID, PROGRESS_CLASS ) )
		SendDlgItemMessage( nID, PBM_SETRANGE, 0, MAKELONG( nMin, nMax ) );
	else if( IsDlgItemClass( nID, UPDOWN_CLASS ) )
		SendDlgItemMessage( nID, UDM_SETRANGE, TRUE, MAKELONG( nMin, nMax ) );

	return;
}

//=============================================================================
// CWindow::GetDlgItemCheck()
//=============================================================================

int CWindow::GetDlgItemCheck( int nID )
{
	char szClassName[64];

	if( GetClassName( GetDlgItem( nID ), szClassName, sizeof( szClassName ) / sizeof( szClassName[0] ) ) )
	{
		if( IsDlgItemClass( nID, "Button" ) )
			return SendDlgItemMessage( nID, BM_GETCHECK, 0, 0 );
	}

	return BST_UNCHECKED;
}

//=============================================================================
// CWindow::CheckDlgItem()
//=============================================================================

void CWindow::CheckDlgItem( int nID )
{
	SendDlgItemMessage( nID, BM_SETCHECK, (WPARAM)BST_CHECKED, 0 );
}

//=============================================================================
// CWindow::UncheckDlgItem()
//=============================================================================

void CWindow::UncheckDlgItem( int nID )
{
	SendDlgItemMessage( nID, BM_SETCHECK, (WPARAM)BST_UNCHECKED, 0 );
}

//=============================================================================
// CWindow::IsDlgItemEnabled()
//=============================================================================

BOOL CWindow::GetDlgItemState( int nID )
{
	return ::IsWindowEnabled( GetDlgItem( nID ) );
}

//=============================================================================
// CWindow::SetDlgItemState()
//=============================================================================

void CWindow::SetDlgItemState( int nID, BOOL bEnabled )
{
	::EnableWindow( GetDlgItem( nID ), bEnabled );
	::UpdateWindow( GetDlgItem( nID ) );
}

//=============================================================================
// CWindow::EnableDlgItem()
//=============================================================================

void CWindow::EnableDlgItem( int nID )
{
	SetDlgItemState( nID, TRUE );
}

//=============================================================================
// CWindow::DisableDlgItem()
//=============================================================================

void CWindow::DisableDlgItem( int nID )
{
	SetDlgItemState( nID, FALSE );
}

//=============================================================================
// CWindow::IsDlgItemClass()
//=============================================================================

BOOL CWindow::IsDlgItemClass( int nID, LPCTSTR lpszClassName )
{
	TCHAR szClassName[128];

	if( !GetClassName( GetDlgItem( nID ), szClassName, sizeof( szClassName ) / sizeof( szClassName[0] ) ) )
		return FALSE;

	if( stricmp( szClassName, lpszClassName ) )
		return FALSE;

	return TRUE;
}

//=============================================================================
// CWindow::WindowProc() - The message center.  All messages arrive here before
//								   being sent to the appropriate message handler.
//=============================================================================

LRESULT CWindow::WindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	m_lResult = 0;

	switch( message )
	{
	case WM_CREATE:
		if( FALSE == WhenCreate( (LPCREATESTRUCT)lParam ) )
			m_lResult = -1;

		return m_lResult;

	case WM_DESTROY:
		WhenDestroy(), DetachWindow();
		return m_lResult;

	case WM_ACTIVATE:
		WhenActivate( LOWORD(wParam), HIWORD(wParam), (HWND)lParam );
		return m_lResult;

	case WM_ACTIVATEAPP:
		WhenActivateApp( (BOOL)wParam, (DWORD)lParam );
		return m_lResult;

	case WM_CAPTURECHANGED:
		WhenCaptureChanged( (HWND)lParam );
		return m_lResult;

	case WM_CHAR:
		WhenChar( (TCHAR)wParam, (int)(0xffff & lParam), (0xff0000 & lParam) >> 16, (DWORD)lParam );
		return m_lResult;

	case WM_CLOSE:
		WhenClose();
		return m_lResult;
	
	case WM_COMMAND:
	
	//	if( lParam && (HWND)lParam != m_hWindow )
	//	{
	//		CWindow *pWindow = CWindow::GetWindow( (HWND) lParam );
	//
	//		if( pWindow )
	//		{
	//			LRESULT lresult = pWindow->SendMessage( message, wParam, 0 );
	//
	//			if( lresult )
	//				return lresult;
	//		}
	//	}
		return WhenCommand( (HWND)lParam, LOWORD(wParam), HIWORD(wParam) );

	case WM_CONTEXTMENU:
		WhenContextMenu( (HWND)wParam, LOWORD(lParam), HIWORD(lParam) );
		return m_lResult;

	case WM_ERASEBKGND:
		WhenEraseBkgnd( (HDC)wParam );
		return m_lResult;

	case WM_INITDIALOG:
		WhenInitDialog();
		return m_lResult;

	case WM_HSCROLL:
		WhenHScroll( (int)LOWORD(wParam), (int)HIWORD(wParam), (HWND)lParam );
		return m_lResult;

	case WM_VSCROLL:
		WhenVScroll( (int)LOWORD(wParam), (int)HIWORD(wParam), (HWND)lParam );
		return m_lResult;

	case WM_LBUTTONDBLCLK:
		WhenLButtonDblClk( (DWORD)wParam, LOWORD(lParam), HIWORD(lParam) );
		return m_lResult;

	case WM_LBUTTONDOWN:
		WhenLButtonDown( (DWORD)wParam, LOWORD(lParam), HIWORD(lParam) );
		return m_lResult;

	case WM_LBUTTONUP:
		WhenLButtonUp( (DWORD)wParam, LOWORD(lParam), HIWORD(lParam) );
		return m_lResult;

	case WM_RBUTTONDBLCLK:
		WhenRButtonDblClk( (DWORD)wParam, LOWORD(lParam), HIWORD(lParam) );
		return m_lResult;

	case WM_RBUTTONDOWN:
		WhenRButtonDown( (DWORD)wParam, LOWORD(lParam), HIWORD(lParam) );
		return m_lResult;

	case WM_RBUTTONUP:
		WhenRButtonUp( (DWORD)wParam, LOWORD(lParam), HIWORD(lParam) );
		return m_lResult;

	case WM_MOUSEMOVE:
		m_ptMouse.x = LOWORD(lParam);
		m_ptMouse.y = HIWORD(lParam);
		WhenMouseMove( (DWORD)wParam, LOWORD(lParam), HIWORD(lParam) );
		return m_lResult;

	case WM_NOTIFY:
		return WhenNotify( ((LPNMHDR)lParam)->code, (int)LOWORD(wParam), ((LPNMHDR)lParam)->hwndFrom, lParam );

	case WM_PAINT:

		m_hUpdateRgn = CreateRectRgn( 0, 0, 0, 0 );

		GetUpdateRgn( m_hWindow, m_hUpdateRgn, FALSE );

		if( wParam )
			WhenPaint( (HDC) wParam, 0 );
		else
		{
			PAINTSTRUCT paintstruct;

			WhenPaint( BeginPaint( &paintstruct ), &paintstruct );

			EndPaint( &paintstruct );
		}

		DeleteObject( m_hUpdateRgn );

		return m_lResult;

	case WM_SIZE:
		WhenSize( (DWORD)wParam, LOWORD( lParam ), HIWORD( lParam ) );
		return m_lResult;

	case WM_TIMER:
		WhenTimer( wParam, (TIMERPROC *) lParam );
		return m_lResult;
	}

	return DefWindowProc( message, wParam, lParam );
}

//=============================================================================
// CWindow::DefWindowProc()
//=============================================================================

LRESULT CWindow::DefWindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	if( !m_pBaseWndProc )
		m_lResult = ::DefWindowProc( m_hWindow, message, wParam, lParam );
	else
		m_lResult = CallWindowProc(m_pBaseWndProc, m_hWindow, message, wParam, lParam );

	return m_lResult;
}

//=============================================================================
// CWindow::WhenCreate()
//=============================================================================

BOOL CWindow::WhenCreate( LPCREATESTRUCT lpCS )
{
	if( -1 == DefWindowProc( WM_CREATE, 0, (LPARAM)lpCS ) )
		return FALSE;

	SendMessage( WM_SETFONT, (WPARAM) GetStockObject( DEFAULT_GUI_FONT ), FALSE );

	return TRUE;
}

//=============================================================================
// CWindow::WhenDestroy()
//=============================================================================

void CWindow::WhenDestroy()
{
	DefWindowProc( WM_DESTROY, 0, 0 );
}

//=============================================================================
// CWindow::WhenActivate()
//=============================================================================

void CWindow::WhenActivate( WORD fActive, BOOL bMinimized, HWND hOtherWindow )
{
	DefWindowProc( WM_ACTIVATE, MAKEWPARAM(fActive, bMinimized), (LPARAM)hOtherWindow );
}

//=============================================================================
// CWindow::WhenActivateApp()
//=============================================================================

void CWindow::WhenActivateApp( BOOL bActive, DWORD dwThreadID )
{
	DefWindowProc( WM_ACTIVATEAPP, (WPARAM)bActive, (LPARAM)dwThreadID );
}

//=============================================================================
// CWindow::WhenCaptureChanged()
//=============================================================================

void CWindow::WhenCaptureChanged( HWND hWindow )
{
	DefWindowProc( WM_CAPTURECHANGED, 0, (LPARAM)hWindow );
}

//=============================================================================
// CWindow::WhenChar()
//=============================================================================

void CWindow::WhenChar( TCHAR tch, int nRepeat, int nScanCode, DWORD dwKeyState )
{
	DefWindowProc( WM_CHAR, (WPARAM)tch, (LPARAM)((dwKeyState & 0xff000000) | nRepeat | (nScanCode << 16) ) );
}

//=============================================================================
// CWindow::WhenClose()
//=============================================================================

void CWindow::WhenClose()
{
	DefWindowProc( WM_CLOSE, 0, 0 );
}

//=============================================================================
// CWindow::WhenCommand()
//=============================================================================

BOOL CWindow::WhenCommand( HWND hWindow, int nCommand, int nNotifyCode )
{
	return DefWindowProc( WM_COMMAND, MAKEWPARAM( nCommand, nNotifyCode ), (LPARAM)hWindow );
}

//=============================================================================
// CWindow::WhenContextMenu()
//=============================================================================

void CWindow::WhenContextMenu( HWND hWindow, int x, int y )
{
	DefWindowProc( WM_CONTEXTMENU, (WPARAM)hWindow, MAKELONG( x, y ) );
}

//=============================================================================
// CWindow::WhenEraseBkgnd()
//=============================================================================

void CWindow::WhenEraseBkgnd( HDC hdcErase )
{
	DefWindowProc( WM_ERASEBKGND, (WPARAM)hdcErase, 0 );
}

//=============================================================================
// CWindow::WhenHScroll()
//=============================================================================

void CWindow::WhenHScroll( int nCode, int nPos, HWND hWindow )
{
	DefWindowProc( WM_HSCROLL, MAKEWPARAM( nCode, nPos ), (LPARAM)hWindow );
}

//=============================================================================
// CWindow::WhenVScroll()
//=============================================================================

void CWindow::WhenVScroll( int nCode, int nPos, HWND hWindow )
{
	DefWindowProc( WM_VSCROLL, MAKEWPARAM( nCode, nPos ), (LPARAM)hWindow );
}

//=============================================================================
// CWindow::WhenInitDialog()
//=============================================================================

BOOL CWindow::WhenInitDialog( void )
{
	return FALSE;
}

//=============================================================================
// CWindow::WhenLButtonDblClk()
//=============================================================================

void CWindow::WhenLButtonDblClk( DWORD dwFlags, int x, int y )
{
	DefWindowProc( WM_LBUTTONDBLCLK, (WPARAM)dwFlags, MAKELONG(x, y) );
}

//=============================================================================
// CWindow::WhenLButtonDown()
//=============================================================================

void CWindow::WhenLButtonDown( DWORD dwFlags, int x, int y )
{
	DefWindowProc( WM_LBUTTONDOWN, (WPARAM)dwFlags, MAKELONG(x, y) );
}

//=============================================================================
// CWindow::WhenLButtonUp()
//=============================================================================

void CWindow::WhenLButtonUp( DWORD dwFlags, int x, int y )
{
	DefWindowProc( WM_LBUTTONUP, (WPARAM)dwFlags, MAKELONG(x, y) );
}

//=============================================================================
// CWindow::WhenRButtonDblClk()
//=============================================================================

void CWindow::WhenRButtonDblClk( DWORD dwFlags, int x, int y )
{
	DefWindowProc( WM_RBUTTONDBLCLK, (WPARAM)dwFlags, MAKELONG(x, y) );
}

//=============================================================================
// CWindow::WhenRButtonDown()
//=============================================================================

void CWindow::WhenRButtonDown( DWORD dwFlags, int x, int y )
{
	DefWindowProc( WM_RBUTTONDOWN, (WPARAM)dwFlags, MAKELONG(x, y) );
}

//=============================================================================
// CWindow::WhenRButtonUp()
//=============================================================================

void CWindow::WhenRButtonUp( DWORD dwFlags, int x, int y )
{
	DefWindowProc( WM_RBUTTONUP, (WPARAM)dwFlags, MAKELONG(x, y) );
}

//=============================================================================
// CWindow::WhenMouseMove()
//=============================================================================

void CWindow::WhenMouseMove( DWORD dwFlags, int x, int y )
{
	DefWindowProc( WM_MOUSEMOVE, (WPARAM)dwFlags, MAKELONG(x, y) );
}

//=============================================================================
// CWindow::WhenNotify()
//=============================================================================

LRESULT CWindow::WhenNotify( UINT nCode, int nSender, HWND hWindow, LPARAM lParam )
{
	if( lParam )
	{
		((LPNMHDR)lParam)->code = nCode;
		((LPNMHDR)lParam)->hwndFrom = hWindow;
		((LPNMHDR)lParam)->idFrom = nSender;
	}

	return DefWindowProc( WM_NOTIFY, (WPARAM)nSender, lParam );
}

//=============================================================================
// CWindow::WhenPaint()
//=============================================================================

void CWindow::WhenPaint( HDC hdcPaint, PAINTSTRUCT *paintstruct )
{
	WPARAM wParam = 0;

	if( paintstruct )
		InvalidateRgn( m_hWindow, m_hUpdateRgn, FALSE );
	
	if( !paintstruct )
		wParam = (WPARAM) hdcPaint;

	DefWindowProc( WM_PAINT, wParam, 0 );
}

//=============================================================================
// CWindow::WhenSize()
//=============================================================================

void CWindow::WhenSize( DWORD dwSizeType, int nWidth, int nHeight )
{
	DefWindowProc( WM_SIZE, (WPARAM)dwSizeType, MAKELPARAM( nWidth, nHeight ) );
}

//=============================================================================
// CWindow::WhenTimer()
//=============================================================================

void CWindow::WhenTimer( int nTimerID, TIMERPROC *lpfnTimerProc )
{
	DefWindowProc( WM_TIMER, nTimerID, (LPARAM) lpfnTimerProc );
}

//=============================================================================
// CWindow::HookFunction()
//=============================================================================

LRESULT CALLBACK CWindow::HookFunction( int nCode, WPARAM wParam, LPARAM lParam )
{
	LRESULT result = CallNextHookEx( m_hHook, nCode, wParam, lParam );

	if( nCode == HCBT_CREATEWND )
	{
		if( m_pHookWindow )
		{
			m_pHookWindow->AttachWindow( (HWND)wParam );
			m_pHookWindow->PostCreateWindow();
		}

		// This is just extra protection against an accidental runaway hook.

		if( m_hHook )
			UnhookWindowsHookEx( m_hHook ), m_hHook = NULL;
	}

	return result;
}

//=============================================================================
// CWindow::PreCreateWindow()
//=============================================================================

BOOL CWindow::PreCreateWindow()
{
	if( m_hHook )
		if( !PostCreateWindow() )
			return FALSE;

	m_hHook = SetWindowsHookEx( WH_CBT, HookFunction, NULL, GetCurrentThreadId() );

	if( !m_hHook )
		return FALSE;

	m_pHookWindow = this;

	return TRUE;
}

//=============================================================================
// CWindow::PostCreateWindow()
//=============================================================================

BOOL CWindow::PostCreateWindow()
{
	if( m_hHook && !UnhookWindowsHookEx( m_hHook ) )
		return FALSE;

	m_pHookWindow = 0;
	m_hHook = 0;

	return TRUE;
}


