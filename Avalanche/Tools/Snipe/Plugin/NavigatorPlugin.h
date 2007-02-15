#pragma once
#include "ManipulatorPlugin.h"

class PropertyArray;
class CGLBaseView;
class CGL3DView;

// Plugin
typedef void *(* PFN_CREATEINSTANCE_NAVIGATORPLUGIN)( void *PluginData, CGLBaseView *pView ); // Return parameter is the PluginInstanceData

// Instance
typedef BOOL (* PFN_SETVIEWANGLE_NAVIGATORINSTANCE ) ( void *pPluginInstanceData, CGL3DView *pView, ViewAngleIndex nIndex, BOOL bSnapToLastBirdsEye );
typedef BOOL (* PFN_ZOOMFIT_NAVIGATORINSTANCE ) ( void *pPluginInstanceData, CGLBaseView *pView, SnipeObjectList *pList );

class NavigatorPlugin : public ManipulatorPlugin
{
public:
// Plugin
   catch_msg PFN_CREATEINSTANCE_NAVIGATORPLUGIN m_pfnCreateInstance_NavigatorPlugin;

// Instance
   catch_msg PFN_SETVIEWANGLE_NAVIGATORINSTANCE m_pfnSetViewAngle_NavigatorInstance;
   catch_msg PFN_ZOOMFIT_NAVIGATORINSTANCE m_pfnZoomFit_NavigatorInstance;


   NavigatorPlugin( SnipePlugin *pSnipePlugin, void *pPluginData, PFN_DESTROY_PLUGINDATA pfnDestroy_PluginData );

   ~NavigatorPlugin()
   {
   }

   void SetCreateNavigatorInstanceCallback(PFN_CREATEINSTANCE_NAVIGATORPLUGIN pfnCreateInstance_NavigatorPlugin) { m_pfnCreateInstance_NavigatorPlugin = pfnCreateInstance_NavigatorPlugin; }
   void SetSetViewAngleCallback(PFN_SETVIEWANGLE_NAVIGATORINSTANCE pfnSetViewAngle_NavigatorInstance) { m_pfnSetViewAngle_NavigatorInstance = pfnSetViewAngle_NavigatorInstance; }
   void SetZoomFitCallback(PFN_ZOOMFIT_NAVIGATORINSTANCE pfnZoomFit_NavigatorInstance) { m_pfnZoomFit_NavigatorInstance = pfnZoomFit_NavigatorInstance; }
   
   PFN_CREATEINSTANCE_NAVIGATORPLUGIN GetCreateNavigatorInstanceCallback() const { return m_pfnCreateInstance_NavigatorPlugin; }
   PFN_SETVIEWANGLE_NAVIGATORINSTANCE GetSetViewAngleCallback() const { return m_pfnSetViewAngle_NavigatorInstance; }
   PFN_ZOOMFIT_NAVIGATORINSTANCE GetZoomFitCallback() const { return m_pfnZoomFit_NavigatorInstance; }

//CommandUIPlugin Overrides
   catch_msg virtual void Validate();

   //Plugin Overrides
   virtual String GetClassType() const;
};
