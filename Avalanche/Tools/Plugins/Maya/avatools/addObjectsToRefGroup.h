//////////////////////////////////////////////////////////////////////////////////////
//
//	ARGUMENTS:
//
//		NO ARGUMENTS
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _addObjectsToRefGroup
#define _addObjectsToRefGroup

class addObjectsToRefGroup : public MPxCommand {
public:
	addObjectsToRefGroup ();
	~addObjectsToRefGroup (); 

	MStatus		doIt( const MArgList& args );
    MStatus     redoIt();
    MStatus     undoIt();
    bool        isUndoable() const;

	static void*	creator();
};

#endif