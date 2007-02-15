#include "stdafx.h"
#include "Snipe.h"
#include "GLEditControl.h"
#include "GLRenderWnd.h"
#include "Resource.h"

/////////////////////////////////////////
// CEditPopup

class CEditPopup : public CEdit
{
private:
   CGLEditControlBase *m_pEditControl;
   BOOL                m_bStoreValueOnDestroy;
   CBrush              m_brush;

public:
   BOOL Create( CGLEditControlBase *pEditControl )
   {
      m_pEditControl = pEditControl;
      m_bStoreValueOnDestroy = TRUE;

      m_brush.DeleteObject();
      m_brush.CreateSolidBrush( m_pEditControl->GetBkColor() );

      CRect rcEdit = pEditControl->GetViewportRect();
      pEditControl->AdjustEditRect( rcEdit );
      UINT style = WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL;
      if (pEditControl->m_bCenterText)
         style |= ES_CENTER;
      if (pEditControl->m_bMultiLine)
         style |= ES_MULTILINE|ES_AUTOVSCROLL|WS_VSCROLL;
      if (!CEdit::Create( style, rcEdit, pEditControl->m_pGLRenderWnd, 0 ))
         return FALSE;

      CFont *pFont;
      pFont = CFont::FromHandle((HFONT)GetStockObject(DEFAULT_GUI_FONT));
      if (!pFont)
         pFont = CFont::FromHandle((HFONT)GetStockObject(ANSI_VAR_FONT));
      static CFont font;
      if (!font.m_hObject)
         font.CreateFont( -11, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial" );
      pFont = &font;
      SetFont( pFont, FALSE );

      SetFocus();

      String strValue;
      pEditControl->GetText( strValue );
      SetWindowText( strValue.Get());
      if (!pEditControl->m_bMultiLine)
         SetSel( 0, -1 );

      return TRUE;
   }

private:
   virtual BOOL PreTranslateMessage(MSG* pMsg)
   {
      if (pMsg->message == WM_KEYDOWN || pMsg->message == WM_CHAR)
      {
         if (pMsg->message == WM_KEYDOWN)
         {
            if (m_pEditControl->OnControlNotify( NOTIFYCTRL_EN_KEYDOWN, MAKEWPARAM(m_pEditControl->m_nID, pMsg->wParam), (LPARAM)m_pEditControl ))
               return TRUE;

            switch (pMsg->wParam)
            {
               case VK_TAB:
                  return CEdit::PreTranslateMessage(pMsg);
               case VK_ESCAPE:
                  m_bStoreValueOnDestroy = FALSE;
                  GetParent()->SetFocus();
                  return TRUE;
               case VK_RETURN:
                  if (m_pEditControl->m_bKillFocusOnEnter)
                     GetParent()->SetFocus();
                  else
                  {
                     if (m_pEditControl->m_bMultiLine)
                     {
                        if (IsDownControl())
                        {
                           //CRect rcWindow;
                           //GetWindowRect( &rcWindow );
                           //CString cstrText;
                           //GetWindowText( cstrText );
                           //String strText = cstrText;
                           //int nLineCount = min(5, strText.GetLineCount() + 1);
                           //SetWindowPos( NULL, 0, 0, rcWindow.Width(), nLineCount * 13, SWP_NOZORDER|SWP_NOMOVE );
                           //m_pEditControl->Move( m_pEditControl->m_ptControl, CSize(m_pEditControl->m_szControl.cx, nLineCount * 14 + 4));
                           break;
                        }
                        else
                           GetParent()->SetFocus();
                     }
                     else
                     {
                        SetControlText();
                        if (IsWindow( m_hWnd ))
                           SetWindowText( "" );
                     }
                  }
                  return TRUE;
            }
         }

         ::TranslateMessage( pMsg );
         ::DispatchMessage( pMsg );

         return TRUE;
      }

      return CEdit::PreTranslateMessage(pMsg);
   }
   void SetControlText()
   {
      CString strText;
      GetWindowText( strText );
      m_pEditControl->SetText( (LPCTSTR)strText, FALSE );
      m_pEditControl->m_pParentControl->OnControlNotify( NOTIFYCTRL_EN_CHANGED, MAKEWPARAM(m_pEditControl->m_nID, TRUE), (LPARAM)m_pEditControl );
   }

   DECLARE_MESSAGE_MAP()
   afx_msg HBRUSH CtlColor( CDC* pDC, UINT nCtlColor )
   {
      pDC->SetBkColor( m_pEditControl->GetBkColor() );
      pDC->SetTextColor( RGB( 0,0,0 ));
      return m_brush;
   }
   afx_msg void OnKillFocus( CWnd *pNewWnd )
   {
      DestroyWindow();
   }
   afx_msg void OnDestroy()
   {
      if (m_bStoreValueOnDestroy)
         SetControlText();
      m_pEditControl = NULL;
      ReleaseCapture();
   }
};

BEGIN_MESSAGE_MAP(CEditPopup, CEdit)
	//{{AFX_MSG_MAP(CEditPopup)
	//}}AFX_MSG_MAP
   ON_WM_CTLCOLOR_REFLECT()
   ON_WM_KILLFOCUS()
   ON_WM_DESTROY()
END_MESSAGE_MAP()

/////////////////////////////////////////
// CGLEditControlBase

CGLEditControlBase::CGLEditControlBase()
{
   m_bCenterText = FALSE;
   m_bReadOnly = FALSE;
   m_bKillFocusOnEnter = TRUE;
   m_bMultiLine = FALSE;

   m_glVScroll.SetVisible( FALSE );
   m_glVScroll.SetLine( 13 );
   m_glVScroll.SetPos( 0 );
}

CGLEditControlBase::~CGLEditControlBase()
{
}

void CGLEditControlBase::SetText( const String &strText, BOOL bNotify ) 
{ 
   StoreText( strText); 
   if (bNotify)
      m_pParentControl->OnControlNotify( NOTIFYCTRL_EN_CHANGED, MAKEWPARAM(m_nID, FALSE), (LPARAM)this );

   if (m_bMultiLine)
   {
      m_glVScroll.SetMax( 3 + strText.GetLineCount() * 13 + 3 );
      m_glVScroll.SetPage( m_szControl.cy );
      m_glVScroll.SetPos( 0 );
   }
}

UINT CGLEditControlBase::GetToolTipText( CPoint point, String &strText ) const
{
   GetText( strText );
   return 0;
}

void CGLEditControlBase::Move( const CPoint &ptControl, const CSize &szControl, Justify justification )
{
   CGLScrollableControl::Move( ptControl, szControl, justification );

   if (m_bMultiLine)
   {
      String strText;
      GetText( strText );
      m_glVScroll.SetMax( 3 + strText.GetLineCount() * 13 + 3 );
      m_glVScroll.SetPage( m_szControl.cy );
   }
}

RGBAFloat CGLEditControlBase::GetTextColor()
{
   if (IsReadOnly() || !IsEnabled())
      return RGBAFloat( 0.0f, 0.0f, 0.0f, 0.5f );
   else
      return RGBAFloat( 0.0f, 0.0f, 0.0f, 1.0f );
}

void CGLEditControlBase::Draw( UINT nDrawFlags/*=DRAW_CHILDREN|DRAW_PARENT*/ )
{
   GLDrawPrepper prep(this);

   if (m_pParentControl && nDrawFlags & DRAW_PARENT)
      m_pParentControl->Draw( DRAW_PARENT ); // this control has transparency, so have our parent draw first if it hasn't already

   GLImageList *pImageList = GetImageList();
   BOOL greyed = IsReadOnly() || !IsEnabled();

   if (pImageList)
      pImageList->DrawDiced( CRect( 0, 0, m_szControl.cx, m_szControl.cy ), greyed?1:0 );

   String strText;
   GetAbbreviatedText( strText );
   RGBAFloat textcolor = GetTextColor();
   glColor4f( textcolor.GetRed(), textcolor.GetGreen(), textcolor.GetBlue(), textcolor.GetAlpha() );

   CRect rcEdit( CPoint(0,0), m_szControl );
   AdjustEditRect( rcEdit );

   SetClip( rcEdit );

   int nTop = 13 - m_glVScroll.GetPos();

   if (m_bCenterText)
   {
      CSize szText = GetTextExtent( strText );
      DrawText( rcEdit.left + (rcEdit.Width() - szText.cx)/2, nTop, strText );
   }
   else
      DrawText( rcEdit.left + 2, nTop, strText );
}

static CPoint f_ptStart;
static CPoint f_ptPrev;
static BOOL f_bDownInEdit;
static BOOL f_bScrubbing;

BOOL CGLEditControlBase::OnLButtonDown( UINT nFlags, CPoint point )
{
   CRect rcEdit( CPoint(0,0), m_szControl );
   AdjustEditRect( rcEdit );

   f_bDownInEdit = rcEdit.PtInRect( point );
   f_bScrubbing = FALSE;

   f_ptStart = f_ptPrev = point;

   return TRUE;
}

BOOL CGLEditControlBase::OnMouseMove( UINT nFlags, CPoint point )
{
   if (m_pCaptureControl==this) {
      if (f_bDownInEdit && abs(point.x-f_ptStart.x)>3)
         f_bScrubbing = TRUE;
      if (f_bScrubbing)
         OnScrub( point.x - f_ptPrev.x );
      f_ptPrev = point;
      return TRUE;
   }
   return FALSE;
}

BOOL CGLEditControlBase::OnLButtonUp( UINT nFlags, CPoint point )
{
   if (f_bDownInEdit && !f_bScrubbing && !m_bReadOnly)
      CreatePopupEdit();

   f_bDownInEdit = FALSE;

   if (f_bScrubbing)
      OnEndScrub();
   f_bScrubbing = FALSE;

   return TRUE;
}

static CEditPopup f_edit;

void CGLEditControlBase::CreatePopupEdit()
{
   if (m_bReadOnly)
      return;

   f_edit.Create( this );
}

CWnd *CGLEditControlBase::GetPopupEdit()
{
   if (f_edit.IsWindowVisible())
      return &f_edit;
   else
      return NULL;
}

/////////////////////////////////////////
// CGLEditControl

GLImageList CGLEditControl::m_glImageListBackground;

void CGLEditControl::PostInit()
{
   CGLEditControlBase::PostInit();

   if (m_glImageListBackground.IsEmpty())
      m_glImageListBackground.Create( IDB_EDITBOX, CRect( 3, 3, 4, 4 ), 7 );
}

void CGLEditControl::StoreText( const String &strText )
{
   if (m_strText == strText)
      return;

   m_strText = strText;
   Invalidate();
}

void CGLEditControl::GetText( String &strText ) const
{
   strText = m_strText;
}

/////////////////////////////////////////
// CGLFileNameEditControl

void CGLFileNameEditControl::StoreText( const String &strText )
{
   CGLEditControl::StoreText( strText );
   m_strAbbreviated = GetAbbreviated( strText );
}

void CGLFileNameEditControl::GetAbbreviatedText( String &strText ) const
{
   strText = m_strAbbreviated;
}

void CGLFileNameEditControl::Move( const CPoint &ptControl, const CSize &szControl, Justify justification/*=JUST_NOCHANGE*/ )
{
   CGLEditControl::Move( ptControl, szControl, justification );
   String strText;
   GetText( strText );
   m_strAbbreviated = GetAbbreviated( strText );
}