#include "stdafx.h"
#include "vertexLight.h"

vertexLight::vertexLight() {}

vertexLight::~vertexLight() {}

void* vertexLight::creator()
{
	return new vertexLight();
}

bool vertexLight::isUndoable() const
{
    return false;
}

MStatus vertexLight::undoIt()
{
    return MS::kFailure;
}

MStatus vertexLight::doIt( const MArgList& args )
{
	MStatus status;

	int shadowmaps = 0,reuseshadowmaps = 0;

	if (args.length() > 0)
	{
		args.get(0,shadowmaps);
		args.get(1,reuseshadowmaps);
	}

	m_shadowmaps = false;
	m_reuseshadowmaps = false;

	if (shadowmaps)
		m_shadowmaps = true;
	if (reuseshadowmaps)
		m_reuseshadowmaps = true;

	status = redoIt();	

	return status;
}

MStatus vertexLight::redoIt()
{
	MStatus status;
	MSelectionList selection;

	status = MGlobal::getActiveSelectionList(selection);

	if (selection.length()==0)
	{
		MGlobal::displayError("Nothing selected.");
		return MS::kFailure;
	}

	if (!MTools::BakeVertexColors(selection,m_shadowmaps,m_reuseshadowmaps))
		return MS::kFailure;

	return MS::kSuccess;
}
