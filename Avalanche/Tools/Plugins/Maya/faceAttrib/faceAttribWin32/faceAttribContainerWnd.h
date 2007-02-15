/****************************************************************************

	$Workfile: faceAttribContainerWnd.h $

	$Archive: /Avalanche/tools/Plugins/Maya/faceAttrib/faceAttribWin32/faceAttribContainerWnd.h $

	$Author: Adam Clayton $

	$Date: 12/16/02 9:51a $

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

#include "FaceAttributes.h"
//------------------------------------------------------------------------
class FaceAttribContainerWnd : public CWnd
{
// Operations
public:
	// sub-classed window member functions
	static FaceAttribContainerWnd*	CreateSubClassedWindow(HWND hWnd);
	static void						DestroySubClassedWindow(FaceAttribContainerWnd* wnd);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(FaceAttribContainerWnd)
	//}}AFX_VIRTUAL

protected:
	// don't allow people to create us directly
	FaceAttribContainerWnd();
	virtual ~FaceAttribContainerWnd();

	// called in leiu of a real create
	BOOL OnSubClassedCreate(void);


	// Generated message map functions
protected:	
	//{{AFX_MSG(FaceAttribContainerWnd)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP( )
};

//------------------------------------------------------------------------

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OBJATTRIBCONTAINERWND_H__INCLUDED_)
