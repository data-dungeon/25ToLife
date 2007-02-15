// Dan  12/28/00  \Dan90\Plugin\HXT\KFM\ExportActionsDialog.cpp
//

#include "stdafx.h"
#include "resource.h"
#include "ExportActionsDialog.h"
#include "F_Protos.h"

#include "SDK\HAction.h"
#include "SDK\HProject.h"
#include "SDK\Misc.h"

CExportActionsDialog::CExportActionsDialog(HActionCacheContainer *hacc)
	: CDialog(CExportActionsDialog::IDD, CWnd::FromHandle(GetMainApplicationWnd()))
{
   m_hacc = hacc;
   m_dir = GetRegistryString("KFM Plugin", "Output Directory", ".");
   m_errortolerance = GetRegistryValue("KFM Plugin", "ReductionTolerance", 1.0f);
}

CExportActionsDialog::~CExportActionsDialog()
{
   m_checklistbox.Detach();
}

void CExportActionsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(   pDX, IDC_ACTIONLIST_COMBO, m_checklistbox);
   DDX_Text(pDX, IDC_ERRORTOLERANCE, m_errortolerance );
   DDV_MinMaxFloat(pDX, m_errortolerance, 0.0f, 1000.0f );
}


BEGIN_MESSAGE_MAP(CExportActionsDialog, CDialog)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
END_MESSAGE_MAP()

BOOL CExportActionsDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
   int iIndex;

   m_selected.RemoveAll();
   for (HActionCache *hac = m_hacc->GetChildActionCache(); hac; hac=(HActionCache *)hac->GetSiblingSame()) {
      String name = hac->GetName();
      iIndex = m_checklistbox.AddString(name.GetNameOnlyWithoutSuffix().Get());
      m_checklistbox.SetItemData(iIndex, (DWORD)hac);
      m_checklistbox.SetCheck(iIndex, TRUE);
   }
   GetDlgItem(IDC_OUTPUTDIR)->SetWindowText(m_dir.Get());

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CExportActionsDialog::OnOK() 
{
	CDialog::OnOK();
   for (int iIndex=0; iIndex < m_checklistbox.GetCount(); iIndex++) {
      if (m_checklistbox.GetCheck(iIndex)) {
          HActionCache *hac = (HActionCache *)m_checklistbox.GetItemData(iIndex);
          m_selected.AddTail(hac);
      }
   }
   SetRegistryString("KFM Plugin", "Output Directory", m_dir);
   SetRegistryValue("KFM Plugin", "ReductionTolerance", m_errortolerance );
}

void CExportActionsDialog::OnBrowse() 
{
   CString caption;
   caption.LoadString(IDS_CHOOSEDIRECTORY);
   if (!DirRequest(m_dir, (LPCTSTR)caption/*NULL*/, m_dir.Get(), this))
      return;
   GetDlgItem(IDC_OUTPUTDIR)->SetWindowText(m_dir.Get());
}
