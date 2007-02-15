//////////////////////////////////////////////////////////////////////////////////////
//	delKeyValueFromNotes
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _delKeyValueFromNotes
#define _delKeyValueFromNotes

class delKeyValueFromNotes : public MPxCommand {
public:
	delKeyValueFromNotes ();
	~delKeyValueFromNotes (); 

	MStatus		doIt( const MArgList& args );
    MStatus     redoIt();
    MStatus     undoIt();
    bool        isUndoable() const;

	static void*	creator();

protected:
	MString m_objectname;
	MString m_key;
};

#endif