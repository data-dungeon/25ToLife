//////////////////////////////////////////////////////////////////////////////////////
//
//	ARGUMENTS:
//
//		ARG #1 :	1 - CLOCKWISE ROTATION 90 degrees
//					0 - COUNTER CLOCKWISE ROTATION 90 degrees
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _rotateUVs
#define _rotateUVs

class rotateUVs : public MPxCommand {
public:
	rotateUVs ();
	~rotateUVs (); 

	MStatus	doIt( const MArgList& args );
    MStatus     redoIt();
    MStatus     undoIt();
    bool        isUndoable() const;

	static void*	creator();

private:
	bool ui_rotclockwise;
	MStringArray selFaces,selUVs;

};

#endif