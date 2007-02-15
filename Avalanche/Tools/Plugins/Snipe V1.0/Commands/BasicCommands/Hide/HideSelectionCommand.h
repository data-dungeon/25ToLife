#pragma once
#include "HCommandPluginHelper.h"

class HComponent;
class HParentableObject;
class MemoryStream;
class HSnipeObjectList;

class HideSelectionCommand : public CommandInstance
{
public:
   CList<HSnipeObject *> m_objectlist;
   CommandInstance *m_pDeselectAllCommand;

   HideSelectionCommand(HCommandPluginHelper *pPlugin);
   virtual ~HideSelectionCommand();

// PluginCommand Overrides
   virtual UndoType GetUndoType();
   virtual BOOL OnDo(StringList &strResults);
   virtual void OnRedo();
   virtual void OnUndo();
};

class HideSelectionCommandPlugin : public HCommandPluginHelper
{
public:
   DECLARE_COMMAND_PLUGINHELPER( HideSelectionCommandPlugin, HCommandPluginHelper )

   HCommandPluginHelper *m_pDeselectAllCommandPlugin;

   HideSelectionCommandPlugin() 
   {
      m_pDeselectAllCommandPlugin = NULL;
   }
//HCommandPluginHelper overrides
   virtual CommandInstance *CreateCommandInstance();
   virtual UINT GetBitmapID() const;
   virtual String GetHelp() const;
   virtual void Init(HCommandPluginManager *pCommandManager);
   virtual const char *GetCLIName() const;

//HPluginHelper overrides
   virtual const char *GetTitleName() const;
};

void SetComponentsVisibiltyToParentsVisibility(CList<HSnipeObject *> *pChangedList);
