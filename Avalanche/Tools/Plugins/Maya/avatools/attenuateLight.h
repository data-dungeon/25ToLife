//////////////////////////////////////////////////////////////////////////////////////
//
//	ARGUMENTS:
//
//		Attribute Note
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _attenuateLight
#define _attenuateLight

class attenuateLight : public MPxCommand {
public:
	attenuateLight ();
	~attenuateLight (); 

	MStatus		doIt( const MArgList& args );
    MStatus     redoIt();
    MStatus     undoIt();
    bool        isUndoable() const;

	static void*	creator();
};

#endif