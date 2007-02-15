#pragma once

#include "HCommandUIPluginHelper.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 2
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class SplitEdge2CommandUIPlugin : public HCommandUIPluginHelper
{
protected:
   HCommandPlugin *m_pSplitEdgeCommandPlugin;
public:
   DECLARE_COMMANDUI_PLUGINHELPER(SplitEdge2CommandUIPlugin, HCommandUIPluginHelper)

   // HCommandUIPluginHelper overrides
   virtual const char *GetCategory() const;

   virtual UINT GetBitmapID() const;
   virtual const char *GetDescription() const;
   virtual void GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const;
   virtual void Init(HCommandPluginManager *pCommandManager);

   //Message Maps
   virtual SnipeObjectType GetCommandObjectType() { return SOT_Edge; }
   virtual catch_msg void OnUpdateCommandUIObject(const HSnipeObject *pSnipeObject, CCmdUI *pCmdUI); 
   virtual catch_msg void OnCommandObject(const HSnipeObject *pSnipeObject, HCommandPlugin *&pOutPlugin, String &strOutParameters);

   //HPluginHelper overrides
   virtual const char *GetTitleName() const;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 3
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class SplitEdge3CommandUIPlugin : public HCommandUIPluginHelper
{
protected:
   HCommandPlugin *m_pSplitEdgeCommandPlugin;
public:
   DECLARE_COMMANDUI_PLUGINHELPER(SplitEdge3CommandUIPlugin, HCommandUIPluginHelper)

   // HCommandUIPluginHelper overrides
   virtual const char *GetCategory() const;

   virtual UINT GetBitmapID() const;
   virtual const char *GetDescription() const;
   virtual void GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const;
   virtual void Init(HCommandPluginManager *pCommandManager);

   //Message Maps
   virtual SnipeObjectType GetCommandObjectType() { return SOT_Edge; }
   virtual catch_msg void OnUpdateCommandUIObject(const HSnipeObject *pSnipeObject, CCmdUI *pCmdUI); 
   virtual catch_msg void OnCommandObject(const HSnipeObject *pSnipeObject, HCommandPlugin *&pOutPlugin, String &strOutParameters);

   //HPluginHelper overrides
   virtual const char *GetTitleName() const;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 4
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class SplitEdge4CommandUIPlugin : public HCommandUIPluginHelper
{
protected:
   HCommandPlugin *m_pSplitEdgeCommandPlugin;
public:
   DECLARE_COMMANDUI_PLUGINHELPER(SplitEdge4CommandUIPlugin, HCommandUIPluginHelper)

   // HCommandUIPluginHelper overrides
   virtual const char *GetCategory() const;

   virtual UINT GetBitmapID() const;
   virtual const char *GetDescription() const;
   virtual void GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const;
   virtual void Init(HCommandPluginManager *pCommandManager);

   //Message Maps
   virtual SnipeObjectType GetCommandObjectType() { return SOT_Edge; }
   virtual catch_msg void OnUpdateCommandUIObject(const HSnipeObject *pSnipeObject, CCmdUI *pCmdUI); 
   virtual catch_msg void OnCommandObject(const HSnipeObject *pSnipeObject, HCommandPlugin *&pOutPlugin, String &strOutParameters);

   //HPluginHelper overrides
   virtual const char *GetTitleName() const;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 5
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class SplitEdge5CommandUIPlugin : public HCommandUIPluginHelper
{
protected:
   HCommandPlugin *m_pSplitEdgeCommandPlugin;
public:
   DECLARE_COMMANDUI_PLUGINHELPER(SplitEdge5CommandUIPlugin, HCommandUIPluginHelper)

   // HCommandUIPluginHelper overrides
   virtual const char *GetCategory() const;

   virtual UINT GetBitmapID() const;
   virtual const char *GetDescription() const;
   virtual void GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const;
   virtual void Init(HCommandPluginManager *pCommandManager);

   //Message Maps
   virtual SnipeObjectType GetCommandObjectType() { return SOT_Edge; }
   virtual catch_msg void OnUpdateCommandUIObject(const HSnipeObject *pSnipeObject, CCmdUI *pCmdUI); 
   virtual catch_msg void OnCommandObject(const HSnipeObject *pSnipeObject, HCommandPlugin *&pOutPlugin, String &strOutParameters);

   //HPluginHelper overrides
   virtual const char *GetTitleName() const;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 6
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class SplitEdge6CommandUIPlugin : public HCommandUIPluginHelper
{
protected:
   HCommandPlugin *m_pSplitEdgeCommandPlugin;
public:
   DECLARE_COMMANDUI_PLUGINHELPER(SplitEdge6CommandUIPlugin, HCommandUIPluginHelper)

   // HCommandUIPluginHelper overrides
   virtual const char *GetCategory() const;

   virtual UINT GetBitmapID() const;
   virtual const char *GetDescription() const;
   virtual void GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const;
   virtual void Init(HCommandPluginManager *pCommandManager);

   //Message Maps
   virtual SnipeObjectType GetCommandObjectType() { return SOT_Edge; }
   virtual catch_msg void OnUpdateCommandUIObject(const HSnipeObject *pSnipeObject, CCmdUI *pCmdUI); 
   virtual catch_msg void OnCommandObject(const HSnipeObject *pSnipeObject, HCommandPlugin *&pOutPlugin, String &strOutParameters);

   //HPluginHelper overrides
   virtual const char *GetTitleName() const;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 7
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class SplitEdge7CommandUIPlugin : public HCommandUIPluginHelper
{
protected:
   HCommandPlugin *m_pSplitEdgeCommandPlugin;
public:
   DECLARE_COMMANDUI_PLUGINHELPER(SplitEdge7CommandUIPlugin, HCommandUIPluginHelper)

   // HCommandUIPluginHelper overrides
   virtual const char *GetCategory() const;

   virtual UINT GetBitmapID() const;
   virtual const char *GetDescription() const;
   virtual void GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const;
   virtual void Init(HCommandPluginManager *pCommandManager);

   //Message Maps
   virtual SnipeObjectType GetCommandObjectType() { return SOT_Edge; }
   virtual catch_msg void OnUpdateCommandUIObject(const HSnipeObject *pSnipeObject, CCmdUI *pCmdUI); 
   virtual catch_msg void OnCommandObject(const HSnipeObject *pSnipeObject, HCommandPlugin *&pOutPlugin, String &strOutParameters);

   //HPluginHelper overrides
   virtual const char *GetTitleName() const;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 8
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class SplitEdge8CommandUIPlugin : public HCommandUIPluginHelper
{
protected:
   HCommandPlugin *m_pSplitEdgeCommandPlugin;
public:
   DECLARE_COMMANDUI_PLUGINHELPER(SplitEdge8CommandUIPlugin, HCommandUIPluginHelper)

   // HCommandUIPluginHelper overrides
   virtual const char *GetCategory() const;

   virtual UINT GetBitmapID() const;
   virtual const char *GetDescription() const;
   virtual void GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const;
   virtual void Init(HCommandPluginManager *pCommandManager);

   //Message Maps
   virtual SnipeObjectType GetCommandObjectType() { return SOT_Edge; }
   virtual catch_msg void OnUpdateCommandUIObject(const HSnipeObject *pSnipeObject, CCmdUI *pCmdUI); 
   virtual catch_msg void OnCommandObject(const HSnipeObject *pSnipeObject, HCommandPlugin *&pOutPlugin, String &strOutParameters);

   //HPluginHelper overrides
   virtual const char *GetTitleName() const;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 9
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class SplitEdge9CommandUIPlugin : public HCommandUIPluginHelper
{
protected:
   HCommandPlugin *m_pSplitEdgeCommandPlugin;
public:
   DECLARE_COMMANDUI_PLUGINHELPER(SplitEdge9CommandUIPlugin, HCommandUIPluginHelper)

   // HCommandUIPluginHelper overrides
   virtual const char *GetCategory() const;

   virtual UINT GetBitmapID() const;
   virtual const char *GetDescription() const;
   virtual void GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const;
   virtual void Init(HCommandPluginManager *pCommandManager);

   //Message Maps
   virtual SnipeObjectType GetCommandObjectType() { return SOT_Edge; }
   virtual catch_msg void OnUpdateCommandUIObject(const HSnipeObject *pSnipeObject, CCmdUI *pCmdUI); 
   virtual catch_msg void OnCommandObject(const HSnipeObject *pSnipeObject, HCommandPlugin *&pOutPlugin, String &strOutParameters);

   //HPluginHelper overrides
   virtual const char *GetTitleName() const;
};
