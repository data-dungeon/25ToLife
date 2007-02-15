// Snipe.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Snipe.h"
#include "MainFrm.h"
#include "Project.h"
#include "ImageDoc.h"
#include "GLImageView.h"
#include "ModelDoc.h"
#include "GLModelView.h"
#include "SceneDoc.h"
#include "GLSceneView.h"
#include "OutputView.h"
#include "SnipeDocTemplate.h"
#include "GLCustomizeDlg.h"
#include "GLSplashControl.h"
#include "MotionDoc.h"
#include "GLMotionView.h"
#include "GLMotionSetView.h"
#include "resource.h"
#include "MotionSetDoc.h"
#include "GLModelFrame.h"

/////////////////////////////////////////////////////////////////////////////
// CSnipeApp

BEGIN_MESSAGE_MAP(CSnipeApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_WINDOW_CLOSEALLDOCUMENTS, OnWindowCloseAll)
   ON_COMMAND(ID_TOOLS_CUSTOMIZE, OnToolsCustomize)
   ON_UPDATE_COMMAND_UI(ID_FILE_MRU_FILE1, OnUpdateRecentFileMenu)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSnipeApp construction

TimeProperty::Info CSnipeApp::m_timeinfo( "Time", "Time", 0, -MAXTIME, MAXTIME );

CSnipeApp::CSnipeApp()
{
	m_pSceneDocTemplate = NULL;
   m_pModelDocTemplate = NULL;
	m_pImageDocTemplate = NULL;

   m_nSnipeAppFlags = 0;
   m_bDisplayCommandInOutput = TRUE;

   m_time.Init( &m_timeinfo, NULL );
   m_timeinfo.m_bUndoable = FALSE;
}

CSnipeApp::~CSnipeApp()
{
   m_docList.RemoveAll();
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CSnipeApp object

CSnipeApp theApp;

CSnipeApp &GetApp()
{
   return theApp;
}

Time GetTime()
{
   return theApp.GetTime();
}

void CSnipeApp::DrawInvalidControls()
{
#ifdef DRAWDIAGNOSTICS
   g_outputWindow << "\nDrawing Invalid Windows:\n";
#endif
   for (POSITION pos=m_glRenderWndList.GetHeadPosition(); pos; ) {
      CGLRenderWnd *pRenderWnd = m_glRenderWndList.GetNext( pos );
      pRenderWnd->DrawInvalidControls();
   }
}

void CSnipeApp::InvalidateAllViews( SnipeObject *pObject )
{
   for (POSITION pos=m_docList.GetHeadPosition(); pos; )
   {
      CBaseDoc *pDoc = m_docList.GetNext( pos );
      pDoc->InvalidateViews( pObject );
   }
}

/////////////////////////////////////////////////////////////////////////////
// CSnipeApp initialization

BOOL CSnipeApp::InitInstance()
{
	InitCommonControls();

	CWinApp::InitInstance();

   //CAlphaSplashWnd splash( L"Splash.png" );

   SetRegistryKey(_T("Avalanche Software"));
   LoadStdProfileSettings( 10 );

	m_pSceneDocTemplate = SNEW CSnipeDocTemplate(IDR_SCENETYPE, RUNTIME_CLASS(CSceneDoc), RUNTIME_CLASS(CGLHeaderFrame), RUNTIME_CLASS(CGLSceneView));
	AddDocTemplate(m_pSceneDocTemplate);

	m_pModelDocTemplate = SNEW CSnipeDocTemplate(IDR_MODELTYPE, RUNTIME_CLASS(CModelDoc), RUNTIME_CLASS(CGLModelFrame), RUNTIME_CLASS(CGLModelView));
	AddDocTemplate(m_pModelDocTemplate);

   m_pImageDocTemplate = SNEW CSnipeDocTemplate(IDR_IMAGETYPE, RUNTIME_CLASS(CImageDoc), RUNTIME_CLASS(CGLHeaderFrame), RUNTIME_CLASS(CGLImageView));
   AddDocTemplate(m_pImageDocTemplate);

   m_pMotionDocTemplate = SNEW CSnipeDocTemplate(IDR_MOTIONTYPE, RUNTIME_CLASS(CMotionDoc), RUNTIME_CLASS(CGLHeaderFrame), RUNTIME_CLASS(CGLMotionView));
   AddDocTemplate(m_pMotionDocTemplate);

   m_pMotionSetDocTemplate = SNEW CSnipeDocTemplate(IDR_MOTIONTYPE, RUNTIME_CLASS(CMotionSetDoc), RUNTIME_CLASS(CGLHeaderFrame), RUNTIME_CLASS(CGLMotionSetView));
   AddDocTemplate(m_pMotionSetDocTemplate);

	CMainFrame* pFrame = SNEW CMainFrame;
	if (!pFrame)
		return FALSE;
	m_pMainWnd = pFrame;

	pFrame->LoadFrame(IDR_MAINFRAME, WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL, NULL);

   WINDOWPLACEMENT wp;
   if (ReadWindowPlacement(&wp))
      pFrame->SetWindowPlacement(&wp);
   else
      pFrame->ShowWindow(SW_SHOWMAXIMIZED);
   pFrame->SetTimer( 1, 1000/30, NULL ); // set timer to fire every 30th of a second to kick in idle messaging

   CGLSplashControl glSplashControl( &pFrame->m_glMainRenderWnd );

   pFrame->UpdateWindow();

   pFrame->m_pMainInterface->Init();
   pFrame->GetMainRenderWnd()->m_glTimeLineBar.m_glTransportControl.m_glCurrentFrameEdit.SetProperty( &m_time );
   pFrame->GetMainRenderWnd()->m_glTimeLineBar.m_glTimeLineFrame.m_glTimeLineView.m_timelinedoc.m_SelectableObjectTypeArray.SetCurrentIndex( 0 );
   pFrame->m_pMainInterface->SetCurrentView( NULL );
   pFrame->GetMainRenderWnd()->SetActiveControl( NULL );

   CString strWelcomeMsg = GetProfileString( "General", "Welcome Message", "Hi there!  Welcome to Snipe!" );
   g_outputWindow << "<W>" << strWelcomeMsg << "\n";
   WriteProfileString( "General", "Welcome Message", strWelcomeMsg );

   pFrame->m_pMainInterface->CreateProject();

   // All tests (REMOVE TODO)
   //SetBasePath( "C:/Dan/Temp" );

   //String strFileName1("c:/Dan/Temp/Maps/Image.tga");
   //String strFileName2("C:/Dan/Temp2/Image.tga");
   //String strFileName3("D:/Dan/Temp2/Image.tga");
   //String strFileName4("Maps/Image.tga");

   //String strRelativeFileName1;
   //String strRelativeFileName2;
   //String strRelativeFileName3;
   //String strRelativeFileName4;

   //strRelativeFileName1 = GetRelativeFileName( strFileName1 );
   //strRelativeFileName2 = GetRelativeFileName( strFileName2 );
   //strRelativeFileName3 = GetRelativeFileName( strFileName3 );
   //strRelativeFileName4 = GetRelativeFileName( strFileName4 );

   //String temp;

   //temp = GetAbsoluteFileName( strRelativeFileName1 );
   //temp = GetAbsoluteFileName( strRelativeFileName2 );
   //temp = GetAbsoluteFileName( strRelativeFileName3 );
   //temp = GetAbsoluteFileName( strRelativeFileName4 );

   return TRUE;
}

static TCHAR BASED_CODE szSection[] = _T("General");
static TCHAR BASED_CODE szWindowPos[] = _T("Window Position");
static TCHAR szFormat[] = _T("%u,%u,%d,%d,%d,%d,%d,%d,%d,%d");

BOOL ReadWindowPlacement(LPWINDOWPLACEMENT pwp)
{
   String tempBuffer = GetApp().GetProfileString( szSection, szWindowPos, NULL );
   CString strBuffer = tempBuffer.Get();
   if (strBuffer.IsEmpty())
      return FALSE;

   WINDOWPLACEMENT wp;
   int nRead = _stscanf(strBuffer, szFormat,
      &wp.flags, &wp.showCmd,
      &wp.ptMinPosition.x, &wp.ptMinPosition.y,
      &wp.ptMaxPosition.x, &wp.ptMaxPosition.y,
      &wp.rcNormalPosition.left, &wp.rcNormalPosition.top,
      &wp.rcNormalPosition.right, &wp.rcNormalPosition.bottom);

   if (nRead != 10)
      return FALSE;

   wp.length = sizeof wp;
   *pwp = wp;
   return TRUE;
}

void WriteWindowPlacement(LPWINDOWPLACEMENT pwp)
{
   TCHAR szBuffer[sizeof("-32767")*8 + sizeof("65535")*2];

   wsprintf(szBuffer, szFormat,
      pwp->flags, pwp->showCmd,
      pwp->ptMinPosition.x, pwp->ptMinPosition.y,
      pwp->ptMaxPosition.x, pwp->ptMaxPosition.y,
      pwp->rcNormalPosition.left, pwp->rcNormalPosition.top,
      pwp->rcNormalPosition.right, pwp->rcNormalPosition.bottom);
   GetApp().WriteProfileString( szSection, szWindowPos, szBuffer );
}

/////////////////////////////////////////////////////////////////////////////
// CSnipeApp message handlers

BOOL CSnipeApp::OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam )
{
   switch (msg)
   {
   case NOTIFYOBJ_VALUECHANGED:
      {
         Property *pProperty = (Property *)lParam;
         if (pProperty == &m_time)
         {
            InvalidateAllViews( NULL );
            return TRUE;
         }
      }
   }

   return SnipeObject::OnObjectNotify( msg, wParam, lParam );
}

void CSnipeApp::OnWindowCloseAll() 
{
   CloseAllDocuments(FALSE);
   DrawInvalidControls();
}

void CSnipeApp::OnToolsCustomize()
{
   CGLCustomizeDlg dlg;
   GetMainFrame()->m_pCustomizeDlg = &dlg;
   dlg.DoModal();
   GetMainFrame()->m_pCustomizeDlg = NULL;
}

UINT FindMenuIndex( CMenu *pMenu, const String &strFind )
{
   UINT nCount = pMenu->GetMenuItemCount();
   for (UINT nIndex=0; nIndex<nCount; nIndex++)
   {
      CString strMenu;
      pMenu->GetMenuString( nIndex, strMenu, MF_BYPOSITION );
      if (strFind == (LPCTSTR)strMenu)
         return nIndex;
   }

   return (UINT)-1;
}

void CSnipeApp::OnUpdateRecentFileMenu(CCmdUI* pCmdUI)
{
   ASSERT_VALID(this);
   if (m_pRecentFileList == NULL) // no MRU files
      pCmdUI->Enable(FALSE);
   else
   {
      if (pCmdUI->m_pMenu && pCmdUI->m_pSubMenu)
         pCmdUI->m_pMenu = pCmdUI->m_pSubMenu;

      m_pRecentFileList->UpdateMenu(pCmdUI);
   }
}

CDocument *CSnipeApp::OpenDocumentFile( LPCTSTR lpszFileName )
{
   String strFileName = lpszFileName;

   SnipeObjectType nSOT = GetFileType( strFileName );
   if (nSOT == SOT_Unknown) 
   {
      AfxMessageBox("Unknown file format.");
      return NULL;
   }

   CWaitCursor waitcursor;

   MemoryStream fs;
   try 
   {
      fs.Open( strFileName, BaseStream::BS_INPUT|BaseStream::BS_ASCII );
   }
   catch (const String &strReason) {
      g_outputWindow << "Error opening file: " << strFileName << " (" << strReason << ")\n";
      return NULL;
   }
   catch (...) {
      g_outputWindow << "Error opening file: " << strFileName << "\n";
      return NULL;
   }

   Project *pProject = GetMainInterface()->GetProject();
   if (pProject && nSOT==SOT_Project)
   {
      pProject->OnEditDelete();
      if (GetMainInterface()->GetProject())
         return NULL;
      pProject = NULL;
   }

   if (!pProject)
      pProject = GetMainInterface()->CreateProject();

   String strOldBasePath = SetBasePath( strFileName.GetPathOnly() );

   OutputWindowTimer timer( "Loaded " + strFileName + " in %1." );
   pProject->Load( fs );

   SetBasePath( strOldBasePath );

   AfxGetApp()->AddToRecentFileList( strFileName.Get() );

   return NULL;
}

BOOL CSnipeApp::IsIdleMessage( MSG* pMsg )
{
   //   for (POSITION docpos=m_docList.GetHeadPosition(); docpos; )
   //   {
   //      CBaseDoc *pDoc = m_docList.GetNext( docpos );
   //      pDoc->OnSnipeIdle();
   //   }

   return CWinApp::IsIdleMessage( pMsg );
}

BOOL CSnipeApp::OnIdle( LONG lCount )
{
   //return CWinApp::OnIdle( lCount );
   BOOL bMoreIdles = CWinThread::OnIdle( lCount ); // skip CWinApp

   for (POSITION docpos=m_docList.GetHeadPosition(); docpos; )
   {
      CBaseDoc *pDoc = m_docList.GetNext( docpos );
      bMoreIdles |= pDoc->OnSnipeIdle();
   }

   return bMoreIdles;
}
