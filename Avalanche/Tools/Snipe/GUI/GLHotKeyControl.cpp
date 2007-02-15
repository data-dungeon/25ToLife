#include "stdafx.h"
#include "GLHotKeyControl.h"
#include "GLRenderWnd.h"

class CHotKeyPopup : public CHotKeyCtrl
{
private:
   CGLHotKeyControl *m_pHotKeyControl;
   BOOL              m_bStoreValueOnDestroy;
   CBrush            m_brush;

public:
   BOOL Create( CGLHotKeyControl *pHotKeyControl )
   {
      m_pHotKeyControl = pHotKeyControl;
      m_bStoreValueOnDestroy = TRUE;

      m_brush.DeleteObject();
      m_brush.CreateSolidBrush( pHotKeyControl->GetBkColor() );

      CRect rcEdit = pHotKeyControl->GetViewportRect();
      pHotKeyControl->AdjustEditRect( rcEdit );
      UINT style = WS_CHILD|WS_VISIBLE;
      if (pHotKeyControl->m_bCenterText)
         style |= ES_CENTER;
      if (!CHotKeyCtrl::Create( style, rcEdit, pHotKeyControl->m_pGLRenderWnd, 0 ))
         return FALSE;

      CFont *pFont;
      pFont = CFont::FromHandle((HFONT)GetStockObject(DEFAULT_GUI_FONT));
      if (!pFont)
         pFont = CFont::FromHandle((HFONT)GetStockObject(ANSI_VAR_FONT));
      SetFont( pFont, FALSE );

      SetFocus();
      SetCapture();

      String strValue;
      pHotKeyControl->GetText( strValue );
      SetWindowText( strValue.Get());
//      SetSel( 0, -1 );

      return TRUE;
   }

private:
   virtual BOOL PreTranslateMessage(MSG* pMsg)
   {
      if (pMsg->message == WM_KEYDOWN || pMsg->message == WM_CHAR) {
         switch (pMsg->wParam) {
            case VK_TAB:
               return CHotKeyCtrl::PreTranslateMessage(pMsg);
            case VK_ESCAPE:
               m_bStoreValueOnDestroy = FALSE;
               GetParent()->SetFocus();
               return TRUE;
            case VK_RETURN:
               GetParent()->SetFocus();
               return TRUE;
            default:
               ::TranslateMessage( pMsg );
               ::DispatchMessage( pMsg );
               break;
         }
         return TRUE;
      }
      return CHotKeyCtrl::PreTranslateMessage(pMsg);
   }

   DECLARE_MESSAGE_MAP()
   afx_msg HBRUSH CtlColor( CDC* pDC, UINT nCtlColor )
   {
      pDC->SetBkColor( m_pHotKeyControl->GetBkColor() );
      pDC->SetTextColor( RGB( 0,0,0 ));
      return m_brush;
   }
   afx_msg void OnKillFocus( CWnd *pNewWnd )
   {
      DestroyWindow();
   }
   afx_msg void OnDestroy()
   {
      if (m_bStoreValueOnDestroy) {
         CString strText;
         GetWindowText( strText );
         m_pHotKeyControl->SetText( (LPCTSTR)strText, TRUE );
      }
      m_pHotKeyControl = NULL;
   }
};

BEGIN_MESSAGE_MAP(CHotKeyPopup, CHotKeyCtrl)
	//{{AFX_MSG_MAP(CEditPopup)
	//}}AFX_MSG_MAP
   ON_WM_CTLCOLOR_REFLECT()
   ON_WM_KILLFOCUS()
   ON_WM_DESTROY()
END_MESSAGE_MAP()

////////////////////////////////////////////////////////
// CGLHotKeyControl

static CHotKeyPopup f_edit;

CGLHotKeyControl::CGLHotKeyControl()
{
}

CGLHotKeyControl::~CGLHotKeyControl()
{
}

void CGLHotKeyControl::CreatePopupEdit()
{
   f_edit.Create( this );
}

CWnd *CGLHotKeyControl::GetPopupEdit()
{
   if (f_edit.IsWindowVisible())
      return &f_edit;
   else
      return NULL;
}

