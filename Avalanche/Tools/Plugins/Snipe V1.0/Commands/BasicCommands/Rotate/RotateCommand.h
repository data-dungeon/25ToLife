#pragma once
#include "HCommandPluginHelper.h"
#include "Matrix44.h"
#include "Rotate.h"

class HComponent;
class HParentableObject;
class MemoryStream;
class HSnipeObjectList;

class RotateCommand : public CommandInstance
{
public:
   Vector      m_vAbout;
   Vector      m_vAxis;
   float       m_fAngle;
   RotateEuler m_euler;
   Matrix44    m_matrix;
   UINT        m_bEuler;

            RotateCommand(HCommandPluginHelper *pPlugin);
   virtual ~RotateCommand();
  
//PluginCommand Overrides
   virtual BOOL ParseToken( const char *szToken, TokenList &tokenlist );
   virtual BOOL OnValidate( String &strOutError );
   virtual UndoType GetUndoType();
   virtual BOOL OnDo(StringList &strResults);
   virtual void OnUndo();
};

class RotateCommandPlugin : public HCommandPluginHelper
{
public:
   DECLARE_COMMAND_PLUGINHELPER( RotateCommandPlugin, HCommandPluginHelper )

//HCommandPluginHelper overrides
   virtual CommandInstance *CreateCommandInstance();
   virtual UINT GetBitmapID() const;
   virtual String GetHelp() const;
   virtual const char *GetCLIName() const;

//HPluginHelper overrides
   virtual const char *GetTitleName() const;
};
