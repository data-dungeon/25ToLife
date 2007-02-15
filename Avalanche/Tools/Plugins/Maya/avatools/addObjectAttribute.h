//////////////////////////////////////////////////////////////////////////////////////
//
//	ARGUMENTS:
//
//		Attribute Note
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _addObjectAttribute
#define _addObjectAttribute

#include <maya/MPxCommand.h>

class addObjectAttribute : public MPxCommand {
public:
	addObjectAttribute ();
	~addObjectAttribute (); 

	MStatus		doIt( const MArgList& args );
    MStatus     redoIt();
    MStatus     undoIt();
    bool        isUndoable() const;

	static void*	creator();

protected:
	MString m_attrtype,m_longname,m_shortname;
	MString m_key,m_value;
};

#endif