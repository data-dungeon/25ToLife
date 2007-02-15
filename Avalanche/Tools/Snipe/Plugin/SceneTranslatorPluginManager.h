#pragma once

#include "SceneTranslatorPlugin.h"
#include "PluginManager.h"

class Scene;

class SceneTranslatorPluginManager : public PluginManager
{
public:
	         SceneTranslatorPluginManager();
	virtual ~SceneTranslatorPluginManager(); // We don't need to delete the entries, the SnipePlugin will do it for us since it points to the same entries

   Scene *Import();
   BOOL Export( Scene *pModel );

   BOOL QueryImportFormat(SceneTranslatorPlugin **pOutEntry, String &strOutFileName );
   BOOL QueryExportFormat(Scene *pScene, SceneTranslatorPlugin **pOutEntry, String &strOutFileName );

   // PluginManager overrides
   virtual String GetFolderName() const { return "Translators"; }
};

