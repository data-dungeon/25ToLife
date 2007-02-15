#pragma once
#include "HPluginHelper.h"

#include "HMotionSetTranslatorPlugin.h"

/////////////////////////////////////////////////////////////////////////////
// C++ Helper class for HMotionSetTranslatorPlugin

class HMotionSetTranslatorPluginHelper : public HPluginHelper
{
public:
   HMotionSetTranslatorPlugin *GetPlugin() { return (HMotionSetTranslatorPlugin *)HPluginHelper::GetPlugin(); }

   virtual BOOL HasImporter() { return FALSE; }
   virtual BOOL HasExporter() { return FALSE; }

   // Pure
   virtual const char *GetFilterName() = 0;
   virtual const char *GetFilterExtension() = 0;

   // Callbacks
   catch_msg virtual void OnImport( HMotionSet *pMotionSet, const char *szFileName) {}
   catch_msg virtual void OnExport( HMotionSet *pMotionSet, const char *szFileName) {}
};

////////////////////////////////////////////////////////////////////////////
// Inlines

inline void OnImport_HMotionSetTranslatorPluginHelper(void *pPluginData, HMotionSet *pMotionSet, const char *szFileName)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   ((HMotionSetTranslatorPluginHelper *)pPluginData)->OnImport( pMotionSet, szFileName );
}

inline void OnExport_HMotionSetTranslatorPluginHelper(void *pPluginData, HMotionSet *pMotionSet, const char *szFileName)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   ((HMotionSetTranslatorPluginHelper *)pPluginData)->OnExport( pMotionSet, szFileName );
}

inline HMotionSetTranslatorPluginHelper *GetMotionSetTranslatorPluginHelper( HMotionSetTranslatorPlugin *pPlugin )
{
   ASSERT(pPlugin);
   return (HMotionSetTranslatorPluginHelper *)pPlugin->GetPluginData();\
}

/////////////////////////////////////////////////////////////////////////
// Macro

#define ADD_SNIPE_MOTIONSET_TRANSLATOR_PLUGIN( pSnipePlugin, helper_class ) \
{ \
   AFX_MANAGE_STATE(AfxGetStaticModuleState()); \
   HMotionSetTranslatorPluginHelper *pHelper = new helper_class; \
   HMotionSetTranslatorPlugin *pPlugin = pSnipePlugin->AddMotionSetTranslatorPlugin(pHelper, Destroy_PluginData); \
   pHelper->SetPlugin(pPlugin); \
   pPlugin->SetClassName( pHelper->GetClassName() ); \
   pPlugin->SetTitleName( pHelper->GetTitleName() ); \
   pPlugin->SetFilterName( pHelper->GetFilterName() ); \
   pPlugin->SetExtension( pHelper->GetFilterExtension() ); \
   pPlugin->SetImportCallback( pHelper->HasImporter() ? OnImport_HMotionSetTranslatorPluginHelper : NULL ); \
   pPlugin->SetExportCallback( pHelper->HasExporter() ? OnExport_HMotionSetTranslatorPluginHelper : NULL ); \
}

#define DECLARE_MOTIONSETTRANSLATOR_PLUGINHELPER(class_name, baseclass_name) \
   DECLARE_PLUGINHELPER(class_name, baseclass_name )
