//////////////////////////////////////////////////////////////////////////////////////
//
//	ARGUMENTS:
//
//		NO ARGUMENTS
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _smoothNormals
#define _smoothNormals

class smoothNormals : public MPxCommand {
public:
	smoothNormals ();
	~smoothNormals (); 

	MStatus		doIt( const MArgList& args );
    MStatus     redoIt();
    MStatus     undoIt();
    bool        isUndoable() const;

	static void*	creator();
};

#endif