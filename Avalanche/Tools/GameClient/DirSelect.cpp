// DirSelect.cpp : implementation file
//

#include "stdafx.h"
#include "gameclient.h"
#include "DirSelect.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDirSelect dialog


CDirSelect::CDirSelect(CWnd* pParent /*=NULL*/)
	: CDialog(CDirSelect::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDirSelect)
	//}}AFX_DATA_INIT
}


void CDirSelect::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDirSelect)
	DDX_Control(pDX, IDC_PATHTREE, m_PathTree);
	DDX_Control(pDX, IDC_DRIVE, m_Drive);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDirSelect, CDialog)
	//{{AFX_MSG_MAP(CDirSelect)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDirSelect message handlers

BOOL CDirSelect::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_Drive.Dir(0x4000, "*.*");
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
