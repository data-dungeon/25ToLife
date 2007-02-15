// ExportTupSceneOptionsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ExportTupSceneOptionsDlg.h"

// ExportTupSceneOptionsDlg dialog

IMPLEMENT_DYNAMIC(ExportTupSceneOptionsDlg, CDialog)

ExportTupSceneOptionsDlg::ExportTupSceneOptionsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ExportTupSceneOptionsDlg::IDD, pParent)
{
   m_bBinary = ::AfxGetApp()->GetProfileInt("Tup Plugin", "ExportBinary", FALSE);
}

ExportTupSceneOptionsDlg::~ExportTupSceneOptionsDlg()
{
}

void ExportTupSceneOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
  DDX_Check(pDX, IDC_EXPORTBINARY_CHECKBOX, m_bBinary);
  ::AfxGetApp()->WriteProfileInt("Tup Plugin", "ExportBinary" ,m_bBinary);
}

BEGIN_MESSAGE_MAP(ExportTupSceneOptionsDlg, CDialog)
   // Empty
END_MESSAGE_MAP()

// ExportTupSceneOptionsDlg message handlers

BOOL ExportTupSceneOptionsDlg::OnInitDialog()
{
   return CDialog::OnInitDialog();
}
