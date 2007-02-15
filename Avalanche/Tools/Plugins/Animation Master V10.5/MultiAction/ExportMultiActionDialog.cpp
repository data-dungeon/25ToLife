// Dan  12/28/00  \Dan90\Plugin\HXT\KFM\ExportActionDialog.cpp

#include "stdafx.h"
#include "Resource.h"
#include "ExportMultiActionDialog.h"
#include "F_Protos.h"

#include "SDK\HModel.h"
#include "SDK\HAction.h"
#include "SDK\Misc.h"

IMPLEMENT_DYNAMIC(CExportMultiActionDialog, CDialog)

CExportMultiActionDialog::CExportMultiActionDialog(HModel *model) :
CDialog( IDD_EXPORTMULTIACTION, CWnd::FromHandle(GetMainApplicationWnd()) )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

   String path = GetRegistryString("Multi Plugin", "File Path", ".");

   m_filename = model->GetName();
   m_filename = m_filename.GetNameOnlyWithoutSuffix()+".act";
   m_filename = CombineNameToPath(path, m_filename);

   m_exportflattened = GetRegistryString("Multi Plugin", "Export Flattened", "ON")=="ON";
   m_exportbaked = GetRegistryString("Multi Plugin", "Export Baked", "ON")=="ON";
   m_exportkfm = GetRegistryString("Multi Plugin", "Export KFM", "ON")=="ON";
   m_includemodelbone = GetRegistryString("Multi Plugin", "Include Model Bone", "OFF")=="ON";
}

void CExportMultiActionDialog::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   DDX_Check(pDX, IDC_EXPORT_FLATTENED, m_exportflattened);  
   DDX_Check(pDX, IDC_EXPORT_BAKED, m_exportbaked);  
   DDX_Check(pDX, IDC_EXPORT_KFM, m_exportkfm);  
   DDX_Check(pDX, IDC_INCLUDEMODELBONE, m_includemodelbone);  
}

BEGIN_MESSAGE_MAP(CExportMultiActionDialog, CDialog)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_BN_CLICKED(IDC_EXPORT_KFM, OnExportKFM)
END_MESSAGE_MAP()

BOOL CExportMultiActionDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

   CWnd *dlgitem = GetDlgItem(IDC_FILENAME);
   String text;
   text = m_filename.GetAbbreviated(dlgitem);
   dlgitem->SetWindowText(text.Get());
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CExportMultiActionDialog::OnOK()
{
   CDialog::OnOK();

	AFX_MANAGE_STATE(AfxGetStaticModuleState());

   String path = m_filename.GetPathOnly();
   SetRegistryString("Multi Plugin", "File Path", path );

   SetRegistryString("Multi Plugin", "Export Flattened", m_exportflattened?"ON":"OFF");
   SetRegistryString("Multi Plugin", "Export Baked", m_exportbaked?"ON":"OFF");
   SetRegistryString("Multi Plugin", "Export KFM", m_exportkfm?"ON":"OFF");
   SetRegistryString("Multi Plugin", "Include Model Bone", m_includemodelbone?"ON":"OFF");
}

void CExportMultiActionDialog::OnBrowse() 
{
   CString filters, title;
   title.LoadString(IDS_EXPORTTITLE);
   filters.LoadString(IDS_FILEFILTERS);

   String ext("act");
   if (!SaveRequest((LPCTSTR)title, (LPCTSTR)filters, m_filename, ext.Get(), ext.Get()))
      return;
   
   CWnd *dlgitem = GetDlgItem(IDC_FILENAME);
   String text;
   text = m_filename.GetAbbreviated(dlgitem);
   dlgitem->SetWindowText(text.Get());
}

