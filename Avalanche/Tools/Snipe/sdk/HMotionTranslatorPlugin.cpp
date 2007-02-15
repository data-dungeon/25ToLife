//
#include "stdafx.h"
#include "MotionTranslatorPlugin.h"
#include "HMotionTranslatorPlugin.h"

void HMotionTranslatorPlugin::SetFilterName(const char *szFilterName)
{
   ((MotionTranslatorPlugin *)this)->SetFilterName(szFilterName);
}

void HMotionTranslatorPlugin::SetExtension(const char *szExtension)
{
   ((MotionTranslatorPlugin *)this)->SetExtension(szExtension);
}

void HMotionTranslatorPlugin::SetImportCallback(SDK_PFN_MOTIONIMPORT lpfnImport)
{
   ((MotionTranslatorPlugin *)this)->SetImportCallback((PFN_MOTIONIMPORT)lpfnImport);
}

void HMotionTranslatorPlugin::SetExportCallback(SDK_PFN_MOTIONEXPORT lpfnExport)
{
   ((MotionTranslatorPlugin *)this)->SetExportCallback((PFN_MOTIONEXPORT)lpfnExport);
}