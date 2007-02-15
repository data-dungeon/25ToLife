//
// Copyright (C) 2002  
// 
// File: pluginMain.cpp
//
// Author: Maya SDK Wizard
//

#include "TupImpExpPch.h"
#include "TupImpExp.h"
#include "TupAnimImpExp.h"
#include <maya/MFnPlugin.h>
#include "DblExp.h"

static char *pTupOptionScript = "TupTranslatorOptions";
static char *pTupDefaultOptions = "consolshapes=0;binaryoutput=0";

static char *pAnmOptionScript = "AnmTranslatorOptions";
static char *pAnmDefaultOptions = "binaryoutput=0;importasclip=0";

static char *pDblOptionScript = "DblTranslatorOptions";
static char *pDblDefaultOptions = "filterchainpath=C:/AvalancheToolSet/FilterChain;processfile=C:/AvalancheToolSet/FilterChain/Chick/Process_Model_DX.twc";

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
	MFnPlugin plugin( obj, "Maya/Max Exchange Translator", "5.0", "Any");

	status = plugin.registerFileTranslator("Tup File", "none", TupImpExp::creator,
								  pTupOptionScript, pTupDefaultOptions);
	if (!status) 
	{
		status.perror("registerNode");
		return status;
	}

   MFnPlugin animplugin( obj, "Maya Animation Exchange Translator", "5.0", "Any");

   status = animplugin.registerFileTranslator("Anm File", "none", TupAnimImpExp::creator,
      pAnmOptionScript, pAnmDefaultOptions);
   if (!status) 
   {
      status.perror("registerNode");
      return status;
   }

   MFnPlugin dblplugin( obj, "Maya DBL Translator", "5.0", "Any");

   status = animplugin.registerFileTranslator("DBL File", "none", DBLExport::creator,
      pDblOptionScript, pDblDefaultOptions);
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

	status = plugin.deregisterFileTranslator( "Tup File" );
	if (!status) 
	{
		status.perror("deregisterNode");
		return status;
	}

   status = plugin.deregisterFileTranslator( "Anm File" );
   if (!status) 
   {
      status.perror("deregisterNode");
      return status;
   }

   status = plugin.deregisterFileTranslator( "DBL File" );
   if (!status) 
   {
      status.perror("deregisterNode");
      return status;
   }

	return status;
}
