//////////////////////////////////////////////////////////////////////////////////////
//
//	ARGUMENTS:
//
//		NONE
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _makeRefLayer
#define _makeRefLayer

class makeRefLayer : public MPxCommand
{
public:
	makeRefLayer ();
	~makeRefLayer ();

	MStatus		doIt( const MArgList& args );
    MStatus     redoIt();
    MStatus     undoIt();
    bool        isUndoable() const;

	static void*	creator();
};

#endif