//////////////////////////////////////////////////////////////////////////////////////
//
//	ARGUMENTS:
//
//		Attribute Note
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _copyAttributes
#define _copyAttributes

class copyAttributes : public MPxCommand {
public:
	copyAttributes ();
	~copyAttributes (); 

	MStatus		doIt( const MArgList& args );
    MStatus     redoIt();
    MStatus     undoIt();
    bool        isUndoable() const;

	static void*	creator();

protected:
	MString m_note;
};

#endif