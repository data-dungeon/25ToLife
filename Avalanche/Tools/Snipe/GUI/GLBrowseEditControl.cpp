#include "stdafx.h"
#include "Snipe.h"
#include "GLBrowseEditControl.h"
#include "../Utility/FileDialog.h"
#include "resource.h"

/////////////////////////////////////////
// CGLBrowseEditControl

BOOL CGLBrowseEditControl::OnControlNotify( ControlNotificationMsg msg, WPARAM wParam, LPARAM lParam )
{
   switch (msg) {
      case NOTIFYCTRL_BN_CLICKED:
         OnBrowse();
         return TRUE;
   }
   return CGLTransparentControl::OnControlNotify( msg, wParam, lParam );
}

// Overridables
void CGLBrowseEditControl::SetText( const String &strText, BOOL bNotify )
{
   m_glFileNameEditControl.SetText( strText, bNotify );
}

void CGLBrowseEditControl::GetText( String &strValue ) const
{
   m_glFileNameEditControl.GetText( strValue );
}

void CGLBrowseEditControl::OnBrowse()
{
   String strFileName;

   ASSERT (!m_strFilters.IsEmpty());

   GetText( strFileName );

   if (m_bOpenDialog) {
      if (!GetOpenFileName(strFileName, m_strFilters, strFileName))
         return;
   }
   else {
      if (!GetSaveFileName(strFileName, m_strFilters, strFileName))
         return;
   }

   SetText(strFileName, TRUE);
};

#define BUTTONSIZE 20

void CGLBrowseEditControl::PostInit()
{
   CGLTransparentControl::PostInit();

   CSize szControl = GetControlSize();
   m_glFileNameEditControl.Init( this, NULL, CPoint(0,0), CSize(szControl.cx-BUTTONSIZE-1, szControl.cy));
   m_glBrowseButton.Init( this, NULL, CPoint(szControl.cx-BUTTONSIZE,0), CSize(BUTTONSIZE, szControl.cy), ID_BROWSEEDIT );
}

void CGLBrowseEditControl::Move( const CPoint &ptControl, const CSize &szControl, Justify justification/*=JUST_NOCHANGE*/ )
{
   CGLTransparentControl::Move( ptControl, szControl, justification );
 
   m_glFileNameEditControl.Move(CPoint(0,0), CSize(szControl.cx-BUTTONSIZE-1, szControl.cy));
   m_glBrowseButton.Move(CPoint(szControl.cx-BUTTONSIZE,0), CSize(BUTTONSIZE, szControl.cy));
}


