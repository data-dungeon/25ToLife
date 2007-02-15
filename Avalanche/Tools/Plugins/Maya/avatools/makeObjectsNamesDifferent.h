//////////////////////////////////////////////////////////////////////////////////////
//
//	ARGUMENTS:
//
//		NO ARGUMENTS
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _makeObjectsNamesDifferent
#define _makeObjectsNamesDifferent

class makeObjectsNamesDifferent : public MPxCommand {
public:
	makeObjectsNamesDifferent ();
	~makeObjectsNamesDifferent (); 

	MStatus	doIt( const MArgList& args );
    MStatus     redoIt();
    MStatus     undoIt();
    bool        isUndoable() const;

	static void*	creator();

private:

};

#endif