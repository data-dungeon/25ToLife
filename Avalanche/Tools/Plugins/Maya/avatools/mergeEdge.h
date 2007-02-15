//////////////////////////////////////////////////////////////////////////////////////
//
//	ARGUMENTS:
//
//		NO ARGUMENTS
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _mergeEdge
#define _mergeEdge

class mergeEdge : public MPxCommand {
public:
	mergeEdge ();
	~mergeEdge (); 

	MStatus	doIt( const MArgList& args );
    MStatus     redoIt();
    MStatus     undoIt();
    bool        isUndoable() const;

	static void*	creator();
};

#endif