//
#include "stdafx.h"
#include "BaseDoc.h"
#include "HierObject.h"
#include "GLBaseFrame.h"
#include "MainFrm.h"
#include "Interface/SnipeNavigator.h"
#include "CommandPlugin.h"
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////////////////////
int SnipeObjectTypeArray::Add( SnipeObjectType nSOT )
{
   SnipeObjectInfo *pInfo = SnipeObjectInfo::GetBySOT( nSOT );
   ASSERT( pInfo );
   return m_infoarray.Add( pInfo );
}

SnipeObjectType SnipeObjectTypeArray::GetCurrentSOT() const
{
   SnipeObjectInfo *pInfo = GetInfoAt(GetCurrentIndex());
   if (pInfo==NULL)
      return SOT_Unknown;
   return pInfo->m_nSOT;
}

SnipeObjectType SnipeObjectTypeArray::GetSnipeObjectTypeAt( UINT nIndex ) const
{
   SnipeObjectInfo *pInfo = GetInfoAt(nIndex);
   if (pInfo==NULL)
      return SOT_Unknown;
   return pInfo->m_nSOT;
}

BOOL SnipeObjectTypeArray::SetCurrentSOT( SnipeObjectType nNewSOT )
{
   for (UINT i=0; i<GetNumInfos(); i++)
   {
      SnipeObjectType nSOT = GetSnipeObjectTypeAt( i );
      if (nSOT == nNewSOT)
      {
         SetCurrentIndex( i );
         return TRUE;
      }
   }
   return FALSE;
}

void SnipeObjectTypeArray::SetCurrentIndex( UINT nIndex )
{
   m_nCurrentIndex = nIndex;
   SnipeObjectInfo *pInfo = GetInfoAt( GetCurrentIndex() );
   SnipeObjectType nSOT = SOT_Unknown;
   if (pInfo)
      nSOT = pInfo->m_nSOT;
   RebuildAvailableManipulatorList( nSOT );
   SetManipulator( NULL );
}

void SnipeObjectTypeArray::RebuildAvailableManipulatorList( SnipeObjectType sot )
{
   m_AvailableManipulatorList.RemoveAll();

   MainInterface *pMI = GetMainInterface();
   if (pMI)
   {
      SnipeManipulatorManager *pSMM = pMI->GetSnipeManipulatorManager();
      SnipeManipulatorList *pList = &pSMM->m_alllist;
      for (POSITION pos=pList->GetHeadPosition(); pos; )
      {
         SnipeManipulator *pManipulator = pList->GetNext( pos );
         if (pManipulator->SupportSOT( sot))
            m_AvailableManipulatorList.AddTail( pManipulator );
      }
   }

   m_AvailableManipulatorList.m_bDirty = TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CBaseDoc, CDocument)

BEGIN_MESSAGE_MAP(CBaseDoc, CDocument)
   ON_UPDATE_COMMAND_UI_RANGE( ID_PLUGIN_COMMAND_FIRST, ID_PLUGIN_COMMAND_LAST, OnUpdatePluginCommand )
   ON_COMMAND_EX_RANGE( ID_PLUGIN_COMMAND_FIRST, ID_PLUGIN_COMMAND_LAST, OnPluginCommand )
   ON_UPDATE_COMMAND_UI(ID_OUTPUTCONTROL, OnUpdateOutputControl)
   ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
END_MESSAGE_MAP()

CBaseDoc::CBaseDoc()
{
   m_basedocflags = 0;
   m_pControlHead = NULL;
   m_pObjectHead = NULL;

   GetApp().GetDocList().AddTail( this );
}

CBaseDoc::~CBaseDoc()
{
   if (m_pObjectHead)
      m_pObjectHead->m_pDocument = NULL;

   POSITION pos = GetApp().GetDocList().Find( this );
   if (pos)
      GetApp().GetDocList().RemoveAt( pos );
}

CommandUIPluginList *CBaseDoc::GetCommandUIPluginList(SnipeDocType nType)
{
   return GetMainInterface()->GetCommandUIPluginManager()->GetCommandUIPluginListDoc( nType );
}

void CBaseDoc::SetObjectHead( HierObject *pObjectHead )
{
   m_pObjectHead = pObjectHead;
   InvalidateViews( pObjectHead );
}

HierObject *CBaseDoc::GetCommandObject() const
{
   ASSERT( m_pObjectHead->IsCommandObject());
   return m_pObjectHead;
}

SelectionList *CBaseDoc::GetSelectionList() const
{
   if (GetCommandObject())
      return GetCommandObject()->GetSelectionList();
   else
      return NULL;
}

void CBaseDoc::AddView( CGLBaseView *pView )
{
   m_viewlist.AddTail( pView );
}

void CBaseDoc::RemoveView( CGLBaseView *pView )
{
   POSITION pos = m_viewlist.Find(pView);
   if (pos)
      m_viewlist.RemoveAt(pos);
}

void CBaseDoc::OnCloseDocument()
	// must close all views now (no prompting) - usually destroys this
{
   POSITION pos = GetApp().GetDocList().Find( this );
   if (pos)
      GetApp().GetDocList().RemoveAt( pos );

   for (POSITION pos=GetMainFrame()->GetMainRenderWnd()->m_glDocumentBar.m_frameList.GetHeadPosition(); pos; ) {
      CGLBaseDocFrame *pBaseDocFrame = GetMainFrame()->GetMainRenderWnd()->m_glDocumentBar.m_frameList.GetNext( pos );
      int nIndex = pBaseDocFrame->m_glTabControl.FindTab( (LPARAM)this );
      if (nIndex != -1) {
         pBaseDocFrame->m_glTabControl.SetItemData( nIndex, NULL );
         pBaseDocFrame->m_glTabControl.RemoveTab( nIndex );
      }
   }

   if (m_pControlHead) {
      delete m_pControlHead;
      m_pControlHead = NULL;
   }

   // destroy all frames viewing this document
	// the last destroy may destroy us
	BOOL bAutoDelete = m_bAutoDelete;
	m_bAutoDelete = FALSE;  // don't destroy document while closing views

	while (!m_viewlist.IsEmpty())
	{
		// get frame attached to the view
		CGLBaseView *pView = m_viewlist.GetHead();
		ASSERT_VALID(pView);
		CGLBaseFrame *pFrame = pView->GetParentFrame();

		delete pFrame;
	}
	m_bAutoDelete = bAutoDelete;

	// clean up contents of document before destroying the document itself
	DeleteContents();

	// delete the document if necessary
	if (m_bAutoDelete)
		delete this;

   GetMainFrame()->GetMainRenderWnd()->SendMessage( WM_IDLEUPDATECMDUI, (WPARAM)TRUE );
   GetMainFrame()->Invalidate( FALSE );
}

BOOL CBaseDoc::UsesObject( SnipeObject *pObject )
{
   if (pObject == GetCommandObject())
      return TRUE;
   else if (pObject->GetParent())
      return UsesObject( pObject->GetParent() );
   else
      return FALSE;
}

void CBaseDoc::InvalidateViews( SnipeObject *pObject )
{
   BOOL bUsesObject = (pObject==NULL) || UsesObject( pObject );

   for (POSITION pos=m_viewlist.GetHeadPosition(); pos; )
   {
      CGLBaseView *pView = m_viewlist.GetNext( pos );
      SnipeManipulatorInstance *pManipulatorInstance = pView->GetManipulatorInstance();
      if (bUsesObject || pManipulatorInstance==pObject || pView->GetNavigatorInstance()==pObject)
         pView->Invalidate();
   }
}

void CBaseDoc::OnUpdatePluginCommand( CCmdUI *pCmdUI )
{
   CommandUIPlugin *pPlugin = GetCommandUIPlugin(pCmdUI->m_nID);
   if (pPlugin==NULL) {
      pCmdUI->ContinueRouting();
      return;
   }

   try {
      pPlugin->GetOnUpdateCommandDocCallback()(pPlugin->GetPluginData(), this, pCmdUI);
   }
   catch(const String &strReason)
   {
      g_outputWindow << pPlugin->FormatError("OnUpdateCommandUI", strReason) << "\n";
   }
   catch(...) {
      g_outputWindow << pPlugin->FormatError("OnUpdateCommandUI", "Unknown") << "\n";
   }
}

BOOL CBaseDoc::OnPluginCommand( UINT nID )
{
   CommandUIPlugin *pPlugin = GetCommandUIPlugin(nID);

   if (pPlugin==NULL)
      return FALSE; // Not for us, continue routing

   CommandPlugin *pCommand = NULL;
   String strParameters;

   try {
      pPlugin->GetOnCommandDocCallback()(pPlugin->GetPluginData(), nID, this, pCommand, strParameters);
   }
   catch(const String &strReason)
   {
      g_outputWindow << pPlugin->FormatError("OnCommand", strReason) << "\n";
      return TRUE;
   }
   catch(...) {
      g_outputWindow << pPlugin->FormatError("OnCommand", "Unknown") << "\n";
      return TRUE;
   }

   if (pCommand==NULL)
   {
      GetApp().DrawInvalidControls();
      return TRUE;
   }

   DoCommand( pCommand, strParameters );

   return TRUE; // we handled the message (regardless of DoCommand failure)
}

void CBaseDoc::ZoomFitAll()
{
   for (POSITION pos=m_viewlist.GetHeadPosition(); pos; )
   {
      CGLBaseView *pView = m_viewlist.GetNext( pos );
      pView->ZoomFitAll();
   }
}

void CBaseDoc::OnUpdateFileSave(CCmdUI* pCmdUI)
{
   pCmdUI->Enable( FALSE );
}

void CBaseDoc::OnUpdateOutputControl( CCmdUI *pCmdUI )
{
   CGLOutputControlCmdUI *pOutputControlCmdUI = (CGLOutputControlCmdUI *)pCmdUI;
   pOutputControlCmdUI->SetCurrentWorkingObject( GetCommandObject() );
   pOutputControlCmdUI->SetCurrentCommandObject( GetCommandObject() );
}

BOOL CBaseDoc::DoCommand( CommandPlugin *pCommand, const String &strParameters, BOOL bTempCommand/*=FALSE*/ )
{
   ASSERT(pCommand);
   CommandList *pCommandList = GetCommandObject()->GetCommandList();
   if (pCommandList==NULL)
   {
      String strMsg("%DOCNAME of type %DOCTYPE cannot execute commands.");
      strMsg.Replace("%DOCNAME", (LPCTSTR)GetTitle()); 
      strMsg.Replace("%DOCTYPE", GetClassName()); 
      g_outputWindow << strMsg << "\n";
      return FALSE;
   }

   StringList strResults;
   return pCommandList->Do(pCommand, strParameters, strResults, bTempCommand);
}
