//
#include "stdafx.h"
#include "HSnipePlugin.h"
#include "MotionAttributesCmd.h"

extern "C" __declspec(dllexport) void InitSnipePlugin( HSnipePlugin *pSnipePlugin )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	pSnipePlugin->SetName( "MotionAttributes" );

	///////////////////////////////////////////////////////////////////////////////////
	// Commands
	///////////////////////////////////////////////////////////////////////////////////

	ADD_SNIPE_COMMAND_PLUGIN( pSnipePlugin, MotionAttributesCommandPlugin );

}
