//////////////////////////////////////////////////////////////////////////////////////
//
//	ARGUMENTS:
//
//		UNIQUE TRANSFORM FLAG
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _instanceObject
#define _instanceObject

class instanceObject : public MPxCommand {
public:
	instanceObject ();
	~instanceObject ();

	MStatus		doIt( const MArgList& args );
    MStatus     redoIt();
    MStatus     undoIt();
    bool        isUndoable() const;

	static void*	creator();

protected:
	int m_uniquetransforms;

};

#endif