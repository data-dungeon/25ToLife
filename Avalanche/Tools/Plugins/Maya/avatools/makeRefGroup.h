//////////////////////////////////////////////////////////////////////////////////////
//
//	ARGUMENTS:
//
//		NONE
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _makeRefGroup
#define _makeRefGroup

class makeRefGroup : public MPxCommand
{
public:
	makeRefGroup ();
	~makeRefGroup ();

	MStatus		doIt( const MArgList& args );
    MStatus     redoIt();
    MStatus     undoIt();
    bool        isUndoable() const;

	static void*	creator();
};

#endif