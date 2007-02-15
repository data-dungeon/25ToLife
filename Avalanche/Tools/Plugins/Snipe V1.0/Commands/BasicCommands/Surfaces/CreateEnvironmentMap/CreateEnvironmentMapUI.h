#pragma once

#include "HCommandUIPluginHelper.h"

class CreateEnvironmentMapCommandUIPlugin : public HCommandUIPluginHelper
{
protected:
     HCommandPlugin *m_pCreateEnvironmentMapCommandPlugin;
public:
   DECLARE_COMMANDUI_PLUGINHELPER(CreateEnvironmentMapCommandUIPlugin, HCommandUIPluginHelper)

// HCommandUIPluginHelper overrides
   virtual const char *GetCategory() const;

   virtual UINT GetBitmapID() const;
   virtual const char *GetDescription() const;
   virtual void Init(HCommandPluginManager *pCommandManager);

   //Message Maps
   virtual SnipeObjectType GetCommandObjectType();
   virtual catch_msg void OnUpdateCommandUIObject(const HSnipeObject *pObject, CCmdUI *pCmdUI);
   virtual catch_msg void OnCommandObject(const HSnipeObject *pObject, HCommandPlugin *&pOutPlugin, String &strOutParameters);

//HPluginHelper overrides
   virtual const char *GetTitleName() const;
};
