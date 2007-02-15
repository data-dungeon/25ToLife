//
#include "stdafx.h"
#include "HCommandPluginManager.h"
#include "CommandPluginManager.h"

HCommandPlugin *HCommandPluginManager::FindCommandByClassName(const char *szPluginName, const char *szClassName)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HCommandPlugin *) ((CommandPluginManager *)this)->FindCommandByClassName( szPluginName, szClassName );
}

HCommandPlugin *HCommandPluginManager::FindCommandByCLIName(const char *szPluginName, const char *szCLIName)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HCommandPlugin *) ((CommandPluginManager *)this)->FindCommandByCLIName( szPluginName, szCLIName );
}

POSITION HCommandPluginManager::GetHeadPosition()
{
   return ((CommandPluginManager *)this)->GetPluginList()->GetHeadPosition();
}

HCommandPlugin *HCommandPluginManager::GetNext(POSITION pos)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HCommandPlugin *) ((CommandPluginManager *)this)->GetPluginList()->GetNext(pos);
}
