#pragma once
#include "HCommandPluginHelper.h"
#include "HSelection.h"

class SaveSelectionCommand : public CommandInstance
{
public:
   enum Operation { OP_NEW, OP_REPLACE, OP_ADD, OP_SUBTRACT, OP_TOGGLE };

   String      m_strName;
   HSelection *m_pSelection;
   Operation   m_operation;
   String      m_strOldDescription;
   String      m_strNewDescription;
   HSelection::HDeleteInfo *m_pSelectionDeleteInfo;

            SaveSelectionCommand(HCommandPluginHelper *pPlugin);
   virtual ~SaveSelectionCommand();
  
//PluginCommand Overrides
   virtual BOOL ParseToken( const char *szToken, TokenList &tokenlist );
   virtual BOOL OnValidate( String &strOutError );
   virtual UndoType GetUndoType();
   virtual BOOL OnDo(StringList &strResults);
   virtual void OnRedo();
   virtual void OnUndo();
};

class SaveSelectionCommandPlugin : public HCommandPluginHelper
{
public:
   DECLARE_COMMAND_PLUGINHELPER( SaveSelectionCommandPlugin, HCommandPluginHelper )

//HCommandPluginHelper overrides
   virtual CommandInstance *CreateCommandInstance();
   virtual UINT GetBitmapID() const;
   virtual String GetHelp() const;
   virtual const char *GetCLIName() const;

//HPluginHelper overrides
   virtual const char *GetTitleName() const;
};
