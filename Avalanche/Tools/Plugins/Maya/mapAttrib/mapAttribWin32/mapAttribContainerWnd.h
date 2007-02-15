/****************************************************************************

	$Workfile: mapAttribContainerWnd.h $

	$Archive: /Avalanche/tools/Plugins/Maya/mapAttrib/mapAttribWin32/mapAttribContainerWnd.h $

	$Author: Adam Clayton $

	$Date: 6/20/03 4:27p $

	CONFIDENTIAL AND PROPRIETARY INFORMATION

	Copyright © Avalanche Software 2002

	All rights reserved.

****************************************************************************/
// objAttribContainerWnd.h : header file

#if !defined(AFX_OBJATTRIBCONTAINERWND_H__INCLUDED_)
#define AFX_OBJATTRIBCONTAINERWND_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// disable warning about long symbol names
#pragma warning(disable:4786)

// #include "ObjectAttributes.h"
#include "MapAttributes.h"
//------------------------------------------------------------------------
class ObjAttribContainerWnd : public CWnd
{
// Operations
public:
	// sub-classed window member functions
	static ObjAttribContainerWnd*	CreateSubClassedWindow(HWND hWnd);
	static void						DestroySubClassedWindow(ObjAttribContainerWnd* wnd);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ObjAttribContainerWnd)
	//}}AFX_VIRTUAL

protected:
	// don't allow people to create us directly
	ObjAttribContainerWnd();
	virtual ~ObjAttribContainerWnd();

	// called in leiu of a real create
	BOOL OnSubClassedCreate(void);


	// Generated message map functions
protected:	
	//{{AFX_MSG(ObjAttribContainerWnd)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP( )
};

//------------------------------------------------------------------------

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OBJATTRIBCONTAINERWND_H__INCLUDED_)
