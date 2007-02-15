//////////////////////////////////////////////////////////////////////////////////////
//
//	ARGUMENTS:
//
//		NO ARGUMENTS
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _selectManipHandleY
#define _selectManipHandleY

class selectManipHandleY : public MPxCommand {
public:
	selectManipHandleY ();
	~selectManipHandleY (); 

	MStatus	doIt( const MArgList& args );
    MStatus     redoIt();
    MStatus     undoIt();
    bool        isUndoable() const;

	static void*	creator();

private:

};

#endif