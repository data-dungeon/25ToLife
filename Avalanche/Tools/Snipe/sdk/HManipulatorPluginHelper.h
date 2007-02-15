#pragma once
#include "HCommandUIPluginHelper.h"
#include "HManipulatorPlugin.h"
#include "Enum.h"
#include "HCMainFrame.h"
#include "HToolBarControl.h"

/////////////////////////////////////////////////////////////////////////////
// C++ Helper class for HManipulatorPlugin
class HCGLBaseView;
class HCBaseDoc;
class HManipulatorPlugin;
class HSelectionNode;
class HSnipeObjectNodeList;
class HPropertyArray;
class ManipulatorInstance;

class HManipulatorPluginHelper : public HCommandUIPluginHelper
{
public:
   // Note All derived MessageMap functions from HCommandUIPLuginHelper like
   // OnCommandObject, OnCommandView, OnCommandDoc, along with their OnUpdates
   // Are not hooked up in Manipulators on Purpose.  They are handled automatically
   // By Snipe.
   // Do not hook up the function pointers.

   // Overridables
   virtual catch_msg ManipulatorInstance *CreateInstance() = 0;
   virtual catch_msg BOOL SupportSOT( SnipeObjectType nType ) = 0;

   // HCommandUIPluginHelper Overrides
   virtual void Place(HCMainFrame *pMainFrame) 
   {
      HCGLToolBarControl *pToolBar = pMainFrame->AddToolBar(GetCategory());
      pToolBar->AddButton(GetPlugin());
   }
   virtual const char *GetCategory() const { return "Manipulators"; }
   virtual const char *GetDescription() const
   {
      static String strDescription;
      strDescription = (String)"Set current manipulator to " + GetTitleName() +".";
      return strDescription.Get();
   };
};

class ManipulatorInstance : public HCallbackHelper
{
private:
   HManipulatorPluginHelper *m_pManipulatorPluginHelper;
public:

            ManipulatorInstance( HManipulatorPluginHelper *pManipulatorHelper ) { m_pManipulatorPluginHelper = pManipulatorHelper; }
   virtual ~ManipulatorInstance() {}

   // Operations
   HManipulatorPluginHelper *GetManipulatorPluginHelper() const { return m_pManipulatorPluginHelper; }

   // Callbacks
   virtual catch_msg HPropertyArray *GetPropertyArray() const { return NULL; }
   virtual catch_msg void DrawPick( HCGLBaseView *pView ) {}
   virtual catch_msg void PreDraw( HCGLBaseView *pView ) {}
   virtual catch_msg void PostDraw( HCGLBaseView *pView ) {}
   virtual catch_msg BOOL OnLButtonDown( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView ) { return FALSE; }
   virtual catch_msg BOOL OnLButtonDblClk( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView ) { return FALSE; }
   virtual catch_msg BOOL OnLButtonUp( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView ) { return FALSE; }
   virtual catch_msg BOOL OnRButtonUp( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView ) { return FALSE; }
   virtual catch_msg BOOL OnRButtonDblClk( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView ) { return FALSE; }
   virtual catch_msg BOOL OnRButtonDown( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView ) { return FALSE; }
   virtual catch_msg BOOL OnMButtonUp( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView ) { return FALSE; }
   virtual catch_msg BOOL OnMButtonDblClk( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView ) { return FALSE; }
   virtual catch_msg BOOL OnMButtonDown( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView ) { return FALSE; }
   virtual catch_msg BOOL OnMouseMove( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView ) { return FALSE; }
   virtual catch_msg BOOL OnMouseWheel( KeyModifiers keymodifier, MouseStates mousestate, short zDelta, CPoint point, HCGLBaseView *pView ) { return FALSE; }
   virtual catch_msg BOOL OnSetCursor( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView ) { return FALSE; }
   virtual catch_msg UINT GetToolTipText( CPoint point, String &strText, const HCGLBaseView *pView ) { return -1; }
   virtual catch_msg BOOL OnArrowKeyDown(UINT nChar, UINT nRepCnt, HCGLBaseView *pView) { return FALSE; }
   virtual catch_msg BOOL OnArrowKeyUp(UINT nChar, UINT nRepCnt, HCGLBaseView *pView) { return FALSE; }

   virtual const ManipulatorInstance *IsKindOf( const String &strClassName ) const { return NULL; }

   //pure
   virtual catch_msg const char *GetStatusbarInstructions( KeyModifiers keymodifier, MouseStates mousestate, HCGLBaseView *pView ) const { return NULL; } // Called Multiple Times
   virtual catch_msg KeyModifiers GetKeyModifiers( HCGLBaseView *pView, MouseStates mousestate ) const { static KeyModifiers keymodifiers; return keymodifiers; } // Called Multiple Times, Mouse related activity will not be called unless the current keyboard state matches your modifiers.

   virtual catch_msg BOOL OnApply( HCGLBaseView *pView ) = 0;
   virtual catch_msg BOOL OnCancel( HCGLBaseView *pView ) = 0;

   virtual const char *GetClassName() const = 0;
};

////////////////////////////////////////////////////////////////////////////
// Inlines

// Plugin ////////////////////
inline void *CreateInstance_HManipulatorPluginHelper(void *pPluginData )
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   return (void *)((HManipulatorPluginHelper *)pPluginData)->CreateInstance();
}

inline BOOL SupportSOT_HManipulatorPluginHelper(void *pPluginData, SnipeObjectType nType)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   return ((HManipulatorPluginHelper *)pPluginData)->SupportSOT( nType );
}

inline HManipulatorPluginHelper *GetManipulatorPluginHelper( HManipulatorPlugin *pPlugin )
{
   ASSERT(pPlugin);
   return (HManipulatorPluginHelper *)pPlugin->GetPluginData();
}

// Instance /////////////////
inline void Destroy_ManipulatorInstance( void *pManipulatorInstance ) 
{ 
   AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
   if (pManipulatorInstance) 
      delete (ManipulatorInstance *)pManipulatorInstance; 
}

inline HPropertyArray *GetPropertyArray_ManipulatorInstance(void *pManipulatorInstance)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   return ((ManipulatorInstance *)pManipulatorInstance)->GetPropertyArray();
}

inline BOOL OnObjectNotify_ManipulatorInstance(void *pManipulatorInstance, ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   return ((ManipulatorInstance *)pManipulatorInstance)->OnObjectNotify( msg, wParam, lParam );
}

inline const char *Instructions_ManipulatorInstance(void *pManipulatorInstance, KeyModifiers keymodifiers, MouseStates mousestate, HCGLBaseView *pView )
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   return ((ManipulatorInstance *)pManipulatorInstance)->GetStatusbarInstructions( keymodifiers, mousestate, pView );
}

inline KeyModifiers KeyModifiers_ManipulatorInstance(void *pManipulatorInstance, HCGLBaseView *pView, MouseStates mousestates )
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   return ((ManipulatorInstance *)pManipulatorInstance)->GetKeyModifiers( pView, mousestates );
}

inline void DrawPick_ManipulatorInstance( void *pManipulatorInstance, HCGLBaseView *pView )
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   ((ManipulatorInstance *)pManipulatorInstance)->DrawPick( pView );
}

inline void PreDraw_ManipulatorInstance( void *pManipulatorInstance, HCGLBaseView *pView )
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   ((ManipulatorInstance *)pManipulatorInstance)->PreDraw( pView );
}

inline void PostDraw_ManipulatorInstance( void *pManipulatorInstance, HCGLBaseView *pView )
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   ((ManipulatorInstance *)pManipulatorInstance)->PostDraw( pView );
}

inline BOOL OnLButtonDown_ManipulatorInstance( void *pManipulatorInstance, KeyModifiers keymodifiers, MouseStates mousestate, CPoint point, HCGLBaseView *pView )
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   return ((ManipulatorInstance *)pManipulatorInstance)->OnLButtonDown( keymodifiers, mousestate, point, pView );
}

inline BOOL OnLButtonDblClk_ManipulatorInstance( void *pManipulatorInstance, KeyModifiers keymodifiers, MouseStates mousestate, CPoint point, HCGLBaseView *pView )
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   return ((ManipulatorInstance *)pManipulatorInstance)->OnLButtonDblClk( keymodifiers, mousestate, point, pView );
}

inline BOOL OnLButtonUp_ManipulatorInstance( void *pManipulatorInstance, KeyModifiers keymodifiers, MouseStates mousestate, CPoint point, HCGLBaseView *pView )
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   return ((ManipulatorInstance *)pManipulatorInstance)->OnLButtonUp( keymodifiers, mousestate, point, pView );
}

inline BOOL OnRButtonDown_ManipulatorInstance( void *pManipulatorInstance, KeyModifiers keymodifiers, MouseStates mousestate, CPoint point, HCGLBaseView *pView )
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   return ((ManipulatorInstance *)pManipulatorInstance)->OnRButtonDown( keymodifiers, mousestate, point, pView );
}

inline BOOL OnRButtonDblClk_ManipulatorInstance( void *pManipulatorInstance, KeyModifiers keymodifiers, MouseStates mousestate, CPoint point, HCGLBaseView *pView )
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   return ((ManipulatorInstance *)pManipulatorInstance)->OnRButtonDblClk( keymodifiers, mousestate, point, pView );
}

inline BOOL OnRButtonUp_ManipulatorInstance( void *pManipulatorInstance, KeyModifiers keymodifiers, MouseStates mousestate, CPoint point, HCGLBaseView *pView )
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   return ((ManipulatorInstance *)pManipulatorInstance)->OnRButtonUp( keymodifiers, mousestate, point, pView );
}

inline BOOL OnMButtonDown_ManipulatorInstance( void *pManipulatorInstance, KeyModifiers keymodifiers, MouseStates mousestate, CPoint point, HCGLBaseView *pView )
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   return ((ManipulatorInstance *)pManipulatorInstance)->OnMButtonDown( keymodifiers, mousestate, point, pView );
}

inline BOOL OnMButtonDblClk_ManipulatorInstance( void *pManipulatorInstance, KeyModifiers keymodifiers, MouseStates mousestate, CPoint point, HCGLBaseView *pView )
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   return ((ManipulatorInstance *)pManipulatorInstance)->OnMButtonDblClk( keymodifiers, mousestate, point, pView );
}

inline BOOL OnMButtonUp_ManipulatorInstance( void *pManipulatorInstance, KeyModifiers keymodifiers, MouseStates mousestate, CPoint point, HCGLBaseView *pView )
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   return ((ManipulatorInstance *)pManipulatorInstance)->OnMButtonUp( keymodifiers, mousestate, point, pView );
}

inline BOOL OnMouseMove_ManipulatorInstance( void *pManipulatorInstance, KeyModifiers keymodifiers, MouseStates mousestate, CPoint point, HCGLBaseView *pView )
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   return ((ManipulatorInstance *)pManipulatorInstance)->OnMouseMove( keymodifiers, mousestate, point, pView );
}

inline BOOL OnMouseWheel_ManipulatorInstance( void *pManipulatorInstance, KeyModifiers keymodifier, MouseStates mousestate, short zDelta, CPoint point, HCGLBaseView *pView )
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   return ((ManipulatorInstance *)pManipulatorInstance)->OnMouseWheel( keymodifier, mousestate, zDelta, point, pView );
}

inline BOOL OnSetCursor_ManipulatorInstance( void *pManipulatorInstance, KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView )
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   return ((ManipulatorInstance *)pManipulatorInstance)->OnSetCursor( keymodifier, mousestate, point, pView );
}

inline UINT GetToolTipText_ManipulatorInstance( void *pManipulatorInstance, CPoint point, String &strText, const HCGLBaseView *pView )
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   return ((ManipulatorInstance *)pManipulatorInstance)->GetToolTipText( point, strText, pView );
}

inline BOOL OnApply_ManipulatorInstance( void *pManipulatorInstance, HCGLBaseView *pView )
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   return ((ManipulatorInstance *)pManipulatorInstance)->OnApply( pView );
}

inline BOOL OnCancel_ManipulatorInstance( void *pManipulatorInstance, HCGLBaseView *pView )
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   return ((ManipulatorInstance *)pManipulatorInstance)->OnCancel( pView );
}

inline BOOL OnArrowKeyDown_ManipulatorInstance( void *pManipulatorInstance, UINT nChar, UINT nRepCnt, HCGLBaseView *pView )
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   return ((ManipulatorInstance *)pManipulatorInstance)->OnArrowKeyDown( nChar, nRepCnt, pView );
}

inline BOOL OnArrowKeyUp_ManipulatorInstance( void *pManipulatorInstance, UINT nChar, UINT nRepCnt, HCGLBaseView *pView )
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   return ((ManipulatorInstance *)pManipulatorInstance)->OnArrowKeyUp( nChar, nRepCnt, pView );
}

/////////////////////////////////////////////////////////////////////////
// Macro

#define ADD_SNIPE_MANIPULATOR_PLUGIN( pSnipePlugin, helper_class ) \
{ \
   try \
   { \
      AFX_MANAGE_STATE(AfxGetStaticModuleState()); \
      HManipulatorPluginHelper *pHelper = new helper_class; \
      HManipulatorPlugin *pPlugin = pSnipePlugin->AddManipulatorPlugin(pHelper, Destroy_PluginData); \
      pHelper->SetPlugin(pPlugin); \
      /*CommandUIPluginHelper*/ \
      pPlugin->SetClassName( pHelper->GetClassName() ); \
      pPlugin->SetTitleName( pHelper->GetTitleName() ); \
      pPlugin->SetDescription( pHelper->GetDescription() ); \
      pPlugin->SetCategory( pHelper->GetCategory() ); \
      pPlugin->SetPropertyArray( pHelper->GetPropertyArray() ); \
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
      pPlugin->SetCreateInstanceCallback ( CreateInstance_HManipulatorPluginHelper ); \
      pPlugin->SetSupportSOTCallback( SupportSOT_HManipulatorPluginHelper ); \
      /* ManipulatorInstance */ \
      pPlugin->SetDestroyInstanceCallback( Destroy_ManipulatorInstance ); \
      pPlugin->SetGetPropertyArrayInstanceCallback( GetPropertyArray_ManipulatorInstance ); \
      pPlugin->SetOnObjectNotifyInstanceCallback( OnObjectNotify_ManipulatorInstance ); \
      pPlugin->SetGetStatusbarInstructionsCallback ( Instructions_ManipulatorInstance ); \
      pPlugin->SetGetKeyModifiersCallback( KeyModifiers_ManipulatorInstance ); \
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
   } \
   catch(...) \
   { \
      String strMsg( "Failed initializing Manipulator: %1."); \
      strMsg.Replace( "%1", #helper_class ); \
      DisplayWarning( strMsg ); \
   } \
}

#define DECLARE_MANIPULATOR_PLUGINHELPER(class_name, baseclass_name) \
   DECLARE_PLUGINHELPER(class_name, baseclass_name )

#define DECLARE_MANIPULATORINSTANCE(class_name, baseclass_name) \
   static const char *GetThisClassName() { return #class_name; } \
   virtual const char *GetClassName() const { return #class_name; } \
   virtual const ManipulatorInstance *IsKindOf( const String &strClassName ) const \
   { \
      if (strClassName == GetThisClassName()) \
         return this; \
      return baseclass_name::IsKindOf( strClassName ); \
   } 

