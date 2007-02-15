//////////////////////////////////////////////////////////////////////////////////////
//
//	ARGUMENTS:
//
//		NO ARGUMENTS
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _pasteKeys
#define _pasteKeys

class pasteKeys : public MPxCommand
{
public:
	pasteKeys ();
	~pasteKeys (); 

	MStatus			doIt( const MArgList& args );
    MStatus			redoIt();
    MStatus			undoIt();
    bool			isUndoable() const;

	static void*	creator();
};

#endif