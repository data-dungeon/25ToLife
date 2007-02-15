// ExportTupMotionOptionsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ExportTupMotionOptionsDlg.h"

// ExportTupMotionOptionsDlg dialog

IMPLEMENT_DYNAMIC(ExportTupMotionOptionsDlg, CDialog)

ExportTupMotionOptionsDlg::ExportTupMotionOptionsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ExportTupMotionOptionsDlg::IDD, pParent)
{
   m_bBinary = ::AfxGetApp()->GetProfileInt("Tup Plugin", "ExportBinary", FALSE);
}

ExportTupMotionOptionsDlg::~ExportTupMotionOptionsDlg()
{
}

void ExportTupMotionOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
  DDX_Check(pDX, IDC_EXPORTBINARY_CHECKBOX, m_bBinary);
  ::AfxGetApp()->WriteProfileInt("Tup Plugin", "ExportBinary" ,m_bBinary);
}

BEGIN_MESSAGE_MAP(ExportTupMotionOptionsDlg, CDialog)
   // Empty
END_MESSAGE_MAP()

// ExportTupMotionOptionsDlg message handlers

BOOL ExportTupMotionOptionsDlg::OnInitDialog()
{
   return CDialog::OnInitDialog();
}
