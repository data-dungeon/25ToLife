#pragma once

#include "HCommandUIPluginHelper.h"

class SelectRingCommandUIPlugin : public HCommandUIPluginHelper
{
protected:
    HCommandPlugin *m_pSelectRingCommandPlugin;

public:
   DECLARE_COMMANDUI_PLUGINHELPER(SelectRingCommandUIPlugin, HCommandUIPluginHelper)

//HCommandUIPluginHelper overrides
   virtual const char *GetCategory() const;
   virtual UINT GetBitmapID() const;
   virtual const char *GetDescription() const;
   virtual void GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const;
   //callbacks
   virtual catch_msg void Place(HCMainFrame *pMainFrame);
   virtual catch_msg void Init(HCommandPluginManager *pManager);

   // Message Maps
   virtual SnipeObjectType GetCommandObjectType() { return SOT_Edge; }
   virtual catch_msg void OnUpdateCommandUIObject(const HSnipeObject *pSnipeObject, CCmdUI *pCmdUI); 
   virtual catch_msg void OnCommandObject(const HSnipeObject *pObject, HCommandPlugin *&pOutPlugin, String &strOutParameters);

//HPluginHelper overrides
   virtual const char *GetTitleName() const;
};
