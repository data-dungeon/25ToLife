#pragma once

#include "SnipeManipulatorManager.h"
class NavigatorPlugin;

class SnipeNavigatorManager : public SnipeManipulatorManager
{
public:
   SnipeNavigatorManager();
   virtual ~SnipeNavigatorManager();

   void AddSnipeNavigator( NavigatorPlugin *pPlugin );
};
