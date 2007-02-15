//////////////////////////////////////////////////////////////////////////////////////
//	listKeysFromNotes
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _listKeysFromNotes
#define _listKeysFromNotes

class listKeysFromNotes : public MPxCommand {
public:
	listKeysFromNotes ();
	~listKeysFromNotes (); 

	MStatus		doIt( const MArgList& args );
    MStatus     redoIt();
    MStatus     undoIt();
    bool        isUndoable() const;

	static void*	creator();

protected:
	MString m_objectname;
};

#endif