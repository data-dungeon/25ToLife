//
#include "stdafx.h"
#include "NavigatorPlugin.h"
#include "HNavigatorPlugin.h"

// Plugin ////////////////////////////////////////////////////////////////////////////////////////
void HNavigatorPlugin::SetCreateNavigatorInstanceCallback(SDK_PFN_CREATEINSTANCE_NAVIGATORPLUGIN pfnCreateInstance)
{
   ((NavigatorPlugin *)this)->SetCreateNavigatorInstanceCallback( (PFN_CREATEINSTANCE_NAVIGATORPLUGIN)pfnCreateInstance );
}

// Instance ////////////////////////////////////////////////////////////////////////////////////////
void HNavigatorPlugin::SetSetViewAngleCallback( SDK_PFN_SETVIEWANGLE_NAVIGATORINSTANCE pfnSetViewAngle )
{
   ((NavigatorPlugin *)this)->SetSetViewAngleCallback( (PFN_SETVIEWANGLE_NAVIGATORINSTANCE)pfnSetViewAngle );
}

void HNavigatorPlugin::SetZoomFitCallback( SDK_PFN_ZOOMFIT_NAVIGATORINSTANCE pfnZoomFit )
{
   ((NavigatorPlugin *)this)->SetZoomFitCallback( (PFN_ZOOMFIT_NAVIGATORINSTANCE)pfnZoomFit );
}
