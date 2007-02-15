#include "stdafx.h"
#include "addKeyValueToNotes.h"

addKeyValueToNotes::addKeyValueToNotes() {}

addKeyValueToNotes::~addKeyValueToNotes() {}

void* addKeyValueToNotes::creator()
{
	return new addKeyValueToNotes();
}

bool addKeyValueToNotes::isUndoable() const
{
    return false;
}

MStatus addKeyValueToNotes::undoIt()
{
    return MS::kSuccess;
}

MStatus addKeyValueToNotes::doIt( const MArgList& args )
{
	MStatus status;

	if (args.length() == 1)
	{
		MString arg;
		args.get(0,arg);
		MStringArray splitargs;
		arg.split('=',splitargs);
		if (splitargs.length()!=2)
			return MS::kFailure;
		m_key = splitargs[0];
		m_value = splitargs[1];
	}
	else if (args.length() == 2)
	{
		args.get(0,m_key);		// key
		args.get(1,m_value);	// value
		m_objectname = "";
	}
	else if (args.length() == 3)
	{
		args.get(0,m_key);		// key
		args.get(1,m_value);	// value
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

MStatus addKeyValueToNotes::redoIt()
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
	

	int successful = MTools::InsertKeyValueToNotes(object,m_key,m_value);

	return MS::kSuccess;
}

