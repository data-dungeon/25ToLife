//////////////////////////////////////////////////////////////////////////////////////
//
//	ARGUMENTS:
//
//		NO ARGUMENTS
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _replaceObjectsWithLayerRef
#define _replaceObjectsWithLayerRef

class replaceObjectsWithLayerRef : public MPxCommand {
public:
	replaceObjectsWithLayerRef ();
	~replaceObjectsWithLayerRef (); 

	MStatus		doIt( const MArgList& args );
    MStatus     redoIt();
    MStatus     undoIt();
    bool        isUndoable() const;

	static void*	creator();
};

#endif