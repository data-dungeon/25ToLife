#include "stdafx.h"
#include "getKeyValueFromNotes.h"

getKeyValueFromNotes::getKeyValueFromNotes() {}

getKeyValueFromNotes::~getKeyValueFromNotes() {}

void* getKeyValueFromNotes::creator()
{
	return new getKeyValueFromNotes();
}

bool getKeyValueFromNotes::isUndoable() const
{
    return false;
}

MStatus getKeyValueFromNotes::undoIt()
{
    return MS::kSuccess;
}

MStatus getKeyValueFromNotes::doIt( const MArgList& args )
{
	MStatus status;

	if (args.length() == 1)
	{
		args.get(0,m_key);		// key
		m_objectname = "";
	}
	else if (args.length() == 2)
	{
		args.get(0,m_key);		// key
		args.get(2,m_objectname); // attribute type
	}
	else
	{
		DISPLAY_ERROR("Bad arguments.")
		return MS::kFailure;
	}

	status = redoIt();	

	return status;
}

MStatus getKeyValueFromNotes::redoIt()
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
	
	int successful = MTools::GetKeyValueFromNotes(object,m_key,m_value);

	setResult(m_value);

	return MS::kSuccess;
}

