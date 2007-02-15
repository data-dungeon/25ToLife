
// precomp'ed header files
#include "pluginMainPCH.h"

// volitile local header files
#include <maya/MFnPlugin.h>
#include "pluginMain.h"
#include "ObjAttribCmd.h"

//------------------------------------------------------------------------
// plugin init function
MStatus initializePlugin( MObject obj )
//
//	Description:
//		this method is called when the plug-in is loaded into Maya.  It 
//		registers all of the services that this plug-in provides with 
//		Maya.
//
//	Arguments:
//		obj - a handle to the plug-in object (use MFnPlugin to access it)
//
{ 
	MStatus   status;
	MFnPlugin plugin( obj, "Avalanche Software", "4.5", "Any");

	status = plugin.registerCommand( kPluginCommand, ObjAttribCmd::creator, ObjAttribCmd::newSyntax );
	if (!status) 
	{
		status.perror("registerCommand");
		return status;
	}

	return status;
}

//------------------------------------------------------------------------
// plugin un-init function
MStatus uninitializePlugin( MObject obj )
//
//	Description:
//		this method is called when the plug-in is unloaded from Maya. It 
//		deregisters all of the services that it was providing.
//
//	Arguments:
//		obj - a handle to the plug-in object (use MFnPlugin to access it)
//
{
	MStatus   status;
	MFnPlugin plugin( obj );

	status = plugin.deregisterCommand( "objAttrib" );
	if (!status) 
	{
		status.perror("deregisterCommand");
		return status;
	}

	return status;
}
