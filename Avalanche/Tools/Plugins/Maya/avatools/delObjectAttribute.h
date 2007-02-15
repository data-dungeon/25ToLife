//////////////////////////////////////////////////////////////////////////////////////
//
//	ARGUMENTS:
//
//		Attribute Note
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _delObjectAttribute
#define _delObjectAttribute

#include <maya/MPxCommand.h>

class delObjectAttribute : public MPxCommand {
public:
	delObjectAttribute ();
	~delObjectAttribute (); 

	MStatus		doIt( const MArgList& args );
    MStatus     redoIt();
    MStatus     undoIt();
    bool        isUndoable() const;

	static void*	creator();

protected:
	MString m_key,m_longname;
};

#endif