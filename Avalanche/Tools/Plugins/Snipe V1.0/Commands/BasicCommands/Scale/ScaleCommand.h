#pragma once
#include "HCommandPluginHelper.h"
#include "Matrix44.h"

class HComponent;
class HParentableObject;
class MemoryStream;
class HSnipeObjectList;

class ScaleCommand : public CommandInstance
{
public:
   Vector   m_vAbout;
   Vector   m_vScale;
   Matrix44 m_matrix;

            ScaleCommand(HCommandPluginHelper *pPlugin);
   virtual ~ScaleCommand();
  
//PluginCommand Overrides
   virtual BOOL ParseToken( const char *szToken, TokenList &tokenlist );
   virtual BOOL OnValidate( String &strOutError );
   virtual UndoType GetUndoType();
   virtual BOOL OnDo(StringList &strResults);
   virtual void OnUndo();
};

class ScaleCommandPlugin : public HCommandPluginHelper
{
public:
   DECLARE_COMMAND_PLUGINHELPER( ScaleCommandPlugin, HCommandPluginHelper )

//HCommandPluginHelper overrides
   virtual CommandInstance *CreateCommandInstance();
   virtual UINT GetBitmapID() const;
   virtual String GetHelp() const;
   virtual const char *GetCLIName() const;

//HPluginHelper overrides
   virtual const char *GetTitleName() const;
};
