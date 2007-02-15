#pragma once
#include "HPlugin.h"

class HPolyModel;
typedef void (* SDK_PFN_MODELIMPORT) (void *pPluginData, HPolyModel *, const char *szFileName);
typedef void (* SDK_PFN_MODELEXPORT) (void *pPluginData, HPolyModel *, const char *szFileName);

class SNIPEEXPORT HModelTranslatorPlugin : public HPlugin
{
public:
   enum ModelType { MT_POLYMODEL, MT_PATCHMODEL, MT_SUBDMODEL };

   void SetFilterName(const char *szFilterName);
   void SetExtension(const char *szExtension);
   void SetModelType( ModelType nModelType );
   void SetImportCallback(SDK_PFN_MODELIMPORT lpfnImport);
   void SetExportCallback(SDK_PFN_MODELEXPORT lpfnExport);
};
