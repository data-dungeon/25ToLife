//////////////////////////////////////////////////////////////////////////////////////
//
//	ARGUMENTS:
//
//		UNIQUE TRANSFORM FLAG
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _duplicateObject
#define _duplicateObject

class duplicateObject : public MPxCommand {
public:
	duplicateObject ();
	~duplicateObject ();

	MStatus		doIt( const MArgList& args );
    MStatus     redoIt();
    MStatus     undoIt();
    bool        isUndoable() const;

	static void*	creator();

protected:
	int m_uniquetransforms;

};

#endif