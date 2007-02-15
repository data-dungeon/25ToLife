//=============================================================================//
//  Copyright ©2000 Avalanche Software.  All Rights Reserved.                  //
//=============================================================================//
//      File: dialog.cpp                                                       //
//    Author: Daniel Silver                                                    //
//   Created: 04/10/2000                                                       //
//=============================================================================//

#include "pch.h"
#include "dialog.h"

BOOL CALLBACK DialogProc( HWND hWindow, UINT message, WPARAM wParam, LPARAM lParam )
{
	return 0;
}

//=============================================================================
// CDialog::CDialog()
//=============================================================================

CDialog::CDialog( void )
{
}

//=============================================================================
// CDialog::CreateDialog()
//=============================================================================

BOOL CDialog::CreateDialogBox( HINSTANCE hInstance, LPCTSTR lpTemplate, HWND hParentWnd )
{
	// Make sure the common controls are initialized before trying to create the dialog.

	InitCommonControls();

	if( !hInstance )
		hInstance = GetModuleHandle(NULL);

	if( !PreCreateWindow() )
		return FALSE;

	HWND hDialog = ::CreateDialog( hInstance, lpTemplate, hParentWnd, DialogProc );

	if( !PostCreateWindow() )
		return FALSE;

	if( hDialog && hDialog == m_hWindow )
		return TRUE;
	
	::DestroyWindow( hDialog );
	return FALSE;
}

//=============================================================================
// CDialog::ModalDialog()

//=============================================================================

int CDialog::ModalDialog( HINSTANCE hInstance, LPCTSTR lpTemplate, HWND hParentWnd )
{
	// Make sure the common controls are initialized before the dialog is created.

	InitCommonControls();

	if( !hInstance )
		hInstance = GetModuleHandle(NULL);

	if( !PreCreateWindow() )
		return FALSE;

	int nResult = DialogBox( hInstance, lpTemplate, hParentWnd, DialogProc );

	PostCreateWindow();

	return nResult;
}

//=============================================================================
// CWindow::EndDialog()
//=============================================================================

BOOL CDialog::EndDialog( int nResult )
{
	if( !ParentHandle() )
		SendMessage( WM_CLOSE, 0, 0 );

	return ::EndDialog( m_hWindow, nResult );
}

//=============================================================================
// CDialog::WhenDialogOK()
//=============================================================================

void CDialog::WhenDialogOK()
{
	EndDialog( IDOK );
}

//=============================================================================
// CDialog::WhenDialogCancel()
//=============================================================================

void CDialog::WhenDialogCancel()
{
	EndDialog( IDCANCEL );
}

//=============================================================================
// CDialog::WhenCommand()
//=============================================================================

BOOL CDialog::WhenCommand( HWND hWindow, int nCommand, int nNotifyCode )
{
	switch( nCommand )
	{
	case IDOK:
		WhenDialogOK();
		break;
	case IDCANCEL:
		WhenDialogCancel();
		break;
	default:
		return CWindow::WhenCommand( hWindow, nCommand, nNotifyCode );
	}

	return TRUE;
}