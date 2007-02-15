// ExportModelDialog.cpp : implementation file
//

#include "stdafx.h"
#include "Hxt.h"
#include "ExportModelDialog.h"
#include "F_Protos.h"
#include "MapLimit.h"

#include "SDK\HModel.h"
#include "SDK\Misc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExportModelDialog dialog

CExportModelDialog::CExportModelDialog(const String &filename)
: CDialog(CExportModelDialog::IDD, CWnd::FromHandle(GetMainApplicationWnd()))
{
   String path = GetRegistryString("TUP Plugin", "File Path", ".");
   m_filename = filename.GetNameOnlyWithoutSuffix() + ".TUP";
   m_filename = CombineNameToPath(path, m_filename);
   m_generatemodelmaps = GetRegistryString("TUP Plugin", "Generate Model Maps", "FALSE")=="TRUE";
   m_maxsize = GetRegistryValue("TUP Plugin", "Max Map Size", (int)ML_256);
   m_minsize = GetRegistryValue("TUP Plugin", "Min Map Size", (int)ML_0);
	m_subtype = GetRegistryValue("TUP Plugin", "Sub Type", (int)ST_1);

   //{{AFX_DATA_INIT(CExportModelDialog)
	//}}AFX_DATA_INIT
}

void CExportModelDialog::FillCombo(int id)
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

BOOL CExportModelDialog::OnInitDialog() 
{
   CWnd *dlgitem = GetDlgItem(IDC_FILENAME);
   String text;
   text = m_filename.GetAbbreviated(dlgitem);
   dlgitem->SetWindowText(text.Get());

   FillCombo(IDC_MINSIZE_COMBO);
   FillCombo(IDC_MAXSIZE_COMBO);
   CDialog::OnInitDialog();

   GetDlgItem(IDC_MINSIZE_COMBO)->EnableWindow(m_generatemodelmaps);
   GetDlgItem(IDC_MAXSIZE_COMBO)->EnableWindow(m_generatemodelmaps);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CExportModelDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CExportModelDialog)
	DDX_Check(pDX, IDC_GENERATEMODELMAPS, m_generatemodelmaps);
	DDX_CBIndex(pDX, IDC_MAXSIZE_COMBO, m_maxsize);
	DDX_CBIndex(pDX, IDC_MINSIZE_COMBO, m_minsize);
	DDX_Radio(pDX, IDC_SUB1, m_subtype);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CExportModelDialog, CDialog)
	//{{AFX_MSG_MAP(CExportModelDialog)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_CBN_SELCHANGE(IDC_MAXSIZE_COMBO, OnMaxSize)
	ON_CBN_SELCHANGE(IDC_MINSIZE_COMBO, OnMinSize)
	ON_BN_CLICKED(IDC_GENERATEMODELMAPS, OnGeneratemodelmaps)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExportModelDialog message handlers

void CExportModelDialog::OnOK() 
{
	CDialog::OnOK();
   String path = m_filename.GetPathOnly();
   SetRegistryString("TUP Plugin", "File Path", path );
   SetRegistryString("TUP Plugin", "Generate Model Maps", m_generatemodelmaps?"TRUE":"FALSE");
   SetRegistryValue("TUP Plugin", "Min Map Size", m_minsize);
   SetRegistryValue("TUP Plugin", "Max Map Size", m_maxsize);
   SetRegistryValue("TUP Plugin", "Sub Type", (int)m_subtype);
}

void CExportModelDialog::OnBrowse() 
{
   CString filters, title;
   title.LoadString(IDS_EXPORTTITLE);
   filters.LoadString(IDS_FILEFILTERS);

   String ext("TUP");
   if (!SaveRequest((LPCTSTR)title, (LPCTSTR)filters, m_filename, ext.Get(), ext.Get()))
      return;
   
   CWnd *dlgitem = GetDlgItem(IDC_FILENAME);
   String text;
   text = m_filename.GetAbbreviated(dlgitem);
   dlgitem->SetWindowText(text.Get());
}

void CExportModelDialog::OnMaxSize() 
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

void CExportModelDialog::OnMinSize() 
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

void CExportModelDialog::OnGeneratemodelmaps() 
{
   m_generatemodelmaps = IsDlgButtonChecked(IDC_GENERATEMODELMAPS);
   GetDlgItem(IDC_MINSIZE_COMBO)->EnableWindow(m_generatemodelmaps);
   GetDlgItem(IDC_MAXSIZE_COMBO)->EnableWindow(m_generatemodelmaps);
}
