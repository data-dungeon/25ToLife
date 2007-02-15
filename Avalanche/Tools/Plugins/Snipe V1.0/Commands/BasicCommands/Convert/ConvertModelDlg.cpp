// ConvertModelDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ConvertModelDlg.h"
//#include "HSelection.h"
#include "HMainInterface.h"
#include ".\convertmodeldlg.h"

// ConvertModelDlg dialog

IMPLEMENT_DYNAMIC(ConvertModelDlg, CDialog)

ConvertModelDlg::ConvertModelDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ConvertModelDlg::IDD, pParent)
{
   m_nSelection = 0;
   m_nFromModelType = SOT_SnipeObject;
   m_nToModelType = SOT_SnipeObject;
}

ConvertModelDlg::~ConvertModelDlg()
{
}

void ConvertModelDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_TOMODELTYPE_COMBO, m_wndToModelTypeCombo);
   DDX_CBIndex(pDX, IDC_TOMODELTYPE_COMBO, m_nSelection);
}

BEGIN_MESSAGE_MAP(ConvertModelDlg, CDialog)
   ON_CBN_SELCHANGE(IDC_TOMODELTYPE_COMBO, OnCbnSelchangeToModelType)
END_MESSAGE_MAP()

// ConvertModelDlg message handlers

BOOL ConvertModelDlg::OnInitDialog()
{
   BOOL bResult = CDialog::OnInitDialog();

   switch (m_nFromModelType)
   {
   case SOT_PolyModel :
      {
//         m_wndToModelTypeCombo.AddString( "BezierSplineModel" );
         m_wndToModelTypeCombo.AddString( "PatchModel" );
//         m_snipeObjectArray.Add(SOT_BezierSplineModel);
         m_snipeObjectArray.Add(SOT_PatchModel);
      }
      break;
   case SOT_PatchModel:
      {
         m_wndToModelTypeCombo.AddString("PolyModel");
         m_snipeObjectArray.Add(SOT_PolyModel);
      }
      break;
   }

   m_nToModelType = m_snipeObjectArray[m_nSelection];

   UpdateData( FALSE );

   return bResult;
}

void ConvertModelDlg::OnCbnSelchangeToModelType()
{
   // TODO: Add your control notification handler code here
   UpdateData( TRUE );

   m_nToModelType = m_snipeObjectArray[m_nSelection];
 
   UpdateData( FALSE );
}
