#pragma once
#include "HPlugin.h"

class HMotionSet;
typedef void (* SDK_PFN_MOTIONSETIMPORT) (void *pPluginData, HMotionSet *, const char *szFileName);
typedef void (* SDK_PFN_MOTIONSETEXPORT) (void *pPluginData, HMotionSet *, const char *szFileName);

class SNIPEEXPORT HMotionSetTranslatorPlugin : public HPlugin
{
public:
   void SetFilterName(const char *szFilterName);
   void SetExtension(const char *szExtension);
   void SetImportCallback(SDK_PFN_MOTIONSETIMPORT lpfnImport);
   void SetExportCallback(SDK_PFN_MOTIONSETEXPORT lpfnExport);
};
