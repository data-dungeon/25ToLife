//////////////////////////////////////////////////////////////////////////////////////
//
//	ARGUMENTS:
//
//		NO ARGUMENTS
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _setBoneOrientToChild
#define _setBoneOrientToChild

class setBoneOrientToChild : public MPxCommand {
public:
	setBoneOrientToChild ();
	~setBoneOrientToChild (); 

	MStatus		doIt( const MArgList& args );
    MStatus     redoIt();
    MStatus     undoIt();
    bool        isUndoable() const;

	static void*	creator();
};

#endif