#pragma once

#include "HCommandUIPluginHelper.h"

class VertexSelectionCommandUIPlugin : public HCommandUIPluginHelper
{
public:
   DECLARE_COMMANDUI_PLUGINHELPER(VertexSelectionCommandUIPlugin, HCommandUIPluginHelper)
//HCommandUIPluginHelper overrides
   virtual UINT GetBitmapID() const;
   virtual const char *GetDescription() const;
   virtual void GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const;
   virtual const char *GetCategory() const;

   //callbacks
   virtual catch_msg void Place(HCMainFrame *pMainFrame);

   // Doc Message Maps
   virtual SnipeViewType GetCommandViewType() { return SVT_CGLBaseView; }
   virtual catch_msg void OnUpdateCommandUIView(const HCGLBaseView *pView, CCmdUI *pCmdUI); 
   virtual catch_msg void OnCommandView(HCGLBaseView *pView, HCommandPlugin *&pOutPlugin, String &strOutParameters);

//HPluginHelper overrides
   virtual const char *GetTitleName() const;
};

class EdgeSelectionCommandUIPlugin : public HCommandUIPluginHelper
{
public:
   DECLARE_COMMANDUI_PLUGINHELPER(EdgeSelectionCommandUIPlugin, HCommandUIPluginHelper)
   //HCommandUIPluginHelper overrides
   virtual UINT GetBitmapID() const;
   virtual const char *GetDescription() const;
   virtual void GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const;
   virtual const char *GetCategory() const;

   //callbacks
   virtual catch_msg void Place(HCMainFrame *pMainFrame);

   // Doc Message Maps
   virtual SnipeViewType GetCommandViewType() { return SVT_CGLBaseView; }
   virtual catch_msg void OnUpdateCommandUIView(const HCGLBaseView *pView, CCmdUI *pCmdUI); 
   virtual catch_msg void OnCommandView(HCGLBaseView *pView, HCommandPlugin *&pOutPlugin, String &strOutParameters);

   //HPluginHelper overrides
   virtual const char *GetTitleName() const;
};

class FaceSelectionCommandUIPlugin : public HCommandUIPluginHelper
{
public:
   DECLARE_COMMANDUI_PLUGINHELPER(FaceSelectionCommandUIPlugin, HCommandUIPluginHelper)

   //HCommandUIPluginHelper overrides
   virtual UINT GetBitmapID() const;
   virtual const char *GetDescription() const;
   virtual void GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const;
   virtual const char *GetCategory() const;

   //callbacks
   virtual catch_msg void Place(HCMainFrame *pMainFrame);

   // Doc Message Maps
   virtual SnipeViewType GetCommandViewType() { return SVT_CGLBaseView; }
   virtual catch_msg void OnUpdateCommandUIView(const HCGLBaseView *pView, CCmdUI *pCmdUI); 
   virtual catch_msg void OnCommandView(HCGLBaseView *pView, HCommandPlugin *&pOutPlugin, String &strOutParameters);

   //HPluginHelper overrides
   virtual const char *GetTitleName() const;
};
