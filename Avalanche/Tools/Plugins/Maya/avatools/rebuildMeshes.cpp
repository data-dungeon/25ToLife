#include "stdafx.h"
#include "rebuildMeshes.h"
#include "SnapMeshesManager.h"

rebuildMeshes::rebuildMeshes() {}

rebuildMeshes::~rebuildMeshes() {}

void* rebuildMeshes::creator()
{
	return new rebuildMeshes();
}

bool rebuildMeshes::isUndoable() const
{
    return false;
}

MStatus rebuildMeshes::undoIt()
{
    return MS::kSuccess;
}

MStatus rebuildMeshes::doIt( const MArgList& args )
{
	MStatus status;

	status = redoIt();	

	return status;
}

MStatus rebuildMeshes::redoIt()
{
	MStatus status;
	MSelectionList selection;
	status = MGlobal::getActiveSelectionList(selection);
	if (selection.length()==0)
	{
		DISPLAY_ERROR("Nothing selected.");
		return MS::kFailure;
	}
	SnapMeshesManager snapMeshesManager;
	status = snapMeshesManager.AddInstancesForCopy(selection);

	FATAL_ERROR(status,"Error adding instances");

//	status = snapMeshesManager.ReorderFaces();

//	FATAL_ERROR(status,"Error reordering faces");

	status = snapMeshesManager.AddToScene();

	FATAL_ERROR(status,"Error adding to scene");

	DISPLAY_INFO("Done");

	return MS::kSuccess; 
}

