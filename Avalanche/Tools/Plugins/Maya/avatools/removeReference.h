//////////////////////////////////////////////////////////////////////////////////////
//
//	ARGUMENTS:
//
//		NONE
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _removeReference
#define _removeReference

class removeReference : public MPxCommand {
public:
	removeReference ();
	~removeReference ();

	MStatus		doIt( const MArgList& args );
    MStatus     redoIt();
    MStatus     undoIt();
    bool        isUndoable() const;

	static void*	creator();
};

#endif