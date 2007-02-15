#pragma once
#include "HPluginHelper.h"

/////////////////////////////////////////////////////////////////////////////
// C++ Helper class for HMotionTranslatorPlugin

class HMotionTranslatorPluginHelper : public HPluginHelper
{
public:
   HMotionTranslatorPlugin *GetPlugin() { return (HMotionTranslatorPlugin *)HPluginHelper::GetPlugin(); }

   virtual BOOL HasImporter() { return FALSE; }
   virtual BOOL HasExporter() { return FALSE; }

   // Pure
   virtual const char *GetFilterName() = 0;
   virtual const char *GetFilterExtension() = 0;

   // Callbacks
   catch_msg virtual void OnImport( HMotion *pMotion, const char *szFileName) {}
   catch_msg virtual void OnExport( HMotion *pMotion, const char *szFileName) {}
};

////////////////////////////////////////////////////////////////////////////
// Inlines

inline void OnImport_HMotionTranslatorPluginHelper(void *pPluginData, HMotion *pMotion, const char *szFileName)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   ((HMotionTranslatorPluginHelper *)pPluginData)->OnImport( pMotion, szFileName );
}

inline void OnExport_HMotionTranslatorPluginHelper(void *pPluginData, HMotion *pMotion, const char *szFileName)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   ((HMotionTranslatorPluginHelper *)pPluginData)->OnExport( pMotion, szFileName );
}

inline HMotionTranslatorPluginHelper *GetMotionTranslatorPluginHelper( HMotionTranslatorPlugin *pPlugin )
{
   ASSERT(pPlugin);
   return (HMotionTranslatorPluginHelper *)pPlugin->GetPluginData();\
}

/////////////////////////////////////////////////////////////////////////
// Macro

#define ADD_SNIPE_MOTION_TRANSLATOR_PLUGIN( pSnipePlugin, helper_class ) \
{ \
   AFX_MANAGE_STATE(AfxGetStaticModuleState()); \
   HMotionTranslatorPluginHelper *pHelper = new helper_class; \
   HMotionTranslatorPlugin *pPlugin = pSnipePlugin->AddMotionTranslatorPlugin(pHelper, Destroy_PluginData); \
   pHelper->SetPlugin(pPlugin); \
   pPlugin->SetClassName( pHelper->GetClassName() ); \
   pPlugin->SetTitleName( pHelper->GetTitleName() ); \
   pPlugin->SetFilterName( pHelper->GetFilterName() ); \
   pPlugin->SetExtension( pHelper->GetFilterExtension() ); \
   pPlugin->SetImportCallback( pHelper->HasImporter() ? OnImport_HMotionTranslatorPluginHelper : NULL ); \
   pPlugin->SetExportCallback( pHelper->HasExporter() ? OnExport_HMotionTranslatorPluginHelper : NULL ); \
}

#define DECLARE_MOTIONTRANSLATOR_PLUGINHELPER(class_name, baseclass_name) \
   DECLARE_PLUGINHELPER(class_name, baseclass_name )
