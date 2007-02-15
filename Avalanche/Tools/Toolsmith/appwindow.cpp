//=============================================================================//
//  Copyright ©2000 Avalanche Software.  All Rights Reserved.                  //
//=============================================================================//
//      File: appwindow.cpp                                                    //
//    Author: Daniel Silver                                                    //
//   Created: 05/03/2000                                                       //
//=============================================================================//

#include "pch.h"
#include "appwindow.h"

CAppWindow *CAppWindow::m_pAppWindows = 0;

//=============================================================================
// CAppWindow::CAppWindow()
//=============================================================================

CAppWindow::CAppWindow( void )
{
	m_pNextAppWindow = 0;
}

//=============================================================================
// CAppWindow::RunProgram()
//=============================================================================

int CAppWindow::RunProgram()
{
	MSG message = {0, WM_NULL};
	
	while( message.message != WM_QUIT )
	{
		if( PeekMessage( &message, NULL, 0, 0, PM_REMOVE ) )
		{
			TranslateMessage( &message );
			DispatchMessage( &message );
		}
		else
		{
			WaitMessage();
		}
	}
	
	return message.wParam;
}

//=============================================================================
// CAppWindow::WhenClose()
//=============================================================================

void CAppWindow::WhenClose( void )
{
	DestroyWindow();
}

//=============================================================================
// CAppWindow::WhenCommand()
//=============================================================================

BOOL CAppWindow::WhenCommand( HWND hWindow, int nCommand, int nNotifyCode )
{
	switch( nCommand )
	{
	case ID_APP_EXIT:
		SendMessage( WM_CLOSE, 0, 0 );
		break;
	case ID_FILE_NEW:
		WhenFileNew();
		break;
	case ID_FILE_OPEN:
		WhenFileOpen();
		break;
	case ID_FILE_SAVE:
		WhenFileSave();
		break;
	case ID_FILE_SAVE_AS:
		WhenFileSaveAs();
		break;
	default:
		return CWindow::WhenCommand( hWindow, nCommand, nNotifyCode );
	}

	return 0;
}

//=============================================================================
// CAppWindow::WhenCreate()
//=============================================================================

BOOL CAppWindow::WhenCreate( LPCREATESTRUCT lpCS )
{
	if( !m_pAppWindows )
		m_pAppWindows = this;
	else
	{
		CAppWindow *pAppWindow = m_pAppWindows;

		while( pAppWindow->m_pNextAppWindow )
			pAppWindow = pAppWindow->m_pNextAppWindow;

		pAppWindow->m_pNextAppWindow = this;
	}

	if( !CWindow::WhenCreate( lpCS ) )
		return FALSE;

	MSG message;

	PeekMessage( &message, NULL, WM_QUIT, WM_QUIT, PM_REMOVE );

	return TRUE;
}

//=============================================================================
// CAppWindow::WhenDestroy()
//=============================================================================

void CAppWindow::WhenDestroy()
{
	if( m_pAppWindows == this )
		m_pAppWindows = m_pNextAppWindow;
	else
	{
		CAppWindow *pAppWindow = m_pAppWindows;

		while( pAppWindow )
		{
			if( pAppWindow->m_pNextAppWindow == this )
				pAppWindow->m_pNextAppWindow = m_pNextAppWindow;

			pAppWindow = pAppWindow->m_pNextAppWindow;
		}
	}

	if( !m_pAppWindows )
		PostQuitMessage(0);

	CWindow::WhenDestroy();
}

//=============================================================================
// CAppWindow::WhenEraseBkgnd()
//=============================================================================

void CAppWindow::WhenEraseBkgnd( HDC hdcErase )
{
	RECT clientRect;
	GetClientRect( &clientRect );

	FillRect( hdcErase, &clientRect, (HBRUSH)(COLOR_APPWORKSPACE + 1) );
}

//=============================================================================
// CAppWindow::WhenFileNew()
//=============================================================================

void CAppWindow::WhenFileNew( void )
{
}

void CAppWindow::WhenFileOpen( void )
{
}

void CAppWindow::WhenFileSave( void )
{
}

void CAppWindow::WhenFileSaveAs( void )
{
}

void CAppWindow::WhenAppExit( void )
{
	SendMessage( WM_CLOSE, 0, 0 );
}