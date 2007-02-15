#pragma once
#include "HCommandPluginHelper.h"
#include "HMotion.h"

class ImportMotionCommand : public CommandInstance
{
private:
   HSnipeObjectList *m_pMotions;
   CList<HMotion::HDeleteInfo *> m_MotionDeleteInfos;
   StringList m_strFileNames;
   BOOL       m_bEditDocuments;

public:
   ImportMotionCommand(HCommandPluginHelper *pPlugin) :
    CommandInstance(pPlugin)
   {
      m_pMotions = NULL;
      m_bEditDocuments = FALSE;
   }
   virtual ~ImportMotionCommand();

//PluginCommand Overrides
   virtual UndoType GetUndoType();
   virtual BOOL OnDo(StringList &strResults);
   virtual void OnRedo();
   virtual void OnUndo();
   virtual BOOL ParseToken( const char *szToken, TokenList &tokenlist );
};

class ImportMotionCommandPlugin : public HCommandPluginHelper
{
public:
   DECLARE_COMMAND_PLUGINHELPER( ImportMotionCommandPlugin, HCommandPluginHelper )

//HCommandPluginHelper overrides
   virtual CommandInstance *CreateCommandInstance();
   virtual UINT GetBitmapID() const;
   virtual String GetHelp() const;
   virtual const char *GetCLIName() const;

//HPluginHelper overrides
   virtual const char *GetTitleName() const;
};
