//////////////////////////////////////////////////////////////////////////////////////
//
//	ARGUMENTS:
//
//		NO ARGUMENTS
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _selectManipHandleZ
#define _selectManipHandleZ

class selectManipHandleZ : public MPxCommand {
public:
	selectManipHandleZ ();
	~selectManipHandleZ (); 

	MStatus	doIt( const MArgList& args );
    MStatus     redoIt();
    MStatus     undoIt();
    bool        isUndoable() const;

	static void*	creator();

private:

};

#endif