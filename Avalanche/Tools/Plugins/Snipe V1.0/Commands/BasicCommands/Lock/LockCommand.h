#pragma once
#include "HCommandPluginHelper.h"

class HComponent;
class HParentableObject;
class MemoryStream;
class HSnipeObjectList;

class LockCommand : public CommandInstance
{
public:
   CList<HSnipeObject *> m_objectlist;
   BOOL                  m_bLock;

   LockCommand(HCommandPluginHelper *pPlugin);
   virtual ~LockCommand();

// PluginCommand Overrides
   virtual UndoType GetUndoType();
   virtual BOOL ParseToken( const char *szToken, TokenList &tokenlist );
   virtual BOOL OnValidate( String &strOutError );
   virtual BOOL OnDo(StringList &strResults);
   virtual void OnUndo();
};

class LockCommandPlugin : public HCommandPluginHelper
{
public:
   DECLARE_COMMAND_PLUGINHELPER( LockCommandPlugin, HCommandPluginHelper )

   HCommandPluginHelper *m_pDeselectAllCommandPlugin;

   LockCommandPlugin() 
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
