/****************************************************************************

	$Workfile: pluginMain.cpp $

	$Archive: /Avalanche/tools/Plugins/Maya/faceAttrib/pluginMain.cpp $

	$Author: Adam Clayton $

	$Date: 12/16/02 9:51a $

	CONFIDENTIAL AND PROPRIETARY INFORMATION

	Copyright © Avalanche Software 2002

	All rights reserved.

****************************************************************************/
// pluginMain.cpp : implementation file

// precomp'ed header files
#include "pluginMainPCH.h"

// volitile local header files
#include "pluginMain.h"
#include <maya/MFnPlugin.h>
#include <maya/MPxCommand.h>
//------------------------------------------------------------------------
// forward declarations of functions inside our dll
MStatus DllImport doIt(const MArgList& args, MStringArray& resultStrings);
void    DllImport CleanUp(void);

//------------------------------------------------------------------------
// Object Attributes class declaration
// - Our Maya command for adding/managing dynamic custom object attributes
// - Note that this is a pretty much empty shell, and all the actual plugin
//   work is done in the MFC dll. This is done to resolve the conflict of DLLMain
//   functions contained in both Maya's plugin (MLL) and the MFC dlls.
//
class CPluginMain : public MPxCommand
{
public:

	// Constructor
	CPluginMain(void)
	{
	}
	// Destructor
	~CPluginMain(void)
	{
	}
	// creator function
	static void* creator(void)
	{
		return new CPluginMain;
	}
	// doIt function
	MStatus doIt(const MArgList& args)
	{
		MStringArray resultStrings;

		// Tell the MFC plugin to doIt
		MStatus result = ::doIt(args, resultStrings);

		// set the result string for the user/mel script
		MPxCommand::setResult(resultStrings);

		// return the result
		return result;
	}
};

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

	status = plugin.registerCommand( "faceAttrib", CPluginMain::creator );
	if (!status) 
	{
		status.perror("registerCommand");
		return status;
	}

	// source the associated MEL script
//	MGlobal::executeCommand("source objAttrib.mel");

//	theObjectAttributes.SetupAttributes();

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

	status = plugin.deregisterCommand( "faceAttrib" );
	if (!status) 
	{
		status.perror("deregisterCommand");
		return status;
	}

	return status;
}