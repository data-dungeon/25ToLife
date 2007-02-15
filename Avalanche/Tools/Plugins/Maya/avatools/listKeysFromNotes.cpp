#include "stdafx.h"
#include "listKeysFromNotes.h"

listKeysFromNotes::listKeysFromNotes() {}

listKeysFromNotes::~listKeysFromNotes() {}

void* listKeysFromNotes::creator()
{
	return new listKeysFromNotes();
}

bool listKeysFromNotes::isUndoable() const
{
    return false;
}

MStatus listKeysFromNotes::undoIt()
{
    return MS::kSuccess;
}

MStatus listKeysFromNotes::doIt( const MArgList& args )
{
	MStatus status;

	if (args.length() == 0)
	{
		m_objectname = "";
	}
	else if (args.length() == 1)
	{
		args.get(0,m_objectname); // attribute type
	}

	status = redoIt();	

	return status;
}

MStatus listKeysFromNotes::redoIt()
{
	MStatus status;
	MDGModifier dgModifier;
	MSelectionList selection;

	MObject object;

	if (m_objectname == "")
	{
		status = MGlobal::getActiveSelectionList(selection);
		FATAL_ERROR(status,"Couldn't get active selection list.");

		if (selection.length()==0)
		{
			DISPLAY_ERROR("No objects selected.");
			return MS::kFailure;
		}
		selection.getDependNode(0,object);
	}
	else
	{
		object = MTools::GetMObject(m_objectname);
	}
	
	MStringArray results;
	MTools::ListKeysFromNotes(object,results);

	setResult(results);

	return MS::kSuccess;
}

