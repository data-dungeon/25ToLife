//
// AVARig.cpp
//

#include "StdAfx.h"

#include <maya/MFnPlugin.h>
#include <maya/MPxCommand.h>

#include "OrientLikeNode.h"
#include "OrientLikeCmd.h"

//------------------------------------------------------------------------
// plugin init function
MStatus initializePlugin( MObject obj )
{ 
	MStatus status;
	MFnPlugin plugin( obj, "Avalanche Software", "4.5", "Any");

	status = plugin.registerCommand( "orientLikeConstraint", OrientLikeCmd::creator, OrientLikeCmd::newSyntax );
	if (!status)
	{
		status.perror("registerCommand failed");
		return status;
	}

	status = plugin.registerTransform( "orientLikeConstraint", OrientLikeNode::id, OrientLikeNode::creator, OrientLikeNode::initialize,
		MPxTransformationMatrix::creator,MPxTransformationMatrix::baseTransformationMatrixId);
	if (!status)
	{
		status.perror("registerNode failed");
		return status;
	}

	return status;
}

//------------------------------------------------------------------------
// plugin un-init function
MStatus uninitializePlugin( MObject obj)
{
	MStatus status;
	MFnPlugin plugin( obj );

	status = plugin.deregisterCommand( "OrientLike" );
	if (!status)
	{
		status.perror("deregisterCommand failed");
		return status;
	}

	status = plugin.deregisterNode( OrientLikeNode::id );
	if (!status)
	{
		status.perror("deregisterNode failed");
		return status;
	}

	return status;
}
