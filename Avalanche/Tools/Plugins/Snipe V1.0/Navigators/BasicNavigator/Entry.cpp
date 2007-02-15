//
#include "stdafx.h"
#include "BasicNavigatorPlugin.h"
#include "HSnipePlugin.h"
#include "Navigator.h"

extern "C" __declspec(dllexport) void InitSnipePlugin( HSnipePlugin *pSnipePlugin )
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   pSnipePlugin->SetName( "SNIPENAVIGATOR" );

   ADD_SNIPE_NAVIGATOR_PLUGIN(pSnipePlugin, SnipeNavigatorPlugin );
}