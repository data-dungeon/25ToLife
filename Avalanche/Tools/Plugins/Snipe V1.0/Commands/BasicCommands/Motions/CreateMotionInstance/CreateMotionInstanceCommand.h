#pragma once

#include "HCommandPluginHelper.h"
#include "HMotion.h"

class HSceneObject;

class CreateMotionInstanceCommand : public CommandInstance
{
public:
   enum Operation { OP_NEW, OP_REPLACE, OP_ADD, OP_SUBTRACT, OP_TOGGLE };

   String                m_strName;
   HSceneObject         *m_pSceneObject;
   HMotion              *m_pMotion;
   HMotionInstance      *m_pMotionInstance;
   HMotionInstance::HDeleteInfo *m_pMotionInstanceDeleteInfo;
   CommandInstance      *m_pSelectCommandInstance;

            CreateMotionInstanceCommand( HCommandPluginHelper *pPlugin );
   virtual ~CreateMotionInstanceCommand();

//PluginCommand Overrides
   virtual void OnParse( const String &strParameters );
   virtual BOOL ParseToken( const char *szToken, TokenList &tokenlist );
   virtual BOOL OnValidate( String &strOutError );
   virtual UndoType GetUndoType();
   virtual BOOL OnDo(StringList &strResults);
   virtual void OnRedo();
   virtual void OnUndo();
};

class CreateMotionInstanceCommandPlugin : public HCommandPluginHelper
{
public:
   DECLARE_COMMAND_PLUGINHELPER( CreateMotionInstanceCommandPlugin, HCommandPluginHelper )

   HCommandPlugin *m_pSelectCommandPlugin;

//HCommandPluginHelper overrides
   virtual catch_msg void Init( HCommandPluginManager *pManager );
   virtual CommandInstance *CreateCommandInstance();
   virtual UINT GetBitmapID() const;
   virtual String GetHelp() const;
   virtual const char *GetCLIName() const;

//HPluginHelper overrides
   virtual const char *GetTitleName() const;
};
