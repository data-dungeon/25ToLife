// SaveSelectionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ExportDBLOptionsDlg.h"
#include "FileDialog.h"

// ExportDBLOptionsDlg dialog

IMPLEMENT_DYNAMIC(ExportDBLOptionsDlg, CDialog)

ExportDBLOptionsDlg::ExportDBLOptionsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ExportDBLOptionsDlg::IDD, pParent)
{
   m_strFilterChain = ::AfxGetApp()->GetProfileString("DBL Plugin", "FilterChain", "C:\\AvalancheToolSet\\FilterChain\\FilterChain.DLL");
   m_strProcessFilename = ::AfxGetApp()->GetProfileString("DBL Plugin", "ProcessFilename", "C:\\AvalancheToolSet\\FilterChain\\Tak3\\Process_Model_DX.twc");
}

ExportDBLOptionsDlg::~ExportDBLOptionsDlg()
{
}

void ExportDBLOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
  DDX_Text(pDX, IDC_FILTERCHAINPATHNAME, m_strFilterChain);
  DDX_Text(pDX, IDC_PROCESSFILENAME, m_strProcessFilename);
  ::AfxGetApp()->WriteProfileString("DBL Plugin", "FilterChain" ,m_strFilterChain);
  ::AfxGetApp()->WriteProfileString("DBL Plugin", "ProcessFilename" ,m_strProcessFilename);
}

BEGIN_MESSAGE_MAP(ExportDBLOptionsDlg, CDialog)
   ON_BN_CLICKED(IDC_BROWSE_BUTTON, OnBrowse)
   ON_BN_CLICKED(IDC_BROWSE2_BUTTON, OnBrowse2)
END_MESSAGE_MAP()

// ExportDBLOptionsDlg message handlers

BOOL ExportDBLOptionsDlg::OnInitDialog()
{
   return CDialog::OnInitDialog();
}

void ExportDBLOptionsDlg::OnBrowse() 
{
   SnipeFileDialog dlg( TRUE, (String)m_strFilterChain, "FilterChain (*.dll)|*.dll|FilterChain||", "FilterChain" );
   if (dlg.DoModal() != IDOK)
      return;

   CWnd *pDlgItem = GetDlgItem(IDC_FILTERCHAINPATHNAME);
   
   m_strFilterChain = dlg.GetPathName();

   pDlgItem->SetWindowText(m_strFilterChain);

   ::AfxGetApp()->WriteProfileString("DBL Plugin", "FilterChainPath" ,m_strFilterChain);  
}

void ExportDBLOptionsDlg::OnBrowse2() 
{
   SnipeFileDialog dlg( TRUE, (String)m_strProcessFilename, "ProcessFile (*.twc)|*.twc|ProcessFile||", "ProcessFile" );
   if (dlg.DoModal() != IDOK)
      return;

   CWnd *pDlgItem = GetDlgItem(IDC_PROCESSFILENAME);

   m_strProcessFilename = dlg.GetPathName();

   pDlgItem->SetWindowText(m_strProcessFilename);

   ::AfxGetApp()->WriteProfileString("DBL Plugin", "ProcessFilename" ,m_strProcessFilename);  
}
