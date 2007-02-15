//////////////////////////////////////////////////////////////////////////////////////
//	addKeyValueToNotes
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _addKeyValueToNotes
#define _addKeyValueToNotes

class addKeyValueToNotes : public MPxCommand {
public:
	addKeyValueToNotes ();
	~addKeyValueToNotes (); 

	MStatus		doIt( const MArgList& args );
    MStatus     redoIt();
    MStatus     undoIt();
    bool        isUndoable() const;

	static void*	creator();

protected:
	MString m_objectname;
	MString m_key,m_value;
};

#endif