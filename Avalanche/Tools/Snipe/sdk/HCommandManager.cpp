//
#include "stdafx.h"
#include "HCommandManager.h"
#include "CommandManager.h"

HCommandEntry *HCommandManager::FindCommand(const char *szPluginName, const char *szClassName)
{
   return (HCommandEntry *) ((CommandManager *)this)->FindCommand( szPluginName, szClassName );
}

POSITION HCommandManager::GetHeadPosition()
{
   return ((CommandManager *)this)->GetHeadPosition();
}

HCommandEntry *HCommandManager::GetNext(POSITION pos)
{
   return (HCommandEntry *) ((CommandManager *)this)->GetNext(pos);
}