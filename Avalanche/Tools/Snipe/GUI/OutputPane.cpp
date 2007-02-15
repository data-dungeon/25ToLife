// OutputPane.cpp : implementation file
//

#include "stdafx.h"
#include "Snipe.h"
#include "OutputPane.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COutputPane

COutputPane::COutputPane()
{
}

COutputPane::~COutputPane()
{
}


BEGIN_MESSAGE_MAP(COutputPane, CWnd)
	//{{AFX_MSG_MAP(COutputPane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// COutputPane message handlers

int COutputPane::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (m_wndOutput.GetSafeHwnd() == 0)
	{
		m_wndOutput.Create(WS_HSCROLL|ES_AUTOHSCROLL|ES_MULTILINE|WS_VISIBLE, CRect(0, 0, 0, 0), this, 0 );
		m_wndOutput.ModifyStyleEx(0, WS_EX_STATICEDGE);

		m_wndOutput.SetFont(XTPPaintManager()->GetRegularFont());
		m_wndOutput.SetMargins(10, 5);		
	}	
	
	return 0;
}

void COutputPane::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	
	if (m_wndOutput.GetSafeHwnd())
	{
		m_wndOutput.MoveWindow(0, 0, cx, cy);		
	}	
	
}
