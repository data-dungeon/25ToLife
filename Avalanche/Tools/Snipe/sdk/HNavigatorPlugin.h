#pragma once
#include "HManipulatorPlugin.h"

class HCGLBaseView;
class HCGL3DView;
class HSnipeObjectList;

// Plugin
typedef void *(* SDK_PFN_CREATEINSTANCE_NAVIGATORPLUGIN)( void *pPluginData, HCGLBaseView *pView );

// Instance
typedef BOOL (* SDK_PFN_SETVIEWANGLE_NAVIGATORINSTANCE ) ( void *pPluginInstanceData, HCGL3DView *pView, ViewAngleIndex nIndex, BOOL bSnapToLastBirdsEye );
typedef BOOL (* SDK_PFN_ZOOMFIT_NAVIGATORINSTANCE ) ( void *pPluginInstanceData, HCGLBaseView *pView, HSnipeObjectList *pList );

class SNIPEEXPORT HNavigatorPlugin : public HManipulatorPlugin
{
public:
// Plugin
   void SetCreateNavigatorInstanceCallback(SDK_PFN_CREATEINSTANCE_NAVIGATORPLUGIN pfnCreateInstance);

//Instance
   void SetSetViewAngleCallback(SDK_PFN_SETVIEWANGLE_NAVIGATORINSTANCE pfnSetViewAngle_NavigatorInstance);
   void SetZoomFitCallback(SDK_PFN_ZOOMFIT_NAVIGATORINSTANCE pfnZoomFit_NavigatorInstance);
};