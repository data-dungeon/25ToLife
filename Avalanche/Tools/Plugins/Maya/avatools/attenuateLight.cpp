#include "stdafx.h"
#include "attenuateLight.h"

attenuateLight::attenuateLight() {}

attenuateLight::~attenuateLight() {}

void* attenuateLight::creator()
{
	return new attenuateLight();
}

bool attenuateLight::isUndoable() const
{
    return false;
}

MStatus attenuateLight::undoIt()
{
    return MS::kSuccess;
}

MStatus attenuateLight::doIt( const MArgList& args )
{
	MStatus status;

	status = redoIt();	

	return status;
}

MStatus attenuateLight::redoIt()
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
	
	MObject object;

	selection.getDependNode(0,object);

	MTools::CreateLightAttenuationNetwork(object);

	return MS::kSuccess;
}

