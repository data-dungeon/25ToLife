// SyncConfirmDialog.cpp : implementation file
//

#include "stdafx.h"
#include "assetmanager.h"
#include "SyncConfirmDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSyncConfirmDialog dialog


CSyncConfirmDialog::CSyncConfirmDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CSyncConfirmDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSyncConfirmDialog)
	m_confirmText1 = _T("");
	m_confirmText2 = _T("");
	m_confirmText3 = _T("");
	m_confirmText4 = _T("");
	//}}AFX_DATA_INIT
}


void CSyncConfirmDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSyncConfirmDialog)
	DDX_Text(pDX, IDC_CONFIRM1, m_confirmText1);
	DDX_Text(pDX, IDC_CONFIRM2, m_confirmText2);
	DDX_Text(pDX, IDC_CONFIRM3, m_confirmText3);
	DDX_Text(pDX, IDC_CONFIRM4, m_confirmText4);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSyncConfirmDialog, CDialog)
	//{{AFX_MSG_MAP(CSyncConfirmDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSyncConfirmDialog message handlers

BOOL CSyncConfirmDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
