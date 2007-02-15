#include "stdafx.h"
#include "GLStringDlg.h"
#include "GLEditControl.h"
#include "Resource.h"

BEGIN_MESSAGE_MAP(CGLStringDlg, CGLDialogWnd)
   ON_COMMAND(IDOK, OnOK)
   ON_COMMAND(IDCANCEL, OnCancel)
END_MESSAGE_MAP()

CGLStringDlg::CGLStringDlg( const String &strTitle, const String &strRequest, const String &strInitial )
{
   m_strTitle = strTitle;
   m_strRequest = strRequest;
   m_strResult = strInitial;
}

CGLStringDlg::~CGLStringDlg()
{
}

BOOL CGLStringDlg::Create()
{
   if (!CGLDialogWnd::Create( AfxGetMainWnd(), m_strTitle.Get() ))
      return FALSE;

   CSize szContent = GetContentControl()->CreateControlsFromDialogTemplate( IDD_STRING );

   CGLStaticTextControl *pRequestStatic = (CGLStaticTextControl *)GetContentControl()->FindChild( IDC_REQUEST_STATIC );
   pRequestStatic->SetText( m_strRequest );

   CGLEditControl *pEdit = (CGLEditControl *)GetContentControl()->FindChild( IDC_STRING_EDIT );
   pEdit->SetText( m_strResult, TRUE );

   MoveWindow( 0, 0, szContent.cx, szContent.cy + m_glTitleBar.m_szControl.cy );
   CenterWindow();

   pEdit->CreatePopupEdit();

   return TRUE;
}

void CGLStringDlg::OnOK()
{
   CGLEditControl *pEdit = (CGLEditControl *)GetContentControl()->FindChild( IDC_STRING_EDIT );
   pEdit->GetText( m_strResult );
   EndModalLoop( IDOK );
}

void CGLStringDlg::OnCancel()
{
   EndModalLoop( IDCANCEL );
}

BOOL StringRequest( const String &strTitle, const String &strRequest, String &strResult )
{
   CGLStringDlg dlg( strTitle, strRequest, strResult );

   if (dlg.DoModal() != IDOK)
      return FALSE;

   strResult = dlg.m_strResult;

   return TRUE;
}
