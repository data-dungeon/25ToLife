//
#include "stdafx.h"
#include "SceneTranslatorPlugin.h"
#include "HSceneTranslatorPlugin.h"

void HSceneTranslatorPlugin::SetFilterName(const char *szFilterName)
{
   ((SceneTranslatorPlugin *)this)->SetFilterName(szFilterName);
}

void HSceneTranslatorPlugin::SetExtension(const char *szExtension)
{
   ((SceneTranslatorPlugin *)this)->SetExtension(szExtension);
}

void HSceneTranslatorPlugin::SetImportCallback(SDK_PFN_SCENEIMPORT lpfnImport)
{
   ((SceneTranslatorPlugin *)this)->SetImportCallback((PFN_SCENEIMPORT)lpfnImport);
}

void HSceneTranslatorPlugin::SetExportCallback(SDK_PFN_SCENEEXPORT lpfnExport)
{
   ((SceneTranslatorPlugin *)this)->SetExportCallback((PFN_SCENEEXPORT)lpfnExport);
}