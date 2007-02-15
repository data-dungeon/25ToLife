 // OutputView.cpp : implementation file
//

#include "stdafx.h"
#include "Snipe.h"
#include "OutputView.h"
#include "Interface/MainInterface.h"
#include "HierObject.h"
#include "Interface/SnipeCommand.h"
#include "CommandPluginManager.h"
#include "CommandPlugin.h"
#include "resource.h"

IMPLEMENT_DYNCREATE(CGLOutputControl, CGLScrollableControl)

BEGIN_MESSAGE_MAP(CGLOutputControl, CGLScrollableControl)
   ON_COMMAND(ID_OUTPUT_CLEAR, OnClear)
END_MESSAGE_MAP()

CGLOutputControl::CGLOutputControl()
{
   m_glVScroll.SetLine( 13 );
   m_nID = ID_OUTPUTCONTROL;
}

CGLOutputControl::~CGLOutputControl()
{
}

void CGLOutputControl::AppendLine(const char *line)
{
   ASSERT(line);
   if (!line)
      return;

   AFX_MANAGE_STATE(AfxGetAppModuleState());
   OutputDebugString( line );

   String strLine(line);
   BOOL bAppendNL = (strLine.Right(1)=='\n')?TRUE:FALSE;

   StringList *pList = strLine.Split('\n');
   ASSERT(pList);

   OutputLine strCurrLine;
   for (POSITION pos = pList->GetHeadPosition(); pos; )
   {
      strCurrLine.m_strText = pList->GetNext( pos );

      strCurrLine.m_color.Set( 0.0f, 0.0f, 0.0f, 1.0f );
      if (strCurrLine.m_strText[0] == '<')
      {
         if (strCurrLine.m_strText[1] == 'E')
         {
            strCurrLine.m_color.Set( 1.0f, 0.0f, 0.0f, 1.0f );
            strCurrLine.m_strText = strCurrLine.m_strText.Right( strCurrLine.m_strText.Length() - 3 );
            MessageBeep( MB_ICONEXCLAMATION );
         }
         else if (strCurrLine.m_strText[1] == 'W')
         {
            strCurrLine.m_color.Set( 1.0f, 0.4f, 0.0f, 1.0f );
            strCurrLine.m_strText = strCurrLine.m_strText.Right( strCurrLine.m_strText.Length() - 3 );
         }
      }

      if (pos)
         strCurrLine.m_strText += '\n';

      if (!m_linelist.IsEmpty())
      {
         OutputLine &strLast = m_linelist.GetTail();
         if (strLast.m_strText.IsEmpty() || strLast.m_strText.Right(1)!='\n')
         {
            if (strCurrLine.m_strText == "<:>")
            {
               strLast.m_strPath = strLast.m_strText;
               strLast.m_strPath += "> ";
               strLast.m_strText.Empty();
            }
            else
               strLast.m_strText += strCurrLine.m_strText;
            continue;
         }
      }

      m_linelist.AddTail( strCurrLine );
   }

   if (bAppendNL)
   {
      OutputLine &strLast = m_linelist.GetTail();
      strLast.m_strText += '\n';
   }

   delete pList;

   if (m_linelist.GetCount() > 50)
   {
      int nRemoveLines = m_linelist.GetCount() - 50;
      for (int i=0; i<nRemoveLines; i++)
         m_linelist.RemoveHead();
   }

   m_glVScroll.SetMax( 3 + (m_linelist.GetCount()+1) * 13 + 3 );
   m_glVScroll.SetPage( m_szControl.cy );
   m_glVScroll.SetPos( m_glVScroll.GetMax());
   
   Invalidate();
}

void CGLOutputControl::OnClear()
{
   m_linelist.RemoveAll();
   Invalidate();
   GetApp().DrawInvalidControls();
}

void CGLOutputControl::Move( const CPoint &ptControl, const CSize &szControl, Justify justification )
{
   CGLScrollableControl::Move( ptControl, szControl, justification );

   m_glVScroll.SetMax( 3 + (m_linelist.GetCount()+1) * 13 + 3 );
   m_glVScroll.SetPage( m_szControl.cy );
}

void CGLOutputControl::Draw( UINT nDrawFlags/*=DRAW_CHILDREN|DRAW_PARENT*/ )
{
   GLDrawPrepper prep(this);

   if (nDrawFlags & DRAW_PARENT)
      m_pParentControl->Draw( DRAW_PARENT ); // this control has transparency, so have our parent draw first if it hasn't already

   DrawBorder();

   glColor3f( 1.0f, 1.0f, 1.0f );
   glBegin( GL_QUADS );
      glVertex2f( 2.0f, 2.0f );
      glVertex2f( m_szControl.cx-2.0f, 2.0f );
      glVertex2f( m_szControl.cx-2.0f, m_szControl.cy-2.0f );
      glVertex2f( 2.0f, m_szControl.cy-2.0f );
   glEnd();

   int nStart = m_glVScroll.GetPos() / 13;
   int nEnd = min(nStart + m_glVScroll.GetPage() / 13 + 1, m_linelist.GetCount()-1);
   int i = 0;
   for (POSITION pos=m_linelist.GetHeadPosition(); pos; i++)
   {
      OutputLine &strLine = m_linelist.GetNext( pos );

      if (i < nStart)
         continue;
      else if (i > nEnd)
         break;

      try {
         float fLeft = 3.0f;
         if (!strLine.m_strPath.IsEmpty())
         {
            glColor4f( 0.0f, 0.0f, 0.5f, 1.0f );
            fLeft = DrawText( fLeft, 3 + 10 - m_glVScroll.GetPos() + i * 13, strLine.m_strPath );
         }
         glColor4f( strLine.m_color.GetRed(), strLine.m_color.GetGreen(), strLine.m_color.GetBlue(), strLine.m_color.GetAlpha());
         DrawText( fLeft, 3 + 10 - m_glVScroll.GetPos() + i * 13, strLine.m_strText );
      }
      catch(...) {
         // Draw Text Failed
      }
   }

   SnipeObject *pObject = GetMainInterface()->GetCurrentWorkingObject();
   if (pObject) {
      String strDesc;
      GetMainInterface()->ObjectToDescription( pObject, strDesc, FALSE );
      glColor3f( 0.0f, 0.0f, 0.5f );
      DrawText( 3, 3 + 10 - m_glVScroll.GetPos() + i * 13, strDesc + "> " );
   }

   CGLScrollableControl::Draw( nDrawFlags & ~DRAW_PARENT );
}

UINT CGLOutputControl::GetContextMenuID( const CPoint &point, UINT &nDefaultItemID )
{
   return IDR_OUTPUTCONTROL;
}

void CGLOutputControl::OnUpdateCmdUI( CFrameWnd* pTarget, BOOL bDisableIfNoHndler )
{
	CGLOutputControlCmdUI state(this);
   state.DoUpdate(pTarget, bDisableIfNoHndler);
}

void CGLOutputControl::SetCurrentWorkingObject( SnipeObject *pObject )
{
   if (GetMainInterface()->GetCurrentWorkingObject() == pObject)
      return;

   GetMainInterface()->SetCurrentWorkingObject( pObject );
   Invalidate();
}

void CGLOutputControl::SetCurrentCommandObject( SnipeObject *pObject )
{
   if (GetMainInterface()->GetCurrentCommandObject() == pObject)
      return;

   GetMainInterface()->SetCurrentCommandObject( pObject );
}

////////////////////////////////////////////////////////
// CGLOutputControlCmdUI

void CGLOutputControlCmdUI::SetCurrentWorkingObject( SnipeObject *pObject )
{
   CGLOutputControl *pOutput = (CGLOutputControl *)m_pOther;

   if (pOutput)
      pOutput->SetCurrentWorkingObject( pObject );
   Enable( TRUE );
}

void CGLOutputControlCmdUI::SetCurrentCommandObject( SnipeObject *pObject )
{
   CGLOutputControl *pOutput = (CGLOutputControl *)m_pOther;

   if (pOutput)
      pOutput->SetCurrentCommandObject( pObject );
   Enable( TRUE );
}

////////////////////////////////////////////////////////
// CGLCommandLineControl

BOOL CGLCommandLineControl::OnControlNotify( ControlNotificationMsg msg, WPARAM wParam, LPARAM lParam )
{
   switch (msg)
   {
      case NOTIFYCTRL_EN_KEYDOWN:
      {
         UINT nKey = HIWORD(wParam);
         switch (nKey)
         {
            case VK_UP:
               if (m_nHistoryIndex==0 || m_commandhistory.IsEmpty())
                  break;
               m_nHistoryIndex--;
               GetPopupEdit()->SetWindowText( m_commandhistory[m_nHistoryIndex].Get() );
               ((CEdit *)GetPopupEdit())->SetSel( 10000, 10000 );
               return TRUE;
            case VK_DOWN:
               if (m_nHistoryIndex==m_commandhistory.GetCount())
                  break;
               m_nHistoryIndex++;
               if (m_nHistoryIndex < m_commandhistory.GetCount())
                  GetPopupEdit()->SetWindowText( m_commandhistory[m_nHistoryIndex].Get() );
               else
                  GetPopupEdit()->SetWindowText( "" );
               ((CEdit *)GetPopupEdit())->SetSel( 10000, 10000 );
               return TRUE;
         }
         break;
      }
   }

   return CGLEditControl::OnControlNotify( msg, wParam, lParam );
}

void CGLCommandLineControl::OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)
{
   CommandList *pCommandList = GetMainInterface()->GetCurrentCommandList();
   SetEnabled( pCommandList!=NULL );
   if (pCommandList)
   {
      SnipeCommand *pTempCommand = pCommandList->GetTempCommand();
      if (pTempCommand)
      {
         SetText( pTempCommand->GetCommandInfo( FALSE, FALSE ), TRUE );
         return;
      }
   }

   SetText( "", TRUE );
}

////////////////////////////////////////////////////////
// CGLOutputFrame

void CGLOutputFrame::PostInit()
{
   CGLTransparentControl::PostInit();

   m_glOutputControl.Init( this );
   m_glCommandLineControl.Init( this );
}

void CGLOutputFrame::Move( const CPoint &ptControl, const CSize &szControl, Justify justification/*=JUST_NOCHANGE*/ )
{
   CGLTransparentControl::Move( ptControl, szControl, justification );

   m_glOutputControl.Move( CPoint(0,0), CSize(szControl.cx, szControl.cy - 22) );
   m_glCommandLineControl.Move( CPoint(0,szControl.cy - 20), CSize(szControl.cx, 20) );
}

BOOL CGLOutputFrame::OnControlNotify( ControlNotificationMsg msg, WPARAM wParam, LPARAM lParam )
{
   switch (msg)
   {
      case NOTIFYCTRL_EN_CHANGED: 
      {
         CGLEditControl *pEditControl = (CGLEditControl *)lParam;
         BOOL bUserChanged = HIWORD( wParam );
         if (pEditControl == &m_glCommandLineControl && bUserChanged)
         {
            CGLCommandLineControl *pCommandEdit = (CGLCommandLineControl *)pEditControl;

            String strText;
            pEditControl->GetText( strText );

            int nSeparator = strText.Find( ' ' );
            String strCommand, strParameters;
            if (nSeparator == -1)
               strCommand = strText;
            else
            {
               strCommand = strText.Left( nSeparator );
               strParameters = strText.Right( strText.Length() - nSeparator - 1 );
            }

            if (strCommand.IsEmpty())
               AfxGetMainWnd()->SetFocus();
            else
            {
               pCommandEdit->m_commandhistory.Add( strText );
               pCommandEdit->m_nHistoryIndex = pCommandEdit->m_commandhistory.GetCount();

               CommandPlugin *pCommandPlugin = GetMainInterface()->GetCommandPluginManager()->FindCommandByCLIName( "", strCommand );
               if (pCommandPlugin)
               {
                  StringList strResults;
                  GetMainInterface()->GetCurrentCommandObject()->GetCommandList()->Do( pCommandPlugin, strParameters, strResults );
               }
               else
               {
                  String strDesc;
                  GetMainInterface()->ObjectToDescription( GetMainInterface()->GetCurrentCommandObject(), strDesc, FALSE );
                  g_outputWindow << strDesc << "<:>" << strCommand << "\n";

                  g_outputWindow << "<E>" << strCommand << " is not a recognized command.\n";
               }
            }
            return TRUE;
         }
         break;
      }
   }

   return CGLTransparentControl::OnControlNotify( msg, wParam, lParam );
}
