//

#include "stdafx.h"
#include "Snipe.h"
#include "GLBaseFrame.h"
#include "GLMainRenderWnd.h"
#include "BaseDoc.h"
#include "SnipeDocTemplate.h"
#include "MainFrm.h"
#include "GLStringDlg.h"
#include "../Utility/FileStream.h"
#include "resource.h"

IMPLEMENT_DYNCREATE(CGLBaseFrame, CGLControl)

CGLBaseFrame::CGLBaseFrame()
{
   m_pViewClass = NULL;
   m_pView = NULL;
   m_bMaximized = FALSE;
   m_pRestoreParent = NULL;
   m_pRestoreControlReplaced = NULL;
}

CGLBaseFrame::~CGLBaseFrame()
{
   if (m_pView)
      delete m_pView;

   if (m_pRestoreControlReplaced)
   {
      ASSERT(m_pRestoreParent);
      int nIndex = m_pRestoreParent->FindChild( this );
      ASSERT( nIndex != -1 );
      m_pRestoreParent->m_childcontrolarray.RemoveAt( nIndex );

      delete m_pRestoreControlReplaced;
   }
}

void CGLBaseFrame::CreateView()
{
   ASSERT( m_pViewClass );

   if (m_pView)
      delete m_pView;

   m_pView = (CGLBaseView *)m_pViewClass->CreateObject();
   if (m_pView == NULL)
   {
      TRACE(traceAppMsg, 0, "Warning: Dynamic create of view %hs failed.\n", m_pViewClass->m_lpszClassName);
      return;
   }

   m_pView->SetParentFrame( this );
   m_pView->Init( this, NULL, CPoint(0,0), CSize(0,0), m_pView->m_nID );
   Move( m_ptControl, m_szControl );
}

void CGLBaseFrame::OnMaximize()
{
   static CRuntimeClass *pRuntimeClass = RUNTIME_CLASS(CGLBaseDocFrame);
   for (CGLControl *pNewParent=GetParent(); pNewParent; pNewParent=pNewParent->GetParent())
   {
      if (pNewParent->IsKindOf( pRuntimeClass ))
      {
         CGLBaseDocFrame *pBaseDocFrame = (CGLBaseDocFrame *)pNewParent;
         pBaseDocFrame->GetCurrentDocument()->m_pControlHead = this;

         m_bMaximized = TRUE;
         m_pRestoreParent = m_pParentControl;
         m_pParentControl = pNewParent;
         m_pRestoreControlReplaced = pNewParent->m_childcontrolarray.GetAt( 1 ); // this index (1) needs to come from a virtual function
         m_pRestoreControlReplaced->SetVisible( FALSE );
         pNewParent->m_childcontrolarray.SetAt( 1, this );

         m_pView->MakeActive();

         pNewParent->Move( pNewParent->m_ptControl, pNewParent->m_szControl );
         UpdateHeader();
         GetApp().DrawInvalidControls();
         break;
      }
   }

   ASSERT( pNewParent );
}

void CGLBaseFrame::OnRestore()
{
   ASSERT( m_pRestoreParent );

   CGLControl *pParent = m_pParentControl;
   ASSERT( pParent->IsKindOf(RUNTIME_CLASS(CGLBaseDocFrame)) );
   CGLBaseDocFrame *pBaseDocFrame = (CGLBaseDocFrame *)pParent;
   pBaseDocFrame->GetCurrentDocument()->m_pControlHead = m_pRestoreControlReplaced;

   m_pRestoreControlReplaced->SetVisible( TRUE );
   m_pParentControl->m_childcontrolarray.SetAt( 1, m_pRestoreControlReplaced );
   m_pRestoreControlReplaced = NULL;
   m_pParentControl = m_pRestoreParent;
   m_pRestoreParent = NULL;
   m_bMaximized = FALSE;

   pParent->Move( pParent->m_ptControl, pParent->m_szControl );
   UpdateHeader();
   GetApp().DrawInvalidControls();
}

void CGLBaseFrame::OnSplit( CGLControl *pFirstCell )
{
   CGLBaseFrame *pSource = (CGLBaseFrame *)pFirstCell;
   m_pViewClass = pSource->m_pViewClass;
}

/////////// 

IMPLEMENT_DYNCREATE(CGLBaseDocFrame, CGLControl)

CGLBaseDocFrame::CGLBaseDocFrame()
{
   GetMainFrame()->GetMainRenderWnd()->m_glDocumentBar.m_frameList.AddTail( this );
   GetMainFrame()->GetMainRenderWnd()->m_glDocumentBar.m_pCurrentFrame = this;
}

CGLBaseDocFrame::~CGLBaseDocFrame()
{
   if (GetMainFrame())
   {
      POSITION pos = GetMainFrame()->GetMainRenderWnd()->m_glDocumentBar.m_frameList.Find( this );
      if (pos)
         GetMainFrame()->GetMainRenderWnd()->m_glDocumentBar.m_frameList.RemoveAt( pos );

      if (GetMainFrame()->GetMainRenderWnd()->m_glDocumentBar.m_pCurrentFrame == this)
         GetMainFrame()->GetMainRenderWnd()->m_glDocumentBar.m_pCurrentFrame = GetMainFrame()->m_glMainRenderWnd.m_glDocumentBar.m_frameList.GetHead();
   }
}

void CGLBaseDocFrame::PostInit()
{
   m_glTabControl.Init( this, NULL, CPoint(0,0), CSize(0,TABHEIGHT), ID_DOCUMENT_TAB );
/*
   int nLastTab = -1;
   for (POSITION pos=GetApp().GetDocList().GetHeadPosition(); pos; ) {
      CBaseDoc *pDoc = GetApp().GetDocList().GetNext( pos );
      CGLTabControl::Tab tab( (LPCTSTR)pDoc->GetTitle(), (LPARAM)pDoc );
      nLastTab = m_glTabControl.AddTab( tab );
   }

   if (nLastTab != -1)
      m_glTabControl.SelectTab( nLastTab );*/
}

BOOL CGLBaseDocFrame::OnControlNotify( ControlNotificationMsg msg, WPARAM wParam, LPARAM lParam )
{
   switch (msg)
   {
      case NOTIFYCTRL_TB_CHANGED:
      {
         UINT nID = LOWORD(wParam);
         int nOldIndex = HIWORD(wParam);
         switch (nID)
         {
            case ID_DOCUMENT_TAB:
            {
               CGLControl *pActiveControl = m_pGLRenderWnd->GetActiveControl();
               if (pActiveControl && pActiveControl->IsDescendentOf( this ))
                  m_pGLRenderWnd->SetActiveControl( NULL );

               CBaseDoc *pOldDoc = (CBaseDoc *)m_glTabControl.GetItemData( nOldIndex );
               CBaseDoc *pNewDoc = GetCurrentDocument();
               // store whatever the control hierarchy currently is in the current doc
               if (pOldDoc)
               {
                  pOldDoc->m_pControlHead = (m_childcontrolarray.GetCount()>1) ? m_childcontrolarray[1] : NULL;
						if (pOldDoc->m_pControlHead)
	                  pOldDoc->m_pControlHead->SetVisible( FALSE );
               }

               // if it's the first time this doc has been shown, set up initial control set
               if (pNewDoc && !pNewDoc->m_pControlHead)
               {
                  pNewDoc->m_pControlHead = pNewDoc->GetDocTemplate()->CreateControl( pNewDoc );
                  pNewDoc->m_pControlHead->Init( this, NULL, CPoint(0,TABHEIGHT), CSize(0,0) );

                  //pFrame->m_glSnipeView.OnSplitBoth();
               }

               // swap in the doc's controls
               m_childcontrolarray.SetSize( 1 ); // remove everything but the tab control
               if (pNewDoc)
               {
                  pNewDoc->m_pControlHead->SetVisible( TRUE );
                  pNewDoc->m_pControlHead->m_pParentControl = this;
                  m_childcontrolarray.Add( pNewDoc->m_pControlHead );
                  if (!pNewDoc->m_viewlist.IsEmpty())
                     pNewDoc->m_viewlist.GetHead()->MakeActive();

                  // refresh the new hierarchy
                  Move( m_ptControl, GetControlSize() );
//                  Update();
               }
               else
                  m_pGLRenderWnd->Invalidate( FALSE );

               return TRUE;
            }
         }
         break;
      }
      case NOTIFYCTRL_TB_REMOVED:
      {
         UINT nID = LOWORD(wParam);
         int nIndex = HIWORD(wParam);
         switch (nID)
         {
            case ID_DOCUMENT_TAB:
            {
               CBaseDoc *pDoc = (CBaseDoc *)m_glTabControl.GetItemData(nIndex);
               if (pDoc)
                  pDoc->OnCloseDocument();
               m_glTabControl.SetVisible( m_glTabControl.GetTabCount()-1 );
               Invalidate();
               return TRUE;
            }
         }
         break;
      }
   }

   return CGLControl::OnControlNotify( msg, wParam, lParam );
}

void CGLBaseDocFrame::Move( const CPoint &ptControl, const CSize &szControl, Justify justification/*=JUST_NOCHANGE*/ )
{
   CGLControl::Move( ptControl, szControl, justification );

   m_glTabControl.Move( CPoint( 0, 0 ), CSize( szControl.cx, TABHEIGHT ));
   if (m_childcontrolarray.GetSize() > 1 && m_childcontrolarray[1]) {
      m_glTabControl.SetVisible( TRUE );
      m_childcontrolarray[1]->Move( CPoint( 0, TABHEIGHT ), CSize( szControl.cx, szControl.cy-TABHEIGHT ));
   }
   else
      m_glTabControl.SetVisible( FALSE );
}

void CGLBaseDocFrame::Draw( UINT nDrawFlags/*=DRAW_CHILDREN|DRAW_PARENT*/ )
{
   if (m_glTabControl.GetTabCount()) {
      CGLControl::Draw( nDrawFlags );
      return;
   }

   GLDrawPrepper prep(this);

   COLORREF bk = GetSysColor( COLOR_APPWORKSPACE );
   glColor3f( (float)GetRValue(bk)/255.0f, (float)GetGValue(bk)/255.0f, (float)GetBValue(bk)/255.0f);
	glBegin( GL_QUADS );
	   glVertex3f( 0, 0, 0 );
	   glVertex3f( m_szControl.cx, 0, 0 );
	   glVertex3f( m_szControl.cx, m_szControl.cy, 0 );
	   glVertex3f( 0, m_szControl.cy, 0 );
	glEnd();

   CGLControl::Draw( nDrawFlags );
}

////////////////////////////////////////////////////////////////////
// CGLDocumentTabControl

BEGIN_MESSAGE_MAP(CGLDocTabControl, CGLTabControl)
   ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
   ON_COMMAND(ID_FILE_SAVE, OnFileSave)
   ON_UPDATE_COMMAND_UI(ID_TABCONTROL_NEWHORIZONTALTABGROUP, OnUpdateTabControlNewHorizontalTabGroup)
   ON_COMMAND(ID_TABCONTROL_NEWHORIZONTALTABGROUP, OnTabControlNewHorizontalTabGroup)
   ON_UPDATE_COMMAND_UI(ID_TABCONTROL_NEWVERTICALTABGROUP, OnUpdateTabControlNewVerticalTabGroup)
   ON_COMMAND(ID_TABCONTROL_NEWVERTICALTABGROUP, OnTabControlNewVerticalTabGroup)
   ON_UPDATE_COMMAND_UI(ID_TABCONTROL_MOVETOPREVIOUSTABGROUP, OnUpdateTabControlMoveToPreviousTabGroup)
   ON_COMMAND(ID_TABCONTROL_MOVETOPREVIOUSTABGROUP, OnTabControlMoveToPreviousTabGroup)
   ON_UPDATE_COMMAND_UI(ID_TABCONTROL_MOVETONEXTTABGROUP, OnUpdateTabControlMoveToNextTabGroup)
   ON_COMMAND(ID_TABCONTROL_MOVETONEXTTABGROUP, OnTabControlMoveToNextTabGroup)
   ON_UPDATE_COMMAND_UI(ID_TABCONTROL_LAYOUT_NEW, OnUpdateTabControlLayoutNew)
   ON_COMMAND(ID_TABCONTROL_LAYOUT_NEW, OnTabControlLayoutNew)
   ON_COMMAND_RANGE(ID_TABCONTROL_LAYOUT_FIRST, ID_TABCONTROL_LAYOUT_LAST, OnLoadLayout)
END_MESSAGE_MAP()

UINT CGLDocTabControl::GetTabMenuID()
{
   return IDR_DOCUMENTTABCONTROL;
}

CBaseDoc *CGLDocTabControl::GetCurrentDocument() const
{
   return (CBaseDoc *)GetItemData( m_nSelectedTab );
}

void CGLDocTabControl::OnUpdateFileSave(CCmdUI *pCmdUI)
{
   pCmdUI->SetText( ("Save " + m_tabarray[m_nSelectedTab].m_name).Get() );
}

void CGLDocTabControl::OnFileSave()
{
//   GetCurrentDocument()->OnFileSave();
}

void CGLDocTabControl::OnUpdateTabControlNewHorizontalTabGroup(CCmdUI *pCmdUI)
{
   pCmdUI->Enable( (m_tabarray.GetCount() > 1) && (GetMainFrame()->GetMainRenderWnd()->m_glDocumentBar.m_glDocFrameSplitter.GetColumnCount() == 1) );
}

void CGLDocTabControl::OnTabControlNewHorizontalTabGroup()
{
   GetMainFrame()->GetMainRenderWnd()->m_glDocumentBar.m_glDocFrameSplitter.AddRow();

   int nOldTab = m_nSelectedTab;

   // switch to a different tab first
   int nNewTab;
   if (m_nSelectedTab == 0)
      nNewTab = 1;
   else
      nNewTab = m_nSelectedTab - 1;
   SelectTab( nNewTab );

   // add a copy of our tab to the new pane
   Tab *pTab = &m_tabarray[nOldTab];
   CGLBaseDocFrame *pNewFrame = GetMainFrame()->GetMainRenderWnd()->m_glDocumentBar.m_frameList.GetTail();
   int nTab = pNewFrame->m_glTabControl.AddTab( *pTab );

   // now remove the tab from us
   pTab->m_lParam = NULL;
   RemoveTab( nOldTab );

   pNewFrame->m_glTabControl.SelectTab( nTab );

   GetApp().DrawInvalidControls();
}

void CGLDocTabControl::OnUpdateTabControlNewVerticalTabGroup(CCmdUI *pCmdUI)
{
   pCmdUI->Enable( (m_tabarray.GetCount() > 1) && (GetMainFrame()->GetMainRenderWnd()->m_glDocumentBar.m_glDocFrameSplitter.GetRowCount() == 1) );
}

void CGLDocTabControl::OnTabControlNewVerticalTabGroup()
{
   GetMainFrame()->GetMainRenderWnd()->m_glDocumentBar.m_glDocFrameSplitter.AddColumn();

   int nOldTab = m_nSelectedTab;

   // switch to a different tab first
   int nNewTab;
   if (m_nSelectedTab == 0)
      nNewTab = 1;
   else
      nNewTab = m_nSelectedTab - 1;
   SelectTab( nNewTab );

   // add a copy of our tab to the new pane
   Tab *pTab = &m_tabarray[nOldTab];
   CGLBaseDocFrame *pNewFrame = GetMainFrame()->GetMainRenderWnd()->m_glDocumentBar.m_frameList.GetTail();
   int nTab = pNewFrame->m_glTabControl.AddTab( *pTab );

   // now remove the tab from us
   pTab->m_lParam = NULL;
   RemoveTab( nOldTab );

   pNewFrame->m_glTabControl.SelectTab( nTab );

   GetApp().DrawInvalidControls();
}

void CGLDocTabControl::OnUpdateTabControlMoveToPreviousTabGroup(CCmdUI *pCmdUI)
{
}

void CGLDocTabControl::OnTabControlMoveToPreviousTabGroup()
{
   // TODO: Add your command handler code here
}

void CGLDocTabControl::OnUpdateTabControlMoveToNextTabGroup(CCmdUI *pCmdUI)
{
}

void CGLDocTabControl::OnTabControlMoveToNextTabGroup()
{
   // TODO: Add your command handler code here
}

void CGLDocTabControl::OnUpdateTabControlLayoutNew(CCmdUI *pCmdUI)
{
   if (pCmdUI->m_pSubMenu)
   {
      String strFileName = (String)"Layouts" + NATIVESEPARATOR + "*.snipelayout";
      strFileName.AppendToAppFolder();
      StringList *layoutlist = strFileName.FindFiles();
      if (layoutlist)
      {
         int i=0;
         for (POSITION pos=layoutlist->GetHeadPosition(); pos; i++)
         {
            String &strLayout = layoutlist->GetNext( pos );
            pCmdUI->m_pSubMenu->InsertMenu( i, MF_BYPOSITION, ID_TABCONTROL_LAYOUT_FIRST+i, strLayout.GetNameOnlyWithoutSuffix().Get() );
         }

         if (i > 0)
            pCmdUI->m_pSubMenu->InsertMenu( i, MF_BYPOSITION|MF_SEPARATOR );

         delete layoutlist;
      }
   }
}

void CGLDocTabControl::OnTabControlLayoutNew()
{
   String strLayoutName;
   if (!StringRequest( "New Layout", "Name", strLayoutName ))
      return;

   String strFileName = (String)"Layouts" + NATIVESEPARATOR + strLayoutName + ".snipelayout";
   strFileName.AppendToAppFolder();

   if (FileExists( strFileName ))
   {
      String strMessage("%1 Layout already exists.\nWould you like to replace it?");
      strMessage.Replace( "%1", strLayoutName );
      if (AfxMessageBox( strMessage.Get(), MB_YESNO ) == IDNO)
         return;
   }

   CWaitCursor waitcursor;
   FileStream fs;

   try 
   {
      fs.Open( strFileName );
   }
   catch (const String &strReason) 
   {
      AfxMessageBox( strReason.Get(), MB_OK|MB_ICONEXCLAMATION );
      return;
   }

   fs << DataHeader( "FileType", "Layout" ) << nl;
   fs << DataHeader( "Version", SNIPEVERSION ) << nl;
   fs << nl;

   try 
   {
      CGLControl *pBaseDocFrame = GetParent();
      CGLControl *pControl = pBaseDocFrame->m_childcontrolarray[1];
      pControl->Save( fs, TRUE );
   }
   catch (const String &strReason) 
   {
      AfxMessageBox( strReason.Get(), MB_OK|MB_ICONEXCLAMATION );
      return;
   }
   catch (...)
   {
      String strReason("Unknown error occurred while saving %1.");
      strReason.Replace("%1", strFileName);
      AfxMessageBox( strReason.Get(), MB_OK|MB_ICONEXCLAMATION );
      return;
   }
}

void CGLDocTabControl::OnLoadLayout( UINT nID )
{
   String strFileName = String::GetAppFolder();
   strFileName.AppendName( "Layouts" );
   strFileName.AppendName( "*.snipelayout" );

   StringList *layoutlist = strFileName.FindFiles();
   if (layoutlist)
   {
      int i = ID_TABCONTROL_LAYOUT_FIRST;
      for (POSITION pos=layoutlist->GetHeadPosition(); pos; i++)
      {
         String &strLayout = layoutlist->GetNext( pos );
         if (i == nID)
         {
            FileStream fs;
            fs.Open( strLayout, FileStream::BS_INPUT|FileStream::BS_ASCII );
            CGLControl *pBaseDocFrame = GetParent();
            if (pBaseDocFrame->Load( fs ))
            {
               delete pBaseDocFrame->m_childcontrolarray[1];
               pBaseDocFrame->Move( pBaseDocFrame->m_ptControl, pBaseDocFrame->m_szControl );
               GetCurrentDocument()->m_pControlHead = pBaseDocFrame->m_childcontrolarray[1];
            }
            break;
         }
      }

      delete layoutlist;
   }

   GetApp().DrawInvalidControls();
}
