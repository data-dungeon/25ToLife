#pragma once

#include "HCommandPluginHelper.h"
#include "HProjectionMapSurfaceDriver.h"

class CreateProjectionMapCommand : public CommandInstance
{
public:
   enum Operation { OP_NEW, OP_REPLACE, OP_ADD, OP_SUBTRACT, OP_TOGGLE };

   String       m_strName;
   HProjectionMapSurfaceDriver *m_pProjectionMap;
   HProjectionMapSurfaceDriver::HDeleteInfo *m_pProjectionMapDeleteInfo;
   HProperty   *m_pSurfaceProperty;
   HImage      *m_pImage;
   CommandInstance *m_pSelectInstance;

            CreateProjectionMapCommand(HCommandPluginHelper *pPlugin);
   virtual ~CreateProjectionMapCommand();
  
//PluginCommand Overrides
   virtual BOOL ParseToken( const char *szToken, TokenList &tokenlist );
   virtual BOOL OnValidate( String &strOutError );
   virtual UndoType GetUndoType();
   virtual BOOL OnDo(StringList &strResults);
   virtual void OnRedo();
   virtual void OnUndo();
};

class CreateProjectionMapCommandPlugin : public HCommandPluginHelper
{
public:
   DECLARE_COMMAND_PLUGINHELPER( CreateProjectionMapCommandPlugin, HCommandPluginHelper )
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
