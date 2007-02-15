#pragma once
#include "HManipulatorPluginHelper.h"
#include "HNavigatorPlugin.h"

/////////////////////////////////////////////////////////////////////////////
// C++ Helper class for HManipulatorPlugin

class HNavigatorPluginHelper : public HManipulatorPluginHelper
{
public:
   // Overridables
   virtual catch_msg ManipulatorInstance *CreateInstance(HCGLBaseView *pView) = 0;

   // HManipulatorPluginHelper overrides
   virtual catch_msg ManipulatorInstance *CreateInstance() { return NULL; } // Navigator has its own create that passes the CGLBaseView, Must use this one
   virtual catch_msg BOOL SupportSOT(SnipeObjectType nType) { return FALSE; }

// HCommandUIPluginHelper Overrides
   virtual void Place(HCMainFrame *pMainFrame) { } // Don't want ManipulatorPluginHelpers Default place
   virtual const char *GetCategory() const { return "Navigators"; }
   virtual const char *GetDescription() const
   {
      static String strDescription;
      strDescription = (String)"Set current navigator to " + GetTitleName() +".";
      return strDescription.Get();
   };
};

class NavigatorInstance : public ManipulatorInstance
{
public:
   HCGLBaseView *m_pView;

   NavigatorInstance( HNavigatorPluginHelper *pNavigatorHelper, HCGLBaseView *pView ) :
      ManipulatorInstance( pNavigatorHelper ) 
   {
      m_pView = pView;
   }

   HNavigatorPluginHelper *GetNavigatorPluginHelper() { return (HNavigatorPluginHelper *)GetManipulatorPluginHelper(); }

// Overridables
   virt_base virtual catch_msg BOOL EvaluateButtons( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView  ) { return FALSE; }
   virt_base virtual catch_msg BOOL SetViewAngle( HCGL3DView *pView, ViewAngleIndex nIndex, BOOL bSnapToLastBirdsEye ) { return FALSE; }
   virt_base virtual catch_msg BOOL ZoomFit( HCGLBaseView *pView, HSnipeObjectList *pList ) { return FALSE; }

// HManipulatorPluginHelper Overrides
   virtual catch_msg BOOL OnApply( HCGLBaseView *pView )  { return FALSE; }
   virtual catch_msg BOOL OnCancel( HCGLBaseView *pView ) { return FALSE; }
   virtual catch_msg BOOL OnLButtonDown( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView ) { return EvaluateButtons( keymodifier, mousestate, point, pView ); }
   virtual catch_msg BOOL OnMButtonDown( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView ) { return EvaluateButtons( keymodifier, mousestate, point, pView ); }
   virtual catch_msg BOOL OnRButtonDown( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView ) { return EvaluateButtons( keymodifier, mousestate, point, pView ); }
   virtual catch_msg BOOL OnLButtonUp( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView ) { return EvaluateButtons( keymodifier, mousestate, point, pView ); }
   virtual catch_msg BOOL OnMButtonUp( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView ) { return EvaluateButtons( keymodifier, mousestate, point, pView ); }
   virtual catch_msg BOOL OnRButtonUp( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView ) { return EvaluateButtons( keymodifier, mousestate, point, pView ); }
   virtual catch_msg BOOL OnArrowKeyDown(UINT nChar, UINT nRepCnt, HCGLBaseView *pView) { return FALSE; }
   virtual catch_msg BOOL OnArrowKeyUp(UINT nChar, UINT nRepCnt, HCGLBaseView *pView) { return FALSE; }
};

////////////////////////////////////////////////////////////////////////////
// Inlines

// Plugin ////////////////////
inline void *CreateInstance_HNavigatorPluginHelper(void *pPluginData, HCGLBaseView *pView )
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   return (void *)((HNavigatorPluginHelper *)pPluginData)->CreateInstance(pView);
}

// Instance ////////////////////
inline BOOL SetViewAngle_NavigatorInstance(void *pNavigatorInstance, HCGL3DView *pView, ViewAngleIndex nIndex, BOOL bSnapToLastBirdsEye )
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   return ((NavigatorInstance *)pNavigatorInstance)->SetViewAngle(pView, nIndex, bSnapToLastBirdsEye);
}

inline BOOL ZoomFit_NavigatorInstance(void *pNavigatorInstance, HCGLBaseView *pView, HSnipeObjectList *pList )
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   return ((NavigatorInstance *)pNavigatorInstance)->ZoomFit(pView, pList);
}

/////////////////////////////////////////////////////////////////////////
// Macro

#define ADD_SNIPE_NAVIGATOR_PLUGIN( pSnipePlugin, helper_class ) \
{ \
   try \
   { \
      AFX_MANAGE_STATE(AfxGetStaticModuleState()); \
      HNavigatorPluginHelper *pHelper = new helper_class; \
      HNavigatorPlugin *pPlugin = pSnipePlugin->AddNavigatorPlugin(pHelper, Destroy_PluginData); \
      pHelper->SetPlugin(pPlugin); \
      /*CommandUIPluginHelper*/ \
      pPlugin->SetClassName( pHelper->GetClassName() ); \
      pPlugin->SetTitleName( pHelper->GetTitleName() ); \
      pPlugin->SetDescription( pHelper->GetDescription() ); \
      pPlugin->SetCategory( pHelper->GetCategory() ); \
      UINT nCount=0; \
      CommandUIAccel *pAccel=NULL; \
      pHelper->GetAccelerators( nCount, pAccel ); \
      if (nCount) \
         pPlugin->SetAccelerators( nCount, pAccel ); \
      pPlugin->SetInitCommandUIPluginCallback( Init_HCommandUIPluginHelper ); \
      pPlugin->SetGetBitmapIDCallback( GetBitmapID_HCommandUIPluginHelper ); \
      pPlugin->SetPlaceCallback( Place_HCommandUIPluginHelper ); \
      pPlugin->SetOnObjectNotifyCallback( OnObjectNotify_HCommandUIPluginHelper ); \
      /* ManipulatorPluginHelper */ \
      pPlugin->SetSupportSOTCallback( SupportSOT_HManipulatorPluginHelper ); \
      /* ManipulatorInstance */ \
      pPlugin->SetDestroyInstanceCallback( Destroy_ManipulatorInstance ); \
      pPlugin->SetGetPropertyArrayInstanceCallback( GetPropertyArray_ManipulatorInstance ); \
      pPlugin->SetOnObjectNotifyInstanceCallback( OnObjectNotify_ManipulatorInstance ); \
      pPlugin->SetGetStatusbarInstructionsCallback ( Instructions_ManipulatorInstance ); \
      pPlugin->SetGetKeyModifiersCallback ( KeyModifiers_ManipulatorInstance ); \
      pPlugin->SetDrawPickCallback( DrawPick_ManipulatorInstance ); \
      pPlugin->SetPreDrawCallback( PreDraw_ManipulatorInstance ); \
      pPlugin->SetPostDrawCallback( PostDraw_ManipulatorInstance ); \
      pPlugin->SetOnLButtonDownCallback( OnLButtonDown_ManipulatorInstance ); \
      pPlugin->SetOnLButtonDblClkCallback( OnLButtonDblClk_ManipulatorInstance ); \
      pPlugin->SetOnLButtonUpCallback( OnLButtonUp_ManipulatorInstance ); \
      pPlugin->SetOnRButtonDownCallback( OnRButtonDown_ManipulatorInstance ); \
      pPlugin->SetOnRButtonDblClkCallback( OnRButtonDblClk_ManipulatorInstance ); \
      pPlugin->SetOnRButtonUpCallback( OnRButtonUp_ManipulatorInstance ); \
      pPlugin->SetOnMButtonDownCallback( OnMButtonDown_ManipulatorInstance ); \
      pPlugin->SetOnMButtonDblClkCallback( OnMButtonDblClk_ManipulatorInstance ); \
      pPlugin->SetOnMButtonUpCallback( OnMButtonUp_ManipulatorInstance ); \
      pPlugin->SetOnMouseMoveCallback( OnMouseMove_ManipulatorInstance ); \
      pPlugin->SetOnMouseWheelCallback( OnMouseWheel_ManipulatorInstance ); \
      pPlugin->SetOnSetCursorCallback( OnSetCursor_ManipulatorInstance ); \
      pPlugin->SetGetToolTipTextCallback( GetToolTipText_ManipulatorInstance ); \
      pPlugin->SetOnApplyCallback( OnApply_ManipulatorInstance ); \
      pPlugin->SetOnCancelCallback( OnCancel_ManipulatorInstance ); \
      pPlugin->SetOnArrowKeyDownCallback( OnArrowKeyDown_ManipulatorInstance ); \
      pPlugin->SetOnArrowKeyUpCallback( OnArrowKeyUp_ManipulatorInstance ); \
      /* NavigatorPluginHelper */ \
      pPlugin->SetCreateNavigatorInstanceCallback ( CreateInstance_HNavigatorPluginHelper ); \
      /* NavigatorInstance */ \
      pPlugin->SetSetViewAngleCallback ( SetViewAngle_NavigatorInstance ); \
      pPlugin->SetZoomFitCallback ( ZoomFit_NavigatorInstance ); \
   } \
   catch(...) \
   { \
      String strMsg( "Failed initializing Navigator: %1."); \
      strMsg.Replace( "%1", #helper_class ); \
      DisplayWarning( strMsg ); \
   } \
}

#define DECLARE_NAVIGATOR_PLUGINHELPER(class_name, baseclass_name) \
   DECLARE_MANIPULATOR_PLUGINHELPER(class_name, baseclass_name )

#define DECLARE_NAVIGATORINSTANCE(class_name, baseclass_name) \
   DECLARE_MANIPULATORINSTANCE(class_name, baseclass_name )
