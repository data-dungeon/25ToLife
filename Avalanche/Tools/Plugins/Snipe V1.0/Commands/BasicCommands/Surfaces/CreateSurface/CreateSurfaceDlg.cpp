// CreateSurfaceDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CreateSurfaceDlg.h"
#include "HSurface.h"
#include "HPolyModel.h"
#include "HMainInterface.h"

// CCreateSurfaceDlg dialog

IMPLEMENT_DYNAMIC(CCreateSurfaceDlg, CDialog)

CCreateSurfaceDlg::CCreateSurfaceDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCreateSurfaceDlg::IDD, pParent)
{
   m_nSurface = 0;
   m_nOperation = 0;
   m_strName = "Untitled";
}

CCreateSurfaceDlg::~CCreateSurfaceDlg()
{
}

void CCreateSurfaceDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_SURFACE_COMBO, m_wndSurfaceCombo);
   DDX_CBIndex(pDX, IDC_SURFACE_COMBO, m_nSurface);
   DDX_Text(pDX, IDC_NAME_EDIT, m_strName);
   DDX_Radio(pDX, IDC_NEW_RADIO, m_nOperation);

   BOOL bNewSurface = (m_nSurface == 0);

   GetDlgItem(IDC_NAME_STATIC)->EnableWindow( bNewSurface );
   GetDlgItem(IDC_NAME_EDIT)->EnableWindow( bNewSurface );
   GetDlgItem(IDC_ADD_RADIO)->EnableWindow( !bNewSurface );
   GetDlgItem(IDC_SUBTRACT_RADIO)->EnableWindow( !bNewSurface );
   GetDlgItem(IDC_TOGGLE_RADIO)->EnableWindow( !bNewSurface );

   if (bNewSurface)
      GetDlgItem(IDC_NEW_RADIO)->SetWindowText( "New Surface" );
   else
      GetDlgItem(IDC_NEW_RADIO)->SetWindowText( "Replace Surface" );

   if (pDX->m_bSaveAndValidate && !bNewSurface)
      m_wndSurfaceCombo.GetWindowText( m_strName );
}

BEGIN_MESSAGE_MAP(CCreateSurfaceDlg, CDialog)
   ON_CBN_SELCHANGE(IDC_SURFACE_COMBO, OnCbnSelchangeSurfaceCombo)
END_MESSAGE_MAP()

// CCreateSurfaceDlg message handlers

BOOL CCreateSurfaceDlg::OnInitDialog()
{
   CDialog::OnInitDialog();

   m_wndSurfaceCombo.AddString( "New" );
   HSnipeObject *pCommandObject = GetHMainInterface()->GetCurrentCommandObject();
   if (pCommandObject && pCommandObject->IsKindOf(SOT_PolyModel))
   {
      HPolyModel *pPolyModel = (HPolyModel *)pCommandObject;
      HSurfaceContainer *pSurfaceContainer = pPolyModel->GetSurfaceContainer();
      if (pSurfaceContainer)
      {
         for (int i=0; i<pSurfaceContainer->GetNumChildren(); i++)
         {
            HSurface *pSurface = (HSurface *)pSurfaceContainer->GetChildAt(i);
            m_wndSurfaceCombo.AddString( pSurface->GetName().Get() );
         }
      }
   }

   UpdateData( FALSE );

   CEdit *pEdit = (CEdit *)GetDlgItem(IDC_NAME_EDIT);
   pEdit->SetFocus();
   pEdit->SetSel( 0, -1 );

   return FALSE;
}

void CCreateSurfaceDlg::OnCbnSelchangeSurfaceCombo()
{
   UpdateData( TRUE );

   if (m_nSurface == 0) // new
   {
      m_nOperation = 0;
      m_strName.Empty();
   }

   UpdateData( FALSE );
}
