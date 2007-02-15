//=============================================================================//
//  Copyright ©2001 Avalanche Software.  All Rights Reserved.                  //
//=============================================================================//
//      File: appwindow.h                                                      //
//    Author: Daniel Silver                                                    //
//   Created: 11/19/2001                                                       //
//=============================================================================//

#ifndef __APPWINDOW_H__
#define __APPWINDOW_H__

#include "window.h"

//=============================================================================
// class CAppWindow
//
//		This is typically the main window for your application.  When you destroy
//	all windows derived from this class, the application will end by receiving a
// quit message.
//
//=============================================================================

class CAppWindow : public CWindow
{
	DECLARE_CLASS( CAppWindow );

public:
	static int RunProgram();

public:
	CAppWindow( void );

protected:
	virtual void WhenClose( void );
	virtual BOOL WhenCommand( HWND hWindow, int nCommand, int nNotifyCode );
	virtual BOOL WhenCreate( LPCREATESTRUCT lpCS );
	virtual void WhenDestroy();
	virtual void WhenEraseBkgnd( HDC hdcErase );
	virtual void WhenFileNew( void );
	virtual void WhenFileOpen( void );
	virtual void WhenFileSave( void );
	virtual void WhenFileSaveAs( void );
	virtual void WhenAppExit( void );

private:
	static CAppWindow *m_pAppWindows;
	CAppWindow  *m_pNextAppWindow;
};

#endif // __APPWINDOW_H__