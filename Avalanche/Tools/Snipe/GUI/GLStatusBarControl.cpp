//

#include "stdafx.h"
#include "GLStatusBarControl.h"
#include "Resource.h"

CGLStatusBarControl::CGLStatusBarControl()
// : m_glProgressControl( TRUE )
{
}

void CGLStatusBarControl::PostInit()
{
   CGLTransparentControl::PostInit();

//   m_glProgressControl.Init( this );
}

void CGLStatusBarControl::Move( const CPoint &ptControl, const CSize &szControl, Justify justification/*=JUST_NOCHANGE*/ )
{
   CGLTransparentControl::Move( ptControl, szControl, justification );

   int nCount = 0;
   for (int i=0; i<m_childcontrolarray.GetCount(); i++)
   {
      CGLControl *pChild = m_childcontrolarray[i];
      if (pChild->IsVisible())
         nCount++;
   }

   if (nCount)
   {
      CPoint pos(2,2);
      CSize size((m_szControl.cx-4 - (nCount-1)*8)/nCount, m_szControl.cy-4 );
      for (int i=0; i<m_childcontrolarray.GetCount(); i++)
      {
         CGLControl *pChild = m_childcontrolarray[i];
         if (pChild->IsVisible())
         {
            pChild->Move( pos, size );
            pos.x += size.cx + 8;
         }
      }
   }
//   m_glProgressControl.Move( CPoint(1,1), CSize(m_szControl.cx-2, m_szControl.cy-2) );
}

///////////////////////////////////////////////////////////////
// CGLStatusPaneControl

CGLStatusPaneControl::CGLStatusPaneControl()
{
   m_pText = NULL;
}

BOOL CGLStatusPaneControl::SetIcon( UINT nBitmapID, int nIconWidth/*=0*/, int nIndex/*=0*/ )
{
   m_nIconIndex = nIndex;
   return m_glImageListIcon.Create( nBitmapID, nIconWidth );
}

BOOL CGLStatusPaneControl::SetText( const char *pText )
{
   if (m_pText == pText)
      return FALSE;

   m_pText = pText;
   Invalidate();
   return TRUE;
}

void CGLStatusPaneControl::PostInit()
{
   CGLControl::PostInit();

   m_glImageListBackground.Create( IDB_STATUSPANE, CRect(8,8,10,10) );
}

void CGLStatusPaneControl::Draw( UINT nDrawFlags/*=DRAW_CHILDREN|DRAW_PARENT*/ )
{
   GLDrawPrepper prep(this);

   if (nDrawFlags & DRAW_PARENT)
      m_pParentControl->Draw( DRAW_PARENT ); // this control has transparency, so have our parent draw first if it hasn't already

   glClear( GL_DEPTH_BUFFER_BIT );

   int nLeft = 0;
   if (!m_glImageListIcon.IsEmpty())
   {
      m_glImageListIcon.Draw( 0.0f, 1.0f, m_nIconIndex );
      nLeft += m_glImageListIcon.GetWidthEach() + 2;
   }
   m_glImageListBackground.DrawDiced( CRect(CPoint(nLeft,0), CSize(m_szControl.cx-nLeft, m_szControl.cy)));

   glColor4f( 0,0,0,1 );
   DrawText( nLeft + 4, 15, m_pText );

   CGLControl::Draw( nDrawFlags );
}

///////////////////////////////////////////////////////////////
// CGLKeyModifierPaneControl

CGLKeyModifierPaneControl::CGLKeyModifierPaneControl()
{
}

BOOL CGLKeyModifierPaneControl::SetIcon( UINT nBitmapID, int nIconWidth/*=0*/, int nIndex/*=0*/ )
{
   m_nIconIndex = nIndex;
   return m_glImageListIcon.Create( nBitmapID, nIconWidth );
}

BOOL CGLKeyModifierPaneControl::SetKeyModifiers( KeyModifiers keymodifiers )
{
   if (m_keymodifiers == keymodifiers)
      return FALSE;

   m_keymodifiers = keymodifiers;
   Invalidate();
   return TRUE;
}

void CGLKeyModifierPaneControl::PostInit()
{
   CGLControl::PostInit();

   m_glImageListBackground.Create( IDB_STATUSPANE, CRect(8,8,10,10) );
}

const char *CGLKeyModifierPaneControl::GetText( KeyModifiers nKeyModifier ) const
{
   switch (nKeyModifier.GetValue())
   {
   case KEYMOD_NONE:
      return NULL;
   case KEYMOD_SHIFT:
      return "Shift";
   case KEYMOD_CTRL:
      return "Ctrl";
   case KEYMOD_ALT:
      return "Alt";
   case KEYMOD_SHIFTCTRL:
      return "Shift+Ctrl";
   case KEYMOD_SHIFTALT:
      return "Shift+Alt";
   case KEYMOD_CTRLALT:
      return "Ctrl+Alt";
   case KEYMOD_SHIFTCTRLALT:
      return "Shift+Ctrl+Alt";
   default:
      return NULL;
   }
}

BOOL CGLKeyModifierPaneControl::IsActive( KeyModifiers nKeyModifier ) const
{
   switch (nKeyModifier.GetValue())
   {
   case KEYMOD_NONE:
      return !IsDownShift() && !IsDownControl() && !IsDownAlt();
   case KEYMOD_SHIFT:
      return IsDownShift() && !IsDownControl() && !IsDownAlt();
   case KEYMOD_CTRL:
      return !IsDownShift() && IsDownControl() && !IsDownAlt();
   case KEYMOD_ALT:
      return !IsDownShift() && !IsDownControl() && IsDownAlt();
   case KEYMOD_SHIFTCTRL:
      return IsDownShift() && IsDownControl() && !IsDownAlt();
   case KEYMOD_SHIFTALT:
      return IsDownShift() && !IsDownControl() && IsDownAlt();
   case KEYMOD_CTRLALT:
      return !IsDownShift() && IsDownControl() && IsDownAlt();
   case KEYMOD_SHIFTCTRLALT:
      return IsDownShift() && IsDownControl() && IsDownAlt();
   default:
      return FALSE;
   }
}

void CGLKeyModifierPaneControl::Draw( UINT nDrawFlags/*=DRAW_CHILDREN|DRAW_PARENT*/ )
{
   GLDrawPrepper prep(this);

   if (nDrawFlags & DRAW_PARENT)
      m_pParentControl->Draw( DRAW_PARENT ); // this control has transparency, so have our parent draw first if it hasn't already

   glClear( GL_DEPTH_BUFFER_BIT );

   int nLeft = 0;
   if (!m_glImageListIcon.IsEmpty())
   {
      m_glImageListIcon.Draw( 0.0f, 1.0f, m_nIconIndex );
      nLeft += m_glImageListIcon.GetWidthEach() + 2;
   }
   m_glImageListBackground.DrawDiced( CRect(CPoint(nLeft,0), CSize(m_szControl.cx-nLeft, m_szControl.cy)));

   glColor4f( 0,0,0,1 );

   nLeft+=4;
   BOOL bPrevText = FALSE;

   for (int i=0; i<8; i++)
   {
      KeyModifiers mask( 1 << i );

      if (m_keymodifiers & mask)
      {
         if (bPrevText)
            nLeft = DrawText( nLeft, 15, ", ");

         const char *szText = GetText(mask);
         BOOL bActive = IsActive(mask);

         if (szText)
         {
            int nNewLeft = DrawText( nLeft, 15, szText );
            if (bActive)
               nLeft =  DrawText( nLeft+1, 15, szText );
            else
               nLeft = nNewLeft;
            bPrevText = TRUE;
         }
      }
   }

   CGLControl::Draw( nDrawFlags );
}

///////////////////////////////////////////////////////////////
// CGLStatusBarWnd

BEGIN_MESSAGE_MAP(CGLStatusBarWnd, CGLRenderWnd)
   ON_WM_CREATE()
END_MESSAGE_MAP()

int CGLStatusBarWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
   if (CGLRenderWnd::OnCreate(lpCreateStruct) == -1)
      return -1;

   m_glStatusBarControl.Init( NULL, this );
//   m_glStatusBarControl.LoadBitmap( IDB_BACKGROUND );

   return 0;
}
