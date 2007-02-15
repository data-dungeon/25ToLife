#pragma once

#include "HCommandUIPluginHelper.h"

class DeselectAllCommandUIPlugin : public HCommandUIPluginHelper
{
protected:
    HCommandPlugin *m_pDeselectAllCommandPlugin;

public:
   DECLARE_COMMANDUI_PLUGINHELPER(DeselectAllCommandUIPlugin, HCommandUIPluginHelper)

//HCommandUIPluginHelper overrides
   virtual const char *GetCategory() const;
   virtual UINT GetBitmapID() const;
   virtual const char *GetDescription() const;
   virtual void GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const;
   //callbacks
   virtual catch_msg void Place(HCMainFrame *pMainFrame);
   virtual catch_msg void Init(HCommandPluginManager *pManager);

   // Message Maps
   virtual SnipeDocType GetCommandDocType() { return SDT_CBaseDoc; }
   virtual catch_msg void OnUpdateCommandUIDoc(const HCBaseDoc *pSnipeObject, CCmdUI *pCmdUI); 
   virtual catch_msg void OnCommandDoc( HCBaseDoc *pObject, HCommandPlugin *&pOutPlugin, String &strOutParameters);

//HPluginHelper overrides
   virtual const char *GetTitleName() const;
};
