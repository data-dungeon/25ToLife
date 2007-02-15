// TupperChainGuiApp.h : main header file for the TUPPERCHAINGUIAPP application
//

#if !defined(AFX_TUPPERCHAINGUIAPP_H__7ADC9DDD_7E12_4256_8F1E_CF46B7FD58D9__INCLUDED_)
#define AFX_TUPPERCHAINGUIAPP_H__7ADC9DDD_7E12_4256_8F1E_CF46B7FD58D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CTupperChainGuiAppApp:
// See TupperChainGuiApp.cpp for the implementation of this class
//

class CTupperChainGuiAppApp : public CWinApp
{
public:
	CTupperChainGuiAppApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTupperChainGuiAppApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CTupperChainGuiAppApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TUPPERCHAINGUIAPP_H__7ADC9DDD_7E12_4256_8F1E_CF46B7FD58D9__INCLUDED_)
