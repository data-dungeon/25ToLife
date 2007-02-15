//////////////////////////////////////////////////////////////////////////////////////
//
//	ARGUMENTS:
//
//		Attribute Note
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _makeGeomConsolidateGroup
#define _makeGeomConsolidateGroup

#include <maya/MPxCommand.h>

class makeGeomConsolidateGroup : public MPxCommand
{
public:
	makeGeomConsolidateGroup();
	~makeGeomConsolidateGroup();

	MStatus		doIt( const MArgList& args );
    MStatus     redoIt();
    MStatus     undoIt();
    bool        isUndoable() const;

	static void*	creator();

protected:
	MString m_note;
};

#endif