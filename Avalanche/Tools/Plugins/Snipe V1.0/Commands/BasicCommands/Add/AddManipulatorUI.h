#pragma once

#include "HCommandUIPluginHelper.h"

class AddManipulatorUIPlugin : public HCommandUIPluginHelper
{
protected:
   HCommandPlugin *m_pAddManipulatorPlugin;

public:
   DECLARE_COMMANDUI_PLUGINHELPER(AddManipulatorUIPlugin, HCommandUIPluginHelper)

   // HCommandUIPluginHelper overrides
   virtual const char *GetCategory() const;

   virtual UINT GetBitmapID() const;
   virtual const char *GetDescription() const;
//   virtual void GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const;
   virtual void Init(HCommandPluginManager *pCommandManager);

   //callbacks
   virtual catch_msg void Place(HCMainFrame *pMainFrame);

   //Message Maps
//   virtual SnipeObjectType GetCommandObjectType() { return SOT_Poly; }
//   virtual catch_msg void OnUpdateCommandUIObject(const HSnipeObject *pSnipeObject, CCmdUI *pCmdUI); 
//   virtual catch_msg void OnCommandObject(const HSnipeObject *pSnipeObject, HCommandPlugin *&pOutPlugin, String &strOutParameters);

   virtual SnipeViewType GetCommandViewType() { return SVT_CGLModelView; };
   virtual catch_msg void OnUpdateCommandUIView(const HCGLBaseView *pView, CCmdUI *pCmdUI) { pCmdUI->Enable(TRUE); }
   virtual catch_msg void OnCommandView(HCGLBaseView *pView, HCommandPlugin *&pOutPlugin, String &strOutParameters);

   //HPluginHelper overrides
   virtual const char *GetTitleName() const;
};
