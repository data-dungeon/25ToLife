#include "delObjectAttribute.h"

#include <maya/MGlobal.h>
#include <maya/MSelectionList.h>
#include <maya/MDGModifier.h>
#include <maya/MString.h>
#include <maya/MDagPath.h>
#include <maya/MFnDagNode.h>
#include <maya/MFileIO.h>
#include <maya/MItDag.h>
#include <maya/MDagModifier.h>
#include <maya/MPlugArray.h>
#include <maya/MPlug.h>
#include <maya/MFnTransform.h>
#include <maya/MTransformationMatrix.h>
#include <maya/MDagPathArray.h>
#include <maya/MObjectArray.h>
#include <maya/MArgList.h>

#include "../../libraries/MTools/MTools.h"

#include <iostream.h>
#include "macros.h"

delObjectAttribute::delObjectAttribute() {}

delObjectAttribute::~delObjectAttribute() {}

void* delObjectAttribute::creator()
{
	return new delObjectAttribute();
}

bool delObjectAttribute::isUndoable() const
{
    return false;
}

MStatus delObjectAttribute::undoIt()
{
    return MS::kSuccess;
}

MStatus delObjectAttribute::doIt( const MArgList& args )
{
	MStatus status;

	if (args.length() == 2)
	{
		args.get(0,m_key);		// key
		args.get(1,m_longname); // longname
	}
	else
	{
		DISPLAY_ERROR("Bad arguments.")
		return MS::kFailure;
	}

	status = redoIt();	

	return status;
}

MStatus delObjectAttribute::redoIt()
{
	MStatus status;
	MSelectionList selection;
	MDGModifier dgModifier;
	MString command;

	status = MGlobal::getActiveSelectionList(selection);
	FATAL_ERROR(status,"Couldn't get active selection list.");

	if (selection.length()<=0)
	{
		DISPLAY_ERROR("No objects selected.");
		return MS::kFailure;
	}

	MDagPathArray dagpatharray;
	dagpatharray.clear();
	
	for (unsigned i = 0; i < selection.length(); i++)
	{
		MDagPath dagpath;
		selection.getDagPath(i,dagpath);

		int successful = MTools::DeleteObjectAttribute(dagpath.node(),m_key,m_longname);
	}

	return MS::kSuccess;
}

