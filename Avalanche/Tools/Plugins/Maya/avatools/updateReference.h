//////////////////////////////////////////////////////////////////////////////////////
//
//	ARGUMENTS:
//
//		NONE
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _updateReference
#define _updateReference

class updateReference : public MPxCommand {
public:
	updateReference ();
	~updateReference ();

	MStatus		doIt( const MArgList& args );
    MStatus     redoIt();
    MStatus     undoIt();
    bool        isUndoable() const;

	static void*	creator();
};

#endif