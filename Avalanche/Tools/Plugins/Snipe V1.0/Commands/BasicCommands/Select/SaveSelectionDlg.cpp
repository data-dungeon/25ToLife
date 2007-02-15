// SaveSelectionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SaveSelectionDlg.h"
#include "HSelection.h"
#include "HMainInterface.h"

// CSaveSelectionDlg dialog

IMPLEMENT_DYNAMIC(CSaveSelectionDlg, CDialog)

CSaveSelectionDlg::CSaveSelectionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSaveSelectionDlg::IDD, pParent)
{
   m_nSelection = 0;
   m_nOperation = 0;
   m_strName = "Untitled";
}

CSaveSelectionDlg::~CSaveSelectionDlg()
{
}

void CSaveSelectionDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_SELECTION_COMBO, m_wndSelectionCombo);
   DDX_CBIndex(pDX, IDC_SELECTION_COMBO, m_nSelection);
   DDX_Text(pDX, IDC_NAME_EDIT, m_strName);
   DDX_Radio(pDX, IDC_NEW_RADIO, m_nOperation);

   BOOL bNewSelection = (m_nSelection == 0);

   GetDlgItem(IDC_NAME_STATIC)->EnableWindow( bNewSelection );
   GetDlgItem(IDC_NAME_EDIT)->EnableWindow( bNewSelection );
   GetDlgItem(IDC_ADD_RADIO)->EnableWindow( !bNewSelection );
   GetDlgItem(IDC_SUBTRACT_RADIO)->EnableWindow( !bNewSelection );
   GetDlgItem(IDC_TOGGLE_RADIO)->EnableWindow( !bNewSelection );

   if (bNewSelection)
      GetDlgItem(IDC_NEW_RADIO)->SetWindowText( "New Selection" );
   else
      GetDlgItem(IDC_NEW_RADIO)->SetWindowText( "Replace Selection" );

   if (pDX->m_bSaveAndValidate && !bNewSelection)
      m_wndSelectionCombo.GetWindowText( m_strName );
}

BEGIN_MESSAGE_MAP(CSaveSelectionDlg, CDialog)
   ON_CBN_SELCHANGE(IDC_SELECTION_COMBO, OnCbnSelchangeSelectionCombo)
END_MESSAGE_MAP()

// CSaveSelectionDlg message handlers

BOOL CSaveSelectionDlg::OnInitDialog()
{
   CDialog::OnInitDialog();

   m_wndSelectionCombo.AddString( "New" );
   HSnipeObject *pCommandObject = GetHMainInterface()->GetCurrentCommandObject();
   if (pCommandObject)
   {
      HSelectionContainer *pSelectionContainer = pCommandObject->GetSelectionContainer();
      if (pSelectionContainer)
      {
         for (int i=0; i<pSelectionContainer->GetNumChildren(); i++)
         {
            HSelection *pSelection = (HSelection *)pSelectionContainer->GetChildAt(i);
            m_wndSelectionCombo.AddString( pSelection->GetName().Get() );
         }
      }
   }

   UpdateData( FALSE );

   CEdit *pEdit = (CEdit *)GetDlgItem(IDC_NAME_EDIT);
   pEdit->SetFocus();
   pEdit->SetSel( 0, -1 );

   return FALSE;
}

void CSaveSelectionDlg::OnCbnSelchangeSelectionCombo()
{
   UpdateData( TRUE );

   if (m_nSelection == 0) // new
   {
      m_nOperation = 0;
      m_strName.Empty();
   }

   UpdateData( FALSE );
}
