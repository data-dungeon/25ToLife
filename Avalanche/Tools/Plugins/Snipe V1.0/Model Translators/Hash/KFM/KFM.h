#pragma once

#include "HMotionTranslatorPlugin.h" 
#include "HMotionTranslatorPluginHelper.h"

#define KFMVERSION 0x12345678+1

class KFMTranslator : public HMotionTranslatorPluginHelper
{
public:
   DECLARE_MOTIONTRANSLATOR_PLUGINHELPER( KFMTranslator, HMotionTranslatorPluginHelper )

public:
   KFMTranslator();

   // HMotionTranslatorPluginHelper overrides
   virtual const char *GetFilterName() { return "KFM Motion"; }
   virtual const char *GetFilterExtension() { return "kfm"; }

   // HPluginHelper overrides
   virtual const char *GetTitleName() const { return "KFM Motion"; }

   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // TUP IMPORTER
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   virtual BOOL HasImporter() { return TRUE; }
   virtual void OnImport( HMotion *pMotion, const char *szFileName);

   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // EXPORTER
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   virtual BOOL HasExporter() { return FALSE; }
 //  virtual void OnExport( HMotion *pMotion, const char *szFileName);

   enum INTERPOLATIONMETHOD { HOLD, LINEAR, SPLINE };

};
