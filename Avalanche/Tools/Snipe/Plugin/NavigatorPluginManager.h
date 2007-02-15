#pragma once

#include "ManipulatorPluginManager.h"

class NavigatorPluginManager : public ManipulatorPluginManager
{
public:
	         NavigatorPluginManager();
	virtual ~NavigatorPluginManager();

   //PluginManager Overrides
   virtual String GetFolderName() const { return "Navigators"; }
};

