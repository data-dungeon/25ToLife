//////////////////////////////////////////////////////////////////////////////////////
//
//	ARGUMENTS:
//
//		NO ARGUMENTS
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _copyKeys
#define _copyKeys

class copyKeys : public MPxCommand
{
public:
	copyKeys ();
	~copyKeys (); 

	MStatus			doIt( const MArgList& args );
    MStatus			redoIt();
    MStatus			undoIt();
    bool			isUndoable() const;

	static void*	creator();
};

#endif