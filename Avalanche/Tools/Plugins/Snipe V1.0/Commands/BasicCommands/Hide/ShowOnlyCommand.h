#pragma once
#include "HCommandPluginHelper.h"

class HComponent;
class HParentableObject;
class MemoryStream;
class HSnipeObjectList;

class ShowOnlyCommand : public CommandInstance
{
public:
   HSnipeObjectList *m_pVertexList;
   CList<HSnipeObject *> m_objectlist;

   ShowOnlyCommand(HCommandPluginHelper *pPlugin);
   virtual ~ShowOnlyCommand();

// PluginCommand Overrides
   virtual UndoType GetUndoType();
   virtual BOOL ParseToken( const char *szToken, TokenList &tokenlist );
   virtual BOOL OnValidate( String &strOutError );
   virtual BOOL OnDo(StringList &strResults);
   virtual void OnRedo();
   virtual void OnUndo();
};

class ShowOnlyCommandPlugin : public HCommandPluginHelper
{
public:
   DECLARE_COMMAND_PLUGINHELPER( ShowOnlyCommandPlugin, HCommandPluginHelper )

   ShowOnlyCommandPlugin() {}

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
