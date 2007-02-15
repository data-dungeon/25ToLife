#pragma once
#include "HCommandPluginHelper.h"

class SnipeObject;
class HHierObject::HConvertInfo;

class ConvertModelCommand : public CommandInstance
{
private:
   SnipeObjectType m_nConvertTo;
   HHierObject *m_pFrom;
   HHierObject::HConvertInfo *m_pConvertInfo;

public:

   ConvertModelCommand(HCommandPluginHelper *pPlugin) :
    CommandInstance(pPlugin)
   {
      m_pFrom = NULL;
      m_nConvertTo = SOT_Unknown;
   }
   virtual ~ConvertModelCommand();

//PluginCommand Overrides
   virtual UndoType GetUndoType();
   virtual BOOL OnDo(StringList &strResults);
   virtual void OnRedo();
   virtual void OnUndo();
   virtual BOOL ParseToken( const char *szToken, TokenList &tokenlist );
   virtual BOOL OnValidate( String &strOutError );
};

class ConvertModelCommandPlugin : public HCommandPluginHelper
{
public:
   DECLARE_COMMAND_PLUGINHELPER( ConvertModelCommandPlugin, HCommandPluginHelper )

//HCommandPluginHelper overrides
   virtual CommandInstance *CreateCommandInstance();
   virtual UINT GetBitmapID() const;
   virtual String GetHelp() const;
   virtual const char *GetCLIName() const;

//HPluginHelper overrides
   virtual const char *GetTitleName() const;
};
