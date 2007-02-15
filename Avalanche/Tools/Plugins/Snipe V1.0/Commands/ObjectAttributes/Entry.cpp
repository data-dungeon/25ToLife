//
#include "stdafx.h"
#include "HSnipePlugin.h"
#include "ObjectAttributesCmd.h"

extern "C" __declspec(dllexport) void InitSnipePlugin( HSnipePlugin *pSnipePlugin )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	pSnipePlugin->SetName( "OBJECTATTRIBUTES" );

	///////////////////////////////////////////////////////////////////////////////////
	// Commands
	///////////////////////////////////////////////////////////////////////////////////

	ADD_SNIPE_COMMAND_PLUGIN( pSnipePlugin, ObjectAttributesCommandPlugin );

}
