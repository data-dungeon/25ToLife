//
#include "stdafx.h"
#include "NavigatorPluginManager.h"

NavigatorPluginManager::NavigatorPluginManager()
{
}

// We don't need to delete the plugins, the SnipePlugin will 
// do it for us since it points to the same plugins
NavigatorPluginManager::~NavigatorPluginManager()
{
}

