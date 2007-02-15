#include "GameClientPCH.h"
#include <maya/MFnPlugin.h>
#include <maya/MPxCommand.h>

//#include "ConnectCameraToGame.h"


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
	MFnPlugin plugin( obj, "Avalanche Software", "5.0", "Any");

/*
	status = plugin.registerCommand( "connectCameraToGame", ConnectCameraToGame::creator );
	if (!status) 
	{
		status.perror("registerCommand");
		return status;
	}
*/

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
/*
	status = plugin.deregisterCommand( "connectCameraToGame" );
	if (!status) 
	{
		status.perror("deregisterCommand");
		return status;
	}
*/

	return status;
}