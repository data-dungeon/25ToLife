#pragma once

#include "HCommandUIPluginHelper.h"

class LockCommandUIPlugin : public HCommandUIPluginHelper
{
protected:
   HCommandPlugin *m_pLockCommandPlugin;

public:
   DECLARE_COMMANDUI_PLUGINHELPER(LockCommandUIPlugin, HCommandUIPluginHelper)

// HCommandUIPluginHelper overrides
   virtual const char *GetCategory() const;

   virtual void Init(HCommandPluginManager *pCommandManager);

   //callbacks
   virtual catch_msg void Place(HCMainFrame *pMainFrame);

   //Message Maps
   virtual SnipeDocType GetCommandDocType() { return SDT_CBaseDoc; }
   virtual catch_msg void OnUpdateCommandUIDoc(const HCBaseDoc *pSnipeObject, CCmdUI *pCmdUI); 
   virtual catch_msg void OnCommandDoc( HCBaseDoc *pObject, HCommandPlugin *&pOutPlugin, String &strOutParameters);
};

class LockSelectedCommandUIPlugin : public LockCommandUIPlugin
{
public:
   DECLARE_COMMANDUI_PLUGINHELPER(LockSelectedCommandUIPlugin, LockCommandUIPlugin)

   // HCommandUIPluginHelper overrides
   virtual UINT GetBitmapID() const;
   virtual const char *GetDescription() const;
   virtual void GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const;

   //callbacks
   virtual catch_msg void Place(HCMainFrame *pMainFrame);

   //HPluginHelper overrides
   virtual const char *GetTitleName() const;
};

class LockUnselectedCommandUIPlugin : public LockCommandUIPlugin
{
public:
   DECLARE_COMMANDUI_PLUGINHELPER(LockUnselectedCommandUIPlugin, LockCommandUIPlugin)

   // HCommandUIPluginHelper overrides
   virtual UINT GetBitmapID() const;
   virtual const char *GetDescription() const;
   virtual void GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const;

   //Message Maps
   virtual catch_msg void OnCommandDoc( HCBaseDoc *pObject, HCommandPlugin *&pOutPlugin, String &strOutParameters);

   //HPluginHelper overrides
   virtual const char *GetTitleName() const;
};

class UnlockSelectedCommandUIPlugin : public LockCommandUIPlugin
{
public:
   DECLARE_COMMANDUI_PLUGINHELPER(UnlockSelectedCommandUIPlugin, LockCommandUIPlugin)

   // HCommandUIPluginHelper overrides
   virtual UINT GetBitmapID() const;
   virtual const char *GetDescription() const;
   virtual void GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const;

   //Message Maps
   virtual catch_msg void OnCommandDoc( HCBaseDoc *pObject, HCommandPlugin *&pOutPlugin, String &strOutParameters);

   //HPluginHelper overrides
   virtual const char *GetTitleName() const;
};

class UnlockAllCommandUIPlugin : public LockCommandUIPlugin
{
public:
   DECLARE_COMMANDUI_PLUGINHELPER(UnlockAllCommandUIPlugin, LockCommandUIPlugin)

   // HCommandUIPluginHelper overrides
   virtual UINT GetBitmapID() const;
   virtual const char *GetDescription() const;
   virtual void GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const;

   //Message Maps
   virtual catch_msg void OnCommandDoc( HCBaseDoc *pObject, HCommandPlugin *&pOutPlugin, String &strOutParameters);

   //HPluginHelper overrides
   virtual const char *GetTitleName() const;
};
