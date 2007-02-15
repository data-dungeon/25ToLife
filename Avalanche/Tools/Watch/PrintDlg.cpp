// PrintDlg.cpp : implementation file
//

#include "stdafx.h"
#include "watch.h"
#include "PrintDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPrintDlg dialog


CPrintDlg::CPrintDlg(CWnd* pParent /*=NULL*/)
	: CModelessDlg(pParent, CPrintDlg::IDD)
{
	//{{AFX_DATA_INIT(CPrintDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	Create(CPrintDlg::IDD, pParent);
//	ShowWindow(SW_SHOW);
}


void CPrintDlg::DoDataExchange(CDataExchange* pDX)
{
	CModelessDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPrintDlg)
	DDX_Control(pDX, IDC_DEBUG_OUT, m_ctlDebugOut);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPrintDlg, CModelessDlg)
	//{{AFX_MSG_MAP(CPrintDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPrintDlg message handlers

//----------------------------------------------------------------------
// CWatchView::DebugPrint
//    Print debug messages to the debug print edit box.
//----------------------------------------------------------------------
void CPrintDlg::DebugPrint(char *format, ...)
{
	char tmp[1024];
	va_list args;
	va_start(args, format);

	vsprintf(tmp, format, args);
	va_end(args);

	int start, end, length;
	length = m_ctlDebugOut.GetWindowTextLength();
	m_ctlDebugOut.GetSel(start, end);
	bool resetcursor = true;
	if (start != length)
		resetcursor = false;
	m_ctlDebugOut.SetSel(length, length, true);
	CString text;
	text.Format("%s", tmp);
	m_ctlDebugOut.ReplaceSel(text);
	length = m_ctlDebugOut.GetWindowTextLength();
	if (resetcursor)
		m_ctlDebugOut.SetSel(length, length, false);
	else
		m_ctlDebugOut.SetSel(start, end, false);
	m_ctlDebugOut.Invalidate();
}

