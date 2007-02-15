//-----------------------------------------------------------------------------
//	File:		OptionsDlg.cpp
//	Created:	Sep 30, 2002
//				Copyright (c) 2002, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Brad Worthen
//
//	Desc:	The COptionsDlg class defines behavior for the options dialog.
//
//	History:	9/30/2002: Initial creation - Brad Worthen
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// Includes:

#include "stdafx.h"
#include "AssetManager.h"
#include "PushButton.h"

//-----------------------------------------------------------------------------
// Defines, etc.:

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CPushButton

CPushButton::CPushButton()
{
}

CPushButton::~CPushButton()
{
}


BEGIN_MESSAGE_MAP(CPushButton, CButton)
	//{{AFX_MSG_MAP(CPushButton)
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPushButton message handlers

void CPushButton::OnKillFocus(CWnd* pNewWnd) 
{
//	CButton::OnKillFocus(pNewWnd);
	
	// TODO: Add your message handler code here
	
}

void CPushButton::OnSetFocus(CWnd* pOldWnd) 
{
//	CButton::OnSetFocus(pOldWnd);
	
	// TODO: Add your message handler code here
	
}
