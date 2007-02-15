// ExportMSetOptionsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ExportMSetOptionsDlg.h"

// ExportMSetOptionsDlg dialog

IMPLEMENT_DYNAMIC(ExportMSetOptionsDlg, CDialog)

ExportMSetOptionsDlg::ExportMSetOptionsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ExportMSetOptionsDlg::IDD, pParent)
{
   m_bBinary = ::AfxGetApp()->GetProfileInt("Tup Plugin", "ExportBinary", FALSE);
}

ExportMSetOptionsDlg::~ExportMSetOptionsDlg()
{
}

void ExportMSetOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
  DDX_Check(pDX, IDC_EXPORTBINARY_CHECKBOX, m_bBinary);
  ::AfxGetApp()->WriteProfileInt("Tup Plugin", "ExportBinary" ,m_bBinary);
}

BEGIN_MESSAGE_MAP(ExportMSetOptionsDlg, CDialog)
   // Empty
END_MESSAGE_MAP()

// ExportMSetOptionsDlg message handlers

BOOL ExportMSetOptionsDlg::OnInitDialog()
{
   return CDialog::OnInitDialog();
}
