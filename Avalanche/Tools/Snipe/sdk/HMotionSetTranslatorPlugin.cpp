//
#include "stdafx.h"
#include "MotionSetTranslatorPlugin.h"
#include "HMotionSetTranslatorPlugin.h"

void HMotionSetTranslatorPlugin::SetFilterName(const char *szFilterName)
{
   ((MotionSetTranslatorPlugin *)this)->SetFilterName(szFilterName);
}

void HMotionSetTranslatorPlugin::SetExtension(const char *szExtension)
{
   ((MotionSetTranslatorPlugin *)this)->SetExtension(szExtension);
}

void HMotionSetTranslatorPlugin::SetImportCallback(SDK_PFN_MOTIONSETIMPORT lpfnImport)
{
   ((MotionSetTranslatorPlugin *)this)->SetImportCallback((PFN_MOTIONSETIMPORT)lpfnImport);
}

void HMotionSetTranslatorPlugin::SetExportCallback(SDK_PFN_MOTIONSETEXPORT lpfnExport)
{
   ((MotionSetTranslatorPlugin *)this)->SetExportCallback((PFN_MOTIONSETEXPORT)lpfnExport);
}