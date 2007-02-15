//=============================================================================//
//  Copyright ©2000 Avalanche Software.  All Rights Reserved.                  //
//=============================================================================//
//      File: dialog.h                                                         //
//    Author: Daniel Silver                                                    //
//   Created: 04/10/2000                                                       //
//=============================================================================//

#ifndef __DIALOG_H__
#define __DIALOG_H__

#include "window.h"

class CDialog : public CWindow
{
public:
	CDialog( void );
	
public:
	virtual BOOL CreateDialogBox( HINSTANCE hInstance, LPCTSTR lpTemplate, HWND hParentWnd );
	virtual int ModalDialog( HINSTANCE hInstance, LPCTSTR lpTemplate, HWND hParentWnd );
	virtual BOOL EndDialog( int nResult );

protected:
	virtual void WhenDialogOK();
	virtual void WhenDialogCancel();
	virtual BOOL WhenCommand( HWND hWindow, int nCommand, int nNotifyCode );
};

#endif // __DIALOG_H__