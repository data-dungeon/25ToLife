#pragma once

#include "HCommandUIPluginHelper.h"

class ImportMotionCommandUIPlugin : public HCommandUIPluginHelper
{
protected:
    HCommandPlugin *m_pImportMotionCommandPlugin;

public:
   DECLARE_COMMANDUI_PLUGINHELPER(ImportMotionCommandUIPlugin, HCommandUIPluginHelper)

// HCommandUIPluginHelper overrides
   virtual const char *GetCategory() const;

   virtual UINT GetBitmapID() const;
   virtual const char *GetDescription() const;
   virtual void Init(HCommandPluginManager *pCommandManager);
   virtual catch_msg void Place(HCMainFrame *pMainFrame);

   // Object Message Maps
   virtual SnipeObjectType GetCommandObjectType();
   virtual catch_msg void OnUpdateCommandUIObject(const HSnipeObject *pObject, CCmdUI *pCmdUI);
   virtual catch_msg void OnCommandObject(const HSnipeObject *pObject, HCommandPlugin *&pOutPlugin, String &strOutParameters);

   // MainFrame Message Maps
   virtual BOOL HasMainFrameCommand() { return TRUE; } 
   virtual catch_msg void OnUpdateCommandUIMainFrame(const HCMainFrame *pMainFrame, CCmdUI *pCmdUI) { pCmdUI->Enable(TRUE); }
   virtual catch_msg void OnCommandMainFrame(HCMainFrame *pMainFrame, HCommandPlugin *&pOutPlugin, String &strOutParameters);

//HPluginHelper overrides
   virtual const char *GetTitleName() const;
};
