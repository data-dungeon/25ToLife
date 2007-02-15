//=============================================================================//
//  Copyright ©2000 Avalanche Software.  All Rights Reserved.                  //
//=============================================================================//
//      File: window.h                                                         //
//    Author: Daniel Silver                                                    //
//   Created: 04/10/2000                                                       //
//=============================================================================//

#ifndef __WINDOW_H__
#define __WINDOW_H__

#include <windows.h>
#include <commctrl.h>

#define RES MAKEINTRESOURCE

#define RC_DLGINIT RES(240)
#define RC_TOOLBAR RES(241)

typedef HRSRC HRESOURCE;

//=============================================================================
// class CWindow
//
//		The base window class of all windows, controls and dialogs.
//
//=============================================================================

#define DECLARE_CLASS( classname )\
	public: virtual LPCTSTR Classname( void ) { return (LPCTSTR)(classname *)#classname; };

class CHandler
{
public:
	CHandler( void );
	virtual ~CHandler();
};

class CWindow : public CHandler
{
	DECLARE_CLASS( CWindow );

public:
	static LRESULT CALLBACK PreWindowProc( HWND hWindow, UINT message, WPARAM wParam, LPARAM lParam );

public:
	CWindow();
	virtual ~CWindow();

public:
	static CWindow *GetWindow( HWND hWindow );
	virtual BOOL AttachWindow( HWND hWindow );							// Connect to a window, such as a dialog box.
	virtual BOOL AttachDialogItem( HWND hDialog, int nDialogId );	// Connect to a dialog item control, such as a button or list box.
	virtual BOOL DetachWindow();												// Disconnect from a window without destroying it.
	virtual BOOL CreateWindowEx( DWORD dwExStyle, LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle,
										  int x, int y, int nWidth, int nHeight, HWND hParent, HMENU hMenu,
										  HINSTANCE hInstance, LPVOID lpUserData );
	virtual BOOL DestroyWindow();												// Disconnect from a window and destroy it.
	virtual BOOL ShowWindow( int nCmdShow );								// See "ShowWindow" in the Platform SDK for more information.
	virtual BOOL UpdateWindow();												// See "UpdateWindow" in the Platform SDK for more information.
	virtual HWND WindowHandle();												// Returns a handle to the window.
	virtual HWND ParentHandle();												// Returns a handle to the window's parent.
	virtual HINSTANCE InstanceHandle();										// Returns the instance handle that created this window.
	virtual BOOL Invalidate();													// Invalidates the contents of the window's client area, causing it be repainted.
	virtual int ClientWidth();													// Returns the width of the window's client area.
	virtual int ClientHeight();												// Returns the height of the window's client area.
	virtual RECT ClientRect();													// Returns the client rect
	virtual int WindowWidth();													// Returns the width of the window
	virtual int WindowHeight();												// Returns the height of the window
	virtual RECT WindowRect();													// Returns the window rect
	virtual BOOL SetWindowText( LPCTSTR pText );							// See "SetWindowText" in the Platform SDK for more information.
	virtual LPCTSTR GetWindowText();											// Returns a pointer to a string that contains the name of this window.
																						//   Each time you call this function, the old string is destroyed
																						//   and a new one is created.  Thus, you should not store this pointer for
																						//   permanent use because it may be moved if this function is called again.
	virtual int GetWindowTextLength();										// Returns the length, in characters, of the window text.
	virtual int CopyWindowText( LPTSTR pText, int nMaxSize );		// Get a copy of the window text and put in the specified buffer.
	virtual BOOL GetClientRect( RECT *pRect );							// See "GetClientRect" in the Platform SDK for more information.
	virtual BOOL GetWindowRect( RECT *pRect );							// See "GetWindowRect" in the Platform SDK for more information.
	virtual LONG SetWindowLong( int nIndex, LONG dwNewLong );		// Set "SetWindowLong" in the Platform SDK for more information.
	virtual LONG GetWindowLong( int nIndex );								// Set "GetWindowLong" in the Platform SDK for more information.
	virtual HDC BeginPaint( PAINTSTRUCT *lpPaintStruct );				// See "BeginPaint" in the Platform SDK for more information.
	virtual BOOL EndPaint( PAINTSTRUCT *lpPaintStruct );				// See "EndPaint" in the Platform SDK for more information.
	virtual LRESULT SendMessage( UINT Msg, WPARAM wParam, LPARAM lParam ); // See "SendMessage" in the Platform SDK for more information.
	virtual BOOL PostMessage( UINT Msg, WPARAM wParam, LPARAM lParam ); // See "PostMessage" in the Platform SDK for more information.
	virtual BOOL EnableWindow();												// Used to enable a control such as a button or edit box.
	virtual BOOL DisableWindow();												// Used to disable a control such as a button or edit box.
	virtual int MessageBox( LPCTSTR lpText, UINT uType );				// See "MessageBox" in the Platform SDK for more information.
	virtual BOOL MoveWindow( int x, int y, int nWidth, int nHeight, BOOL bRepaint = TRUE ); // See "MoveWindow" in the Platform SDK for more information.
	virtual BOOL SetMenu( HMENU hMenu );
	virtual HMENU GetMenu( void );
	virtual BOOL LoadMenu( int nResource );
	virtual BOOL DestroyMenu( void );
	virtual POINT Mouse( void );
	
	// These functions are mostly based on their Win32 API equivilents.  They allow you to
	// work with dialog boxes and common controls more easily.  If you need help, just
	// highlight the function and press F1.

	virtual LRESULT SendDlgItemMessage( int nID, UINT message, WPARAM wParam, LPARAM lParam );
	virtual HWND GetDlgItem( int nDialogItemID );
	virtual int GetDlgItemText( int nID, LPTSTR lpszString, int nMaxCount );
	virtual BOOL SetDlgItemText( int nID, LPCTSTR lpszString );
	virtual int GetDlgItemInt( int nID );
	virtual void SetDlgItemInt( int nID, int nNumber );
	virtual double GetDlgItemValue( int nID );
	virtual void SetDlgItemValue( int nID, double dValue );
	virtual int GetDlgItemPos( int nID );											// Gets the position of a trackbar or other type of slider or progress meter.
	virtual void SetDlgItemPos( int nID, int nPosition );						// Sets the position of a trackbar or other type of slider or progress meter.
	virtual BOOL GetDlgItemRange( int nID, LPINT lpMin, LPINT lpMax );	// This applies only to dialog controls that have range, like trackbars and spin controls.
	virtual void SetDlgItemRange( int nID, int nLower, int nUpper );		// This applies only to dialog controls that have range, like trackbars and spin controls.
	virtual int GetDlgItemCheck( int nID );
	virtual void CheckDlgItem( int nID );
	virtual void UncheckDlgItem( int nID );
	virtual BOOL GetDlgItemState( int nID );
	virtual void SetDlgItemState( int nID, BOOL bEnabled );
	virtual void EnableDlgItem( int nID );
	virtual void DisableDlgItem( int nID );
	virtual BOOL IsDlgItemClass( int nID, LPCTSTR lpszClassName );

protected:
	virtual LRESULT WindowProc( UINT message, WPARAM wParam, LPARAM lParam );		// Do not call this function directory.  It is the central
																											//   control station for incoming messages.  It is responsible
																											//   for dispatching the message handlers (listed below.)
																											//   Override this function only if you need to add more
																											//   message handlers.
	virtual LRESULT DefWindowProc( UINT message, WPARAM wParam, LPARAM lParam );	// Do not call this function directly, unless you wish to
																											//   bypass the default message handling routine of each base
																											//   window class and go directly to the message handler that
																											//   was registered when the window or control was created.
	virtual BOOL WhenCreate( LPCREATESTRUCT lpCS );											// This function is called in response to the WM_CREATE message.
																											//   Return TRUE to continue the creation of the window, or FALSE
																											//   to stop the window from being created.
	virtual void WhenDestroy();																	// This function is called in response to the WM_DESTROY message.
	virtual void WhenActivate( WORD fActive, BOOL bMinimized, HWND hOtherWindow );// This function is called in response to the WM_ACTIVATE message.
	virtual void WhenActivateApp( BOOL bActive, DWORD dwThreadID );					// This function is called in response to the WM_ACTIVATEAPP message.
	virtual void WhenCaptureChanged( HWND hWindow );										// This function is called in response to the WM_CAPTURECHANGED message.
	virtual void WhenChar( TCHAR tch, int nRepeat, int nScanCode, DWORD dwKeyState );// This function is called in response to the WM_CHAR message.
	virtual void WhenClose();																		// This function is called in response to the WM_CLOSE message.
	virtual BOOL WhenCommand( HWND hWindow, int nCommand, int nNotifyCode );		// This function is called in response to the WM_COMMAND message.
	virtual void WhenContextMenu( HWND hWindow, int x, int y );							// This function is called in response to the WM_CONTEXTMENU message.
	virtual void WhenEraseBkgnd( HDC hdcErase );												// This function is called in response to the WM_WHENERASEBKGND message.
	virtual void WhenHScroll( int nCode, int nPos, HWND hWindow );
	virtual void WhenVScroll( int nCode, int nPos, HWND hWindow );
	virtual BOOL WhenInitDialog( void );
	virtual void WhenLButtonDblClk( DWORD dwFlags, int x, int y );						// This function is called in response to the WM_LBUTTONDBLCLK message.
	virtual void WhenLButtonDown( DWORD dwFlags, int x, int y );						// This function is called in response to the WM_LBUTTONDOWN message.
	virtual void WhenLButtonUp( DWORD dwFlags, int x, int y );							// This function is called in response to the WM_LBUTTONUP message.
	virtual void WhenRButtonDblClk( DWORD dwFlags, int x, int y );						// This function is called in response to the WM_RBUTTONDBLCLK message.
	virtual void WhenRButtonDown( DWORD dwFlags, int x, int y );						// This function is called in response to the WM_RBUTTONDOWN message.
	virtual void WhenRButtonUp( DWORD dwFlags, int x, int y );							// This function is called in response to the WM_RBUTTONUP message.
	virtual void WhenMouseMove( DWORD dwFlags, int x, int y );							// This function is called in response to the WM_MOUSEMOVE message.
	virtual LRESULT WhenNotify( UINT nCode, int nSender, HWND hWindow, LPARAM lParam ); // This function is called in response to the WM_NOTIFY message.
	virtual void WhenPaint( HDC hdcPaint, PAINTSTRUCT *pstruct );						// This function is called in response to the WM_PAINT message.
	virtual void WhenSize( DWORD dwSizeType,  int nWidth, int nHeight );				// This function is called in response to the WM_SIZE message.
	virtual void WhenTimer( int nTimerID, TIMERPROC *lpfnTimerProc );					// This function is called in response to the WM_TIMER message.
	
protected:
	BOOL PreCreateWindow();				// Hooks into creation process of a window.  Call this function just
												//   before you use a Windows API function to create a window,
												//   such as ::CreateWindowEx() and ::CreateDialog().  Inappropriate
												//   use of this function can have disassertous consequences!
	BOOL PostCreateWindow();			// Removes the hook set by PreCreateWindow().  Call this function
												//   after the window creation function has been called.  If window
												//   creation fails, and this function is not called, the OS will crash
												//   very quickly!
												// NOTE: These two functions are called automatically when using member
												//   functions to create a window.  You only need to call these functions
												//   yourself when you are deriving your own window creation technique.
private:
	static LRESULT CALLBACK HookFunction( int nCode, WPARAM wParam, LPARAM lParam );	// This is the hook function that coordinates the proper attachment of
																												// a newly created window.  It is activated by PreCreateWindow()
																												// and deactivated when PostCreateWindow() is called.
protected:
	HWND m_hWindow;						// Window handle.  This is passed to most of the Windows API functions.
	WNDPROC m_pBaseWndProc;				// Window procedure to call for default message handling.

private:
	static CWindow *m_pWindows;		// First window in a singly-linked list of windows.
	static CWindow *m_pHookWindow;	// Window that is waiting to be attached in hook function.
	static HHOOK m_hHook;				// Handle to the hook function.
	CWindow *m_pNextWindow;				// Next window in linked list.
	TCHAR m_szWindowText[256];
	LRESULT m_lResult;
	HRGN	m_hUpdateRgn;
	POINT m_ptMouse;
};

#endif // __WINDOW_H__