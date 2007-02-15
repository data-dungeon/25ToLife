#pragma once

#include "SelectUI.h"

class LassoSelectCommandUIPlugin : public SelectCommandUIPlugin
{
public:
   DECLARE_COMMANDUI_PLUGINHELPER(LassoSelectCommandUIPlugin, SelectCommandUIPlugin)

//HCommandUIPluginHelper overrides
   virtual UINT GetBitmapID() const;
   virtual const char *GetDescription() const;
   virtual void GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const;

   // callbacks
   virtual catch_msg void Place(HCMainFrame *pMainFrame);

   // MainFrame Message Maps
   virtual SnipeDocType GetCommandDocType() { return SDT_CBaseDoc; }
   virtual catch_msg void OnUpdateCommandUIDoc(const HCBaseDoc *pSnipeObject, CCmdUI *pCmdUI); 
   virtual catch_msg void OnCommandDoc( HCBaseDoc *pObject, HCommandPlugin *&pOutPlugin, String &strOutParameters);

//HPluginHelper overrides
   virtual const char *GetTitleName() const;
};
