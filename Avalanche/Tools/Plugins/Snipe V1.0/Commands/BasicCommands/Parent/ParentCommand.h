#pragma once
#include "HCommandPluginHelper.h"

class HHierObject::HParentInfo;

class ParentCommand : public CommandInstance
{
protected:
   HHierObject *m_pParent;
   HHierObject *m_pChild;
   HHierObject *m_pOldParent;
   HHierObject::HParentInfo *m_pParentInfo;

public:
   ParentCommand(HCommandPluginHelper *pPlugin) :
    CommandInstance(pPlugin)
   {
      m_pParent = NULL;
      m_pChild = NULL;
      m_pOldParent = NULL;
   }
   virtual ~ParentCommand();

//PluginCommand Overrides
   BOOL ParseToken( const char *szToken, TokenList &tokenlist );
   virtual UndoType GetUndoType();
   virtual BOOL OnDo(StringList &strResults);
   virtual void OnRedo();
   virtual void OnUndo();
   virtual BOOL OnValidate( String &strOutError );
};

class ParentCommandPlugin : public HCommandPluginHelper
{
public:
   DECLARE_COMMAND_PLUGINHELPER( ParentCommandPlugin, HCommandPluginHelper )

//HCommandPluginHelper overrides
   virtual CommandInstance *CreateCommandInstance();
   virtual UINT GetBitmapID() const;
   virtual String GetHelp() const;
   virtual const char *GetCLIName() const;

//HPluginHelper overrides
   virtual const char *GetTitleName() const;
};
