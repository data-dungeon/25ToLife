#include "stdafx.h"
#include "snapverticestoclosest.h"
#include "SnapMeshesManager.h"

snapVerticesToClosest::snapVerticesToClosest() {}

snapVerticesToClosest::~snapVerticesToClosest() {}

void* snapVerticesToClosest::creator()
{
	return new snapVerticesToClosest();
}

bool snapVerticesToClosest::isUndoable() const
{
    return false;
}

MStatus snapVerticesToClosest::undoIt()
{
    return MS::kSuccess;
}

MStatus snapVerticesToClosest::doIt( const MArgList& args )
{
	MStatus status;

	m_tolerance = 10.00f;

	if (args.length() > 0)
		args.get(0,m_tolerance);

	status = redoIt();	

	return status;
}

MStatus snapVerticesToClosest::redoIt()
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
	snapMeshesManager.SetPositionTolerance((float)m_tolerance);
	status = snapMeshesManager.AddInstancesForSnap(selection);

	FATAL_ERROR(status,"Error adding instances");

	status = snapMeshesManager.SnapInstancesToBase();

	FATAL_ERROR(status,"Error snapping instance to base");

	status = snapMeshesManager.AddToScene();

	FATAL_ERROR(status,"Error adding to scene");

	DISPLAY_INFO("Done");

	return MS::kSuccess; 
}

