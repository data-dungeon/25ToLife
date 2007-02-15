#include "stdafx.h"
#include "delKeyValueFromNotes.h"

delKeyValueFromNotes::delKeyValueFromNotes() {}

delKeyValueFromNotes::~delKeyValueFromNotes() {}

void* delKeyValueFromNotes::creator()
{
	return new delKeyValueFromNotes();
}

bool delKeyValueFromNotes::isUndoable() const
{
    return false;
}

MStatus delKeyValueFromNotes::undoIt()
{
    return MS::kSuccess;
}

MStatus delKeyValueFromNotes::doIt( const MArgList& args )
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

MStatus delKeyValueFromNotes::redoIt()
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
	
	int successful = MTools::DeleteKeyValueFromNotes(object,m_key);

	return MS::kSuccess;
}

