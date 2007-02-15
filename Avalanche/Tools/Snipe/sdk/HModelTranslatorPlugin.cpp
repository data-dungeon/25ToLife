//
#include "stdafx.h"
#include "ModelTranslatorPlugin.h"
#include "HModelTranslatorPlugin.h"

void HModelTranslatorPlugin::SetFilterName(const char *szFilterName)
{
   ((ModelTranslatorPlugin *)this)->SetFilterName(szFilterName);
}

void HModelTranslatorPlugin::SetExtension(const char *szExtension)
{
   ((ModelTranslatorPlugin *)this)->SetExtension(szExtension);
}

void HModelTranslatorPlugin::SetModelType(HModelTranslatorPlugin::ModelType nModelType)
{
   ((ModelTranslatorPlugin *)this)->SetModelType((ModelTranslatorPlugin::ModelType)nModelType);
}

void HModelTranslatorPlugin::SetImportCallback(SDK_PFN_MODELIMPORT lpfnImport)
{
   ((ModelTranslatorPlugin *)this)->SetImportCallback((PFN_MODELIMPORT)lpfnImport);
}

void HModelTranslatorPlugin::SetExportCallback(SDK_PFN_MODELEXPORT lpfnExport)
{
   ((ModelTranslatorPlugin *)this)->SetExportCallback((PFN_MODELEXPORT)lpfnExport);
}