#include "stdafx.h"
#include "scaleUVs.h"

scaleUVs::scaleUVs() {}

scaleUVs::~scaleUVs() {}

void* scaleUVs::creator()
{
	return new scaleUVs();
}

bool scaleUVs::isUndoable() const
{
    return true;
}

MStatus scaleUVs::undoIt()
{
	MStatus status;
	MDGModifier dgModifier;
	MString command;

	command = (MString("select -r "));
	for (unsigned i = 0; i < m_selUVs.length(); i++)
		command += m_selUVs[i] + " ";

	MGlobal::executeCommand(command);
	dgModifier.doIt();

	char grow[256],shrink[256];

	if (m_increment_by<=0)
		return MS::kFailure;

	sprintf(grow,"%f",(float)m_increment_by);
	sprintf(shrink,"%f",(float)(1.0/m_increment_by));
	
	if ((m_increment_tile)&&(m_udirection))
		command = (MString("polyEditUV -pu 0.0 -pv 0.0 -su ") + shrink);
	else if ((!m_increment_tile)&&(m_udirection))
		command = (MString("polyEditUV -pu 0.0 -pv 0.0 -su ") + grow);
	else if ((m_increment_tile)&&(!m_udirection))
		command = (MString("polyEditUV -pu 0.0 -pv 0.0 -sv ") + shrink);
	else if ((!m_increment_tile)&&(!m_udirection))
		command = (MString("polyEditUV -pu 0.0 -pv 0.0 -sv ") + grow);

	MGlobal::executeCommand(command);
	dgModifier.doIt();

	// RESELECT THE POLYS
	command = (MString("select -r "));
	for (i = 0; i < m_selFaces.length(); i++)
		command += m_selFaces[i] + " ";


	MGlobal::executeCommand(command);
	dgModifier.doIt();

    return MS::kSuccess;
}

MStatus scaleUVs::doIt( const MArgList& args )
{
	MStatus status;

	if (args.length() == 3)
	{
		args.get(0,m_increment_tile);
		args.get(1,m_udirection);
		args.get(2,m_increment_by);
	}
	else
	{
		return MS::kFailure;
	}

	status = redoIt();	

	return status;
}

MStatus scaleUVs::redoIt()
{
	MStatus status;
	MSelectionList selection;
	MDGModifier dgModifier;
	MString command;

	status = MGlobal::getActiveSelectionList(selection);
	FATAL_ERROR(status,"scaleUVs: Couldn't get active selection list.");

	command = MString("filterExpand -sm 34 -ex true");
	MGlobal::executeCommand(command,m_selFaces);
	dgModifier.doIt();
	if (m_selFaces.length() == 0) {
		FATAL_ERROR(status,"scaleUVs: No polygons selected.");
	}

	command = (MString("polyListComponentConversion -ff -tuv "));

	MGlobal::executeCommand(command,m_selUVs);
	dgModifier.doIt();

	command = (MString("select -r "));
	for (unsigned i = 0; i < m_selUVs.length(); i++)
		command += m_selUVs[i] + " ";

	MGlobal::executeCommand(command);
	dgModifier.doIt();

	char grow[256],shrink[256];

	if (m_increment_by<=0)
		return MS::kFailure;

	sprintf(grow,"%f",(float)m_increment_by);
	sprintf(shrink,"%f",(float)(1.0/m_increment_by));

	if ((m_increment_tile)&&(m_udirection))
		command = (MString("polyEditUV -pu 0.0 -pv 0.0 -su ") + grow);
	else if ((!m_increment_tile)&&(m_udirection))
		command = (MString("polyEditUV -pu 0.0 -pv 0.0 -su ") + shrink);
	else if ((m_increment_tile)&&(!m_udirection))
		command = (MString("polyEditUV -pu 0.0 -pv 0.0 -sv ") + grow);
	else if ((!m_increment_tile)&&(!m_udirection))
		command = (MString("polyEditUV -pu 0.0 -pv 0.0 -sv ") + shrink);

	MGlobal::executeCommand(command);
	dgModifier.doIt();

	command = (MString("select -r "));
	for (i = 0; i < m_selFaces.length(); i++)
		command += m_selFaces[i] + " ";


	MGlobal::executeCommand(command);
	dgModifier.doIt();

    return MS::kSuccess;
}