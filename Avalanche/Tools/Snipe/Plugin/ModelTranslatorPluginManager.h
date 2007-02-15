#pragma once

#include "ModelTranslatorPlugin.h"
#include "PluginManager.h"

class PolyModel;

class ModelTranslatorPluginManager : public PluginManager
{
public:
	         ModelTranslatorPluginManager();
	virtual ~ModelTranslatorPluginManager(); // We don't need to delete the entries, the SnipePlugin will do it for us since it points to the same entries

   PolyModel *Import();
   BOOL Export( PolyModel *pModel );

   BOOL QueryImportFormat(ModelTranslatorPlugin **pOutEntry, String &strOutFileName );
   BOOL QueryExportFormat(PolyModel *pModel, ModelTranslatorPlugin **pOutEntry, String &strOutFileName );

   // PluginManager overrides
   virtual String GetFolderName() const { return "Translators"; }
};

