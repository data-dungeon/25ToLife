// NoAccelEdit.cpp : implementation file
//

#include "stdafx.h"
#include "AMViewer.h"
#include "NoAccelEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNoAccelEdit

CNoAccelEdit::CNoAccelEdit()
{
}

CNoAccelEdit::~CNoAccelEdit()
{
}


BEGIN_MESSAGE_MAP(CNoAccelEdit, CEdit)
	//{{AFX_MSG_MAP(CNoAccelEdit)
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNoAccelEdit message handlers

BOOL CNoAccelEdit::PreTranslateMessage(MSG* pMsg) 
{
   if (pMsg->message == WM_KEYDOWN) {
      switch (pMsg->wParam) {
         case VK_TAB:
            return CEdit::PreTranslateMessage(pMsg);
         case VK_RETURN:
      		GetParent()->SetFocus();
            return TRUE;
         case VK_ESCAPE:
            ResetText();
            GetParentFrame()->SetFocus();
            return TRUE;
      }
   }
   ::TranslateMessage(pMsg);
   ::DispatchMessage(pMsg);
   return TRUE;
}

void CNoAccelEdit::StoreText()
{
   GetWindowText( m_originaltext );
}

void CNoAccelEdit::ResetText()
{
   SetWindowText( m_originaltext );
}

void CNoAccelEdit::OnSetFocus(CWnd* pOldWnd) 
{
   StoreText();
   CEdit::OnSetFocus( pOldWnd );
}
