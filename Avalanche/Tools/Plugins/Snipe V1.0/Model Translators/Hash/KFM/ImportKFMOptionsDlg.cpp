// ImportKFMOptionsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ImportKFMOptionsDlg.h"

// ImportTupSceneOptionsDlg dialog

IMPLEMENT_DYNAMIC(ImportKFMOptionsDlg, CDialog)

ImportKFMOptionsDlg::ImportKFMOptionsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ImportKFMOptionsDlg::IDD, pParent)
{
   m_bReduceKeys = ::AfxGetApp()->GetProfileInt("KFM Plugin", "ReduceKeys", FALSE);
   m_fTolerance = (float)atof(::AfxGetApp()->GetProfileString("KFM Plugin", "ReductionTolerance", FALSE));
}

ImportKFMOptionsDlg::~ImportKFMOptionsDlg()
{
}

void ImportKFMOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
   DDX_Check(pDX, IDC_CHECK_KEYREDUCTION, m_bReduceKeys);
   ::AfxGetApp()->WriteProfileInt("KFM Plugin", "ReduceKeys" ,m_bReduceKeys);
   DDX_Text(pDX, IDC_TEXT_REDUCTIONTOLERANCE, m_fTolerance);
   ::AfxGetApp()->WriteProfileString("KFM Plugin", "ReductionTolerance" ,String(m_fTolerance).Get());
}

BEGIN_MESSAGE_MAP(ImportKFMOptionsDlg, CDialog)
   // Empty
END_MESSAGE_MAP()

// ImportKFMOptionsDlg message handlers

BOOL ImportKFMOptionsDlg::OnInitDialog()
{
   return CDialog::OnInitDialog();
}