// ExportModelsDialog.cpp : implementation file
//

#include "stdafx.h"
#include "ie.h"
#include "ExportModelsDialog.h"
#include "HashHand.h"
#include "F_Protos.h"
#include "MapLimit.h"
#include <Direct.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExportModelsDialog dialog


CExportModelsDialog::CExportModelsDialog(ObjectCacheContainerHandle *occh)
	: CDialog(CExportModelsDialog::IDD, CWnd::FromHandle(GetMainApplicationWnd()))
{
   m_occh = occh;
   m_dir = GetRegistryString("AV4 Plugin", "Output Directory", ".");
   m_generatemodelmaps = GetRegistryString("AV4 Plugin", "Generate Model Maps", "FALSE")=="TRUE";
   m_minsize = GetRegistryValue("AV4 Plugin", "Min Map Size", (int)ML_0);
   m_maxsize = GetRegistryValue("AV4 Plugin", "Max Map Size", (int)ML_256);
	//{{AFX_DATA_INIT(CExportModelsDialog)
	//}}AFX_DATA_INIT
}

void CExportModelsDialog::FillCombo(int id)
{
   int index;
   CComboBox *combo = (CComboBox *)GetDlgItem(id);
   index = combo->AddString("0");
   combo->SetItemData(index, (DWORD) ML_0);

   index = combo->AddString("1");
   combo->SetItemData(index, (DWORD) ML_1);

   index = combo->AddString("2");
   combo->SetItemData(index, (DWORD) ML_2);

   index = combo->AddString("4");
   combo->SetItemData(index, (DWORD) ML_4);

   index = combo->AddString("8");
   combo->SetItemData(index, (DWORD) ML_8);

   index = combo->AddString("16");
   combo->SetItemData(index, (DWORD) ML_16);

   index = combo->AddString("32");
   combo->SetItemData(index, (DWORD) ML_32);

   index = combo->AddString("64");
   combo->SetItemData(index, (DWORD) ML_64);

   index = combo->AddString("128");
   combo->SetItemData(index, (DWORD) ML_128);

   index = combo->AddString("256");
   combo->SetItemData(index, (DWORD) ML_256);
}

BOOL CExportModelsDialog::OnInitDialog() 
{
   FillCombo(IDC_MINSIZE_COMBO);
   FillCombo(IDC_MAXSIZE_COMBO);
   GetDlgItem(IDC_MINSIZE_COMBO)->EnableWindow(m_generatemodelmaps);
   GetDlgItem(IDC_MAXSIZE_COMBO)->EnableWindow(m_generatemodelmaps);

   CDialog::OnInitDialog();

   int iIndex;
   m_selected.RemoveAll();
   for (ModelCacheHandle *mch = m_occh->GetFirstModelCacheHandle(); mch; mch=mch->GetSiblingModelCacheHandle()) {
      String name = mch->GetName();
      iIndex = m_checklistbox.AddString(name.GetNameOnlyWithoutSuffix().Get());
      m_checklistbox.SetItemData(iIndex, (DWORD)mch);
      m_checklistbox.SetCheck(iIndex, TRUE);
   }
   GetDlgItem(IDC_OUTPUTDIR)->SetWindowText(m_dir.Get());

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CExportModelsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CExportModelsDialog)
	DDX_Check(pDX, IDC_GENERATEMODELMAPS, m_generatemodelmaps);
	DDX_CBIndex(pDX, IDC_MINSIZE_COMBO, m_minsize);
	DDX_CBIndex(pDX, IDC_MAXSIZE_COMBO, m_maxsize);
	DDX_Control(   pDX, IDC_MODELLIST_COMBO, m_checklistbox);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CExportModelsDialog, CDialog)
	//{{AFX_MSG_MAP(CExportModelsDialog)
	ON_CBN_SELCHANGE(IDC_MAXSIZE_COMBO, OnMaxSize)
	ON_CBN_SELCHANGE(IDC_MINSIZE_COMBO, OnMinSize)
	ON_BN_CLICKED(IDC_GENERATEMODELMAPS, OnGeneratemodelmaps)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExportModelsDialog message handlers

void CExportModelsDialog::OnOK() 
{
	CDialog::OnOK();
   for (int iIndex=0; iIndex < m_checklistbox.GetCount(); iIndex++) {
      if (m_checklistbox.GetCheck(iIndex)) {
          ModelCacheHandle *mch = (ModelCacheHandle *)m_checklistbox.GetItemData(iIndex);
          m_selected.AddTail(mch);
      }
   }
   SetRegistryString("AV4 Plugin", "Output Directory", m_dir);
   SetRegistryString("AV4 Plugin", "Generate Model Maps", m_generatemodelmaps?"TRUE":"FALSE");
   SetRegistryValue("AV4 Plugin", "Min Map Size", m_minsize);
   SetRegistryValue("AV4 Plugin", "Max Map Size", m_maxsize);
}

void CExportModelsDialog::OnMaxSize() 
{
   CComboBox *mincombo = (CComboBox *)GetDlgItem(IDC_MINSIZE_COMBO);
   CComboBox *maxcombo = (CComboBox *)GetDlgItem(IDC_MAXSIZE_COMBO);
   
   int index = maxcombo->GetCurSel();
   int maxlimit = maxcombo->GetItemData(index);

   index = mincombo->GetCurSel();
   int minlimit = mincombo->GetItemData(index);
   
   
   if (maxlimit < minlimit)
      maxcombo->SetCurSel(index);
}

void CExportModelsDialog::OnMinSize() 
{
   CComboBox *mincombo = (CComboBox *)GetDlgItem(IDC_MINSIZE_COMBO);
   CComboBox *maxcombo = (CComboBox *)GetDlgItem(IDC_MAXSIZE_COMBO);
   
   int index = mincombo->GetCurSel();
   int minlimit = mincombo->GetItemData(index);
   
   index = maxcombo->GetCurSel();
   int maxlimit = maxcombo->GetItemData(index);
   
   if (minlimit > maxlimit)
      mincombo->SetCurSel(index);
}

void CExportModelsDialog::OnGeneratemodelmaps() 
{
   m_generatemodelmaps = IsDlgButtonChecked(IDC_GENERATEMODELMAPS);
   GetDlgItem(IDC_MINSIZE_COMBO)->EnableWindow(m_generatemodelmaps);
   GetDlgItem(IDC_MAXSIZE_COMBO)->EnableWindow(m_generatemodelmaps);
}

void CExportModelsDialog::OnBrowse() 
{
   CString caption;
   caption.LoadString(IDS_CHOOSEDIRECTORY);
   if (!DirRequest(m_dir, (LPCTSTR)caption/*NULL*/, m_dir.Get(), this))
      return;
   GetDlgItem(IDC_OUTPUTDIR)->SetWindowText(m_dir.Get());
}
