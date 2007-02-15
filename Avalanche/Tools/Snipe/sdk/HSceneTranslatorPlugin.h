#pragma once
#include "HPlugin.h"

class HScene;
typedef void (* SDK_PFN_SCENEIMPORT) (void *pPluginData, HScene *, const char *szFileName);
typedef void (* SDK_PFN_SCENEEXPORT) (void *pPluginData, HScene *, const char *szFileName);

class SNIPEEXPORT HSceneTranslatorPlugin : public HPlugin
{
public:
   void SetFilterName(const char *szFilterName);
   void SetExtension(const char *szExtension);
   void SetImportCallback(SDK_PFN_SCENEIMPORT lpfnImport);
   void SetExportCallback(SDK_PFN_SCENEEXPORT lpfnExport);
};
