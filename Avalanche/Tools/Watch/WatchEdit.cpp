// WatchEdit.cpp : implementation file
//

#include "stdafx.h"
#include "watch.h"
#include "WatchEdit.h"
#include "WatchSlider.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWatchEdit

CWatchEdit::CWatchEdit()
{
	fSlider = 0;
	textOnly = false;
}

CWatchEdit::~CWatchEdit()
{
}


BEGIN_MESSAGE_MAP(CWatchEdit, CEdit)
	//{{AFX_MSG_MAP(CWatchEdit)
	ON_CONTROL_REFLECT(EN_CHANGE, OnChange)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWatchEdit message handlers

void CWatchEdit::OnChange() 
{
	CString str;
	GetWindowText(str);
	if (fSlider)
		fSlider->EditOnChange(str);
}

HBRUSH CWatchEdit::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	if (textOnly)
		return CEdit::OnCtlColor(pDC, pWnd, nCtlColor);
	else
		return ::GetSysColorBrush(COLOR_3DFACE);
}
