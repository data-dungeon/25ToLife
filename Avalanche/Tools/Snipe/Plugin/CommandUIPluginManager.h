#pragma once

#include "SDK/Enum.h"
#include "PluginManager.h"

class CGLToolBarControl;
class CommandUIPlugin;

typedef CList<CommandUIPlugin *> CommandUIPluginList;
class CMainFrame;

class CommandUIPluginManager : public PluginManager
{
private:
   static CommandUIPluginList m_allcommanduilist; // This contains derived classes like Manipulators
   static CArray<CommandUIPluginList, CommandUIPluginList *> m_sotarray;
   static CArray<CommandUIPluginList, CommandUIPluginList *> m_svtarray;
   static CArray<CommandUIPluginList, CommandUIPluginList *> m_sdtarray;
   static CommandUIPluginList m_mainframelist;
   static BOOL m_bHasInit;
public:
	         CommandUIPluginManager();
	virtual ~CommandUIPluginManager(); // We don't need to delete the plugins, the SnipePlugin will do it for us since it points to the same plugins

   CommandUIPluginList *GetCommandUIPluginListObject(SnipeObjectType nSOTType);
   CommandUIPluginList *GetCommandUIPluginListView(SnipeViewType nSVTType);
   CommandUIPluginList *GetCommandUIPluginListDoc(SnipeDocType nSDTType);
   CommandUIPluginList *GetCommandUIPluginListMainFrame();
   CommandUIPluginList *GetCommandUIPluginListAll();

   void Remove(CommandUIPlugin *pPlugin);

   // PluginManager overrides
   virtual String GetFolderName() const { return "Commands"; }
   virtual void AddPlugin(Plugin *pPlugin);
   virtual void InitPlugins();
};

inline CommandUIPluginList *CommandUIPluginManager::GetCommandUIPluginListObject(SnipeObjectType nSOTType)
{
   ASSERT(nSOTType < SOT_NUMOBJECTS);
   return &m_sotarray[nSOTType];
}

inline CommandUIPluginList *CommandUIPluginManager::GetCommandUIPluginListView(SnipeViewType nSVTType)
{
   ASSERT(nSVTType < SVT_NUMVIEWS);
   return &m_svtarray[nSVTType];
}

inline CommandUIPluginList *CommandUIPluginManager::GetCommandUIPluginListDoc(SnipeDocType nSDTType)
{
   ASSERT(nSDTType < SDT_NUMDOCS);
   return &m_sdtarray[nSDTType];
}

inline CommandUIPluginList *CommandUIPluginManager::GetCommandUIPluginListMainFrame()
{
   return &m_mainframelist;
}

inline CommandUIPluginList *CommandUIPluginManager::GetCommandUIPluginListAll()
{
   return &m_allcommanduilist; 
}
