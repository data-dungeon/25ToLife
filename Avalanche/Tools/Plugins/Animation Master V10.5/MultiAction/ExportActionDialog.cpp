// Dan  12/28/00  \Dan90\Plugin\HXT\KFM\ExportActionDialog.cpp

#include "stdafx.h"
#include "Resource.h"
#include "ExportActionDialog.h"
#include "F_Protos.h"

#include "SDK\HAction.h"
#include "SDK\Misc.h"

IMPLEMENT_DYNAMIC(CExportActionDialog, CDialog)

CExportActionDialog::CExportActionDialog(HActionCache *hac) :
CDialog( IDD_EXPORTACTION, CWnd::FromHandle(GetMainApplicationWnd()) )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

   m_errortolerance = GetRegistryValue("KFM Plugin", "ReductionTolerance", 1.0f);
   String path = GetRegistryString("KFM Plugin", "File Path", ".");

   m_filename = hac->GetName();
   m_filename = m_filename.GetNameOnlyWithoutSuffix() + ".kfm";
   m_filename = CombineNameToPath(path, m_filename);
}

void CExportActionDialog::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   DDX_Text(pDX, IDC_ERRORTOLERANCE, m_errortolerance );
   DDV_MinMaxFloat(pDX, m_errortolerance, 0.0f, 1000.0f );
}

BEGIN_MESSAGE_MAP(CExportActionDialog, CDialog)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
END_MESSAGE_MAP()

BOOL CExportActionDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

   CWnd *dlgitem = GetDlgItem(IDC_FILENAME);
   String text;
   text = m_filename.GetAbbreviated(dlgitem);
   dlgitem->SetWindowText(text.Get());
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CExportActionDialog::OnOK()
{
   CDialog::OnOK();

	AFX_MANAGE_STATE(AfxGetStaticModuleState());
   SetRegistryValue("KFM Plugin", "ReductionTolerance", m_errortolerance );
   String path = m_filename.GetPathOnly();
   SetRegistryString("KFM Plugin", "File Path", path );
}

void CExportActionDialog::OnBrowse() 
{
   CString filters, title;
   title.LoadString(IDS_EXPORTTITLE);
   filters.LoadString(IDS_FILEFILTERS);

   String ext("kfm");
   if (!SaveRequest((LPCTSTR)title, (LPCTSTR)filters, m_filename, ext.Get(), ext.Get()))
      return;
   
   CWnd *dlgitem = GetDlgItem(IDC_FILENAME);
   String text;
   text = m_filename.GetAbbreviated(dlgitem);
   dlgitem->SetWindowText(text.Get());
}

