// TClampDialog.cpp : implementation file
//

#include "stdafx.h"
//#include "Wizard.h"
#include "TClampDialog.h"
#include "F_Protos.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// TClampDialog dialog

TClampDialog::TClampDialog(CWnd* pParent /*=NULL*/)
	: CDialog(TClampDialog::IDD, pParent)
//TClampDialog::TClampDialog()
//: CDialog(TClampDialog::IDD, CWnd::FromHandle(GetMainApplicationWnd()))
{
	//{{AFX_DATA_INIT(TClampDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	SingleClamp = false;
	MultipleClampStretch = false;
	MultipleClampNoStretch = false;
}


void TClampDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(TClampDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(TClampDialog, CDialog)
	//{{AFX_MSG_MAP(TClampDialog)
	ON_BN_CLICKED(IDC_CLAMPSINGLE, OnClampsingle)
	ON_BN_CLICKED(IDC_CLMPMULTNOSTRETCH, OnClmpmultnostretch)
	ON_BN_CLICKED(IDC_CLMPMULTSTRETCH, OnClmpmultstretch)
	ON_BN_CLICKED(IDC_BLENDCHECKBOX, OnBlendcheckbox)
	ON_BN_CLICKED(IDC_BROWSEALPHAIMAGE, OnBrowsealphaimage)
	ON_BN_CLICKED(IDC_BROWSEPRIMIMAGE, OnBrowseprimimage)
	ON_BN_CLICKED(IDC_BROWSESECIMAGE, OnBrowsesecimage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// TClampDialog message handlers

void TClampDialog::OnClampsingle() 
{
	// TODO: Add your control notification handler code here
	SingleClamp = true;
	StoreDialogDefaults();
	CDialog::OnOK();
}

void TClampDialog::OnClmpmultnostretch() 
{
	// TODO: Add your control notification handler code here
	MultipleClampStretch = true;
	StoreDialogDefaults();
	CDialog::OnOK();
}

void TClampDialog::OnClmpmultstretch() 
{
	// TODO: Add your control notification handler code here
	MultipleClampNoStretch = true;
	StoreDialogDefaults();
	CDialog::OnOK();
}

void TClampDialog::StoreDialogDefaults()
{
	SetRegistryString("TClamp Plugin", "Blend Mapping",BlendMapping);
 	SetRegistryString("TClamp Plugin", "Primary Map Path", PrimaryMapPath);
 	SetRegistryString("TClamp Plugin", "Secondary Map Path", SecondaryMapPath);
 	SetRegistryString("TClamp Plugin", "Alpha Map Path", AlphaMapPath);
}

void TClampDialog::OnBlendcheckbox()
{
	// TODO: Add your control notification handler code here
	unsigned BlendOn = IsDlgButtonChecked(IDC_BLENDCHECKBOX);

	BlendMapping = BlendOn;

	GetDlgItem(IDC_PRIMIMAGELABEL)->EnableWindow(BlendOn);
	GetDlgItem(IDC_PRIMARYIMAGEEDIT)->EnableWindow(BlendOn);
	GetDlgItem(IDC_BROWSEPRIMIMAGE)->EnableWindow(BlendOn);

	GetDlgItem(IDC_SECIMAGELABEL)->EnableWindow(BlendOn);
	GetDlgItem(IDC_SECONDARYIMAGEEDIT)->EnableWindow(BlendOn);
	GetDlgItem(IDC_BROWSESECIMAGE)->EnableWindow(BlendOn);

	GetDlgItem(IDC_ALPHAIMAGELABEL)->EnableWindow(BlendOn);
	GetDlgItem(IDC_ALPHAIMAGEEDIT)->EnableWindow(BlendOn);
	GetDlgItem(IDC_BROWSEALPHAIMAGE)->EnableWindow(BlendOn);

}

BOOL TClampDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	BlendMapping = GetRegistryValue("TClamp Plugin","Blend Mapping",0);
 	PrimaryMapPath = GetRegistryString("TClamp Plugin", "Primary Map Path","NONE");
 	SecondaryMapPath = GetRegistryString("TClamp Plugin", "Secondary Map Path","NONE");
 	AlphaMapPath = GetRegistryString("TClamp Plugin", "Alpha Map Path","NONE");

	GetDlgItem(IDC_PRIMARYIMAGEEDIT)->SetWindowText(PrimaryMapPath.buf);
	GetDlgItem(IDC_SECONDARYIMAGEEDIT)->SetWindowText(SecondaryMapPath.buf);
	GetDlgItem(IDC_ALPHAIMAGEEDIT)->SetWindowText(AlphaMapPath.buf);

	if (BlendMapping)
		CheckDlgButton(IDC_BLENDCHECKBOX,true);
	else
		CheckDlgButton(IDC_BLENDCHECKBOX,false);


	GetDlgItem(IDC_PRIMIMAGELABEL)->EnableWindow(BlendMapping);
	GetDlgItem(IDC_PRIMARYIMAGEEDIT)->EnableWindow(BlendMapping);
	GetDlgItem(IDC_BROWSEPRIMIMAGE)->EnableWindow(BlendMapping);

	GetDlgItem(IDC_SECIMAGELABEL)->EnableWindow(BlendMapping);
	GetDlgItem(IDC_SECONDARYIMAGEEDIT)->EnableWindow(BlendMapping);
	GetDlgItem(IDC_BROWSESECIMAGE)->EnableWindow(BlendMapping);

	GetDlgItem(IDC_ALPHAIMAGELABEL)->EnableWindow(BlendMapping);
	GetDlgItem(IDC_ALPHAIMAGEEDIT)->EnableWindow(BlendMapping);
	GetDlgItem(IDC_BROWSEALPHAIMAGE)->EnableWindow(BlendMapping);

	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void TClampDialog::OnBrowsealphaimage() 
{
	// TODO: Add your control notification handler code here
	CString pathname = BrowseDialog();
	GetDlgItem(IDC_ALPHAIMAGEEDIT)->SetWindowText((LPCTSTR)pathname);
	AlphaMapPath = pathname;	
}

void TClampDialog::OnBrowseprimimage() 
{
	// TODO: Add your control notification handler code here
	CString pathname = BrowseDialog();
	GetDlgItem(IDC_PRIMARYIMAGEEDIT)->SetWindowText((LPCTSTR)pathname);
	PrimaryMapPath = pathname;	
}

void TClampDialog::OnBrowsesecimage() 
{
	// TODO: Add your control notification handler code here
	CString pathname = BrowseDialog();
	GetDlgItem(IDC_SECONDARYIMAGEEDIT)->SetWindowText((LPCTSTR)pathname);
	SecondaryMapPath = pathname;	
}

CString TClampDialog::BrowseDialog()
{
	CFileDialog dlg(true,NULL,NULL,NULL,"Images (*.tga,*.tif)|*.tga;*.tif|All Files (*.*)|*.*||",NULL);
	dlg.DoModal();
	return dlg.GetPathName();
}
