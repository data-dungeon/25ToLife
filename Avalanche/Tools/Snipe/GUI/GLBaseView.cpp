//

#include "stdafx.h"
#include "GLControls.h"
#include "GLRenderWnd.h"
#include "MainFrm.h"
#include "Snipe.h"
#include "BaseDoc.h"
#include "GLBaseView.h"
#include "GLBaseFrame.h"
#include "GLViewSplitter.h"
#include "SnipeDocTemplate.h"
#include "Objects/HierObject.h"
#include "Interface/GLHitArray.h"
#include "ManipulatorPlugin.h"
#include "Interface/SnipeManipulator.h"
#include "Interface/MainInterface.h"
#include "Interface/SnipeNavigator.h"
#include "Interface/SelectionList.hpp"
#include "Component.h"
#include "GLViewControl.h"
#include "resource.h"

#define HITDIAMETER 7

IMPLEMENT_DYNAMIC(CGLBaseView, CGLControl)

BOOL CGLBaseView::m_bSinglePick;

Property::Info     CGLBaseView::m_categoryDisplayInfo( "Display", "Display" );
BoolProperty::Info    CGLBaseView::m_bDisplayFrameRateInfo( "Frame Rate", "FrameRate", TRUE );

BEGIN_MESSAGE_MAP(CGLBaseView, CGLControl)
   ON_COMMAND(ID_VIEW_ZOOMFITSELECTED, OnViewZoomFitSelected)
   ON_COMMAND(ID_VIEW_ZOOMFITALL, OnViewZoomFitAll)
   ON_UPDATE_COMMAND_UI(ID_SPLIT_BOTH, OnUpdateSplit)
   ON_COMMAND(ID_SPLIT_BOTH, OnSplitBoth)
   ON_UPDATE_COMMAND_UI(ID_SPLIT_HORIZONTALLY, OnUpdateSplit)
   ON_COMMAND(ID_SPLIT_HORIZONTALLY, OnSplitHorizontally)
   ON_UPDATE_COMMAND_UI(ID_SPLIT_VERTICALLY, OnUpdateSplit)
   ON_COMMAND(ID_SPLIT_VERTICALLY, OnSplitVertically)
   ON_UPDATE_COMMAND_UI_RANGE( ID_PLUGIN_COMMAND_FIRST, ID_PLUGIN_COMMAND_LAST, OnUpdatePluginCommand )
   ON_COMMAND_EX_RANGE( ID_PLUGIN_COMMAND_FIRST, ID_PLUGIN_COMMAND_LAST, OnPluginCommand )
   ON_COMMAND(IDR_COMPONENTTYPE_COMBO, OnComponentComboChanged)
   ON_UPDATE_COMMAND_UI(IDR_COMPONENTTYPE_COMBO, OnUpdateComponentCombo)
   ON_COMMAND(IDR_MANIPULATOR_COMBO, OnManipulatorComboChanged)
   ON_UPDATE_COMMAND_UI(IDR_MANIPULATOR_COMBO, OnUpdateManipulatorCombo)
   ON_UPDATE_COMMAND_UI(ID_OUTPUTCONTROL, OnUpdateOutputControl)
   ON_UPDATE_COMMAND_UI(ID_VIEW_TOGGLEMAXIMIZE, OnUpdateToggleMaximize)
   ON_COMMAND(ID_VIEW_TOGGLEMAXIMIZE, OnToggleMaximize)
END_MESSAGE_MAP()

CGLBaseView::CGLBaseView()
{
   m_pNavigatorInstance = NULL;
   m_pBaseFrame = NULL;
   m_pDocument = NULL;
   m_nPickObjectType = SOT_Unknown;
   m_nBaseViewFlags = 0;
   m_starttime.QuadPart = 0;
   m_pDoPickExcludeList = NULL;
   m_pCurrentViewControl = NULL;
   m_pMouseOverViewControl = NULL;

   m_categoryDisplay.Init( &m_categoryDisplayInfo, this );
      m_bDisplayFrameRate.Init( &m_bDisplayFrameRateInfo, &m_categoryDisplay );
}

CGLBaseView::~CGLBaseView()
{
   if (m_pGLRenderWnd && m_pGLRenderWnd->m_pActiveControl==this)
      m_pGLRenderWnd->SetActiveControl( NULL );

   if (m_pDocument)
      m_pDocument->RemoveView(this);

   SetNavigatorInstance( NULL );

   if (MainInterface::GetCurrentView() == this)
      MainInterface::SetCurrentView( NULL );
}

void CGLBaseView::OnUpdateToggleMaximize( CCmdUI *pCmdUI )
{
   static CRuntimeClass *pRuntimeClass = RUNTIME_CLASS(CGLBaseDocFrame);
   pCmdUI->Enable( m_pBaseFrame->m_bMaximized || !m_pBaseFrame->GetParent()->IsKindOf( pRuntimeClass ) );
}

void CGLBaseView::OnToggleMaximize()
{
   if (m_pBaseFrame->m_bMaximized)
      m_pBaseFrame->OnRestore();
   else
      m_pBaseFrame->OnMaximize();
}

void CGLBaseView::OnUpdateSplit( CCmdUI *pCmdUI )
{
   pCmdUI->Enable( !m_pBaseFrame->m_bMaximized );
}

void CGLBaseView::OnActivate( ActivationMsg msg, CGLControl *pOther )
{
   switch (msg)
   {
      case ACTIVATION_INACTIVE:
         break;
      case ACTIVATION_ACTIVE:
         GetMainInterface()->SetCurrentView( this ); // set null in view destructor
         break;
   }
   CGLControl::OnActivate( msg, pOther );
}

void CGLBaseView::OnMouseEnter()
{
   CGLMainRenderWnd *pMainWnd = &GetMainFrame()->m_glMainRenderWnd;
   pMainWnd->m_glStatusPaneControl.SetVisible( FALSE );
   pMainWnd->m_glModStatusPaneControl.SetVisible( TRUE );
   pMainWnd->m_glMouseStatusPaneControl[MOUSESTATUSPANE_LEFT].SetVisible( TRUE );
   pMainWnd->m_glMouseStatusPaneControl[MOUSESTATUSPANE_MIDDLE].SetVisible( TRUE );
   pMainWnd->m_glMouseStatusPaneControl[MOUSESTATUSPANE_RIGHT].SetVisible( TRUE );
   pMainWnd->m_glMouseStatusPaneControl[MOUSESTATUSPANE_WHEEL].SetVisible( TRUE );
   pMainWnd->m_glStatusBarControl.Move( pMainWnd->m_glStatusBarControl.m_ptControl, pMainWnd->m_glStatusBarControl.m_szControl );
}

void CGLBaseView::OnMouseLeave()
{
   CGLMainRenderWnd *pMainWnd = &GetMainFrame()->m_glMainRenderWnd;
   pMainWnd->m_glModStatusPaneControl.SetVisible( FALSE );
   pMainWnd->m_glMouseStatusPaneControl[MOUSESTATUSPANE_LEFT].SetVisible( FALSE );
   pMainWnd->m_glMouseStatusPaneControl[MOUSESTATUSPANE_MIDDLE].SetVisible( FALSE );
   pMainWnd->m_glMouseStatusPaneControl[MOUSESTATUSPANE_RIGHT].SetVisible( FALSE );
   pMainWnd->m_glMouseStatusPaneControl[MOUSESTATUSPANE_WHEEL].SetVisible( FALSE );
   pMainWnd->m_glStatusPaneControl.SetVisible( TRUE );
   pMainWnd->m_glStatusBarControl.Move( pMainWnd->m_glStatusBarControl.m_ptControl, pMainWnd->m_glStatusBarControl.m_szControl );
}

SnipeObject *CGLBaseView::GetObjectHead()
{
   return GetDocument()->GetObjectHead();
}

SnipeNavigatorInstance *CGLBaseView::SetNavigatorInstance( SnipeNavigator *pNavigator )
{
   if (m_pNavigatorInstance)
   {
      m_pNavigatorInstance->Destroy();
      m_pNavigatorInstance = NULL;
   }

   if (pNavigator == NULL) 
      return NULL;

   m_pNavigatorInstance = pNavigator->CreateInstance( this );
   if (m_pNavigatorInstance)
      GetMainFrame()->GetMainRenderWnd()->m_glPanelBar.m_glPropertyFrame.m_glNavigatorPropertyBar.SetObject( m_pNavigatorInstance );

   GetMainFrame()->GetMainRenderWnd()->OnSetCursor( GetMainFrame()->GetMainRenderWnd(), 0, 0 );
   GetMainFrame()->GetMainRenderWnd()->RefreshStatusBarInstructions();

   return m_pNavigatorInstance;
}

BOOL CGLBaseView::SetOperation( Operation nOperation )
{
   if (m_nCurrentOperation == nOperation)
      return FALSE;

   if (nOperation == OP_IDLE) {
      m_nCurrentOperation = nOperation;
      return TRUE;
   }

   //switch( m_nCurrentOperation )
   //{
   //   case OP_MANIPULATING:
   //      GetManipulatorInstance()->OnCancel( this );
   //      break;
   //   case OP_NAVIGATING:
   //      GetNavigatorInstance()->OnCancel( this );
   //      break;
   //}

   m_nCurrentOperation = nOperation;
   return TRUE;
}

CommandUIPlugin *CGLBaseView::GetCommandUIPlugin(UINT nID) 
{ 
   CommandUIPluginList *pList = GetCommandUIPluginList(GetThisSVT());
   for (POSITION pos = pList->GetHeadPosition(); pos; ) {
      CommandUIPlugin *pPlugin = pList->GetNext(pos);
      if (pPlugin->GetCmdID() == nID)
         return pPlugin;
   }
   return NULL;
}

const CGLBaseView *CGLBaseView::IsKindOf( SnipeViewType nType ) const
{
   if (nType == GetThisSVT())
      return this;
   return NULL;
} 

CommandUIPluginList *CGLBaseView::GetCommandUIPluginList(SnipeViewType nType)
{
   return GetMainInterface()->GetCommandUIPluginManager()->GetCommandUIPluginListView( nType ) ;
}

CGLBaseDocFrame *CGLBaseView::FindDocFrame()
{
   for (CGLControl *pControl=m_pParentControl; pControl; pControl=pControl->m_pParentControl) {
      if (pControl->IsKindOf(RUNTIME_CLASS(CGLBaseDocFrame)))
         return (CGLBaseDocFrame *)pControl;
   }
   return NULL;
}

UINT CGLBaseView::GetToolTipText( CPoint point, String &strText ) const
{
   if (GetManipulatorInstance()) {
      UINT nResult = GetManipulatorInstance()->GetToolTipText( point, strText, this );
      if (nResult != -1)
         return nResult;
   }
   if (m_nID != -1)
   {
      if (strText.LoadString( m_nID ) == 0)
         strText.Empty();
   }
   return m_nID;
}

void CGLBaseView::ZoomFitAll()
{
   SnipeObjectList *pList = NULL;

   SnipeObjectType nSOT = GetCurrentSelectionSOT();
   if (GetObjectHead())
      pList = GetObjectHead()->CreateListFromSOT( nSOT );

   if (!pList)
   {
      pList = SNEW SnipeObjectList;
      if (GetObjectHead())
         pList->AddHead( GetObjectHead() );
   }

   if (!GetNavigatorInstance() || !GetNavigatorInstance()->ZoomFit( this, pList ))
      DefaultZoomFit( pList );

   delete pList;
}

void CGLBaseView::ZoomFitSelected()
{
   if (GetDocument()->GetSelectionList())
   {
      SnipeObjectList *pList = GetDocument()->GetSelectionList()->CreateSnipeObjectList();

      if (!GetNavigatorInstance()->ZoomFit( this, pList ))
         DefaultZoomFit( pList );

      delete pList;
   }
}

// Message Maps
void CGLBaseView::OnViewZoomFitAll()
{
   ZoomFitAll();

   Invalidate();
   GetApp().DrawInvalidControls();
}

void CGLBaseView::OnViewZoomFitSelected()
{
   ZoomFitSelected();

   Invalidate();
   GetApp().DrawInvalidControls();
}

void CGLBaseView::OnSplitBoth()
{
   CGLSplitterControl *pSplitter = SNEW CGLSplitterControl( m_pBaseFrame, 2, 2 );
   m_pBaseFrame->UpdateHeader();
   GetApp().DrawInvalidControls();
}

void CGLBaseView::OnSplitHorizontally()
{
   CGLSplitterControl *pSplitter = SNEW CGLSplitterControl( m_pBaseFrame, 1, 2 );
   m_pBaseFrame->UpdateHeader();
   GetApp().DrawInvalidControls();
}

void CGLBaseView::OnSplitVertically()
{
   CGLSplitterControl *pSplitter = SNEW CGLSplitterControl( m_pBaseFrame, 2, 1 );
   m_pBaseFrame->UpdateHeader();
   GetApp().DrawInvalidControls();
}

void CGLBaseView::OnUpdatePluginCommand( CCmdUI *pCmdUI )
{
   CommandUIPlugin *pPlugin = GetCommandUIPlugin(pCmdUI->m_nID);

   if (pPlugin==NULL) {
      pCmdUI->ContinueRouting();
      return;
   }

   try {
      pPlugin->GetOnUpdateCommandViewCallback()(pPlugin->GetPluginData(), this, pCmdUI);
   }
   catch(const String &strReason)
   {
      g_outputWindow << pPlugin->FormatError("OnUpdateCommandUI", strReason) << "\n";
   }
   catch(...) {
      g_outputWindow << pPlugin->FormatError("OnUpdateCommandUI", "Unknown") << "\n";
   }
   return;
}

BOOL CGLBaseView::OnPluginCommand( UINT nID )
{
   CommandUIPlugin *pPlugin = GetCommandUIPlugin(nID);
   
   if (pPlugin==NULL)
      return FALSE; // Not for us, continue routing

   CommandPlugin *pCommand = NULL;
   String strParameters;

   try {
      pPlugin->GetOnCommandViewCallback()(pPlugin->GetPluginData(), nID, this, pCommand, strParameters);
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

   CommandList *pCommandList = GetMainInterface()->GetCurrentCommandList();
   if (pCommandList==NULL) {
      String strMsg("%VIEWTYPE cannot execute commands.");
      strMsg.Replace("%VIEWTYPE", GetClassName()); 
      g_outputWindow << strMsg << "\n";
      return TRUE;
   }

   StringList strResults;
   pCommandList->Do(pCommand, strParameters, strResults);
   return TRUE;
}

// GLControl Overrides
void CGLBaseView::PostInit()
{
   CGLControl::PostInit();

   CGLBaseDocFrame *pBaseDocFrame = FindDocFrame();
   if (pBaseDocFrame)
   {
      m_pDocument = pBaseDocFrame->GetCurrentDocument();
      m_pDocument->AddView( this );
   }
   SetNavigatorInstance( GetMainInterface()->GetCurrentNavigator() );
   MakeActive();
   ZoomFitAll();
}

BOOL CGLBaseView::GetContextMenu( const CPoint &ptControl, CMenu &menu )
{
   SelectionList *pSelectionList = GetMainInterface()->GetCurrentSelectionList();
   if (pSelectionList && !pSelectionList->IsEmpty())
   {
      POSITION pos = pSelectionList->GetHeadPosition();
      SnipeObject *pObject = pSelectionList->GetNext( pos );
      return pObject->GetContextMenu( menu );
   }

   return CGLControl::GetContextMenu( ptControl, menu );
}

BOOL CGLBaseView::OnCmdMsg(UINT nID, int nCode, void* pExtra,	AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// first pump through selected object
   SelectionList *pSelectionList = GetMainInterface()->GetCurrentSelectionList();
   if (pSelectionList && !pSelectionList->IsEmpty())
   {
      SnipeObject *pObject = pSelectionList->GetFocusObject();
      if (!pObject)
         pObject = pSelectionList->GetHead();
      if (pObject && pObject->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
         return TRUE;
   }

   if (m_pDocument->GetCommandObject() && m_pDocument->GetCommandObject()->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
      return TRUE;

	// then pump through pane
	if (CCmdTarget::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// lastly, pump through document
	if (m_pDocument != NULL)
		return m_pDocument->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);

	return FALSE;
}

void CGLBaseView::Draw( UINT nDrawFlags/*=DRAW_CHILDREN|DRAW_PARENT*/ )
{
   CRect rcWindow;
   m_pGLRenderWnd->GetClientRect( &rcWindow );

   MakeRenderContextCurrent();

   glPushAttrib( GL_ENABLE_BIT );
   CRect rcViewport = GetViewportRect();

   GLfloat oldscissor[4];
   glGetFloatv( GL_SCISSOR_BOX, oldscissor );
   glScissor( rcViewport.left, rcWindow.bottom - rcViewport.bottom, rcViewport.Width(), rcViewport.Height());

   GLint oldviewport[4];
   glGetIntegerv( GL_VIEWPORT, oldviewport );
   glViewport( rcViewport.left, rcWindow.bottom - rcViewport.bottom, rcViewport.Width(), rcViewport.Height());					// Reset The Current Viewport

   glClearColor( 91.0f/255.0f * 0.75f, 117.0f/255.0f * 0.75f, 132.0f/255.0f * 0.75f, 1.0f );
   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

   GLboolean bOldDepthtest = glSet( GL_DEPTH_TEST, TRUE );
   GLboolean bOldLighting = glSet( GL_LIGHTING, FALSE );

   if (m_bDoPick)
      DrawContents(); // virtual
   else
   {
      StartTimer();
      //RGBAFloat base( 91.0f/255.0f, 117.0f/255.0f, 132.0f/255.0f, 1.0f );
      //RGBAFloat c[4] = { base * 0.75f,
      //                   base * 0.5f,
      //                   base * 0.75f,
      //                   base * 1.0f };
      //c[0].SetAlpha(1.0f);
      //c[1].SetAlpha(1.0f);
      //c[2].SetAlpha(1.0f);
      //c[3].SetAlpha(1.0f);
      //DrawGradient( c );
      SetWindowsProjectionMatrix();

      DrawContents(); // virtual

      DrawFocusRing();
      DrawShadow();
      EndTimer();
   }

   if (glAddSwapHintRectWIN)
      glAddSwapHintRectWIN( rcViewport.left, rcWindow.bottom - rcViewport.bottom, rcViewport.Width(), rcViewport.Height());

   glViewport( oldviewport[0], oldviewport[1], oldviewport[2], oldviewport[3] );
   glScissor( oldscissor[0], oldscissor[1], oldscissor[2], oldscissor[3] );

   glSet( GL_LIGHTING, bOldLighting );
   glSet( GL_DEPTH_TEST, bOldDepthtest );
   glPopAttrib();
}

BOOL CGLBaseView::StartTimer()
{
   if (!m_bDisplayFrameRate)
      return FALSE;

   if (m_starttime.QuadPart)
      return FALSE;

   TimerStart(m_starttime);
   return TRUE;
}

void CGLBaseView::EndTimer()
{
   if (!m_bDisplayFrameRate)
      return;

   if (!m_starttime.QuadPart)
      return;

   float fElapsed = TimerStop(m_starttime);
   CSize ctrlsize = GetControlSize();
   glColor3f(1.0f,1.0f,1.0f);
   char text[32];
   if (fElapsed < 1.0f)
   {
      float fFPS = 1.0f / fElapsed;
      if (fFPS < 10.0f)
         sprintf( text, "%.1g fps", fFPS );
      else
         sprintf( text, "%d fps", (ULONG)fFPS );
   }
   else
      sprintf( text, "%.2g spf", fElapsed );
   DrawText( ctrlsize.cx-50, ctrlsize.cy-10, text );
   m_starttime.QuadPart = 0;
}

void CGLBaseView::CountPickObjectType(UINT &nCount)
{
   nCount = 0;
   if (m_pDocument && m_pDocument->m_pObjectHead)
      m_pDocument->m_pObjectHead->CountPickObjectType( this, nCount );
}

void CGLBaseView::DrawContents()
{
   if (m_pDocument && m_pDocument->m_pObjectHead)
      m_pDocument->m_pObjectHead->Draw( this );

   if (!m_bDoPick)
      DrawViewControls();
}

void CGLBaseView::DrawViewControls()
{
   GLboolean bOldLighting = glSet( GL_LIGHTING, FALSE );
   GLboolean bOldDepthtest = glSet( GL_DEPTH_TEST, FALSE );

   for (POSITION pos=m_ViewControlList.GetHeadPosition(); pos; )
   {
      CGLViewControl *pViewControl = m_ViewControlList.GetNext( pos );
      pViewControl->Draw();
   }

   glSet( GL_DEPTH_TEST, bOldDepthtest );
   glSet( GL_LIGHTING, bOldLighting );
}

// fill the hitNames array with the pick stack for best hit
void CGLBaseView::HitTestManipulator( CPoint point, PickHit &pickHit) const
{
   pickHit.SetNumNames(0);

   if (!GetManipulatorInstance())
      return;

   GLHitArray glHitArray(8192); // size is actual number of UINTs

   glSelectBuffer( glHitArray.GetBufferSizeInBytes(), glHitArray.GetBuffer() );
   glRenderMode( GL_SELECT );

   glInitNames();
   glPushName(NULL);

   glMatrixMode(GL_MODELVIEW);
   glPushMatrix();
   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   glLoadIdentity();

   // get viewport info
   GLint GLViewport[4];
   GetGLViewportInfo(GLViewport);

   ControlToClient( point );
   CRect rcWindow;
   m_pGLRenderWnd->GetClientRect(&rcWindow);
   point.y = rcWindow.Height() - point.y;

   gluPickMatrix( point.x, point.y-3, HITDIAMETER, HITDIAMETER, GLViewport );

   ///////////////////////////////////
   CGLBaseView *pBaseView = (CGLBaseView *)this;
   pBaseView->m_bDoPick = TRUE;
   pBaseView->SetProjectionMatrix();
   GetManipulatorInstance()->DrawPick( this );
   pBaseView->m_bDoPick = FALSE;
   ///////////////////////////////////

   glMatrixMode(GL_PROJECTION);
   glPopMatrix();
   glMatrixMode(GL_MODELVIEW);
   glPopMatrix();
   pBaseView->SetProjectionMatrix();

   glPopName();

   glHitArray.SetNumHits( glRenderMode( GL_RENDER ) );

   UINT nHit = 0;
   if (glHitArray.GetNumHits() > 0)
   {
      ASSERT(glHitArray.GetHit(0)->GetName(0));

      GLuint nZMinMin = glHitArray.GetHit(0)->GetZMin();
      UINT nBestHitIndex = 0;

      for (UINT i= 1; i < glHitArray.GetNumHits(); i++)
      {
         const GLHitArray::GLHit *pTestHit = glHitArray.GetHit(i);
         ASSERT(pTestHit->GetName(0));

         if (pTestHit->GetZMin() < nZMinMin)
         {
            nZMinMin = pTestHit->GetZMin();
            nBestHitIndex = i;
         }
      }
      const GLHitArray::GLHit *pBestHit = glHitArray.GetHit(nBestHitIndex);

      pickHit.SetZMin(pBestHit->GetZMin());
      pickHit.SetZMax(pBestHit->GetZMax());

      int numHitNames = pBestHit->GetNumNames();
      pickHit.SetNumNames(numHitNames);
      for (int nNameIndex = 0; nNameIndex < numHitNames; nNameIndex++)
      {
         pickHit.SetName(nNameIndex,pBestHit->GetName(nNameIndex));
      }
   }
}

BOOL CGLBaseView::OnLButtonDown( UINT nFlags, CPoint point )
{
   BOOL bMadeActive = MakeActive( TRUE );

   KeyModifiers activekeymodifier;
   activekeymodifier.SetFromWindowsFlags( nFlags );

   MouseStates mousestate;
   mousestate.SetFromWindowsFlags( nFlags );

   if (m_pMouseOverViewControl)
   {
      m_pCurrentViewControl = m_pMouseOverViewControl;
      m_currentViewControlPick = m_mouseOverViewControlPick;
      KeyModifiers keymodifiers = m_pCurrentViewControl->GetKeyModifiers( mousestate, m_currentViewControlPick );
      if (keymodifiers & activekeymodifier)
      {
         if (m_pCurrentViewControl->OnLButtonDown( activekeymodifier, mousestate, point, m_currentViewControlPick ))
         {
            SetOperation(OP_VIEWCONTROLING);
            GetApp().DrawInvalidControls();
            return TRUE;
         }
      }
   }

   SnipeManipulatorInstance *pManipulatorInstance = GetManipulatorInstance();
   if (pManipulatorInstance)
   {
      KeyModifiers keymodifiers = pManipulatorInstance->GetKeyModifiers( this, mousestate );
      if (keymodifiers & activekeymodifier)
      {
         if (pManipulatorInstance->OnLButtonDown( activekeymodifier, mousestate, point, this ))
         {
            SetOperation(OP_MANIPULATING);
            GetApp().DrawInvalidControls();
            return TRUE;
         }
      }
   }

   SnipeNavigatorInstance *pNavigatorInstance = GetNavigatorInstance();
   if (pNavigatorInstance)
   {
      KeyModifiers keymodifiers = pNavigatorInstance->GetKeyModifiers( this, mousestate );
      if (keymodifiers & activekeymodifier)
      {
         if (pNavigatorInstance->OnLButtonDown( activekeymodifier, mousestate, point, this )) {
            SetOperation(OP_NAVIGATING);
            GetApp().DrawInvalidControls();
            return TRUE;
         }
      }
   }

   if (bMadeActive)
   {
      GetApp().DrawInvalidControls();
      return TRUE;
   }

   return FALSE;
}

BOOL CGLBaseView::OnMButtonDown( UINT nFlags, CPoint point )
{
   BOOL bMadeActive = MakeActive( TRUE );

   KeyModifiers activekeymodifier;
   activekeymodifier.SetFromWindowsFlags( nFlags );

   MouseStates mousestate;
   mousestate.SetFromWindowsFlags( nFlags );

   SnipeManipulatorInstance *pManipulatorInstance = GetManipulatorInstance();
   if (pManipulatorInstance)
   {
      KeyModifiers keymodifiers = pManipulatorInstance->GetKeyModifiers( this, mousestate );
      if (keymodifiers & activekeymodifier)
      {
         if (pManipulatorInstance->OnMButtonDown( activekeymodifier, mousestate, point, this ))
         {
            SetOperation(OP_MANIPULATING);
            GetApp().DrawInvalidControls();
            return TRUE;
         }
      }
   }

   SnipeNavigatorInstance *pNavigatorInstance = GetNavigatorInstance();
   if (pNavigatorInstance)
   {
      KeyModifiers keymodifiers = pNavigatorInstance->GetKeyModifiers( this, mousestate );
      if (keymodifiers & activekeymodifier)
      {
         if (pNavigatorInstance->OnMButtonDown( activekeymodifier, mousestate, point, this )) {
            SetOperation(OP_NAVIGATING);
            GetApp().DrawInvalidControls();
            return TRUE;
         }
      }
   }

   if (bMadeActive)
   {
      GetApp().DrawInvalidControls();
      return TRUE;
   }

   return CGLControl::OnMButtonDown( nFlags, point );
}

BOOL CGLBaseView::OnRButtonDown( UINT nFlags, CPoint point )
{
   BOOL bMadeActive = MakeActive( TRUE );

   KeyModifiers activekeymodifier;
   activekeymodifier.SetFromWindowsFlags( nFlags );

   MouseStates mousestate;
   mousestate.SetFromWindowsFlags( nFlags );

   SnipeManipulatorInstance *pManipulatorInstance = GetManipulatorInstance();
   if (pManipulatorInstance)
   {
      KeyModifiers keymodifiers = pManipulatorInstance->GetKeyModifiers( this, mousestate );
      if (keymodifiers & activekeymodifier)
      {
         if (pManipulatorInstance->OnRButtonDown( activekeymodifier, mousestate, point, this ))
         {
            SetOperation(OP_MANIPULATING);
            GetApp().DrawInvalidControls();
            return TRUE;
         }
      }
   }

   SnipeNavigatorInstance *pNavigatorInstance = GetNavigatorInstance();
   if (pNavigatorInstance)
   {
      KeyModifiers keymodifiers = pNavigatorInstance->GetKeyModifiers( this, mousestate );
      if (keymodifiers & activekeymodifier)
      {
         if (pNavigatorInstance->OnRButtonDown( activekeymodifier, mousestate, point, this )) {
            SetOperation(OP_NAVIGATING);
            GetApp().DrawInvalidControls();
            return TRUE;
         }
      }
   }

   if (bMadeActive)
   {
      GetApp().DrawInvalidControls();
      return TRUE;
   }

   return CGLControl::OnRButtonDown( nFlags, point );
}

BOOL CGLBaseView::OnLButtonDblClk( UINT nFlags, CPoint point )
{
   BOOL bMadeActive = MakeActive( TRUE );

   KeyModifiers activekeymodifier;
   activekeymodifier.SetFromWindowsFlags( nFlags );

   MouseStates mousestate;
   mousestate.SetFromWindowsFlags( nFlags );

   SnipeManipulatorInstance *pManipulatorInstance = GetManipulatorInstance();
   if (pManipulatorInstance)
   {
      KeyModifiers keymodifiers = pManipulatorInstance->GetKeyModifiers( this, mousestate );
      if (keymodifiers & activekeymodifier)
      {
         if (pManipulatorInstance->OnLButtonDblClk(activekeymodifier, mousestate, point, this))
         {
            SetOperation(OP_MANIPULATING);
            GetApp().DrawInvalidControls();
            return TRUE;
         }
      }
   }

   SnipeNavigatorInstance *pNavigatorInstance = GetNavigatorInstance();
   if (pNavigatorInstance)
   {
      KeyModifiers keymodifiers = pNavigatorInstance->GetKeyModifiers( this, mousestate );
      if (keymodifiers & activekeymodifier)
      {
         if (pNavigatorInstance->OnLButtonDblClk( activekeymodifier, mousestate, point, this )) {
            SetOperation(OP_MANIPULATING);
            GetApp().DrawInvalidControls();
            return TRUE;
         }
      }
   }

   if (bMadeActive)
   {
      GetApp().DrawInvalidControls();
      return TRUE;
   }

   return CGLControl::OnLButtonDblClk( nFlags, point );
}

BOOL CGLBaseView::OnMButtonDblClk( UINT nFlags, CPoint point )
{
   BOOL bMadeActive = MakeActive( TRUE );

   KeyModifiers activekeymodifier;
   activekeymodifier.SetFromWindowsFlags( nFlags );

   MouseStates mousestate;
   mousestate.SetFromWindowsFlags( nFlags );

   SnipeManipulatorInstance *pManipulatorInstance = GetManipulatorInstance();
   if (pManipulatorInstance)
   {
      KeyModifiers keymodifiers = pManipulatorInstance->GetKeyModifiers( this, mousestate );
      if (keymodifiers & activekeymodifier)
      {
         if (pManipulatorInstance->OnMButtonDblClk(activekeymodifier, mousestate, point, this))
         {
            SetOperation(OP_MANIPULATING);
            GetApp().DrawInvalidControls();
            return TRUE;
         }
      }
   }

   SnipeNavigatorInstance *pNavigatorInstance = GetNavigatorInstance();
   if (pNavigatorInstance)
   {
      KeyModifiers keymodifiers = pNavigatorInstance->GetKeyModifiers( this, mousestate );
      if (keymodifiers & activekeymodifier)
      {
         if (pNavigatorInstance->OnMButtonDblClk( activekeymodifier, mousestate, point, this )) {
            SetOperation(OP_MANIPULATING);
            GetApp().DrawInvalidControls();
            return TRUE;
         }
      }
   }

   if (bMadeActive)
   {
      GetApp().DrawInvalidControls();
      return TRUE;
   }

   return CGLControl::OnMButtonDblClk( nFlags, point );
}

BOOL CGLBaseView::OnRButtonDblClk( UINT nFlags, CPoint point )
{
   BOOL bMadeActive = MakeActive( TRUE );

   KeyModifiers activekeymodifier;
   activekeymodifier.SetFromWindowsFlags( nFlags );

   MouseStates mousestate;
   mousestate.SetFromWindowsFlags( nFlags );

   SnipeManipulatorInstance *pManipulatorInstance = GetManipulatorInstance();
   if (pManipulatorInstance)
   {
      KeyModifiers keymodifiers = pManipulatorInstance->GetKeyModifiers( this, mousestate );
      if (keymodifiers & activekeymodifier)
      {
         if (pManipulatorInstance->OnRButtonDblClk( activekeymodifier, mousestate, point, this ))
         {
            SetOperation(OP_MANIPULATING);
            GetApp().DrawInvalidControls();
            return TRUE;
         }
      }
   }

   SnipeNavigatorInstance *pNavigatorInstance = GetNavigatorInstance();
   if (pNavigatorInstance)
   {
      KeyModifiers keymodifiers = pNavigatorInstance->GetKeyModifiers( this, mousestate );
      if (keymodifiers & activekeymodifier)
      {
         if (pNavigatorInstance->OnRButtonDblClk( activekeymodifier, mousestate, point, this )) {
            SetOperation(OP_MANIPULATING);
            GetApp().DrawInvalidControls();
            return TRUE;
         }
      }
   }

   if (bMadeActive)
   {
      GetApp().DrawInvalidControls();
      return TRUE;
   }

   return CGLControl::OnRButtonDblClk( nFlags, point );
}

BOOL CGLBaseView::OnMouseMove(UINT nFlags, CPoint point)
{
   StartTimer();

   KeyModifiers activekeymodifier;
   activekeymodifier.SetFromWindowsFlags( nFlags );

   MouseStates mousestate;
   mousestate.SetFromWindowsFlags( nFlags );

   if (m_pCurrentViewControl)
   {
      KeyModifiers keymodifiers = m_pCurrentViewControl->GetKeyModifiers( mousestate, m_currentViewControlPick );
      if (keymodifiers & activekeymodifier)
      {
         if (m_pCurrentViewControl->OnMouseMove( activekeymodifier, mousestate, point, m_currentViewControlPick ))
         {
            RefreshStatusBarInstructions();
            GetApp().DrawInvalidControls();
            return TRUE;
         }
      }
   }
   else if (GetOperation() == OP_IDLE)
   {
      m_pMouseOverViewControl = HitTestViewControls( point, m_mouseOverViewControlPick);
   }

   SnipeManipulatorInstance *pManipulatorInstance = GetManipulatorInstance();
   if (pManipulatorInstance)
   {
      KeyModifiers keymodifiers = pManipulatorInstance->GetKeyModifiers( this, mousestate );
      if (keymodifiers & activekeymodifier)
      {
         if (pManipulatorInstance->OnMouseMove( activekeymodifier, mousestate, point, this ))
         {
            RefreshStatusBarInstructions();
            GetApp().DrawInvalidControls();
            return TRUE;
         }
      }
   }

   SnipeNavigatorInstance *pNavigatorInstance = GetNavigatorInstance();
   if (pNavigatorInstance)
   {
      KeyModifiers keymodifiers = pNavigatorInstance->GetKeyModifiers( this, mousestate );
      if (keymodifiers & activekeymodifier)
      {
         if (pNavigatorInstance->OnMouseMove( activekeymodifier, mousestate, point, this ))
         {
            RefreshStatusBarInstructions();
            GetApp().DrawInvalidControls();
            return TRUE;
         }
      }
   }

   RefreshStatusBarInstructions();

   return CGLControl::OnMouseMove( nFlags, point );
}

BOOL CGLBaseView::OnLButtonUp(UINT nFlags, CPoint point)
{
   BOOL bMadeActive = MakeActive( TRUE );
   BOOL bResult = TRUE;

   KeyModifiers activekeymodifier;
   activekeymodifier.SetFromWindowsFlags( nFlags );

   MouseStates mousestatecheck;
   mousestatecheck.SetFromWindowsFlags( nFlags|MK_LBUTTON );

   MouseStates mousestate;
   mousestate.SetFromWindowsFlags( nFlags );

   if (!(nFlags & (MK_LBUTTON|MK_MBUTTON|MK_RBUTTON)))
      bResult = SetOperation(OP_IDLE); // whether the operation changed

   if (m_pCurrentViewControl)
   {
      KeyModifiers keymodifiers = m_pCurrentViewControl->GetKeyModifiers( mousestatecheck, m_currentViewControlPick );
      if (keymodifiers & activekeymodifier)
      {
         if (m_pCurrentViewControl->OnLButtonUp( activekeymodifier, mousestate, point, m_currentViewControlPick ))
         {
            m_pCurrentViewControl = NULL;
            GetApp().DrawInvalidControls();
            return TRUE;
         }
         m_pCurrentViewControl = NULL;
      }
   }

   SnipeManipulatorInstance *pManipulatorInstance = GetManipulatorInstance();
   if (pManipulatorInstance)
   {
      KeyModifiers keymodifiers = pManipulatorInstance->GetKeyModifiers( this, mousestatecheck );
      if (keymodifiers & activekeymodifier)
      {
         if (pManipulatorInstance->OnLButtonUp( activekeymodifier, mousestate, point, this ))
         {
            GetApp().DrawInvalidControls();
            return TRUE;
         }
      }
   }

   SnipeNavigatorInstance *pNavigatorInstance = GetNavigatorInstance();
   if (pNavigatorInstance)
   {
      KeyModifiers keymodifiers = pNavigatorInstance->GetKeyModifiers( this, mousestatecheck );
      if (keymodifiers & activekeymodifier)
      {
         if (pNavigatorInstance->OnLButtonUp( activekeymodifier, mousestate, point, this )) {
            GetApp().DrawInvalidControls();
            return TRUE;
         }
      }
   }

   if (bMadeActive)
   {
      GetApp().DrawInvalidControls();
      return TRUE;
   }

   return bResult;
}

BOOL CGLBaseView::OnMButtonUp(UINT nFlags, CPoint point)
{
   BOOL bMadeActive = MakeActive( TRUE );
   BOOL bResult = TRUE;

   KeyModifiers activekeymodifier;
   activekeymodifier.SetFromWindowsFlags( nFlags );

   MouseStates mousestatecheck;
   mousestatecheck.SetFromWindowsFlags( nFlags|MK_MBUTTON );

   MouseStates mousestate;
   mousestate.SetFromWindowsFlags( nFlags );

   if (!(nFlags & (MK_LBUTTON|MK_MBUTTON|MK_RBUTTON)))
      bResult = SetOperation(OP_IDLE); // whether the operation changed

   SnipeManipulatorInstance *pManipulatorInstance = GetManipulatorInstance();
   if (pManipulatorInstance)
   {
      KeyModifiers keymodifiers = pManipulatorInstance->GetKeyModifiers( this, mousestatecheck );
      if (keymodifiers & activekeymodifier)
      {
         if (pManipulatorInstance->OnMButtonUp( activekeymodifier, mousestate, point, this ))
         {
            GetApp().DrawInvalidControls();
            return TRUE;
         }
      }
   }

   SnipeNavigatorInstance *pNavigatorInstance = GetNavigatorInstance();
   if (pNavigatorInstance)
   {
      KeyModifiers keymodifiers = pNavigatorInstance->GetKeyModifiers( this, mousestatecheck );
      if (keymodifiers & activekeymodifier)
      {
         if (pNavigatorInstance->OnMButtonUp( activekeymodifier, mousestate, point, this )) {
            GetApp().DrawInvalidControls();
            return TRUE;
         }
      }
   }

   if (bMadeActive)
   {
      GetApp().DrawInvalidControls();
      return TRUE;
   }

   return bResult;
}

BOOL CGLBaseView::OnRButtonUp(UINT nFlags, CPoint point)
{
   BOOL bMadeActive = MakeActive( TRUE );
   BOOL bResult = TRUE;

   KeyModifiers activekeymodifier;
   activekeymodifier.SetFromWindowsFlags( nFlags );

   MouseStates mousestatecheck;
   mousestatecheck.SetFromWindowsFlags( nFlags|MK_RBUTTON );

   MouseStates mousestate;
   mousestate.SetFromWindowsFlags( nFlags );

   if (!(nFlags & (MK_LBUTTON|MK_MBUTTON|MK_RBUTTON)))
      bResult = SetOperation(OP_IDLE); // whether the operation changed

   SnipeManipulatorInstance *pManipulatorInstance = GetManipulatorInstance();
   if (pManipulatorInstance)
   {
      KeyModifiers keymodifiers = pManipulatorInstance->GetKeyModifiers( this, mousestatecheck );
      if (keymodifiers & activekeymodifier)
      {
         if (pManipulatorInstance->OnRButtonUp( activekeymodifier, mousestate, point, this ))
         {
            GetApp().DrawInvalidControls();
            return TRUE;
         }
      }
   }

   SnipeNavigatorInstance *pNavigatorInstance = GetNavigatorInstance();
   if (pNavigatorInstance)
   {
      KeyModifiers keymodifiers = pNavigatorInstance->GetKeyModifiers( this, mousestatecheck );
      if (keymodifiers & activekeymodifier)
      {
         if (pNavigatorInstance->OnRButtonUp( activekeymodifier, mousestate, point, this )) {
            GetApp().DrawInvalidControls();
            return TRUE;
         }
      }
   }

   if (bMadeActive)
   {
      GetApp().DrawInvalidControls();
      return TRUE;
   }

   return bResult;
}

BOOL CGLBaseView::OnMouseWheel( UINT nFlags, short zDelta, CPoint point )
{
   KeyModifiers activekeymodifier;
   activekeymodifier.SetFromWindowsFlags( nFlags );

   MouseStates mousestate;
   mousestate = MOUSESTATE_WHEEL;

   SnipeManipulatorInstance *pManipulatorInstance = GetManipulatorInstance();
   if (pManipulatorInstance)
   {
      KeyModifiers keymodifiers = pManipulatorInstance->GetKeyModifiers( this, mousestate );
      if (keymodifiers & activekeymodifier)
      {
         if (pManipulatorInstance->OnMouseWheel( activekeymodifier, mousestate, zDelta, point, this ))
         {
            GetApp().DrawInvalidControls();
            return TRUE;
         }
      }
   }

   SnipeNavigatorInstance *pNavigatorInstance = GetNavigatorInstance();
   if (pNavigatorInstance)
   {
      KeyModifiers keymodifiers = pNavigatorInstance->GetKeyModifiers( this, mousestate );
      if (keymodifiers & activekeymodifier)
      {
         if (pNavigatorInstance->OnMouseWheel( activekeymodifier, mousestate, zDelta, point, this )) {
            SetOperation(OP_NAVIGATING);
            GetApp().DrawInvalidControls();
            SetOperation(OP_IDLE);
            return TRUE;
         }
      }
   }

   return FALSE;
}

void CGLBaseView::RefreshStatusBarInstructions( BOOL bForce/*=FALSE*/ )
{
   KeyModifiers activekeymodifier;
   activekeymodifier.SetFromKeyStates();

   SnipeNavigatorInstance *pNavigatorInstance = GetNavigatorInstance();
   SnipeManipulatorInstance *pManipulatorInstance = GetManipulatorInstance();

   KeyModifiers combinedkeymodifiers;

   CGLMainRenderWnd *pMainRenderWnd = &GetMainFrame()->m_glMainRenderWnd;

   for (int nPane=0; nPane<4; nPane++)
   {
      MouseStates mousestate;
      const char *pInstructions = NULL;

      switch (nPane)
      {
         case 0:
            mousestate = MOUSESTATE_LEFT;
            break;
         case 1:
            mousestate = MOUSESTATE_MIDDLE;
            break;
         case 2:
            mousestate = MOUSESTATE_RIGHT;
            break;
         case 3:
            mousestate = MOUSESTATE_WHEEL;
            break;
      }

      if (pManipulatorInstance)
      {
         KeyModifiers keymodifiers = pManipulatorInstance->GetKeyModifiers( this, mousestate );
         if (keymodifiers & activekeymodifier)
         {
            if (!pInstructions)
               pInstructions = pManipulatorInstance->GetStatusbarInstructions( activekeymodifier, mousestate, this );
         }
         combinedkeymodifiers |= keymodifiers;
      }

      if (pNavigatorInstance)
      {
         KeyModifiers keymodifiers = pNavigatorInstance->GetKeyModifiers( this, mousestate );
         if (keymodifiers & activekeymodifier && !pInstructions)
            pInstructions = pNavigatorInstance->GetStatusbarInstructions( activekeymodifier, mousestate, this );

         combinedkeymodifiers |= keymodifiers;
      }

      if (!pInstructions && mousestate == MOUSESTATE_RIGHT && activekeymodifier == KEYMOD_NONE)
         pInstructions = "Menu";

      pMainRenderWnd->m_glMouseStatusPaneControl[nPane].SetText( pInstructions );
   }

   pMainRenderWnd->m_glModStatusPaneControl.SetKeyModifiers( combinedkeymodifiers );

   if (bForce)
      pMainRenderWnd->m_glModStatusPaneControl.Invalidate();
}

BOOL CGLBaseView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
   switch(nChar)
   {
   case VK_LEFT:
   case VK_UP:
   case VK_DOWN:
   case VK_RIGHT:
      {
         SnipeManipulatorInstance *pManipulatorInstance = GetManipulatorInstance();
         if (pManipulatorInstance)
         {
            if (pManipulatorInstance->OnArrowKeyDown( nChar, nRepCnt, this ))
            {
               GetApp().DrawInvalidControls();
               return TRUE;
            }
         }

         SnipeNavigatorInstance *pNavigatorInstance = GetNavigatorInstance();
         if (pNavigatorInstance)
         {
            if (pNavigatorInstance->OnArrowKeyDown( nChar, nRepCnt, this ))
            {
               GetApp().DrawInvalidControls();
               return TRUE;
            }
         }
         break;
      }
   }
   return FALSE;
}

BOOL CGLBaseView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
   switch(nChar)
   {
   case VK_LEFT:
   case VK_UP:
   case VK_DOWN:
   case VK_RIGHT:
      {
         SnipeManipulatorInstance *pManipulatorInstance = GetManipulatorInstance();
         if (pManipulatorInstance)
         {
            if (pManipulatorInstance->OnArrowKeyUp( nChar, nRepCnt, this ))
            {
               GetApp().DrawInvalidControls();
               return TRUE;
            }
         }

         SnipeNavigatorInstance *pNavigatorInstance = GetNavigatorInstance();
         if (pNavigatorInstance)
         {
            if (pNavigatorInstance->OnArrowKeyUp( nChar, nRepCnt, this ))
            {
               GetApp().DrawInvalidControls();
               return TRUE;
            }
         }
         break;
      }
   }
   return FALSE;
}

void CGLBaseView::DoPick( const CRect &rcPickWindow, PickHitArray &pickHitArray, BOOL *bOutSinglePick/*=NULL*/, SnipeObjectType kindof/*=SOT_Unknown*/, SnipeObjectList *pExcludeList/*=NULL*/ )
{
   pickHitArray.SetNumHits(0);

   m_bSinglePick = FALSE;

   int nNumPickElements = 1;

   if (kindof == SOT_Unknown)
      kindof = GetCurrentSelectionSOT();

   SnipeObjectInfo *pSnipeObjectInfo = SnipeObjectInfo::GetBySOT(kindof);
   nNumPickElements = pSnipeObjectInfo->GetNumPickElements();

   m_nPickObjectType = kindof;

   UINT nMaxObjects;
   CountPickObjectType(nMaxObjects);
   nMaxObjects++;
   
   GLHitArray glHitArray(nMaxObjects * 8);
 
   CRect rcPick(rcPickWindow);
   ControlToClient(rcPick);

   if ((abs(rcPick.Width())<HITDIAMETER)&&(abs(rcPick.Height())<HITDIAMETER))
      m_bSinglePick = TRUE;

   double width = max(HITDIAMETER, abs(rcPick.Width()+1));
   double height = max(HITDIAMETER, abs(rcPick.Height()+1));   

   m_bDoPick = TRUE;
   m_pDoPickExcludeList = pExcludeList;

   if (!m_bSinglePick)
      nNumPickElements = 1;

   for (int nPass = 0; nPass < nNumPickElements; nPass++)
   {
      if (m_bSinglePick)
         SnipeObject::SetCurrentPickElement(nPass);
      else
         SnipeObject::SetCurrentPickElement(-1);

      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();
      glMatrixMode(GL_PROJECTION);
      glPushMatrix();
      glLoadIdentity();

      // get viewport info
      GLint GLViewport[4];
      GetGLViewportInfo(GLViewport);

      // find center of selection rectangle
      CRect rcWindow;
      m_pGLRenderWnd->GetWindowRect(&rcWindow);
      CPoint centerpoint((rcPick.left + rcPick.right)/2, rcWindow.Height() - (rcPick.top + rcPick.bottom)/2);

      // No idea why we need the 3
      gluPickMatrix(centerpoint.x, centerpoint.y-3, width, height, GLViewport);

      glSelectBuffer( glHitArray.GetBufferSizeInBytes(), glHitArray.GetBuffer() );
      glRenderMode( GL_SELECT );

      glInitNames();
      glPushName(NULL);

      Draw();
 
      glPopName();

      glHitArray.SetNumHits( glRenderMode( GL_RENDER ) );

      if ((m_bSinglePick)&&(glHitArray.GetNumHits()>0))
         break;

      glMatrixMode(GL_PROJECTION);
      glPopMatrix();
      glMatrixMode(GL_MODELVIEW);
      glPopMatrix();
   }

   m_pDoPickExcludeList = NULL;
   m_bDoPick = FALSE;
   m_nPickObjectType = SOT_Unknown;

   // for single picking find nearest
   if ((m_bSinglePick)&&(glHitArray.GetNumHits()>0))
   {
      ASSERT(glHitArray.GetHit(0)->GetName(0));

      GLuint nZMinMin = glHitArray.GetHit(0)->GetZMin();
      UINT nBestIndex = 0;

      for (UINT nTestIndex = 1; nTestIndex < glHitArray.GetNumHits(); nTestIndex++)
      {
         const GLHitArray::GLHit *pTestHit = glHitArray[nTestIndex];
         ASSERT(pTestHit->GetName(0));//some how some objects were introduced while picking without a snipeobject pointer

         if (pTestHit->GetZMin()<nZMinMin)
         {
            nZMinMin = pTestHit->GetZMin();
            nBestIndex = nTestIndex;
         }
      }

      const GLHitArray::GLHit *pBestHit = glHitArray[nBestIndex];
      PickHit newHit;
      newHit.SetZMin(pBestHit->GetZMin());
      newHit.SetZMax(pBestHit->GetZMax());

      UINT nNumNames = pBestHit->GetNumNames();
      newHit.SetNumNames(nNumNames);
      for (UINT nNameIndex = 0; nNameIndex < nNumNames; nNameIndex++)
      {
         newHit.SetName(nNameIndex,pBestHit->GetName(nNameIndex));
      }
      pickHitArray.AddHit(newHit);
   }
   else
   {
      pickHitArray.SetNumHits(glHitArray.GetNumHits());
      for (UINT nHitIndex = 0; nHitIndex < glHitArray.GetNumHits(); nHitIndex++)
      {
         const GLHitArray::GLHit *pHit = glHitArray.GetHit(nHitIndex);
         PickHit newHit;
         newHit.SetZMin(pHit->GetZMin());
         newHit.SetZMax(pHit->GetZMax());

         UINT nNumNames = pHit->GetNumNames();
         newHit.SetNumNames(nNumNames);
         for (UINT nNameIndex = 0; nNameIndex < nNumNames; nNameIndex++)
         {
            newHit.SetName(nNameIndex,pHit->GetName(nNameIndex));
         }
         pickHitArray.SetHit(nHitIndex,newHit);
      }
   }

   if (bOutSinglePick)
      *bOutSinglePick = m_bSinglePick;
}

SnipeObjectType CGLBaseView::GetCurrentSelectionSOT() const 
{ 
   return GetSelectableObjectTypeArray()->GetCurrentSOT(); 
}

BOOL CGLBaseView::SetCurrentSelectionSOT( SnipeObjectType nSOT ) 
{ 
   BOOL bResult = GetSelectableObjectTypeArray()->SetCurrentSOT( nSOT );
   ConvertSelectionToCurrentSelectionType( nSOT );
   GetDocument()->InvalidateViews( NULL );
   return bResult;
}

BOOL CGLBaseView::SetCurrentSelectionIndex( UINT nIndex )
{
   GetSelectableObjectTypeArray()->SetCurrentIndex( nIndex );
   ConvertSelectionToCurrentSelectionType( GetSelectableObjectTypeArray()->GetCurrentSOT() );
   GetDocument()->InvalidateViews( NULL );
   return TRUE;
}

UINT CGLBaseView::GetNumSelectionSOTs() const 
{ 
   return GetSelectableObjectTypeArray()->GetNumInfos(); 
}

SnipeObjectType CGLBaseView::GetSelectionSOTAt( UINT nIndex ) const 
{ 
   return GetSelectableObjectTypeArray()->GetSnipeObjectTypeAt( nIndex ); 
}

SnipeObjectTypeArray *CGLBaseView::GetSelectableObjectTypeArray() const
{
   if (GetDocument())
      return &GetDocument()->m_SelectableObjectTypeArray;
   else
      return NULL;
}

SnipeManipulatorInstance *CGLBaseView::GetManipulatorInstance() const
{
   SnipeObjectTypeArray *pSnipeObjectTypeArray = GetSelectableObjectTypeArray();
   if (pSnipeObjectTypeArray)
      return pSnipeObjectTypeArray->GetManipulatorInstance();
   else
      return NULL;
}

void CGLBaseView::SetManipulatorInstance( SnipeManipulator *pManipulator )
{
   SnipeManipulatorInstance *pCurrentManipulatorInstance = GetManipulatorInstance();

   if (pCurrentManipulatorInstance && pManipulator == pCurrentManipulatorInstance->m_pSnipeManipulator)
      return;

   pCurrentManipulatorInstance = GetSelectableObjectTypeArray()->SetManipulator( pManipulator );

   if (pCurrentManipulatorInstance && GetMainInterface()->GetCurrentView() == this)
      GetMainFrame()->GetMainRenderWnd()->m_glPanelBar.m_glPropertyFrame.m_glManipulatorPropertyBar.SetObject( pCurrentManipulatorInstance );

   GetMainFrame()->GetMainRenderWnd()->OnSetCursor( GetMainFrame()->GetMainRenderWnd(), 0, 0 );
   GetMainFrame()->GetMainRenderWnd()->RefreshStatusBarInstructions();
}

SnipeManipulatorList *CGLBaseView::GetAvailableManipulatorList()
{
   SnipeObjectTypeArray *pSnipeObjectTypeArray = GetSelectableObjectTypeArray();
   if (pSnipeObjectTypeArray)
      return pSnipeObjectTypeArray->GetAvailableManipulatorList();
   else
      return NULL;
}

void CGLBaseView::OnUpdateComponentCombo(CCmdUI *pCmdUI)
{
   CComponentComboCmdUI *pComponentComboCmdUI = (CComponentComboCmdUI *)pCmdUI;
   pComponentComboCmdUI->SetAvailableComponentTypes( GetSelectableObjectTypeArray() );
   pComponentComboCmdUI->Enable( GetSelectableObjectTypeArray()->GetNumInfos() );
}

void CGLBaseView::OnComponentComboChanged()
{
   // Don't put code here, put it in SetCurrentSelectionIndex and SetCurrentSelectionSOT !!!
   SetCurrentSelectionIndex(GetMainFrame()->m_glMainRenderWnd.m_glComponentCombo.GetCurSel());
}

void CGLBaseView::OnUpdateManipulatorCombo(CCmdUI *pCmdUI)
{
   CManipulatorComboCmdUI *pManipulatorComboCmdUI = (CManipulatorComboCmdUI *)pCmdUI;
   pManipulatorComboCmdUI->SetAvailableManipulators( GetAvailableManipulatorList() );
   pManipulatorComboCmdUI->SetManipulator( GetManipulatorInstance()?GetManipulatorInstance()->m_pSnipeManipulator:NULL );
   pManipulatorComboCmdUI->Enable( GetAvailableManipulatorList() && GetAvailableManipulatorList()->GetCount());
}

void CGLBaseView::OnManipulatorComboChanged()
{
   SetManipulatorInstance( GetMainFrame()->m_glMainRenderWnd.m_glManipulatorCombo.GetManipulator() );
}

void CGLBaseView::OnUpdateOutputControl( CCmdUI *pCmdUI )
{
   CGLOutputControlCmdUI *pOutputControlCmdUI = (CGLOutputControlCmdUI *)pCmdUI;
   pOutputControlCmdUI->SetCurrentWorkingObject( GetDocument()->GetCommandObject() );
   pOutputControlCmdUI->SetCurrentCommandObject( GetDocument()->GetCommandObject() );
}

BOOL CGLBaseView::OnSetCursor( UINT message, CPoint point )
{
   KeyModifiers activekeymodifier;
   activekeymodifier.SetFromKeyStates();

   MouseStates mousestate;
   mousestate.SetFromMouseStates();

   if (m_pMouseOverViewControl)
   {
      KeyModifiers keymodifiers = m_pMouseOverViewControl->GetKeyModifiers( mousestate, m_mouseOverViewControlPick );
      if (keymodifiers & activekeymodifier)
         return m_pMouseOverViewControl->OnSetCursor( activekeymodifier, mousestate, point, m_mouseOverViewControlPick );
   }

   SnipeManipulatorInstance *pManipulatorInstance = GetManipulatorInstance();
   if (pManipulatorInstance)
   {
      KeyModifiers keymodifiers = pManipulatorInstance->GetKeyModifiers( this, mousestate );
      if (keymodifiers & activekeymodifier)
         return pManipulatorInstance->OnSetCursor( activekeymodifier, mousestate, point, this );
   }

   SnipeNavigatorInstance *pNavigatorInstance = GetNavigatorInstance();
   if (pNavigatorInstance)
   {
      KeyModifiers keymodifiers = pNavigatorInstance->GetKeyModifiers( this, mousestate );
      if (keymodifiers & activekeymodifier)
        return pNavigatorInstance->OnSetCursor( activekeymodifier, mousestate, point, this );
   }

   return FALSE;
}

String CGLBaseView::GetTitle() const
{
   String strTitle;

   CBaseDoc *pDoc = GetDocument();
   if (pDoc)
   {
      strTitle = pDoc->GetTitle();
      if (pDoc->m_viewlist.GetCount() > 1)
      {
         strTitle += ":";
         int i = 1;
         for (POSITION pos=pDoc->m_viewlist.GetHeadPosition(); pos; i++)
         {
            CGLBaseView *pView = pDoc->m_viewlist.GetNext( pos );
            if (pView == this)
            {
               strTitle += i;
               break;
            }
         }
      }
   }

   return strTitle;
}

BOOL CGLBaseView::OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam )
{
   switch (msg)
   {
      case NOTIFYOBJ_VALUECHANGED:
         if (m_pBaseFrame)
            m_pBaseFrame->UpdateHeader();
         Invalidate();
         break;
   }

   return ParentableObject::OnObjectNotify( msg, wParam, lParam );
}

void CGLBaseView::DrawSelectedObjects()
{
   SelectionList *pSelectionList = GetDocument()->GetSelectionList();

   if (pSelectionList)
   {
      glDisable( GL_LIGHTING );
      BOOL bWasColorMaterial = glSet( GL_COLOR_MATERIAL, TRUE );
      BOOL bWasCullFace = glSet( GL_CULL_FACE, FALSE );
      glPolygonOffset( 1.0f, 0.5f );

      for (POSITION pos=pSelectionList->GetHeadPosition(); pos; )
      {
         SnipeObject *pSnipeObject = pSelectionList->GetNext( pos );
         if (!pSnipeObject->IsFocusObject()) // we'll catch it later in DrawFocus()
            pSnipeObject->DrawSelected( this );
      }

      if (IsMirrorMode())
      {
         UINT nCount = 0;
         for (POSITION pos=pSelectionList->GetHeadPosition(); pos; )
         {
            nCount++;
            SnipeObject *pSnipeObject = pSelectionList->GetNext( pos );
            SnipeObject *pMirrorObject = pSnipeObject->GetMirrorObject();
            if (pMirrorObject)
               pMirrorObject->DrawMirrored( this );
         }
      }

      SnipeObject *pFocusObject = SelectionList::GetFocusObject();
      if (pFocusObject)
      {
         if (SelectionList::GetNearestFocusObject())
            SelectionList::GetNearestFocusObject()->DrawNearestFocused( this, pFocusObject );

         pFocusObject->DrawFocused( this );
      }

      glPolygonOffset( 1.0f, 1.0f );
      glSet( GL_CULL_FACE, bWasCullFace );
      glSet( GL_COLOR_MATERIAL, bWasColorMaterial );
   }
}

void CGLBaseView::RemoveViewControl( CGLViewControl *pViewControl )
{
   POSITION pos = m_ViewControlList.Find( pViewControl );
   if (pos)
      m_ViewControlList.RemoveAt( pos );
}

CGLViewControl *CGLBaseView::HitTestViewControls( const CPoint &point, PickHit &pickHit )
{
   UINT nMaxObjects = GetNumViewControls() + 1;
   GLHitArray glHitArray( nMaxObjects * 8 );

   glSelectBuffer( glHitArray.GetBufferSizeInBytes(), glHitArray.GetBuffer() );
   glRenderMode( GL_SELECT );

   glInitNames();
   glPushName( NULL );

   glMatrixMode( GL_MODELVIEW );
   glPushMatrix();
   glMatrixMode( GL_PROJECTION );
   glPushMatrix();
   glLoadIdentity();

   // get viewport info
   GLint GLViewport[4];
   GetGLViewportInfo( GLViewport );

   // find center of selection rectangle
   CPoint ptClient = point;
   ControlToClient( ptClient );
   CRect rcWindow;
   m_pGLRenderWnd->GetWindowRect( &rcWindow );
   CPoint ptCenter( ptClient.x, rcWindow.Height() - ptClient.y );

   // No idea why we need the 3
   gluPickMatrix( ptCenter.x, ptCenter.y-3, 1, 1, GLViewport );

   m_bDoPick = TRUE;
   {
      CRect rcWindow;
      m_pGLRenderWnd->GetClientRect( &rcWindow );

      MakeRenderContextCurrent();

      glPushAttrib( GL_ENABLE_BIT );
      CRect rcViewport = GetViewportRect();

      GLfloat oldscissor[4];
      glGetFloatv( GL_SCISSOR_BOX, oldscissor );
      glScissor( rcViewport.left, rcWindow.bottom - rcViewport.bottom, rcViewport.Width(), rcViewport.Height());

      GLint oldviewport[4];
      glGetIntegerv( GL_VIEWPORT, oldviewport );
      glViewport( rcViewport.left, rcWindow.bottom - rcViewport.bottom, rcViewport.Width(), rcViewport.Height());					// Reset The Current Viewport

      glClearColor( 91.0f/255.0f * 0.75f, 117.0f/255.0f * 0.75f, 132.0f/255.0f * 0.75f, 1.0f );
      glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

      DrawViewControls();

      glViewport( oldviewport[0], oldviewport[1], oldviewport[2], oldviewport[3] );
      glScissor( oldscissor[0], oldscissor[1], oldscissor[2], oldscissor[3] );

      glPopAttrib();
   }
   m_bDoPick = FALSE;

   glMatrixMode(GL_PROJECTION);
   glPopMatrix();
   glMatrixMode(GL_MODELVIEW);
   glPopMatrix();

   glPopName();

   glHitArray.SetNumHits( glRenderMode( GL_RENDER ) );

   if (glHitArray.GetNumHits() > 0)
   {
      ASSERT(glHitArray.GetHit(0)->GetName(0));

      GLuint nZMinMin = glHitArray.GetHit(0)->GetZMin();
      UINT nBestIndex = 0;

      for (UINT nTestIndex = 1; nTestIndex < glHitArray.GetNumHits(); nTestIndex++)
      {
         const GLHitArray::GLHit *pTestHit = glHitArray[nTestIndex];
         ASSERT(pTestHit->GetName(0));//some how some objects were introduced while picking without a control pointer

         if (pTestHit->GetZMin() < nZMinMin)
         {
            nZMinMin = pTestHit->GetZMin();
            nBestIndex = nTestIndex;
         }
      }

      const GLHitArray::GLHit *pBestHit = glHitArray.GetHit(nBestIndex);
      pickHit.SetZMin(pBestHit->GetZMin());
      pickHit.SetZMax(pBestHit->GetZMax());
      UINT nNumNames = pBestHit->GetNumNames();
      pickHit.SetNumNames(nNumNames);
      for (UINT nNameIndex = 0; nNameIndex < nNumNames; nNameIndex++)
      {
         pickHit.SetName(nNameIndex,pBestHit->GetName(nNameIndex));
      }
      return (CGLViewControl *)(pBestHit->GetName(0));
   }
   return NULL;
}
