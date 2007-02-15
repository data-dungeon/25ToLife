#pragma once
#include "HCommandPluginHelper.h"
class HSnipeObjectList;

class RotateFocusCommand : public CommandInstance
{
private:
   BOOL m_bRotateTowardsPrevious;

   HSnipeObject *m_pPrevFocusObject;
   HSnipeObject *m_pPrevNearestFocusObject;

public:
   RotateFocusCommand(HCommandPluginHelper *pPlugin) :
    CommandInstance(pPlugin)
   {
      m_bRotateTowardsPrevious = FALSE;
      m_pPrevFocusObject = NULL;
      m_pPrevNearestFocusObject = NULL;
   }

//PluginCommand Overrides
   virtual void OnParse( const String &strParameters );
   virtual BOOL ParseToken( const char *szToken, TokenList &tokenlist );
   virtual UndoType GetUndoType();
   virtual BOOL OnDo(StringList &strResults);
   virtual void OnUndo();
};

class RotateFocusCommandPlugin : public HCommandPluginHelper
{
public:
   DECLARE_COMMAND_PLUGINHELPER( RotateFocusCommandPlugin, HCommandPluginHelper )

//HCommandPluginHelper overrides
   virtual CommandInstance *CreateCommandInstance();
   virtual UINT GetBitmapID() const;
   virtual String GetHelp() const;
   virtual const char *GetCLIName() const;

//HPluginHelper overrides
   virtual const char *GetTitleName() const;
};
