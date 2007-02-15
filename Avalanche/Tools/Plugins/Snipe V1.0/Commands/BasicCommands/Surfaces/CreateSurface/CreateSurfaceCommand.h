#pragma once
#include "HCommandPluginHelper.h"
#include "HSurface.h"

class CreateSurfaceCommand : public CommandInstance
{
public:
   enum Operation { OP_NEW, OP_REPLACE, OP_ADD, OP_SUBTRACT, OP_TOGGLE };

   String      m_strName;
   HSurface   *m_pSurface;
   Operation   m_operation;
   String      m_strOldDescription;
   String      m_strNewDescription;
   HSurface::HDeleteInfo *m_pSurfaceDeleteInfo;
   CommandInstance *m_pSelectInstance;

            CreateSurfaceCommand(HCommandPluginHelper *pPlugin);
   virtual ~CreateSurfaceCommand();
  
//PluginCommand Overrides
   virtual BOOL ParseToken( const char *szToken, TokenList &tokenlist );
   virtual BOOL OnValidate( String &strOutError );
   virtual UndoType GetUndoType();
   virtual BOOL OnDo(StringList &strResults);
   virtual void OnRedo();
   virtual void OnUndo();
};

class CreateSurfaceCommandPlugin : public HCommandPluginHelper
{
public:
   DECLARE_COMMAND_PLUGINHELPER( CreateSurfaceCommandPlugin, HCommandPluginHelper )
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
