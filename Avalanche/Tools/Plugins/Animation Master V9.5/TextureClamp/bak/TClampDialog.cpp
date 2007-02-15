#include "stdafx.h"
#include "F_Protos.h"
#include "TClampDialog.h"
#include "Hxt.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CTClampDialog::CTClampDialog(TClampParameters *parameters)
: CDialog(CTClampDialog::IDD, CWnd::FromHandle(GetMainApplicationWnd()))
{
	//{{AFX_DATA_INIT(CDupDialog)
//	m_parameters = parameters;
	//}}AFX_DATA_INIT
}


void CTClampDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMEDialog)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTClampDialog, CDialog)
	//{{AFX_MSG_MAP(CDupDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDupDialog message handlers
BOOL CTClampDialog::OnInitDialog()
{
   CDialog::OnInitDialog();

   return FALSE;
}

void CTClampDialog::OnOK() 
{
	CDialog::OnOK();
}
