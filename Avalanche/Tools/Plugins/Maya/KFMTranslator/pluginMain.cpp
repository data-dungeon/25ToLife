//
// Copyright (C) 2002  
// 
// File: pluginMain.cpp
//
// Author: Maya SDK Wizard
//

#define REQUIRE_IOSTREAM

#include <maya/MStatus.h>
#include <maya/MFnPlugin.h>

#include "kfmtranslator.h"

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
	MFnPlugin plugin( obj, "KFM Exchange Translator", "1.0", "Any");

	status = plugin.registerFileTranslator("KFMTranslator", "none", KFMTranslator::creator,
								  (char *)KFMOptionScript, (char *)KFMDefaultOptions);
	if (!status)
	{
		status.perror("registerNode");
		return status;
	}
	
	return status;
}

MStatus uninitializePlugin( MObject obj)
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

	status = plugin.deregisterFileTranslator( "KFMTranslator" );
	if (!status)
	{
		status.perror("deregisterNode");
		return status;
	}

	return status;
}
