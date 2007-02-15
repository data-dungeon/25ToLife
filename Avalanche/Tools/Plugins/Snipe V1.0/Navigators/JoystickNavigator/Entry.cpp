//
#include "stdafx.h"
#include "JoystickNavigatorPlugin.h"
#include "HSnipePlugin.h"

extern "C" __declspec(dllexport) void InitSnipePlugin( HSnipePlugin *pSnipePlugin )
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   pSnipePlugin->SetName( "JOYSTICKNAVIGATOR" );

   ADD_SNIPE_NAVIGATOR_PLUGIN( pSnipePlugin, JoystickNavigatorPlugin );
}