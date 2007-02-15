#pragma once

#include "HCommandPluginHelper.h"
#include "HClampMapSurfaceDriver.h"

class CreateClampMapCommand : public CommandInstance
{
public:
   enum Operation { OP_NEW, OP_REPLACE, OP_ADD, OP_SUBTRACT, OP_TOGGLE };

   String       m_strName;
   HClampMapSurfaceDriver *m_pClampMap;
   HClampMapSurfaceDriver::HDeleteInfo *m_pClampMapDeleteInfo;
   HProperty   *m_pSurfaceProperty;
   HImage      *m_pImage;
   CommandInstance *m_pSelectInstance;

            CreateClampMapCommand(HCommandPluginHelper *pPlugin);
   virtual ~CreateClampMapCommand();
  
//PluginCommand Overrides
   virtual BOOL ParseToken( const char *szToken, TokenList &tokenlist );
   virtual BOOL OnValidate( String &strOutError );
   virtual UndoType GetUndoType();
   virtual BOOL OnDo(StringList &strResults);
   virtual void OnRedo();
   virtual void OnUndo();
};

class CreateClampMapCommandPlugin : public HCommandPluginHelper
{
public:
   DECLARE_COMMAND_PLUGINHELPER( CreateClampMapCommandPlugin, HCommandPluginHelper )
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
