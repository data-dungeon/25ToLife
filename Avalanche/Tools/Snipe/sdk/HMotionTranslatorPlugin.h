#pragma once
#include "HPlugin.h"

class HMotion;
typedef void (* SDK_PFN_MOTIONIMPORT) (void *pPluginData, HMotion *, const char *szFileName);
typedef void (* SDK_PFN_MOTIONEXPORT) (void *pPluginData, HMotion *, const char *szFileName);

class SNIPEEXPORT HMotionTranslatorPlugin : public HPlugin
{
public:
   void SetFilterName(const char *szFilterName);
   void SetExtension(const char *szExtension);
   void SetImportCallback(SDK_PFN_MOTIONIMPORT lpfnImport);
   void SetExportCallback(SDK_PFN_MOTIONEXPORT lpfnExport);
};
