#pragma once

#include "HCommandUIPluginHelper.h"

class SubdividePatchUpCommandUIPlugin : public HCommandUIPluginHelper
{
protected:
   HCommandPlugin *m_pSubdivideCommandPlugin;
public:
   DECLARE_COMMANDUI_PLUGINHELPER(SubdividePatchUpCommandUIPlugin, HCommandUIPluginHelper)

// HCommandUIPluginHelper overrides
   virtual const char *GetCategory() const;

   virtual UINT GetBitmapID() const;
   virtual const char *GetDescription() const;
   virtual void Init(HCommandPluginManager *pCommandManager);

   //callbacks
   virtual catch_msg void Place(HCMainFrame *pMainFrame);

   //Message Maps
   virtual SnipeObjectType GetCommandObjectType();
   virtual catch_msg void OnUpdateCommandUIObject(const HSnipeObject *pObject, CCmdUI *pCmdUI);
   virtual catch_msg void OnCommandObject(const HSnipeObject *pObject, HCommandPlugin *&pOutPlugin, String &strOutParameters);

//HPluginHelper overrides
   virtual const char *GetTitleName() const;
};

class SubdivideModelUpCommandUIPlugin : public HCommandUIPluginHelper
{
protected:
   HCommandPlugin *m_pSubdivideCommandPlugin;
public:
   DECLARE_COMMANDUI_PLUGINHELPER(SubdivideModelUpCommandUIPlugin, HCommandUIPluginHelper)

   // HCommandUIPluginHelper overrides
   virtual const char *GetCategory() const;

   virtual UINT GetBitmapID() const;
   virtual const char *GetDescription() const;
   virtual void GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const;
   virtual void Init(HCommandPluginManager *pCommandManager);

   //callbacks
   virtual catch_msg void Place(HCMainFrame *pMainFrame);

   //Message Maps
   virtual SnipeViewType GetCommandViewType() { return SVT_CGLModelView; };
   virtual catch_msg void OnUpdateCommandUIView(const HCGLBaseView *pView, CCmdUI *pCmdUI) { pCmdUI->Enable(TRUE); }
   virtual catch_msg void OnCommandView(HCGLBaseView *pView, HCommandPlugin *&pOutPlugin, String &strOutParameters);

   //HPluginHelper overrides
   virtual const char *GetTitleName() const;
};
