// SyncUtils.h : main header file for the SYNCUTILS DLL
//

#if !defined(AFX_SYNCUTILS_H__38A08597_D056_411C_9B4D_432CD9657A66__INCLUDED_)
#define AFX_SYNCUTILS_H__38A08597_D056_411C_9B4D_432CD9657A66__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CSyncUtilsApp
// See SyncUtils.cpp for the implementation of this class
//

class CSyncUtilsApp : public CWinApp
{
public:
	CSyncUtilsApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSyncUtilsApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CSyncUtilsApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SYNCUTILS_H__38A08597_D056_411C_9B4D_432CD9657A66__INCLUDED_)
