#pragma once

#include "HCommandUIPluginHelper.h"

class DeleteFaceCommandUIPlugin : public HCommandUIPluginHelper
{
protected:
   HCommandPlugin *m_pDeleteFaceCommandPlugin;
public:
   DECLARE_COMMANDUI_PLUGINHELPER(DeleteFaceCommandUIPlugin, HCommandUIPluginHelper)

   // HCommandUIPluginHelper overrides
   virtual const char *GetCategory() const;

   virtual UINT GetBitmapID() const;
   virtual const char *GetDescription() const;
   virtual void Init(HCommandPluginManager *pCommandManager);

   //callbacks
   virtual catch_msg void Place(HCMainFrame *pMainFrame);

   //Message Maps
   virtual SnipeObjectType GetCommandObjectType() { return SOT_Poly; }
   virtual catch_msg void OnUpdateCommandUIObject(const HSnipeObject *pSnipeObject, CCmdUI *pCmdUI); 
   virtual catch_msg void OnCommandObject(const HSnipeObject *pSnipeObject, HCommandPlugin *&pOutPlugin, String &strOutParameters);

   //HPluginHelper overrides
   virtual const char *GetTitleName() const;
};
