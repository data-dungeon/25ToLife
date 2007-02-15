//
// Copyright (C) 2002  
// 
// File: pluginMain.cpp
//
// Author: Maya SDK Wizard
//

#include <maya/MStatus.h>
//#include <maya/MPxFileTranslator.h>

#include "mffTranslator.h"
//#include "mffImport.h"

#include <maya/MFnPlugin.h>

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
	MFnPlugin plugin( obj, "Maya/Max Exchange Translator", "4.5", "Any");

	status = plugin.registerFileTranslator("mffTranslator", "none", mffTranslator::creator,
								  (char *)mffOptionScript, (char *)mffDefaultOptions);
	if (!status) {
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

	status = plugin.deregisterFileTranslator( "mffTranslator" );
	if (!status) {
		status.perror("deregisterNode");
		return status;
	}

	return status;
}
