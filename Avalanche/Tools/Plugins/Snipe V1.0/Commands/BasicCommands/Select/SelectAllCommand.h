#pragma once
#include "SelectBaseCommand.h"

class HComponent;
class HParentableObject;
class MemoryStream;
class HSnipeObjectList;

class SelectAllCommand : public SelectBaseCommand
{
public:
   SnipeObjectType m_nSOT;

   SelectAllCommand(HCommandPluginHelper *pPlugin);
  
//PluginCommand Overrides
   virtual BOOL OnDo(StringList &strResults);
};

class SelectAllCommandPlugin : public HCommandPluginHelper
{
public:
   DECLARE_COMMAND_PLUGINHELPER( SelectAllCommandPlugin, HCommandPluginHelper )

//HCommandPluginHelper overrides
   virtual CommandInstance *CreateCommandInstance();
   virtual UINT GetBitmapID() const;
   virtual String GetHelp() const;
   virtual const char *GetCLIName() const;

//HPluginHelper overrides
   virtual const char *GetTitleName() const;
};
