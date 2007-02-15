#pragma once

#include "HCommandUIPluginHelper.h"

class ShowAllCommandUIPlugin : public HCommandUIPluginHelper
{
protected:
    HCommandPlugin *m_pShowAllCommandPlugin;

public:
   DECLARE_COMMANDUI_PLUGINHELPER(ShowAllCommandUIPlugin, HCommandUIPluginHelper)

// HCommandUIPluginHelper overrides
   virtual const char *GetCategory() const;

   virtual UINT GetBitmapID() const;
   virtual const char *GetDescription() const;
   virtual void GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const;
   virtual void Init(HCommandPluginManager *pCommandManager);

   //callbacks
   virtual catch_msg void Place(HCMainFrame *pMainFrame);

   //Message Maps
   virtual SnipeViewType GetCommandViewType();
   virtual catch_msg void OnUpdateCommandUIView(const HCGLBaseView *, CCmdUI *pCmdUI); 
   virtual catch_msg void OnCommandView(HCGLBaseView *, HCommandPlugin *&pOutPlugin, String &strOutParameters);

//HPluginHelper overrides
   virtual const char *GetTitleName() const;
};
