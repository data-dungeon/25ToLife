/****************************************************************************

	$Workfile: mapAttribWin32.h $

	$Archive: /Avalanche/tools/Plugins/Maya/mapAttrib/mapAttribWin32/mapAttribWin32.h $

	$Author: Adam Clayton $

	$Date: 6/20/03 3:37p $

	CONFIDENTIAL AND PROPRIETARY INFORMATION

	Copyright © Avalanche Software 2002

	All rights reserved.

****************************************************************************/
// objAttribWin32.h : main header file for the OBJATTRIBWIN32 DLL

#if !defined(AFX_OBJATTRIBWIN32_H__280874C4_FF41_4B93_A4E6_6BCCC31E72B1__INCLUDED_)
#define AFX_OBJATTRIBWIN32_H__280874C4_FF41_4B93_A4E6_6BCCC31E72B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

// main symbols
#include "resource.h"		

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

#endif // !defined(AFX_OBJATTRIBWIN32_H__280874C4_FF41_4B93_A4E6_6BCCC31E72B1__INCLUDED_)
