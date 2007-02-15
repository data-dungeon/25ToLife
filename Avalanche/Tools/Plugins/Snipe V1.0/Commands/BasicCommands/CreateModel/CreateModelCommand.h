#pragma once

#include "HCommandPluginHelper.h"

class CreateModelCommand : public CommandInstance
{
public:
   enum ModelType { MT_POLY, MT_PATCH, MT_SPLINE };

   String m_strName;
   ModelType m_nModelType;
   HPolyModel *m_pModel;
   HSnipeObject::HDeleteInfo *m_pDeleteInfo;
   CommandInstance *m_pSelectInstance;
   BOOL m_bEdit;

            CreateModelCommand(HCommandPluginHelper *pPlugin);
   virtual ~CreateModelCommand();
  
//PluginCommand Overrides
   virtual BOOL ParseToken( const char *szToken, TokenList &tokenlist );
   virtual BOOL OnValidate( String &strOutError );
   virtual UndoType GetUndoType();
   virtual BOOL OnDo(StringList &strResults);
   virtual void OnRedo();
   virtual void OnUndo();
};

class CreateModelCommandPlugin : public HCommandPluginHelper
{
public:
   DECLARE_COMMAND_PLUGINHELPER( CreateModelCommandPlugin, HCommandPluginHelper )
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
