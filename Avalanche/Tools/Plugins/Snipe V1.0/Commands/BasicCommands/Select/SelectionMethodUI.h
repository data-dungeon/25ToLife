#pragma once

#include "SelectUI.h"

class SelectionMethodCommandUIPlugin : public SelectCommandUIPlugin
{
public:
   DECLARE_COMMANDUI_PLUGINHELPER(SelectionMethodCommandUIPlugin, SelectCommandUIPlugin)
//HCommandUIPluginHelper overrides
   virtual UINT GetBitmapID() const;
   virtual const char *GetDescription() const;

   //callbacks
   virtual catch_msg void Place(HCMainFrame *pMainFrame);

   // Message Maps
   virtual SnipeDocType GetCommandDocType() { return SDT_CBaseDoc; }
   virtual catch_msg void OnUpdateCommandUIDoc(const HCBaseDoc *pSnipeObject, CCmdUI *pCmdUI); 
   virtual catch_msg void OnCommandDoc( HCBaseDoc *pObject, HCommandPlugin *&pOutPlugin, String &strOutParameters);

//HPluginHelper overrides
   virtual const char *GetTitleName() const;
};
