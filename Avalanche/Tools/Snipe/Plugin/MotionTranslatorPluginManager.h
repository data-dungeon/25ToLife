#pragma once

#include "MotionTranslatorPlugin.h"
#include "PluginManager.h"
#include "SnipeObject.h"

class Motion;

class MotionTranslatorPluginManager : public PluginManager
{
public:
	         MotionTranslatorPluginManager();
	virtual ~MotionTranslatorPluginManager(); // We don't need to delete the entries, the SnipePlugin will do it for us since it points to the same entries

   SnipeObjectList *Import( StringList &strFileNames ); // pass empty string to prompt, delete list when done with it
   BOOL Export( Motion *pMotion );

   BOOL QueryImportFormat(MotionTranslatorPlugin **pOutEntry, StringList &strOutFileNames );
   BOOL QueryExportFormat(Motion *pMotion, MotionTranslatorPlugin **pOutEntry, String &strOutFileName );
   MotionTranslatorPlugin *FindEntryFromSuffix( const String &strSuffix );

   // PluginManager overrides
   virtual String GetFolderName() const { return "Translators"; }
};

