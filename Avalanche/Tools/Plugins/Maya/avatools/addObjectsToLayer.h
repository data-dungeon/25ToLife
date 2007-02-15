//////////////////////////////////////////////////////////////////////////////////////
//
//	ARGUMENTS:
//
//		arg 0 - Recurse
//		arg 1 - Norecurse
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _addObjectsToLayer
#define _addObjectsToLayer

class addObjectsToLayer : public MPxCommand {
public:
	addObjectsToLayer ();
	~addObjectsToLayer ();

	MStatus		doIt( const MArgList& args );
    MStatus     redoIt();
    MStatus     undoIt();
    bool        isUndoable() const;

	static void*	creator();

protected:
	int m_noRecurse;

};

#endif