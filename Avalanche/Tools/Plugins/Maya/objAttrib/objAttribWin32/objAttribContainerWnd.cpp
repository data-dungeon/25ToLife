/****************************************************************************

	$Workfile: objAttribContainerWnd.cpp $

	$Archive: /Avalanche/tools/Plugins/Maya/objAttrib/objAttribWin32/objAttribContainerWnd.cpp $

	$Author: Adam Clayton $

	$Date: 12/16/02 9:33a $

	CONFIDENTIAL AND PROPRIETARY INFORMATION

	Copyright © Avalanche Software 2002

	All rights reserved.

****************************************************************************/
// objAttribContainerWnd.cpp : implementation file

// precomp'ed header files
#include "StdAfx.h"

// volitile local header files
#include "objAttribWin32.h"
#include "objAttribContainerWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace AttrControls;

//------------------------------------------------------------------------
ObjAttribContainerWnd::ObjAttribContainerWnd()
{
	
}

//------------------------------------------------------------------------
ObjAttribContainerWnd::~ObjAttribContainerWnd()
{

}

BEGIN_MESSAGE_MAP(ObjAttribContainerWnd, CWnd)
	//{{AFX_MSG_MAP(ObjAttribContainerWnd)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//------------------------------------------------------------------------
ObjAttribContainerWnd* ObjAttribContainerWnd::CreateSubClassedWindow(HWND hWnd)
	// subclass the window
	// Notes: When a window is dynamically subclassed, windows messages 
	// will route through the CWnd's message map and call message handlers
	// in the CWnd's class first. Messages that are passed to the base 
	// class will be passed to the default message handler in the window.
	// This member function attaches the Windows control to a CWnd object
	// and replaces the window's WndProc and AfxWndProc functions. The
	// function stores a pointer to the old WndProc in the CWnd object.
	// The window must not already be attached to an MFC object when this
	// function is called.
{
	ObjAttribContainerWnd* pObjAttribContainerWnd = new ObjAttribContainerWnd;
	
	pObjAttribContainerWnd->SubclassWindow(hWnd);
	theObjectAttributes.hDlg = hWnd;

	// attempt a sub-classed create
	if(!pObjAttribContainerWnd->OnSubClassedCreate())
	{
		// FAILURE: Clean up and return NULL
		// release
		pObjAttribContainerWnd->UnsubclassWindow();
		
		// destroy
		delete pObjAttribContainerWnd;
		
		// report error
		return NULL;
	}

	return pObjAttribContainerWnd;
}

//------------------------------------------------------------------------
void ObjAttribContainerWnd::DestroySubClassedWindow(ObjAttribContainerWnd* wnd)
{
	// release
	wnd->UnsubclassWindow();

	// destroy
	delete wnd;
}

//------------------------------------------------------------------------
// --                         Message Handlers                          --
//------------------------------------------------------------------------
BOOL ObjAttribContainerWnd::OnSubClassedCreate(void)
{
//	theObjectAttributes.SetupAttributes();

	return TRUE;
}

void ObjAttribContainerWnd::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize( nType, cx, cy );
}
