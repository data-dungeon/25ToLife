#pragma once
#include "HPluginHelper.h"
#include "HCommandUIPlugin.h"
#include "Enum.h"

/////////////////////////////////////////////////////////////////////////////
// C++ Helper class for HCommandUIPlugin
class HCMainFrame;
class HCGLBaseView;
class HCBaseDoc;
class HCommandUIPlugin;
class HSnipeObject;
class CommandUIAccel;
class HPropertyArray;

class HCommandUIPluginHelper : public HPluginHelper
{
public:
   UINT GetCmdID() { return GetPlugin()->GetCmdID(); }
   HCommandUIPlugin *GetPlugin() { return (HCommandUIPlugin *)HPluginHelper::GetPlugin(); }

   virtual catch_msg HPropertyArray *GetPropertyArray() const { return NULL; }
   virtual catch_msg void Place(HCMainFrame *pMainFrame) {}
   virtual catch_msg BOOL OnObjectNotify(ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam) { return FALSE; }
   virtual void GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel) const { nCount = 0; pAccel = NULL; }
   virtual catch_msg void Init(HCommandPluginManager *pManager) { }; // to resolve HCommandPlugin *

   // Object Message Maps
   virtual SnipeObjectType GetCommandObjectType() { return SOT_Unknown; };
   virtual catch_msg void OnUpdateCommandUIObject(const HSnipeObject *pObject, CCmdUI *pCmdUI) { pCmdUI->Enable(FALSE); } 
   virtual catch_msg void OnCommandObject(const HSnipeObject *pObject, HCommandPlugin *&pOutPlugin, String &strOutParameters) {}

   // View Message Maps
   virtual SnipeViewType GetCommandViewType() { return SVT_Unknown; };
   virtual catch_msg void OnUpdateCommandUIView(const HCGLBaseView *pView, CCmdUI *pCmdUI) { pCmdUI->Enable(FALSE); } 
   virtual catch_msg void OnCommandView(HCGLBaseView *pView, HCommandPlugin *&pOutPlugin, String &strOutParameters) {}

   // Doc Message Maps
   virtual SnipeDocType GetCommandDocType() { return SDT_Unknown; };
   virtual catch_msg void OnUpdateCommandUIDoc(const HCBaseDoc *pDoc, CCmdUI *pCmdUI) { pCmdUI->Enable(FALSE); } 
   virtual catch_msg void OnCommandDoc(HCBaseDoc *pDoc, HCommandPlugin *&pOutPlugin, String &strOutParameters) {}

   // MainFrame Message Maps
   virtual BOOL HasMainFrameCommand() { return FALSE; } 
   virtual catch_msg void OnUpdateCommandUIMainFrame(const HCMainFrame *pMainFrame, CCmdUI *pCmdUI) { pCmdUI->Enable(FALSE); } 
   virtual catch_msg void OnCommandMainFrame(HCMainFrame *pMainFrame, HCommandPlugin *&pOutPlugin, String &strOutParameters) {}


   //pure
   virtual catch_msg UINT GetBitmapID() const = 0; // Called Multiple Times
   virtual const char *GetDescription() const = 0;
   virtual const char *GetCategory() const = 0;
};

////////////////////////////////////////////////////////////////////////////
// Inlines

inline void Init_HCommandUIPluginHelper(void *pPluginData, HCommandPluginManager *pManager)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   ((HCommandUIPluginHelper *)pPluginData)->Init( pManager );
}

inline UINT GetBitmapID_HCommandUIPluginHelper(void *pPluginData)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   return ((HCommandUIPluginHelper *)pPluginData)->GetBitmapID();
}

inline void Place_HCommandUIPluginHelper(void *pPluginData, HCMainFrame *pMainFrame)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   ((HCommandUIPluginHelper *)pPluginData)->Place( pMainFrame );
}

inline BOOL OnObjectNotify_HCommandUIPluginHelper(void *pPluginData, ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   return ((HCommandUIPluginHelper *)pPluginData)->OnObjectNotify( msg, wParam, lParam );
}

inline void OnCommandObject_HCommandUIPluginHelper(void *pPluginData, UINT nCmdID, const HSnipeObject *pObj, HCommandPlugin *&pOutEntry, String &strParameters)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState()); \
   ((HCommandUIPluginHelper *)pPluginData)->OnCommandObject( pObj, pOutEntry, strParameters );
}

inline void OnUpdateCommandUIObject_HCommandUIPluginHelper(void *pPluginData, const HSnipeObject *pObj, CCmdUI *pCmdUI)
{
//   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   ((HCommandUIPluginHelper *)pPluginData)->OnUpdateCommandUIObject( pObj, pCmdUI );
}

inline void OnCommandView_HCommandUIPluginHelper(void *pPluginData, UINT nCmdID, HCGLBaseView *pView, HCommandPlugin *&pOutEntry, String &strParameters)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState()); \
   ((HCommandUIPluginHelper *)pPluginData)->OnCommandView( pView, pOutEntry, strParameters );
}

inline void OnUpdateCommandUIView_HCommandUIPluginHelper(void *pPluginData, const HCGLBaseView *pView, CCmdUI *pCmdUI)
{
//   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   ((HCommandUIPluginHelper *)pPluginData)->OnUpdateCommandUIView( pView, pCmdUI );
}

inline void OnCommandDoc_HCommandUIPluginHelper(void *pPluginData, UINT nCmdID, HCBaseDoc *pDoc, HCommandPlugin *&pOutEntry, String &strParameters)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState()); \
   ((HCommandUIPluginHelper *)pPluginData)->OnCommandDoc( pDoc, pOutEntry, strParameters );
}

inline void OnUpdateCommandUIDoc_HCommandUIPluginHelper(void *pPluginData, const HCBaseDoc *pDoc, CCmdUI *pCmdUI)
{
//   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   ((HCommandUIPluginHelper *)pPluginData)->OnUpdateCommandUIDoc( pDoc, pCmdUI );
}

inline void OnCommandMainFrame_HCommandUIPluginHelper(void *pPluginData, UINT nCmdID, HCMainFrame *pMainFrame, HCommandPlugin *&pOutEntry, String &strParameters)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState()); \
   ((HCommandUIPluginHelper *)pPluginData)->OnCommandMainFrame( pMainFrame, pOutEntry, strParameters );
}

inline void OnUpdateCommandUIMainFrame_HCommandUIPluginHelper(void *pPluginData, const HCMainFrame *pMainFrame, CCmdUI *pCmdUI)
{
//   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   ((HCommandUIPluginHelper *)pPluginData)->OnUpdateCommandUIMainFrame( pMainFrame, pCmdUI );
}

inline HCommandUIPluginHelper *GetCommandUIPluginHelper( HCommandUIPlugin *pPlugin )
{
   ASSERT(pPlugin);
   return (HCommandUIPluginHelper *)pPlugin->GetPluginData();\
}

/////////////////////////////////////////////////////////////////////////
// Macro

#define ADD_SNIPE_COMMAND_UI_PLUGIN( pSnipePlugin, helper_class ) \
{ \
   try \
   { \
      AFX_MANAGE_STATE(AfxGetStaticModuleState()); \
      HCommandUIPluginHelper *pHelper = new helper_class; \
      HCommandUIPlugin *pPlugin = pSnipePlugin->AddCommandUIPlugin(pHelper, Destroy_PluginData); \
      pHelper->SetPlugin(pPlugin); \
      pPlugin->SetClassName( pHelper->GetClassName() ); \
      pPlugin->SetTitleName( pHelper->GetTitleName() ); \
      /*Common Sets*/\
      pPlugin->SetDescription( pHelper->GetDescription() ); \
      pPlugin->SetCategory( pHelper->GetCategory() ); \
      pPlugin->SetPropertyArray( pHelper->GetPropertyArray() ); \
      UINT nCount=0; \
      CommandUIAccel *pAccel=NULL; \
      pHelper->GetAccelerators( nCount, pAccel ); \
      if (nCount && pAccel) \
         pPlugin->SetAccelerators( nCount, pAccel ); \
      pPlugin->SetInitCommandUIPluginCallback( Init_HCommandUIPluginHelper ); \
      pPlugin->SetGetBitmapIDCallback( GetBitmapID_HCommandUIPluginHelper ); \
      pPlugin->SetPlaceCallback( Place_HCommandUIPluginHelper ); \
      pPlugin->SetOnObjectNotifyCallback( OnObjectNotify_HCommandUIPluginHelper ); \
      /*CommandUI Specific*/ \
      pPlugin->SetMessageMapForObject( pHelper->GetCommandObjectType(), \
      OnCommandObject_HCommandUIPluginHelper, OnUpdateCommandUIObject_HCommandUIPluginHelper ); \
      pPlugin->SetMessageMapForView( pHelper->GetCommandViewType(), \
      OnCommandView_HCommandUIPluginHelper, OnUpdateCommandUIView_HCommandUIPluginHelper ); \
      pPlugin->SetMessageMapForDoc( pHelper->GetCommandDocType(), \
      OnCommandDoc_HCommandUIPluginHelper, OnUpdateCommandUIDoc_HCommandUIPluginHelper ); \
      if (pHelper->HasMainFrameCommand()) \
         pPlugin->SetMessageMapForMainFrame( \
      OnCommandMainFrame_HCommandUIPluginHelper, OnUpdateCommandUIMainFrame_HCommandUIPluginHelper ); \
   } \
   catch(...) \
   { \
      String strMsg( "Failed initializing CommandUI: %1."); \
      strMsg.Replace( "%1", #helper_class ); \
      DisplayWarning( strMsg ); \
   } \
}

#define DECLARE_COMMANDUI_PLUGINHELPER(class_name, baseclass_name) \
   DECLARE_PLUGINHELPER(class_name, baseclass_name )
