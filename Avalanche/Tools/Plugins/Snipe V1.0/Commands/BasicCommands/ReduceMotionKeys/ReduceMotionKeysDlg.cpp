// ReduceMotionKeysDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ReduceMotionKeysDlg.h"
#include "HMainInterface.h"
#include ".\ReduceMotionKeysDlg.h"

// ReduceMotionKeysDlg dialog

IMPLEMENT_DYNAMIC(ReduceMotionKeysDlg, CDialog)

ReduceMotionKeysDlg::ReduceMotionKeysDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ReduceMotionKeysDlg::IDD, pParent)
{
   m_fTolerance = 0.001f;
}

ReduceMotionKeysDlg::~ReduceMotionKeysDlg()
{
}

void ReduceMotionKeysDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   DDX_Text(pDX, IDC_TOLERANCE_EDIT, m_fTolerance);
}
