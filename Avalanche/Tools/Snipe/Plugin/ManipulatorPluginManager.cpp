//
#include "stdafx.h"
#include "ManipulatorPluginManager.h"

ManipulatorPluginManager::ManipulatorPluginManager()
{
}

// We don't need to delete the plugins, the SnipePlugin will 
// do it for us since it points to the same plugins
ManipulatorPluginManager::~ManipulatorPluginManager()
{
}