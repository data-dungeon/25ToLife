// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "Snipe.h"
#include "MainFrm.h"
#include "Project.h"
#include "PolyModel.h"
#include "Gui/OutputView.h"
#include "BitmapObject.h"
#include "ModelDoc.h"
#include "ModelTranslatorPluginManager.h"
#include "SceneTranslatorPluginManager.h"
#include "MotionTranslatorPluginManager.h"
#include "Interface/SnipeNavigator.h"
#include "../Utility/FileDialog.h"
#include "../Utility/FileStream.h"
#include "Interface/SelectionList.hpp"
#include "resource.h"
#include ".\mainfrm.h"
#include "MotionSet.h"
#include "MotionSetTranslatorPluginManager.h"

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

CGLSharedRenderWnd CMainFrame::m_glSharedRenderWnd;

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
   ON_WM_CLOSE()

   ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
   ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
   ON_COMMAND(ID_EDIT_REDO, OnEditRedo)
   ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateEditRedo)

   ON_COMMAND(ID_MIRROR_MODE, OnMirrorMode)

   ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
   ON_UPDATE_COMMAND_UI(ID_FILE_OPEN, OnUpdateFileOpen)

   ON_COMMAND(ID_FILE_NEW_PROJECT, OnFileNewProject)
   ON_COMMAND(ID_FILE_SAVEPROJECT, OnSaveProject)
   ON_UPDATE_COMMAND_UI(ID_FILE_SAVEPROJECT, OnUpdateSaveProject)
   ON_COMMAND(ID_FILE_SAVEPROJECTAS, OnFileSaveProjectAs)
   ON_UPDATE_COMMAND_UI(ID_FILE_SAVEPROJECTAS, OnUpdateFileSaveProjectAs)

   ON_COMMAND(ID_FILE_NEW_SCENE, OnFileNewScene)
   ON_COMMAND(ID_FILE_NEW_MODEL_POLYGONAL, OnFileNewModelPolygonal)
   ON_COMMAND(ID_FILE_NEW_MODEL_PATCH, OnFileNewModelPatch)
   ON_COMMAND(ID_FILE_NEW_MOTIONSET, OnFileNewMotionSet)

//   ON_COMMAND(ID_FILE_NEW, OnFileNew)

   ON_COMMAND(ID_IMPORT_MODEL, OnImportModel)
   ON_UPDATE_COMMAND_UI(ID_IMPORT_MODEL, OnUpdateImportModel)

   ON_COMMAND(ID_IMPORT_SCENE, OnImportScene)
   ON_UPDATE_COMMAND_UI(ID_IMPORT_SCENE, OnUpdateImportScene)

   ON_COMMAND(ID_IMPORT_MOTION, OnImportMotion)
   ON_UPDATE_COMMAND_UI(ID_IMPORT_MOTION, OnUpdateImportMotion)

   ON_COMMAND(ID_IMPORT_IMAGE, OnImportImage)
   ON_UPDATE_COMMAND_UI(ID_IMPORT_IMAGE, OnUpdateImportImage)

   ON_COMMAND(ID_IMPORT_MOTIONSET, OnImportMotionSet)
   ON_UPDATE_COMMAND_UI(ID_IMPORT_MOTIONSET, OnUpdateImportMotionSet)

   ON_UPDATE_COMMAND_UI_RANGE( ID_PLUGIN_COMMAND_FIRST, ID_PLUGIN_COMMAND_LAST, OnUpdatePluginCommand )
   ON_COMMAND_RANGE( ID_PLUGIN_COMMAND_FIRST, ID_PLUGIN_COMMAND_LAST, OnPluginCommand )
 
   ON_UPDATE_COMMAND_UI(IDR_HISTORYCONTROL, OnUpdateHistoryControl)
   ON_UPDATE_COMMAND_UI(IDR_COMPONENTTYPE_COMBO, OnUpdateComponentCombo)
   ON_UPDATE_COMMAND_UI(IDR_MANIPULATOR_COMBO, OnUpdateManipulatorCombo)
   ON_UPDATE_COMMAND_UI(ID_OUTPUTCONTROL, OnUpdateOutputControl)
   ON_UPDATE_COMMAND_UI(ID_MIRROR_MODE, OnUpdateMirrorMode)
   ON_COMMAND(ID_COMMANDLINE, OnGotoCommandLine)
   ON_UPDATE_COMMAND_UI(ID_COMMANDLINE, OnUpdateGotoCommandLine)

   ON_WM_SIZE()
END_MESSAGE_MAP()

static UINT indicators[] =
{	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame *g_pMainFrame;

MainInterface *GetMainInterface()
{
   if (g_pMainFrame)
      return g_pMainFrame->GetMainInterface();
   else
      return NULL;
}

CMainFrame::CMainFrame() :
 m_bFullScreen(FALSE)
{
   m_mainframeflags = 0;
  // window placement persistence
   ::memset( &m_wpInitial, 0, sizeof(WINDOWPLACEMENT) );
   m_wpInitial.length = sizeof(WINDOWPLACEMENT);
   m_wpInitial.showCmd = SW_HIDE;

   g_pMainFrame = this;
   m_pCustomizeDlg = NULL;

   m_pMainInterface = SNEW MainInterface;
}

CMainFrame::~CMainFrame()
{
   m_pMainInterface->SetCurrentNavigator( NULL );
   if (m_pMainInterface)
      delete m_pMainInterface;
   g_pMainFrame = NULL;
}

CMainFrame *GetMainFrame()
{
   return g_pMainFrame;
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
   if (!m_glSharedRenderWnd.Create( this ))
      return -1;

   if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

   // create a view to occupy the client area of the frame
	if (!GetMainRenderWnd()->Create( this ))
	{
		TRACE0("Failed to create view window\n");
		return -1;
	}

 //  if (!m_glStatusBarWnd.Create( this ))
	//{
	//	TRACE0("Failed to create view window\n");
	//	return -1;
	//}

   DragAcceptFiles();

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
   HINSTANCE hInst = AfxFindResourceHandle( MAKEINTRESOURCE(IDR_MAINFRAME), RT_GROUP_ICON );
   HICON hIcon = ::LoadIcon(hInst, MAKEINTRESOURCE(IDR_MAINFRAME));
   HCURSOR hCursor = GetApp().LoadStandardCursor( IDC_ARROW );
	cs.lpszClass = AfxRegisterWndClass( 0, hCursor, 0, hIcon );

	return TRUE;
}

void CMainFrame::OnClose()
{
   if (m_pCustomizeDlg)
      return;

   Project *pProject = m_pMainInterface->GetProject();
   if (pProject)
   {
      pProject->OnEditDelete();
      if (m_pMainInterface->GetProject())
         return;
      m_pMainInterface->SetProject(NULL);
   }

   WINDOWPLACEMENT wp;
   wp.length = sizeof wp;
   if (GetWindowPlacement(&wp)) {
      wp.flags = 0;
      if (IsZoomed())
         wp.flags |= WPF_RESTORETOMAXIMIZED;
      WriteWindowPlacement(&wp);
   }

   CFrameWnd::OnClose();
}

HACCEL CMainFrame::GetDefaultAccelerator()
{
	HACCEL hAccelTable = m_hAccelTable;

   if (GetMainInterface()->GetCommandUIManager()->m_hAccel)
      return GetMainInterface()->GetCommandUIManager()->m_hAccel;

	return hAccelTable;
   
}

BOOL CMainFrame::OnEraseBkgnd(CDC* pDC)
{
	// Fool Windows into thinking that we cleared to remove flicker

	return TRUE;
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame message handlers

void CMainFrame::OnSetFocus(CWnd* /*pOldWnd*/)
{
	// forward focus to the view window
	GetMainRenderWnd()->SetFocus();
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
   if (m_pCustomizeDlg)
      return ((CWnd *)m_pCustomizeDlg)->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);

	// let the view have first crack at the command
	if (GetMainRenderWnd()->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// otherwise, do default handling
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CMainFrame::OnEditUndo()
{
   CommandList *pCommandList = m_pMainInterface->GetCurrentCommandList();
   pCommandList->Undo();
   GetApp().DrawInvalidControls();
}

void CMainFrame::OnUpdateEditUndo(CCmdUI *pCmdUI)
{
   CommandList *pCommandList = m_pMainInterface->GetCurrentCommandList();
   pCmdUI->Enable(pCommandList && pCommandList->HasUndo());
}

void CMainFrame::OnEditRedo()
{
   CommandList *pCommandList = m_pMainInterface->GetCurrentCommandList();
   pCommandList->Redo();
   GetApp().DrawInvalidControls();
}

void CMainFrame::OnUpdateEditRedo(CCmdUI *pCmdUI)
{
   CommandList *pCommandList = m_pMainInterface->GetCurrentCommandList();
   pCmdUI->Enable(pCommandList && pCommandList->HasRedo());
}

void CMainFrame::OnUpdateSaveProject(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable( m_pMainInterface->GetProject()!=NULL );
}

void CMainFrame::OnSaveProject()
{
   Project *pProject = m_pMainInterface->GetProject();
   pProject->SaveFile();
}

void CMainFrame::OnUpdateFileSaveProjectAs(CCmdUI *pCmdUI)
{
   pCmdUI->Enable( m_pMainInterface->GetProject()!=NULL );
}

void CMainFrame::OnFileSaveProjectAs()
{
   Project *pProject = m_pMainInterface->GetProject();
   pProject->SaveFile( TRUE );
}

void CMainFrame::OnUpdateFileOpen(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable( TRUE );
}

static BOOL AddFilter( UINT nID, String &strFilters, String &strCombined )
{
   String strCurrent;
   strCurrent = String::GetResourceString(nID);

   UINT nStart = strCurrent.Find('|');
   if (nStart == -1) 
   {
      ASSERT(FALSE);
      return FALSE;
   }

   UINT nEnd = strCurrent.Find(++nStart, '|');
   if (nEnd == -1) 
   {
      ASSERT(FALSE);
      return FALSE;
   }

   if (!strCombined.IsEmpty())
      strCombined+=";";

   strCombined+=strCurrent.Mid(nStart, nEnd-nStart);
   strFilters+=strCurrent;
   return TRUE;
}

void CMainFrame::OnFileOpen()
{
   String strFilters;
   String strCombinedFilters;
   AddFilter( IDS_PROJECTFILTER, strFilters, strCombinedFilters );
   AddFilter( IDS_SCENEFILTER, strFilters, strCombinedFilters );
   AddFilter( IDS_MODELFILTER, strFilters, strCombinedFilters );
   strFilters = String::GetResourceString( IDS_ALLFORMATS ) + '|' + strCombinedFilters + '|' + strFilters + '|';

   String strInitFileName = GetApp().GetProfileString( "Files", "Last Open", "BOGUS" );
   if (strInitFileName == "BOGUS")
      strInitFileName.Empty();

   String strFileName;
   if (!GetOpenFileName( strInitFileName, strFilters, strFileName ))
      return;

   ::AfxGetApp()->WriteProfileString( "Files", "Last Open", strFileName.Get() );

   GetApp().OpenDocumentFile( strFileName.Get() );
}

BOOL SnipeObject::Load( BaseStream &bs )
{
   static MemoryStream ms( 1024 );
   static SnipeObjectHeader snipeobjectheader;
   static PropertyHeader propertyheader;
   static DataHeader dataheader;

   while (bs.ReadLine( ms ))
   {
      if (SnipeObjectHeader::IsHeader(ms))
      {
         ms >> snipeobjectheader;

         if (!snipeobjectheader.m_bBeginHeader)
         {
            OnEndObjectLoad();
            return TRUE;
         }

         SnipeObjectInfo *pInfo = SnipeObjectInfo::GetByClassName( snipeobjectheader.m_strKey );
         if (!pInfo)
         {
            // snipeobjectheader.SkipToEnd( bs ); // TODO
            String strMsg("<E>Skipping unknown object: %1.\n");
            strMsg.Replace("%1", snipeobjectheader.m_strKey);
            g_outputWindow << strMsg;
            continue;
         }

         SnipeObject *pObject = CreateChild( pInfo );
         if (!pObject || snipeobjectheader.m_bEndHeader)
            continue;

         pObject->Load( bs );
      }
      else if (PropertyHeader::IsHeader(ms))
      {
         ms >> propertyheader;

         if (!propertyheader.m_bBeginHeader)
            return TRUE;

         Property *pProperty = FindProperty( propertyheader.m_strKey );
         if (!pProperty)
         {
            String strMsg("<W>Skipping unknown property: %1.\n");
            strMsg.Replace("%1", propertyheader.m_strKey);
            g_outputWindow << strMsg;

            // propertyheader.SkipToEnd( bs ); // TODO
            continue;
         }

         if (!propertyheader.m_strValue.IsEmpty())
         {
            MemoryStream valuestream;
            valuestream.Open( propertyheader.m_strValue.Get(), propertyheader.m_strValue.Length(), BaseStream::BS_ASCII|BaseStream::BS_OUTPUT);
            pProperty->ReadValue( valuestream );
            pProperty->OnValueStored( 0, FALSE );
         }

         if (propertyheader.m_bEndHeader)
            continue;

         pProperty->Load( bs );
      }
      else if (DataHeader::IsHeader(ms))
      {
         ms >> dataheader;

         if (!dataheader.m_bBeginHeader)
            continue;

         if (!ParseData( bs, dataheader ))
         {
            // dataheader.SkipToEnd( bs );
         }
      }
   }

   return TRUE;
}

void SnipeObject::OnEndProjectLoad()
{
   PropertyArray *pPropertyArray = GetPropertyArray();
   if (pPropertyArray)
   {
      for (int i=0; i<pPropertyArray->GetCount(); i++)
      {
         Property *pProperty = pPropertyArray->GetAt( i );
         pProperty->OnEndProjectLoad();
      }
   }
}

SnipeObject *SnipeObject::CreateChild( const SnipeObjectInfo *pInfo )
{
   return pInfo->CreateObject();
}

void CMainFrame::OnUpdateImportModel(CCmdUI* pCmdUI) 
{
   PluginList *pPluginList = GetMainInterface()->GetModelTranslatorPluginManager()->GetPluginList();

   BOOL bEnable = FALSE;
   for (POSITION pos = pPluginList->GetHeadPosition(); pos; )
   {
      ModelTranslatorPlugin *pPlugin = (ModelTranslatorPlugin *)pPluginList->GetNext( pos );
      if (pPlugin->GetImportCallback()) {
         bEnable = TRUE;
         break;
      }
   }
   pCmdUI->Enable(bEnable);
}

void CMainFrame::OnImportModel() 
{
   PolyModel *pModel = m_pMainInterface->GetModelTranslatorPluginManager()->Import();
   
   if ( pModel==NULL )
      return;

   pModel->OnObjectEdit();
}

void CMainFrame::OnUpdateImportScene(CCmdUI* pCmdUI) 
{
   PluginList *pPluginList = GetMainInterface()->GetSceneTranslatorPluginManager()->GetPluginList();

   BOOL bEnable = FALSE;
   for (POSITION pos = pPluginList->GetHeadPosition(); pos; )
   {
      SceneTranslatorPlugin *pPlugin = (SceneTranslatorPlugin *)pPluginList->GetNext( pos );
      if (pPlugin->GetImportCallback()) {
         bEnable = TRUE;
         break;
      }
   }
   pCmdUI->Enable(bEnable);

}

void CMainFrame::OnImportScene() 
{
   Scene *pScene = m_pMainInterface->GetSceneTranslatorPluginManager()->Import();

   if ( pScene==NULL )
      return;

   pScene->OnObjectEdit();
}

void CMainFrame::OnUpdateImportMotion(CCmdUI* pCmdUI) 
{
   PluginList *pPluginList = GetMainInterface()->GetMotionTranslatorPluginManager()->GetPluginList();

   BOOL bEnable = FALSE;
   for (POSITION pos = pPluginList->GetHeadPosition(); pos; )
   {
      SceneTranslatorPlugin *pPlugin = (SceneTranslatorPlugin *)pPluginList->GetNext( pos );
      if (pPlugin->GetImportCallback()) {
         bEnable = TRUE;
         break;
      }
   }
   pCmdUI->Enable(bEnable);
}

void CMainFrame::OnImportMotion() 
{
   StringList strResults;
   GetMainInterface()->DoCommand( "ImportMotion", "-e", strResults );
}

void CMainFrame::OnUpdateImportMotionSet(CCmdUI* pCmdUI) 
{
   PluginList *pPluginList = GetMainInterface()->GetMotionSetTranslatorPluginManager()->GetPluginList();

   BOOL bEnable = FALSE;
   for (POSITION pos = pPluginList->GetHeadPosition(); pos; )
   {
      SceneTranslatorPlugin *pPlugin = (SceneTranslatorPlugin *)pPluginList->GetNext( pos );
      if (pPlugin->GetImportCallback()) {
         bEnable = TRUE;
         break;
      }
   }
   pCmdUI->Enable(bEnable);
}

void CMainFrame::OnImportMotionSet() 
{
   MotionSet *pMotionSet = m_pMainInterface->GetMotionSetTranslatorPluginManager()->Import();

   if ( pMotionSet==NULL )
      return;

   pMotionSet->OnObjectEdit();
}

void CMainFrame::OnFileNewModelPolygonal()
{
   Project *pProject = m_pMainInterface->CreateProject();
   m_pMainInterface->SetCurrentCommandObject(pProject);

   String strParameters = "-edit -type POLY";
   StringList strResults;
   GetMainInterface()->DoCommand("CreateModel", strParameters, strResults);
}

void CMainFrame::OnFileNewModelPatch()
{
   Project *pProject = m_pMainInterface->CreateProject();
   m_pMainInterface->SetCurrentCommandObject(pProject);

   String strParameters = "-edit -type PATCH";
   StringList strResults;
   GetMainInterface()->DoCommand("CreateModel", strParameters, strResults);
}

void CMainFrame::OnFileNewScene()
{
   Project *pProject = m_pMainInterface->CreateProject();

   Scene *pScene = pProject->AddScene();

   Camera *pCameraCache = pProject->AddCamera();

   Camera *pCameraInstance = (Camera*)pScene->AddInstance(pCameraCache);
   pCameraInstance->GetTransform().m_vTranslate.StoreValue( Vector(0,20,80), 0 );
   pCameraInstance->GetTransform().m_vRotate.StoreValue( Vector(-10,0,0), 0 );

   Light *pLightCache = pProject->AddLight();
   Light *pLightInstance = (Light*)pScene->AddInstance(pLightCache);
   pLightInstance->GetTransform().m_vTranslate.StoreValue( Vector(40,30,60), 0 );
   pLightInstance->GetTransform().m_vRotate.StoreValue( Vector(-20,35,0), 0 );

   pScene->OnObjectEdit();
}

void CMainFrame::OnFileNewMotionSet()
{
   Project *pProject = m_pMainInterface->CreateProject();

   MotionSet *pMotionSet = pProject->AddMotionSet();

   pMotionSet->OnObjectEdit();
}

void CMainFrame::OnFileNew()
{
}

void CMainFrame::OnUpdateImportImage(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable(BitmapObject::GetFormatCount());
}

void CMainFrame::OnImportImage() 
{
   Image *pImage = Image::Create();

   if (!pImage->QueryLoadImage())
   {
      delete pImage;
      return;
   }

   m_pMainInterface->CreateProject()->m_imagecontainer.AddChildAtTail( pImage );
   pImage->OnObjectEdit();
}

void CMainFrame::OnUpdateMirrorMode(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable( TRUE );
   pCmdUI->SetCheck( SnipeObject::m_bMirrorMode );
}

void CMainFrame::OnMirrorMode()
{
   SnipeObject::SetMirrorMode(!SnipeObject::IsMirrorMode());
   if (SnipeObject::IsMirrorMode())
   {
      SelectionList *pList = m_pMainInterface->GetCurrentSelectionList();
      if (pList)
         for (POSITION pos = pList->GetHeadPosition(); pos; )
            pList->GetNext(pos)->GetMirrorObject();
   }
   if (m_pMainInterface->GetCurrentCommandObject())
   {
      GetApp().InvalidateAllViews(m_pMainInterface->GetCurrentCommandObject());
      GetApp().DrawInvalidControls();
   }
}

CGLToolBarControl *CMainFrame::FindToolBar(const String &strName) const
{
   return m_glMainRenderWnd.m_glToolBarContainer.FindToolBar( strName );
}

CGLToolBarControl *CMainFrame::AddToolBar( const String &strName )
{
   return m_glMainRenderWnd.m_glToolBarContainer.AddToolBar( strName );
}

CommandUIPlugin *CMainFrame::GetCommandUIPlugin( UINT nCmdID)
{
   CommandUIPluginList *pList = GetMainInterface()->GetCommandUIPluginManager()->GetCommandUIPluginListMainFrame();
   for (POSITION pos = pList->GetHeadPosition(); pos; ) {
      CommandUIPlugin *pPlugin = pList->GetNext(pos);
      if (pPlugin->GetCmdID() == nCmdID)
         return pPlugin;
   } 
   return NULL;
}

void CMainFrame::OnUpdatePluginCommand( CCmdUI *pCmdUI )
{
   CommandUIPlugin *pPlugin = GetCommandUIPlugin(pCmdUI->m_nID);     
   if (pPlugin==NULL) {
      pCmdUI->Enable(FALSE);
      return;
   }

   try {
      pPlugin->GetOnUpdateCommandMainFrameCallback()(pPlugin->GetPluginData(), this, pCmdUI);
   }
   catch(const String &strReason)
   {
      g_outputWindow << pPlugin->FormatError("OnUpdateCommandUI", strReason) << "\n";
   }
   catch(...) {
      g_outputWindow << pPlugin->FormatError("OnUpdateCommandUI", "Unknown") << "\n";
   }
}

void CMainFrame::OnPluginCommand( UINT nID )
{
   CommandUIPlugin *pPlugin = GetCommandUIPlugin(nID);

   if (!pPlugin)
   {
      ASSERT(FALSE); // We should never get here if pPlugin is NULL
      return;
   }

   CommandPlugin *pCommand = NULL;
   String strParameters;

   try {
      pPlugin->GetOnCommandMainFrameCallback()(pPlugin->GetPluginData(), nID, this, pCommand, strParameters);
   }
   catch(const String &strReason)
   {
      g_outputWindow << pPlugin->FormatError("OnCommand", strReason) << "\n";
      return;
   }
   catch(...) {
      g_outputWindow << pPlugin->FormatError("OnCommand", "Unknown") << "\n";
      return;
   }

   if (pCommand==NULL)
   {
      GetApp().DrawInvalidControls();
      return;
   }

   CommandList *pCommandList = GetMainInterface()->GetCurrentCommandList();
   if (pCommandList==NULL) {
      String strMsg("No Active HistoryList.");
      g_outputWindow << strMsg << "\n";
      return;
   }

   StringList strResults;
   pCommandList->Do(pCommand, strParameters, strResults);
}

void CMainFrame::OnUpdateHistoryControl(CCmdUI *pCmdUI)
{
   CHistoryCmdUI *pHistoryCmdUI = (CHistoryCmdUI *)pCmdUI;
   pHistoryCmdUI->SetCommandObject( m_pMainInterface->GetCurrentCommandObject() );
}

void CMainFrame::OnUpdateComponentCombo(CCmdUI *pCmdUI)
{
   CComponentComboCmdUI *pComponentComboCmdUI = (CComponentComboCmdUI *)pCmdUI;
   pComponentComboCmdUI->SetAvailableComponentTypes( NULL );
}

void CMainFrame::OnUpdateManipulatorCombo(CCmdUI *pCmdUI)
{
   CManipulatorComboCmdUI *pManipulatorComboCmdUI = (CManipulatorComboCmdUI *)pCmdUI;
   pManipulatorComboCmdUI->SetAvailableManipulators( NULL );
}

void CMainFrame::OnUpdateOutputControl( CCmdUI *pCmdUI )
{
   CGLOutputControlCmdUI *pOutputControlCmdUI = (CGLOutputControlCmdUI *)pCmdUI;
   pOutputControlCmdUI->SetCurrentWorkingObject( m_pMainInterface->GetProject());
   pOutputControlCmdUI->SetCurrentCommandObject( m_pMainInterface->GetProject());
}

void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
   CFrameWnd::OnSize(nType, cx, cy);
}

GLuint CMainFrame::CreateCallListFromGeoResource( UINT nResourceID )
{
   m_glSharedRenderWnd.MakeRenderContextCurrent();

   GLuint glCallList = 0;
   HINSTANCE hInstance = AfxGetInstanceHandle();

   HRSRC hResInfo = ::FindResource( hInstance, MAKEINTRESOURCE(nResourceID), "Geometry" );
   if (hResInfo == NULL)
      return 0;

   HGLOBAL hResData = ::LoadResource( hInstance, hResInfo );
   if (hResData == NULL)
      return 0;

   void *pResource = ::LockResource( hResData );
   if (hResData == NULL)
   {
      ::FreeResource( hResData );
      return 0;
   }

   DWORD nSize = ::SizeofResource( hInstance, hResInfo );
   MemoryStream ms;
   if (!ms.Open( (char *)pResource, nSize, MemoryStream::BS_INPUT | MemoryStream::BS_BINARY ))
      goto Error_Exit;

   UINT nVersion;
   ms >> nVersion;
   if (nVersion != 2)
      goto Error_Exit;

   glCallList = glGenLists( 1 );
   if (!glCallList)
      goto Error_Exit;
   glNewList( glCallList, GL_COMPILE_AND_EXECUTE );
   {
      UINT nVertexCount;
      ms >> nVertexCount;
      CArray<Vector> vertexarray;
      vertexarray.SetSize( nVertexCount );
      for (UINT i=0; i<nVertexCount; i++)
         ms >> vertexarray[i];

      UINT nVertexNormalCount;
      ms >> nVertexNormalCount;
      CArray<Vector> vertexnormalarray;
      vertexnormalarray.SetSize( nVertexNormalCount );
      for (UINT i=0; i<nVertexNormalCount; i++)
         ms >> vertexnormalarray[i];

      //UINT nUVCount;
      //ms >> nUVCount;
      //CArray<Vector2> uvarray;
      //uvarray.SetSize( nUVCount );
      //for (UINT i=0; i<nUVCount; i++)
      //   ms >> uvarray[i];

      UINT nPolyCount;
      ms >> nPolyCount;
      for (UINT i=0; i<nPolyCount; i++)
      {
         UINT nNumVertices;
         ms >> nNumVertices;

         //UINT nNumUVSets;
         //ms >> nNumUVSets;

         glBegin( GL_POLYGON );
         for (UINT nVert=0; nVert<nNumVertices; nVert++)
         {
            UINT nVertexIndex;
            ms >> nVertexIndex;

            UINT nVertexNormalIndex;
            ms >> nVertexNormalIndex;
            glNormal3fv( &vertexnormalarray[nVertexNormalIndex].x );

            //for (UINT nUVSet=0; nUVSet<nNumUVSets; nUVSet++)
            //{
            //   UINT nUVIndex;
            //   ms >> nUVIndex;
            //   glTexCoord2fv( &uvarray[nUVIndex].x );
            //}

            glVertex3fv( &vertexarray[nVertexIndex].x );
         }
         glEnd();
      }
   }
   glEndList();

Error_Exit:
   ::UnlockResource( hResData );
   ::FreeResource( hResData );
   return glCallList;
}

void CMainFrame::OnFileNewProject()
{
   Project *pProject = m_pMainInterface->GetProject();
   if (pProject)
   {
      pProject->OnEditDelete();
      if (m_pMainInterface->GetProject())
         return;
      m_pMainInterface->SetProject(NULL);
   }

   m_pMainInterface->CreateProject();
}

void CMainFrame::OnGotoCommandLine()
{
   m_glMainRenderWnd.m_glTimeLineBar.m_glOutputFrame.m_glCommandLineControl.CreatePopupEdit();
}

void CMainFrame::OnUpdateGotoCommandLine(CCmdUI *pCmdUI)
{
   m_glMainRenderWnd.m_glTimeLineBar.m_glOutputTabControl.SelectTab( 0 );
   pCmdUI->Enable( m_glMainRenderWnd.m_glTimeLineBar.m_glOutputFrame.m_glCommandLineControl.IsEnabled() );
}
