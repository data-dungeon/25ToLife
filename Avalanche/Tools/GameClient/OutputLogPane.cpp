// OutputLogPane.cpp : implementation file
//

#include "stdafx.h"
#include "gameclient.h"
#include "OutputLogPane.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COutputLogPane dialog
IMPLEMENT_DYNAMIC(COutputLogPane, CGameClientPane);


COutputLogPane::COutputLogPane(CWnd* pParent /*=NULL*/)
	: CGameClientPane(COutputLogPane::IDD, pParent)
{
	//{{AFX_DATA_INIT(COutputLogPane)
	m_Text = _T("");
	//}}AFX_DATA_INIT
}


void COutputLogPane::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COutputLogPane)
	DDX_Control(pDX, IDC_LOG, m_Log);
	DDX_Text(pDX, IDC_LOG, m_Text);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COutputLogPane, CDialog)
	//{{AFX_MSG_MAP(COutputLogPane)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_CLEAROUTPUTLOG, OnClearoutputlog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COutputLogPane message handlers

void COutputLogPane::AppendText(CString text)
{
	m_Log.SetSel(-1, -1);
	m_Log.ReplaceSel(text);
	m_Log.SetSel(-1, -1);
}

void COutputLogPane::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	if(IsWindow(*this))
	{
		CRect rect;
		GetClientRect(&rect);

		CWnd* button = GetDlgItem(IDC_CLEAROUTPUTLOG);
		if(IsWindow(*button))
		{
			CRect butrect;
			button->GetClientRect(&butrect);

			CRect log = rect;
			log.DeflateRect(0, 0, 0, butrect.Height() + 2);
			
			if(IsWindow(m_Log))
				m_Log.MoveWindow(&log);

			butrect.left = rect.left;
			butrect.right = rect.right;
			butrect.top = log.bottom + 2;
			butrect.bottom = rect.bottom;
			button->MoveWindow(&butrect);
		}
	}
}

void COutputLogPane::OnClearoutputlog() 
{
	m_Log.SetWindowText("");
}

BOOL COutputLogPane::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CHARFORMAT cf;
	strcpy(cf.szFaceName, "Courier New");
	cf.dwMask = CFM_FACE;
	m_Log.SetDefaultCharFormat(cf);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
