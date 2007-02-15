#include "stdafx.h"
#include "copyVertexColors.h"
#include "CompareMeshes.h"

copyVertexColors::copyVertexColors() {}

copyVertexColors::~copyVertexColors() {}

void* copyVertexColors::creator()
{
	return new copyVertexColors();
}

bool copyVertexColors::isUndoable() const
{
    return false;
}

MStatus copyVertexColors::undoIt()
{
    return MS::kSuccess;
}

MStatus copyVertexColors::doIt( const MArgList& args )
{
	MStatus status;

	status = redoIt();	

	return status;
}

MStatus copyVertexColors::redoIt()
{
	MStatus status;
	MDGModifier dgModifier;
	MSelectionList selection;

	status = MGlobal::getActiveSelectionList(selection);
	FATAL_ERROR(status,"[updateInstances] Couldn't get active selection list.");

	if (selection.length()<=0)
	{
		DISPLAY_ERROR("No objects selected.");
		return MS::kFailure;
	}

	MDagPath sourcedagpath;
	selection.getDagPath(selection.length()-1,sourcedagpath);
	if(!MTools::GetFirstShape(sourcedagpath,sourcedagpath,true))
		return MS::kFailure;

	CompareMeshesManager compareMeshes;
	if (sourcedagpath.apiType()!=MFn::kMesh)
	{
		DISPLAY_ERROR("Source object is not a poly mesh.");
		return MS::kFailure;
	}
	compareMeshes.AddSourceMesh(sourcedagpath);

	MDagPath destdagpath;

	for (unsigned i = 0; i < selection.length()-1; i++)
	{
		selection.getDagPath(i,destdagpath);
		if(!MTools::GetFirstShape(destdagpath,destdagpath,true))
			return MS::kFailure;
		if (destdagpath.apiType()==MFn::kMesh)
		{
			MTools::CopyVertexColors(sourcedagpath,destdagpath);
		}
	}

	return MS::kSuccess;
}

