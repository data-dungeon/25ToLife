#pragma once

#include "HCommandPluginHelper.h"
#include "HRotoscope.h"
class HSnipeObjectList;

class CreateRotoscopeCommand : public CommandInstance
{
public:
   enum Operation { OP_NEW, OP_REPLACE, OP_ADD, OP_SUBTRACT, OP_TOGGLE };

   String           m_strName;
   HRotoscope      *m_pRotoscope;
   HRotoscope::HDeleteInfo *m_pRotoscopeDeleteInfo;
   HHierObject     *m_pObject;
   HImage          *m_pImage;
   CommandInstance *m_pSelectInstance;

            CreateRotoscopeCommand( HCommandPluginHelper *pPlugin );
   virtual ~CreateRotoscopeCommand();
  
//PluginCommand Overrides
   virtual BOOL ParseToken( const char *szToken, TokenList &tokenlist );
   virtual BOOL OnValidate( String &strOutError );
   virtual UndoType GetUndoType();
   virtual BOOL OnDo(StringList &strResults);
   virtual void OnRedo();
   virtual void OnUndo();
};

class CreateRotoscopeCommandPlugin : public HCommandPluginHelper
{
public:
   DECLARE_COMMAND_PLUGINHELPER( CreateRotoscopeCommandPlugin, HCommandPluginHelper )
   HCommandPlugin *m_pSelectCommandPlugin;

//HCommandPluginHelper overrides
   virtual CommandInstance *CreateCommandInstance();
   virtual UINT GetBitmapID() const;
   virtual String GetHelp() const;
   virtual const char *GetCLIName() const;
   virtual catch_msg void Init( HCommandPluginManager *pManager );

//HPluginHelper overrides
   virtual const char *GetTitleName() const;
};
