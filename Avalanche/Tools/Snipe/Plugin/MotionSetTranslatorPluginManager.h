#pragma once

#include "MotionSetTranslatorPlugin.h"
#include "PluginManager.h"

class MotionSet;

class MotionSetTranslatorPluginManager : public PluginManager
{
public:
	         MotionSetTranslatorPluginManager();
	virtual ~MotionSetTranslatorPluginManager(); // We don't need to delete the entries, the SnipePlugin will do it for us since it points to the same entries

   MotionSet *Import();
   BOOL Export( MotionSet *pMotionSet );

   BOOL QueryImportFormat(MotionSetTranslatorPlugin **pOutEntry, String &strOutFileName );
   BOOL QueryExportFormat(MotionSet *pMotionSet, MotionSetTranslatorPlugin **pOutEntry, String &strOutFileName );

   // PluginManager overrides
   virtual String GetFolderName() const { return "Translators"; }
};

