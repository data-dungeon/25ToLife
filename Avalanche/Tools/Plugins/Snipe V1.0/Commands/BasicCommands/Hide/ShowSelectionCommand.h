#pragma once
#include "HCommandPluginHelper.h"

class HComponent;
class HParentableObject;
class MemoryStream;
class HSnipeObjectList;

class ShowSelectionCommand : public CommandInstance
{
public:
   CList<HSnipeObject *> m_objectlist;

   ShowSelectionCommand(HCommandPluginHelper *pPlugin) :
    CommandInstance(pPlugin)
   {
   }
   virtual ~ShowSelectionCommand();
  
//PluginCommand Overrides
   virtual UndoType GetUndoType();
   virtual BOOL OnDo(StringList &strResults);
   virtual void OnUndo();
};

class ShowSelectionCommandPlugin : public HCommandPluginHelper
{
public:
   DECLARE_COMMAND_PLUGINHELPER( ShowSelectionCommandPlugin, HCommandPluginHelper )

//HCommandPluginHelper overrides
   virtual CommandInstance *CreateCommandInstance();
   virtual UINT GetBitmapID() const;
   virtual String GetHelp() const;
   virtual const char *GetCLIName() const;

//HPluginHelper overrides
   virtual const char *GetTitleName() const;
};
