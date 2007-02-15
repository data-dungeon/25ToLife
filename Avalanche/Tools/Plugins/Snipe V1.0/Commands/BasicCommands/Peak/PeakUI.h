#pragma once

#include "HCommandUIPluginHelper.h"

class PeakEdgeCommandUIPlugin : public HCommandUIPluginHelper
{
protected:
   HCommandPlugin *m_pPeakEdgeCommandPlugin;
public:
   DECLARE_COMMANDUI_PLUGINHELPER(PeakEdgeCommandUIPlugin, HCommandUIPluginHelper)

   // HCommandUIPluginHelper overrides
   virtual const char *GetCategory() const;

   virtual UINT GetBitmapID() const;
   virtual const char *GetDescription() const;
   virtual void GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const;
   virtual void Init(HCommandPluginManager *pCommandManager);

   //callbacks
   virtual catch_msg void Place(HCMainFrame *pMainFrame);

   //Message Maps
   virtual SnipeObjectType GetCommandObjectType() { return SOT_Component; }
   virtual catch_msg void OnUpdateCommandUIObject(const HSnipeObject *pSnipeObject, CCmdUI *pCmdUI); 
   virtual catch_msg void OnCommandObject(const HSnipeObject *pSnipeObject, HCommandPlugin *&pOutPlugin, String &strOutParameters);

   //HPluginHelper overrides
   virtual const char *GetTitleName() const;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////

class SmoothEdgeCommandUIPlugin : public HCommandUIPluginHelper
{
protected:
   HCommandPlugin *m_pPeakEdgeCommandPlugin;
public:
   DECLARE_COMMANDUI_PLUGINHELPER(SmoothEdgeCommandUIPlugin, HCommandUIPluginHelper)

   // HCommandUIPluginHelper overrides
   virtual const char *GetCategory() const;

   virtual UINT GetBitmapID() const;
   virtual const char *GetDescription() const;
   virtual void GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const;
   virtual void Init(HCommandPluginManager *pCommandManager);

   //callbacks
   virtual catch_msg void Place(HCMainFrame *pMainFrame);

   //Message Maps
   virtual SnipeObjectType GetCommandObjectType() { return SOT_Component; }
   virtual catch_msg void OnUpdateCommandUIObject(const HSnipeObject *pSnipeObject, CCmdUI *pCmdUI); 
   virtual catch_msg void OnCommandObject(const HSnipeObject *pSnipeObject, HCommandPlugin *&pOutPlugin, String &strOutParameters);

   //HPluginHelper overrides
   virtual const char *GetTitleName() const;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////

class TogglePeakEdgeCommandUIPlugin : public HCommandUIPluginHelper
{
protected:
   HCommandPlugin *m_pPeakEdgeCommandPlugin;
public:
   DECLARE_COMMANDUI_PLUGINHELPER(TogglePeakEdgeCommandUIPlugin, HCommandUIPluginHelper)

   // HCommandUIPluginHelper overrides
   virtual const char *GetCategory() const;

   virtual UINT GetBitmapID() const;
   virtual const char *GetDescription() const;
//   virtual void GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const;
   virtual void Init(HCommandPluginManager *pCommandManager);

   //callbacks
   virtual catch_msg void Place(HCMainFrame *pMainFrame);

   //Message Maps
   virtual SnipeObjectType GetCommandObjectType() { return SOT_Component; }
   virtual catch_msg void OnUpdateCommandUIObject(const HSnipeObject *pSnipeObject, CCmdUI *pCmdUI); 
   virtual catch_msg void OnCommandObject(const HSnipeObject *pSnipeObject, HCommandPlugin *&pOutPlugin, String &strOutParameters);

   //HPluginHelper overrides
   virtual const char *GetTitleName() const;
};
