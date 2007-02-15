#include "addObjectAttribute.h"

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

addObjectAttribute::addObjectAttribute() {}

addObjectAttribute::~addObjectAttribute() {}

void* addObjectAttribute::creator()
{
	return new addObjectAttribute();
}

bool addObjectAttribute::isUndoable() const
{
    return false;
}

MStatus addObjectAttribute::undoIt()
{
    return MS::kSuccess;
}

MStatus addObjectAttribute::doIt( const MArgList& args )
{
	MStatus status;

	if (args.length() == 5)
	{
		args.get(0,m_attrtype); // attribute type
		args.get(1,m_longname); // longname
		args.get(2,m_shortname);// shortname
		args.get(3,m_key);		// key
		args.get(4,m_value);	// value
	}
	else
	{
		DISPLAY_ERROR("Bad arguments.")
		return MS::kFailure;
	}

	status = redoIt();	

	return status;
}

MStatus addObjectAttribute::redoIt()
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

		int successful = MTools::AddObjectAttribute(dagpath.node(),m_attrtype,m_longname,m_shortname,m_key,m_value);
		if (!successful)
		{
			DISPLAY_ERROR("Couldn't add attribute.");
		}
	}

	return MS::kSuccess;
}

