#pragma once
#include "HCommandUIPluginHelper.h"

class SelectCommandUIPlugin : public HCommandUIPluginHelper
{
public:
   DECLARE_COMMANDUI_PLUGINHELPER(SelectCommandUIPlugin, HCommandUIPluginHelper)

// HCommandUIPluginHelper overrides
   virtual const char *GetCategory() const;
};