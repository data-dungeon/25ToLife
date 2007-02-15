#pragma once
#include "HCommandPluginHelper.h"
#include "../Utility/ATime.h"

class HSnipeObjectList;

class StoreValueCommand : public CommandInstance
{
   HProperty *m_pProperty;
   String m_strValue;
   Time m_time;

public:
   StoreValueCommand(HCommandPluginHelper *pPlugin) :
    CommandInstance(pPlugin),
    m_time(TAGGEDTIME)
   {
      m_pProperty = NULL;
   }
   virtual ~StoreValueCommand();

// CommandInstance overrides
   virtual BOOL ParseToken( const char *szToken, TokenList &tokenlist );

//PluginCommand overrides
   virtual UndoType GetUndoType();
   virtual BOOL OnValidate( String &strOutError );
   virtual BOOL OnDo( StringList &strResults );
   virtual void OnUndo();
};

class StoreValueCommandPlugin : public HCommandPluginHelper
{
public:
   DECLARE_COMMAND_PLUGINHELPER( StoreValueCommandPlugin, HCommandPluginHelper )

//HCommandPluginHelper overrides
   virtual CommandInstance *CreateCommandInstance();
   virtual UINT GetBitmapID() const;
   virtual String GetHelp() const;
   virtual const char *GetCLIName() const;

//HPluginHelper overrides
   virtual const char *GetTitleName() const;
};
