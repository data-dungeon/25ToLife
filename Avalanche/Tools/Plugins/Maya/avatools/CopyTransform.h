//////////////////////////////////////////////////////////////////////////////////////
//
//	ARGUMENTS:
//
//		NO ARGUMENTS
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _copyTransform
#define _copyTransform

class copyTransform : public MPxCommand {
public:
	copyTransform ();
	~copyTransform (); 

	MStatus		doIt( const MArgList& args );
    MStatus     redoIt();
    MStatus     undoIt();
    bool        isUndoable() const;

	static void*	creator();

private:
	bool m_immediateonly;
};

#endif