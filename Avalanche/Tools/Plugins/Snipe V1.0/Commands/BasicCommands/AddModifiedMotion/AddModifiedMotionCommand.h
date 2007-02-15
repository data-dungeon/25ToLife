#pragma once
#include "HCommandPluginHelper.h"

class SnipeObject;
class HHierObject::HConvertInfo;
class HFloatMotionCurve;

class AddModifiedMotionCommand : public CommandInstance
{
private:
   HHierObject *m_pHierObject;

public:

   AddModifiedMotionCommand(HCommandPluginHelper *pPlugin) :
    CommandInstance(pPlugin)
   {
   }
   virtual ~AddModifiedMotionCommand();

//PluginCommand Overrides
   virtual UndoType GetUndoType();
   virtual BOOL OnDo(StringList &strResults);
   virtual void OnRedo();
   virtual void OnUndo();
   virtual BOOL ParseToken( const char *szToken, TokenList &tokenlist );
};

class AddModifiedMotionCommandPlugin : public HCommandPluginHelper
{
public:
   DECLARE_COMMAND_PLUGINHELPER( AddModifiedMotionCommandPlugin, HCommandPluginHelper )

//HCommandPluginHelper overrides
   virtual CommandInstance *CreateCommandInstance();
   virtual UINT GetBitmapID() const;
   virtual String GetHelp() const;
   virtual const char *GetCLIName() const;

//HPluginHelper overrides
   virtual const char *GetTitleName() const;
};
