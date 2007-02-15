// ExportActionsDialog.cpp : implementation file
//

#include "stdafx.h"
#include "ie.h"
#include "ExportActionsDialog.h"
#include "HashHand.h"
#include "F_Protos.h"
#include "MapLimit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExportActionsDialog dialog


CExportActionsDialog::CExportActionsDialog(ActionCacheContainerHandle *acch)
	: CDialog(CExportActionsDialog::IDD, CWnd::FromHandle(GetMainApplicationWnd()))
{
   m_acch = acch;
   m_dir = GetRegistryString("OBJ Plugin", "Output Directory", ".");
   m_generatemodelmaps = GetRegistryString("OBJ Plugin", "Generate Model Maps", "FALSE")=="TRUE";
   m_minsize = GetRegistryValue("OBJ Plugin", "Min Map Size", (int)ML_0);
   m_maxsize = GetRegistryValue("OBJ Plugin", "Max Map Size", (int)ML_256);
	m_subtype = GetRegistryValue("OBJ Plugin", "Sub Type", (int)ST_1);
	//{{AFX_DATA_INIT(CExportActionsDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CExportActionsDialog::FillCombo(int id)
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

BOOL CExportActionsDialog::OnInitDialog() 
{
   FillCombo(IDC_MINSIZE_COMBO);
   FillCombo(IDC_MAXSIZE_COMBO);
   GetDlgItem(IDC_MINSIZE_COMBO)->EnableWindow(m_generatemodelmaps);
   GetDlgItem(IDC_MAXSIZE_COMBO)->EnableWindow(m_generatemodelmaps);

   CDialog::OnInitDialog();

   int iIndex;
   m_selected.RemoveAll();
   for (ActionCacheHandle *ach = m_acch->GetFirstActionCacheHandle(); ach; ach=ach->GetSiblingActionCacheHandle()) {
      String name = ach->GetName();
      iIndex = m_checklistbox.AddString(name.GetNameOnlyWithoutSuffix().Get());
      m_checklistbox.SetItemData(iIndex, (DWORD)ach);
      m_checklistbox.SetCheck(iIndex, TRUE);
   }
   GetDlgItem(IDC_OUTPUTDIR)->SetWindowText(m_dir.Get());
   CString text;
   text.LoadString(IDS_ACTIONSTOEXPORT);
   GetDlgItem(IDC_LISTLABEL)->SetWindowText(text);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CExportActionsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CExportActionsDialog)
	DDX_Check(pDX, IDC_GENERATEMODELMAPS, m_generatemodelmaps);
	DDX_CBIndex(pDX, IDC_MINSIZE_COMBO, m_minsize);
	DDX_CBIndex(pDX, IDC_MAXSIZE_COMBO, m_maxsize);
	DDX_Control(   pDX, IDC_MODELLIST_COMBO, m_checklistbox);
	DDX_Radio(pDX, IDC_SUB1, m_subtype);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CExportActionsDialog, CDialog)
	//{{AFX_MSG_MAP(CExportActionsDialog)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_CBN_SELCHANGE(IDC_MAXSIZE_COMBO, OnMaxSize)
	ON_CBN_SELCHANGE(IDC_MINSIZE_COMBO, OnMinSize)
	ON_BN_CLICKED(IDC_GENERATEMODELMAPS, OnGeneratemodelmaps)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExportActionsDialog message handlers

void CExportActionsDialog::OnOK() 
{
	CDialog::OnOK();
   for (int iIndex=0; iIndex < m_checklistbox.GetCount(); iIndex++) {
      if (m_checklistbox.GetCheck(iIndex)) {
          ActionCacheHandle *ach = (ActionCacheHandle *)m_checklistbox.GetItemData(iIndex);
          m_selected.AddTail(ach);
      }
   }
   SetRegistryString("OBJ Plugin", "Output Directory", m_dir);
   SetRegistryString("OBJ Plugin", "Generate Model Maps", m_generatemodelmaps?"TRUE":"FALSE");
   SetRegistryValue("OBJ Plugin", "Min Map Size", m_minsize);
   SetRegistryValue("OBJ Plugin", "Max Map Size", m_maxsize);
   SetRegistryValue("OBJ Plugin", "Sub Type", (int)m_subtype);
}

void CExportActionsDialog::OnMaxSize() 
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

void CExportActionsDialog::OnMinSize() 
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

void CExportActionsDialog::OnGeneratemodelmaps() 
{
   m_generatemodelmaps = IsDlgButtonChecked(IDC_GENERATEMODELMAPS);
   GetDlgItem(IDC_MINSIZE_COMBO)->EnableWindow(m_generatemodelmaps);
   GetDlgItem(IDC_MAXSIZE_COMBO)->EnableWindow(m_generatemodelmaps);
}

void CExportActionsDialog::OnBrowse() 
{
   CString caption;
   caption.LoadString(IDS_CHOOSEDIRECTORY);
   if (!DirRequest(m_dir, (LPCTSTR)caption/*NULL*/, m_dir.Get(), this))
      return;
   GetDlgItem(IDC_OUTPUTDIR)->SetWindowText(m_dir.Get());
}
