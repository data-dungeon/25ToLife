//////////////////////////////////////////////////////////////////////////////////////
//
//	ARGUMENTS:
//
//		UNIQUE TRANSFORM FLAG
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _createReference
#define _createReference

class createReference : public MPxCommand {
public:
	createReference ();
	~createReference ();

	MStatus		doIt( const MArgList& args );
    MStatus     redoIt();
    MStatus     undoIt();
    bool        isUndoable() const;

	static void*	creator();

protected:
	int m_uniquetransforms;
	int m_createinstance;

};

#endif