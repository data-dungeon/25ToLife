//

#include "stdafx.h"
#include "GLCustomizeDlg.h"
#include "MainFrm.h"
#include "GLStringDlg.h"
#include "../Utility/HotKey.h"
#include "Resource.h"

GLImageList CGLCustomizeDlg::m_glImageListTabBorder;
GLImageList CGLCustomizeDlg::m_glImageListTabs;

BEGIN_MESSAGE_MAP(CGLCustomizeDlg, CGLDialogWnd)
   ON_COMMAND(ID_CLOSE, OnClose)
   ON_COMMAND_RANGE(0,65535, OnCommand)
END_MESSAGE_MAP()

CGLCustomizeDlg::CGLCustomizeDlg() :
 m_glTabControl( TRUE, TRUE, FALSE )
{
   m_glTabControl.m_bAutoDelete = FALSE;
   m_glToolBarsPane.m_bAutoDelete = FALSE;
   m_glCommandsPane.m_bAutoDelete = FALSE;
   m_glCloseBtn.m_bAutoDelete = FALSE;
}

CGLCustomizeDlg::~CGLCustomizeDlg()
{
}

BOOL CGLCustomizeDlg::Create()
{
   CRect rcDlg( CPoint(0,0), CSize( 522, 495 ));
   CRect rcMainWnd;
   AfxGetMainWnd()->GetWindowRect( &rcMainWnd );
   rcDlg.OffsetRect( ((rcMainWnd.left + rcMainWnd.right) - rcDlg.Width()) / 2,
                     ((rcMainWnd.top + rcMainWnd.bottom) - rcDlg.Height()) / 2 );

   if (!CGLDialogWnd::Create( AfxGetMainWnd(), "Customize", rcDlg ))
      return FALSE;

   CSize szContent = GetContentControl()->GetControlSize();

   m_glImageListTabBorder.Create( IDB_TABBORDER2, CRect( 10, 30, 20, 32 ) );
   m_glImageListTabs.Create( IDB_TOOLBARBUTTON, CRect( 12, 0, 16, 24 ), 28 );

   m_glTabControl.m_pglImageListBorder = &m_glImageListTabBorder;
   m_glTabControl.m_pglImageListTabs = &m_glImageListTabs;
   m_glTabControl.m_nTabIndent = 0;
   m_glTabControl.m_nTabSpacing = -1;
   m_glTabControl.m_nTabTextMargin = 4;
   m_glTabControl.m_bCenter = TRUE;
   InsertControl( &m_glTabControl, CRect(20,20, szContent.cx-20, szContent.cy-20-22-20) );

   m_glTabControl.AddTab( "Toolbars", (LPARAM)&m_glToolBarsPane );
   m_glTabControl.AddTab( "Commands", (LPARAM)&m_glCommandsPane );

   InsertControl( &m_glCloseBtn, CRect( CPoint(szContent.cx-20-70, szContent.cy-20-22 ), CSize( 70, 22 )), ID_CLOSE );

   ShowWindow( SW_SHOW );

   GetMainFrame()->GetMainRenderWnd()->OnIdleUpdateCmdUI();

   return TRUE;
}

void CGLCustomizeDlg::OnClose()
{
   EndModalLoop( ID_CLOSE );
}

void CGLCustomizeDlg::OnCommand( UINT nID )
{
   m_glTabControl.SelectTab( COMMANDS_PANE );
   GetApp().DrawInvalidControls();
   m_glCommandsPane.SelectCommand( nID );
   GetApp().DrawInvalidControls();
}

////////////////////////////////////////////////////////
// CGLToolBarsPane

void CGLToolBarsPane::PostInit()
{
   CGLTransparentControl::PostInit();

   m_szControl = CreateControlsFromDialogTemplate( IDD_TOOLBARS );

   m_pglToolBarList = (CGLListControl *)FindChild( IDC_TOOLBAR_LIST );

   for (int i=0; i<GetMainFrame()->m_glMainRenderWnd.m_glToolBarContainer.GetToolBarCount(); i++)
   {
      CGLBasicListItem::Info info( GetMainFrame()->m_glMainRenderWnd.m_glToolBarContainer.GetToolBarAt( i )->GetName() );
      m_pglToolBarList->InsertItem( (LPARAM)&info );
   }
}

BOOL CGLToolBarsPane::OnControlNotify( ControlNotificationMsg msg, WPARAM wParam, LPARAM lParam )
{
   switch (msg) {
      case NOTIFYCTRL_BN_CLICKED:
      {
         CGLToolBarShelf *pShelf = &GetMainFrame()->m_glMainRenderWnd.m_glToolBarContainer;
         UINT nID = (UINT)wParam;
         switch (nID) {
            case IDC_NEW_BUTTON:
            {
               int i = 1;
               String strName;
               while (TRUE)
               {
                  strName = (String)"Custom " + i++;
                  if (!pShelf->FindToolBar( strName ))
                     break;
               }

               if (!StringRequest( "Rename Toolbar", "Toolbar name:", strName ))
                  break;

               pShelf->AddToolBar( strName );
               CGLBasicListItem::Info info( strName );
               m_pglToolBarList->InsertItem( (LPARAM)&info );
               break;
            }
            case IDC_RENAME_BUTTON:
            {
               String strName;
               if (!StringRequest( "Rename Toolbar", "Toolbar name:", strName ))
                  break;
               break;
            }
            case IDC_DELETE_BUTTON:
               AfxMessageBox( "Delete Toolbar" );
               break;
            case IDC_RESET_BUTTON:
               AfxMessageBox( "Reset Toolbar" );
               break;
         }
         return TRUE;
      }
   }
   return CGLTransparentControl::OnControlNotify( msg, wParam, lParam );
}

////////////////////////////////////////////////////////
// CGLCommandsPane

CGLCommandsPane::CGLCommandsPane()
{
   m_glCommandList.m_bAutoDelete = FALSE;
}

CGLCommandsPane::~CGLCommandsPane()
{
}

void CGLCommandsPane::PostInit()
{
   CGLTransparentControl::PostInit();

   m_szControl = CreateControlsFromDialogTemplate( IDD_COMMANDS );

   m_pglCategoryList = (CGLListControl *)FindChild( IDC_GROUP_LIST );
   m_pglShortcutsForCommandList = (CGLListControl *)FindChild( IDC_SHORTCUTSFORCOMMAND_LIST );
   CGLListControl *pglCommandList = (CGLListControl *)FindChild( IDC_COMMAND_LIST );
   m_glCommandList.Init( this, NULL, pglCommandList->m_ptControl, pglCommandList->m_szControl, IDC_COMMAND_LIST );
   delete pglCommandList;

   FillCategoriesList();
}

void CGLCommandsPane::FillCategoriesList()
{
   CMap< String, const String &, UINT, UINT &> categorymap;

   for (POSITION pos = GetMainInterface()->GetCommandUIManager()->m_commanduimap.GetStartPosition(); pos; ) {
      UINT nID;
      CommandUI *pCommandUI;
      GetMainInterface()->GetCommandUIManager()->m_commanduimap.GetNextAssoc(pos, nID, pCommandUI);

      categorymap.SetAt( pCommandUI->m_strCategory, nID );
   }

   CGLBasicListItem::Info info( (String)"All (" + categorymap.GetCount() + " Categories)" );
   CGLListItem *pAllItem = m_pglCategoryList->InsertItem( (LPARAM)&info );
   for (POSITION pos = categorymap.GetStartPosition(); pos; ) {
      String strCategory;
      UINT nID;
      categorymap.GetNextAssoc(pos, strCategory, nID);

      CGLBasicListItem::Info info( strCategory );
      m_pglCategoryList->InsertItem( (LPARAM)&info );
   }

   m_pglCategoryList->Select( pAllItem );
}

void CGLCommandsPane::FillCommandsList()
{
   m_glCommandList.DeleteAllItems();

   CGLBasicListItem *pItem = (CGLBasicListItem *)m_pglCategoryList->m_pCaretItem;
   if (!pItem)
      return;

   BOOL bAll = pItem == m_pglCategoryList->m_childcontrolarray[0];
   String strCategory = pItem->GetInfo()->m_strName;
   for (POSITION pos = GetMainInterface()->GetCommandUIManager()->m_commanduimap.GetStartPosition(); pos; ) {
      UINT nID;
      CommandUI *pCommandUI;
      GetMainInterface()->GetCommandUIManager()->m_commanduimap.GetNextAssoc(pos, nID, pCommandUI);

      if (bAll || pCommandUI->m_strCategory == strCategory)
      {
         CGLCommandListItem::Info info( pCommandUI );
         CGLListItem *pInsertAfter = GLLI_FIRST;
         for (int i=0; i<m_glCommandList.m_childcontrolarray.GetCount(); i++)
         {
            CGLCommandListItem *pListItem = (CGLCommandListItem *)m_glCommandList.m_childcontrolarray[i];
            if (pCommandUI->m_strName > pListItem->GetInfo()->m_pCommandUI->m_strName)
               pInsertAfter = pListItem;
            else
               break;
         }
         CGLListItem *pItem = m_glCommandList.InsertItem( (LPARAM)&info, pInsertAfter );
      }
   }
}

void CGLCommandsPane::FillShortcutsList()
{
   m_pglShortcutsForCommandList->DeleteAllItems();

   CommandUI *pCommandUI = m_glCommandList.m_pCurrentCommandUI;
   if (!pCommandUI)
      return;

   for (POSITION accelpos = pCommandUI->m_actualaccellist.GetHeadPosition(); accelpos; )
   {
      CommandUIAccel &commanduiaccel = pCommandUI->m_actualaccellist.GetNext(accelpos);
      String strItemDesc = ::GetHotKeyDescription( commanduiaccel.m_key, commanduiaccel.m_virt );

      CGLBasicListItem::Info info( strItemDesc );
      m_pglShortcutsForCommandList->InsertItem( (LPARAM)&info );
   }
}

void CGLCommandsPane::SelectCommand( UINT nID )
{
//   if (!m_glCommandList.Select( nID )) // couldn't find it in currently chosen category
//   {
      // Attempt to swith the category to the one for the given ID
      CommandUI *pCommandUI;
      if (GetMainInterface()->GetCommandUIManager()->m_commanduimap.Lookup( nID, pCommandUI ))
      {
         for (int i=0; i<m_pglCategoryList->m_childcontrolarray.GetCount(); i++)
         {
            CGLBasicListItem *pItem = (CGLBasicListItem *)m_pglCategoryList->m_childcontrolarray[i];
            if (pItem->GetInfo()->m_strName == pCommandUI->m_strCategory)
            {
               m_pglCategoryList->Select( pItem );
               break;
            }
         }
      }

      m_glCommandList.Select( nID );
//   }
}

BOOL CGLCommandsPane::OnControlNotify( ControlNotificationMsg msg, WPARAM wParam, LPARAM lParam )
{
   switch (msg) {
      case NOTIFYCTRL_LC_CARETCHANGED:
      {
         UINT nID = (UINT)wParam;
         switch (nID) {
            case IDC_GROUP_LIST:
            {
               FillCommandsList();
               return TRUE;
            }
         }
         break;
      }
   }
   return CGLTransparentControl::OnControlNotify( msg, wParam, lParam );
}

////////////////////////////////////////////////////////
// CGLCommandListControl

CGLCommandListControl::CGLCommandListControl()
{
   m_nID = IDC_COMMAND_LIST;
   m_pCurrentCommandUI = NULL;
}

CGLCommandListControl::~CGLCommandListControl()
{
}

CGLListItem *CGLCommandListControl::AllocItem( CGLListControl *pListControl, LPARAM lParam )
{
   return SNEW CGLCommandListItem( (CGLCommandListItem::Info *)lParam );
}

BOOL CGLCommandListControl::Select( UINT nID )
{
   for (int i=0; i<m_childcontrolarray.GetCount(); i++)
   {
      CGLCommandListItem *pItem = (CGLCommandListItem *)m_childcontrolarray[i];
      if (pItem->GetInfo()->m_pCommandUI->m_nCmdID == nID)
         return Select( pItem );
   }
   Select( NULL, GLTC_CARET );
   return FALSE;
}

BOOL CGLCommandListControl::Select( CGLListItem *pItem, UINT nCode/*=GLTC_CARET*/ )
{
   CGLCommandListItem *pCommandListItem = (CGLCommandListItem *)pItem;
   if (pCommandListItem)
      m_pCurrentCommandUI = pCommandListItem->GetInfo()->m_pCommandUI;
   else
      m_pCurrentCommandUI = NULL;
   Invalidate();

   ((CGLCommandsPane *)m_pParentControl)->FillShortcutsList();

   return CGLListControl::Select( pItem, nCode );
}

////////////////////////////////////////////////////////
// CGLCommandListItem

CGLCommandListItem::CGLCommandListItem( const Info *pInfo ) :
 m_info( *pInfo )
{
   m_nIndex = -1;
}

void CGLCommandListItem::SetIndex( UINT nIndex )
{
   m_nIndex = nIndex;
}

void CGLCommandListItem::Draw( UINT nDrawFlags/*=DRAW_CHILDREN|DRAW_PARENT*/ )
{
   GLDrawPrepper prep(this);

   if (nDrawFlags & DRAW_PARENT)
      m_pParentControl->Draw( DRAW_PARENT ); // this control has transparency, so have our parent draw first if it hasn't already

   if (m_nIndex % 2)
   {
      glColor4f( 0.0f, 0.0f, 0.0f, 0.06f );
      glBegin( GL_QUADS );
         glVertex2f( 0.0f, 0.0f );
         glVertex2f( m_szControl.cx, 0.0f );
         glVertex2f( m_szControl.cx, m_szControl.cy );
         glVertex2f( 0.0f, m_szControl.cy );
      glEnd();
   }

   CommandUI *pCommandUI = ((CGLCommandListControl *)GetListControl())->m_pCurrentCommandUI;
   if (pCommandUI == GetInfo()->m_pCommandUI)
   {
      RGBAFloat rgba( 1.0f, 1.0f, 0.5f, 0.8f );
      RGBAFloat c[4] = { rgba, rgba, rgba*0.9f, rgba*0.9f };
      DrawGradient( c, CRect( 0, 0, m_szControl.cx, GetItemHeight() ));
   }

   glColor4f( 0.0f, 0.0f, 0.0f, 0.15f );
   glBegin( GL_LINES );
      glVertex2f( 0, m_szControl.cy );
      glVertex2f( m_szControl.cx, m_szControl.cy );
   glEnd();

   float x = MARGIN;

      GLImageList *pImageList = GetInfo()->m_pCommandUI->GetImageList();

   if (!pImageList->IsEmpty())
   {
      x += 12;
      x -= pImageList->GetWidthEach() / 2;
      pImageList->Draw( x, 0.0f, 0 );
   }

   x = MARGIN + 26;

   if (!GetInfo()->m_pCommandUI->m_strName.IsEmpty())
   {
      glColor3f( 0.0f, 0.0f, 0.0f );
      DrawText( x, 12, GetInfo()->m_pCommandUI->m_strName );
   }
   else
   {
      glColor3f( 1.0f, 0.0f, 0.0f );
      DrawText( x, 12, String((int)GetInfo()->m_pCommandUI->m_nCmdID) );
   }

   CGLControl::Draw( nDrawFlags ); // Skip CGLListItem
}
