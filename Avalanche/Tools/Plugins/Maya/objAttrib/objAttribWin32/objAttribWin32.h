/****************************************************************************

	$Workfile: objAttribWin32.h $

	$Archive: /Avalanche/tools/Plugins/Maya/objAttrib/objAttribWin32/objAttribWin32.h $

	$Author: Adam Clayton $

	$Date: 12/16/02 9:33a $

	CONFIDENTIAL AND PROPRIETARY INFORMATION

	Copyright © Avalanche Software 2002

	All rights reserved.

****************************************************************************/
// objAttribWin32.h : main header file for the OBJATTRIBWIN32 DLL

#pragma once
#define __OBJATTRIBWIN32_H__

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

// macro defn's for DLL import and export
#define DllImport  __declspec( dllimport )
#define DllExport  __declspec( dllexport )


//------------------------------------------------------------------------
// ObjAttribWin32App
// See objAttribWin32.cpp for the implementation of this class
//
class ObjAttribWin32App : public CWinApp
{
public:
	ObjAttribWin32App();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ObjAttribWin32App)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL
	
	//{{AFX_MSG(ObjAttribWin32App)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
//------------------------------------------------------------------------

extern ObjAttribWin32App theApp;

// global objects
class ObjAttribContainerWnd;
extern ObjAttribContainerWnd* g_objAttribContainerWnd;

extern HINSTANCE hInstance; // global HINSTANCE

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

