//

#include "stdafx.h"
#include "PluginManager.h"

PluginManager::~PluginManager()
{
}

void PluginManager::AddPlugin(Plugin *pPlugin)
{
   m_list.AddTail(pPlugin);
}
