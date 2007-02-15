//////////////////////////////////////////////////////////////////////////////////////
//
//	ARGUMENTS:
//
//		ARG #1 :	1 - IN U DIRECTION
//					0 - IN V DIRECTION
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _flipUVs
#define _flipUVs

class flipUVs : public MPxCommand {
public:
	flipUVs ();
	~flipUVs (); 

	MStatus	doIt( const MArgList& args );
    MStatus     redoIt();
    MStatus     undoIt();
    bool        isUndoable() const;

	static void*	creator();

private:
	bool ui_udirection;
	MStringArray selFaces,selUVs;

};

#endif