#pragma once
#include "HCommandPluginHelper.h"
#include "HMotionCurveDriver.h"

class SnipeObject;
class HHierObject::HConvertInfo;
class HFloatMotionCurve;

class ReduceMotionKeysCommand : public CommandInstance
{
private:
   float m_fErrorTolerance;
   HHierObject *m_pHierObject;
   CArray< HMotionCurveDriver::HRebuildMotionCurveDriverInfo * > m_rebuildmotioncurvedriverinfos;

public:

   ReduceMotionKeysCommand(HCommandPluginHelper *pPlugin) :
    CommandInstance(pPlugin)
   {
   }
   virtual ~ReduceMotionKeysCommand();

//PluginCommand Overrides
   virtual UndoType GetUndoType();
   virtual BOOL OnDo(StringList &strResults);
   virtual void OnRedo();
   virtual void OnUndo();
   virtual BOOL ParseToken( const char *szToken, TokenList &tokenlist );
};

class ReduceMotionKeysCommandPlugin : public HCommandPluginHelper
{
public:
   DECLARE_COMMAND_PLUGINHELPER( ReduceMotionKeysCommandPlugin, HCommandPluginHelper )

//HCommandPluginHelper overrides
   virtual CommandInstance *CreateCommandInstance();
   virtual UINT GetBitmapID() const;
   virtual String GetHelp() const;
   virtual const char *GetCLIName() const;

//HPluginHelper overrides
   virtual const char *GetTitleName() const;
};
