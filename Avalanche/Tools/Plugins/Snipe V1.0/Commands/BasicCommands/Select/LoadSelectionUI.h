#pragma once

#include "HCommandUIPluginHelper.h"

class LoadSelectionBaseCommandUIPlugin : public HCommandUIPluginHelper
{
protected:
     HCommandPlugin *m_pLoadSelectionCommandPlugin;
public:
   DECLARE_COMMANDUI_PLUGINHELPER(LoadSelectionBaseCommandUIPlugin, HCommandUIPluginHelper)

// HCommandUIPluginHelper overrides
   virtual const char *GetCategory() const;

   virtual void Init(HCommandPluginManager *pCommandManager);

   //Message Maps
   virtual SnipeObjectType GetCommandObjectType();
   virtual catch_msg void OnUpdateCommandUIObject(const HSnipeObject *pObject, CCmdUI *pCmdUI);
};

///////////////////////////////////////////////////////////////////////////////////////////////
class LoadSelectionReplaceCommandUIPlugin : public LoadSelectionBaseCommandUIPlugin
{
public:
   DECLARE_COMMANDUI_PLUGINHELPER(LoadSelectionReplaceCommandUIPlugin, LoadSelectionBaseCommandUIPlugin)

   // HCommandUIPluginHelper overrides
   virtual UINT GetBitmapID() const;
   virtual const char *GetDescription() const;

   //Message Maps
   virtual catch_msg void OnCommandObject(const HSnipeObject *pObject, HCommandPlugin *&pOutPlugin, String &strOutParameters);

   //HPluginHelper overrides
   virtual const char *GetTitleName() const;
};

///////////////////////////////////////////////////////////////////////////////////////////////
class LoadSelectionAddCommandUIPlugin : public LoadSelectionBaseCommandUIPlugin
{
public:
   DECLARE_COMMANDUI_PLUGINHELPER(LoadSelectionAddCommandUIPlugin, LoadSelectionBaseCommandUIPlugin)

   // HCommandUIPluginHelper overrides
   virtual UINT GetBitmapID() const;
   virtual const char *GetDescription() const;

   //Message Maps
   virtual catch_msg void OnCommandObject(const HSnipeObject *pObject, HCommandPlugin *&pOutPlugin, String &strOutParameters);

   //HPluginHelper overrides
   virtual const char *GetTitleName() const;
};

///////////////////////////////////////////////////////////////////////////////////////////////
class LoadSelectionSubtractCommandUIPlugin : public LoadSelectionBaseCommandUIPlugin
{
public:
   DECLARE_COMMANDUI_PLUGINHELPER(LoadSelectionSubtractCommandUIPlugin, LoadSelectionBaseCommandUIPlugin)

   // HCommandUIPluginHelper overrides
   virtual UINT GetBitmapID() const;
   virtual const char *GetDescription() const;

   //Message Maps
   virtual catch_msg void OnCommandObject(const HSnipeObject *pObject, HCommandPlugin *&pOutPlugin, String &strOutParameters);

   //HPluginHelper overrides
   virtual const char *GetTitleName() const;
};

///////////////////////////////////////////////////////////////////////////////////////////////
class LoadSelectionToggleCommandUIPlugin : public LoadSelectionBaseCommandUIPlugin
{
public:
   DECLARE_COMMANDUI_PLUGINHELPER(LoadSelectionToggleCommandUIPlugin, LoadSelectionBaseCommandUIPlugin)

   // HCommandUIPluginHelper overrides
   virtual UINT GetBitmapID() const;
   virtual const char *GetDescription() const;

   //Message Maps
   virtual catch_msg void OnCommandObject(const HSnipeObject *pObject, HCommandPlugin *&pOutPlugin, String &strOutParameters);

   //HPluginHelper overrides
   virtual const char *GetTitleName() const;
};
