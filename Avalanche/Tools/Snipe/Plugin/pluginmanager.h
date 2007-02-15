#pragma once

#include "Plugin.h"

class PluginManager
{
protected:
   PluginList m_list;

public:

   PluginManager() {}

   virtual ~PluginManager();
   PluginList *GetPluginList() { return &m_list; }

   // Overridables
   virt_base virtual void InitPlugins() {};
   virt_base virtual void AddPlugin(Plugin *pPlugin);

   // Pure
   virt_base virtual String GetFolderName() const = 0;
};