#pragma once

#include "ActionTranslatorPlugin.h"
#include "PluginManager.h"

class Action;

class ActionTranslatorPluginManager : public PluginManager
{
public:
	         ActionTranslatorPluginManager();
	virtual ~ActionTranslatorPluginManager(); // We don't need to delete the entries, the SnipePlugin will do it for us since it points to the same entries

   Action *Import();
   BOOL Export( Action *pAction );

   BOOL QueryImportFormat(ActionTranslatorPlugin **pOutEntry, String &strOutFileName );
   BOOL QueryExportFormat(Action *pAction, ActionTranslatorPlugin **pOutEntry, String &strOutFileName );

   // PluginManager overrides
   virtual String GetFolderName() const { return "Action Translators"; }
};

