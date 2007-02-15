#pragma once
#include "SelectBaseCommand.h"
class HSnipeObjectList;

class DeselectAllCommand : public SelectBaseCommand
{
public:
   DeselectAllCommand(HCommandPluginHelper *pPlugin) :
    SelectBaseCommand(pPlugin)
   {
   }

//PluginCommand Overrides
   virtual BOOL OnDo(StringList &strResults);
};

class DeselectAllCommandPlugin : public HCommandPluginHelper
{
public:
   DECLARE_COMMAND_PLUGINHELPER( DeselectAllCommandPlugin, HCommandPluginHelper )

//HCommandPluginHelper overrides
   virtual CommandInstance *CreateCommandInstance();
   virtual UINT GetBitmapID() const;
   virtual String GetHelp() const;
   virtual const char *GetCLIName() const;

//HPluginHelper overrides
   virtual const char *GetTitleName() const;
};
